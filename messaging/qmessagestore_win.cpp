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
#include "qmessagefilter_p.h"
#include "winhelpers_p.h"
#include <qdebug.h>

using namespace WinHelpers;

struct FolderHeapNode {
    QMessageFilter filter;
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

// FolderHeap is a binary heap used to merge sort messages from different folders and stores
class FolderHeap {
public:
    FolderHeap(QMessageStore::ErrorCode *lastError, MapiSessionPtr mapiSession, const QList<FolderHeapNodePtr> &protoHeap, const QMessageOrdering &ordering);
    QMessage takeFront(QMessageStore::ErrorCode *lastError);
    bool isEmpty() const { return _heap.count() == 0; }

private:
    void sink(int i); // Also known as sift-down
    QMessageFilter _filter;
    QMessageOrdering _ordering;
    QList<FolderHeapNodePtr> _heap;
    MapiSessionPtr _mapiSession;
};

FolderHeap::FolderHeap(QMessageStore::ErrorCode *lastError, MapiSessionPtr mapiSession, const QList<FolderHeapNodePtr> &protoHeap, const QMessageOrdering &ordering)
{
    Q_UNUSED(lastError)

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
        QMessageIdList messageIdList(node->folder->queryMessages(&ignoredError, node->filter, ordering, 1));
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
    QMessageIdList messageIdList(node->folder->queryMessages(lastError, node->filter, _ordering, 1, node->offset));
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

    MapiStoreIterator storeIt(QMessageFilterPrivate::storeIterator(filter, &d_ptr->p_ptr->lastError, mapiSession));
    for (MapiStorePtr store(storeIt.next()); store && store->isValid(); store = storeIt.next()) {
        MapiFolderIterator folderIt(QMessageFilterPrivate::folderIterator(filter, &d_ptr->p_ptr->lastError, store));
        for (MapiFolderPtr folder(folderIt.next()); folder && folder->isValid(); folder = folderIt.next()) {
            FolderHeapNodePtr node(new FolderHeapNode);
            node->filter = filter;
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
        }
    }

    FolderHeap folderHeap(&d_ptr->p_ptr->lastError, mapiSession, folderNodes, ordering);
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

bool QMessageStore::addMessage(QMessage *message)
{
    bool result(false);

    if (!message || !message->id().isValid()) {

        QMessageStore::ErrorCode* lError = &d_ptr->p_ptr->lastError;

        *lError = QMessageStore::NoError;

        MapiSessionPtr session(MapiSession::createSession(lError, d_ptr->p_ptr->_mapiInitialized));
        if (*lError == QMessageStore::NoError) {

            MapiStorePtr mapiStore = session->findStore(lError,message->parentAccountId(),false);

            if (*lError == QMessageStore::NoError && !mapiStore.isNull()) {

                MapiFolderPtr mapiFolder;

                // Find the parent folder for this message

                QMessageFolder folder(message->parentFolderId());

                *lError = QMessageStore::NoError;

                if(folder.id().isValid())
                    mapiFolder = mapiStore->findFolder(lError, QMessageFolderIdPrivate::folderRecordKey(folder.id()));
                else
                    mapiFolder = mapiStore->findFolder(lError, message->standardFolder());

                if(*lError == QMessageStore::NoError && !mapiFolder.isNull()) {
                    IMessage* mapiMessage = mapiFolder->createMessage(*message,session,lError);
                    if (*lError == QMessageStore::NoError && mapiMessage) {

                        if (FAILED(mapiMessage->SaveChanges(0))) {
                            qWarning() << "Unable to save changes on message.";
                            mapiRelease(mapiMessage);
                            return false;
                        }

                        //set the new QMessageId
                        //we can only be guaranteed of an entry id after IMessage->SaveChanges has been called

                        SizedSPropTagArray(2, columns) = {2, {PR_RECORD_KEY, PR_ENTRYID}};
                        SPropValue *properties(0);
                        ULONG count;
                        HRESULT rv = mapiMessage->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), 0, &count, &properties);
                        if (HR_SUCCEEDED(rv) && (properties[0].ulPropTag == PR_RECORD_KEY) && (properties[1].ulPropTag == PR_ENTRYID)) {
                            MapiRecordKey recordKey(properties[0].Value.bin.lpb, properties[0].Value.bin.cb);
                            MapiEntryId entryId(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);
                            message->d_ptr->_id = QMessageIdPrivate::from(recordKey, mapiFolder->recordKey(), mapiFolder->storeKey(), entryId);
                            MAPIFreeBuffer(properties);
                        }
                        else
                        {
                            qWarning() << "Unable to set the new ID in message.";
                            result = false;
                        }

                        mapiMessage->Release();

                        result = true;

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

QMessageStore::NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter)
{
    Q_UNUSED(filter)
    return 0; // stub
}

void QMessageStore::unregisterNotificationFilter(NotificationFilterId notificationFilterId)
{
    Q_UNUSED(notificationFilterId)
}

