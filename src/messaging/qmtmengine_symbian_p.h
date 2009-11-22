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

#ifndef QMTMENGINE_SYMBIAN_H
#define QMTMENGINE_SYMBIAN_H

#include <QMap>
#include <QSet>
#include <QList>
#include <QHash>

#include <e32base.h>
#include <msvapi.h> // MMsvSessionObserver
#include "qmessagestore.h"
#include "qmessagestore_symbian_p.h"
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "qmessagefolderfilter.h"
#include "QMessageServiceAction.h"

class CRichText;
class CCharFormatLayer;
class CParaFormatLayer;
class CMsvSession;
class CClientMtmRegistry;
class CSmsClientMtm;
class CMmsClientMtm;
class CSmtpClientMtm;
class CImap4ClientMtm;
class CPop3ClientMtm;
class CMsvFindOperation;
class QMessageId;
class QMessageAccount;
class CMessagesFindOperation;

struct MessageEvent
{
    QMessageStorePrivate::NotificationType notificationType;
    TMsvId messageId;
    QMessageStore::NotificationFilterIdSet matchingFilters;
};

struct MessageQueryInfo
{
    int operationId;
    bool isQuery;
    QString body;
    QMessageDataComparator::Options options;
    QMessageFilter filter;
    QMessageOrdering ordering;
    int offset;
    int limit;
    QMessageServiceActionPrivate* privateAction;
    CMessagesFindOperation* findOperation;
    int currentFilterListIndex;
    QMessageIdList ids;
    int count;
};

class CMTMEngine : public CActive, public MMsvSessionObserver
{
public:
    enum MTMType
    {
        MTMTypeSMS,
        MTMTypeMMS,
        MTMTypeSMTP,
        MTMTypeIMAP,
        MTMTypePOP3
    };    
    
    static CMTMEngine* instance();

