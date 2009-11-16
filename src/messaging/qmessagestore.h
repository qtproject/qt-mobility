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
#ifndef QMESSAGESTORE_H
#define QMESSAGESTORE_H
#include <QObject>
#include <QSet>
#include <QMetaType>

#include <qmessageglobal.h>
#include <qmessagefilter.h>
#include <qmessageordering.h>
#include <qmessagefolderordering.h>
#include <qmessageaccountordering.h>
#include <qmessage.h>
#include <qmessagefolder.h>
#include <qmessageaccount.h>

class QMessageStorePrivate;

class Q_MESSAGING_EXPORT QMessageStore : public QObject
{
    Q_OBJECT

    friend class QMessageStorePrivate;

public:
    enum RemovalOption
    {
        RemoveLocalCopyOnly = 1,
        RemoveOnOriginatingServer
    };

    enum ErrorCode
    {
        NoError = 0,
        InvalidId,
        ConstraintFailure,
        ContentInaccessible,
        NotYetImplemented,
        FrameworkFault,
        WorkingMemoryOverflow,
        Busy
    };

    typedef int NotificationFilterId;
    typedef QSet<QMessageStore::NotificationFilterId> NotificationFilterIdSet;

    QMessageStore::ErrorCode lastError() const;

    QMessageIdList queryMessages(const QMessageFilter &filter = QMessageFilter(), const QMessageOrdering &ordering = QMessageOrdering(), uint limit = 0, uint offset = 0) const;
    QMessageIdList queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options = 0, const QMessageOrdering &ordering = QMessageOrdering(), uint limit = 0, uint offset = 0) const;

    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter = QMessageFolderFilter(), const QMessageFolderOrdering &ordering = QMessageFolderOrdering(), uint limit = 0, uint offset = 0) const;

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter(), const QMessageAccountOrdering &ordering = QMessageAccountOrdering(), uint limit = 0, uint offset = 0) const;

    int countMessages(const QMessageFilter &filter = QMessageFilter()) const;
    int countFolders(const QMessageFolderFilter &filter = QMessageFolderFilter()) const;
    int countAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter()) const;

    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, RemovalOption option = RemoveOnOriginatingServer);
    bool removeMessages(const QMessageFilter &filter, RemovalOption option = RemoveOnOriginatingServer);

    QMessage message(const QMessageId &id) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    QMessageAccount account(const QMessageAccountId &id) const;

    NotificationFilterId registerNotificationFilter(const QMessageFilter &filter);
    void unregisterNotificationFilter(NotificationFilterId filterId);

    static QMessageStore* instance();
    
signals:
    void messageAdded(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);

private:
    friend class QGlobalStaticDeleter<QMessageStore>;
    QMessageStore(QObject *parent = 0);
    virtual ~QMessageStore();

    QMessageStorePrivate *d_ptr;
};

Q_DECLARE_METATYPE(QMessageStore::NotificationFilterId)
Q_DECLARE_METATYPE(QMessageStore::NotificationFilterIdSet)

#endif
