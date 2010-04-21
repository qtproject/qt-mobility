/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagemanager.h"
#include "qmessagestore_p.h"
#include "qmessage_p.h"
#include "qmessageid_p.h"
#include "qmessagefolderid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessageaccount_p.h"
#include "qmessagecontentcontainer.h"
#include "winhelpers_p.h"
#include <QCoreApplication>
#include <QMutex>
#include <qdebug.h>
#include <QThread>

QTM_BEGIN_NAMESPACE

namespace {

class MutexTryLocker
{
    QMutex *_mutex;
    QString _location;

public:
    MutexTryLocker(QMutex *mutex)
        : _mutex(0)
    {
        if (mutex->tryLock()) {
            _mutex = mutex;
        }
    }

    ~MutexTryLocker()
    {
        if (_mutex) {

            _mutex->unlock();

            QMessageManager::Error le = QMessageManager::NoError;
            MapiSessionPtr p = MapiSession::createSession(&le);
            if(le == QMessageManager::NoError)
                p->flushNotifyQueue();

        }
    }

    operator bool() const
    {
        return (_mutex != 0);
    }
};

}

class QMessageStorePrivatePlatform : public QObject
{
    Q_OBJECT

public:
    QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q);
    ~QMessageStorePrivatePlatform();

    QMessageStorePrivate *d_ptr;
    QMessageStore *q_ptr;
    QMessageManager::Error error;

    MapiSessionPtr session;
    QMutex mutex;

private slots:
    void appDestroyed();
};

QMessageStorePrivatePlatform::QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q)
    :d_ptr(d),
     q_ptr(q),
     error(QMessageManager::NoError),
     session(MapiSession::createSession(&error)),
     mutex(QMutex::Recursive)
{
    connect(QCoreApplication::instance(), SIGNAL(destroyed()), this, SLOT(appDestroyed()));

    if (session && (error == QMessageManager::NoError)) {
        MapiSession *o(session.data());
        connect(o, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)), q, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)));
        connect(o, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)), q, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)));
        connect(o, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)), q, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)));
    }
}

QMessageStorePrivatePlatform::~QMessageStorePrivatePlatform()
{
}

void QMessageStorePrivatePlatform::appDestroyed()
{
    // We need to terminate our session before main finishes
    session.clear();
}

QMessageStorePrivate::QMessageStorePrivate()
    :p_ptr(0),
     q_ptr(0)
{
}

QMessageStorePrivate::~QMessageStorePrivate()
{
    delete p_ptr;
}

void QMessageStorePrivate::initialize(QMessageStore *store)
{
    q_ptr = store;
    p_ptr = new QMessageStorePrivatePlatform(this, store);
}

#ifdef Q_OS_WIN
QMutex* QMessageStorePrivate::mutex(QMessageStore* store)
{
    return &(store->d_ptr->p_ptr->mutex);
}

QMutex* QMessageStorePrivate::mutex(QMessageManager& manager)
{
    return &(manager.store->d_ptr->p_ptr->mutex);
}
#endif

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
    if (!d->q_ptr) {
        d->initialize(new QMessageStore());
    }
    return d->q_ptr;
}

QMessageManager::Error QMessageStore::error() const
{
    return d_ptr->p_ptr->error;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageIdList result;

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);

    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
        result = d_ptr->p_ptr->session->queryMessages(&d_ptr->p_ptr->error, filter, sortOrder, limit, offset);
    }

    return result;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageIdList result;

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (matchFlags & QMessageDataComparator::MatchFullWord) {
        d_ptr->p_ptr->error = QMessageManager::NotYetImplemented;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
        result = d_ptr->p_ptr->session->queryMessages(&d_ptr->p_ptr->error, filter, sortOrder, limit, offset, body, matchFlags);
    }

    return result;
}

QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageFolderIdList result;

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    }

    d_ptr->p_ptr->error = QMessageManager::NoError;
    foreach (const MapiFolderPtr &folder, d_ptr->p_ptr->session->filterFolders(&d_ptr->p_ptr->error, filter, sortOrder, limit, offset)) {
        result.append(folder->id());
    }

    return result;
}

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageAccountIdList result;

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    }

    d_ptr->p_ptr->error = QMessageManager::NoError;
    foreach (const MapiStorePtr &store, d_ptr->p_ptr->session->filterStores(&d_ptr->p_ptr->error, filter, sortOrder, limit, offset)) {
        result.append(store->id());
    }

    return result;
}

int QMessageStore::countMessages(const QMessageFilter& filter) const
{
    return queryMessages(filter).count();
}

int QMessageStore::countFolders(const QMessageFolderFilter& filter) const
{
    return queryFolders(filter).count();
}

int QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
{
    return queryAccounts(filter).count();
}

bool QMessageStore::removeMessage(const QMessageId& id, QMessageManager::RemovalOption option)
{
    // TODO: implement option
    Q_UNUSED(option)

    bool result(false);

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;

        QMessageIdList ids;
        ids.append(id);
        d_ptr->p_ptr->session->removeMessages(&d_ptr->p_ptr->error, ids);

        result = (d_ptr->p_ptr->error == QMessageManager::NoError);
    }

    return result;
}

bool QMessageStore::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option)
{
    // TODO: implement option
    Q_UNUSED(option)

    bool result(false);

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;

        QMessageIdList ids = queryMessages(filter, QMessageSortOrder(), 0, 0);
        if (d_ptr->p_ptr->error == QMessageManager::NoError) {
            d_ptr->p_ptr->session->removeMessages(&d_ptr->p_ptr->error, ids);
        }

        result = (d_ptr->p_ptr->error == QMessageManager::NoError);
    }

    return result;
}

bool QMessageStore::addMessage(QMessage *message)
{
    bool result(false);

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
    }

    if (message && !message->id().isValid()) {
        QMessageManager::Error* lError = &d_ptr->p_ptr->error;

        MapiStorePtr mapiStore = d_ptr->p_ptr->session->findStore(lError,message->parentAccountId(),false);
        if (*lError == QMessageManager::NoError && !mapiStore.isNull()) {

            //check store/message type compatibility
            if(!(mapiStore->types() & message->type()))
            {
                *lError = QMessageManager::ConstraintFailure;
                return false;
            }

            MapiFolderPtr mapiFolder;

            // Find the parent folder for this message
            QMessageFolderId folderId(message->parentFolderId());
            if (folderId.isValid()) {
#ifdef _WIN32_WCE
                mapiFolder = mapiStore->openFolder(lError,QMessageFolderIdPrivate::entryId(folderId));
#else
                mapiFolder = mapiStore->openFolderWithKey(lError, QMessageFolderIdPrivate::folderRecordKey(folderId));
#endif
            } else {
                mapiFolder = mapiStore->findFolder(lError, message->standardFolder());
            }

            if (*lError == QMessageManager::NoError && !mapiFolder.isNull()) {
                IMessage* mapiMessage = mapiFolder->createMessage(lError, *message, d_ptr->p_ptr->session);
                if (*lError == QMessageManager::NoError) {
                    //set the new QMessageId
                    //we can only be guaranteed of an entry id after IMessage->SaveChanges has been called
#ifdef _WIN32_WCE
                    SizedSPropTagArray(1, columns) = {1, {PR_ENTRYID}};
#else
                    SizedSPropTagArray(2, columns) = {2, {PR_RECORD_KEY, PR_ENTRYID}};
#endif
                    SPropValue *properties(0);
                    ULONG count;
                    HRESULT rv = mapiMessage->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), 0, &count, &properties);
#ifdef _WIN32_WCE
                    if (HR_SUCCEEDED(rv) && (properties[0].ulPropTag == PR_ENTRYID)) {
#else
                    if (HR_SUCCEEDED(rv) && (properties[0].ulPropTag == PR_RECORD_KEY) && (properties[1].ulPropTag == PR_ENTRYID)) {
#endif
#ifdef _WIN32_WCE
                        MapiRecordKey recordKey;
                        MapiEntryId entryId(properties[0].Value.bin.lpb, properties[0].Value.bin.cb);
                        message->d_ptr->_id = QMessageIdPrivate::from(mapiFolder->storeEntryId(), entryId, recordKey, mapiFolder->entryId());
#else
                        MapiRecordKey recordKey(properties[0].Value.bin.lpb, properties[0].Value.bin.cb);
                        MapiEntryId entryId(properties[1].Value.bin.lpb, properties[1].Value.bin.cb);
                        message->d_ptr->_id = QMessageIdPrivate::from(mapiFolder->storeKey(), entryId, recordKey, mapiFolder->recordKey());
#endif
                        message->d_ptr->_modified = false;

                        MAPIFreeBuffer(properties);
                    } else {
                        qWarning() << "Unable to set the new ID in message.";
                        result = false;
                    }

                    mapiMessage->Release();
                    result = true;
                } else {
                    qWarning() << "Cannot createMessage";
                }
            } else {
                qWarning() << "Cannot get MAPI folder from store";
            }
        } else {
            qWarning() << "Cannot get default store";
        }
    } else {
        qWarning() << "Valid message ID at addition";
    }
    return result;
}

