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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QMTMENGINE_SYMBIAN_H
#define QMTMENGINE_SYMBIAN_H

#include <QMap>
#include <QSet>
#include <QList>

#include <e32base.h>
#include <msvapi.h> // MMsvSessionObserver
#include "qmessagestore.h"
#include "qmessagestore_symbian_p.h"
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "qmessagefolderfilter.h"
#include "QMessageServiceAction.h"


QTM_BEGIN_NAMESPACE

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
    QString body;
    QMessageDataComparator::Options options;
    QMessageFilter filter;
    QMessageOrdering ordering;
    QMessageServiceActionPrivate* privateAction;
    CMessagesFindOperation* findOperation;
    int currentFilterListIndex;
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
    bool countMessages(const QMessageFilter &filter, uint limit) const;
    bool countMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, uint limit) const;
    bool showMessage(const QMessageId &id);
    bool composeMessage(const QMessage &message);
    
    QMessage message(const QMessageId& id) const;
    
    bool storeMMS(QMessage &message, TMsvId dest);
    void sendMMS();
    bool storeEmail(QMessage &message, TMsvId dest);
    void sendEmail(QMessage &message);
    bool storeSMS(QMessage &message, TMsvId dest);
    bool sendSMS(QMessage &message);
    bool retrieve(const QMessageContentContainerId& id);
    bool retrieveBody(const QMessageId& id);
    bool retrieveHeader(const QMessageId& id);
    
    QMessageStore::NotificationFilterId registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
                                        const QMessageFilter& filter);
    void unregisterNotificationFilter(QMessageStore::NotificationFilterId notificationFilterId);

    void notification(TMsvSessionEvent aEvent, TUid aMsgType, TMsvId aFolderId, TMsvId aMessageId);
    void filterAndOrderMessagesReady(bool success, int operationId, QMessageIdList ids, int numberOfHandledFilters);

private:
    void updateEmailAccountsL() const;
    bool switchToMTMRootEntry(MTMType aMTMType);
    TMsvId mtmServiceEntryIdL(MTMType aMTMType);
    CBaseMtm* mtmByType(MTMType aMTMType);
    CBaseMtm* mtmByUid(TUid aMTMUid);
    TUid mtmUidByType(MTMType aMTMType);
    TMsvId standardFolderId(QMessage::StandardFolder standardFolder);
    
    QMessageAccountIdList accountsByType(QMessage::Type type) const;
    
    QMessageFolder folderL(const QMessageFolderId &id) const;

    static bool accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2);
    void orderAccounts(QMessageAccountIdList& accountIds,  const QMessageAccountOrdering &ordering) const;
    static bool folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2);
    void orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderOrdering &ordering) const;
    static bool messageLessThan(const QMessageId messageId1, const QMessageId messageId2);
    void orderMessages(QMessageIdList& messageIds,  const QMessageOrdering &ordering) const;
    
    QMessageAccountIdList filterAccounts(const QMessageAccountFilter& filter) const;
    QMessageFolderIdList filterMessageFolders(const QMessageFolderFilter& filter) const;

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
    void retrieveL(const QMessageContentContainerId& id);
    void retrieveBodyL(const QMessageId& id) const;
    void retrieveHeaderL(const QMessageId& id);
    
    QDateTime symbianTTimetoQDateTime(const TTime& time) const;
    TTime qDateTimeToSymbianTTime(const QDateTime& date) const;

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
    
    CMsvFindOperation*  ipFindOperation;
    
    QMessageStore::NotificationFilterId _filterId;
    QMap<QMessageStore::NotificationFilterId, QMessageFilter> _filters;
    
    mutable QList<QMessageAccount> iAccounts;
    
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
    void getAccountSpecificMessagesL(QMessageAccount& messageAccount, const TMsvSelectionOrdering ordering, QMessageFilterPrivate* privateFolderFilter = NULL);
    void getMTMSpecificMessagesL(TUid mtmUid, const TMsvSelectionOrdering ordering, QMessageFilterPrivate* privateFolderFilter = NULL);
    void getMTMSpecificMessagesFromFolderL(TUid mtmUid, const TMsvSelectionOrdering ordering, TMsvId standardFolderId);

private: // Data
    CMTMEngine& iOwner;
    CMsvSession* ipMsvSession;
    
    int iNumberOfHandledFilters;
    int iOperationId;
    QMessageIdList iIdList;
    QMessageFilterPrivate::SortedMessageFilterList iFilterList;
    
    CMsvEntrySelection* ipEntrySelection;
    CMsvFindOperation* ipMsvFindOperation;
    
    mutable RTimer iTimer;
};


QTM_END_NAMESPACE
#endif // QMTMENGINE_SYMBIAN_H