    CMTMEngine();
    ~CMTMEngine();
    
    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountOrdering &ordering, uint limit, uint offset) const;
    int countAccounts(const QMessageAccountFilter &filter) const;
    QMessageAccount account(const QMessageAccountId &id) const;
    QMessageAccountId defaultAccount(QMessage::Type type) const;
    
    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QMessageFolderOrdering &ordering, uint limit, uint offset) const;
    int countFolders(const QMessageFolderFilter &filter) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    
    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, QMessageStore::RemovalOption option);
    bool removeMessages(const QMessageFilter &filter, QMessageStore::RemovalOption option);
    bool queryMessages(QMessageServiceActionPrivate& privateAction, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const;
    bool queryMessages(QMessageServiceActionPrivate& privateAction, const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const;
    bool countMessages(QMessageServiceActionPrivate& privateAction, const QMessageFilter &filter);
    bool showMessage(const QMessageId &id);
    bool composeMessage(const QMessage &message);
    
    QMessage message(const QMessageId& id) const;
    
    bool storeMMS(QMessage &message, TMsvId dest);
    void sendMMS();
    bool storeEmail(QMessage &message, TMsvId dest);
    void sendEmail(QMessage &message);
    bool storeSMS(QMessage &message, TMsvId dest);
    bool sendSMS(QMessage &message);
    bool retrieve(const QMessageId &messageId, const QMessageContentContainerId& id);
    bool retrieveBody(const QMessageId& id);
    bool retrieveHeader(const QMessageId& id);
    
    QMessageStore::NotificationFilterId registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
                                        const QMessageFilter& filter);
    void unregisterNotificationFilter(QMessageStore::NotificationFilterId notificationFilterId);

    void notification(TMsvSessionEvent aEvent, TUid aMsgType, TMsvId aFolderId, TMsvId aMessageId);
    void filterAndOrderMessagesReady(bool success, int operationId, QMessageIdList ids, int numberOfHandledFilters,
                                     bool resultSetOrdered);

private:
    void updateEmailAccountsL() const;
    bool switchToMTMRootEntry(MTMType aMTMType);
    TMsvId mtmServiceEntryIdL(MTMType aMTMType) const;
    CBaseMtm* mtmByType(MTMType aMTMType);
    CBaseMtm* mtmByUid(TUid aMTMUid);
    TUid mtmUidByType(MTMType aMTMType);
    TMsvId standardFolderId(QMessage::StandardFolder standardFolder);
    
    QMessageFolderIdList allFolders() const;
    QMessageFolderIdList folderIdsByAccountId(const QMessageAccountId& accountId) const;
    QMessageFolderIdList folderIdsByServiceEntryId(const TMsvId& serviceEntryId) const;
    QMessageFolderId createQMessageFolderId(const TMsvId& serviceEntryId, const TMsvId& folderId) const;
    TMsvId serviceEntryIdFromQMessageFolderId(const QMessageFolderId& folderId) const;
    TMsvId folderIdFromQMessageFolderId(const QMessageFolderId& folderId) const;
    
    QMessageAccountIdList accountsByType(QMessage::Type type) const;
    QMessageAccountId accountIdByServiceId(TMsvId serviceId) const;
    
    QMessageFolder folderL(const QMessageFolderId &id) const;

    static bool accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2);
    void orderAccounts(QMessageAccountIdList& accountIds,  const QMessageAccountOrdering &ordering) const;
    void applyOffsetAndLimitToAccountIds(QMessageAccountIdList& idList, int offset, int limit) const;
    static bool folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2);
    void orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderOrdering &ordering) const;
    static bool messageLessThan(const QMessage& message1, const QMessage& message2);
    void orderMessages(QMessageIdList& messageIds,  const QMessageOrdering &ordering) const;

    void queryMessagesL(QMessageServiceActionPrivate& privateAction, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const;
    void queryMessagesL(QMessageServiceActionPrivate& privateAction, const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const;
    void countMessagesL(QMessageServiceActionPrivate& privateAction, const QMessageFilter &filter);
    void applyOffsetAndLimitToMsgIds(QMessageIdList& idList, int offset, int limit) const;
    
    QMessageFolderIdList filterMessageFolders(const QMessageFolderFilter& filter, bool& filterHandled) const;
    QMessageFolderIdList filterMessageFoldersL(const QMessageFolderFilter& filter, bool& filterHandled) const;
    void applyOffsetAndLimitToMsgFolderIds(QMessageFolderIdList& idList, int offset, int limit) const;
    
    bool removeMessageL(const QMessageId &id, QMessageStore::RemovalOption option);
    void copyMessageL(TMsvId aMessageId, TMsvId aFolder);
    QMessage messageL(const QMessageId& id) const;
    QMessage smsMessageL(CMsvEntry& receivedEntry, long int messageId) const;
    QMessage mmsMessageL(CMsvEntry& receivedEntry, long int messageId) const;
    QMessage emailMessageL(CMsvEntry& receivedEntry, long int messageId) const;
    QMessage pop3MessageL(CMsvEntry& receivedEntry, long int messageId) const;
    QMessage imap4MessageL(CMsvEntry& receivedEntry, long int messageId) const;
    bool composeSMSL(const QMessage &message);
    bool composeMMSL(const QMessage &message);
    bool composeEmailL(const QMessage &message);
    void tryToDeliverMessageNotifications();
    void updateMMSL(QMessage &message);
    void updateSMSL(QMessage &message);
    void updateEmailL(QMessage &message);
    void showMessageL(const QMessageId &id);
    
    void storeMMSL(QMessage &message, TMsvId dest);
    void sendMMSL();
    void storeEmailL(QMessage &message, TMsvId dest);
    void sendEmailL(QMessage &message);
    void storeSMSL(QMessage &message, TMsvId dest);
    void sendSMSL(QMessage &message);
    void retrieveL(const QMessageId &messageId, const QMessageContentContainerId& id);
    void retrieveBodyL(const QMessageId& id) const;
    void retrieveHeaderL(const QMessageId& id) const;
    
    QDateTime symbianTTimetoQDateTime(const TTime& time) const;
    TTime qDateTimeToSymbianTTime(const QDateTime& date) const;
    
    CMsvEntry* retrieveCMsvEntry(TMsvId id = 0) const;
    void releaseCMsvEntry(CMsvEntry* pEntry) const;

private: // from CActive
    void RunL();
    void DoCancel();

private: //from MMsvSessionObserver:
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
                             TAny* aArg2, TAny* aArg3);

private:
    QMessageStorePrivate* ipMessageStorePrivate;
    
    bool iSessionReady;
    bool iListenForNotifications;

    mutable CRichText*        ipRichText;
    mutable CCharFormatLayer* ipCharFormatLayer;
    mutable CParaFormatLayer* ipParaFormatLayer;

    MTMType             iMTMType;       
    
    CMsvSession*        ipMsvSession;
    CClientMtmRegistry* ipClientMtmReg;
    CSmsClientMtm*      ipSmsMtm;
    CMmsClientMtm*      ipMmsMtm;
    CSmtpClientMtm*     ipSmtpMtm;
    CImap4ClientMtm*    ipImap4Mtm;
    CPop3ClientMtm*     ipPop3Mtm;
    TBuf<KMaxPath>      iPath;
    
    QString iSMSAccountidAsString;
    QString iMMSAccountidAsString;

    mutable RPointerArray<CMsvEntry> iCmsvEntryPoolFree;
    mutable RPointerArray<CMsvEntry> iCmsvEntryPoolInUse;
    
    CMsvFindOperation*  ipFindOperation;
    
    QMessageStore::NotificationFilterId _filterId;
    QMap<QMessageStore::NotificationFilterId, QMessageFilter> _filters;
    
    mutable QHash<QString, QMessageAccount> iAccounts;
    
    int                 iDeliveryTriesCounter;
    QList<MessageEvent> iUndeliveredMessageEvents;
    RTimer              iTimer; // Timer used for delaying delivering of received
                                // messages until messages are ready to be read
    
    mutable int iOperationIds;
    mutable QList<MessageQueryInfo> iMessageQueries;
    
    mutable QMessageAccountOrdering iCurrentAccountOrdering;
    mutable QMessageFolderOrdering iCurrentFolderOrdering;
    mutable QMessageOrdering iCurrentMessageOrdering;
    
    friend class QMessageServiceAction;
    friend class CMessagesFindOperation;
};