bool QMessageStore::updateMessage(QMessage *message)
{
    bool result(false);

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
    }

    //check store/message type compatibility
    if(MapiStorePtr mapiStore = d_ptr->p_ptr->session->findStore(&d_ptr->p_ptr->error,message->parentAccountId()))
    {
        if(!(mapiStore->types() & message->type()))
        {
            d_ptr->p_ptr->error = QMessageManager::ConstraintFailure;
            return false;
        }
    }
    else{
        qWarning() << "Unable to retrieve MAPI store for message at update";
        return false;
    }


    if (message && message->id().isValid()) {
        QMessageManager::Error* lError = &d_ptr->p_ptr->error;

        d_ptr->p_ptr->session->updateMessage(lError, *message);
        if (*lError == QMessageManager::NoError) {
            result = true;
        } else {
            qWarning() << "Cannot updateMessage";
        }
    } else {
        qWarning() << "Invalid message ID at update";
    }

    return result;
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    QMessage result;

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
    }

    return d_ptr->p_ptr->session->message(&d_ptr->p_ptr->error, id);
}

QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    QMessageFolder result;

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
    }

    return d_ptr->p_ptr->session->folder(&d_ptr->p_ptr->error, id);
}

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    QMessageAccount result;

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
    }

    MapiStorePtr mapiStore(d_ptr->p_ptr->session->findStore(&d_ptr->p_ptr->error, id));
    if (mapiStore && mapiStore->isValid()) {
        result = QMessageAccountPrivate::from(mapiStore->id(), mapiStore->name(), mapiStore->address(), mapiStore->types());
    }

    return result;
}

QMessageManager::NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter)
{
    QMessageManager::NotificationFilterId result(0);

    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return result;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
        return result;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
    }

    return d_ptr->p_ptr->session->registerNotificationFilter(&d_ptr->p_ptr->error, filter);
}

void QMessageStore::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    MutexTryLocker locker(&d_ptr->p_ptr->mutex);
    if (!locker) {
        d_ptr->p_ptr->error = QMessageManager::Busy;
        return;
    }

    if (!d_ptr->p_ptr->session) {
        d_ptr->p_ptr->error = QMessageManager::ContentInaccessible;
    } else {
        d_ptr->p_ptr->error = QMessageManager::NoError;
        d_ptr->p_ptr->session->unregisterNotificationFilter(&d_ptr->p_ptr->error, notificationFilterId);
    }
}

#include "qmessagestore_win.moc"

QTM_END_NAMESPACE
