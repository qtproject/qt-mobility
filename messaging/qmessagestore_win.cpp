/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagestore_p.h"
#include "qmessage_p.h"
#include "qmessageid_p.h"
#include "qmessagefolderid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessagefolder_p.h"
#include "qmessageaccount_p.h"
#include "qmessageordering_p.h"
#include "winhelpers_p.h"

#include <qdebug.h>

struct FolderHeapNode {
    MapiStorePtr store;
    MapiFolderPtr folder;
    MapiRecordKey key;
    MapiRecordKey parentStoreKey;
    MapiEntryId parentStoreEntryId;
    QString name;
    MapiEntryId entryId;
    bool hasSubFolders;
    uint messageCount;
    uint offset; // TODO replace this with LPMAPITABLE for efficiency
    QMessage front;
};

typedef QSharedPointer<FolderHeapNode> FolderHeapNodePtr;

class FolderHeap {
public:
    FolderHeap(QMessageStore::ErrorCode *lastError, MapiSessionPtr mapiSession, const QList<FolderHeapNodePtr> &protoHeap, const QMessageFilter &filter, const QMessageOrdering &ordering);
    QMessage takeFront(QMessageStore::ErrorCode *lastError);
    bool isEmpty() const { return _heap.count() == 0; }

private:
    void sink(int i);
    QMessageFilter _filter;
    QMessageOrdering _ordering;
    QList<FolderHeapNodePtr> _heap;
    MapiSessionPtr _mapiSession;
};

FolderHeap::FolderHeap(QMessageStore::ErrorCode *lastError, MapiSessionPtr mapiSession, const QList<FolderHeapNodePtr> &protoHeap, const QMessageFilter &filter, const QMessageOrdering &ordering)
{
    _filter = filter;
    _ordering = ordering;
    _mapiSession = mapiSession;

    foreach (const FolderHeapNodePtr &folder, protoHeap) {
        QMessageStore::ErrorCode ignoredError(QMessageStore::NoError);
        FolderHeapNodePtr node(folder);
        if (!node->folder) {
            qWarning() << "Unable to access folder:" << node->name;
            continue;
        }

        node->offset = 0;

        // TODO: Would be more efficient to use a LPMAPITABLE directly instead of calling MapiFolder queryMessages and message functions.
        QMessageIdList messageIdList(node->folder->queryMessages(&ignoredError, filter, ordering, 1));
        if (ignoredError == QMessageStore::NoError) {
            if (!messageIdList.isEmpty()) {
                node->front = _mapiSession->message(&ignoredError, messageIdList.front());
                if (ignoredError == QMessageStore::NoError) {
                    _heap.append(node);
                    qDebug() << "valid folder:" << node->name;
                }
            }
        }
    }

    for (int i = _heap.count()/2 - 1; i >= 0; --i)
        sink(i);
}

QMessage FolderHeap::takeFront(QMessageStore::ErrorCode *lastError)
{
    QMessage result(_heap[0]->front);

    FolderHeapNodePtr node(_heap[0]);

    MapiStorePtr store = _mapiSession->openStore(lastError, node->parentStoreEntryId);
    if (*lastError != QMessageStore::NoError)
        return result;

    node->folder = store->openFolder(lastError, node->entryId);
    if (*lastError != QMessageStore::NoError)
        return result;

    ++node->offset;
    // TODO: Would be more efficient to use a LPMAPITABLE directly instead of calling MapiFolder queryMessages and message functions.
    QMessageIdList messageIdList(node->folder->queryMessages(lastError, _filter, _ordering, 1, node->offset));
    if (*lastError != QMessageStore::NoError)
        return result;

    if (messageIdList.isEmpty()) {
        if (_heap.count() > 1) {
            _heap[0] = _heap.takeLast();
        } else {
            _heap.pop_back();
        }
    } else {
        node->front = _mapiSession->message(lastError, messageIdList.front());
        if (*lastError != QMessageStore::NoError)
            return result;
    }
    sink(0);
    return result;
}

void FolderHeap::sink(int i)
{
    while (true) {
        int left(2*i + 1);
        if (left >= _heap.count())
            return;
        int right(left + 1);
        int minimum(left);
        if ((right < _heap.count())
            && (QMessageOrderingPrivate::compare(_ordering, _heap[right]->front, _heap[left]->front)))
            minimum = right;
        if (QMessageOrderingPrivate::compare(_ordering, _heap[i]->front, _heap[minimum]->front))
            return;
        FolderHeapNodePtr temp(_heap[minimum]);
        _heap[minimum] = _heap[i];
        _heap[i] = temp;
        i = minimum;
    }
}