class CMessagesFindOperation : public CActive
{
public:
    CMessagesFindOperation(CMTMEngine& aOwner, CMsvSession* apMsvSession, int aOperationId); 
    ~CMessagesFindOperation();

    void filterAndOrderMessages(const QMessageFilter& filter,
                                const QMessageOrdering& ordering,
                                const QString body = QString(),
                                QMessageDataComparator::Options options = 0);
    void filterAndOrderMessages(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                const QMessageOrdering& ordering,
                                const QString body = QString(),
                                QMessageDataComparator::Options options = 0);
    
protected: // From CActive
    void RunL();
    void DoCancel();
    
private:
    void getAllMessagesL(const TMsvSelectionOrdering ordering = TMsvSelectionOrdering());
    void getStandardFolderSpecificMessagesL(TMsvId standardFolderId, const TMsvSelectionOrdering ordering = TMsvSelectionOrdering());
    void getAccountSpecificMessagesL(QMessageAccount& messageAccount, const TMsvSelectionOrdering ordering, QMessageFilterPrivate* privateFolderFilter = NULL);
    void getServiceSpecificMessagesL(TMsvId serviceId, const TMsvSelectionOrdering ordering, QMessageFilterPrivate* privateFolderFilter = NULL);
    void getServiceSpecificMessagesFromFolderL(TMsvId serviceId, const TMsvSelectionOrdering ordering, TMsvId standardFolderId = NULL);

private: // Data
    CMTMEngine& iOwner;
    CMsvSession* ipMsvSession;
    
    int iNumberOfHandledFilters;
    int iOperationId;
    TMsvSelectionOrdering iOrdering;
    bool iResultCorrectlyOrdered;
    QMessageIdList iIdList;
    QMessageFilterPrivate::SortedMessageFilterList iFilterList;
    
    CMsvEntrySelection* ipEntrySelection;
    CMsvFindOperation* ipMsvFindOperation;
    
    mutable RTimer iTimer;
};

#endif // QMTMENGINE_SYMBIAN_H
