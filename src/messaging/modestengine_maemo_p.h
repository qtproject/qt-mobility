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

#ifndef MODESTENGINE_MAEMO_H
#define MODESTENGINE_MAEMO_H

#include "qmessagemanager.h"
#include "qmessagefilter_p.h"
#include "qmessageservice.h"

#include "gconf/gconf-client.h"
#include "libosso.h"

#include <QMap>
#include <QString>
#include <QDBusArgument>
#include <QDBusPendingCallWatcher>
#include <QFileInfoList>
#include <QThread>
#include <QMutex>
#include <QEventLoop>

class QDBusInterface;
class QFileSystemWatcher;
class QEventLoop;

QTM_BEGIN_NAMESPACE

typedef QMap< QString, QString > ModestStringMap;
typedef QList< ModestStringMap > ModestStringMapList;

static const int maxCacheSize = 100000;

class QMessageService;
class QMessageServicePrivate;
class QMessageStorePrivate;

struct MessageQueryInfo
{
    int queryId;
    QString body;
    QMessageDataComparator::MatchFlags matchFlags;
    QMessageFilter filter;
    QMessageSortOrder sortOrder;
    int limit;
    int offset;
    QMessageServicePrivate* privateService;
    QDBusPendingCallWatcher* pendingCallWatcher;
    int currentFilterListIndex;
    int handledFiltersCount;
    QMessageIdList ids;
    QString realAccountId;
    bool isQuery;
    bool returnWithSingleShot;
};

struct ModestUnreadMessageDBusStruct
{
    qlonglong timeStamp;
    QString subject;
};

struct ModestAccountsUnreadMessagesDBusStruct
{
    QString accountId;
    QString accountName;
    QString accountProtocol;
    qlonglong unreadCount;
    QList<ModestUnreadMessageDBusStruct> unreadMessages;
};

struct ModestMessage
{
    QString id;
    QString subject;
    QString folder;
    QString sender;
    long long size;
    bool hasAttachment;
    bool isUnread;
    long long timeStamp;
};

typedef enum {
    MessagingModestMessageNotDefined  = 0,
    MessagingModestMessageAnswered    = 1<<0,
    MessagingModestMessageDeleted     = 1<<1,
    MessagingModestMessageDraft       = 1<<2,
    MessagingModestMessageFlagged     = 1<<3,
    MessagingModestMessageSeen        = 1<<4,
    MessagingModestMessageAttachments = 1<<5,
    MessagingModestMessageCached      = 1<<6,
    MessagingModestMessagePartial     = 1<<7,
    MessagingModestMessageExpunged    = 1<<8,
    MessagingModestMessageHasPriority = 1<<9|1<<10
} MessagingModestMessageFlags;

typedef enum {
    MessagingModestMessagePriorityNotDefined = 0,
    MessagingModestMessageHighPriority       = 1<<9|1<<10,
    MessagingModestMessageNormalPriority     = 0<<9|0<<10,
    MessagingModestMessageLowPriority        = 0<<9|1<<10,
    MessagingModestMessageSuspendedPriority  = 1<<9|0<<10
} MessagingModestMessagePriority;

struct MessagingModestMimePart
{   
    QString mimeType;
    bool isAttachment;
    QString fileName;
    QString contentId;
};

struct MessagingModestMessage
{
    QString id;
    QString url;
    QString accountId;
    QString folderId;
    QString mimeType;
    QString from;
    QString to;
    QString cc;
    QString bcc;
    QString replyTo;
    QString subject;
    qlonglong dateReceived;
    qlonglong dateSent;
    qlonglong size;
    MessagingModestMessageFlags flags;
    MessagingModestMessagePriority priority;
    QList<MessagingModestMimePart> mimeParts;
};

struct INotifyEvent
{
    int watchDescriptor;
    uint32_t mask;
    QString fileName;
};

class INotifyWatcher : public QThread
{
    Q_OBJECT

public:
    enum FileNotification
    {
        FileNotificationAdded,
        FileNotificationUpdated,
        FileNotificationRemoved
    };

    INotifyWatcher();
    ~INotifyWatcher();

    void run();
    int addFile(const QString& path, uint eventsToWatch = 0);
    QStringList files() const;
    int addDirectory(const QString& path, uint eventsToWatch = 0);
    QStringList directories() const;
    void clear();

private slots:
    void notifySlot();

signals:
   void fileChanged(int watchDescriptor, QString filePath, uint events);

private: //Data
    int m_inotifyFileDescriptor;
    QMutex m_mutex;
    QMap<int, QString> m_files;
    QMap<int, QString> m_dirs;
};

class ModestEngine : public QObject
{
    Q_OBJECT

public:
    enum EmailProtocol
    {
        EmailProtocolUnknown = -1,
        EmailProtocolPop3 = 1,
        EmailProtocolIMAP,
    };

