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

#include "support.h"
#include <qmessageaccountid.h>
#include <qmessagefolderid.h>
#include <qmessageid.h>
#include <qmessagemanager.h>
#include <qmessage_symbian_p.h>
#include <messagingutil_p.h>

#include <QDebug>

#include <pop3set.h>
#include <imapset.h>
#include <smtpset.h>
#include <iapprefs.h>
#include <cemailaccounts.h>
#include <metadatabase.h>
#include <commsdat.h>

#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
const TInt KDefaultImapFetchSize = 10;
const TInt KDefaultPopFetchSize = 30;
const TInt KDefaultSyncLimit  = 30;
const TInt KDefaultPopPort = 110;
const TInt KDefaultImapPort = 993;
const TInt KDefaultSmtpPort = 465;
_LIT(KDefaultAddress,"user.name@testemail.com");
_LIT8(KDefaultUserName,"username");
_LIT8(KDefaultPassword,"password");
_LIT(KDefaultServerIn, "imap.server.testemail.com");
_LIT(KDefaultServerOut, "smtp.server.testemail.com");
#else
_LIT(KPopServer, "ban-sindhub01.intra");
_LIT8(KPopLoginName, "ban-sindhub01");
_LIT8(KPopPassword, "ban-sindhub01");

_LIT(KImapServer, "ban-sindhub01.intra");
_LIT8(KImapPassword,"ban-sindhub01");
_LIT8(KImapLoginName,"ban-sindhub01");

_LIT(KSmtpServerAddress, "ban-sindhub01.intra");
_LIT(KEmailAlias, "Messaging example");
#endif

#define IMAP_ACCOUNTID_PREFIX 0x2000E53F
#define POP3_ACCOUNTID_PREFIX 0x2000E53E

#define KDocumentsEntryIdValue    0x1008

class CSymbianMessagingSession : public CBase, MMsvSessionObserver
{
public:
    CSymbianMessagingSession();
    ~CSymbianMessagingSession();
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
    CMsvSession* ipMsvSession;
};

CSymbianMessagingSession::CSymbianMessagingSession()
{
    TRAPD(err, ipMsvSession = CMsvSession::OpenSyncL(*this));
    Q_UNUSED(err)
}

CSymbianMessagingSession::~CSymbianMessagingSession()
{
    delete ipMsvSession;
}

void CSymbianMessagingSession::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}

QTM_BEGIN_NAMESPACE

using namespace MessagingUtil;

class MapiSession
{
public:
    static QMessageId addMessage(const Support::Parameters &params);
};

