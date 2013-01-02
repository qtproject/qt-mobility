/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QMESSAGESTOREPRIVATE_SYMBIAN_H
#define QMESSAGESTOREPRIVATE_SYMBIAN_H

#include "qmessagestore_p.h"
#include "qmessageservice.h"
#include <QEventLoop>


QTM_BEGIN_NAMESPACE

class CMTMEngine;
#ifdef FREESTYLEMAILUSED
class CFSEngine;
#endif

class QMessageStorePrivate : public QObject
{
    Q_OBJECT

public:
    enum NotificationType
    {
        Added,
        Updated,
        Removed
    };

    QMessageStorePrivate();
    ~QMessageStorePrivate();
    
    void initialize(QMessageStore *store);
    
    QMessageIdList queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    QMessageIdList queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    int countMessages(const QMessageFilter& filter) const;
    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter(), const QMessageAccountSortOrder &sortOrder = QMessageAccountSortOrder(), uint limit = 0, uint offset = 0) const;
    int countAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter()) const;
    QMessageAccount account(const QMessageAccountId &id) const;

    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter = QMessageFolderFilter(), const QMessageFolderSortOrder &sortOrder = QMessageFolderSortOrder(), uint limit = 0, uint offset = 0) const;
    int countFolders(const QMessageFolderFilter &filter = QMessageFolderFilter()) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    
    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, QMessageManager::RemovalOption option);
    bool removeMessages(const QMessageFilter &filter, QMessageManager::RemovalOption option);

    bool removeAccount(const QMessageAccountId &id);
    void removeAccountComplete(int error);
    void accountRemoved(const QMessageAccountId &id);
    
    QMessage message(const QMessageId& id) const;
    
    QMessageManager::NotificationFilterId registerNotificationFilter(const QMessageFilter &filter);
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId);

    void messageNotification(QMessageStorePrivate::NotificationType type, const QMessageId& id,
                             const QMessageManager::NotificationFilterIdSet &matchingFilters);

public Q_SLOTS:   
    void stateChanged(QMessageService::State a);
    void messagesFound(const QMessageIdList &ids);
    void messagesCounted(int count);

private:    
    QMessageStore* q_ptr;

    CMTMEngine* _mtmEngine;

    QMessageManager::Error _error;
    
    NotificationType _notificationType;
    mutable QEventLoop loop;
    mutable QMessageIdList _ids;
    mutable int _count;
    
    friend class QMessageStore;
};


QTM_END_NAMESPACE
#endif // QMESSAGESTOREPRIVATE_SYMBIAN_H
