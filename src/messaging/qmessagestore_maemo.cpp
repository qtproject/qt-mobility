/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagestore.h"
#include "qmessagestore_p.h"
#include "modestengine_maemo_p.h"
#include "telepathyengine_maemo_p.h"
#include "maemohelpers_p.h"
#include "eventloggerengine_maemo_p.h"


QTM_BEGIN_NAMESPACE

class QMessageStorePrivatePlatform
{
public:
    QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q)
        :d_ptr(d), q_ptr(q) {}
    QMessageStorePrivate *d_ptr;
    QMessageStore *q_ptr;
    EventLoggerEngine *el;
};

QMessageStorePrivate::QMessageStorePrivate()
    :q_ptr(0),
     p_ptr(0)
{

}

QMessageStorePrivate::~QMessageStorePrivate()
{
}

void QMessageStorePrivate::initialize(QMessageStore *store)
{ 
    q_ptr = store;
    p_ptr = new QMessageStorePrivatePlatform(this, store);
    p_ptr->el= EventLoggerEngine::instance();
}

void QMessageStorePrivate::messageNotification(QMessageStorePrivate::NotificationType type, const QMessageId& id,
                                               const QMessageManager::NotificationFilterIdSet &matchingFilters)
{
    switch (type) {
    case Added:
        emit q_ptr->messageAdded(id, matchingFilters);
        break;
    case Updated:
        emit q_ptr->messageUpdated(id, matchingFilters);
        break;
    case Removed:
        emit q_ptr->messageRemoved(id, matchingFilters);
        break;
    }
}


Q_GLOBAL_STATIC(QMessageStorePrivate,data);

QMessageStore::QMessageStore(QObject *parent)
    : QObject(parent),
      d_ptr(data())
{
    Q_ASSERT(d_ptr != 0);
    Q_ASSERT(d_ptr->q_ptr == 0); // QMessageStore should be singleton
 //   d_ptr->initialize(this);
    // be sure that singletons are initialized
    EventLoggerEngine::instance();
    TelepathyEngine::instance();
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

QMessageManager::Error QMessageStore::error() const
{
    return QMessageManager::NoError;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageIdList messageIds;

    bool isFiltered = false;
    bool isSorted = false;
    
    messageIds = ModestEngine::instance()->queryMessagesSync(filter, sortOrder, limit, offset,
                                                             isFiltered, isSorted);
    
    //    messageIds += d_ptr->p_ptr->el->filterAndOrderMessages(filter,sortOrder,QString(),QMessageDataComparator::MatchFlags());
    messageIds += EventLoggerEngine::instance()->filterAndOrderMessages(filter,sortOrder,QString(),QMessageDataComparator::MatchFlags());

    if (!isFiltered) {
        MessagingHelper::filterMessages(messageIds, filter);
    }
    if (!isSorted) {
        MessagingHelper::orderMessages(messageIds, sortOrder);
    }
    MessagingHelper::applyOffsetAndLimitToMessageIdList(messageIds, limit, offset);

    return messageIds;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{

    QMessageIdList messageIds;

    bool isFiltered = false;
    bool isSorted = false;
    messageIds = ModestEngine::instance()->queryMessagesSync(filter, body, matchFlags, sortOrder, limit, offset,
                                                             isFiltered, isSorted);
    //    messageIds +=d_ptr->p_ptr->el->filterAndOrderMessages(filter,sortOrder,body,matchFlags);
    messageIds +=EventLoggerEngine::instance()->filterAndOrderMessages(filter,sortOrder,body,matchFlags);

    if (!isFiltered) {
        MessagingHelper::filterMessages(messageIds, filter);
    }
    if (!isSorted) {
        MessagingHelper::orderMessages(messageIds, sortOrder);
    }
    MessagingHelper::applyOffsetAndLimitToMessageIdList(messageIds, limit, offset);

    return messageIds;
}

QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageFolderIdList folderIds;

    bool isFiltered = false;
    bool isSorted = false;
    folderIds = ModestEngine::instance()->queryFolders(filter, sortOrder, limit, offset, isFiltered, isSorted);
    if (!isFiltered) {
        MessagingHelper::filterFolders(folderIds, filter);
    }
    if (!isSorted) {
        MessagingHelper::orderFolders(folderIds, sortOrder);
    }
    MessagingHelper::applyOffsetAndLimitToFolderIdList(folderIds, limit, offset);

    return folderIds;
}

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageAccountIdList accountIds;

    bool isFiltered = false;
    bool isSorted = false;
    accountIds = ModestEngine::instance()->queryAccounts(filter, sortOrder, limit, offset, isFiltered, isSorted);
    if (!isFiltered) {
        MessagingHelper::filterAccounts(accountIds, filter);
    }
    accountIds += TelepathyEngine::instance()->queryAccounts(filter, sortOrder, limit, offset, isFiltered, isSorted);
    if (!isFiltered) {
        MessagingHelper::filterAccounts(accountIds, filter);
    }
    if (!isSorted) {
        MessagingHelper::orderAccounts(accountIds, sortOrder);
    }
    MessagingHelper::applyOffsetAndLimitToAccountIdList(accountIds, limit, offset);

    return accountIds;
}

