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
#include "qmessagestore_symbian_p.h"
#include "qmtmengine_symbian_p.h"
#ifdef FREESTYLEMAILUSED
#include "qfsengine_symbian_p.h"
#endif

#include <QString>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QMessageStorePrivate,messageStorePrivate);

QMessageStorePrivate::QMessageStorePrivate()
  : q_ptr(0),
    _mtmEngine(0),
    _error(QMessageManager::NoError)
{
}

QMessageStorePrivate::~QMessageStorePrivate()
{
}

void QMessageStorePrivate::initialize(QMessageStore *store)
{
    q_ptr = store;
    _mtmEngine = CMTMEngine::instance();
  //  _fsEngine = CFSEngine::instance();
}

QMessageIdList QMessageStorePrivate::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageIdList ids;
    QMessageService service;
    connect(&service, SIGNAL(messagesFound(const QMessageIdList&)), this, SLOT(messagesFound(const QMessageIdList&)));
    if (service.queryMessages(filter, sortOrder, limit, offset)) {
        QObject::connect(&service, SIGNAL(messagesFound(const QMessageIdList&)), &loop, SLOT(quit()));
        QObject::connect(&service, SIGNAL(stateChanged(QMessageService::State)), this, SLOT(stateChanged(QMessageService::State)));
        loop.exec();
        ids = _ids;
        _ids.clear();
    }
    return ids;
}

QMessageIdList QMessageStorePrivate::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageIdList ids;
    QMessageService service;
    connect(&service, SIGNAL(messagesFound(const QMessageIdList&)), this, SLOT(messagesFound(const QMessageIdList&)));
    if (service.queryMessages(filter, body, matchFlags, sortOrder, limit, offset)) {
        QObject::connect(&service, SIGNAL(messagesFound(const QMessageIdList&)), &loop, SLOT(quit()));
        QObject::connect(&service, SIGNAL(stateChanged(QMessageService::State)), this, SLOT(stateChanged(QMessageService::State)));
        loop.exec();
        ids = _ids;
        _ids.clear();
    }
    return ids;
}

int QMessageStorePrivate::countMessages(const QMessageFilter& filter) const
{
    int count = 0;
    QMessageService service;
    connect(&service, SIGNAL(messagesCounted(int)), this, SLOT(messagesCounted(int)));
    if (service.countMessages(filter)) {
        QObject::connect(&service, SIGNAL(messagesCounted(int)), &loop, SLOT(quit()));
        QObject::connect(&service, SIGNAL(stateChanged(QMessageService::State)), this, SLOT(stateChanged(QMessageService::State)));
        loop.exec();
        count = _count;
        _count = 0;
    }
    return count;
}

void QMessageStorePrivate::stateChanged(QMessageService::State newState)
{
    if (newState == QMessageService::FinishedState) {
        loop.quit();
    }
}

void QMessageStorePrivate::messagesFound(const QMessageIdList &ids)
{
    _ids = ids;
    loop.quit();
}

void QMessageStorePrivate::messagesCounted(int count)
{
    _count = count;
    loop.quit();
}

QMessageAccountIdList QMessageStorePrivate::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    //return CFSEngine::instance()->queryAccounts(filter, sortOrder, limit, offset);
    return _mtmEngine->queryAccounts(filter, sortOrder, limit, offset);
}

int QMessageStorePrivate::countAccounts(const QMessageAccountFilter &filter) const
{
    return _mtmEngine->countAccounts(filter);
}

QMessageFolderIdList QMessageStorePrivate::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    return _mtmEngine->queryFolders(filter, sortOrder, limit, offset);
}

int QMessageStorePrivate::countFolders(const QMessageFolderFilter& filter) const
{
    return _mtmEngine->countFolders(filter);
}

QMessageFolder QMessageStorePrivate::folder(const QMessageFolderId& id) const
{
    return _mtmEngine->folder(id);
}


bool QMessageStorePrivate::addMessage(QMessage *m)
{
    return _mtmEngine->addMessage(m);
}

