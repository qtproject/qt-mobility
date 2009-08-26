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
#include "qmessagesortkey_p.h"
#include "winhelpers_p.h"

#include <qdebug.h>

struct FolderHeapNode {
    LPMAPIFOLDER folder;
    MapiRecordKey key;
    MapiRecordKey parentStoreKey;
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
    FolderHeap(QMessageStore::ErrorCode *lastError, MapiSessionPtr mapiSession, const QList<FolderHeapNodePtr> &protoHeap, const QMessageFilterKey &filterKey, const QMessageSortKey &sortKey);
    QMessage takeFront(QMessageStore::ErrorCode *lastError);
    bool isEmpty() const { return _heap.count() == 0; }

private:
    void sink(int i);
    QMessageFilterKey _filterKey;
    QMessageSortKey _sortKey;
    QList<FolderHeapNodePtr> _heap;
    MapiSessionPtr _mapiSession;
};

FolderHeap::FolderHeap(QMessageStore::ErrorCode *lastError, MapiSessionPtr mapiSession, const QList<FolderHeapNodePtr> &protoHeap, const QMessageFilterKey &filterKey, const QMessageSortKey &sortKey)
{
    _heap = protoHeap;
    _filterKey = filterKey;
    _sortKey = sortKey;
    _mapiSession = mapiSession;

    int i(0);
    while (i < _heap.count()) {
        FolderHeapNodePtr node(_heap[i]);
        if (mapiSession->openEntry(lastError, node->entryId, &node->folder) != S_OK)
            *lastError = QMessageStore::ContentInaccessible;
        if (*lastError != QMessageStore::NoError)
            return; //TODO consider just removing the bad folder

        MapiFolder mapiFolder(node->folder, node->key, node->parentStoreKey, node->name, node->entryId, node->hasSubFolders, node->messageCount);
        node->offset = 0;
        // TODO: Would be more efficient to use a LPMAPITABLE directly instead of calling MapiFolder queryMessages and message functions.
        QMessageIdList messageIdList(mapiFolder.queryMessages(lastError, filterKey, sortKey, 1));
        if (*lastError != QMessageStore::NoError)
            return; //TODO consider just removing the bad folder
        if (messageIdList.isEmpty()) {
            _heap.removeAt(i);
        } else {
            node->front = mapiSession->message(lastError, messageIdList.front());
            if (*lastError != QMessageStore::NoError)
            return; //TODO consider just removing the bad folder
            ++i;
        }
    }

    for (int i = _heap.count()/2 - 1; i >= 0; --i)
        sink(i);
}

QMessage FolderHeap::takeFront(QMessageStore::ErrorCode *lastError)
{
    QMessage result(_heap[0]->front);
    FolderHeapNodePtr node(_heap[0]);
    if (_mapiSession->openEntry(lastError, node->entryId, &node->folder) != S_OK)
        *lastError = QMessageStore::ContentInaccessible;
    if (*lastError != QMessageStore::NoError)
        return result;

    MapiFolder mapiFolder(node->folder, node->key, node->parentStoreKey, node->name, node->entryId, node->hasSubFolders, node->messageCount);
    ++node->offset;
    QMessageIdList messageIdList(mapiFolder.queryMessages(lastError, _filterKey, _sortKey, 1, node->offset));

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
            && (QMessageSortKeyPrivate::compare(_sortKey, _heap[right]->front, _heap[left]->front)))
            minimum = right;
        if (QMessageSortKeyPrivate::compare(_sortKey, _heap[i]->front, _heap[minimum]->front))
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
    d_ptr->initialize(this);
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

QMessageIdList QMessageStore::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    QMessageIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
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

            MapiFolderPtr folder(folders.back()->nextSubFolder(&d_ptr->p_ptr->lastError));
            if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
                return result;
            if (folder->isValid()) {
                FolderHeapNodePtr node(new FolderHeapNode);
                node->folder = folder->folder();
                node->key = folder->recordKey();
                node->parentStoreKey = folder->storeKey();
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

    FolderHeap folderHeap(&d_ptr->p_ptr->lastError, mapiSession, folderNodes, key, sortKey);
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    uint workingLimit(offset + limit); // TODO: Improve this it's inefficient
    while (!folderHeap.isEmpty()) {
        if (!workingLimit)
            break;
        QMessage front(folderHeap.takeFront(&d_ptr->p_ptr->lastError));
        if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
            return result;
        result.append(front.id());
        --workingLimit;
    }

    if (offset) {
        return result.mid(offset, (limit ? limit : -1));
    } else {
        return result;
    }
}

