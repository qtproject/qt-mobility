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
#include "qmfstore_maemo6_p.h"
#include "qmessagefolderid_p.h"
#include "telepathyengine_maemo6_p.h"
#include "maemo6helpers_p.h"
#include "storageengine_maemo6_p.h"
#include <QDebug>


QTM_BEGIN_NAMESPACE

class QMessageStorePrivatePlatform
{
public:
    QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q)
        :d_ptr(d), q_ptr(q) {}
    QMessageStorePrivate *d_ptr;
    QMessageStore *q_ptr;
};

QMessageStorePrivate::QMessageStorePrivate()
    :q_ptr(0),
     p_ptr(0),
     error(QMessageManager::NoError)
{

}

QMessageStorePrivate::~QMessageStorePrivate()
{
}

void QMessageStorePrivate::initialize(QMessageStore *store)
{ 
    q_ptr = store;
    p_ptr = new QMessageStorePrivatePlatform(this, store);

    StorageEngine::instance();//->initialize(true);
    QMFStore::instance();
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
    qDebug() << "QMessageStore::QMessageStore";
 //   d_ptr->initialize(this);
    qDebug() << "QMessageStore::QMessageStore exit";

    connect(QMFStore::instance(), SIGNAL(messageAdded(const QMessageId &, const QMessageManager::NotificationFilterIdSet)), 
	    this, SIGNAL(messageAdded(const QMessageId &, const QMessageManager::NotificationFilterIdSet)));
    connect(QMFStore::instance(), SIGNAL(messageUpdated(const QMessageId &, const QMessageManager::NotificationFilterIdSet &)),
	    this, SIGNAL(messageUpdated(const QMessageId &, const QMessageManager::NotificationFilterIdSet &)));
    connect(QMFStore::instance(), SIGNAL(messageRemoved(const QMessageId &, const QMessageManager::NotificationFilterIdSet &)),
	    this, SIGNAL(messageRemoved(const QMessageId &, const QMessageManager::NotificationFilterIdSet &)));

    connect(StorageEngine::instance(), SIGNAL(messageAdded(const QMessageId &, const QMessageManager::NotificationFilterIdSet)), 
	    this, SIGNAL(messageAdded(const QMessageId &, const QMessageManager::NotificationFilterIdSet)));
    connect(StorageEngine::instance(), SIGNAL(messageUpdated(const QMessageId &, const QMessageManager::NotificationFilterIdSet &)),
	    this, SIGNAL(messageUpdated(const QMessageId &, const QMessageManager::NotificationFilterIdSet &)));
    connect(StorageEngine::instance(), SIGNAL(messageRemoved(const QMessageId &, const QMessageManager::NotificationFilterIdSet &)),
	    this, SIGNAL(messageRemoved(const QMessageId &, const QMessageManager::NotificationFilterIdSet &)));

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
    if (d_ptr->error != QMessageManager::NoError) {
	return d_ptr->error;
    } else if (StorageEngine::instance()->error() != QMessageManager::NoError) {
	return StorageEngine::instance()->error();
    }
    return QMFStore::instance()->error();
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    bool useHelpers(false);
    QMessageIdList messageIds;

    QMessageFilter handledFilter = filter;
    MessagingHelper::handleNestedFiltersFromMessageFilter(handledFilter);

    // returns the filtered but not ordered list
    messageIds = StorageEngine::instance()->queryMessagesSync(handledFilter, sortOrder, limit, offset);    

    // QMFStore does not need to use helpers, but messages from StorageEngine needs to be sorted
    if (messageIds.count() > 0)
	useHelpers = true;

    messageIds += QMFStore::instance()->queryMessages(handledFilter, sortOrder, limit, offset, d_ptr->error);

    // since QMFStore does sorting and applies offset and limits, 
    // we need to do those here only if there are messages from StorageEngine as well
    if (useHelpers) {
	MessagingHelper::orderMessages(messageIds, sortOrder);        
	MessagingHelper::applyOffsetAndLimitToMessageIdList(messageIds, limit, offset);    
    }

    d_ptr->error = QMessageManager::NoError;

    return messageIds;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    bool useHelpers(false);
    QMessageIdList messageIds;

    QMessageFilter handledFilter = filter;
    MessagingHelper::handleNestedFiltersFromMessageFilter(handledFilter);

    // returns the filtered but not ordered list
    messageIds = StorageEngine::instance()->queryMessagesSync(handledFilter, body, matchFlags, sortOrder, limit, offset);

    // QMFStore does not need to use helpers, but messages from StorageEngine needs to be sorted
    if (messageIds.count() > 0)
	useHelpers = true;

    messageIds += QMFStore::instance()->queryMessages(handledFilter, body, matchFlags, sortOrder, limit, offset, d_ptr->error);

    // since QMFStore does sorting and applies offset and limits, 
    // we need to do those here only if there are messages from StorageEngine as well
    if (useHelpers) {
	MessagingHelper::orderMessages(messageIds, sortOrder);
	MessagingHelper::applyOffsetAndLimitToMessageIdList(messageIds, limit, offset);
    }

    d_ptr->error = QMessageManager::NoError;

    return messageIds;
}

QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageFolderIdList folderIds;
    QMessageFolderFilter handledFilter = filter;
    MessagingHelper::handleNestedFiltersFromFolderFilter(handledFilter);

    folderIds = QMFStore::instance()->queryFolders(handledFilter, sortOrder, limit, offset, d_ptr->error);


    folderIds += StorageEngine::instance()->queryFolders(handledFilter, sortOrder, limit, offset);

    // we have to sort the list after concatenation
    MessagingHelper::orderFolders(folderIds, sortOrder);

    MessagingHelper::applyOffsetAndLimitToFolderIdList(folderIds, limit, offset);
    
    d_ptr->error = QMessageManager::NoError;

    return folderIds;
}

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{

    QMessageAccountIdList accountIds;

    bool isFiltered = true;
    bool isSorted = true;

    accountIds = TelepathyEngine::instance()->queryAccounts(filter, sortOrder, limit, offset, isFiltered, isSorted);

    if (!isFiltered) {
        MessagingHelper::filterAccounts(accountIds, filter);
    }

    // QMFStore does both filtering and sorting
    accountIds += QMFStore::instance()->queryAccounts(filter, sortOrder, limit, offset, d_ptr->error);

    // sort messages if it is needed
    if (!isSorted) {
        MessagingHelper::orderAccounts(accountIds, sortOrder);
    }

    MessagingHelper::applyOffsetAndLimitToAccountIdList(accountIds, limit, offset);

    d_ptr->error = QMessageManager::NoError;

    return accountIds;
}

int QMessageStore::countMessages(const QMessageFilter& filter) const
{
    int count = 0;

    d_ptr->error = QMessageManager::NoError;

    count += QMFStore::instance()->countMessages(filter, d_ptr->error);
    count += StorageEngine::instance()->countMessagesSync(filter);

    return count;
}

int QMessageStore::countFolders(const QMessageFolderFilter& filter) const
{
    int count = 0;

    d_ptr->error = QMessageManager::NoError;

    count += QMFStore::instance()->countFolders(filter, d_ptr->error);
    count += StorageEngine::instance()->countFolders(filter);

    return count;
}

int QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
{
    int count = 0;

    d_ptr->error = QMessageManager::NoError;

    count += QMFStore::instance()->countAccounts(filter, d_ptr->error);
    count += TelepathyEngine::instance()->countAccounts(filter);

    return count;
}

bool QMessageStore::removeMessage(const QMessageId& id, QMessageManager::RemovalOption option)
{
    bool ret = false;

    if (!id.isValid()) {
	d_ptr->error = QMessageManager::InvalidId;
	return false;
    }

    d_ptr->error = QMessageManager::NoError;

    if (id.toString().startsWith("QMF_")) {
	ret = QMFStore::instance()->removeMessage(id, option, d_ptr->error);
    } else {
	ret = StorageEngine::instance()->removeMessage(id);
    }

    return ret;
}