class QMessageStorePrivatePlatform
{
public:
    QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q);
    ~QMessageStorePrivatePlatform();

    QMessageStorePrivate *d_ptr;
    QMessageStore *q_ptr;
    QMessageStore::ErrorCode lastError;

    bool _mapiInitialized;
};

QMessageStorePrivatePlatform::QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q)
    :d_ptr(d), 
     q_ptr(q)
{
    _mapiInitialized = false;
    lastError = QMessageStore::NoError;
#ifndef QT_NO_THREAD
    // Note MAPIINIT is ignored on Windows Mobile but used on Outlook 2007 see
    // msdn ms862621 vs cc842343
    MAPIINIT_0 MAPIINIT = { 0, MAPI_MULTITHREAD_NOTIFICATIONS };
    if (MAPIInitialize(&MAPIINIT) == S_OK)
        _mapiInitialized = true;
#else
    if (MAPIInitialize(0) == S_OK)
        _mapiInitialized = true;
#endif
    if (!_mapiInitialized) {
        qWarning() << "Failed to initialize MAPI";
        lastError = QMessageStore::ContentInaccessible;
    }
}

QMessageStorePrivatePlatform::~QMessageStorePrivatePlatform()
{
    MAPIUninitialize();
}

QMessageStorePrivate::QMessageStorePrivate()
    :p_ptr(0),
     q_ptr(0)
{
}

void QMessageStorePrivate::initialize(QMessageStore *store)
{
    q_ptr = store;
    p_ptr = new QMessageStorePrivatePlatform(this, store);
}

Q_GLOBAL_STATIC(QMessageStorePrivate,data);

QMessageStore::QMessageStore(QObject *parent)
    : QObject(parent),
      d_ptr(data())
{
    Q_ASSERT(d_ptr != 0);
    Q_ASSERT(d_ptr->q_ptr == 0); // QMessageStore should be singleton
}

QMessageStore::~QMessageStore()
{
    d_ptr = 0; // should be cleaned up by automatically
}

QMessageStore* QMessageStore::instance()
{
    QMessageStorePrivate *d = data();
    Q_ASSERT(d != 0);
    if (!d->q_ptr)
        d->initialize(new QMessageStore());
    return d->q_ptr;
}

