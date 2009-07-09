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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef MessagingAPI
#define MessagingAPI
#include <QSharedDataPointer>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QDataStream>
#include <QStringList>
#include <QDateTime>
#include <QObject>
#include <QFlags>
#include "qmessageid.h"
#include "qmessagecontentcontainerid.h"
#include "qmessagefolderid.h"
#include "qmessageaccountid.h"
#include "qmessagecontentcontainer.h"
#include "qmessageaddress.h"
#include "qmessage.h"
#include "qmessagefolder.h"
#include "qmessageaccount.h"
#include "qmessagedatacomparator.h"
#include "qmessageaccountfilterkey.h"
#include "qmessageaccountsortkey.h"
#include "qmessagefolderfilterkey.h"
#include "qmessagefoldersortkey.h"
#include "qmessagefilterkey.h"
#include "qmessagesortkey.h"

class QMessageStorePrivate;

class QMessageStore : public QObject
{
    Q_OBJECT

public:
    enum RemovalOption
    {
        NoRemovalRecord = 1,
        CreateRemovalRecord
    };

    enum ErrorCode
    {
        NoError = 0,
        InvalidId, 
        ConstraintFailure,
        ContentInaccessible,
        NotYetImplemented,
        FrameworkFault,
        WorkingMemoryOverflow
    };

    QMessageStore(QObject *parent = 0);
    virtual ~QMessageStore();

    QMessageStore::ErrorCode lastError() const;

    QMessageIdList queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit = 0, uint offset = 0) const;
    QMessageFolderIdList queryFolders(const QMessageFolderFilterKey &key, const QMessageFolderSortKey &sortKey, uint limit = 0, uint offset = 0) const;
    QMessageAccountIdList queryAccounts(const QMessageAccountFilterKey &key, const QMessageAccountSortKey &sortKey, uint limit = 0, uint offset = 0) const;
    int countMessages(const QMessageFilterKey &key) const;
    int countFolders(const QMessageFolderFilterKey &key) const;
    int countAccounts(const QMessageAccountFilterKey &key) const;

    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, RemovalOption option = NoRemovalRecord);
    bool removeMessages(const QMessageFilterKey &key, RemovalOption option = NoRemovalRecord);

    QMessage message(const QMessageId &id) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    QMessageAccount account(const QMessageAccountId &id) const;

    void setMaximumWorkingMemory(uint maximumBytes);
    uint maximumWorkingMemory();
    
    static QMessageStore* instance();
    
signals:
    void messagesAdded(const QMessageIdList &ids);
    void messagesRemoved(const QMessageIdList &ids);
    void messagesUpdated(const QMessageIdList &ids);

public slots:
    void startNotifications(const QMessageFilterKey &key);
    void stopNotifications(const QMessageFilterKey &key);

private:
    QSharedDataPointer<QMessageStorePrivate> d_ptr;
};


class QMessageServiceActionPrivate;

class QMessageServiceAction : public QObject
{
    Q_OBJECT

public:
    enum Activity {
        Pending = 0,
        InProgress,
        Successful,
        Failed
    };

    QMessageServiceAction(QObject *parent = 0);
    ~QMessageServiceAction();

    void send(const QMessage &message, const QMessageAccountId &accountId);
    void compose(const QMessage &message);
    void retrieve(const QMessageId &id);
    void retrieve(const QMessageContentContainerId &id);
    void show(const QMessageId &id);
    void exportUpdates();
    Activity activity() const;
    QString lastErrorString() const;

public slots:
    void cancelOperation();

signals:
    void activityChanged(QMessageServiceAction::Activity a);

private:
    QSharedDataPointer<QMessageServiceActionPrivate> d_ptr;
};
#endif