int QMessageStore::countMessages(const QMessageFilter& filter) const
{
    int count = 0;

    count += ModestEngine::instance()->countMessagesSync(filter);

    return count;
}

int QMessageStore::countFolders(const QMessageFolderFilter& filter) const
{
    int count = 0;

    count += ModestEngine::instance()->countFolders(filter);

    return count;
}

int QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
{
    int count = 0;

    count += ModestEngine::instance()->countAccounts(filter);
    count += TelepathyEngine::instance()->countAccounts(filter);
    return count;
}

bool QMessageStore::removeMessage(const QMessageId& id, QMessageManager::RemovalOption option)
{
    if (id.toString().startsWith("MO_")) {
        return ModestEngine::instance()->removeMessage(id, option);
    }

    return false;
}

bool QMessageStore::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option)
{
    QMessageIdList ids = queryMessages(filter, QMessageSortOrder(), 0, 0);

    for (int i=0; i < ids.count(); i++) {
        if (ids[i].toString().startsWith("MO_")) {
            if (!ModestEngine::instance()->removeMessage(ids[i], option)) {
                return false;
            }
        }
    }

    return true;
}

bool QMessageStore::addMessage(QMessage *m)
{
    bool retVal = true;

    QMessageAccountId accountId = m->parentAccountId();
    QMessage::Type msgType = QMessage::NoType;

    // Check message type
    if (m->type() == QMessage::AnyType || m->type() == QMessage::NoType) {
        if (accountId.isValid()) {
            // ParentAccountId was defined => Message type can be read
            // from parent account
            QMessageAccount account = QMessageAccount(accountId);
            QMessage::TypeFlags types = account.messageTypes();
            if (types & QMessage::Sms) {
                msgType = QMessage::Sms;
            } else if (account.messageTypes() & QMessage::InstantMessage) {
                msgType = QMessage::InstantMessage;
            } else if (types & QMessage::Mms) {
                msgType = QMessage::Mms;
            } else if (types & QMessage::Email) {
                msgType = QMessage::Email;
            }
        }
        if (msgType == QMessage::NoType) {
            retVal = false;
        }
    }

    if (retVal) {
        // Check account
        if (!accountId.isValid()) {
            accountId = QMessageAccount::defaultAccount(m->type());
            if (!accountId.isValid()) {
                retVal = false;
            }
        }
    }

    QMessageAccount account(accountId);
    if (retVal) {
        // Check account/message type compatibility
        if (!(account.messageTypes() & m->type()) && (msgType == QMessage::NoType)) {
            retVal = false;
        }
    }

    if (retVal) {
        // Set default account if unset
        if (!m->parentAccountId().isValid()) {
            m->setParentAccountId(accountId);
        }

        if (account.messageTypes() & QMessage::Sms) {
            retVal = false; //TODO:
            qWarning() << "QMessageManager::add not yet implemented for SMS";
        } else if (account.messageTypes() & QMessage::InstantMessage) {
            retVal = false; //TODO:
            qWarning() << "QMessageManager::add not yet implemented for Instant Message";
        } else if (account.messageTypes() & QMessage::Mms) {
            retVal = false; //TODO:
            qWarning() << "QMessageManager::add not yet implemented for MMS";
        } else if (account.messageTypes() & QMessage::Email) {
            retVal = ModestEngine::instance()->addMessage(*m);
        }
    }

    return retVal;
}

bool QMessageStore::updateMessage(QMessage *m)
{
    bool retVal = false;

    if (m->type() == QMessage::Sms) {
        retVal = false; //TODO:
        qWarning() << "QMessageManager::update not yet implemented for SMS";
    } else if (m->type() == QMessage::InstantMessage) {
        retVal = false; //TODO:
        qWarning() << "QMessageManager::update not yet implemented for Instant Message";
    } else if (m->type() == QMessage::Mms) {
        retVal = false; //TODO:
        qWarning() << "QMessageManager::update not yet implemented for Instant MMS";
    } else if (m->type() == QMessage::Email) {
        retVal = ModestEngine::instance()->updateMessage(*m);
    }

    return retVal;
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    if (id.toString().startsWith("MO_")) {
        return ModestEngine::instance()->message(id);
    } else {
        return d_ptr->p_ptr->el->message(id);
    }
}

QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    if (id.toString().startsWith("MO_")) {
        return ModestEngine::instance()->folder(id);
    }

    return QMessageFolder();
}

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    if (id.toString().startsWith("MO_")) {
        return ModestEngine::instance()->account(id);
    } else {
        return  TelepathyEngine::instance()->account(id);
    }
}

QMessageManager::NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter)
{
    QMessageManager::NotificationFilterId id = d_ptr->p_ptr->el->registerNotificationFilter(*d_ptr,filter);
    return ModestEngine::instance()->registerNotificationFilter(*data(), filter, id);
}

void QMessageStore::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    ModestEngine::instance()->unregisterNotificationFilter(notificationFilterId);
    d_ptr->p_ptr->el->unregisterNotificationFilter( notificationFilterId);
}


QTM_END_NAMESPACE
