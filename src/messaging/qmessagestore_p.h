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
#ifndef QMESSAGESTORE_H
#define QMESSAGESTORE_H
#include <QObject>
#include <QSet>
#include <QMetaType>

#include <qmessageglobal.h>
#include <qmessagefilter.h>
#include <qmessagesortorder.h>
#include <qmessagefoldersortorder.h>
#include <qmessageaccountsortorder.h>
#include <qmessage.h>
#include <qmessagefolder.h>
#include <qmessageaccount.h>
#include <qmessagemanager.h>


QTM_BEGIN_NAMESPACE

class QMessageStorePrivate;

class QMessageStore : public QObject
{
    Q_OBJECT

    friend class QMessageStorePrivate;

public:
    QMessageManager::Error error() const;

    QMessageIdList queryMessages(const QMessageFilter &filter = QMessageFilter(), const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0) const;
    QMessageIdList queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags = 0, const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0) const;

    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter = QMessageFolderFilter(), const QMessageFolderSortOrder &sortOrder = QMessageFolderSortOrder(), uint limit = 0, uint offset = 0) const;

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter(), const QMessageAccountSortOrder &sortOrder = QMessageAccountSortOrder(), uint limit = 0, uint offset = 0) const;

    int countMessages(const QMessageFilter &filter = QMessageFilter()) const;
    int countFolders(const QMessageFolderFilter &filter = QMessageFolderFilter()) const;
    int countAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter()) const;

    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, QMessageManager::RemovalOption option = QMessageManager::RemoveOnOriginatingServer);
    bool removeMessages(const QMessageFilter &filter, QMessageManager::RemovalOption option = QMessageManager::RemoveOnOriginatingServer);

    bool removeAccount(const QMessageAccountId &id);

    QMessage message(const QMessageId &id) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    QMessageAccount account(const QMessageAccountId &id) const;

    QMessageManager::NotificationFilterId registerNotificationFilter(const QMessageFilter &filter);
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId filterId);

    static QMessageStore* instance();

Q_SIGNALS:
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

    void accountRemoved(const QMessageAccountId &id);
private:
    friend class QGlobalStaticDeleter<QMessageStore>;
    QMessageStore(QObject *parent = 0);
    virtual ~QMessageStore();

    QMessageStorePrivate *d_ptr;
};
QTM_END_NAMESPACE

#endif