bool QMessageStorePrivate::updateMessage(QMessage *m)
{
    return _mtmEngine->updateMessage(m);
}

bool QMessageStorePrivate::removeMessage(const QMessageId &id, QMessageManager::RemovalOption option)
{
    return _mtmEngine->removeMessage(id, option);
}

bool QMessageStorePrivate::removeMessages(const QMessageFilter &filter, QMessageManager::RemovalOption option)
{
    bool retVal = true;
    
    QMessageIdList ids;
    QMessageService service;
    connect(&service, SIGNAL(messagesFound(const QMessageIdList&)), this, SLOT(messagesFound(const QMessageIdList&)));
    if (service.queryMessages(filter)) {
        QObject::connect(&service, SIGNAL(messagesFound(const QMessageIdList&)), &loop, SLOT(quit()));
        QObject::connect(&service, SIGNAL(stateChanged(QMessageService::State)), this, SLOT(stateChanged(QMessageService::State)));
        loop.exec();
        ids = _ids;
        _ids.clear();
        for (int i=0; i < ids.count(); i++) {
            if (!_mtmEngine->removeMessage(ids[i], option)) {
                retVal = false;
            }
        }
    } else {
        retVal = false;
    }
    
    return retVal;
}

QMessage QMessageStorePrivate::message(const QMessageId& id) const
{
    return _mtmEngine->message(id);
}

QMessageAccount QMessageStorePrivate::account(const QMessageAccountId &id) const
{
    return _mtmEngine->account(id);
    //return CFSEngine::instance()->account(id);
}

QMessageManager::NotificationFilterId QMessageStorePrivate::registerNotificationFilter(const QMessageFilter &filter)
{
    return _mtmEngine->registerNotificationFilter(*this, filter);
}

void QMessageStorePrivate::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    _mtmEngine->unregisterNotificationFilter(notificationFilterId);    
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

QMessageStore::QMessageStore(QObject *parent)
    : QObject(parent),
      d_ptr(messageStorePrivate())
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
    QMessageStorePrivate *d = messageStorePrivate();
    Q_ASSERT(d != 0);
    if (!d->q_ptr)
        new QMessageStore();
    return d->q_ptr;
}

QMessageManager::Error QMessageStore::error() const
{
    return QMessageManager::NoError;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return messageStorePrivate()->queryMessages(filter, sortOrder, limit, offset);
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return messageStorePrivate()->queryMessages(filter, body, matchFlags, sortOrder, limit, offset);
}

QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    return messageStorePrivate()->queryFolders(filter, sortOrder, limit, offset);
}

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    return messageStorePrivate()->queryAccounts(filter, sortOrder, limit, offset);
}

int QMessageStore::countMessages(const QMessageFilter& filter) const
{
    return messageStorePrivate()->countMessages(filter);
}

int QMessageStore::countFolders(const QMessageFolderFilter& filter) const
{
    return messageStorePrivate()->countFolders(filter);
}

int QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
{
    return messageStorePrivate()->countAccounts(filter);
}

bool QMessageStore::removeMessage(const QMessageId& id, QMessageManager::RemovalOption option)
{
    return messageStorePrivate()->removeMessage(id, option);
}

bool QMessageStore::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option)
{
    return messageStorePrivate()->removeMessages(filter, option);
}

bool QMessageStore::addMessage(QMessage *m)
{
    return messageStorePrivate()->addMessage(m);   	
}

bool QMessageStore::updateMessage(QMessage *m)
{
	return messageStorePrivate()->updateMessage(m);
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    return messageStorePrivate()->message(id);
}

QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    return messageStorePrivate()->folder(id);
}

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    return messageStorePrivate()->account(id);
}

QMessageManager::NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter)
{
    return messageStorePrivate()->registerNotificationFilter(filter);
}

void QMessageStore::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    messageStorePrivate()->unregisterNotificationFilter(notificationFilterId);
}

#include "moc_qmessagestore_symbian_p.cpp"
QTM_END_NAMESPACE