// The class 'MapiSession' is a friend of QMessageContentContainer - hijack it here
// => This is needed to make it possible to set parentFolderId
QMessageId MapiSession::addMessage(const Support::Parameters &params)
{
    QString parentAccountName(params["parentAccountName"]);
    QString parentFolderPath(params["parentFolderPath"]);
    QString to(params["to"]);
    QString cc(params["cc"]);
    QString from(params["from"]);
    QString date(params["date"]);
    QString receivedDate(params["receivedDate"]);
    QString subject(params["subject"]);
    QString text(params["text"]);
    QString mimeType(params["mimeType"]);
    QString attachments(params["attachments"]);
    QString priority(params["priority"]);
    QString size(params["size"]);
    QString type(params["type"]);
    QString read(params["status-read"]);
    QString hasAttachments(params["status-hasAttachments"]);

    QMessageManager manager;

    if (!to.isEmpty() && !from.isEmpty() && !date.isEmpty() && !subject.isEmpty() &&
        !parentAccountName.isEmpty() && !parentFolderPath.isEmpty()) {
        // Find the named account
        QMessageAccountIdList accountIds(manager.queryAccounts(QMessageAccountFilter::byName(parentAccountName)));
        if (accountIds.count() == 1) {
            // Find the specified folder
            QMessageFolderFilter filter(QMessageFolderFilter::byName(parentFolderPath) & QMessageFolderFilter::byParentAccountId(accountIds.first()));
            QMessageFolderIdList folderIds(manager.queryFolders(filter));
            if (folderIds.count() == 1) {
                QMessage message;

                message.setParentAccountId(accountIds.first());
                message.d_ptr->_parentFolderId = folderIds.first();

                QList<QMessageAddress> toList;
                foreach (const QString &addr, to.split(",")) {
                    toList.append(QMessageAddress(QMessageAddress::Email, addr.trimmed()));
                }
                message.setTo(toList);

                QList<QMessageAddress> ccList;
                foreach (const QString &addr, cc.split(",")) {
					if (!addr.isEmpty()) {
                    ccList.append(QMessageAddress(QMessageAddress::Email, addr.trimmed()));
					}
                }
                message.setCc(ccList);

                message.setFrom(QMessageAddress(QMessageAddress::Email, from));
                message.setSubject(subject);

                QDateTime dt(QDateTime::fromString(date, Qt::ISODate));
                dt.setTimeSpec(Qt::UTC);
                message.setDate(dt);

                if (type.isEmpty()) {
                    message.setType(QMessage::Email);
                } else {
                    if (type.toLower() == "mms") {
                        message.setType(QMessage::Mms);
                    } else if (type.toLower() == "sms") {
                        message.setType(QMessage::Sms);
                    } else if (type.toLower() == "instantmessage") {
                        message.setType(QMessage::InstantMessage);
                    } else {
                        message.setType(QMessage::Email);
                    }
                }

                if (!receivedDate.isEmpty()) {
                    QDateTime dt(QDateTime::fromString(receivedDate, Qt::ISODate));
                    dt.setTimeSpec(Qt::UTC);
                    message.setReceivedDate(dt);
                }

                if (!priority.isEmpty()) {
                    if (priority.toLower() == "high") {
                        message.setPriority(QMessage::HighPriority);
                    } else if (priority.toLower() == "low") {
                        message.setPriority(QMessage::LowPriority);
                    }
                }

                /*if (!size.isEmpty()) {
                    message.d_ptr->_size = size.toUInt();
                }*/

                if (!text.isEmpty()) {
                    message.setBody(text, mimeType.toAscii());
                }

                if (!attachments.isEmpty()) {
                    message.appendAttachments(attachments.split("\n"));
                }

                QMessage::StatusFlags flags(0);
                if (read.toLower() == "true") {
                    flags |= QMessage::Read;
                }
                if (hasAttachments.toLower() == "true") {
                    flags |= QMessage::HasAttachments;
                }
                message.setStatus(flags);

                if (!manager.addMessage(&message)) {
                    qWarning() << "Unable to addMessage:" << to << from << date << subject;
                } else {
                    return message.id();
                }
            } else {
                qWarning() << "Unable to locate parent folder:" << parentFolderPath;
            }
        } else {
            qWarning() << "Unable to locate parent account:" << parentAccountName;
        }
    } else {
        qWarning() << "Necessary information missing";
    }

    return QMessageId();
}
QTM_END_NAMESPACE