QMessageIdList QMessageStore::queryMessages(const QString &body, const QMessageFilterKey &key, const QMessageSortKey &sortKey,  QMessageDataComparator::Options options, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
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
QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilterKey &key, const QMessageFolderSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    QMessageFolderIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
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

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilterKey &key, const QMessageAccountSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    QMessageAccountIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
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

int QMessageStore::countMessages(const QMessageFilterKey& key) const
{
    return queryMessages(key).count();
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
int QMessageStore::countFolders(const QMessageFolderFilterKey& key) const
{
    return queryFolders(key).count();
}
#endif

int QMessageStore::countAccounts(const QMessageAccountFilterKey& key) const
{
    return queryAccounts(key).count();
}

bool QMessageStore::removeMessage(const QMessageId& id, RemovalOption option)
{
    Q_UNUSED(id)
    Q_UNUSED(option)
    return false; // stub
}

bool QMessageStore::removeMessages(const QMessageFilterKey& key, QMessageStore::RemovalOption option)
{
    Q_UNUSED(key)
    Q_UNUSED(option)
    return true; // stub
}

bool QMessageStore::addMessage(QMessage *m)
{
    bool result(false);

    if (!m || !m->id().isValid()) {
        // Find the parent folder for this message
        QMessageFolder folder(m->parentFolderId());
        if (folder.id().isValid()) {
            MapiSessionPtr session(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
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

                                QString subj("Test message");
                                wchar_t *buffer = new wchar_t[subj.length() + 1];
                                subj.toWCharArray(buffer);
                                buffer[subj.length()] = 0;

                                // Set the message's properties
                                {
                                    SPropValue prop = { 0 };
                                    prop.ulPropTag = PR_SUBJECT;
                                    prop.Value.LPSZ = buffer;
                                    rv = HrSetOneProp(message, &prop);
                                    if (HR_FAILED(rv)) {
                                        qWarning() << "Unable to set subject in message...";
                                    }
                                }

                                delete [] buffer;

                                {
                                    SPropValue prop = { 0 };
                                    prop.ulPropTag = PR_MESSAGE_FLAGS;
                                    prop.Value.l = (MSGFLAG_UNSENT | MSGFLAG_FROMME);
                                    rv = HrSetOneProp(message, &prop);
                                    if (HR_FAILED(rv)) {
                                        qWarning() << "Unable to set flags in message...";
                                    }
                                }

                                if (HR_FAILED(message->SaveChanges(0))) {
                                    qWarning() << "Unable to save changes on message...";
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

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    return mapiSession->message(&d_ptr->p_ptr->lastError, id);
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    QMessageFolder result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    // Get the store key, TODO move QMessageIdPrivate definition into qmessage_p.h and use it
    MapiRecordKey storeRecordKey(QMessageFolderIdPrivate::storeRecordKey(id));
    QMessageAccountId accountId(QMessageAccountIdPrivate::from(storeRecordKey));
    MapiStorePtr mapiStore(mapiSession->findStore(&d_ptr->p_ptr->lastError, accountId));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    // Find the root folder for this store
    MapiFolderPtr storeRoot(mapiStore->rootFolder(&d_ptr->p_ptr->lastError));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    LPMAPIFOLDER folder;
    QMessageStore::ErrorCode ignoredError;
    MapiEntryId entryId(QMessageFolderIdPrivate::entryId(id));
#if 0 // force lookup using record keys, TODO remove
    if (false) {
#else
    // Try to quickly retrieve the folder based on its EntryId.
    if (mapiSession->openEntry(&ignoredError, entryId, &folder) == S_OK) {
#endif
        const int nCols(3);
        enum { displayNameColumn = 0, recordKeyColumn, parentEntryIdColumn };
        SizedSPropTagArray(nCols, columns) = {nCols, {PR_DISPLAY_NAME, PR_RECORD_KEY, PR_PARENT_ENTRYID}};
        SPropValue *properties(0);
        ULONG count;

        if (folder->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties) == S_OK) {
            LPSPropValue recordKeyProp(&properties[recordKeyColumn]);
            MapiRecordKey folderKey(reinterpret_cast<const char*>(recordKeyProp->Value.bin.lpb), recordKeyProp->Value.bin.cb);

            LPSPropValue entryIdProp(&properties[parentEntryIdColumn]);
            MapiEntryId parentEntryId(entryIdProp->Value.bin.lpb, entryIdProp->Value.bin.cb);

            QString displayName(QStringFromLpctstr(properties[displayNameColumn].Value.LPSZ));
            QMessageFolderId folderId(QMessageFolderIdPrivate::from(folderKey, storeRecordKey, entryId));
            QMessageAccountId accountId(QMessageAccountIdPrivate::from(storeRecordKey));

            MAPIFreeBuffer(properties);
            folder->Release();

            QStringList path;
            path.append(displayName);

            QMessageFolderId parentId;
            MapiEntryId ancestorEntryId(parentEntryId);
            LPMAPIFOLDER ancestorFolder(0);
            SPropValue *ancestorProperties(0);

            // Iterate through ancestors towards the root
            while (mapiSession->openEntry(&ignoredError, ancestorEntryId, &ancestorFolder) == S_OK) {
                if (ancestorFolder->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &ancestorProperties) == S_OK) {
                    ancestorFolder->Release();

                    LPSPropValue ancestorRecordKeyProp(&ancestorProperties[recordKeyColumn]);
                    MapiRecordKey ancestorRecordKey(reinterpret_cast<const char*>(ancestorRecordKeyProp->Value.bin.lpb), ancestorRecordKeyProp->Value.bin.cb);

                    if (ancestorEntryId == parentEntryId) { 
                        // This ancestor is the parent of the folder being retrieved, create a QMessageFolderId for the parent
                        parentId = QMessageFolderIdPrivate::from(ancestorRecordKey, storeRecordKey, parentEntryId);
                    }

                    LPSPropValue entryIdProp(&ancestorProperties[parentEntryIdColumn]);                    
                    ancestorEntryId = MapiEntryId(entryIdProp->Value.bin.lpb, entryIdProp->Value.bin.cb);

                    MAPIFreeBuffer(ancestorProperties);

                    if (ancestorRecordKey == storeRoot->recordKey()) {
                        // Reached the root and have a complete path for the folder being retrieved
                        return QMessageFolderPrivate::from(folderId, accountId, parentId, displayName, path.join("/"));
                    }

                    // Prepare to consider next ancestor
                    QString ancestorName(QStringFromLpctstr(ancestorProperties[displayNameColumn].Value.LPSZ));
                    if (!ancestorName.isEmpty())
                        path.prepend(ancestorName);
                } else {
                    ancestorFolder->Release();
                    break;
                }
            }
        }
    }

    // Failed to quickly retrieve the folder, fallback to an exhaustive search of all folders
    result = mapiStore->folderFromId(&d_ptr->p_ptr->lastError, id);
    return result;
}
#endif

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    QMessageAccount result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    MapiStorePtr mapiStore(mapiSession->findStore(&d_ptr->p_ptr->lastError, id));
    if (mapiStore->isValid()) {
        QMessageAccountId id(mapiStore->id());
        QString name(mapiStore->name());
        QMessage::TypeFlags types(QMessage::Email);
        if (mapiStore->name() == "SMS") { // On Windows Mobile SMS store is named "SMS"
            types = QMessage::TypeFlags(QMessage::Sms);
        }
        result = QMessageAccountPrivate::from(id, name, types);
    }
    return result;
}

void QMessageStore::startNotifications(const QMessageFilterKey &key)
{
    Q_UNUSED(key)    
}

void QMessageStore::stopNotifications()
{
}
