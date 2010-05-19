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

#ifndef QFSENGINE_SYMBIAN_H
#define QFSENGINE_SYMBIAN_H

#include <qmessageglobal.h>
#include <QObject>
#include <QMap>
#include <QSet>
#include <QList>
#include <QHash>

#include <e32base.h>

#include "qmessagemanager.h"
#include "qmessagestore_symbian_p.h"
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "qmessagefolderfilter.h"
#include "qmessageservice.h"
#include <emailinterfacefactory.h>
#include <memailclientapi.h>
#include <memailmessagesearch.h>
#include <memailmessage.h>
#include <emailinterfacefactory.h>
#include <emailsorting.h>
#include <memailmessagesearch.h>
#include <memailfolder.h>
#include <mmailboxsyncobserver.h>
#ifdef FREESTYLEMAILBOXOBSERVERUSED
#include <mmailboxcontentobserver.h>
#endif

using namespace EmailInterface;

QTM_BEGIN_NAMESPACE

class CFSMessagesFindOperation;
class QMessageId;
class QMessageAccount;

struct FSMessageQueryInfo
{
    int operationId;
    bool isQuery;
    QString body;
    QMessageDataComparator::MatchFlags matchFlags;
    QMessageFilter filter;
    QMessageSortOrder sortOrder;
    int offset;
    int limit;
    QMessageServicePrivate* privateService;
    CFSMessagesFindOperation* findOperation;
    int currentFilterListIndex;
    QMessageIdList ids;
    int count;
};

struct FSSearchOperation
{
    MEmailMailbox* m_mailbox;
    MEmailMessageSearchAsync* m_search;
};

#ifdef FREESTYLEMAILBOXOBSERVERUSED
class CFSEngine : public MMailboxContentObserver, public MMailboxSyncObserver
#else
class CFSEngine : public MMailboxSyncObserver
#endif
{
public: 
    
    static CFSEngine* instance();

    CFSEngine();
    ~CFSEngine();
    
    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const;
    int countAccounts(const QMessageAccountFilter &filter) const;
    QMessageAccount account(const QMessageAccountId &id) const;
    QMessageAccountId defaultAccount(QMessage::Type type) const;
    
    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const;
    int countFolders(const QMessageFolderFilter &filter) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    
    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, QMessageManager::RemovalOption option);
    bool removeMessages(const QMessageFilter &filter, QMessageManager::RemovalOption option);
    bool queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    bool countMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter);
    bool showMessage(const QMessageId &id);
    bool composeMessage(const QMessage &message);   
    QMessage message(const QMessageId& id) const;
    bool sendEmail(QMessage &message);
   
    bool retrieve(const QMessageId &messageId, const QMessageContentContainerId& id);
    bool retrieveBody(const QMessageId& id);
    bool retrieveHeader(const QMessageId& id);
    bool exportUpdates(const QMessageAccountId &id);
    
    QByteArray attachmentContent(long int messageId, unsigned int attachmentId);
    QString attachmentTextContent(long int messageId, unsigned int attachmentId, const QByteArray &charset);
    
    QMessageManager::NotificationFilterId registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
                                        const QMessageFilter& filter);
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId);

    void filterAndOrderMessagesReady(bool success, int operationId, QMessageIdList ids, int numberOfHandledFilters,
                                     bool resultSetOrdered);
    
    void setMtmAccountIdList(QMessageAccountIdList accountList);
    
public: // from MMailboxSyncObserver
    void MailboxSynchronisedL(TInt aResult);
    
#ifdef FREESTYLEMAILBOXOBSERVERUSED
    void setPluginObserversL();
    
public:
        // from MMailboxContentObserver
    void NewMessageEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aNewMessages, const TFolderId& aParentFolderId);
    void MessageChangedEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aChangedMessages, const TFolderId& aParentFolderId);
    void MessageDeletedEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aDeletedMessages, const TFolderId& aParentFolderId);       
#endif
        