bool QMessageStore::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option)
{
    bool noErrors;

    d_ptr->error = QMessageManager::NoError;

    noErrors = QMFStore::instance()->removeMessages(filter, option, d_ptr->error);

    QMessageIdList ids = StorageEngine::instance()->queryMessagesSync(filter, QMessageSortOrder(), 0, 0);
    for (int i = 0; i < ids.size(); i++) {
        if (!removeMessage(ids.at(i), option))
            noErrors = false;
    }
    
    return noErrors;
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
	    d_ptr->error = QMessageManager::ConstraintFailure;
        }
    }

    if (retVal) {
        // Check account
        if (!accountId.isValid()) {
            accountId = QMessageAccount::defaultAccount(m->type());
            if (!accountId.isValid()) {
		d_ptr->error = QMessageManager::ConstraintFailure;
                retVal = false;
            }
        }
    }

    QMessageAccount account(accountId);
    if (retVal) {
        // Check account/message type compatibility
        if (!(account.messageTypes() & m->type()) && (msgType == QMessage::NoType)) {
            retVal = false;
	    d_ptr->error = QMessageManager::ConstraintFailure;
        }
    }

    if (retVal) {
        // Set default account if unset
        if (!m->parentAccountId().isValid()) {
            m->setParentAccountId(accountId);
        }
        if (account.messageTypes() & QMessage::Sms) {
	    retVal = StorageEngine::instance()->addMessage(*m);
        } else if (account.messageTypes() & QMessage::InstantMessage) {
            retVal = false; //TODO:
	    d_ptr->error = QMessageManager::NotYetImplemented;
            qWarning() << "QMessageManager::add not yet implemented for Instant Message";
        } else if (account.messageTypes() & QMessage::Mms) {
            retVal = false; //TODO:
	    d_ptr->error = QMessageManager::NotYetImplemented;
            qWarning() << "QMessageManager::add not yet implemented for MMS";
        } else if (account.messageTypes() & QMessage::Email) {
            retVal = QMFStore::instance()->addMessage(*m, d_ptr->error);
	}
    }

    return retVal;
}

bool QMessageStore::updateMessage(QMessage *m)
{
    bool retVal = false;

    d_ptr->error = QMessageManager::NoError;

    if (m->type() == QMessage::Sms) {
        //retVal = false; //TODO:
        //qWarning() << "QMessageManager::update not yet implemented for SMS";
        retVal = StorageEngine::instance()->updateMessage(*m);
    } else if (m->type() == QMessage::InstantMessage) {
	d_ptr->error = QMessageManager::NotYetImplemented;
	retVal = false; //TODO:
        qWarning() << "QMessageManager::update not yet implemented for Instant Message";
    } else if (m->type() == QMessage::Mms) {
	d_ptr->error = QMessageManager::NotYetImplemented;
        retVal = false; //TODO:
        qWarning() << "QMessageManager::update not yet implemented for Instant MMS";
    } else if (m->type() == QMessage::Email) {
        retVal = QMFStore::instance()->updateMessage(*m, d_ptr->error);
    } else {
	d_ptr->error = QMessageManager::ConstraintFailure;
    }

    return retVal;
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    if (!id.isValid()) {
	d_ptr->error = QMessageManager::InvalidId;
	return QMessage();
    }

    d_ptr->error = QMessageManager::NoError;

    if (id.toString().startsWith("QMF_")) {
	return QMFStore::instance()->message(id, d_ptr->error); 
    } else {
	return StorageEngine::instance()->message(id);
    }
}

QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    if (!id.isValid()) {
	d_ptr->error = QMessageManager::InvalidId;
	return QMessageFolder();
    }

    d_ptr->error = QMessageManager::NoError;

    if (id.toString().startsWith("QMF_")) {
	return QMFStore::instance()->folder(id, d_ptr->error);     
    } else if (id.toString().startsWith(FOLDER_PREFIX_SMS)) {
        return StorageEngine::instance()->folder(id);
    }

    return QMessageFolder();
}

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    if (!id.isValid()) {
	d_ptr->error = QMessageManager::InvalidId;
	return QMessageAccount();
    }

    d_ptr->error = QMessageManager::NoError;

    if (id.toString().startsWith("QMF_")) {
	return QMFStore::instance()->account(id, d_ptr->error);
    } else {
        return TelepathyEngine::instance()->account(id);
    }

    return QMessageAccount();
}

QMessageManager::NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter)
{    
    QMessageManager::NotificationFilterId id;
    
    id = QMFStore::instance()->registerNotificationFilter(filter);
    StorageEngine::instance()->registerNotificationFilter(id, filter);
    
    return id;
}

void QMessageStore::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{    
    QMFStore::instance()->unregisterNotificationFilter(notificationFilterId);
    StorageEngine::instance()->unregisterNotificationFilter(notificationFilterId);
}

QTM_END_NAMESPACE
