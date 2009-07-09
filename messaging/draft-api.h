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

namespace QMessageDataComparator {

    enum EqualityComparator
    {
        Equal,
        NotEqual
    };

    enum InclusionComparator 
    {
        Includes,
        Excludes
    };

    enum RelationComparator
    {
        LessThan,
        LessThanEqual,
        GreaterThan,
        GreaterThanEqual
    };

    enum Option
    {
        FullWord        = 0x1,
        CaseSensitive   = 0x2
    };
    Q_DECLARE_FLAGS(Options, Option)

}
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessageDataComparator::Options)


class QMessageAccountFilterKeyPrivate;

class QMessageAccountFilterKey
{
public:

    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options();

    QMessageAccountFilterKey();
    bool isEmpty() const;

    QMessageAccountFilterKey operator~() const;
    QMessageAccountFilterKey operator&(const QMessageAccountFilterKey &other) const;
    QMessageAccountFilterKey operator|(const QMessageAccountFilterKey &other) const;
    const QMessageAccountFilterKey& operator&=(const QMessageAccountFilterKey &other);
    const QMessageAccountFilterKey& operator|=(const QMessageAccountFilterKey &other);

    bool operator==(const QMessageAccountFilterKey &other) const;
    const QMessageAccountFilterKey& operator=(const QMessageAccountFilterKey &other);

    static QMessageAccountFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageAccountFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageAccountFilterKey id(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageAccountFilterKey fromAddress(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageAccountFilterKey fromAddress(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageAccountFilterKey name(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageAccountFilterKey name(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    QSharedDataPointer<QMessageAccountFilterKeyPrivate> d_ptr;
};


class QMessageAccountSortKeyPrivate;

class QMessageAccountSortKey {
public:
    QMessageAccountSortKey();
    bool isEmpty() const;

    bool operator==(const QMessageAccountSortKey &other) const;
    const QMessageAccountSortKey& operator=(const QMessageAccountSortKey &other);

    static QMessageAccountSortKey name(Qt::SortOrder order = Qt::AscendingOrder);

private:
    QSharedDataPointer<QMessageAccountSortKeyPrivate> d_ptr;
};


class QMessageFolderFilterKeyPrivate;

class QMessageFolderFilterKey
{
public:

    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options();

    QMessageFolderFilterKey();
    bool isEmpty() const;

    QMessageFolderFilterKey operator~() const;
    QMessageFolderFilterKey operator&(const QMessageFolderFilterKey &other) const;
    QMessageFolderFilterKey operator|(const QMessageFolderFilterKey &other) const;
    const QMessageFolderFilterKey& operator&=(const QMessageFolderFilterKey &other);
    const QMessageFolderFilterKey& operator|=(const QMessageFolderFilterKey &other);

    bool operator==(const QMessageFolderFilterKey &other) const;
    const QMessageFolderFilterKey& operator=(const QMessageFolderFilterKey &other);

    static QMessageFolderFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey id(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey displayName(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilterKey displayName(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey path(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilterKey path(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    QSharedDataPointer<QMessageFolderFilterKeyPrivate> d_ptr;
};


class QMessageFolderSortKeyPrivate;

class QMessageFolderSortKey {
public:
    QMessageFolderSortKey();
    bool isEmpty() const;

    QMessageFolderSortKey operator+(const QMessageFolderSortKey &other) const;
    QMessageFolderSortKey& operator+=(const QMessageFolderSortKey &other);

    bool operator==(const QMessageFolderSortKey &other) const;
    const QMessageFolderSortKey& operator=(const QMessageFolderSortKey &other);

    static QMessageFolderSortKey displayName(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageFolderSortKey path(Qt::SortOrder order = Qt::AscendingOrder);

private:
    QSharedDataPointer<QMessageFolderSortKeyPrivate> d_ptr;
};


class QMessageFilterKeyPrivate;

class QMessageFilterKey
{
public:

    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options();

    QMessageFilterKey();
    bool isEmpty() const;

    QMessageFilterKey operator~() const;
    QMessageFilterKey operator&(const QMessageFilterKey &other) const;
    QMessageFilterKey operator|(const QMessageFilterKey &other) const;
    const QMessageFilterKey& operator&=(const QMessageFilterKey &other);
    const QMessageFilterKey& operator|=(const QMessageFilterKey &other);

    bool operator==(const QMessageFilterKey &other) const;
    const QMessageFilterKey& operator=(const QMessageFilterKey &other);

    static QMessageFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp);

    static QMessageFilterKey sender(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey sender(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey subject(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey subject(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey body(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey body(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey priority(QMessage::Priority priority, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);

    static QMessageFilterKey size(int value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey size(int value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    QSharedDataPointer<QMessageFilterKeyPrivate> d_ptr;
};


class QMessageSortKeyPrivate;

class QMessageSortKey {
public:
    QMessageSortKey();
    bool isEmpty() const;

    QMessageSortKey operator+(const QMessageSortKey &other) const;
    QMessageSortKey& operator+=(const QMessageSortKey &other);

    bool operator==(const QMessageSortKey &other) const;
    const QMessageSortKey& operator=(const QMessageSortKey &other);

    static QMessageSortKey type(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey sender(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey recipients(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey subject(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey timeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey receptionTimeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey status(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey priority(Qt::SortOrder order = Qt::AscendingOrder);

    static QMessageSortKey size(Qt::SortOrder order = Qt::AscendingOrder);

private:
    QSharedDataPointer<QMessageSortKeyPrivate> d_ptr;
};


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