private:

    void queryMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    void queryMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const;
    void countMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter);
    
    QMessageAccountIdList accountsByType(QMessage::Type type) const;
    void updateEmailAccountsL() const;
    void updateMessageL(QMessage* message);
    MEmailMessage* createFSMessageL(const QMessage &message, const MEmailMailbox* mailbox);
    QMessageFolderIdList folderIdsByAccountId(const QMessageAccountId& accountId) const;
    QMessageFolderIdList folderIdsByAccountIdL(const QMessageAccountId& accountId) const;
    QMessageFolderIdList filterMessageFoldersL(const QMessageFolderFilter& filter, bool& filterHandled) const;
    QMessageFolderIdList allFolders() const;
    QMessageFolder folderL(const QMessageFolderId &id) const;
    bool fsFolderL(const QMessageFolderId& id, MEmailMailbox* mailbox, MEmailFolder* folder) const;
    TFolderType standardFolderId(QMessage::StandardFolder standardFolder);
    
    QMessageFolderIdList filterMessageFolders(const QMessageFolderFilter& filter, bool& filterHandled) const;
    QMessage CreateQMessageL(MEmailMessage* aMessage) const; 
    void AddContentToMessage(MEmailMessageContent* aContent, QMessage* aMessage) const;
    QDateTime symbianTTimetoQDateTime(const TTime& time) const;
    TTime qDateTimeToSymbianTTime(const QDateTime& date) const;
    
    QMessage messageL(const QMessageId& id) const;
    static bool accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2);
    void orderAccounts(QMessageAccountIdList& accountIds,  const QMessageAccountSortOrder &sortOrder) const;
    void applyOffsetAndLimitToAccountIds(QMessageAccountIdList& idList, int offset, int limit) const;
    
    static bool folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2);
    void orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderSortOrder &sortOrder) const;
    void applyOffsetAndLimitToMsgFolderIds(QMessageFolderIdList& idList, int offset, int limit) const;
    void handleNestedFiltersFromFolderFilter(QMessageFolderFilter &filter) const;

    static bool messageLessThan(const QMessage& message1, const QMessage& message2);
    void orderMessages(QMessageIdList& messageIds,  const QMessageSortOrder &sortOrder) const;
    void applyOffsetAndLimitToMsgIds(QMessageIdList& idList, int offset, int limit) const;

    void handleNestedFiltersFromMessageFilter(QMessageFilter &filter) const;
    void exportUpdatesL(const QMessageAccountId &id);
    
    friend class QMessageService;
    friend class CMessagesFindOperation;
    
private:

    CEmailInterfaceFactory* m_factory;
    mutable MEmailInterface* m_ifPtr;
    mutable MEmailClientApi* m_clientApi;
    mutable QHash<QString, QMessageAccount> m_accounts;
    mutable int m_operationIds;
    mutable QList<FSMessageQueryInfo> m_messageQueries;
    QMessageAccountIdList m_mtmAccountList;
    TMailboxId m_mailboxId;
    QMessageStorePrivate* ipMessageStorePrivate;
    bool iListenForNotifications;
    QMessageManager::NotificationFilterId m_filterId;
    QMap<QMessageManager::NotificationFilterId, QMessageFilter> m_filters;
    QMessageAccount m_account;
    friend class QMessageService;
    friend class CFSMessagesFindOperation;
    
    mutable QMessageAccountSortOrder m_currentAccountOrdering;
    mutable QMessageFolderSortOrder m_currentFolderOrdering;
    mutable QMessageSortOrder m_currentMessageOrdering;


};
    
class CFetchOperationWait : public CBase, public MEmailFetchObserver
    {
public:
    static CFetchOperationWait* NewLC();
    virtual ~CFetchOperationWait();        
    void Wait();
    
public: // From MEmailFetchObserver
    virtual void DataFetchedL(const TInt aResult);
    
private:
    void ConstructL();
    CFetchOperationWait();
    
private:               
    CActiveSchedulerWait* iWait;
};

class CFSMessagesFindOperation : public QObject, MEmailSearchObserver
{
    Q_OBJECT
    
public:
    CFSMessagesFindOperation(CFSEngine& aOwner, int aOperationId); 
    ~CFSMessagesFindOperation();

    void filterAndOrderMessages(const QMessageFilter& filter,
                                const QMessageSortOrder& sortOrder,
                                const QString body = QString(),
                                QMessageDataComparator::MatchFlags matchFlags = 0);
    void filterAndOrderMessages(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                const QMessageSortOrder& sortOrder,
                                const QString body = QString(),
                                QMessageDataComparator::MatchFlags matchFlags = 0);

    enum Field {None = 0, Sender, Recipients, Subject, Body};

    
public slots:
    void SearchCompleted();

private:
    // from memailmessagesearch
    virtual void HandleResultL(MEmailMessage* aMessage);
    virtual void SearchCompletedL();

    void filterAndOrderMessagesL(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                const QMessageSortOrder& sortOrder,
                                const QString body = QString(),
                                QMessageDataComparator::MatchFlags matchFlags = 0);
    
    void getAllMessagesL(TEmailSortCriteria& sortCriteria);
    void getAccountSpecificMessagesL(QMessageAccount& messageAccount, TEmailSortCriteria& sortCriteria);
    void getFolderSpecificMessagesL(QMessageFolder& messageFolder, TEmailSortCriteria sortCriteria);
    
    bool fillsSearchKeyCriteria(QMessageId& messageId);

    
private: // Data
    CFSEngine& m_owner;
    
    int m_numberOfHandledFilters;
    int m_operationId;
    int m_activeSearchCount;
    //TMsvSelectionOrdering iOrdering;
    bool m_resultCorrectlyOrdered;
    QMessageIdList m_idList;
    QMessageFilterPrivate::SortedMessageFilterList m_filterList;
    QMessageIdList m_excludeIdList;

    MEmailClientApi* m_clientApi;
    RMailboxPtrArray m_mailboxes;
    
    CEmailInterfaceFactory* m_factory; 
    MEmailInterface* m_interfacePtr; 
    bool m_receiveNewMessages;
    QList<FSSearchOperation> m_searchOperations;
    
    Field m_searchField;
    QMessageDataComparator::MatchFlags m_matchFlags;
    QString m_searchKey;
};


QTM_END_NAMESPACE

#endif // QFSENGINE_SYMBIAN_H
