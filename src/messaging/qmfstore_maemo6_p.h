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
#ifndef QMFSTORE_MAEMO6_H
#define QMFSTORE_MAEMO6_H

#include <qmobilityglobal.h>
#include <qmailstore.h>
#include "qmfhelpers_maemo6_p.h"
#include "qmessagestore.h"

QTM_BEGIN_NAMESPACE

class QMFStorePrivate;

class QMFStore : public QObject
{
    Q_OBJECT

    friend class QMFStorePrivate;

public:
    QMessageManager::Error error() const;

    QMessageIdList queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset, QMessageManager::Error &error) const;
    QMessageIdList queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset, QMessageManager::Error &error) const;

    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset, QMessageManager::Error &error) const;

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset, QMessageManager::Error &error) const;

    int countMessages(const QMessageFilter &filter, QMessageManager::Error &error) const;
    int countFolders(const QMessageFolderFilter &filter, QMessageManager::Error &error) const;
    int countAccounts(const QMessageAccountFilter &filter, QMessageManager::Error &error) const;

    bool addMessage(QMessage &m, QMessageManager::Error &error);
    bool updateMessage(QMessage &m, QMessageManager::Error &error);
    bool removeMessage(const QMessageId &id, QMessageManager::RemovalOption option, QMessageManager::Error &error);
    bool removeMessages(const QMessageFilter &filter, QMessageManager::RemovalOption option, QMessageManager::Error &error);

    QMessage message(const QMessageId &id, QMessageManager::Error &error) const;
    QMessageFolder folder(const QMessageFolderId &id, QMessageManager::Error &error) const;
    QMessageAccount account(const QMessageAccountId &id, QMessageManager::Error &error) const;

    QMessageManager::NotificationFilterId registerNotificationFilter(const QMessageFilter &filter);
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId filterId);

    QByteArray getMimePart(const QMessageId &messageId, const QMessageContentContainerId &contId);
    QMessageAccountId defaultAccount() const;

    static QMFStore* instance();

signals:
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

private:
    friend class QGlobalStaticDeleter<QMFStore>;
    QMFStore(QObject *parent = 0);
    virtual ~QMFStore();

    QMFStorePrivate *d_ptr;
};


class QMFStorePrivate : public QObject
{
    Q_OBJECT

public:
    QMFStorePrivate() 
        : QObject(), 
          _store(QMailStore::instance()), 
          _notify(true),
          _filterId(0)
    {
    }

    QMailStore *_store;
    bool _notify;
    QMessageManager::NotificationFilterId _filterId;
    QMap<QMessageManager::NotificationFilterId, QMailMessageKey> _filters;

    static QMailStore *convert(QMFStore *store);

    Q_SCOPED_STATIC_DECLARE(QMFStore,storeInstance);

    static void registerMessageStatus(QMailStore *store, const QString &field);
    static void createNonexistentFolder(QMailStore *store, const QString &path, quint64 status); 

public slots:
    void messagesAdded(const QMailMessageIdList &ids);
    void messagesRemoved(const QMailMessageIdList &ids);
    void messagesUpdated(const QMailMessageIdList &ids);

signals:
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

private:
    void processFilters(const QMailMessageIdList &ids, void (QMFStorePrivate::*signal)(const QMessageId &, const QMessageManager::NotificationFilterIdSet &));
};


QTM_END_NAMESPACE

#endif

