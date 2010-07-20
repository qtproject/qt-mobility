/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


QTM_BEGIN_NAMESPACE

class QMessageStorePrivatePlatform
{
public:
    QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q)
        :d_ptr(d), q_ptr(q) {}
    QMessageStorePrivate *d_ptr;
    QMessageStore *q_ptr;
    //...
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

QMessageManager::Error QMessageStore::error() const
{
    return QMessageManager::NotYetImplemented;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    Q_UNUSED(filter)
    Q_UNUSED(sortOrder)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageIdList(); // stub
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    Q_UNUSED(filter)
    Q_UNUSED(sortOrder)
    Q_UNUSED(body)
    Q_UNUSED(matchFlags)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageIdList(); // stub
}

QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    Q_UNUSED(filter)
    Q_UNUSED(sortOrder)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageFolderIdList(); // stub
}

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    Q_UNUSED(filter)
    Q_UNUSED(sortOrder)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageAccountIdList(); // stub
}

int QMessageStore::countMessages(const QMessageFilter& filter) const
{
    Q_UNUSED(filter)
    return 0; // stub
}

int QMessageStore::countFolders(const QMessageFolderFilter& filter) const
{
    Q_UNUSED(filter)
    return 0; // stub
}

int QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
{
    Q_UNUSED(filter)
    return 0; // stub
}

bool QMessageStore::removeMessage(const QMessageId& id, QMessageManager::RemovalOption option)
{
    Q_UNUSED(id)
    Q_UNUSED(option)
    return false; // stub
}

bool QMessageStore::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option)
{
    Q_UNUSED(filter)
    Q_UNUSED(option)
    return true; // stub
}

bool QMessageStore::addMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

bool QMessageStore::updateMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    Q_UNUSED(id)
    return QMessage(); // stub
}

QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    Q_UNUSED(id)
    return QMessageFolder(); // stub
}

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    Q_UNUSED(id)
    return QMessageAccount(); // stub
}

QMessageManager::NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter)
{
    Q_UNUSED(filter)
    return 0; // stub
}

void QMessageStore::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    Q_UNUSED(notificationFilterId)
}


QTM_END_NAMESPACE