    enum NotificationType
    {
        None = 0,
        Added,
        Updated,
        Removed
    };

    static ModestEngine* instance();

    ModestEngine();
    ~ModestEngine();

    bool exportUpdates(const QMessageAccountId &id);

    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                        uint limit, uint offset, bool &isFiltered, bool &isSorted) const;
    int countAccounts(const QMessageAccountFilter &filter) const;
    QMessageAccount account(const QMessageAccountId &id) const;
    QMessageAccountId defaultAccount() const;

    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder,
                                      uint limit, uint offset, bool &isFiltered, bool &isSorted) const;
    int countFolders(const QMessageFolderFilter &filter) const;
    QMessageFolder folder(const QMessageFolderId &id) const;

    QMessage message(const QMessageId &id, bool useCache = true) const;
    bool addMessage(QMessage &message);
    bool updateMessage(QMessage &message);
    bool removeMessage(const QMessageId &id, QMessageManager::RemovalOption option);

    bool queryMessages(QMessageService& messageService, const QMessageFilter &filter,
                       const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool queryMessages(QMessageService& messageService, const QMessageFilter &filter,
                       const QString &body, QMessageDataComparator::MatchFlags matchFlags,
                       const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool countMessages(QMessageService& messageService, const QMessageFilter &filter);

    QMessageIdList queryMessagesSync(const QMessageFilter &filter, const QMessageSortOrder &sortOrder,
                                     uint limit, uint offset, bool &isFiltered, bool &isSorted) const;
    QMessageIdList queryMessagesSync(const QMessageFilter &filter, const QString &body,
                                     QMessageDataComparator::MatchFlags matchFlags,
                                     const QMessageSortOrder &sortOrder, uint limit, uint offset,
                                     bool &isFiltered, bool &isSorted) const;
    int countMessagesSync(const QMessageFilter &filter) const;

    bool sendEmail(QMessage &message);
    bool composeEmail(const QMessage &message);
    bool showMessage(const QMessageId &id);

    QMessageManager::NotificationFilterId registerNotificationFilter(QMessageStorePrivate& messageStore,
                                                                     const QMessageFilter& filter,
                                                                     QMessageManager::NotificationFilterId id = 0);
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId);
    QByteArray getMimePart (const QMessageId &id, const QString &attachmentId);

    void clearHeaderCache();

private:
    QFileInfoList localFolders() const;
    void appendLocalSubFolders(QFileInfoList& fileInfoList, int startIndex) const;
    void appendIMAPSubFolders(QFileInfoList& fileInfoList, int startIndex) const;
    QFileInfoList accountFolders(QMessageAccountId& accountId) const;
    QString localRootFolder() const;
    QString accountRootFolder(QMessageAccountId& accountId) const;
    EmailProtocol accountEmailProtocol(QMessageAccountId& accountId) const;
    QString accountEmailProtocolAsString(const QMessageAccountId& accountId) const;
    QString accountUsername(QMessageAccountId& accountId) const;
    QString accountHostname(QMessageAccountId& accountId) const;

    void updateEmailAccounts() const;

    bool filterMessage(const QMessage& message, QMessageFilterPrivate::SortedMessageFilterList filterList, int start) const;
    bool queryAndFilterMessages(MessageQueryInfo &msgQueryInfo) const;
    bool startQueryingAndFilteringMessages(MessageQueryInfo &msgQueryInfo) const;
    bool searchMessages(MessageQueryInfo &msgQueryInfo, const QStringList& accountIds,
                        const QStringList& folderUris, const QString& body,
                        const QDateTime& startTimeStamp, const QDateTime& endTimeStamp,
                        const QDateTime& startReceptionTimeStamp, const QDateTime& endReceptionTimeStamp) const;
    void searchNewMessages(const QString& searchString, const QString& folderToSearch,
                           const QDateTime& startDate, const QDateTime& endDate,
                           int searchflags, uint minimumMessageSize) const;
    void handleQueryFinished(int queryIndex) const;

    void watchAllKnownEmailFolders();
    void notification(const QMessageId& messageId, NotificationType notificationType) const;

    QMessageAccountId accountIdFromModestMessageId(const QString& modestMessageId) const;
    QMessageFolderId folderIdFromModestMessageId(const QString& modestMessageId,
                                                 const QMessageAccountId accountId = QMessageAccountId()) const;

    MessagingModestMessage messageFromModest(const QString& accountId,
                                             const QString &folderId,
                                             const QString& messageId) const;

    QString modestAccountIdFromAccountId(const QMessageAccountId& accountId) const;
    QString modestFolderIdFromFolderId(const QMessageFolderId& folderId) const;
    QString modestFolderUriFromFolderId(const QMessageFolderId& folderId) const;
    QString modestAccountIdFromMessageId(const QMessageId& messageId, bool checkProtocol = true) const;
    QString modestAccountIdFromFolderId(const QMessageFolderId& folderId, bool checkProtocol = true) const;
    QString modestFolderIdFromMessageId(const QMessageId& messageId) const;
    QString modestMessageIdFromMessageId(const QMessageId& messageId) const;
    void replaceProtocol(QString& id, const QString& newProtocol) const;
    QMessageAccountId realAccountId(const MessagingModestMessage& modestMessage) const;
    QMessageAccountId accountIdFromMessageId(const QMessageId& messageId) const;
    QMessageAccountId accountIdFromFolderId(const QMessageFolderId& folderId) const;
    QMessageAccountId accountIdFromModestAccountId(const QString& accountId) const;
    QMessageFolderId folderIdFromModestFolderId(const QMessageAccountId& accountId,
                                                bool isLocalFolder,
                                                const QString& folderId) const;
    QMessageId messageIdFromModestMessageId(const QString& messageId) const;
    QMessageId messageIdFromModestMessageFilePath(const QString& messageFilePath) const;

    QMessage messageFromModestMessage(const MessagingModestMessage& modestMessage,
                                      QMessageAccountId accountId = QMessageAccountId()) const;
    void appendAttachmentToMessage(QMessage& message, QMessageContentContainer& attachment) const;

    static QString unescapeString(const QString& string);
    static QString escapeString(const QString& string);

    QMessage::StandardFolder standardFolderFromModestFolderId(const QString& modestFolderId) const;
    QString modestFolderIdFromStandardFolder(QMessage::StandardFolder standardFolder) const;

    ModestStringMap getModestSenderInfo(QMessage &message);
    ModestStringMap getModestRecipients(QMessage &message);
    ModestStringMap getModestMessageData(QMessage &message);
    ModestStringMapList getModestAttachments(QMessage &message);
    ModestStringMapList getModestImages(QMessage &message);
    uint getModestPriority(QMessage &message);
    ModestStringMap getModestHeaders(QMessage &message);

private slots:
    void searchMessagesHeadersReceivedSlot(QDBusMessage msg);
    void searchMessagesHeadersFetchedSlot(QDBusMessage msg);
    void folderUpdatedSlot(QDBusMessage msg);
    void messageReadChangedSlot(QDBusMessage msg);
    void pendingGetUnreadMessagesFinishedSlot(QDBusPendingCallWatcher* pendingCallWatcher);
    void pendingSearchFinishedSlot(QDBusPendingCallWatcher* pendingCallWatcher);
    void fileChangedSlot(int watchDescriptor, QString filePath, uint events);
    void sendEmailCallEnded(QDBusPendingCallWatcher *watcher);
    void addMessageCallEnded(QDBusPendingCallWatcher *watcher);
    void stateChanged(QMessageService::State newState);
    void returnQueryResultsSlot();

private: //Data
    GConfClient *m_gconfclient;

    QDBusInterface *m_ModestDBusInterface;
    QDBusInterface *m_QtmPluginDBusInterface;

    INotifyWatcher m_MailFoldersWatcher;

    mutable QHash<QString, QMessageAccount> iAccounts;
    mutable QMessageAccountId iDefaultEmailAccountId;

    mutable int m_queryIds;
    mutable QList<MessageQueryInfo> m_pendingMessageQueries;

    QMap<QMessageManager::NotificationFilterId, QMessageFilter> m_filters;
    QMessageManager::NotificationFilterId m_filterId;
    QMessageStorePrivate* m_messageStore;

    QMap<QString, QDateTime> accountsLatestTimestamp;

    mutable QStringList m_latestAddOrRemoveNotifications;

    mutable QMap<QString, QMessage> m_messageCache;

    // Following variables are used for sync queries
    mutable QMessageService m_service;
    mutable QEventLoop      m_eventLoop;
    mutable QMessageIdList  m_ids;
    mutable int             m_count;
    mutable bool            m_isSorted;
    mutable bool            m_isFiltered;
};

QTM_END_NAMESPACE

// Marshall the MyStructure data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const QtMobility::ModestStringMap &map);

// Retrieve the MyStructure data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, QtMobility::ModestStringMap &map);

Q_DECLARE_METATYPE(QtMobility::ModestStringMap);
Q_DECLARE_METATYPE(QtMobility::ModestStringMapList);
Q_DECLARE_METATYPE(QtMobility::INotifyWatcher::FileNotification);
Q_DECLARE_METATYPE(QtMobility::ModestUnreadMessageDBusStruct);
Q_DECLARE_METATYPE(QtMobility::ModestAccountsUnreadMessagesDBusStruct);
Q_DECLARE_METATYPE(QtMobility::ModestMessage);
Q_DECLARE_METATYPE(QtMobility::MessagingModestMimePart);

#endif // MODESTENGINE_MAEMO_H