namespace Support {

using namespace CommsDat;

void deleteServiceRelatedEntriesFromFolder(TMsvId serviceId, TMsvId folderId)
{
    CSymbianMessagingSession* pSession = new(ELeave) CSymbianMessagingSession();
    CleanupStack::PushL(pSession);
    CMsvEntry* pEntry = CMsvEntry::NewL(*(pSession->ipMsvSession), folderId, TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
    CleanupStack::PushL(pEntry);
    CMsvEntrySelection* pEntries = pEntry->ChildrenWithServiceL(serviceId);
    CleanupStack::PushL(pEntries);
    for (int i = 0; i < pEntries->Count(); i++) {
        CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
        CMsvOperation* pMsvOperation = pEntry->DeleteL(pEntries->At(i), pMsvOperationWait->iStatus);
        CleanupStack::PushL(pMsvOperation);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy(pMsvOperation);
        CleanupStack::PopAndDestroy(pMsvOperationWait);
    }
    CleanupStack::PopAndDestroy(pEntries);
    CleanupStack::PopAndDestroy(pEntry);
    CleanupStack::PopAndDestroy(pSession);
}

void clearMessageStoreL()
{
    CEmailAccounts* pEmailAccounts = CEmailAccounts::NewLC();

    RArray<TImapAccount> imapAccounts(10);
    pEmailAccounts->GetImapAccountsL(imapAccounts);
    for (int i=0; i < imapAccounts.Count(); i++) {
        CImImap4Settings* settings = new(ELeave) CImImap4Settings();
        CleanupStack::PushL(settings);
        pEmailAccounts->LoadImapSettingsL(imapAccounts[i], *settings);
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
        if (settings->ServerAddress().Compare(KDefaultServerIn) == 0) {
#else        
        if (settings->ServerAddress().Compare(KImapServer) == 0) {
#endif            
            pEmailAccounts->DeleteImapAccountL(imapAccounts[i]);
            deleteServiceRelatedEntriesFromFolder(imapAccounts[i].iImapService, KDocumentsEntryIdValue);
        }
        CleanupStack::PopAndDestroy(settings);
    }

    RArray<TPopAccount> popAccounts(10);
    pEmailAccounts->GetPopAccountsL(popAccounts);
    for (int i=0; i < popAccounts.Count(); i++) {
        CImPop3Settings* settings = new(ELeave) CImPop3Settings();
        CleanupStack::PushL(settings);
        pEmailAccounts->LoadPopSettingsL(popAccounts[i], *settings);
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
        if (settings->ServerAddress().Compare(KDefaultServerIn) == 0) {
#else        
        if (settings->ServerAddress().Compare(KPopServer) == 0) {
#endif            
            pEmailAccounts->DeletePopAccountL(popAccounts[i]);
            deleteServiceRelatedEntriesFromFolder(popAccounts[i].iPopService, KDocumentsEntryIdValue);
        }
        CleanupStack::PopAndDestroy(settings);
    }

    RArray<TSmtpAccount> smtpAccounts(10);
    pEmailAccounts->GetSmtpAccountsL(smtpAccounts);
    for (int i=0; i < smtpAccounts.Count(); i++) {
        CImSmtpSettings* settings = new(ELeave) CImSmtpSettings();
        CleanupStack::PushL(settings);
        pEmailAccounts->LoadSmtpSettingsL(smtpAccounts[i], *settings);
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
        if (settings->ServerAddress().Compare(KDefaultServerOut) == 0) {
#else        
        if (settings->ServerAddress().Compare(KSmtpServerAddress) == 0) {
#endif            
            pEmailAccounts->DeleteSmtpAccountL(smtpAccounts[i]);
            deleteServiceRelatedEntriesFromFolder(smtpAccounts[i].iSmtpService, KDocumentsEntryIdValue);
        }
        CleanupStack::PopAndDestroy(settings);
    }

    CleanupStack::PopAndDestroy(pEmailAccounts);
}

void clearMessageStore()
{
    TRAPD(err, clearMessageStoreL());
    Q_UNUSED(err)
}

QMessageAccountId createPopAndSmtpAccountL(const TDesC& accountName, const TDesC& fromAddress)
{
    CEmailAccounts* pEmailAccounts = CEmailAccounts::NewLC();

    // Create IAP Preferences for IMAP & POP3
    CImIAPPreferences* pImIAPPreferences = CImIAPPreferences::NewLC();

    // Create POP3 Account
    CImPop3Settings* pImPop3Settings = new(ELeave) CImPop3Settings();
    CleanupStack::PushL(pImPop3Settings);
    pEmailAccounts->PopulateDefaultPopSettingsL(*pImPop3Settings, *pImIAPPreferences);
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
    pImPop3Settings->SetAutoSendOnConnect(ETrue);
    pImPop3Settings->SetDisconnectedUserMode(ETrue);
    pImPop3Settings->SetDeleteEmailsWhenDisconnecting(EFalse);
    pImPop3Settings->SetAcknowledgeReceipts(EFalse);
    pImPop3Settings->SetGetMailOptions(EGetPop3EmailMessages);
    pImPop3Settings->SetInboxSynchronisationLimit(KDefaultSyncLimit);
    pImPop3Settings->SetPopulationLimitL(KDefaultPopFetchSize);
    pImPop3Settings->SetSecureSockets(EFalse);
    pImPop3Settings->SetSSLWrapper(EFalse);
    
    pImPop3Settings->SetLoginNameL(KDefaultUserName);
    pImPop3Settings->SetPasswordL(KDefaultPassword);
    pImPop3Settings->SetServerAddressL(KDefaultServerIn); 
    pImPop3Settings->SetPort(KDefaultPopPort);
#else    
    pImPop3Settings->SetServerAddressL(KPopServer);
    pImPop3Settings->SetLoginNameL(KPopLoginName);
    pImPop3Settings->SetPasswordL(KPopPassword);
    pImPop3Settings->SetPort(110);
#endif    
    TPopAccount popAccount = pEmailAccounts->CreatePopAccountL(accountName, *pImPop3Settings, *pImIAPPreferences, EFalse);

    // Create SMTP Account
    CImSmtpSettings *pImSmtpSettings = new (ELeave) CImSmtpSettings();
    CleanupStack::PushL(pImSmtpSettings);
    pEmailAccounts->PopulateDefaultSmtpSettingsL(*pImSmtpSettings, *pImIAPPreferences);
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
    pImSmtpSettings->SetBodyEncoding(EMsgOutboxMIME);
    pImSmtpSettings->SetAddVCardToEmail(EFalse);
    pImSmtpSettings->SetAddSignatureToEmail(EFalse);
    pImSmtpSettings->SetRequestReceipts(EFalse);
    pImSmtpSettings->SetSMTPAuth(ETrue);
    pImSmtpSettings->SetToCcIncludeLimitL(KSmtpToCcIncludeLimitMax);
    pImSmtpSettings->SetSecureSockets(EFalse);
    pImSmtpSettings->SetSSLWrapper(EFalse);
    
    pImSmtpSettings->SetEmailAddressL(KDefaultAddress);
    pImSmtpSettings->SetLoginNameL(KDefaultUserName);
    pImSmtpSettings->SetPasswordL(KDefaultPassword);
    pImSmtpSettings->SetPort(KDefaultSmtpPort);
    pImSmtpSettings->SetServerAddressL(KDefaultServerOut);
#else    
    pImSmtpSettings->SetServerAddressL(KSmtpServerAddress);
    pImSmtpSettings->SetEmailAliasL(KEmailAlias);
    pImSmtpSettings->SetPort(25);
#endif    
    pImSmtpSettings->SetEmailAddressL(fromAddress);
    pImSmtpSettings->SetReplyToAddressL(fromAddress);
    pImSmtpSettings->SetReceiptAddressL(fromAddress);
    pEmailAccounts->CreateSmtpAccountL(popAccount, *pImSmtpSettings, *pImIAPPreferences, EFalse);

    CleanupStack::PopAndDestroy(pImSmtpSettings);
    CleanupStack::PopAndDestroy(pImPop3Settings);
    CleanupStack::PopAndDestroy(pImIAPPreferences);
    CleanupStack::PopAndDestroy(pEmailAccounts);

#if defined(FREESTYLEMAILUSED) 
    return QMessageAccountId(addIdPrefix(QString::number(popAccount.iPopService), SymbianHelpers::EngineTypeFreestyle));
#elif defined(FREESTYLENMAILUSED)
    quint64 id = POP3_ACCOUNTID_PREFIX;
    id << 16;
    id += popAccount.iPopService; 
    return QMessageAccountId(addIdPrefix(QString::number(id), SymbianHelpers::EngineTypeFreestyle));
#else    
    return QMessageAccountId(addIdPrefix(QString::number(popAccount.iPopService), SymbianHelpers::EngineTypeMTM));
#endif    
}

QMessageAccountId createPopAndSmtpAccount(const TDesC& accountName, const TDesC& fromAddress)
{
    QMessageAccountId retVal;
    TRAPD(err, retVal = createPopAndSmtpAccountL(accountName, fromAddress));
    Q_UNUSED(err)
    return retVal;
}

bool imapAccountByNameL(const TDesC& accountName, TImapAccount& account)
{
    CEmailAccounts* pEmailAccounts = CEmailAccounts::NewLC();

    RArray<TImapAccount> imapAccounts(10);
    pEmailAccounts->GetImapAccountsL(imapAccounts);
    for (int i=0; i < imapAccounts.Count(); i++) {
        if (imapAccounts[i].iImapAccountName.Compare(accountName) == 0) {
            account = imapAccounts[i];
            CleanupStack::PopAndDestroy(pEmailAccounts);
            return true;
        }
    }

    CleanupStack::PopAndDestroy(pEmailAccounts);
    return false;
}

bool imapAccountByName(const TDesC& accountName, TImapAccount& account)
{
    bool retVal = false;
    TRAPD(err, retVal = imapAccountByNameL(accountName, account));
    Q_UNUSED(err)
    return retVal;
}

bool pop3AccountByNameL(const TDesC& accountName, TPopAccount& account)
{
    CEmailAccounts* pEmailAccounts = CEmailAccounts::NewLC();

    RArray<TPopAccount> popAccounts(10);
    pEmailAccounts->GetPopAccountsL(popAccounts);
    for (int i=0; i < popAccounts.Count(); i++) {
        if (popAccounts[i].iPopAccountName.Compare(accountName) == 0) {
            account = popAccounts[i];
            CleanupStack::PopAndDestroy(pEmailAccounts);
            return true;
        }
    }

    CleanupStack::PopAndDestroy(pEmailAccounts);
    return false;
}

bool pop3AccountByName(const TDesC& accountName, TPopAccount& account)
{
    bool retVal = false;
    TRAPD(err, retVal = pop3AccountByNameL(accountName, account));
    Q_UNUSED(err)
    return retVal;
}

bool smtpAccountByNameL(const TDesC& accountName, TSmtpAccount& account)
{
    CEmailAccounts* pEmailAccounts = CEmailAccounts::NewLC();

    RArray<TSmtpAccount> smtpAccounts(10);
    pEmailAccounts->GetSmtpAccountsL(smtpAccounts);
    for (int i=0; i < smtpAccounts.Count(); i++) {
        if (smtpAccounts[i].iSmtpAccountName.Compare(accountName) == 0) {
            account = smtpAccounts[i];
            CleanupStack::PopAndDestroy(pEmailAccounts);
            return true;
        }
    }

    CleanupStack::PopAndDestroy(pEmailAccounts);
    return false;
}

bool smtpAccountByName(const TDesC& accountName, TSmtpAccount& account)
{
    bool retVal = false;
    TRAPD(err, retVal = smtpAccountByNameL(accountName, account));
    Q_UNUSED(err)
    return retVal;
}

QMessageAccountId createImapAndSmtpAccountL(const TDesC& accountName, const TDesC& fromAddress)
{
    CEmailAccounts* pEmailAccounts = CEmailAccounts::NewLC();

    // Create IAP Preferences for IMAP & SMTP
    CImIAPPreferences* pImIAPPreferences = CImIAPPreferences::NewLC();

    // Create IMAP Account
    CImImap4Settings* pImap4Settings = new(ELeave)CImImap4Settings;
    CleanupStack::PushL(pImap4Settings);
    pEmailAccounts->PopulateDefaultImapSettingsL(*pImap4Settings, *pImIAPPreferences);
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
    pImap4Settings->SetAutoSendOnConnect(ETrue);
    pImap4Settings->SetDisconnectedUserMode(ETrue);
    pImap4Settings->SetDeleteEmailsWhenDisconnecting(EFalse);
    pImap4Settings->SetAcknowledgeReceipts(EFalse);
    pImap4Settings->SetSynchronise(EUseLocal);
    pImap4Settings->SetInboxSynchronisationLimit(KDefaultSyncLimit);
    pImap4Settings->SetBodyTextSizeLimitL(KDefaultImapFetchSize);
    pImap4Settings->SetPathSeparator('/');
    pImap4Settings->SetUpdatingSeenFlags(ETrue);
    
    pImap4Settings->SetLoginNameL(KDefaultUserName);
    pImap4Settings->SetPasswordL(KDefaultPassword);
    pImap4Settings->SetServerAddressL(KDefaultServerIn); 
    pImap4Settings->SetPort(KDefaultImapPort);
    pImap4Settings->SetSecureSockets(EFalse);
    pImap4Settings->SetSSLWrapper(ETrue);    
#else
    pImap4Settings->SetServerAddressL(KImapServer);
    pImap4Settings->SetLoginNameL(KImapLoginName);
    pImap4Settings->SetPasswordL(KImapPassword);
    pImap4Settings->SetPort(143);
#endif   
    TImapAccount imapAccount = pEmailAccounts->CreateImapAccountL(accountName, *pImap4Settings, *pImIAPPreferences, EFalse);

    // Create SMTP Account
    CImSmtpSettings* pImSmtpSettings = new (ELeave) CImSmtpSettings();
    CleanupStack::PushL(pImSmtpSettings);
    pEmailAccounts->PopulateDefaultSmtpSettingsL(*pImSmtpSettings, *pImIAPPreferences);

#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
    pImSmtpSettings->SetBodyEncoding(EMsgOutboxMIME);
    pImSmtpSettings->SetAddVCardToEmail(EFalse);
    pImSmtpSettings->SetAddSignatureToEmail(EFalse);
    pImSmtpSettings->SetRequestReceipts(EFalse);
    pImSmtpSettings->SetSMTPAuth(ETrue);
    pImSmtpSettings->SetToCcIncludeLimitL(KSmtpToCcIncludeLimitMax);
    pImSmtpSettings->SetSecureSockets(EFalse);
    pImSmtpSettings->SetSSLWrapper(EFalse);
    
    pImSmtpSettings->SetEmailAddressL(KDefaultAddress);
    pImSmtpSettings->SetLoginNameL(KDefaultUserName);
    pImSmtpSettings->SetPasswordL(KDefaultPassword);
    pImSmtpSettings->SetPort(KDefaultSmtpPort);
    pImSmtpSettings->SetServerAddressL(KDefaultServerOut);
#else    
    pImSmtpSettings->SetServerAddressL(KSmtpServerAddress);
    pImSmtpSettings->SetEmailAliasL(KEmailAlias);
    pImSmtpSettings->SetPort(25);
#endif
    pImSmtpSettings->SetEmailAddressL(fromAddress);
    pImSmtpSettings->SetReplyToAddressL(fromAddress);
    pImSmtpSettings->SetReceiptAddressL(fromAddress);
    TSmtpAccount smtpAccount = pEmailAccounts->CreateSmtpAccountL(imapAccount, *pImSmtpSettings, *pImIAPPreferences, EFalse);
    
    TSmtpAccount defaultAccount;
    if (pEmailAccounts->DefaultSmtpAccountL(defaultAccount) == KErrNotFound)
        {
        pEmailAccounts->SetDefaultSmtpAccountL(smtpAccount);
        }

    CleanupStack::PopAndDestroy(pImSmtpSettings);
    CleanupStack::PopAndDestroy(pImap4Settings);
    CleanupStack::PopAndDestroy(pImIAPPreferences);
    CleanupStack::PopAndDestroy(pEmailAccounts);

#if defined(FREESTYLEMAILUSED) 
    return QMessageAccountId(addIdPrefix(QString::number(imapAccount.iImapService), SymbianHelpers::EngineTypeFreestyle));
#elif defined(FREESTYLENMAILUSED)
    quint64 id = IMAP_ACCOUNTID_PREFIX;
    id = id << 32;
    id += imapAccount.iImapService; 
    return QMessageAccountId(addIdPrefix(QString::number(id), SymbianHelpers::EngineTypeFreestyle));
#else    
    return QMessageAccountId(addIdPrefix(QString::number(imapAccount.iImapService), SymbianHelpers::EngineTypeMTM));
#endif    
}

QMessageAccountId createImapAndSmtpAccount(const TDesC& accountName, const TDesC& fromAddress)
{
    QMessageAccountId retVal;
    TRAPD(err, retVal = createImapAndSmtpAccountL(accountName, fromAddress));
    Q_UNUSED(err)
    return retVal;
}

QMessageAccountId addAccount(const Parameters &params)
{
    QString accountName(params["name"]);
    TPtrC16 symbianAccountName(KNullDesC);
    symbianAccountName.Set(reinterpret_cast<const TUint16*>(accountName.utf16()));

    QString fromAddress(params["fromAddress"]);
    TPtrC16 symbianFromAddress(KNullDesC);
    symbianFromAddress.Set(reinterpret_cast<const TUint16*>(fromAddress.utf16()));

    return createImapAndSmtpAccount(symbianAccountName, symbianFromAddress);
}

QMessageFolderId addFolderL(const TDesC& symbianAccountName, const TDesC& symbianFolderName)
{
    TSmtpAccount account;
    bool retVal = smtpAccountByName(symbianAccountName, account);
    Q_UNUSED(retVal)

    CSymbianMessagingSession* pSession = new CSymbianMessagingSession();
    CleanupStack::PushL(pSession);
    CMsvEntry* pEntry = CMsvEntry::NewL(*(pSession->ipMsvSession), KDocumentsEntryIdValue, TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone,ETrue));
    CleanupStack::PushL(pEntry);

    TMsvEntry newFolder;
    newFolder.iType = KUidMsvFolderEntry;
    newFolder.iMtm = KUidMsvLocalServiceMtm;
    newFolder.iServiceId = account.iSmtpService;
    newFolder.SetReadOnly(EFalse);
    newFolder.SetVisible(ETrue);
    newFolder.iDescription.Set(symbianFolderName);
    newFolder.iDetails.Set(symbianFolderName);
    newFolder.iDate.HomeTime();
    pEntry->CreateL(newFolder);
    TMsvId folderId = newFolder.Id();

    CleanupStack::PopAndDestroy(pEntry);
    CleanupStack::PopAndDestroy(pSession);

    QString nullString = "00000000";
    QString serviceEntryIdString = QString::number(account.iRelatedService);
    serviceEntryIdString = nullString.left(8-serviceEntryIdString.length()) + serviceEntryIdString;
    QString folderIdString = QString::number(folderId);
    folderIdString = nullString.left(8-folderIdString.length()) + folderIdString;
    return addIdPrefix(serviceEntryIdString+folderIdString, SymbianHelpers::EngineTypeMTM);
}

QMessageFolderId addFolder(const Parameters &params)
{
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
    return QMessageFolderId(); // Local folders can not be created for FreeStyle
#else    
    QString accountName(params["parentAccountName"]);
    TPtrC16 symbianAccountName(KNullDesC);
    symbianAccountName.Set(reinterpret_cast<const TUint16*>(accountName.utf16()));

    QString folderName(params["name"]);
    TPtrC16 symbianFolderName(KNullDesC);
    symbianFolderName.Set(reinterpret_cast<const TUint16*>(folderName.utf16()));

    QMessageFolderId id;
    TRAPD(err, id = addFolderL(symbianAccountName, symbianFolderName));
    Q_UNUSED(err)
    return id;
#endif
}

QMessageId addMessage(const Parameters &params)
{
    return MapiSession::addMessage(params);
}

}