QMessageStore::ErrorCode QMessageStore::lastError() const
{
    return d_ptr->p_ptr->lastError;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    QMessageIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(MapiSession::createSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    QList<FolderHeapNodePtr> folderNodes;

    foreach (const MapiStorePtr &store, mapiSession->allStores(&d_ptr->p_ptr->lastError)) {
        QList<MapiFolderPtr> folders; // depth first search;
        folders.append(store->rootFolder(&d_ptr->p_ptr->lastError));
        if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
            return result;

        while (!folders.isEmpty()) {
            if (!folders.back()->isValid()) {
                folders.pop_back();
                continue;
            }

            QMessageStore::ErrorCode ignored(QMessageStore::NoError);
            MapiFolderPtr folder(folders.back()->nextSubFolder(&ignored, *store));
            if ((!folder || !folder->isValid()) && (ignored == QMessageStore::NoError)) {
                folders.pop_back(); // No more subfolders
                continue;
            }
            if (ignored != QMessageStore::NoError) {
                continue; // Bad subfolder, skip it
            }

            if (folder && folder->isValid()) {
                FolderHeapNodePtr node(new FolderHeapNode);
                node->store = store;
                node->folder = folder;
                node->key = folder->recordKey();
                node->parentStoreKey = folder->storeKey();
                node->parentStoreEntryId = store->entryId();
                node->name = folder->name();
                node->entryId = folder->entryId();
                node->hasSubFolders = folder->hasSubFolders();
                node->messageCount = folder->messageCount();
                folderNodes.append(node);
                folders.append(folder);
                if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
                    return result;
            } else {
                folders.pop_back();
            }
        }
    }

    FolderHeap folderHeap(&d_ptr->p_ptr->lastError, mapiSession, folderNodes, filter, ordering);
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    int count = 0 - offset;
    while (!folderHeap.isEmpty()) {
        // TODO: avoid retrieving unwanted messages
        if (limit && (count == limit))
            break;

        QMessage front(folderHeap.takeFront(&d_ptr->p_ptr->lastError));
        if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
            return result;

        if (count >= 0) {
            result.append(front.id());
        }
        ++count;
    }

    if (offset) {
        return result.mid(offset, (limit ? limit : -1));
    } else {
        return result;
    }
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    Q_UNUSED(filter)
    Q_UNUSED(ordering)
    Q_UNUSED(body)
    Q_UNUSED(options)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    QMessageIdList result;

    if (offset) {
        return result.mid(offset, (limit ? limit : -1));
    } else {
        return result;
    }
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderOrdering &ordering, uint limit, uint offset) const
{
    Q_UNUSED(filter)
    Q_UNUSED(ordering)
    QMessageFolderIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(MapiSession::createSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    foreach (const MapiStorePtr &store, mapiSession->allStores(&d_ptr->p_ptr->lastError)) {
        result.append(store->folderIds(&d_ptr->p_ptr->lastError));
    }

    if (offset) {
        return result.mid(offset, (limit ? limit : -1));
    } else {
        return result;
    }
}
#endif

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountOrdering &ordering, uint limit, uint offset) const
{
    Q_UNUSED(filter)
    Q_UNUSED(ordering)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    QMessageAccountIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(MapiSession::createSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    foreach (const MapiStorePtr &store, mapiSession->allStores(&d_ptr->p_ptr->lastError)) {
        result.append(QMessageAccountId(store->id()));
    }

    if (offset) {
        return result.mid(offset, (limit ? limit : -1));
    } else {
        return result;
    }
}

int QMessageStore::countMessages(const QMessageFilter& filter) const
{
    return queryMessages(filter).count();
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
int QMessageStore::countFolders(const QMessageFolderFilter& filter) const
{
    return queryFolders(filter).count();
}
#endif

int QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
{
    return queryAccounts(filter).count();
}

bool QMessageStore::removeMessage(const QMessageId& id, RemovalOption option)
{
    Q_UNUSED(id)
    Q_UNUSED(option)
    return false; // stub
}

bool QMessageStore::removeMessages(const QMessageFilter& filter, QMessageStore::RemovalOption option)
{
    Q_UNUSED(filter)
    Q_UNUSED(option)
    return true; // stub
}


namespace {

FILETIME toFileTime(const QDateTime &dt)
{
    FILETIME ft = {0};

    QDate date(dt.date());
    QTime time(dt.time());

    SYSTEMTIME st = {0};
    st.wYear = date.year();
    st.wMonth = date.month();
    st.wDay = date.day();
    st.wHour = time.hour();
    st.wMinute = time.minute();
    st.wSecond = time.second();
    st.wMilliseconds = time.msec();

    SystemTimeToFileTime(&st, &ft);
    return ft;
}

ADRLIST *createAddressList(int count)
{
    ADRLIST *list(0);

    uint size = CbNewADRLIST(count);
    MAPIAllocateBuffer(size, reinterpret_cast<LPVOID*>(&list));
    if (list) {
        memset(list, 0, size);
        list->cEntries = count;

        for (int i = 0; i < count; ++i) {
            list->aEntries[i].cValues = 2;
            MAPIAllocateBuffer(2 * sizeof(SPropValue), reinterpret_cast<LPVOID*>(&list->aEntries[i].rgPropVals));
        }
    }

    return list;
}

void fillAddressEntry(ADRENTRY &entry, const QMessageAddress &addr, LONG type, QList<LPTSTR> &addresses)
{
    entry.rgPropVals[0].ulPropTag = PR_RECIPIENT_TYPE;
    entry.rgPropVals[0].Value.l = type;

    QString addressStr("[%1:%2]");
    addressStr = addressStr.arg(addr.type() == QMessageAddress::Phone ? "SMS" : "SMTP");
    addressStr = addressStr.arg(addr.recipient());

    // TODO: Escape illegal characters, as per: http://msdn.microsoft.com/en-us/library/cc842281.aspx

    uint len = addressStr.length();
    LPTSTR address = new TCHAR[len + 1];
    memcpy(address, addressStr.utf16(), len * sizeof(TCHAR));
    address[len] = 0;

    entry.rgPropVals[1].ulPropTag = PR_DISPLAY_NAME;
    entry.rgPropVals[1].Value.LPSZ = address;

    addresses.append(address);
}

bool resolveAddressList(ADRLIST *list, IMAPISession *session)
{
    bool result(false);

    if (session) {
        IAddrBook *book(0);
        HRESULT rv = session->OpenAddressBook(0, 0, AB_NO_DIALOG, &book);
        if (HR_SUCCEEDED(rv)) {
            rv = book->ResolveName(0, MAPI_UNICODE, 0, list);
            if (HR_SUCCEEDED(rv)) {
                result = true;
            } else {
                qWarning() << "Unable to resolve addresses.";
            }

            book->Release();
        } else {
            qWarning() << "Unable to open address book.";
        }
    }

    return result;
}

void destroyAddressList(ADRLIST *list, QList<LPTSTR> &addresses)
{
    foreach (LPTSTR address, addresses) {
        delete [] address;
    }

    addresses.clear();

    for (uint i = 0; i < list->cEntries; ++i) {
        MAPIFreeBuffer(list->aEntries[i].rgPropVals);
    }
    
    MAPIFreeBuffer(list);
}

bool setMapiProperty(IMAPIProp *object, ULONG tag, const QString &value)
{
    SPropValue prop = { 0 };
    prop.ulPropTag = tag;
    prop.Value.LPSZ = reinterpret_cast<LPWSTR>(const_cast<quint16*>(value.utf16()));
    return HR_SUCCEEDED(HrSetOneProp(object, &prop));
}

bool setMapiProperty(IMAPIProp *object, ULONG tag, LONG value)
{
    SPropValue prop = { 0 };
    prop.ulPropTag = tag;
    prop.Value.l = value;
    return HR_SUCCEEDED(HrSetOneProp(object, &prop));
}

bool setMapiProperty(IMAPIProp *object, ULONG tag, FILETIME value)
{
    SPropValue prop = { 0 };
    prop.ulPropTag = tag;
    prop.Value.ft = value;
    return HR_SUCCEEDED(HrSetOneProp(object, &prop));
}

}

bool QMessageStore::addMessage(QMessage *m)
{
    bool result(false);

    if (!m || !m->id().isValid()) {
        // Find the parent folder for this message
        QMessageFolder folder(m->parentFolderId());
        if (folder.id().isValid()) {
            MapiSessionPtr session(MapiSession::createSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
            if (d_ptr->p_ptr->lastError == QMessageStore::NoError) {
                if (MapiStorePtr store = session->findStore(&d_ptr->p_ptr->lastError, m->parentAccountId())) {
                    if (MapiFolderPtr mapiFolder = store->findFolder(&d_ptr->p_ptr->lastError, QMessageFolderIdPrivate::folderRecordKey(folder.id()))) {
                        LPMESSAGE message(0);
                        HRESULT rv = mapiFolder->folder()->CreateMessage(0, 0, &message);
                        if (HR_SUCCEEDED(rv)) {
                            // Find the identifier of the new message
                            SizedSPropTagArray(2, columns) = {2, {PR_RECORD_KEY, PR_ENTRYID}};
                            SPropValue *properties(0);
                            ULONG count;
                            rv = message->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), 0, &count, &properties);
                            if (HR_SUCCEEDED(rv)) {
                                MapiRecordKey recordKey(reinterpret_cast<const char*>(properties[0].Value.bin.lpb), properties[0].Value.bin.cb);
                                MapiEntryId entryId(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);
                                m->d_ptr->_id = QMessageIdPrivate::from(recordKey, mapiFolder->recordKey(), mapiFolder->storeKey(), entryId);

                                MAPIFreeBuffer(properties);

                                // Set the message's properties
                                if (!setMapiProperty(message, PR_SUBJECT, m->subject())) {
                                    qWarning() << "Unable to set subject in message.";
                                }

                                LONG flags = (MSGFLAG_UNSENT | MSGFLAG_UNMODIFIED | MSGFLAG_FROMME);
                                if (m->status() & QMessage::HasAttachments) {
                                    flags |= MSGFLAG_HASATTACH;
                                }
                                if (!setMapiProperty(message, PR_MESSAGE_FLAGS, flags)) {
                                    qWarning() << "Unable to set flags in message.";
                                }

                                QString emailAddress = m->from().recipient();
                                if (!setMapiProperty(message, PR_SENDER_EMAIL_ADDRESS, emailAddress)) {
                                    qWarning() << "Unable to set sender address in message.";
                                }

                                QStringList headers;
                                foreach (const QByteArray &name, m->headerFields()) {
                                    foreach (const QString &value, m->headerFieldValues(name)) {
                                        // TODO: Do we need soft line-breaks?
                                        headers.append(QString("%1: %2").arg(QString(name)).arg(value));
                                    }
                                }
                                if (!headers.isEmpty()) {
                                    QString transportHeaders = headers.join("\r\n").append("\r\n\r\n");
                                    if (!setMapiProperty(message, PR_TRANSPORT_MESSAGE_HEADERS, transportHeaders)) {
                                        qWarning() << "Unable to set transport headers in message.";
                                    }
                                }

                                if (!setMapiProperty(message, PR_CLIENT_SUBMIT_TIME, toFileTime(m->date()))) {
                                    qWarning() << "Unable to set submit time in message.";
                                }

                                uint recipientCount(m->to().count() + m->cc().count() + m->bcc().count());
                                if (recipientCount) {
                                    ADRLIST *list = createAddressList(recipientCount);
                                    if (list) {
                                        int index = 0;
                                        QList<LPTSTR> addresses;

                                        foreach (const QMessageAddress &addr, m->to()) {
                                            ADRENTRY &entry(list->aEntries[index]);
                                            fillAddressEntry(entry, addr, MAPI_TO, addresses);
                                            ++index;
                                        }

                                        foreach (const QMessageAddress &addr, m->cc()) {
                                            ADRENTRY &entry(list->aEntries[index]);
                                            fillAddressEntry(entry, addr, MAPI_CC, addresses);
                                            ++index;
                                        }

                                        foreach (const QMessageAddress &addr, m->bcc()) {
                                            ADRENTRY &entry(list->aEntries[index]);
                                            fillAddressEntry(entry, addr, MAPI_BCC, addresses);
                                            ++index;
                                        }

                                        if (resolveAddressList(list, session->session())) {
                                            rv = message->ModifyRecipients(MODRECIP_ADD, list);
                                            if (HR_FAILED(rv)) {
                                                qWarning() << "Unable to store address list for message.";
                                            }
                                        } else {
                                            qWarning() << "Unable to resolve address list for message.";
                                        }

                                        destroyAddressList(list, addresses);
                                    } else {
                                        qWarning() << "Unable to allocate address list for message.";
                                    }
                                }

                                // Store all the custom field data in a single block
                                QStringList customFieldData;
                                foreach (const QString &key, m->customFields()) {
                                    customFieldData.append(key + "\n" + m->customField(key));
                                }
                                if (!customFieldData.isEmpty()) {
                                    ULONG tag = WinHelpers::createNamedProperty(message, "customFieldData");
                                    if (tag) {
                                        WinHelpers::setNamedProperty(message, tag, customFieldData.join("\n\n"));
                                    } else {
                                        qWarning() << "Unable to store custom field data for message.";
                                    }
                                }

                                if (!m->contentIds().isEmpty()) {
                                    // This is a multipart message
                                    // TODO: multipart addition
                                } else {
                                    // This message has only a body
                                    QByteArray subType(m->contentSubType().toLower());

                                    if ((subType == "rtf") || (subType == "html")) {
                                        // TODO: non-plain storage
                                    } else {
                                        // Mark this message as plain text
                                        LONG textFormat(EDITOR_FORMAT_PLAINTEXT);
                                        if (!setMapiProperty(message, PR_MSG_EDITOR_FORMAT, textFormat)) {
                                            qWarning() << "Unable to set message editor format in message.";
                                        }

                                        QString body(m->textContent());
                                        if (!setMapiProperty(message, PR_BODY, body)) {
                                            qWarning() << "Unable to set body in message.";
                                        }
                                    }
                                }

                                if (HR_FAILED(message->SaveChanges(0))) {
                                    qWarning() << "Unable to save changes on message.";
                                }

                                message->Release();
                                return true;
                            } else {
                                qWarning() << "Cannot get properties for new message";
                            }

                            message->Release();
                        } else {
                            qWarning() << "Cannot CreateMessage";
                        }
                    } else {
                        qWarning() << "Cannot get MAPI folder from store";
                    }
                } else {
                    qWarning() << "Cannot get default store";
                }
            } else {
                qWarning() << "Cannot get session";
            }
        } else {
            qWarning() << "Invalid parent folder";
        }
    } else {
        qWarning() << "Valid message ID at addition";
    }

    return result;
}

bool QMessageStore::updateMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    QMessage result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(MapiSession::createSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    return mapiSession->message(&d_ptr->p_ptr->lastError, id);
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    QMessageFolder result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(MapiSession::createSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    return mapiSession->folder(&d_ptr->p_ptr->lastError, id);
}
#endif

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    QMessageAccount result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(MapiSession::createSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    MapiStorePtr mapiStore(mapiSession->findStore(&d_ptr->p_ptr->lastError, id));
    if (mapiStore && mapiStore->isValid()) {
        result = QMessageAccountPrivate::from(mapiStore->id(), mapiStore->name(), mapiStore->address(), mapiStore->types());
    }

    return result;
}

void QMessageStore::startNotifications(const QMessageFilter &filter)
{
    Q_UNUSED(filter)    
}

void QMessageStore::stopNotifications()
{
}
