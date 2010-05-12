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

#include "qmessagestore_symbian_p.h"
#include "qmtmengine_symbian_p.h"
#include "qmessage_symbian_p.h"
#include "qmessagesortorder_p.h"
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageaccountsortorder_p.h"
#include "qmessagefoldersortorder_p.h"
#include "qmessagesortorder_p.h"
#include "qmessagefolder.h"
#include "qmessagefolder_p.h"
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"
#include "qmessageservice_symbian_p.h"
#include "qmessagecontentcontainer_symbian_p.h"
#include "qmessagecontentcontainer_p.h"

#include <msvstd.h>
#include <msvids.h> // TMsvId's
#include <mtclreg.h>   // CClientMtmRegistry
#include <smsclnt.h>   // CSmsClientMtm
#include <mmsclient.h> // CMmsClientMtm
#include <smtcmtm.h>   // CSmtpClientMtm
#include <impcmtm.h>   // CImap4ClientMtm
#include <popcmtm.h>   // CPop3ClientMtm
#include <cemailaccounts.h> //CEmailAccounts
#include <msvfind.h>   // CMsvFindOperation  
#include <mtmdef.h>    // TMsvPartList
#include <utf.h>       // CnvUtfConverter
#include <mmsvattachmentmanager.h>
#include <cmsvmimeheaders.h> // Attachemt mimeheader
#include <eikenv.h>
#include <smut.h>
#include <smuthdr.h>
#include <mtuireg.h> // CMtmUiRegistry
#include <mtmuibas.h> // CBaseMtmUi
#include <SendUiConsts.h>
#include <sendui.h>    // SendUi API
#include <CMessageData.h> //CMessageData
#include <apgcli.h>
#include <rsendas.h>
#include <rsendasmessage.h>
#include <cmsvrecipientlist.h>
#include <imapset.h>
#include <miutmsg.h>
#include <charconv.h>
#include <imcvtext.h> // KImcvMultipart declaration
#include <smscmds.h>

#include <QTextCodec>
#include <messagingutil_p.h>


QTM_BEGIN_NAMESPACE

const TInt KWaitAfterReceivedMessage = 100000; // = 0.1 seconds
#define KDocumentsEntryIdValue    0x1008

Q_GLOBAL_STATIC(CMTMEngine,mtmEngine);

CMTMEngine::CMTMEngine()
 : CActive(EPriorityStandard)
{
    iFsSession.Connect();
    CActiveScheduler::Add(this);
    iTimer.CreateLocal();

    TRAPD(err,
        ipMsvSession = CMsvSession::OpenSyncL(*this);
        iSessionReady = ETrue;
        ipClientMtmReg = CClientMtmRegistry::NewL(*ipMsvSession);
        ipSmsMtm = static_cast<CSmsClientMtm*>(ipClientMtmReg->NewMtmL(KUidMsgTypeSMS));
        ipMmsMtm = static_cast<CMmsClientMtm*>(ipClientMtmReg->NewMtmL(KUidMsgTypeMultimedia));
        ipSmtpMtm = static_cast<CSmtpClientMtm*>(ipClientMtmReg->NewMtmL(KUidMsgTypeSMTP));
        ipImap4Mtm = static_cast<CImap4ClientMtm*>(ipClientMtmReg->NewMtmL(KUidMsgTypeIMAP4));
        ipPop3Mtm = static_cast<CPop3ClientMtm*>(ipClientMtmReg->NewMtmL(KUidMsgTypePOP3));
        );
    Q_UNUSED(err)

    // Create & Add SMS Account
    TRAPD(accountError,
        iSMSAccountidAsString = SymbianHelpers::addIdPrefix(QString::number(mtmServiceEntryIdL(CMTMEngine::MTMTypeSMS)),SymbianHelpers::EngineTypeMTM);
        QMessageAccount smsAcc = QMessageAccountPrivate::from(QMessageAccountId(iSMSAccountidAsString),
                                                              QString("SMS"),
                                                              mtmServiceEntryIdL(CMTMEngine::MTMTypeSMS),
                                                              0,
                                                              QMessage::Sms);
        iAccounts.insert(iSMSAccountidAsString, smsAcc);


        // Create & Add MMS Account
        iMMSAccountidAsString = SymbianHelpers::addIdPrefix(QString::number(mtmServiceEntryIdL(CMTMEngine::MTMTypeMMS)),SymbianHelpers::EngineTypeMTM);
        QMessageAccount mmsAcc = QMessageAccountPrivate::from(QMessageAccountId(iMMSAccountidAsString),
                                                              QString("MMS"),
                                                              mtmServiceEntryIdL(CMTMEngine::MTMTypeMMS),
                                                              0,
                                                              QMessage::Mms | QMessage::Email);
        iAccounts.insert(iMMSAccountidAsString, mmsAcc);
        updateEmailAccountsL();
        );
    Q_UNUSED(accountError)
    
    TRAPD(err2,
        TBuf<KMaxPath> privatePath;
        FsSession().CreatePrivatePath(EDriveC);
        FsSession().PrivatePath(privatePath);
        iPath.Append(_L("c:"));
        iPath.Append(privatePath);
        iPath.Append(_L("tempattachments\\"));                         
        CFileMan* pFileMan = CFileMan::NewL(FsSession());
        CleanupStack::PushL(pFileMan);
        pFileMan->RmDir(iPath);
        FsSession().MkDirAll(iPath);
        CleanupStack::PopAndDestroy(pFileMan);
    );
    Q_UNUSED(err2)
}

CMTMEngine::~CMTMEngine()
{
    iCmsvEntryPoolFree.ResetAndDestroy();
    iCmsvEntryPoolInUse.ResetAndDestroy();

    delete ipPop3Mtm;
    delete ipImap4Mtm;
    delete ipSmtpMtm;
    delete ipMmsMtm;
    delete ipSmsMtm;

    delete ipClientMtmReg;
    delete ipMsvSession;
    
    TRAPD(error,
        TBuf<KMaxPath> privatePath;
        FsSession().CreatePrivatePath(EDriveC);
        FsSession().PrivatePath(privatePath);
        TBuf<KMaxPath> path;
        path.Append(_L("c:"));
        path.Append(privatePath);
        path.Append(_L("tempattachments\\"));                         
        CFileMan* pFileMan=CFileMan::NewL(FsSession());
        CleanupStack::PushL(pFileMan);
        pFileMan->RmDir(path);
        CleanupStack::PopAndDestroy(pFileMan);
        );
    Q_UNUSED(error)

    Cancel();
    iTimer.Close();
    iFsSession.Close();
}

CMTMEngine* CMTMEngine::instance()
{
    return mtmEngine();
}

bool CMTMEngine::accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2)
{
    CMTMEngine* pMTMEngine = mtmEngine();
    return QMessageAccountSortOrderPrivate::lessThan(pMTMEngine->iCurrentAccountOrdering,
                                                    pMTMEngine->account(accountId1),
                                                    pMTMEngine->account(accountId2));
}

void CMTMEngine::orderAccounts(QMessageAccountIdList& accountIds, const QMessageAccountSortOrder &sortOrder) const
{
    iCurrentAccountOrdering = sortOrder;
    qSort(accountIds.begin(), accountIds.end(), CMTMEngine::accountLessThan);
}

bool CMTMEngine::folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2)
{
    CMTMEngine* pMTMEngine = mtmEngine();
    return QMessageFolderSortOrderPrivate::lessThan(pMTMEngine->iCurrentFolderOrdering,
                                                   pMTMEngine->folder(folderId1),
                                                   pMTMEngine->folder(folderId2));
}

void CMTMEngine::orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderSortOrder &sortOrder) const
{
    iCurrentFolderOrdering = sortOrder;
    qSort(folderIds.begin(), folderIds.end(), CMTMEngine::folderLessThan);
}

bool CMTMEngine::messageLessThan(const QMessage& message1, const QMessage& message2)
{
    CMTMEngine* pMTMEngine = mtmEngine();
    return QMessageSortOrderPrivate::lessThan(pMTMEngine->iCurrentMessageOrdering, message1, message2);
}

void CMTMEngine::orderMessages(QMessageIdList& messageIds, const QMessageSortOrder &sortOrder) const
{
    iCurrentMessageOrdering = sortOrder;
    QList<QMessage> messages;
    for (int i=0; i < messageIds.count(); i++) {
        messages.append(message(messageIds[i]));
    }
    qSort(messages.begin(), messages.end(), CMTMEngine::messageLessThan);
    messageIds.clear();
    for (int i=0; i < messages.count(); i++) {
        messageIds.append(messages[i].id());
    }
}

QMessageAccountIdList CMTMEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageAccountIdList accountIds;

    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err)
    
    QMessageAccountFilterPrivate* privateMessageAccountFilter = QMessageAccountFilterPrivate::implementation(filter);
    if (filter.isEmpty()) {
        if (!privateMessageAccountFilter->_notFilter) {
            // All accounts are returned for empty filter
            foreach (QMessageAccount value, iAccounts) {
                accountIds.append(value.id());
            }
        }
    } else {
        if (privateMessageAccountFilter->_valid) {
            foreach (QMessageAccount value, iAccounts) {
                if (privateMessageAccountFilter->filter(value)) {
                    accountIds.append(value.id());
                }
            }
        } else {
            foreach (QMessageAccount value, iAccounts) {
                if (privateMessageAccountFilter->filter(value)) {
                    accountIds.append(value.id());
                }
            }
        }
    }

    if (!sortOrder.isEmpty()) {
        orderAccounts(accountIds, sortOrder);
    }

    applyOffsetAndLimitToAccountIds(accountIds, offset, limit);
    
    return accountIds;
}

void CMTMEngine::applyOffsetAndLimitToAccountIds(QMessageAccountIdList& idList, int offset, int limit) const
{
    if (offset > 0) {
        if (offset > idList.count()) {
            idList.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                idList.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = idList.count()-1; i >= limit; i--) {
            idList.removeAt(i);
        }
    }
}

int CMTMEngine::countAccounts(const QMessageAccountFilter &filter) const
{
    return queryAccounts(filter, QMessageAccountSortOrder(), 0, 0).count();
}

QMessageAccount CMTMEngine::account(const QMessageAccountId &id) const
{
    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err)
    return iAccounts[id.toString()];
}

QMessageAccountId CMTMEngine::accountIdByServiceId(TMsvId serviceId) const
{
    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err)

    foreach (QMessageAccount value, iAccounts) {
        if (value.d_ptr->_service1EntryId == serviceId ||
            value.d_ptr->_service2EntryId == serviceId) {
            return value.id();
        }
    }

    return QMessageAccountId();
}

QMessageAccountId CMTMEngine::defaultAccount(QMessage::Type type) const
{
    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err)

    if (type == QMessage::Email) {
        // Email
        return idefaultEmailAccountId;
    } else {
        // Sms & Mms
        foreach (QMessageAccount value, iAccounts) {
            if ((value.messageTypes() & type) == (int)type) {
                return value.id();
            }
        }
    }

    return QMessageAccountId();
}

QMessageAccountIdList CMTMEngine::accountsByType(QMessage::Type type) const
{
    QMessageAccountIdList accountIds;
    
    foreach (QMessageAccount value, iAccounts) {
        if ((value.messageTypes() & type) == (int)type) {
            accountIds.append(value.id());
        }
    }
    
    return accountIds;
}

void CMTMEngine::updateEmailAccountsL() const
{
    QStringList keys = iAccounts.keys();
    keys.removeOne(iSMSAccountidAsString);
    keys.removeOne(iMMSAccountidAsString);
    
    CEmailAccounts* pEmailAccounts = CEmailAccounts::NewLC();

    // Default Email account will be the first Email account in the iAccounts list
    TSmtpAccount defaultAccount;
    TInt err = pEmailAccounts->DefaultSmtpAccountL(defaultAccount);    
    if (err == KErrNone) {
        QString idAsString;
        if (defaultAccount.iRelatedService != 0) {
            idAsString = SymbianHelpers::addIdPrefix(QString::number(defaultAccount.iRelatedService),SymbianHelpers::EngineTypeMTM);
        } else {
            idAsString = SymbianHelpers::addIdPrefix(QString::number(defaultAccount.iSmtpService),SymbianHelpers::EngineTypeMTM);
        }
        if (!iAccounts.contains(idAsString)) {
            QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(idAsString),
                                                                   QString::fromUtf16(defaultAccount.iSmtpAccountName.Ptr(), defaultAccount.iSmtpAccountName.Length()),
                                                                   defaultAccount.iRelatedService,
                                                                   defaultAccount.iSmtpService,
                                                                   QMessage::Email);
            iAccounts.insert(idAsString, account);
            idefaultEmailAccountId = account.id();
        } else {
            keys.removeOne(idAsString);
        }
    } else {
        idefaultEmailAccountId = QMessageAccountId();
    }

    RArray<TImapAccount> imapAccounts(10);
    pEmailAccounts->GetImapAccountsL(imapAccounts);
    CleanupClosePushL(imapAccounts);
    for (int i=0; i < imapAccounts.Count(); i++) {
        QString idAsString = SymbianHelpers::addIdPrefix(QString::number(imapAccounts[i].iImapService),SymbianHelpers::EngineTypeMTM);
        if (!iAccounts.contains(idAsString)) {
            QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(idAsString),
                                                                   QString::fromUtf16(imapAccounts[i].iImapAccountName.Ptr(), imapAccounts[i].iImapAccountName.Length()),
                                                                   imapAccounts[i].iImapService,
                                                                   imapAccounts[i].iSmtpService,
                                                                   QMessage::Email);
            iAccounts.insert(idAsString, account);
        } else {
            keys.removeOne(idAsString);
        }
    }
    CleanupStack::PopAndDestroy(&imapAccounts);
    
    RArray<TPopAccount> popAccounts(10);
    pEmailAccounts->GetPopAccountsL(popAccounts);
    CleanupClosePushL(popAccounts);
    for (int i=0; i < popAccounts.Count(); i++) {
        QString idAsString = SymbianHelpers::addIdPrefix(QString::number(popAccounts[i].iPopService),SymbianHelpers::EngineTypeMTM);
        if (!iAccounts.contains(idAsString)) {
            QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(SymbianHelpers::addIdPrefix(QString::number(popAccounts[i].iPopService),SymbianHelpers::EngineTypeMTM)),
                                                                   QString::fromUtf16(popAccounts[i].iPopAccountName.Ptr(), popAccounts[i].iPopAccountName.Length()),
                                                                   popAccounts[i].iPopService,
                                                                   popAccounts[i].iSmtpService,
                                                                   QMessage::Email);
            iAccounts.insert(idAsString, account);
        } else {
            keys.removeOne(idAsString);
        }
    }
    CleanupStack::PopAndDestroy(&popAccounts);

    RArray<TSmtpAccount> smtpAccounts(10);
    pEmailAccounts->GetSmtpAccountsL(smtpAccounts);
    CleanupClosePushL(smtpAccounts);
    for (int i=0; i < smtpAccounts.Count(); i++) {
        if (smtpAccounts[i].iRelatedService == 0) {
            QString idAsString = SymbianHelpers::addIdPrefix(QString::number(smtpAccounts[i].iSmtpService),SymbianHelpers::EngineTypeMTM);
            if (!iAccounts.contains(idAsString)) {
                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(SymbianHelpers::addIdPrefix(QString::number(smtpAccounts[i].iSmtpService),SymbianHelpers::EngineTypeMTM)),
                                                                       QString::fromUtf16(smtpAccounts[i].iSmtpAccountName.Ptr(), smtpAccounts[i].iSmtpAccountName.Length()),
                                                                       smtpAccounts[i].iRelatedService,
                                                                       smtpAccounts[i].iSmtpService,
                                                                       QMessage::Email);
                iAccounts.insert(idAsString, account);
            } else {
                keys.removeOne(idAsString);
            }
        }
    }
    CleanupStack::PopAndDestroy(&smtpAccounts);
    
    for (int i=0; i < keys.count(); i++) {
        iAccounts.remove(keys[i]);
    }
    
    CleanupStack::PopAndDestroy(pEmailAccounts);
}

TUid CMTMEngine::mtmUidByType(MTMType aMTMType)
{
    switch(aMTMType) {
    case CMTMEngine::MTMTypeSMS:  return KUidMsgTypeSMS;
    case CMTMEngine::MTMTypeMMS:  return KUidMsgTypeMultimedia;
    case CMTMEngine::MTMTypeSMTP: return KUidMsgTypeSMTP;
    case CMTMEngine::MTMTypeIMAP: return KUidMsgTypeIMAP4;
    case CMTMEngine::MTMTypePOP3: return KUidMsgTypePOP3;
    }
    
    return TUid();
}

CBaseMtm* CMTMEngine::mtmByUid(TUid aMTMUid)
{
    if (aMTMUid == KUidMsgTypeSMS) {
        return ipSmsMtm;
    } else if (aMTMUid == KUidMsgTypeMultimedia) {
        return ipMmsMtm;
    } else if (aMTMUid == KUidMsgTypeSMTP) {
        return ipSmtpMtm;
    } else if (aMTMUid == KUidMsgTypeIMAP4) {
        return ipImap4Mtm;
    } else if (aMTMUid == KUidMsgTypePOP3) {
        return ipPop3Mtm;
    }

    return 0;
}

TMsvId CMTMEngine::mtmServiceEntryIdL(MTMType aMTMType) const
{
    TUid mtmUID;
    switch(aMTMType) {
    case CMTMEngine::MTMTypeSMS:
        mtmUID = KUidMsgTypeSMS;
        break;
    case CMTMEngine::MTMTypeMMS:
        mtmUID = KUidMsgTypeMultimedia;
        break;
    case CMTMEngine::MTMTypeSMTP:
        mtmUID = KUidMsgTypeSMTP;
        break;
    case CMTMEngine::MTMTypeIMAP:
        mtmUID = KUidMsgTypeIMAP4;
        break;
    case CMTMEngine::MTMTypePOP3:
        mtmUID = KUidMsgTypePOP3;
        break;
    }
    
    CMsvEntry* pCurrentEntry = ipMsvSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(pCurrentEntry);
    
    // Sorting or grouping is not needed
    pCurrentEntry->SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone, ETrue));
    
    // Loop through child entries until MTM specific
    // root entry has been found 
    TMsvId mtmRootEntryId = KMsvRootIndexEntryId;
    TInt count = pCurrentEntry->Count();
    for(TInt i = 0; i<count; i++) {
        const TMsvEntry& child = (*pCurrentEntry)[i];
        if (child.iMtm == mtmUID) {
            mtmRootEntryId = child.Id();
            break;
        }
    }
    CleanupStack::PopAndDestroy(pCurrentEntry);
    
    if (mtmRootEntryId == KMsvRootIndexEntryId) {
        // MTM specific root entry was not found
        return 0;
    }
    
    TMsvEntry indexEntry;
    TMsvId mtmServiceEntryId;
    TInt error = ipMsvSession->GetEntry(mtmRootEntryId, mtmServiceEntryId, indexEntry);
    if (error != KErrNone) {
        // MTM specific service entry Id was not found
        return 0;
    }
    
    return mtmServiceEntryId;
}

bool CMTMEngine::switchToMTMRootEntry(MTMType aMTMType)
{
    TRAPD(err,
        CBaseMtm* pMTM = mtmByType(aMTMType);
        TMsvId mtmEntryId = mtmServiceEntryIdL(aMTMType);
        
        CMsvEntry* pMTMServiceEntry = ipMsvSession->GetEntryL(mtmEntryId);
        CleanupStack::PushL(pMTMServiceEntry);
        // Set service entry as current entry of ClientMTM class
        pMTM->SetCurrentEntryL(pMTMServiceEntry); // Takes ownership of pMTMServiceEntry
        CleanupStack::Pop(pMTMServiceEntry);
    );
    if (err != KErrNone)
        return false;
    else
        return true;
}

CBaseMtm* CMTMEngine::mtmByType(MTMType aMTMType)
{
    switch(aMTMType) {
    case CMTMEngine::MTMTypeSMS: return ipSmsMtm;
    case CMTMEngine::MTMTypeMMS: return ipMmsMtm;
    case CMTMEngine::MTMTypeSMTP: return ipSmtpMtm;
    case CMTMEngine::MTMTypeIMAP: return ipImap4Mtm;
    case CMTMEngine::MTMTypePOP3: return ipPop3Mtm;
    }
    
    return 0;
}

TMsvId CMTMEngine::standardFolderId(QMessage::StandardFolder standardFolder)
{
    switch(standardFolder) {
    case QMessage::InboxFolder:  return KMsvGlobalInBoxIndexEntryIdValue;
    case QMessage::OutboxFolder: return KMsvGlobalOutBoxIndexEntryIdValue;
    case QMessage::DraftsFolder: return KMsvDraftEntryIdValue;
    case QMessage::SentFolder:   return KMsvSentEntryIdValue;
    case QMessage::TrashFolder:  return KMsvDeletedEntryFolderEntryIdValue;
    }
    
    return TMsvId();
}

bool CMTMEngine::addMessage(QMessage* message)
{
    bool retVal = false;
    
    if (message->type() == QMessage::NoType) {
        if (message->parentAccountId().isValid()){    
            QMessageAccount account = QMessageAccount(message->parentAccountId());
            QMessage::TypeFlags types = account.messageTypes();
            if (types & QMessage::Sms) {
                message->setType(QMessage::Sms);
            } else if (types & QMessage::Mms) {
                message->setType(QMessage::Mms);
            } else if (types & QMessage::Email) {
                message->setType(QMessage::Email);
            }
        } else {
            return false;
        }
    }

    if (!message->parentAccountId().isValid() && message->type() != QMessage::NoType) {
        message->setParentAccountId(QMessageAccount::defaultAccount(message->type()));
    }
    
    if (message->type() == QMessage::Sms){
        retVal = storeSMS(*message);
    }
    else if (message->type() == QMessage::Mms){
        retVal = storeMMS(*message);
    }
    else if (message->type() == QMessage::Email){
        retVal = storeEmail(*message);
    }
    
    return retVal;
}

bool CMTMEngine::updateMessage(QMessage* m)
{
    bool retVal = true;
    
    if (m->parentAccountId().isValid()){    
        QMessageAccount account = QMessageAccount(m->parentAccountId());
        QMessage::TypeFlags types = account.messageTypes();
        if (types & QMessage::Sms){
            m->setType(QMessage::Sms);
        } else if (types & QMessage::Mms){
            m->setType(QMessage::Mms);
        } else if (types & QMessage::Email){
            m->setType(QMessage::Email);
        }
    } 
    
    if (m->type() == QMessage::Sms){
        TRAPD(err, updateSMSL(*m));
        if (err != KErrNone)
            retVal = false;
    } else if (m->type() == QMessage::Mms){
        TRAPD(err, updateMMSL(*m));
        if (err != KErrNone)
            retVal = false;
    }  else if (m->type() == QMessage::Email){
        TRAPD(err, updateEmailL(*m));
        if (err != KErrNone)
            retVal = false;
    }

    return retVal;
}

// Copy message to another folder.
void CMTMEngine::copyMessageL(TMsvId aMessageId, TMsvId aFolder)
{
    ipSmsMtm->SwitchCurrentEntryL(aMessageId);
    TMsvSelectionOrdering selection;
    selection.SetShowInvisibleEntries(ETrue);
    CMsvEntry* pParentEntry = CMsvEntry::NewL(ipSmsMtm->Session(),
                                             ipSmsMtm->Entry().Entry().Parent(),
                                             selection);
    CleanupStack::PushL(pParentEntry);
    // iSmsMtm points to the parent
    pParentEntry->CopyL(aMessageId, aFolder);
    CleanupStack::PopAndDestroy(); // parentEntry
}

bool CMTMEngine::removeMessage(const QMessageId &id, QMessageManager::RemovalOption option)
{
    if (!iSessionReady)
        return false;
    
    bool retVal = false;
    TRAPD(err, retVal = removeMessageL(id, option));
    if (err != KErrNone)
        retVal = false;
    
    return retVal;
}

bool CMTMEngine::showMessage(const QMessageId &id)
{
    if (!iSessionReady)
        return false;
    
    TRAPD(err, showMessageL(id));
    if (err != KErrNone)
        return false;
    else
        return true;
}

void CMTMEngine::showMessageL(const QMessageId &id)
{
    long int messageId = SymbianHelpers::stripIdPrefix(id.toString()).toLong();
    
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(pEntry);
    CBaseMtm* mtm = ipClientMtmReg->NewMtmL(pEntry->Entry().iMtm);
    CleanupStack::PushL(mtm);
    
    CMtmUiRegistry* mtmUiRegistry = CMtmUiRegistry::NewL(*ipMsvSession);
    CleanupStack::PushL(mtmUiRegistry); 

    CBaseMtmUi* ui = mtmUiRegistry->NewMtmUiL(*mtm); 
    CleanupStack::PushL(ui);
    
    ui->BaseMtm().SwitchCurrentEntryL(messageId);
    CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
    waiter->Start(); 
    CMsvOperation* op = ui->OpenL(waiter->iStatus);
    CleanupStack::PushL(op);
    
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(6); // op,waiter,ui,mtmuiregistry,mtm,pEntry
}

bool CMTMEngine::composeMessage(const QMessage &message)
{
    bool retVal = true;
    
    if (message.parentAccountId().isValid()){    
        QMessageAccount account = QMessageAccount(message.parentAccountId());
        QMessage::TypeFlags types = account.messageTypes();
        if (types & QMessage::Sms){
            TRAPD(err, retVal = composeSMSL(message));
            if (err != KErrNone)
                retVal = false;
        } else if (types & QMessage::Mms){
            TRAPD(err, retVal = composeMMSL(message));
            if (err != KErrNone)
                retVal = false;
        } else if (types & QMessage::Email){
            TRAPD(err, retVal = composeEmailL(message));
            if (err != KErrNone)
                retVal = false;
        }
    } else {
        if (message.type() == QMessage::Sms){
            TRAPD(err, retVal = composeSMSL(message));
            if (err != KErrNone)
                retVal = false;
        }
        else if (message.type() == QMessage::Mms){
            TRAPD(err, retVal = composeMMSL(message));
            if (err != KErrNone)
                retVal = false;
        }
        else if (message.type() == QMessage::Email){
            TRAPD(err, retVal = composeEmailL(message));
            if (err != KErrNone)
                retVal = false;
        }
    }
    
    return retVal;
}

bool CMTMEngine::composeSMSL(const QMessage &message)
{
    CSendUi *sendUi = CSendUi::NewL();
    CleanupStack::PushL(sendUi);
    CMessageData* messageData = CMessageData::NewLC();    
    // Add receivers
    QList<QMessageAddress> list(message.to());
    TPtrC16 receiver(KNullDesC);
    QString qreceiver;
    for (int i = 0; i < list.size(); ++i) {
        qreceiver = list.at(i).addressee();
        receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
        messageData->AppendToAddressL(receiver, KNullDesC);
    }
    // Set Body text
    QString body = message.textContent();
    TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
    HBufC* bd = msg.AllocLC();
    TPtr ptr(bd->Des());

    ipParaFormatLayer = CParaFormatLayer::NewL();
    ipCharFormatLayer = CCharFormatLayer::NewL();   
    ipRichText = CRichText::NewL(ipParaFormatLayer,ipCharFormatLayer);
    TInt pos = 0; // Insertion position of text will be zero
    ipRichText->Reset();
    ipRichText->InsertL(pos, ptr);
    messageData->SetBodyTextL(ipRichText);
    
    sendUi->CreateAndSendMessageL(KSenduiMtmSmsUid, messageData, KNullUid, ETrue);
    CleanupStack::PopAndDestroy(3); //bd, messageData and sendUi
    return true;
}

bool CMTMEngine::composeMMSL(const QMessage &message)
{
    CSendUi *sendUi = CSendUi::NewL();
    CleanupStack::PushL(sendUi);
    TUid bioTypeUid = KNullUid; 
    CMessageData* messageData = CMessageData::NewLC();
    
    // Add receivers
    QList<QMessageAddress> list(message.to());
    TPtrC16 receiver(KNullDesC);
    QString qreceiver;
    for (int i = 0; i < list.size(); ++i) {
        qreceiver = list.at(i).addressee();
        receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
        messageData->AppendToAddressL(receiver, KNullDesC);
    }
    
    // Set Subject
    QString subject = message.subject();
    TPtrC16 sbj(reinterpret_cast<const TUint16*>(subject.utf16()));
    messageData->SetSubjectL(&sbj); 
    
    QByteArray filePath;
    QMessageContentContainerIdList contentIds = message.contentIds();
    foreach (QMessageContentContainerId id, contentIds){
        QMessageContentContainer container = message.find(id);
        QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
        QString fileName = QString(filePath);
        QString body = container.textContent();
        if (!fileName.isEmpty()){ // content is attachment
            filePath.replace(QByteArray("/"), QByteArray("\\"));
            QString temp_path = QString(filePath);
            TPtrC16 attachmentPath(KNullDesC);
            attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
            messageData->AppendAttachmentL(attachmentPath);
        }
        else if (!body.isEmpty()) { // content is Body text
            TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
            HBufC* bd = msg.AllocLC();
            TPtr ptr(bd->Des());
            ipParaFormatLayer = CParaFormatLayer::NewL();
            ipCharFormatLayer = CCharFormatLayer::NewL();   
            ipRichText = CRichText::NewL(ipParaFormatLayer, ipCharFormatLayer);
            TInt pos = 0; // Insertion position of text will be zero
            ipRichText->Reset();
            ipRichText->InsertL(pos, ptr);
            messageData->SetBodyTextL(ipRichText);
            CleanupStack::PopAndDestroy(bd);
        }
    }
    QString messageBody = message.textContent();
    if (!messageBody.isEmpty()) { // no contents, only body text
        TPtrC16 msg(reinterpret_cast<const TUint16*>(messageBody.utf16()));
        HBufC* bd = msg.AllocLC();
        TPtr ptr(bd->Des());
        ipParaFormatLayer = CParaFormatLayer::NewL();
        ipCharFormatLayer = CCharFormatLayer::NewL();   
        ipRichText = CRichText::NewL(ipParaFormatLayer, ipCharFormatLayer);
        TInt pos = 0; // Insertion position of text will be zero
        ipRichText->Reset();
        ipRichText->InsertL(pos, ptr);
        messageData->SetBodyTextL(ipRichText);
        CleanupStack::PopAndDestroy(bd);
    }
      
    sendUi->CreateAndSendMessageL(KSenduiMtmMmsUid, messageData, KNullUid, ETrue);
    CleanupStack::PopAndDestroy(2); //messageData and sendUi
    return true;
}

bool CMTMEngine::composeEmailL(const QMessage &message)
{
    CSendUi *sendUi = CSendUi::NewL();
    CleanupStack::PushL(sendUi);
    CMessageData* messageData = CMessageData::NewLC();
    
    // Add receivers
    QList<QMessageAddress> list(message.to());
    TPtrC16 receiver(KNullDesC);
    QString qreceiver;
    for (int i = 0; i < list.size(); ++i) {
        qreceiver = list.at(i).addressee();
        receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
        messageData->AppendToAddressL(receiver, KNullDesC);
    }
    //cc
    QList<QMessageAddress> ccList(message.cc());
    TPtrC16 ccReceiver(KNullDesC);
    QString ccQreceiver;
    for (int i = 0; i < ccList.size(); ++i) {
        ccQreceiver = ccList.at(i).addressee();
        ccReceiver.Set(reinterpret_cast<const TUint16*>(ccQreceiver.utf16()));
        messageData->AppendCcAddressL(ccReceiver, KNullDesC);
    }
    //bcc
    QList<QMessageAddress> bccList(message.bcc());
    TPtrC16 bccReceiver(KNullDesC);
    QString bccQreceiver;
    for (int i = 0; i < bccList.size(); ++i) {
        bccQreceiver = bccList.at(i).addressee();
        bccReceiver.Set(reinterpret_cast<const TUint16*>(bccQreceiver.utf16()));
        messageData->AppendBccAddressL(bccReceiver, KNullDesC);
    }
    
    // Set Subject
    QString subject = message.subject();
    TPtrC16 sbj(reinterpret_cast<const TUint16*>(subject.utf16()));
    messageData->SetSubjectL(&sbj); 
    
    QByteArray filePath;
    QMessageContentContainerIdList contentIds = message.contentIds();
    foreach (QMessageContentContainerId id, contentIds){
        QMessageContentContainer container = message.find(id);
        QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
        QString body = container.textContent();
        QString fileName = QString(filePath);
        if (!fileName.isEmpty()){ // content is attachment
            filePath.replace(QByteArray("/"), QByteArray("\\"));
            QString temp_path = QString(filePath);
            TPtrC16 attachmentPath(KNullDesC);
            attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
            messageData->AppendAttachmentL(attachmentPath);
        }
        else if (!body.isEmpty()) { // content is Body text
            TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
            HBufC* bd = msg.AllocLC();
            TPtr ptr(bd->Des());
            ipParaFormatLayer = CParaFormatLayer::NewL();
            ipCharFormatLayer = CCharFormatLayer::NewL();   
            ipRichText = CRichText::NewL(ipParaFormatLayer, ipCharFormatLayer);
            TInt pos = 0; // Insertion position of text will be zero
            ipRichText->Reset();
            ipRichText->InsertL(pos, ptr);
            messageData->SetBodyTextL(ipRichText);
            CleanupStack::PopAndDestroy(bd);
        }
    }
    QString messageBody = message.textContent();
    if (!messageBody.isEmpty()) { // no contents, only body text
        TPtrC16 msg(reinterpret_cast<const TUint16*>(messageBody.utf16()));
        HBufC* bd = msg.AllocLC();
        TPtr ptr(bd->Des());
        ipParaFormatLayer = CParaFormatLayer::NewL();
        ipCharFormatLayer = CCharFormatLayer::NewL();   
        ipRichText = CRichText::NewL(ipParaFormatLayer, ipCharFormatLayer);
        TInt pos = 0; // Insertion position of text will be zero
        ipRichText->Reset();
        ipRichText->InsertL(pos, ptr);
        messageData->SetBodyTextL(ipRichText);
        CleanupStack::PopAndDestroy(bd);
    }
    
    sendUi->CreateAndSendMessageL(KSenduiMtmSmtpUid, messageData, KNullUid, ETrue);
    CleanupStack::PopAndDestroy(2); //messageData and sendUi
    return true;
}

bool CMTMEngine::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    TRAPD(err, retrieveL(messageId, id));
        if (err != KErrNone)
            return false;
        else
            return true;
}

void CMTMEngine::retrieveL(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    Q_UNUSED(id); // all attachments are retrieved (cannot retrieve only one)
    
    long int messId = SymbianHelpers::stripIdPrefix(messageId.toString()).toLong();
    
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(messId);
    CleanupStack::PushL(pEntry);
    
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();

    if (pEntry->Entry().iMtm == KUidMsgTypeIMAP4){    
        TPckgBuf<TInt> parameter;
        
        ipImap4Mtm->SwitchCurrentEntryL(pEntry->OwningService());
        
        sel->AppendL(messId);
        
        CMsvOperation* opConnect = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMConnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opConnect);
        wait->Start();
        CActiveScheduler::Start();    
        
        TImImap4GetPartialMailInfo info;
        info.iPartialMailOptions = EAttachmentsOnly;
        TPckg<TImImap4GetPartialMailInfo> bodyInfo(info);

        CMsvOperation* opPopulate = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMPopulate,
                                                    *sel, bodyInfo, wait->iStatus);
        CleanupStack::PushL(opPopulate);
        wait->Start();
        CActiveScheduler::Start();
        
        if (wait->iStatus.Int() != KErrNone) { 
            if (wait->iStatus.Int() == KErrNotFound){
                // TODO: set messagestatus removed
            }
        }
        
        CMsvOperation* opDisconnect = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opDisconnect);
        
        wait->Start();
        CActiveScheduler::Start();    
        
        CleanupStack::PopAndDestroy(3);
    }
    
    if (pEntry->Entry().iMtm == KUidMsgTypePOP3){
        TPckgBuf<TInt> parameter;
        
        ipPop3Mtm->SwitchCurrentEntryL(pEntry->OwningService());    

        sel->AppendL(pEntry->EntryId());
        
        CMsvOperation* opConnect = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMConnect,
                                                    *sel, parameter, wait->iStatus);
        
        CleanupStack::PushL(opConnect);
        wait->Start();
        CActiveScheduler::Start();    
        
        CImPop3Settings *popSettings = new (ELeave) CImPop3Settings;
        CleanupStack::PushL(popSettings);
        CEmailAccounts *emailAccounts = CEmailAccounts::NewLC();
        TPopAccount account;
        emailAccounts->GetPopAccountL(pEntry->OwningService(), account);
        emailAccounts->LoadPopSettingsL(account, *popSettings);
        // cannot retrieve only attachment, have to retrieve entire message
        popSettings->SetGetMailOptions(EGetPop3EmailMessages);
        emailAccounts->SavePopSettingsL(account,*popSettings);
        CleanupStack::PopAndDestroy(emailAccounts);
        CleanupStack::PopAndDestroy(popSettings);

        CMsvOperation* opPopulate = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMPopulate,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opPopulate);
        wait->Start();
        CActiveScheduler::Start();
        
        CMsvOperation* opDisconnect = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMDisconnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opDisconnect);
        
        wait->Start();
        CActiveScheduler::Start();    
        CleanupStack::PopAndDestroy(3);
        
    }
    
    CleanupStack::PopAndDestroy(wait);
    CleanupStack::PopAndDestroy(sel);    
    CleanupStack::PopAndDestroy(pEntry);
}

bool CMTMEngine::retrieveBody(const QMessageId& id)
{
    TRAPD(err, retrieveBodyL(id));
        if (err != KErrNone)
            return false;
        else
            return true;
}

void CMTMEngine::retrieveBodyL(const QMessageId& id) const
{
    long int messageId = SymbianHelpers::stripIdPrefix(id.toString()).toLong();
    
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(pEntry);
    
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();

    if (pEntry->Entry().iMtm == KUidMsgTypeIMAP4){    
        TPckgBuf<TInt> parameter;
        
        ipImap4Mtm->SwitchCurrentEntryL(pEntry->OwningService());
        
        sel->AppendL(messageId);
        
        CMsvOperation* opConnect = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMConnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opConnect);
        wait->Start();
        CActiveScheduler::Start();    
        
        TImImap4GetPartialMailInfo info;
        info.iPartialMailOptions = EBodyTextOnly;
        TPckg<TImImap4GetPartialMailInfo> bodyInfo(info);

        CMsvOperation* opPopulate = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMPopulate,
                                                    *sel, bodyInfo, wait->iStatus);
        CleanupStack::PushL(opPopulate);
        wait->Start();
        CActiveScheduler::Start();
        
        if (wait->iStatus.Int() != KErrNone) { 
            if (wait->iStatus.Int() == KErrNotFound){
                // TODO: set messagestatus removed
            }
        }
        
        CMsvOperation* opDisconnect = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opDisconnect);
        
        wait->Start();
        CActiveScheduler::Start();    
        
        CleanupStack::PopAndDestroy(3);
    }
    
    if (pEntry->Entry().iMtm == KUidMsgTypePOP3){
        TPckgBuf<TInt> parameter;
        
        ipPop3Mtm->SwitchCurrentEntryL(pEntry->OwningService());    

        sel->AppendL(pEntry->EntryId());
        
        CMsvOperation* opConnect = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMConnect,
                                                    *sel, parameter, wait->iStatus);
        
        CleanupStack::PushL(opConnect);
        wait->Start();
        CActiveScheduler::Start();    
        
        CImPop3Settings *popSettings = new (ELeave) CImPop3Settings;
        CleanupStack::PushL(popSettings);
        CEmailAccounts *emailAccounts = CEmailAccounts::NewLC();
        TPopAccount account;
        emailAccounts->GetPopAccountL(pEntry->OwningService(), account);
        emailAccounts->LoadPopSettingsL(account, *popSettings);
        // cannot retrieve only body, have to retrieve entire message
        popSettings->SetGetMailOptions(EGetPop3EmailMessages);
        emailAccounts->SavePopSettingsL(account,*popSettings);
        CleanupStack::PopAndDestroy(emailAccounts);
        CleanupStack::PopAndDestroy(popSettings);

        CMsvOperation* opPopulate = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMPopulate,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opPopulate);
        wait->Start();
        CActiveScheduler::Start();
        
        CMsvOperation* opDisconnect = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMDisconnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opDisconnect);
        
        wait->Start();
        CActiveScheduler::Start();    
        CleanupStack::PopAndDestroy(3);
        
    }
    
    CleanupStack::PopAndDestroy(wait);
    CleanupStack::PopAndDestroy(sel);    
    CleanupStack::PopAndDestroy(pEntry);
}

bool CMTMEngine::retrieveHeader(const QMessageId& id)
{
    TRAPD(err, retrieveHeaderL(id));
        if (err != KErrNone)
            return false;
        else
            return true;
}

void CMTMEngine::retrieveHeaderL(const QMessageId& id) const
{
    long int messageId = SymbianHelpers::stripIdPrefix(id.toString()).toLong();
    
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(pEntry);
    
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();

    if (pEntry->Entry().iMtm == KUidMsgTypeIMAP4){    
        TPckgBuf<TInt> parameter;
        
        ipImap4Mtm->SwitchCurrentEntryL(pEntry->OwningService());
        
        sel->AppendL(messageId);
        
        CMsvOperation* opConnect = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMConnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opConnect);
        wait->Start();
        CActiveScheduler::Start();    
            
        TImImap4GetMailInfo info;
        info.iGetMailBodyParts = EGetImap4EmailHeaders;
        TPckg<TImImap4GetMailInfo> bodyInfo(info);
        
        CMsvOperation* opPopulate = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMPopulate,
                                                    *sel, bodyInfo, wait->iStatus);
        CleanupStack::PushL(opPopulate);
        wait->Start();
        CActiveScheduler::Start();
        
        CMsvOperation* opDisconnect = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opDisconnect);
        
        wait->Start();
        CActiveScheduler::Start();    
        
        CleanupStack::PopAndDestroy(3);
    }
    
    if (pEntry->Entry().iMtm == KUidMsgTypePOP3){
        TPckgBuf<TInt> parameter;
        
        ipPop3Mtm->SwitchCurrentEntryL(pEntry->OwningService());    

        sel->AppendL(pEntry->EntryId());
        
        CMsvOperation* opConnect = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMConnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opConnect);
        wait->Start();
        CActiveScheduler::Start();    
        
        CImPop3Settings *popSettings = new (ELeave) CImPop3Settings;
        CleanupStack::PushL(popSettings);
        CEmailAccounts *emailAccounts = CEmailAccounts::NewLC();
        TPopAccount account;
        emailAccounts->GetPopAccountL(pEntry->OwningService(), account);
        emailAccounts->LoadPopSettingsL(account, *popSettings);
        popSettings->SetGetMailOptions(EGetPop3EmailHeaders);
        emailAccounts->SavePopSettingsL(account,*popSettings);
        CleanupStack::PopAndDestroy(emailAccounts);
        CleanupStack::PopAndDestroy(popSettings);

        CMsvOperation* opPopulate = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMPopulate,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opPopulate);
        wait->Start();
        CActiveScheduler::Start();
        
        CMsvOperation* opDisconnect = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMDisconnect,
                                                    *sel, parameter, wait->iStatus);
        CleanupStack::PushL(opDisconnect);
        
        wait->Start();
        CActiveScheduler::Start();    
        
        CleanupStack::PopAndDestroy(3);
        
    }
    
    CleanupStack::PopAndDestroy(wait);
    CleanupStack::PopAndDestroy(sel);    
    CleanupStack::PopAndDestroy(pEntry);
}

bool CMTMEngine::exportUpdates(const QMessageAccountId &id)
{
    TRAPD(err, exportUpdatesL(id));
    if (err != KErrNone) {
        return false;
    } else {
        return true;
    }
}

void CMTMEngine::exportUpdatesL(const QMessageAccountId &id) const
{
    QMessageAccount account = this->account(id);
    CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(account.d_ptr->_service1EntryId);
    if (!pEntry) {
        User::Leave(KErrNotFound);
    }
    if (pEntry->Entry().iMtm == KUidMsgTypeIMAP4) {
        TPckgBuf<TInt> parameter;
        CMsvEntrySelection* pMsvEntrySelection = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(pMsvEntrySelection);
        CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
        
        ipImap4Mtm->SwitchCurrentEntryL(account.d_ptr->_service1EntryId);
        pMsvEntrySelection->AppendL(account.d_ptr->_service1EntryId);
        
        CMsvOperation* pMsvOperation = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMConnect, *pMsvEntrySelection,
                                                                        parameter, pMsvOperationWait->iStatus);
        CleanupStack::PushL(pMsvOperation);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy(pMsvOperation);

        pMsvOperation = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMFullSync, *pMsvEntrySelection,
                                                         parameter, pMsvOperationWait->iStatus);
        CleanupStack::PushL(pMsvOperation);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy(pMsvOperation);
        
        pMsvOperation = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect, *pMsvEntrySelection,
                                                         parameter, pMsvOperationWait->iStatus);
        CleanupStack::PushL(pMsvOperation);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy(pMsvOperation);
        
        CleanupStack::PopAndDestroy(pMsvOperationWait);
        CleanupStack::PopAndDestroy(pMsvEntrySelection);
    }
    releaseCMsvEntryAndPopFromCleanupStack(pEntry);
}

bool CMTMEngine::removeMessageL(const QMessageId &id, QMessageManager::RemovalOption /*option*/)
{
    long int messageId = SymbianHelpers::stripIdPrefix(id.toString()).toLong();
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(pEntry);
    
    if (pEntry->Entry().iMtm == KUidMsgTypeSMS) {
        if (!ipSmsMtm)
            return false;
        ipSmsMtm->SwitchCurrentEntryL(messageId);
        TMsvId parent = ipSmsMtm->Entry().Entry().Parent();
        ipSmsMtm->SwitchCurrentEntryL(parent);
        ipSmsMtm->Entry().DeleteL(messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypeMultimedia) {
        if (!ipMmsMtm)
            return false;
        ipMmsMtm->SwitchCurrentEntryL(messageId);
        TMsvId parent = ipMmsMtm->Entry().Entry().Parent();
        ipMmsMtm->SwitchCurrentEntryL(parent);
        ipMmsMtm->Entry().DeleteL(messageId);
    }  else if (pEntry->Entry().iMtm == KUidMsgTypeSMTP) {
        if (!ipSmtpMtm)
            return false;
        ipSmtpMtm->SwitchCurrentEntryL(messageId);
        TMsvId parent = ipSmtpMtm->Entry().Entry().Parent();
        ipSmtpMtm->SwitchCurrentEntryL(parent);
        ipSmtpMtm->Entry().DeleteL(messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypeIMAP4) {
        if (!ipImap4Mtm)
            return false;
        ipImap4Mtm->SwitchCurrentEntryL(messageId);
        TMsvId parent = ipImap4Mtm->Entry().Entry().Parent();
        ipImap4Mtm->SwitchCurrentEntryL(parent);
        CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
        CMsvOperation* pMsvOperation = ipImap4Mtm->Entry().DeleteL(messageId, pMsvOperationWait->iStatus);
        CleanupStack::PushL(pMsvOperation);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy(pMsvOperation);
        CleanupStack::PopAndDestroy(pMsvOperationWait);
    } else if (pEntry->Entry().iMtm == KUidMsgTypePOP3) {
        if (!ipPop3Mtm)
            return false;
        ipPop3Mtm->SwitchCurrentEntryL(messageId);
        TMsvId parent = ipPop3Mtm->Entry().Entry().Parent();
        ipPop3Mtm->SwitchCurrentEntryL(parent);
        CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
        CMsvOperation* pMsvOperation = ipPop3Mtm->Entry().DeleteL(messageId, pMsvOperationWait->iStatus);
        CleanupStack::PushL(pMsvOperation);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy(pMsvOperation);
        CleanupStack::PopAndDestroy(pMsvOperationWait);
    } 
    
    CleanupStack::PopAndDestroy(pEntry);

    return true;
}

bool CMTMEngine::removeMessages(const QMessageFilter& /*filter*/, QMessageManager::RemovalOption /*option*/)
{
    return false;
}

void CMTMEngine::handleNestedFiltersFromMessageFilter(QMessageFilter &filter) const
{
    QMessageFilterPrivate* pMFFilter = QMessageFilterPrivate::implementation(filter);
    if (pMFFilter->_filterList.count() > 0) {
        int filterListCount = pMFFilter->_filterList.count();
        for (int i=0; i < filterListCount; i++) {
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                QMessageFilterPrivate* pMFFilter2 = QMessageFilterPrivate::implementation(pMFFilter->_filterList[i][j]);
                if (pMFFilter2->_field == QMessageFilterPrivate::ParentAccountIdFilter) {
                    QMessageAccountIdList accountIds = queryAccounts(*pMFFilter2->_accountFilter, QMessageAccountSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (accountIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < accountIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < accountIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_accountFilter;
                        pMFFilter2->_accountFilter = 0;
                        pMFFilter2->_field = QMessageFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                    }
                } else if (pMFFilter2->_field == QMessageFilterPrivate::ParentFolderIdFilter) { 
                    QMessageFolderIdList folderIds = queryFolders(*pMFFilter2->_folderFilter, QMessageFolderSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (folderIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < folderIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < folderIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < folderIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < folderIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_folderFilter;
                        pMFFilter2->_folderFilter = 0;
                        pMFFilter2->_field = QMessageFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                    }
                } else {
                    break;
                }
            }
        }
    } else {
        if (pMFFilter->_field == QMessageFilterPrivate::ParentAccountIdFilter) {
            QMessageAccountIdList accountIds = queryAccounts(*pMFFilter->_accountFilter, QMessageAccountSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (accountIds.count() > 0) {
                for (int i=0; i < accountIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_accountFilter;
                        pMFFilter->_accountFilter = 0;
                        pMFFilter->_field = QMessageFilterPrivate::ParentAccountId;
                        pMFFilter->_value = accountIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFilter::byParentAccountId(accountIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFilter::byParentAccountId(accountIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_accountFilter;
                pMFFilter->_accountFilter = 0;
                pMFFilter->_field = QMessageFilterPrivate::Id;
            }
        } else if (pMFFilter->_field == QMessageFilterPrivate::ParentFolderIdFilter) {
            QMessageFolderIdList folderIds = queryFolders(*pMFFilter->_folderFilter, QMessageFolderSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (folderIds.count() > 0) {
                for (int i=0; i < folderIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_folderFilter;
                        pMFFilter->_folderFilter = 0;
                        pMFFilter->_field = QMessageFilterPrivate::ParentFolderId;
                        pMFFilter->_value = folderIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFilter::byParentFolderId(folderIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFilter::byParentFolderId(folderIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_folderFilter;
                pMFFilter->_folderFilter = 0;
                pMFFilter->_field = QMessageFilterPrivate::Id;
            }
        }
    }
}

bool CMTMEngine::queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    TRAPD(err, queryMessagesL(privateService, filter, sortOrder, limit, offset));
    if (err != KErrNone) {
        return false;
    }
    return true;
}

void CMTMEngine::queryMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    MessageQueryInfo queryInfo;
    queryInfo.operationId = ++iOperationIds;
    if (queryInfo.operationId == 100000) {
        queryInfo.operationId = 1;
    }
    queryInfo.isQuery = true;
    queryInfo.filter = filter;
    queryInfo.sortOrder = sortOrder;
    queryInfo.offset = offset;
    queryInfo.limit = limit;
    queryInfo.findOperation = new CMessagesFindOperation((CMTMEngine&)*this, ipMsvSession, queryInfo.operationId);
    queryInfo.privateService = &privateService;
    queryInfo.currentFilterListIndex = 0;
    iMessageQueries.append(queryInfo);
    
    handleNestedFiltersFromMessageFilter(iMessageQueries[iMessageQueries.count()-1].filter);
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(iMessageQueries[iMessageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(iMessageQueries[iMessageQueries.count()-1].filter,
                                                        iMessageQueries[iMessageQueries.count()-1].sortOrder);
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0], iMessageQueries[iMessageQueries.count()-1].sortOrder);
    }
}

bool CMTMEngine::queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    TRAPD(err, queryMessagesL(privateService, filter, body, matchFlags, sortOrder, limit, offset));
    if (err != KErrNone) {
        return false;
    }
    return true;
}

void CMTMEngine::queryMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    MessageQueryInfo queryInfo;
    queryInfo.operationId = ++iOperationIds;
    if (queryInfo.operationId == 100000) {
        queryInfo.operationId = 1;
    }
    queryInfo.isQuery = true;
    queryInfo.body = body;
    queryInfo.matchFlags = matchFlags;
    queryInfo.filter = filter;
    queryInfo.sortOrder = sortOrder;
    queryInfo.offset = offset;
    queryInfo.limit = limit;
    queryInfo.findOperation = new CMessagesFindOperation((CMTMEngine&)*this, ipMsvSession, queryInfo.operationId);
    queryInfo.privateService = &privateService;
    queryInfo.currentFilterListIndex = 0;
    iMessageQueries.append(queryInfo);
    
    handleNestedFiltersFromMessageFilter(iMessageQueries[iMessageQueries.count()-1].filter);
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(iMessageQueries[iMessageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(iMessageQueries[iMessageQueries.count()-1].filter,
                                                        iMessageQueries[iMessageQueries.count()-1].sortOrder,
                                                        body,
                                                        matchFlags);
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0],
                                                        iMessageQueries[iMessageQueries.count()-1].sortOrder,
                                                        body,
                                                        matchFlags);
    }
}

bool CMTMEngine::countMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter)
{
    TRAPD(err, countMessagesL(privateService, filter));
    if (err != KErrNone) {
        return false;
    }
    return true;
}

void CMTMEngine::countMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter)
{
    MessageQueryInfo queryInfo;
    queryInfo.operationId = ++iOperationIds;
    if (queryInfo.operationId == 100000) {
        queryInfo.operationId = 1;
    }
    queryInfo.isQuery = false;
    queryInfo.matchFlags = 0;
    queryInfo.filter = filter;
    queryInfo.sortOrder = QMessageSortOrder();
    queryInfo.offset = 0;
    queryInfo.limit = 0;
    queryInfo.findOperation = new CMessagesFindOperation((CMTMEngine&)*this, ipMsvSession, queryInfo.operationId);
    queryInfo.privateService = &privateService;
    queryInfo.currentFilterListIndex = 0;
    queryInfo.count = 0;
    iMessageQueries.append(queryInfo);
    
    handleNestedFiltersFromMessageFilter(iMessageQueries[iMessageQueries.count()-1].filter);
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(iMessageQueries[iMessageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(iMessageQueries[iMessageQueries.count()-1].filter,
                                                        iMessageQueries[iMessageQueries.count()-1].sortOrder);
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0], iMessageQueries[iMessageQueries.count()-1].sortOrder);
    }
}

void CMTMEngine::filterAndOrderMessagesReady(bool success, int operationId, QMessageIdList ids, int numberOfHandledFilters,
                                             bool resultSetOrdered)
{
    int index=0;
    for (; index < iMessageQueries.count(); index++) {
        if (iMessageQueries[index].operationId == operationId) {
            break;
        }
    }

    if (success) {
        // If there are unhandled filters, loop through all filters and do filtering for ids using unhandled filters.
        QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(iMessageQueries[index].filter);
        if (pf->_filterList.count() > 0) {
            if (pf->_filterList[iMessageQueries[index].currentFilterListIndex].count() > numberOfHandledFilters) {
                for (int i=0; i < ids.count(); i++) {
                    QMessage msg = message(ids[i]);
                    for (int j=numberOfHandledFilters; j < pf->_filterList[iMessageQueries[index].currentFilterListIndex].count(); j++) {
                        QMessageFilterPrivate* pf2 = QMessageFilterPrivate::implementation(pf->_filterList[iMessageQueries[index].currentFilterListIndex][j]);
                        if (!pf2->filter(msg)) {
                            ids.removeAt(i);
                            i--;
                            break;
                        }
                    }
                }
            }
        }

        if (pf->_filterList.count() > 0) {
            // Filter contains filterlist (or filterlists), not just one single filter 
            if (iMessageQueries[index].currentFilterListIndex == 0) {
                iMessageQueries[index].ids << ids;
                iMessageQueries[index].count = ids.count(); 
            } else {
                // Append new ids to resultset
                for (int i=0; i < ids.count(); i++) {
                    if (!iMessageQueries[index].ids.contains(ids[i])) {
                        iMessageQueries[index].ids.append(ids[i]);
                        iMessageQueries[index].count++;; 
                    }
                }
            }
            
            iMessageQueries[index].currentFilterListIndex++;
            if (iMessageQueries[index].currentFilterListIndex < pf->_filterList.count()) {
                // There are still unhandled filter lists left
                iMessageQueries[index].findOperation->filterAndOrderMessages(pf->_filterList[iMessageQueries[index].currentFilterListIndex],
                                                                             iMessageQueries[index].sortOrder,
                                                                             iMessageQueries[index].body,
                                                                             iMessageQueries[index].matchFlags);
                return;
            } else {
                // All filters successfully handled
                if (iMessageQueries[index].isQuery) {
                    if (!iMessageQueries[index].sortOrder.isEmpty()) {
                        // Make sure that messages are correctly ordered
                        orderMessages(iMessageQueries[index].ids, iMessageQueries[index].sortOrder);
                    }
                    applyOffsetAndLimitToMsgIds(iMessageQueries[index].ids,
                                                iMessageQueries[index].offset,
                                                iMessageQueries[index].limit);
                    iMessageQueries[index].privateService->messagesFound(iMessageQueries[index].ids, true, true);

                    //emit iMessageQueries[index].privateService->messagesFound(iMessageQueries[index].ids);
                } else {
                    emit iMessageQueries[index].privateService->messagesCounted(iMessageQueries[index].count);
                }
                iMessageQueries[index].privateService->_active = false;
                emit iMessageQueries[index].privateService->stateChanged(QMessageService::FinishedState);
            }
        } else {
            // There was only one single filter to handle
            if (numberOfHandledFilters == 0) {
                // The one and only filter was not handled
                // => Do filtering for all returned messages
                for (int i=ids.count()-1; i >= 0; i--) {
                    QMessage msg = message(ids[i]);
                    if (!pf->filter(msg)) {
                        ids.removeAt(i);
                    }
                }
            }
            // => All filters successfully handled
            if (iMessageQueries[index].isQuery) {
                // Make sure that messages are correctly ordered
                if (!iMessageQueries[index].sortOrder.isEmpty() && !resultSetOrdered) {
                    orderMessages(ids, iMessageQueries[index].sortOrder);
                }
                // Handle offest & limit
                applyOffsetAndLimitToMsgIds(ids, iMessageQueries[index].offset, iMessageQueries[index].limit);
                //emit iMessageQueries[index].privateService->messagesFound(ids);
                iMessageQueries[index].privateService->messagesFound(ids, true, true);
            } else {
                emit iMessageQueries[index].privateService->messagesCounted(ids.count());
            }
            iMessageQueries[index].privateService->_active = false;
            emit iMessageQueries[index].privateService->stateChanged(QMessageService::FinishedState);
        }
    } else {
        iMessageQueries[index].privateService->_active = false;
        if (iMessageQueries[index].privateService->_error == QMessageManager::NoError) {
            iMessageQueries[index].privateService->_error = QMessageManager::RequestIncomplete;
        }
        emit iMessageQueries[index].privateService->stateChanged(QMessageService::FinishedState);
    }

    delete iMessageQueries[index].findOperation;
    iMessageQueries.removeAt(index);
}


void CMTMEngine::applyOffsetAndLimitToMsgIds(QMessageIdList& idList, int offset, int limit) const
{
    if (offset > 0) {
        if (offset > idList.count()) {
            idList.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                idList.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = idList.count()-1; i >= limit; i--) {
            idList.removeAt(i);
        }
    }
}

QMessageFolderIdList CMTMEngine::filterMessageFolders(const QMessageFolderFilter& filter, bool& filterHandled) const
{
    QMessageFolderIdList ids;
    TRAPD(err, ids = filterMessageFoldersL(filter, filterHandled));
    Q_UNUSED(err)
    return ids;
}

QMessageFolderIdList CMTMEngine::filterMessageFoldersL(const QMessageFolderFilter& filter, bool& filterHandled) const
{
    filterHandled = false;
    QMessageFolderIdList ids;
    
    if (filter.isEmpty()) {
        QMessageFolderFilterPrivate* pf = QMessageFolderFilterPrivate::implementation(filter);
        if (!pf->_notFilter) {
            ids = allFolders();
        }
        filterHandled = true;
    } else {
        QMessageFolderFilterPrivate* pf = QMessageFolderFilterPrivate::implementation(filter);
        if (!pf->_valid) {
            return QMessageFolderIdList();
        }
    
        switch (pf->_field) {
        case QMessageFolderFilterPrivate::Id:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (pf->_value.toString().length() > QString(SymbianHelpers::mtmPrefix).length()) {
                    bool folderOk = false;
                    long int folderId = folderIdFromQMessageFolderId(QMessageFolderId(pf->_value.toString()));
                    CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(folderId);
                    if (pEntry) {
                        if (pEntry->Entry().iType == KUidMsvFolderEntry) {
                            folderOk = true;
                        }
                        releaseCMsvEntryAndPopFromCleanupStack(pEntry);
                    }
                    if (cmp == QMessageDataComparator::Equal) {
                        if (folderOk) {
                            ids.append(QMessageFolderId(pf->_value.toString()));
                        }
                    } else { // NotEqual
                        ids = allFolders();
                        if (folderOk) {
                            ids.removeOne(QMessageFolderId(pf->_value.toString()));
                        }
                    }
                } else {
                    if (cmp == QMessageDataComparator::NotEqual) {
                        ids = allFolders();
                    }
                }
                filterHandled = true;
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (pf->_ids.count() > 0) { // QMessageIdList
                    QMessageFolderIdList ids2;
                    for (int i=0; i < pf->_ids.count(); i++) {
                        long int folderId = folderIdFromQMessageFolderId(QMessageFolderId(pf->_ids[i].toString()));
                        CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(folderId);
                        if (pEntry) {
                            const TMsvEntry& entry = pEntry->Entry();
                            if (entry.iType == KUidMsvFolderEntry) {
                                ids2.append(pf->_ids[i]);
                            }
                            releaseCMsvEntryAndPopFromCleanupStack(pEntry);
                        }
                    }
                    if (cmp == QMessageDataComparator::Includes) {
                        ids << ids2;
                    } else { // Excludes
                        ids = allFolders();
                        for (int i=0; i < ids2.count(); i++) {
                            ids.removeOne(ids2[i]);
                        }
                    }
                    filterHandled = true;
                } else {
                    // Empty QMessageIdList as a list
                    if (cmp == QMessageDataComparator::Excludes) {
                        ids = allFolders();
                    }
                    filterHandled = true;
                
                    // QMessageFilter 
                    /*if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }*/
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::Name:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    // TODO:
                } else { // NotEqual
                    // TODO:
                }
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    if (pf->_value.toString().isEmpty() || pf->_value.toString().length() == 0) {
                        filterHandled = true;
                    }
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::Path:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    // TODO:
                } else { // NotEqual
                    // TODO:
                }
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    if (pf->_value.toString().isEmpty() || pf->_value.toString().length() == 0) {
                        filterHandled = true;
                    }
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::ParentAccountId:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    if (pf->_value.toString().length() > QString(SymbianHelpers::mtmPrefix).length()) {
                        ids = folderIdsByAccountId(QMessageAccountId(pf->_value.toString()));
                    }
                } else { // NotEqual
                    ids = allFolders();
                    if (pf->_value.toString().length() > QString(SymbianHelpers::mtmPrefix).length()) {
                        QMessageFolderIdList ids2 = folderIdsByAccountId(QMessageAccountId(pf->_value.toString()));
                        for (int i = 0; i < ids2.count(); i++) {
                            ids.removeOne(ids2[i]);
                        }
                    }
                }
                filterHandled = true;
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::ParentFolderId:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    long int folderId = pf->_value.toString().toLong();
                    CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(folderId);
                    if (pEntry) {
                        const TMsvEntry& entry = pEntry->Entry();
                        if (entry.iType == KUidMsvFolderEntry) {
                            CMsvEntrySelection* pSelection = pEntry->ChildrenWithTypeL(KUidMsvFolderEntry);
                            CleanupStack::PushL(pSelection);
                            if (pSelection->Count() > 0) {
                                for(TInt i = 0; i < pSelection->Count(); i++) {
                                    ids.append(QMessageFolderId(SymbianHelpers::addIdPrefix(QString::number(pSelection->At(i)),SymbianHelpers::EngineTypeMTM)));
                                }
                            }
                            CleanupStack::PopAndDestroy(pSelection);
                        }
                        releaseCMsvEntryAndPopFromCleanupStack(pEntry);
                    }
                } else { // NotEqual
                    // TODO:
                }
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::AncestorFolderIds:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (!pf->_value.isNull()) { // QMessageFolderId
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                } else { // QMessageFolderFilter
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                }
            }
            break;
            }
    	case QMessageFolderFilterPrivate::ParentAccountIdFilter:
        case QMessageFolderFilterPrivate::None:
            break;        
        }
    }
    
    if (!filterHandled) {
        ids = allFolders();
    }

    return ids;
}

QMessageFolderIdList CMTMEngine::allFolders() const
{
    QMessageFolderIdList ids;
    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err)
    foreach (QMessageAccount value, iAccounts) {
        QMessageFolderIdList ids2 = folderIdsByAccountId(value.id());
        ids << ids2;
    }
    return ids;
}

QMessageFolderIdList CMTMEngine::folderIdsByAccountId(const QMessageAccountId& accountId) const
{
    QMessageAccount messageAccount = account(accountId);
    QMessageFolderIdList ids = folderIdsByServiceEntryId(messageAccount.d_ptr->_service1EntryId,
                                                         messageAccount.d_ptr->_service1EntryId);
    if (messageAccount.d_ptr->_service2EntryId) {
        QMessageFolderIdList ids2 = folderIdsByServiceEntryId(messageAccount.d_ptr->_service2EntryId,
                                                              messageAccount.d_ptr->_service1EntryId);
        ids << ids2;
    }
    return ids;
}

QMessageFolderIdList CMTMEngine::folderIdsByServiceEntryId(const TMsvId& serviceEntryId,
                                                           const TMsvId& folderServiceEntryId) const
{
    QMessageFolderIdList ids;

    CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(serviceEntryId);
    if (pEntry) {
        const TMsvEntry& entry = pEntry->Entry();
        if (entry.iMtm == KUidMsgTypeSMS || entry.iMtm == KUidMsgTypeMultimedia || entry.iMtm == KUidMsgTypeSMTP) {
            // Add all Standard Folders to FolderIdList
            if (entry.iMtm != KUidMsgTypeSMTP) {
                // SMTP is for sending only => Don't add Inbox for SMTP
                ids.append(createQMessageFolderId(folderServiceEntryId, KMsvGlobalInBoxIndexEntryId));
            }
            ids.append(createQMessageFolderId(folderServiceEntryId, KMsvGlobalOutBoxIndexEntryId));
            ids.append(createQMessageFolderId(folderServiceEntryId, KMsvDraftEntryId));
            ids.append(createQMessageFolderId(folderServiceEntryId, KMsvSentEntryId));
            ids.append(createQMessageFolderId(folderServiceEntryId, KMsvDeletedEntryFolderEntryId));
            pEntry->SetEntryL(KDocumentsEntryIdValue);
        }
    
        if (entry.iMtm == KUidMsgTypePOP3) {
            ids.append(createQMessageFolderId(folderServiceEntryId, serviceEntryId));
        } else {
            CMsvEntryFilter* pFilter = CMsvEntryFilter::NewLC();
            pFilter->SetService(serviceEntryId);
            pFilter->SetType(KUidMsvFolderEntry);
            CMsvEntrySelection* pSelection = new(ELeave) CMsvEntrySelection;
            CleanupStack::PushL(pSelection);
            ipMsvSession->GetChildIdsL(pEntry->Entry().Id(), *pFilter, *pSelection);
            if (pSelection->Count() > 0) {
                for(TInt i = 0; i < pSelection->Count(); i++) {
                    ids.append(createQMessageFolderId(folderServiceEntryId, pSelection->At(i)));
                }
            }
            CleanupStack::PopAndDestroy(pSelection);
            CleanupStack::PopAndDestroy(pFilter);
        }
        releaseCMsvEntryAndPopFromCleanupStack(pEntry);
    }
    
    return ids;
}

QMessageFolderId CMTMEngine::createQMessageFolderId(const TMsvId& serviceEntryId, const TMsvId& folderId) const
{
    QString nullString = "00000000";
    QString serviceEntryIdString = QString::number(serviceEntryId);
    serviceEntryIdString = nullString.left(8-serviceEntryIdString.length()) + serviceEntryIdString;
    QString folderIdString = QString::number(folderId);
    folderIdString = nullString.left(8-folderIdString.length()) + folderIdString;
    return SymbianHelpers::addIdPrefix(serviceEntryIdString+folderIdString,SymbianHelpers::EngineTypeMTM);
}

TMsvId CMTMEngine::serviceEntryIdFromQMessageFolderId(const QMessageFolderId& folderId) const
{
    return SymbianHelpers::stripIdPrefix(folderId.toString()).left(8).toLong(); 
}

TMsvId CMTMEngine::folderIdFromQMessageFolderId(const QMessageFolderId& folderId) const
{
    return SymbianHelpers::stripIdPrefix(folderId.toString()).right(8).toLong(); 
}

void CMTMEngine::handleNestedFiltersFromFolderFilter(QMessageFolderFilter &filter) const
{
    QMessageFolderFilterPrivate* pMFFilter = QMessageFolderFilterPrivate::implementation(filter);
    if (pMFFilter->_filterList.count() > 0) {
        int filterListCount = pMFFilter->_filterList.count();
        for (int i=0; i < filterListCount; i++) {
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                QMessageFolderFilterPrivate* pMFFilter2 = QMessageFolderFilterPrivate::implementation(pMFFilter->_filterList[i][j]);
                if (pMFFilter2->_field == QMessageFolderFilterPrivate::ParentAccountIdFilter) {
                    QMessageAccountIdList accountIds = queryAccounts(*pMFFilter2->_accountFilter, QMessageAccountSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (accountIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < accountIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFolderFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFolderFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < accountIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_accountFilter;
                        pMFFilter2->_accountFilter = 0;
                        pMFFilter2->_field = QMessageFolderFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                    }
                } else {
                    break;
                }
            }
        }
    } else {
        if (pMFFilter->_field == QMessageFolderFilterPrivate::ParentAccountIdFilter) {
            QMessageAccountIdList accountIds = queryAccounts(*pMFFilter->_accountFilter, QMessageAccountSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (accountIds.count() > 0) {
                for (int i=0; i < accountIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_accountFilter;
                        pMFFilter->_accountFilter = 0;
                        pMFFilter->_field = QMessageFolderFilterPrivate::ParentAccountId;
                        pMFFilter->_value = accountIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFolderFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFolderFilter::byParentAccountId(accountIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFolderFilter::byParentAccountId(accountIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_accountFilter;
                pMFFilter->_accountFilter = 0;
                pMFFilter->_field = QMessageFolderFilterPrivate::Id;
            }
        }
    }
}

QMessageFolderIdList CMTMEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageFolderIdList ids;
    
    QMessageFolderFilter copyOfFilter = filter;
    handleNestedFiltersFromFolderFilter(copyOfFilter);
    
    QMessageFolderFilterPrivate* pMFFilter = QMessageFolderFilterPrivate::implementation(copyOfFilter);

    if (pMFFilter->_filterList.count() > 0) {
        for (int i=0; i < pMFFilter->_filterList.count(); i++) {
            bool filterHandled;
            QMessageFolderIdList ids2 = filterMessageFolders(pMFFilter->_filterList[i][0], filterHandled);
            for (int x=ids2.count()-1; x >= 0; x--) {
                QMessageFolder mf = folder(ids2[x]);
                int j = filterHandled ? 1 : 0;
                for (; j < pMFFilter->_filterList[i].count(); j++) {
                    if (!QMessageFolderFilterPrivate::implementation(pMFFilter->_filterList[i][j])->filter(mf)) {
                        ids2.removeAt(x);
                        break;
                    }
                }
            }
            for (int j=0; j < ids2.count(); j++) {
                if (!ids.contains(ids2[j])) {
                   ids.append(ids2[j]);
                }
            }
        }
    } else {
        bool filterHandled;
        ids = filterMessageFolders(copyOfFilter, filterHandled);
        if (!filterHandled) {
            for (int i=ids.count()-1; i >= 0; i--) {
                if (!QMessageFolderFilterPrivate::implementation(copyOfFilter)->filter(ids[i])) {
                    ids.removeAt(i);
                }
            }
        }
    }
    
    if (!sortOrder.isEmpty()) {
        orderFolders(ids, sortOrder);
    }
    
    applyOffsetAndLimitToMsgFolderIds(ids, offset, limit);
    
    return ids;
}

void CMTMEngine::applyOffsetAndLimitToMsgFolderIds(QMessageFolderIdList& idList, int offset, int limit) const
{
    if (offset > 0) {
        if (offset > idList.count()) {
            idList.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                idList.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = idList.count()-1; i >= limit; i--) {
            idList.removeAt(i);
        }
    }
}

int CMTMEngine::countFolders(const QMessageFolderFilter &filter) const
{
    return queryFolders(filter, QMessageFolderSortOrder(), 0, 0).count();
}

QMessageFolder CMTMEngine::folder(const QMessageFolderId &id) const
{
    if (!iSessionReady)
        return QMessageFolder();
    
    QMessageFolder folder;
    TRAPD(err, folder = folderL(id));
    Q_UNUSED(err)
       
    return folder;
}

QMessageFolder CMTMEngine::folderL(const QMessageFolderId &id) const
{
    QMessageFolder folder;

    TMsvId folderId = folderIdFromQMessageFolderId(id);
    TMsvId serviceId = serviceEntryIdFromQMessageFolderId(id);
    CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(folderId);
    if (pEntry) {
        if (pEntry->Entry().iMtm == KUidMsgTypePOP3) {
            pEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
        } 
        QMessageAccountId accountId(accountIdByServiceId(serviceId));
        QMessageFolderId parentId;
        QString name = QString::fromUtf16(pEntry->Entry().iDetails.Ptr(), pEntry->Entry().iDetails.Length());
        folder = QMessageFolderPrivate::from(id, accountId, parentId, name, name);
        releaseCMsvEntryAndPopFromCleanupStack(pEntry);
    }
    
    return folder;
}

QMessage CMTMEngine::message(const QMessageId& id) const
{
    if (!iSessionReady)
        return QMessage();
    
    QMessage message;
    TRAPD(err, message = messageL(id));
    Q_UNUSED(err)

    return message;
}

QMessage CMTMEngine::messageL(const QMessageId& id) const
{
    QMessage message;

    long int messageId = SymbianHelpers::stripIdPrefix(id.toString()).toLong();
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(pEntry);
    
    if (pEntry->Entry().iMtm == KUidMsgTypeSMS) {
        if (ipSmsMtm)
            message = smsMessageL(*pEntry, messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypeMultimedia) {
        if (ipMmsMtm)
            message = mmsMessageL(*pEntry, messageId);
    }  else if (pEntry->Entry().iMtm == KUidMsgTypeSMTP) {
        if (ipSmtpMtm)
            message = emailMessageL(*pEntry, messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypeIMAP4) {
        if (ipImap4Mtm)
            message = emailMessageL(*pEntry, messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypePOP3) {
        if (ipPop3Mtm)
            message = emailMessageL(*pEntry, messageId);
    }

    CleanupStack::PopAndDestroy(pEntry);

    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = id; 
    privateMessage->_modified = false;
    
    return message;
}

bool CMTMEngine::storeMMS(QMessage &message)
{
    if (!iSessionReady) {
        return false;
    }
    
    TRAPD(err, storeMMSL(message));
    if (err != KErrNone) {
        return false;
    }
    
    return true;
}

bool CMTMEngine::sendMMS(QMessage &message)
{
    if (!iSessionReady) {
        return false;
    }

    TRAPD(err, sendMMSL(message));
    if (err != KErrNone) {
        return false;
    }
    
    return true;
}

bool CMTMEngine::storeEmail(QMessage &message)
{
    if (!iSessionReady) {
        return false;
    }
    
    TRAPD(err, storeEmailL(message));
    if (err != KErrNone) {
        return false;
    }
    
    return true;
}

bool CMTMEngine::sendEmail(QMessage &message)
{
    if (!iSessionReady) {
        return false;
    }

    TRAPD(err, sendEmailL(message));
    if (err != KErrNone) {
        return false;
    }
    
    return true;
}

bool CMTMEngine::storeSMS(QMessage &message)
{
    if (!iSessionReady)
        return false;
    
    TRAPD(err, storeSMSL(message));
    if (err != KErrNone) {
        return false;
    }
    
    return true;
}

void CMTMEngine::storeSMSL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }
    
    TMsvId destinationFolderId;
    if (message.parentFolderId().isValid()) {
        destinationFolderId = folderIdFromQMessageFolderId(message.parentFolderId());
    } else {
        // parentFolderId was not defined for new Message
        // => Message will be created into defined standard Folder (Default value is Drafts Folder)
        destinationFolderId =  standardFolderId(message.standardFolder());
    }
    
    // Switch current SMS MTM context to folder entry    
    ipSmsMtm->SwitchCurrentEntryL(destinationFolderId);

    // Create a new SMS message entry as a child of the current context
    //
    // Note: CreateMessageL sets following values to new message entry:
    //       entry.iType = KUidMsvMessageEntry;
    //       entry.iRelatedId = <ID of the current SMS service>;
    //       entry.iServiceId = KMsvLocalServiceIndexEntryId;
    //       entry.iMtm = <SMS Message Type UID>;
    //       entry.SetVisible(EFalse);
    //       entry.SetInPreparation(ETrue);
    //       entry.iDate.UniversalTime(); <= Not set in older platforms
    //
    // Note: CreateMessageL automatically creates SMS header
    //       that contains default service settings & default
    //       service center address
    //
    // Note: CreateMessageL switches current SMS MTM context to
    //       a new SMS message context
    ipSmsMtm->CreateMessageL(KUidMsgTypeSMS.iUid);
    
    // Get the current context (new message context)
    CMsvEntry& newMessageContext = ipSmsMtm->Entry();
    
    // Copy entry values from the new message context index entry
    TMsvEntry entry = newMessageContext.Entry();
    
    // Set priority values to message entry
    switch (message.priority()) {
    case QMessage::HighPriority:
        entry.SetPriority(EMsvHighPriority);
        break;
    case QMessage::NormalPriority:
        entry.SetPriority(EMsvMediumPriority);
        break;
    case QMessage::LowPriority:
        entry.SetPriority(EMsvLowPriority);
        break;
    }
    
    // Set message read status to message entry
    if (message.status() & QMessage::Read) { 
        entry.SetUnread(false);
        entry.SetNew(false);
    } else {
        entry.SetUnread(true);
        entry.SetNew(true);
    }
    
    // Set first message addressee to message entry
    // and all message addressees to SMS message
    QList<QMessageAddress> list(message.to());
    if (!list.empty()){
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < list.size(); ++i) {
            qreceiver = list.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            if (i == 0) {
                // Set addressee to message entry 
                entry.iDetails.Set(receiver);
            }
            // Add addressee to SMS message
            ipSmsMtm->AddAddresseeL(receiver); 
        }
    }
    
    // Set body to message entry and SMS message
    QString body = message.textContent();
    if (!body.isEmpty()){
        TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));

        // Set body to message entry 
        entry.iDescription.Set(msg);

        // Set body to SMS message
        CRichText& body = ipSmsMtm->Body();
        body.Reset();
        body.InsertL(0, msg);
    }

    // Set date to message entry
    if (!message.receivedDate().isNull() || !message.date().isNull()) {
        if (!message.date().isNull()) {
            entry.iDate = qDateTimeToSymbianTTime(message.date());
        } else {
            entry.iDate = qDateTimeToSymbianTTime(message.receivedDate());
        }
    }
    
    // Set new message's context's index entry to the specified values.
    // <=> Changes are set into cache only
    newMessageContext.ChangeL(entry);
    
    // Commit cached changes to the storage
    // Note: SaveMessageL sets following values to message entry:
    //       entry.SetVisible(ETrue);
    //       entry.SetInPreparation(EFalse);
    ipSmsMtm->SaveMessageL();  

    // Get message id from new SMS message index entry 
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = QMessageId(SymbianHelpers::addIdPrefix(QString::number(entry.Id())));
}

bool CMTMEngine::sendSMS(QMessage &message)
{
    if (!iSessionReady) {
        return false;
    }
    
    TRAPD(err, sendSMSL(message));
    if (err != KErrNone) {
        return false;
    }
    
    return true;
}

bool CMTMEngine::validateSMS()
{
    
    // Validate SMS body.
    TMsvPartList result(KMsvMessagePartNone);
    result = ipSmsMtm->ValidateMessage(KMsvMessagePartBody);
    if (result != KMsvMessagePartNone ) {
        return false;
    }
    
    // Validate SMS recipient
    result = ipSmsMtm->ValidateMessage(KMsvMessagePartRecipient);
    if ( result != KMsvMessagePartNone ) {
        return false;
    }
    
    return true;
}

void CMTMEngine::sendSMSL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }

    if (!message.id().isValid()) {
        QMessagePrivate::setStandardFolder(message, QMessage::DraftsFolder);
        storeSMSL(message);
    }

    long int messageId = SymbianHelpers::stripIdPrefix(message.id().toString()).toLong();
    if (messageId == 0) {
        User::Leave(KErrNotReady);
    }

    // Switch current SMS MTM context to message entry    
    ipSmsMtm->SwitchCurrentEntryL(messageId);
    
    // Load the cache with the message data 
    ipSmsMtm->LoadMessageL(); 
    
    // Copy entry values from the message context index entry
    TMsvEntry entry = ipSmsMtm->Entry().Entry();    

    // Update date to UniversalTime
    // <=> Date field is used to control message send time
    entry.iDate.UniversalTime();

    // Update message sending state
    entry.SetSendingState(KMsvSendStateWaiting);
    
    // Set SMS Service & delivery settings to the SMS header
    CSmsHeader& smsHeader = ipSmsMtm->SmsHeader();
    CSmsSettings* pSmsSettings = CSmsSettings::NewL();
    CleanupStack::PushL(pSmsSettings);
 
    pSmsSettings->CopyL(ipSmsMtm->ServiceSettings());
    pSmsSettings->SetDelivery(ESmsDeliveryImmediately);
    pSmsSettings->SetDeliveryReport(EFalse);
    smsHeader.SetSmsSettingsL(*pSmsSettings);
 
    if (smsHeader.Message().ServiceCenterAddress().Length() == 0) {
        CSmsSettings* pSmsServiceSettings = &(ipSmsMtm->ServiceSettings());
        if (!pSmsServiceSettings->ServiceCenterCount()) {
            User::Leave(KErrNotReady);
        } else {
            CSmsNumber* pSmsCenterNumber = CSmsNumber::NewL();
            CleanupStack::PushL(pSmsCenterNumber);
            pSmsCenterNumber->SetAddressL((pSmsServiceSettings->GetServiceCenter(pSmsServiceSettings->DefaultServiceCenter())).Address());
            smsHeader.Message().SetServiceCenterAddressL(pSmsCenterNumber->Address());
            CleanupStack::PopAndDestroy(pSmsCenterNumber);
        }
    }
 
    CleanupStack::PopAndDestroy(pSmsSettings);    

    // Update message's context's index entry to the new values.
    // <=> Changes are set into cache only
    ipSmsMtm->Entry().ChangeL(entry);
    
    // Commit cached changes to the storage
    ipSmsMtm->SaveMessageL();
    
    if (validateSMS()) {
        // Switch current SMS MTM context to SMS message parent folder entry
        ipSmsMtm->SwitchCurrentEntryL(ipSmsMtm->Entry().Entry().Parent());
        
        CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
    
        // Move SMS Message to Outbox
        CMsvOperation* pMsvOperation = ipSmsMtm->Entry().MoveL(messageId,
                                                               KMsvGlobalOutBoxIndexEntryId,
                                                               pMsvOperationWait->iStatus);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        delete pMsvOperation;
        
        // Send SMS Message
        CMsvEntrySelection* pMsvEntrySelection = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(pMsvEntrySelection);
    
        // Add SMS Message Id to selection
        pMsvEntrySelection->AppendL(messageId); 
     
        // Add selection (containing SMS Message Id) to task scheduler
        TBuf8<1> dummyParams;
        pMsvOperation = ipSmsMtm->InvokeAsyncFunctionL(ESmsMtmCommandScheduleCopy,
                                                       *pMsvEntrySelection,
                                                       dummyParams,
                                                       pMsvOperationWait->iStatus);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        delete pMsvOperation;    
     
        CleanupStack::PopAndDestroy(pMsvEntrySelection);    
    
        CleanupStack::PopAndDestroy(pMsvOperationWait);
    } else {
        User::Leave(KErrCorrupt);
    }
}

void CMTMEngine::storeMMSL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }
    
    TMsvId destinationFolderId;
    if (message.parentFolderId().isValid()) {
        destinationFolderId = folderIdFromQMessageFolderId(message.parentFolderId());
    } else {
        // parentFolderId was not defined for new Message
        // => Message will be created into defined standard Folder (Default value is Drafts Folder)
        destinationFolderId =  standardFolderId(message.standardFolder());
    }
        
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    wait->Start();
    CMsvOperation* operation = ipMmsMtm->CreateNewEntryL(destinationFolderId, wait->iStatus);
    CleanupStack::PushL(operation);
    CActiveScheduler::Start();
    if (wait->iStatus.Int() != KErrNone){ 
    //TODO: handle error
    }
    TPckgBuf<TMsvId> pkg;
    pkg.Copy( operation->ProgressL());
    TMsvId indexEntry= pkg();
    CleanupStack::PopAndDestroy(2); // operation and wait
    ipMmsMtm->SwitchCurrentEntryL(indexEntry);
    ipMmsMtm->LoadMessageL();
            
    // Add receivers
    QList<QMessageAddress> list(message.to());
    TPtrC16 receiver(KNullDesC);
    QString qreceiver;
    for (int i = 0; i < list.size(); ++i) {
        qreceiver = list.at(i).addressee();
        receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
        ipMmsMtm->AddAddresseeL(receiver); 
        }
    
    // Set Subject
    QString subject = message.subject();
    TPtrC16 sbj(reinterpret_cast<const TUint16*>(subject.utf16()));
    ipMmsMtm->SetSubjectL(sbj); 
    
    TMsvEntry ent = ipMmsMtm->Entry().Entry();       
    ent.SetInPreparation(EFalse); 
    ent.SetVisible(ETrue);
    
    switch (message.priority()) {
    case QMessage::HighPriority:
        ent.SetPriority(EMsvHighPriority);
        break;
    case QMessage::NormalPriority:
        ent.SetPriority(EMsvMediumPriority);
        break;
    case QMessage::LowPriority:
        ent.SetPriority(EMsvLowPriority);
        break;
    }
    if (message.status() & QMessage::Read) { 
        ent.SetUnread(false);
        ent.SetNew(false);
    } else {
        ent.SetUnread(true);
        ent.SetNew(true);
    }
    ipMmsMtm->Entry().ChangeL(ent); 
    // Save the changes    
    ipMmsMtm->SaveMessageL();
    
    CMsvStore* store = ipMmsMtm->Entry().EditStoreL();    
    CleanupStack::PushL(store); 
    
    // attachments.
    QByteArray filePath;
    QMessageContentContainerIdList contentIds = message.contentIds();
    foreach (QMessageContentContainerId id, contentIds){
        QMessageContentContainer container = message.find(id);
        filePath = QMessageContentContainerPrivate::attachmentFilename(container);
        QString body = container.textContent();
        if (!filePath.isEmpty()) { // content is attachment
            int last = filePath.lastIndexOf("/");
            int count = filePath.count();
            QByteArray name = filePath.right(count-last-1);
            QString fileName = QString(name);
            filePath.replace(QByteArray("/"), QByteArray("\\"));
            QString temp_path = QString(filePath);
            TPtrC16 attachmentPath(KNullDesC);
            attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
            TFileName attachmentFile;
            attachmentFile.Append(attachmentPath);    
        
            RFile attachment;    
            User::LeaveIfError(attachment.Open(FsSession(),attachmentFile, EFileShareReadersOnly | EFileRead));    
            CleanupClosePushL(attachment);  
            
            TInt fileSize;
            attachment.Size(fileSize);
            TInt maxFileSize = 600000;
            if (fileSize > maxFileSize){
                return;
            }
            
            HBufC8* pFileContent = HBufC8::NewL(fileSize);
            TPtr8 fileContent(pFileContent->Des());
            attachment.Read(fileContent);
            attachment.Close();
            CleanupStack::PushL(pFileContent);
            RFile file2;
            TFileName tempFileName;
            RFs fileServer;
            User::LeaveIfError(fileServer.Connect());
            CleanupClosePushL(fileServer);
            TInt err = file2.Temp(fileServer,iPath,tempFileName,EFileWrite);
            if (err != KErrNone){
                return;
            }
            CleanupClosePushL(file2);
            err = file2.Write(*pFileContent);
            if (err != KErrNone){
                return;
            }
            file2.Close();
            User::LeaveIfError(file2.Open(FsSession(),tempFileName, EFileShareAny|EFileRead));
            // Mime header    
            CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();    
            CleanupStack::PushL(mimeHeaders); 
            TPtrC16 suggestedName(reinterpret_cast<const TUint16*>(fileName.utf16()));
            mimeHeaders->SetSuggestedFilenameL(suggestedName);     
            // Represents a single attachment and information about the attachment    
            CMsvAttachment* attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);    
            CleanupStack::PushL(attaInfo); 
            
            // Mime Type    
            QByteArray content = container.contentType();
            if (content.isEmpty()){
                RApaLsSession session;
                //TPtrC16 filePtr(reinterpret_cast<const TUint16*>(fileString.utf16()));
                TBuf8<20> fileType;
                if(session.Connect() == KErrNone){                        
                    TDataRecognitionResult fileDataType;                     
                    session.RecognizeData(tempFileName, *pFileContent, *&fileDataType);                                         
                    fileType.Copy(fileDataType.iDataType.Des8());
                    content = QByteArray((const char*)fileType.Ptr(), fileType.Length());
                    session.Close();                
                }
            }
            else{
                content.append("/");
                content.append(container.contentSubType());       
            }
           
            QString mime = QString(content);
            TPtrC16 mType(reinterpret_cast<const TUint16*>(mime.utf16()));
            HBufC8* mimeType = CnvUtfConverter::ConvertFromUnicodeToUtf8L(mType);
            
            TMsvAttachmentId attachId = KMsvNullIndexEntryId;        
            ipMmsMtm->CreateAttachment2L(*store, file2, *mimeType, *mimeHeaders, attaInfo, attachId);
            store->CommitL();
            file2.Close();
            
            CleanupStack::Pop(attaInfo);    
            CleanupStack::PopAndDestroy(mimeHeaders);  
            CleanupStack::Pop(&file2);
            CleanupStack::PopAndDestroy(&fileServer);
            CleanupStack::PopAndDestroy(pFileContent);
            CleanupStack::PopAndDestroy(); //attachment
        }
        else if (!body.isEmpty()) { // Add body
            TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
            TMsvAttachmentId attachmentid = KMsvNullIndexEntryId;
            TFileName bodyFile( _L("body.txt") );
            ipMmsMtm->CreateTextAttachmentL(*store, attachmentid, msg, bodyFile, ETrue); 
            store->CommitL();
        }

    }
    QString messageBody = message.textContent();
    if (!messageBody.isEmpty()) { // no contents, only body text
        TPtrC16 msg(reinterpret_cast<const TUint16*>(messageBody.utf16()));
        TMsvAttachmentId attachmentid = KMsvNullIndexEntryId;
        TFileName bodyFile( _L("body.txt") );
        ipMmsMtm->CreateTextAttachmentL(*store, attachmentid, msg, bodyFile, ETrue); 
        store->CommitL();
    }
         
    CleanupStack::PopAndDestroy(); // store    
    
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = QMessageId(SymbianHelpers::addIdPrefix(QString::number(indexEntry,SymbianHelpers::EngineTypeMTM)));
    // Save the changes
    ipMmsMtm->SaveMessageL();
    
    if (!message.receivedDate().isNull() || !message.date().isNull()) {
        // Change the date to given date
        CMsvEntry* pEntry = ipMsvSession->GetEntryL(indexEntry);
        CleanupStack::PushL(pEntry);
        TMsvEntry changedEntry = pEntry->Entry();
        if (!message.date().isNull()) {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.date());
        } else {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.receivedDate());
        }
        pEntry->ChangeL(changedEntry);
        CleanupStack::PopAndDestroy(pEntry);
    }
}

void CMTMEngine::updateSMSL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }
    
    QMessageId id = message.id();
    long int messageId = SymbianHelpers::stripIdPrefix(id.toString()).toLong();
    if (messageId == 0)
        return;
    
    CMsvEntry* entry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(entry);
    
    ipSmsMtm->SwitchCurrentEntryL(messageId);
    ipSmsMtm->LoadMessageL();
    
    TMsvEntry tEntry = ipSmsMtm->Entry().Entry();
    switch (message.priority()) {
    case QMessage::HighPriority:
        tEntry.SetPriority(EMsvHighPriority);
        break;
    case QMessage::NormalPriority:
        tEntry.SetPriority(EMsvMediumPriority);
        break;
    case QMessage::LowPriority:
        tEntry.SetPriority(EMsvLowPriority);
        break;            
    }
    if (message.status() & QMessage::Read) { 
        tEntry.SetUnread(false);
        tEntry.SetNew(false);
    } else {
        tEntry.SetUnread(true);
        tEntry.SetNew(true);
    }
    ipSmsMtm->Entry().ChangeL(tEntry); 
    // Save the changes    
    ipSmsMtm->SaveMessageL();
    
    QList<QMessageAddress> list(message.to());
    if (!list.empty()){
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < list.size(); ++i) {
            qreceiver = list.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            ipSmsMtm->AddAddresseeL(receiver); 
            ipSmsMtm->SaveMessageL();
        }
    }
    
    CMsvStore* store = ipSmsMtm->Entry().EditStoreL(); 
    CleanupStack::PushL(store);
    
    QString body = message.textContent();
    if (!body.isEmpty()){
        TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
        if (!ipRichText) {
            ipCharFormatLayer = CCharFormatLayer::NewL();
            ipParaFormatLayer = CParaFormatLayer::NewL();
            ipRichText=CRichText::NewL(ipParaFormatLayer,ipCharFormatLayer);
        }
        ipRichText->Reset();
        ipRichText->InsertL(0, msg);
        store->StoreBodyTextL(*ipRichText);
        store->CommitL();
    } 
    
    CleanupStack::PopAndDestroy(2); // store, entry
    
    if (!message.receivedDate().isNull() || !message.date().isNull()) {
        // Change the date to given date
        CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
        CleanupStack::PushL(pEntry);
        TMsvEntry changedEntry = pEntry->Entry();
        if (!message.date().isNull()) {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.date());
        } else {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.receivedDate());
        }
        pEntry->ChangeL(changedEntry);
        CleanupStack::PopAndDestroy(pEntry);
    }
}

void CMTMEngine::updateMMSL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }
    
    QMessageId id = message.id();
    long int messageId = SymbianHelpers::stripIdPrefix(id.toString()).toLong();
    if (messageId == 0)
        return;
    
    ipMmsMtm->SwitchCurrentEntryL(messageId);
    ipMmsMtm->LoadMessageL();
            
    // Add receivers
    QList<QMessageAddress> list(message.to());
    if (!list.empty()){
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < list.size(); ++i) {
            qreceiver = list.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            ipMmsMtm->AddAddresseeL(receiver); 
            }
    }
    
    // Set Subject
    QString subject = message.subject();
    if (!subject.isEmpty()){
        TPtrC16 sbj(reinterpret_cast<const TUint16*>(subject.utf16()));
        ipMmsMtm->SetSubjectL(sbj); 
    }
        
    TMsvEntry ent = ipMmsMtm->Entry().Entry();       
    ent.SetInPreparation(EFalse); 
    ent.SetVisible(ETrue);    
    
    switch (message.priority()) {
    case QMessage::HighPriority:
        ent.SetPriority(EMsvHighPriority);
        break;
    case QMessage::NormalPriority:
        ent.SetPriority(EMsvMediumPriority);
        break;
    case QMessage::LowPriority:
        ent.SetPriority(EMsvLowPriority);
        break;
    }
    if (message.status() & QMessage::Read) { 
        ent.SetUnread(false);
        ent.SetNew(false);
    } else {
        ent.SetUnread(true);
        ent.SetNew(true);
    }
    ipMmsMtm->Entry().ChangeL(ent); 
    // Save the changes    
    ipMmsMtm->SaveMessageL();
    
    CMsvStore* store = ipMmsMtm->Entry().EditStoreL();    
    CleanupStack::PushL(store); 
    
    // attachments.
    QByteArray filePath;
    QMessageContentContainerIdList contentIds = message.contentIds();
    foreach (QMessageContentContainerId id, contentIds){
        QMessageContentContainer container = message.find(id);
        QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
        filePath = QMessageContentContainerPrivate::attachmentFilename(container);
        if (!filePath.isEmpty()) { // content is attachment
            int last = filePath.lastIndexOf("/");
            int count = filePath.count();
            QByteArray name = filePath.right(count-last-1);
            QString fileName = QString(name);
            filePath.replace(QByteArray("/"), QByteArray("\\"));
            QString temp_path = QString(filePath);
            TPtrC16 attachmentPath(KNullDesC);
            attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
            TFileName attachmentFile;
            attachmentFile.Append(attachmentPath);    
        
            RFile attachment;    
            User::LeaveIfError(attachment.Open(FsSession(),attachmentFile, EFileShareReadersOnly | EFileRead));    
            CleanupClosePushL(attachment);  
            
            TInt fileSize;
            attachment.Size(fileSize);
            TInt maxFileSize = 600000;
            if (fileSize > maxFileSize){
                return;
            }
            
            HBufC8* pFileContent = HBufC8::NewL(fileSize);
            TPtr8 fileContent(pFileContent->Des());
            attachment.Read(fileContent);
            attachment.Close();
            CleanupStack::PushL(pFileContent);
            RFile file2;
            TFileName tempFileName;
            RFs fileServer;
            User::LeaveIfError(fileServer.Connect());
            CleanupClosePushL(fileServer);
            TInt err = file2.Temp(fileServer,iPath,tempFileName,EFileWrite);
            if (err != KErrNone){
                return;
            }
            CleanupClosePushL(file2);
            err = file2.Write(*pFileContent);
            if (err != KErrNone){
                return;
            }
            file2.Close();
            User::LeaveIfError(file2.Open(FsSession(),tempFileName, EFileShareAny|EFileRead));
            // Mime header    
            CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();    
            CleanupStack::PushL(mimeHeaders); 
            TPtrC16 suggestedName(reinterpret_cast<const TUint16*>(fileName.utf16()));
            mimeHeaders->SetSuggestedFilenameL(suggestedName);     
            // Represents a single attachment and information about the attachment    
            CMsvAttachment* attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);    
            CleanupStack::PushL(attaInfo); 
            
            // Mime Type    
            QByteArray content = container.contentType();
            if (content.isEmpty()){
                RApaLsSession session;
                TBuf8<20> fileType;
                if(session.Connect() == KErrNone){                        
                    TDataRecognitionResult fileDataType;                     
                    session.RecognizeData(tempFileName, *pFileContent, *&fileDataType);                                         
                    fileType.Copy(fileDataType.iDataType.Des8());
                    content = QByteArray((const char*)fileType.Ptr(), fileType.Length());
                    session.Close();                
                }
            }
            else{
                content.append("/");
                content.append(container.contentSubType());       
            }       
            QString mime = QString(content);
            TPtrC16 mType(reinterpret_cast<const TUint16*>(mime.utf16()));
            HBufC8* mimeType = CnvUtfConverter::ConvertFromUnicodeToUtf8L(mType);
            
            TMsvAttachmentId attachId = KMsvNullIndexEntryId;        
            ipMmsMtm->CreateAttachment2L(*store, file2, *mimeType, *mimeHeaders, attaInfo, attachId);
            store->CommitL();
            file2.Close();
            
            CleanupStack::Pop(attaInfo);    
            CleanupStack::PopAndDestroy(mimeHeaders);  
            CleanupStack::Pop(&file2);
            CleanupStack::PopAndDestroy(&fileServer);
            CleanupStack::PopAndDestroy(pFileContent);
            CleanupStack::PopAndDestroy(); //attachment
        }
        else if (pPrivateContainer->_id == message.bodyId()) { // content is body text
            QString body = container.textContent();
            TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
            TMsvAttachmentId attachmentid = KMsvNullIndexEntryId;
            TFileName bodyFile( _L("body.txt") );
            ipMmsMtm->CreateTextAttachmentL(*store, attachmentid, msg, bodyFile, ETrue); 
            store->CommitL();
        }
    }
    
    QString messageBody = message.textContent();
    if (!messageBody.isEmpty()) { // no contents, only body text
        TPtrC16 msg(reinterpret_cast<const TUint16*>(messageBody.utf16()));
        TMsvAttachmentId attachmentid = KMsvNullIndexEntryId;
        TFileName bodyFile( _L("body.txt") );
        ipMmsMtm->CreateTextAttachmentL(*store, attachmentid, msg, bodyFile, ETrue); 
        store->CommitL();
    }
        
    CleanupStack::PopAndDestroy(); // store    
    // Save the changes
    ipMmsMtm->SaveMessageL();
    
    if (!message.receivedDate().isNull() || !message.date().isNull()) {
        // Change the date to given date
        CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
        CleanupStack::PushL(pEntry);
        TMsvEntry changedEntry = pEntry->Entry();
        if (!message.date().isNull()) {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.date());
        } else {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.receivedDate());
        }
        pEntry->ChangeL(changedEntry);
        CleanupStack::PopAndDestroy(pEntry);
    }
}

void CMTMEngine::updateEmailL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }

    QMessageId id = message.id();
    long int messageId = SymbianHelpers::stripIdPrefix(id.toString()).toLong();
    if (messageId == 0)
        return;
    
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
    TUid mtmUid = pEntry->Entry().iMtm;
    delete pEntry;
    
    TMsvEntry msvEntry;
    if (mtmUid == KUidMsgTypeSMTP) {
        ipSmtpMtm->SwitchCurrentEntryL(messageId);
        ipSmtpMtm->LoadMessageL();
        msvEntry = ipSmtpMtm->Entry().Entry();
    } else if (mtmUid == KUidMsgTypePOP3) {
        ipPop3Mtm->SwitchCurrentEntryL(messageId);
        ipPop3Mtm->LoadMessageL();
        msvEntry = ipPop3Mtm->Entry().Entry();
    } else if (mtmUid == KUidMsgTypeIMAP4) {
        ipImap4Mtm->SwitchCurrentEntryL(messageId);
        ipImap4Mtm->LoadMessageL();
        msvEntry = ipImap4Mtm->Entry().Entry();
    }

    switch (message.priority()) {
    case QMessage::HighPriority:
        msvEntry.SetPriority(EMsvHighPriority);
        break;
    case QMessage::NormalPriority:
        msvEntry.SetPriority(EMsvMediumPriority);
        break;
    case QMessage::LowPriority:
        msvEntry.SetPriority(EMsvLowPriority);
        break;            
    }
    if (message.status() & QMessage::Read) {
        msvEntry.SetUnread(false);
        msvEntry.SetNew(false);
    } else {
        msvEntry.SetUnread(true);
        msvEntry.SetNew(true);
    }
    
    msvEntry.iDescription.Set(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    msvEntry.iDetails.Set(TPtrC(reinterpret_cast<const TUint16*>(message.from().addressee().utf16())));
    
    if (mtmUid == KUidMsgTypeSMTP) {
        ipSmtpMtm->Entry().ChangeL(msvEntry);
    } else {
        CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
        CMsvOperation* pMsvOperation = NULL;
        if (mtmUid == KUidMsgTypePOP3) {
            pMsvOperation = ipPop3Mtm->Entry().ChangeL(msvEntry, pMsvOperationWait->iStatus);
        } else {
            pMsvOperation = ipImap4Mtm->Entry().ChangeL(msvEntry, pMsvOperationWait->iStatus);
        }
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        delete pMsvOperation;
        CleanupStack::PopAndDestroy(pMsvOperationWait);
    }
    
    // Save the changes    
    ipSmtpMtm->SaveMessageL();
    
    CMsvEntry* entry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(entry);
    
    CImEmailMessage* mailMsg = CImEmailMessage::NewLC(*entry); 
    
    CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
    
    CImMimeHeader* mime = CImMimeHeader::NewLC(); 
    
    QByteArray filePath;
    QMessageContentContainerIdList contentIds = message.contentIds();
    foreach (QMessageContentContainerId id, contentIds){
        QMessageContentContainer container = message.find(id);
        QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
        filePath = QMessageContentContainerPrivate::attachmentFilename(container);
        if (!filePath.isEmpty()) { // content is attachment
            filePath.replace(QByteArray("/"), QByteArray("\\"));
            QString temp_path = QString(filePath);
            TPtrC16 attachmentPath(KNullDesC);
            attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
            TFileName attachmentFile;
            attachmentFile.Append(attachmentPath);    
        
            RFile attachment;    
            User::LeaveIfError(attachment.Open(FsSession(),attachmentFile, EFileShareReadersOnly | EFileRead));    
            CleanupClosePushL(attachment);   
            
            CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
            QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
            int last = filePath.lastIndexOf("/");
            int count = filePath.count();
            QByteArray name = filePath.right(count-last-1);
            QString fileName = QString(name);
            CleanupStack::PushL(attachmentInfo);  
            attachmentInfo->SetAttachmentNameL(TPtrC(reinterpret_cast<const TUint16*>(fileName.utf16())));
            mailMsg->AttachmentManager().AddAttachmentL(attachment, attachmentInfo, waiter->iStatus);
            waiter->Start();
            CActiveScheduler::Start();
            CleanupStack::Pop(attachmentInfo);
            CleanupStack::Pop(&attachment); // close file
        } else if (pPrivateContainer->_id == message.bodyId()) { // content is body text
            CParaFormatLayer* paragraphFormatLayer = CParaFormatLayer::NewL();
            CleanupStack::PushL(paragraphFormatLayer);
        
            CCharFormatLayer* characterFormatLayer = CCharFormatLayer::NewL(); 
            CleanupStack::PushL(characterFormatLayer);
        
            CRichText* bodyText = CRichText::NewL(paragraphFormatLayer, characterFormatLayer, CEditableText::EFlatStorage, 256);
            CleanupStack::PushL(bodyText);
            
            //content type
            QByteArray contentType = container.contentType();
            TPtrC8 content((TUint8 *)(contentType.constData()));
            mime->SetContentTypeL(content);
            // content subtype
            QByteArray subType = container.contentSubType();
            TPtrC8 contentSubType((TUint8 *)(subType.constData()));
            mime->SetContentSubTypeL(contentSubType);
            // content charset
            QByteArray charset = container.contentCharset();
            if (charset == "UTF-8") {
                mime->SetMimeCharset(KCharacterSetIdentifierUtf8);
            }
            if (charset == "UTF-16") {
                mime->SetMimeCharset(KCharacterSetIdentifierUcs2);
            }
            // Insert the contents of a buffer into the document at specified position
            bodyText->InsertL(0, TPtrC(reinterpret_cast<const TUint16*>(container.textContent().utf16())));
            mailMsg->StoreBodyTextWithMimeHeaderL(messageId, *bodyText, *mime, waiter->iStatus);
            
            waiter->Start();
            CActiveScheduler::Start();
                    
            CleanupStack::PopAndDestroy(3); // bodyText, characterFormatLayer, paragraphFormatLayer
        }
        
    }  
    
    QString messageBody = message.textContent();
    if (!messageBody.isEmpty()) {
        CParaFormatLayer* paragraphFormatLayer = CParaFormatLayer::NewL();
        CleanupStack::PushL(paragraphFormatLayer);

        CCharFormatLayer* characterFormatLayer = CCharFormatLayer::NewL(); 
        CleanupStack::PushL(characterFormatLayer);

        CRichText* bodyText = CRichText::NewL(paragraphFormatLayer, characterFormatLayer, CEditableText::EFlatStorage, 256);
        CleanupStack::PushL(bodyText);
        
        //content type
        QByteArray contentType = message.contentType();
        TPtrC8 content((TUint8 *)(contentType.constData()));
        mime->SetContentTypeL(content);
        // content subtype
        QByteArray subType = message.contentSubType();
        TPtrC8 contentSubType((TUint8 *)(subType.constData()));
        mime->SetContentSubTypeL(contentSubType);
        // content charset
        QByteArray charset = message.contentCharset();
        if (charset == "UTF-8") {
            mime->SetMimeCharset(KCharacterSetIdentifierUtf8);
        }
        if (charset == "UTF-16") {
            mime->SetMimeCharset(KCharacterSetIdentifierUcs2);
        }
        
        // Insert the contents of a buffer into the document at specified position
        bodyText->InsertL(0, TPtrC(reinterpret_cast<const TUint16*>(message.textContent().utf16())));
        mailMsg->StoreBodyTextWithMimeHeaderL(messageId, *bodyText, *mime, waiter->iStatus);
        waiter->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy(3);
        //bodyText, characterFormatLayer, paragraphFormatLayer,
    }

    CMsvStore* store = entry->EditStoreL();
    CleanupStack::PushL(store);
    CImHeader* emailEntry = CImHeader::NewLC();
    TRAP_IGNORE(emailEntry->RestoreL(*store));
    mime->StoreL(*store);
    emailEntry->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    
    QList<QMessageAddress> toList(message.to());
    if (toList.count() > 0){
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < toList.size(); ++i) {
            qreceiver = toList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            emailEntry->ToRecipients().AppendL(receiver); 
        }
    }
    
    QList<QMessageAddress> ccList(message.cc());
    if (ccList.count() > 0){
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < ccList.size(); ++i) {
            qreceiver = ccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            emailEntry->CcRecipients().AppendL(receiver); 
        }
    }
        
    QList<QMessageAddress> bccList(message.bcc());
    if (bccList.count() > 0){
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < bccList.size(); ++i) {
            qreceiver = bccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            emailEntry->BccRecipients().AppendL(receiver); 
        }
    }
     
    emailEntry->StoreL(*store);
    // Store the changes permanently
    store->CommitL();
    
    CleanupStack::PopAndDestroy(6, entry); 
    // mailMsg, emailEntry, store, waiter, entry, mime   

    if (!message.receivedDate().isNull() || !message.date().isNull()) {
        // Change the date to given date
        CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
        CleanupStack::PushL(pEntry);
        TMsvEntry changedEntry = pEntry->Entry();
        if (!message.date().isNull()) {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.date());
        } else {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.receivedDate());
        }
        if (mtmUid == KUidMsgTypeSMTP) {
            ipSmtpMtm->Entry().ChangeL(changedEntry);
        } else {
            CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
            CMsvOperation* pMsvOperation = NULL;
            if (mtmUid == KUidMsgTypePOP3) {
                pMsvOperation = ipPop3Mtm->Entry().ChangeL(changedEntry, pMsvOperationWait->iStatus);
            } else {
                pMsvOperation = ipImap4Mtm->Entry().ChangeL(changedEntry, pMsvOperationWait->iStatus);
            }
            pMsvOperationWait->Start();
            CActiveScheduler::Start();
            delete pMsvOperation;
            CleanupStack::PopAndDestroy(pMsvOperationWait);
        }
        CleanupStack::PopAndDestroy(pEntry);
    }
}

void CMTMEngine::sendMMSL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }

    bool messageCreated = false;
    if (!message.id().isValid()) {
        QMessagePrivate::setStandardFolder(message, QMessage::OutboxFolder);
        storeMMSL(message);
        messageCreated = true;
    }
    
    long int messageId = SymbianHelpers::stripIdPrefix(message.id().toString()).toLong();
    if (messageId == 0) {
        User::Leave(KErrNotReady);
    }

    CMsvEntry* pMsvEntry = retrieveCMsvEntryAndPushToCleanupStack(messageId);
    CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
    
    CMsvOperation* pMsvOperation = NULL;
    if (!messageCreated) {
        ipMmsMtm->SwitchCurrentEntryL(pMsvEntry->Entry().Parent());
        pMsvOperation = ipMmsMtm->Entry().CopyL(messageId, KMsvGlobalOutBoxIndexEntryId, pMsvOperationWait->iStatus);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        delete pMsvOperation;
        pMsvOperation = NULL;
    }
    
    // Start sending the message via the Server MTM to the MMS server    
    CMsvEntrySelection* pMsvEntrySelection = new(ELeave) CMsvEntrySelection;
    pMsvEntrySelection->AppendL(messageId);
    ipMmsMtm->SwitchCurrentEntryL(KMsvGlobalOutBoxIndexEntryId);
    pMsvOperation = ipMmsMtm->SendL(*pMsvEntrySelection, pMsvOperationWait->iStatus);
    pMsvOperationWait->Start();
    CActiveScheduler::Start();
    delete pMsvOperation;
   
    while (pMsvOperationWait->iStatus == KRequestPending) {
         CActiveScheduler::Start();
    }
    
    CleanupStack::PopAndDestroy(pMsvOperationWait);
    releaseCMsvEntryAndPopFromCleanupStack(pMsvEntry);    
}

QString CMTMEngine::privateFolderPath()
{
    // Take drive letter from Application full name (Note: TUidName is 10 characters long <=> TBuf<10>)
    //TUidName applicationDrive;
    //applicationDrive.Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName().Left(2));
    
    // Get Application private folder path from FileSession
    TPath applicationPrivateFolderPathWithoutDriveLetter;
    FsSession().PrivatePath(applicationPrivateFolderPathWithoutDriveLetter);

    // Combine drive letter and private folder path to complete path
    TPath driveLetterAndPath;
    driveLetterAndPath.Copy(_L("C:"));
    driveLetterAndPath.Append(applicationPrivateFolderPathWithoutDriveLetter);
    
    return QString::fromUtf16(driveLetterAndPath.Ptr(), driveLetterAndPath.Length());
}

void CMTMEngine::storeEmailL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }

    TMsvId destinationFolderId = 0;
    TMsvId imapDestinationFolderId = 0;
    if (message.parentFolderId().isValid()) {
        destinationFolderId = folderIdFromQMessageFolderId(message.parentFolderId());
        CMsvEntry* pMsvEntry = retrieveCMsvEntryAndPushToCleanupStack(destinationFolderId);
        if (pMsvEntry->Entry().iMtm == KUidMsgTypeIMAP4 || pMsvEntry->Entry().iMtm == KUidMsgTypePOP3) {
            // New IMAP or POP3 message can not be created to IMAP or POP3 service folder.
            // => Create IMAP or POP3 message into Drafts folder instead
            destinationFolderId = KMsvDraftEntryId; 
            if (pMsvEntry->Entry().iMtm == KUidMsgTypeIMAP4) {
                // It's possible to copy newly created IMAP message from Drafts folder
                // to IMAP service folder.
                // Note: IMAP message can not be created directly to IMAP service folder.
                imapDestinationFolderId = destinationFolderId;
            }
        }
        releaseCMsvEntryAndPopFromCleanupStack(pMsvEntry);
    } else {
        // parentFolderId was not defined for new Message
        // => Message will be created into defined standard Folder (Default value is Drafts Folder)
        destinationFolderId =  standardFolderId(message.standardFolder());
    }
    
    QMessageAccount messageAccount = this->account(message.parentAccountId());
    
    CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
    TMsvEmailTypeList msvEmailTypeList = 0;
    TMsvPartList msvPartList = 0;
    if (message.status() & QMessage::HasAttachments == QMessage::HasAttachments) {
        msvPartList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
    } else {
        msvPartList = KMsvMessagePartBody;
    }
    CImEmailOperation* pImEmailOperation = CImEmailOperation::CreateNewL(pMsvOperationWait->iStatus, *ipMsvSession, destinationFolderId,
                                                                         messageAccount.d_ptr->_service2EntryId, msvPartList,
                                                                         msvEmailTypeList, KUidMsgTypeSMTP);
    CleanupStack::PushL(pImEmailOperation);
    pMsvOperationWait->Start();
    CActiveScheduler::Start();
    
    TMsvId newMessageId;
    TPckgC<TMsvId> paramPack(newMessageId);
    paramPack.Set(pImEmailOperation->ProgressL());
    newMessageId = paramPack();
    CleanupStack::PopAndDestroy(pImEmailOperation);
    
    ipSmtpMtm->SwitchCurrentEntryL(newMessageId);
    ipSmtpMtm->LoadMessageL();
    
    TMsvEntry msvEntry = ipSmtpMtm->Entry().Entry();
    switch (message.priority()) {
    case QMessage::HighPriority:
        msvEntry.SetPriority(EMsvHighPriority);
        break;
    case QMessage::NormalPriority:
        msvEntry.SetPriority(EMsvMediumPriority);
        break;
    case QMessage::LowPriority:
        msvEntry.SetPriority(EMsvLowPriority);
        break;            
    }
    if (message.status() & QMessage::Read) { 
        msvEntry.SetUnread(false);
        msvEntry.SetNew(false);
    } else {
        msvEntry.SetUnread(true);
        msvEntry.SetNew(true);
    }
    
    ipSmtpMtm->Entry().ChangeL(msvEntry); 
    ipSmtpMtm->SaveMessageL();

    CMsvEntry* pMsvEntry = retrieveCMsvEntryAndPushToCleanupStack(newMessageId);

    // Create email message MIME header 
    CImMimeHeader* pImMimeHeader = CImMimeHeader::NewLC(); 
    QByteArray contentType = message.contentType();
    TPtrC8 content((TUint8 *)(contentType.constData()));
    pImMimeHeader->SetContentTypeL(content);
    QByteArray subType = message.contentSubType();
    TPtrC8 contentSubType((TUint8 *)(subType.constData()));
    pImMimeHeader->SetContentSubTypeL(contentSubType);
    QByteArray charset = message.contentCharset();
    if (charset == "UTF-8") {
        pImMimeHeader->SetMimeCharset(KCharacterSetIdentifierUtf8);
    }
    if (charset == "UTF-16") {
        pImMimeHeader->SetMimeCharset(KCharacterSetIdentifierUcs2);
    }

    CImEmailMessage* pImEmailMessage = CImEmailMessage::NewL(*pMsvEntry);
    CleanupStack::PushL(pImEmailMessage);
    
    if (message.bodyId() == QMessageContentContainerPrivate::bodyContentId()) {
        // Message contains only body (not attachments)
        QString messageBody = message.textContent();
        if (!messageBody.isEmpty()) {
            CParaFormatLayer* pParaFormatLayer = CParaFormatLayer::NewL();
            CleanupStack::PushL(pParaFormatLayer);
            CCharFormatLayer* pCharFormatLayer = CCharFormatLayer::NewL(); 
            CleanupStack::PushL(pCharFormatLayer);
    
            CRichText* pBodyRichText = CRichText::NewL(pParaFormatLayer, pCharFormatLayer, CEditableText::EFlatStorage, 256);
            CleanupStack::PushL(pBodyRichText);
    
            // Insert the contents of a buffer into the document at specified position
            pBodyRichText->InsertL(0, TPtrC(reinterpret_cast<const TUint16*>(message.textContent().utf16())));
            // Note: Email message MIME header is same as Body MIME header
            pImEmailMessage->StoreBodyTextWithMimeHeaderL(newMessageId, *pBodyRichText, *pImMimeHeader,
                                                          pMsvOperationWait->iStatus);
            pMsvOperationWait->Start();
            CActiveScheduler::Start();
            
            CleanupStack::PopAndDestroy(pBodyRichText);
            CleanupStack::PopAndDestroy(pCharFormatLayer);
            CleanupStack::PopAndDestroy(pParaFormatLayer);
        }
    } else {
        // Message contains body and attachments
        QMessageContentContainerIdList contentIds = message.contentIds();
        foreach (QMessageContentContainerId id, contentIds){
            QMessageContentContainer container = message.find(id);
            QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
            if (pPrivateContainer->_id == message.bodyId()) {
                // ContentContainer is body
                if (!container.textContent().isEmpty()) {
                    // Create MIME header for body
                    CImMimeHeader* pBodyImMimeHeader = CImMimeHeader::NewLC();
                    QByteArray contentType = container.contentType();
                    TPtrC8 content((TUint8 *)(contentType.constData()));
                    pBodyImMimeHeader->SetContentTypeL(content);
                    QByteArray subType = container.contentSubType();
                    TPtrC8 contentSubType((TUint8 *)(subType.constData()));
                    pBodyImMimeHeader->SetContentSubTypeL(contentSubType);
                    QByteArray charset = container.contentCharset();
                    if (charset == "UTF-8") {
                        pBodyImMimeHeader->SetMimeCharset(KCharacterSetIdentifierUtf8);
                    }
                    if (charset == "UTF-16") {
                        pBodyImMimeHeader->SetMimeCharset(KCharacterSetIdentifierUcs2);
                    }
    
                    // Create CRichText containing body text
                    CParaFormatLayer* pParaFormatLayer = CParaFormatLayer::NewL();
                    CleanupStack::PushL(pParaFormatLayer);
                    CCharFormatLayer* pCharFormatLayer = CCharFormatLayer::NewL(); 
                    CleanupStack::PushL(pCharFormatLayer);
            
                    CRichText* pBodyRichText = CRichText::NewL(pParaFormatLayer, pCharFormatLayer, CEditableText::EFlatStorage);
                    CleanupStack::PushL(pBodyRichText);
                    // Insert the contents of a buffer into the document at specified position
                    pBodyRichText->InsertL(0, TPtrC(reinterpret_cast<const TUint16*>(container.textContent().utf16())));
            
                    // Store MIME Header and Body text to message
                    pImEmailMessage->StoreBodyTextWithMimeHeaderL(newMessageId, *pBodyRichText, *pBodyImMimeHeader, pMsvOperationWait->iStatus);
                    pMsvOperationWait->Start();
                    CActiveScheduler::Start();
                    
                    CleanupStack::PopAndDestroy(pBodyRichText);
                    CleanupStack::PopAndDestroy(pCharFormatLayer);
                    CleanupStack::PopAndDestroy(pParaFormatLayer);
                    CleanupStack::PopAndDestroy(pBodyImMimeHeader);
                }
            } else {
                // ContentContainer is attachment
                QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
                // Replace Qt style path separator "/" with Symbian path separator "\"
                filePath.replace(QByteArray("/"), QByteArray("\\"));
                // Make sure that "." folder is translated to application private folder
                // For example: "." = "<drive letter>:\private\<application specific private folder>\" 
                if (filePath.startsWith('.')) {
                    filePath.remove(0,2); // Remove ".\"
                    filePath.insert(0,privateFolderPath().toAscii());
                }
                QString temp_path = QString(filePath);
                TPtrC16 attachmentPath(KNullDesC);
                attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
                TFileName attachmentFileName;
                attachmentFileName.Append(attachmentPath);    
            
                RFile attachmentFile;    
                User::LeaveIfError(attachmentFile.Open(FsSession(), attachmentFileName,
                                                       EFileShareReadersOnly | EFileRead));    
                CleanupClosePushL(attachmentFile);   
                
                CMsvAttachment* pMsvAttachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
                int last = filePath.lastIndexOf("\\");
                int count = filePath.count();
                QByteArray name = filePath.right(count-last-1);
                QString fileName = QString(name);
                CleanupStack::PushL(pMsvAttachment);  
                pMsvAttachment->SetAttachmentNameL(TPtrC(reinterpret_cast<const TUint16*>(fileName.utf16())));
                // Note: Following call transfers ownership of attachmentFile and pMsvAttachment to AttachmentManager
                pImEmailMessage->AttachmentManager().AddAttachmentL(attachmentFile, pMsvAttachment, pMsvOperationWait->iStatus);
                pMsvOperationWait->Start();
                CActiveScheduler::Start();
                CleanupStack::Pop(pMsvAttachment); // Pop attachment from CleanupStack
                CleanupStack::Pop(&attachmentFile); // Pop file from CleanupStack
            }        
        }
    }
    CleanupStack::PopAndDestroy(pImEmailMessage);

    CMsvStore* pMsvStore = pMsvEntry->EditStoreL();
    CleanupStack::PushL(pMsvStore);
    
    pImMimeHeader->StoreL(*pMsvStore);
    
    CImHeader* pImHeader = CImHeader::NewLC();
    pImHeader->RestoreL(*pMsvStore);
    pImHeader->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    pImHeader->SetFromL(TPtrC(reinterpret_cast<const TUint16*>(message.from().addressee().utf16())));
    
    QList<QMessageAddress> toList(message.to());
    if (toList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < toList.size(); ++i) {
            qreceiver = toList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            pImHeader->ToRecipients().AppendL(receiver); 
        }
    }
    
    QList<QMessageAddress> ccList(message.cc());
    if (ccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < ccList.size(); ++i) {
            qreceiver = ccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            pImHeader->CcRecipients().AppendL(receiver); 
        }
    }
        
    QList<QMessageAddress> bccList(message.bcc());
    if (bccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < bccList.size(); ++i) {
            qreceiver = bccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            pImHeader->BccRecipients().AppendL(receiver); 
        }
    }
   
    pImHeader->StoreL(*pMsvStore);
    CleanupStack::PopAndDestroy(pImHeader);

    // Store the changes permanently
    pMsvStore->CommitL();
    CleanupStack::PopAndDestroy(pMsvStore);
    CleanupStack::PopAndDestroy(pImMimeHeader);

    TMsvEntry changedEntry = pMsvEntry->Entry();
    changedEntry.iDescription.Set(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    if (toList.count() > 0) {
        changedEntry.iDetails.Set(TPtrC(reinterpret_cast<const TUint16*>(toList.at(0).addressee().utf16())));
    }
    if (!message.receivedDate().isNull() || !message.date().isNull()) {
        // Change the date to given date
        if (!message.date().isNull()) {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.date());
        } else {
            changedEntry.iDate = qDateTimeToSymbianTTime(message.receivedDate());
        }
    }
    pMsvEntry->ChangeL(changedEntry);
    
    if (imapDestinationFolderId != 0) {
        TMsvId parent = pMsvEntry->Entry().Parent();
        ipImap4Mtm->SwitchCurrentEntryL(parent);        
        CMsvOperation* pMsvOperation = ipImap4Mtm->Entry().MoveL(newMessageId, imapDestinationFolderId,
                                                                 pMsvOperationWait->iStatus);
        CleanupStack::PushL(pMsvOperation);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy(pMsvOperation);
    }
    
    releaseCMsvEntryAndPopFromCleanupStack(pMsvEntry);

    CleanupStack::PopAndDestroy(pMsvOperationWait);
    
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = QMessageId(SymbianHelpers::addIdPrefix(QString::number(newMessageId),SymbianHelpers::EngineTypeMTM));
}

void CMTMEngine::sendEmailL(QMessage &message)
{
    if (!iSessionReady) {
        User::Leave(KErrNotReady);
    }

    QMessageAccountId accountId = message.parentAccountId();
    QMessageAccount messageAccount;
    if (accountId.isValid()) {
        messageAccount = account(accountId);
    } else {
        accountId = defaultAccount(QMessage::Email);
        messageAccount = account(accountId);
    }
    
    bool messageCreated = false;
    if (!message.id().isValid()) {
        // Message id is not valid
        // => Message is not in message store
        //    => New message must be created into Outbox folder
        QMessagePrivate::setStandardFolder(message, QMessage::OutboxFolder);
        message.setParentAccountId(accountId);
        storeEmailL(message);
        messageCreated = true;
    }    
    
    long int messageId = SymbianHelpers::stripIdPrefix(message.id().toString()).toLong();
    if (messageId == 0) {
        User::Leave(KErrNotReady);
    }    

    CMsvEntry* pMsvEntry = retrieveCMsvEntryAndPushToCleanupStack(messageId);
    CMsvOperationWait* pMsvOperationWait = CMsvOperationWait::NewLC();
    
    CMsvOperation* pMsvOperation = NULL;
    if (!messageCreated) {
        // Sending (old) message that's in message store
        // => Copy message from its original location to Outbox folder
        ipSmtpMtm->SwitchCurrentEntryL(pMsvEntry->Entry().Parent());
        pMsvOperation = ipSmtpMtm->Entry().CopyL(messageId, KMsvGlobalOutBoxIndexEntryId, pMsvOperationWait->iStatus);
        pMsvOperationWait->Start();
        CActiveScheduler::Start();
        delete pMsvOperation;
        pMsvOperation = NULL;
    }    
    
    ipSmtpMtm->SwitchCurrentEntryL(pMsvEntry->Entry().Parent());
    // Following sends Email and _moves_ Email from Outbox Folder to Sent Folder
    pMsvOperation = ipSmtpMtm->Entry().CopyL(messageId, messageAccount.d_ptr->_service2EntryId, pMsvOperationWait->iStatus);
    pMsvOperationWait->Start();
    CActiveScheduler::Start();
    delete pMsvOperation;    
    
    CleanupStack::PopAndDestroy(pMsvOperationWait);
    releaseCMsvEntryAndPopFromCleanupStack(pMsvEntry);   
}

QDateTime CMTMEngine::symbianTTimetoQDateTime(const TTime& time) const
{
    TDateTime dateTime = time.DateTime();
    QDate qdate = QDate(dateTime.Year(), static_cast<int>(dateTime.Month())+1, dateTime.Day()+1);
    QTime qtime = QTime(dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()/1000 );
    return QDateTime(qdate, qtime, Qt::UTC);
}

TTime CMTMEngine::qDateTimeToSymbianTTime(const QDateTime& date) const
{
    TDateTime dateTime;
    dateTime.SetYear(date.date().year());
    dateTime.SetMonth(static_cast<TMonth>(date.date().month()-1));
    dateTime.SetDay(date.date().day()-1);
    dateTime.SetHour(date.time().hour());
    dateTime.SetMinute(date.time().minute());
    dateTime.SetSecond(date.time().second());
    dateTime.SetMicroSecond(date.time().msec()*1000);
    return TTime(dateTime);
}

QMessage CMTMEngine::smsMessageL(CMsvEntry& receivedEntry, long int messageId) const
{
    QMessage message;
    int size = 0;
    message.setType(QMessage::Sms);

    CMsvStore* pStore = receivedEntry.ReadStoreL();
    CleanupStack::PushL(pStore);
    
    const TMsvEntry& entry = receivedEntry.Entry();
    message.setDate(symbianTTimetoQDateTime(entry.iDate));
    message.setReceivedDate(symbianTTimetoQDateTime(entry.iDate));

    QMessageAccount account = accountsByType(QMessage::Sms)[0];
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_parentFolderId = createQMessageFolderId(account.d_ptr->_service1EntryId, entry.Parent());
    privateMessage->_parentAccountId = account.id();
    if (!entry.Unread()) {
        privateMessage->_status = privateMessage->_status | QMessage::Read; 
    }

    switch (entry.Priority()) {
    case EMsvHighPriority: 
        message.setPriority(QMessage::HighPriority);
        break;
    case EMsvMediumPriority:
        message.setPriority(QMessage::NormalPriority);
        break;
    case EMsvLowPriority:
        message.setPriority(QMessage::LowPriority);
        break;
    }
    
    if (entry.Parent() == KMsvGlobalInBoxIndexEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
    } else if (entry.Parent() == KMsvDraftEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::DraftsFolder);
    } else if (entry.Parent() == KMsvSentEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
    } else if (entry.Parent() == KMsvDeletedEntryFolderEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::TrashFolder);
    }       
    
    // Read message sender
    ipSmsMtm->SwitchCurrentEntryL(messageId);
    ipSmsMtm->LoadMessageL();
    CSmsHeader& header = ipSmsMtm->SmsHeader();
    message.setFrom(QMessageAddress(QMessageAddress::Phone, QString::fromUtf16(header.FromAddress().Ptr(), header.FromAddress().Length())));
    QMessagePrivate::setSenderName(message, QString::fromUtf16(header.FromAddress().Ptr(), header.FromAddress().Length()));
    
    // Read message recipients
    const CArrayPtrFlat<CSmsNumber>&  array = header.Recipients();
    QMessageAddressList messageAddresslist;
    for (int i=0; i < array.Count(); i++) {
        CSmsNumber* smsNumber = array.At(i);
        TPtrC recipientNumber = smsNumber->Address();
        messageAddresslist.append(QMessageAddress(QMessageAddress::Phone, QString::fromUtf16(recipientNumber.Ptr(), recipientNumber.Length())));
    }
    message.setTo(messageAddresslist);

    // Read message body
    if (pStore->HasBodyTextL()) {
        if (!ipRichText) {
            ipCharFormatLayer = CCharFormatLayer::NewL();
            ipParaFormatLayer = CParaFormatLayer::NewL();
            ipRichText=CRichText::NewL(ipParaFormatLayer, ipCharFormatLayer);
        }
        ipRichText->Reset();
        pStore->RestoreBodyTextL(*ipRichText);
        HBufC* pMessage = HBufC::NewLC(ipRichText->DocumentLength());
        TPtr ptr2(pMessage->Des());
        ipRichText->Extract(ptr2);
        if (pMessage->Length() > 0) {
            size += pMessage->Length();
            message.setBody(QString::fromUtf16(pMessage->Ptr(), pMessage->Length()));
            if (pMessage->Length() <= 100) {
				message.setSubject(QString::fromUtf16(pMessage->Ptr(), pMessage->Length()));
            } else {
				message.setSubject(QString::fromUtf16(pMessage->Ptr(), 100));
            }     
        }
        CleanupStack::PopAndDestroy(pMessage);
    }
    
    CleanupStack::PopAndDestroy(pStore);
    
    privateMessage->_size = size;
    
    return message;
}

QMessage CMTMEngine::mmsMessageL(CMsvEntry& receivedEntry, long int messageId) const
{
    QMessage message;
    int size = 0;
    message.setType(QMessage::Mms);
    
    CMsvStore* pStore = receivedEntry.ReadStoreL();
    CleanupStack::PushL(pStore);

    const TMsvEntry& entry = receivedEntry.Entry();
    message.setDate(symbianTTimetoQDateTime(entry.iDate));
    message.setReceivedDate(symbianTTimetoQDateTime(entry.iDate));

    QMessageAccount account = accountsByType(QMessage::Mms)[0];
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_parentFolderId = createQMessageFolderId(account.d_ptr->_service1EntryId, entry.Parent());
    privateMessage->_parentAccountId = account.id();
    if (!entry.Unread()) {
        privateMessage->_status = privateMessage->_status | QMessage::Read; 
    }

    switch (entry.Priority()) {
    case EMsvHighPriority:
        message.setPriority(QMessage::HighPriority);
        break;
    case EMsvMediumPriority:
        message.setPriority(QMessage::NormalPriority);
        break;
    case EMsvLowPriority:
        message.setPriority(QMessage::LowPriority);
        break;
    }

    if (entry.Parent() == KMsvGlobalInBoxIndexEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
    } else if (entry.Parent() == KMsvDraftEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::DraftsFolder);
    } else if (entry.Parent() == KMsvSentEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
    } else if (entry.Parent() == KMsvDeletedEntryFolderEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::TrashFolder);
    }       
    
    // Read message sender
    ipMmsMtm->SwitchCurrentEntryL(messageId);
    ipMmsMtm->LoadMessageL();
    message.setFrom(QMessageAddress(QMessageAddress::Phone, QString::fromUtf16(ipMmsMtm->Sender().Ptr(), ipMmsMtm->Sender().Length())));
    QMessagePrivate::setSenderName(message, QString::fromUtf16(ipMmsMtm->Sender().Ptr(), ipMmsMtm->Sender().Length()));
    
    // Read message subject
    if (receivedEntry.Entry().iDescription.Length() > 0)  {
        message.setSubject(QString::fromUtf16(receivedEntry.Entry().iDescription.Ptr(),
                                              receivedEntry.Entry().iDescription.Length()));
    }
    
    // Read message body & attachments
    TInt count = pStore->AttachmentManagerL().AttachmentCount();
    TBool textAdded = EFalse;
    if (count > 0) {
        privateMessage->_status = privateMessage->_status | QMessage::HasAttachments;
    }
    for (TInt i = 0; i < count; i++) {
        CMsvAttachment* pAttachment = pStore->AttachmentManagerL().GetAttachmentInfoL(i);
        CleanupStack::PushL(pAttachment);
        if (pAttachment->MimeType() == _L8("text/plain") && !textAdded) {
            // Read message body
            textAdded = ETrue;
            RFile file = pStore->AttachmentManagerL().GetAttachmentFileL(i);
            CleanupClosePushL(file);
            TInt fileSize;
            size += fileSize;
            file.Size(fileSize);
            HBufC8* pFileContent = HBufC8::NewLC(fileSize);
            TPtr8 fileContent(pFileContent->Des());
            file.Read(fileContent);
            HBufC* pMsg = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*pFileContent);
            CleanupStack::PopAndDestroy(pFileContent);
            CleanupStack::PopAndDestroy(&file);
            if (pMsg->Length() > 0) {
                message.setBody(QString::fromUtf16(pMsg->Ptr(), pMsg->Length()));
                if (receivedEntry.Entry().iDescription.Length() <= 0) { // no subject
					if (pMsg->Length() <= 100) {
						message.setSubject(QString::fromUtf16(pMsg->Ptr(), pMsg->Length()));
					}
					else {
						message.setSubject(QString::fromUtf16(pMsg->Ptr(), 100));
					}
                }
            }
            delete pMsg;
        } else {
            QByteArray mimeType;
            QByteArray mimeSubType;
            QByteArray mimeTypeAndSubType = QByteArray((const char *)pAttachment->MimeType().Ptr(), pAttachment->MimeType().Length());
            int index = mimeTypeAndSubType.indexOf("/");
            if (index != -1) {
                mimeType = mimeTypeAndSubType.left(index).trimmed();
                mimeSubType = mimeTypeAndSubType.mid(index + 1).trimmed();
            }
            QByteArray path = QString::fromUtf16(pAttachment->FilePath().Ptr(),pAttachment->FilePath().Length()).toLocal8Bit();
            index = path.lastIndexOf('\\');
            QByteArray name;
            if (index != -1) {
                name = path.right(path.length()-index-1);
            } else {
                name = path;
            }
            int attachmentSize = pAttachment->Size();
            size += attachmentSize;
            QMessageContentContainer attachment = QMessageContentContainerPrivate::from(messageId, pAttachment->Id(), name, mimeType, mimeSubType, attachmentSize);
            appendAttachmentToMessage(message, attachment);
        }
        CleanupStack::PopAndDestroy(pAttachment);
    }

    CleanupStack::PopAndDestroy(pStore);
    
    privateMessage->_size = size;
    
    return message;
}

QMessage CMTMEngine::emailMessageL(CMsvEntry& receivedEntry, long int messageId) const
{
    QMessage message;
    int size = 0;
    message.setType(QMessage::Email);

    const TMsvEntry& entry = receivedEntry.Entry();
    message.setDate(symbianTTimetoQDateTime(entry.iDate));
    message.setReceivedDate(symbianTTimetoQDateTime(entry.iDate));
    
    QMessageAccount messageAccount = account(accountIdByServiceId(entry.iServiceId));
    message.setParentAccountId(messageAccount.id());

    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_parentFolderId = createQMessageFolderId(messageAccount.d_ptr->_service1EntryId, entry.Parent());
    if (!entry.Unread()) {
        privateMessage->_status = privateMessage->_status | QMessage::Read; 
    }
    
    switch (entry.Priority()) {
    case EMsvHighPriority:
        message.setPriority(QMessage::HighPriority);
        break;
    case EMsvMediumPriority:
        message.setPriority(QMessage::NormalPriority);
        break;
    case EMsvLowPriority:
        message.setPriority(QMessage::LowPriority);
        break;
    }

    if (entry.iMtm == KUidMsgTypePOP3) {
        // All incoming POP3 messages are in the root of the POP3 service
        QMessageAccount messageAccount = account(message.parentAccountId());
        if (entry.Parent() == messageAccount.d_ptr->_service1EntryId) {
            QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
        }
    } else if (entry.iMtm == KUidMsgTypeIMAP4) {
        // All incoming IMAP4 messages are in the folders which can
        // be found from the root of the IMAP4 service
        QMessageAccount messageAccount = account(message.parentAccountId());
        CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(entry.Parent());
        if (pEntry->Entry().Parent() == messageAccount.d_ptr->_service1EntryId) {
            QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
        }
        releaseCMsvEntryAndPopFromCleanupStack(pEntry);
    }
    if (entry.Parent() == KMsvDraftEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::DraftsFolder);
    } else if (entry.Parent() == KMsvSentEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
    } else if (entry.Parent() == KMsvDeletedEntryFolderEntryId) {
        QMessagePrivate::setStandardFolder(message,QMessage::TrashFolder);
    }       
    
    CImHeader* emailEntry = CImHeader::NewLC();
    CImEmailMessage* emailMessage = CImEmailMessage::NewLC(receivedEntry);
    CImMimeHeader* pImMimeHeader = CImMimeHeader::NewLC();

    TInt mimeHeaderReadError = KErrNotFound;
    if (receivedEntry.HasStoreL()) {
        CMsvStore* msvStore = receivedEntry.ReadStoreL();
        CleanupStack::PushL(msvStore);
        TRAP_IGNORE(emailEntry->RestoreL(*msvStore));
        size += emailEntry->DataSize();
        TRAP(mimeHeaderReadError, pImMimeHeader->RestoreL(*msvStore));
        if (mimeHeaderReadError == KErrNone) {
            size += pImMimeHeader->Size();
        }
        CleanupStack::PopAndDestroy(); // store
        }
    
    if (!ipRichText) {
        ipCharFormatLayer = CCharFormatLayer::NewL();
        ipParaFormatLayer = CParaFormatLayer::NewL();
        ipRichText = CRichText::NewL(ipParaFormatLayer, ipCharFormatLayer);
    }
    ipRichText->Reset();
    emailMessage->GetBodyTextL(messageId,
                               CImEmailMessage::EThisMessageOnly,
                               *ipRichText,
                               *ipParaFormatLayer,
                               *ipCharFormatLayer);
	
    // From GetBodyTextL() documentation:
    // A list containing the entry Ids for each body text part within
    // the specified message is created during this call. The list can
    // be retrieved after this call has completed by calling Selection().
    if (emailMessage->Selection().Count() > 0) {
        if (pImMimeHeader->ContentType() == KImcvMultipart) {
            // Get body content type (CImMimeHeader) from first body part
            CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(emailMessage->Selection()[0]);
            if (pEntry->HasStoreL()) { 
                CMsvStore* pMsvStore = pEntry->ReadStoreL();
                CleanupStack::PushL(pMsvStore);
                TRAP(mimeHeaderReadError, pImMimeHeader->RestoreL(*pMsvStore));
                if (mimeHeaderReadError == KErrNone) {
                    size += pImMimeHeader->Size();
                }
                CleanupStack::PopAndDestroy(pMsvStore);
            }
            releaseCMsvEntryAndPopFromCleanupStack(pEntry);
        }
    }
    
    QByteArray mimeType;
    if (mimeHeaderReadError == KErrNone) {
        TPtrC8 type = pImMimeHeader->ContentType();
        if (type.Length() > 0) {
            QByteArray contentType = QByteArray((const char *)type.Ptr(),type.Length());
            mimeType.append(contentType);
            mimeType.append("/");
            
            TPtrC8 subType = pImMimeHeader->ContentSubType();
            if (subType.Length() > 0) {
                QByteArray contentSubType = QByteArray((const char *)subType.Ptr(),subType.Length());
                mimeType.append(contentSubType);
            }    
            TUint charset = pImMimeHeader->MimeCharset();
            if (charset) {
                mimeType.append(";charset=");
                if (charset == KCharacterSetIdentifierUcs2)
                    mimeType.append("UTF-16");
                else if (charset == KCharacterSetIdentifierUtf8)
                    mimeType.append("UTF-8");
            }
        }
    }
    CleanupStack::PopAndDestroy(pImMimeHeader);
    
    HBufC* pMessage = HBufC::NewLC(ipRichText->DocumentLength());
    TPtr ptr2(pMessage->Des());
    ipRichText->Extract(ptr2);
    if (pMessage->Length() > 0) {
        QString text = QString::fromUtf16(pMessage->Ptr(),pMessage->Length());
        message.setBody(text, mimeType);
        size += text.size();
    }
    CleanupStack::PopAndDestroy(pMessage);
        
    //attachment(s)
    emailMessage->GetAttachmentsListL(messageId, CImEmailMessage::EAllAttachments, CImEmailMessage::EThisMessageOnly);
    TInt c = emailMessage->AttachmentManager().AttachmentCount();
    if (c > 0) {
        privateMessage->_status = privateMessage->_status | QMessage::HasAttachments; 
    }
    for (TInt i = 0; i < c; i++) {
       CMsvAttachment* pAttachment = emailMessage->AttachmentManager().GetAttachmentInfoL(i);
       CleanupStack::PushL(pAttachment);

       QByteArray mimeType;
       QByteArray mimeSubType;
       CImMimeHeader* pImMimeHeader = CImMimeHeader::NewLC();
       CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(pAttachment->Id());
       if (pEntry->HasStoreL()) { 
           CMsvStore* pMsvStore = pEntry->ReadStoreL();
           CleanupStack::PushL(pMsvStore);
           TRAP(mimeHeaderReadError, pImMimeHeader->RestoreL(*pMsvStore));
           if (mimeHeaderReadError == KErrNone) {
               size += pImMimeHeader->Size();
           }
           CleanupStack::PopAndDestroy(pMsvStore);
       }
       releaseCMsvEntryAndPopFromCleanupStack(pEntry);
       if (mimeHeaderReadError == KErrNone) {
           mimeType = QByteArray((const char *)pImMimeHeader->ContentType().Ptr(), pImMimeHeader->ContentType().Length());
           mimeSubType = QByteArray((const char *)pImMimeHeader->ContentSubType().Ptr(), pImMimeHeader->ContentSubType().Length());
       }
       CleanupStack::PopAndDestroy(pImMimeHeader);
       
       QByteArray name = QString::fromUtf16(pAttachment->AttachmentName().Ptr(), pAttachment->AttachmentName().Length()).toLocal8Bit();
       int attachmentSize = pAttachment->Size();
       size += attachmentSize;
       QMessageContentContainer attachment = QMessageContentContainerPrivate::from(messageId, pAttachment->Id(), name, mimeType, mimeSubType, attachmentSize);
       appendAttachmentToMessage(message, attachment);
       CleanupStack::PopAndDestroy(pAttachment);
    }
     
    //from
    TPtrC from = emailEntry->From();
    if (from.Length() > 0) {
        message.setFrom(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(from.Ptr(), from.Length())));
        QMessagePrivate::setSenderName(message, QString::fromUtf16(from.Ptr(), from.Length()));
    } else {
        if (entry.iDetails.Length() > 0)  {
            QString fromString = QString::fromUtf16(receivedEntry.Entry().iDetails.Ptr(), receivedEntry.Entry().iDetails.Length());
            message.setFrom(QMessageAddress(QMessageAddress::Email, fromString));
            QMessagePrivate::setSenderName(message, fromString);
        }
    }
   
    //to
    CDesCArray& toArray = emailEntry->ToRecipients();
    QList<QMessageAddress> toList;
    for (TInt i = 0; i < toArray.Count(); i++)
    {
        TPtrC16 to(toArray.MdcaPoint(i));
        toList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(to.Ptr(), to.Length())));            
    }
    message.setTo(toList);
    
    //cc
    CDesCArray& ccArray = emailEntry->CcRecipients();
    QList<QMessageAddress> ccList;
    for (TInt i = 0; i < ccArray.Count(); i++)
    {
        TPtrC16 cc(ccArray.MdcaPoint(i));
        ccList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(cc.Ptr(), cc.Length())));            
    }
    message.setCc(ccList);
    
    //bcc
    CDesCArray& bccArray = emailEntry->BccRecipients();
    QList<QMessageAddress> bccList;
    for (TInt i = 0; i < bccArray.Count(); i++)
    {
        TPtrC16 bcc(bccArray.MdcaPoint(i));
        bccList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(bcc.Ptr(), bcc.Length())));            
    }
    message.setBcc(bccList);
    
    // Read message subject
    TPtrC subject = emailEntry->Subject();
    if (subject.Length() > 0) {
        message.setSubject(QString::fromUtf16(subject.Ptr(), subject.Length()));
    } else {
        if (entry.iDescription.Length() > 0)  {
            message.setSubject(QString::fromUtf16(receivedEntry.Entry().iDescription.Ptr(),
                                                  receivedEntry.Entry().iDescription.Length()));
        }    
    }
    
    CleanupStack::PopAndDestroy(emailMessage);
    CleanupStack::PopAndDestroy(emailEntry);

    privateMessage->_size = size;
    
    return message;
}

void CMTMEngine::appendAttachmentToMessage(QMessage& message, QMessageContentContainer& attachment) const
{
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    QMessageContentContainerPrivate* container = QMessagePrivate::containerImplementation(message);
    
    if (container->_attachments.isEmpty()) {
        QMessageContentContainerId existingBodyId(message.bodyId());
        if (existingBodyId == QMessageContentContainerPrivate::bodyContentId()) {
            // The body content is in the message itself - move it to become the first attachment
            QMessageContentContainer newBody(message);
            QMessageContentContainerPrivate::implementation(newBody)->setDerivedMessage(0);
    
            container->setContentType("multipart", "mixed", "");
            privateMessage->_bodyId = container->prependContent(newBody);
        } else {
            // This message is now multipart
            container->setContentType("multipart", "mixed", "");
        }
    
        container->_available = true;
    }
    
    container->appendContent(attachment);
    
    bool haveAttachments = !container->_attachments.isEmpty();
    message.setStatus(QMessage::HasAttachments,haveAttachments);
    
    privateMessage->_modified = true;
}

QByteArray CMTMEngine::attachmentContent(long int messageId, unsigned int attachmentId)
{
    QByteArray result;
    TRAP_IGNORE(result = attachmentContentL(messageId, attachmentId));
    return result;
}

QByteArray CMTMEngine::attachmentContentL(long int messageId, unsigned int attachmentId)
{
    QByteArray result;
    
    CMsvEntry* pEntry = retrieveCMsvEntryAndPushToCleanupStack(messageId);
    if (pEntry->Entry().iMtm == KUidMsgTypeMultimedia) { // MMS
        CMsvStore* pStore = pEntry->ReadStoreL();
        CleanupStack::PushL(pStore);
        RFile file = pStore->AttachmentManagerL().GetAttachmentFileL(attachmentId);
        CleanupClosePushL(file);
        TInt fileSize;
        file.Size(fileSize);
        TBuf<KMaxFileName> fileName;
        file.Name(fileName);
        HBufC8* pFileContent = HBufC8::NewL(fileSize);
        TPtr8 fileContent(pFileContent->Des());
        file.Read(fileContent);
        CleanupStack::PopAndDestroy(&file);
        CleanupStack::PushL(pFileContent);
        
        result = QByteArray((char*)pFileContent->Ptr(),pFileContent->Length());
        
        CleanupStack::PopAndDestroy(pFileContent);
        CleanupStack::PopAndDestroy(pStore);
    } else { // Email
        CImEmailMessage* pImEmailMessage = CImEmailMessage::NewLC(*pEntry);
        RFile file = pImEmailMessage->AttachmentManager().GetAttachmentFileL(attachmentId);
        CleanupClosePushL(file);
        TInt fileSize;
        file.Size(fileSize);
        TBuf<KMaxFileName> fileName;
        file.Name(fileName);
        HBufC8* pFileContent = HBufC8::NewL(fileSize);
        TPtr8 fileContent(pFileContent->Des());
        file.Read(fileContent);
        CleanupStack::PopAndDestroy(&file);
        CleanupStack::PushL(pFileContent);
        
        result = QByteArray((char*)pFileContent->Ptr(),pFileContent->Length());
        
        CleanupStack::PopAndDestroy(pFileContent);
        CleanupStack::PopAndDestroy(pImEmailMessage);
    }
    releaseCMsvEntryAndPopFromCleanupStack(pEntry);
    
    return result;
}

QString CMTMEngine::attachmentTextContent(long int messageId, unsigned int attachmentId, const QByteArray &charset)
{
    QString result;
    
    QByteArray data = attachmentContent(messageId, attachmentId);
    if (!data.isEmpty()) {
        // Convert attachment data to string form
        QTextCodec *codec;
        if (!charset.isEmpty()) {
            codec = QTextCodec::codecForName(charset);
        } else {
            codec = QTextCodec::codecForLocale();
        }

        if (codec) {
            result = codec->toUnicode(data);
        }
    }
    
    return result;
}

QMessage CMTMEngine::pop3MessageL(CMsvEntry& /*receivedEntry*/, long int /*messageId*/) const
{
    return QMessage();
}

QMessage CMTMEngine::imap4MessageL(CMsvEntry& /*receivedEntry*/, long int /*messageId*/) const
{
    return QMessage();
}

CMsvEntry* CMTMEngine::retrieveCMsvEntryAndPushToCleanupStack(TMsvId id) const
{
    CMsvEntry* pEntry = NULL;

    if (iCmsvEntryPoolFree.Count() > 0) {
        pEntry = iCmsvEntryPoolFree[iCmsvEntryPoolFree.Count()-1];
        TInt retVal = iCmsvEntryPoolInUse.Append(pEntry);
        if (retVal != KErrNone) {
            delete pEntry;
            pEntry = NULL;
        }
    } else {
        if (id == 0) {
            id = KMsvRootIndexEntryId;
        }
        TRAPD(err, pEntry = ipMsvSession->GetEntryL(id));
        if (err == KErrNone) {
            TInt retVal = iCmsvEntryPoolInUse.Append(pEntry);
            if (retVal != KErrNone) {
                delete pEntry;
                pEntry = NULL;
            }
        }
    }
    
    if (id != 0 && pEntry) {
        TRAPD(err, pEntry->SetEntryL(id));
        if (err != KErrNone) {
            TInt pos = iCmsvEntryPoolInUse.Find(pEntry);
            if (pos != KErrNotFound) {
                iCmsvEntryPoolInUse.Remove(pos);
                TInt retVal = iCmsvEntryPoolFree.Append(pEntry);
                if (retVal != KErrNone) {
                    delete pEntry;
                }
            }        
            pEntry = NULL;
        }
    }
    
    if (pEntry) {
        TCleanupItem entryCleanup(cmsvEntryCleanup, pEntry);
        CleanupStack::PushL(entryCleanup);
    }

    return pEntry;
}

void CMTMEngine::cmsvEntryCleanup(TAny* aCMsvEntry)
{
    CMTMEngine* pMTMEngine = mtmEngine();
    CMsvEntry* pEntry = (CMsvEntry*)aCMsvEntry;

    TInt pos = pMTMEngine->iCmsvEntryPoolInUse.Find(pEntry);
    if (pos != KErrNotFound) {
        pMTMEngine->iCmsvEntryPoolInUse.Remove(pos);
        TInt retVal = pMTMEngine->iCmsvEntryPoolFree.Append(pEntry);
        if (retVal != KErrNone) {
            delete pEntry;
        }
    }
}

void CMTMEngine::releaseCMsvEntryAndPopFromCleanupStack(CMsvEntry* pEntry) const
{
    if (pEntry) {
        CleanupStack::Pop(); // Entry cleanup
    }

    TInt pos = iCmsvEntryPoolInUse.Find(pEntry);
    if (pos != KErrNotFound) {
        iCmsvEntryPoolInUse.Remove(pos);
        TInt retVal = iCmsvEntryPoolFree.Append(pEntry);
        if (retVal != KErrNone) {
            delete pEntry;
        }
    }
}

QMessageManager::NotificationFilterId CMTMEngine::registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
                                                                           const QMessageFilter &filter)
{
    ipMessageStorePrivate = &aPrivateStore;
    iListenForNotifications = true;    

    int filterId = ++_filterId;
    _filters.insert(filterId, filter);
    return filterId;
}

void CMTMEngine::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    _filters.remove(notificationFilterId);
    if (_filters.count() == 0) {
        iListenForNotifications = false;
    }
}

void CMTMEngine::notification(TMsvSessionEvent aEvent, TUid aMsgType, TMsvId aFolderId, TMsvId aMessageId)
{
    if (aFolderId == 0x100001 || aFolderId == 0x100002) { // MMS Notifications Folder
        // Ignore MMS Notifications <=> Wait until actual MMS message has been received
        return;
    }

    QMessageManager::NotificationFilterIdSet matchingFilters;

    // Copy the filter map to protect against modification during traversal
    QMap<int, QMessageFilter> filters(_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();
    QMessage message;
    bool messageRetrieved = false;
    bool unableToReadAndFilterMessage = false;
    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());

        if (filter.isEmpty()) {
            // Empty filter matches to all messages
            matchingFilters.insert(it.key());
        } else {
            QMessageFilterPrivate* privateMessageFilter = QMessageFilterPrivate::implementation(filter);
            if (privateMessageFilter->_field == QMessageFilterPrivate::Type) {
                if (aMsgType == KUidMsgTypeSMS) {
                    message.setType(QMessage::Sms);
                } else if (aMsgType == KUidMsgTypeMultimedia) {
                    message.setType(QMessage::Mms);
                } else if (aMsgType == KUidMsgTypeSMTP ||
                           aMsgType == KUidMsgTypePOP3 ||
                           aMsgType == KUidMsgTypeIMAP4) {
                    message.setType(QMessage::Email);
                } else {
                    message.setType(QMessage::NoType);
                }
            } else if ((privateMessageFilter->_field == QMessageFilterPrivate::StandardFolder) &&
                       (aMsgType == KUidMsgTypeSMS || aMsgType == KUidMsgTypeMultimedia)) {
                if (aFolderId == KMsvGlobalInBoxIndexEntryId) {
                    QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
                } else if (aFolderId == KMsvDraftEntryId) {
                    QMessagePrivate::setStandardFolder(message,QMessage::DraftsFolder);
                } else if (aFolderId == KMsvSentEntryId) {
                    QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
                } else if (aFolderId == KMsvDeletedEntryFolderEntryId) {
                    QMessagePrivate::setStandardFolder(message,QMessage::TrashFolder);
                }
            } else if (!messageRetrieved) {
                message = this->message(QMessageId(SymbianHelpers::addIdPrefix(QString::number(aMessageId),SymbianHelpers::EngineTypeMTM)));
                if (message.type() == QMessage::NoType) {
                    unableToReadAndFilterMessage = true;
                    matchingFilters.clear();
                    break;
                } else {
                    messageRetrieved = true;
                }
            }
            if (privateMessageFilter->filter(message)) {
                matchingFilters.insert(it.key());
            }
        }
    }

    QMessageStorePrivate::NotificationType notificationType = QMessageStorePrivate::Removed;
    if (aEvent == EMsvEntriesCreated) {
        notificationType = QMessageStorePrivate::Added; 
    } else if (aEvent == EMsvEntriesChanged || aEvent == EMsvEntriesMoved) {
        notificationType = QMessageStorePrivate::Updated; 
    } if (aEvent == EMsvEntriesDeleted) {
        notificationType = QMessageStorePrivate::Removed; 
    }

    if (matchingFilters.count() > 0) {
        // Check if there are already pending events for
        // currently handled MessageId
        bool pendingEventsForCurrentlyHandledMessageId = false;
        for (int i=0; i < iUndeliveredMessageEvents.count(); i++) {
            if (iUndeliveredMessageEvents[i].messageId == aMessageId) {
                pendingEventsForCurrentlyHandledMessageId = true;
                break;
            }
        }
        if (pendingEventsForCurrentlyHandledMessageId) {
            // There are pending notification events for this messageId.
            // => Add new notification event to notification event queue to
            //    make sure that all events will be delivered in correct order.
            MessageEvent event;
            event.messageId = aMessageId;
            event.notificationType = notificationType;
            event.matchingFilters = matchingFilters;
            event.unfiltered = false;
            if (iUndeliveredMessageEvents.count() == 0) {
                iDeliveryTriesCounter = 0;
            }
            iUndeliveredMessageEvents.append(event);
            tryToDeliverMessageNotifications();
        } else {
            // No pending notification events for this messageId.
            // => Deliver notification immediately
            ipMessageStorePrivate->messageNotification(notificationType,
                                                       QMessageId(SymbianHelpers::addIdPrefix(QString::number(aMessageId),SymbianHelpers::EngineTypeMTM)),
                                                       matchingFilters);
        }
    } else if (unableToReadAndFilterMessage) {
        if (notificationType != QMessageStorePrivate::Removed) {
            MessageEvent event;
            event.messageId = aMessageId;
            event.notificationType = notificationType;
            event.unfiltered = true;
            if (iUndeliveredMessageEvents.count() == 0) {
                iDeliveryTriesCounter = 0;
            }
            iUndeliveredMessageEvents.append(event);
            tryToDeliverMessageNotifications();
        } else {
            // Message was removed before reading was possible
            // => All avents related to removed messageId can be ignored
            // => Remove all related events from undelivered message events queue
            for (int i=iUndeliveredMessageEvents.count()-1; i >= 0; i--) {
                if (iUndeliveredMessageEvents[i].messageId == aMessageId) {
                    iUndeliveredMessageEvents.removeAt(i);
                }
            }
        }
    }
}

void CMTMEngine::tryToDeliverMessageNotifications()
{
    if (!IsActive()) {
        int count = iUndeliveredMessageEvents.count();
        while (count--) {
            // Try to deliver the oldest message event notification first
            MessageEvent event = iUndeliveredMessageEvents[0];
            bool eventHandlingPossible = true;
            if (event.notificationType != QMessageStorePrivate::Removed && event.unfiltered) {
                QMessage message = this->message(QMessageId(SymbianHelpers::addIdPrefix(QString::number(event.messageId),SymbianHelpers::EngineTypeMTM)));
                if (message.type() == QMessage::NoType) {
                    eventHandlingPossible = false;
                } else {
                    event.matchingFilters.clear();
                    // Copy the filter map to protect against modification during traversal
                    QMap<int, QMessageFilter> filters(_filters);
                    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();
                    for ( ; it != end; ++it) {
                        const QMessageFilter &filter(it.value());
                        if (filter.isEmpty()) {
                            // Empty filter matches to all messages
                            event.matchingFilters.insert(it.key());
                        } else {
                            QMessageFilterPrivate* privateMessageFilter = QMessageFilterPrivate::implementation(filter);
                            if (privateMessageFilter->filter(message)) {
                                event.matchingFilters.insert(it.key());
                            }
                        }
                    }
                }
            }
    
            if (eventHandlingPossible) {
                // New message entry was ready to be read
                // Remove message event from queue
                iUndeliveredMessageEvents.removeFirst();
                iDeliveryTriesCounter = 0;
                if (event.matchingFilters.count() > 0) { 
                    // Deliver message event notification
                    ipMessageStorePrivate->messageNotification(event.notificationType,
                                                               QMessageId(SymbianHelpers::addIdPrefix(QString::number(event.messageId),SymbianHelpers::EngineTypeMTM)),
                                                               event.matchingFilters);
                }
            } else {
                // New message entry was NOT ready to be read
                iDeliveryTriesCounter++;
                if (iDeliveryTriesCounter < 50) { 
                    // Wait for 0.1 seconds to see if message would
                    // be ready to be read & delivered after wait.
                    iTimer.After(iStatus, KWaitAfterReceivedMessage);
                    SetActive();
                    break;
                } else {
                    // Remove problematic message event from queue
                    iUndeliveredMessageEvents.removeFirst();
                    iDeliveryTriesCounter = 0;
                }
            }
        }
    }
}

void CMTMEngine::RunL()
{
    tryToDeliverMessageNotifications();
}


void CMTMEngine::DoCancel()
{
    iTimer.Cancel();
}

void CMTMEngine::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
                                    TAny* aArg2, TAny* /*aArg3*/)
{
    switch (aEvent) {
    case EMsvServerReady:
        iSessionReady = ETrue;
        break;
    case EMsvEntriesCreated:
    case EMsvEntriesChanged:
    case EMsvEntriesDeleted:
    case EMsvEntriesMoved:
        if (aArg2 && iListenForNotifications) {
            CMsvEntrySelection* entries = static_cast<CMsvEntrySelection*>(aArg1);
            
            if (entries != NULL) {
               TInt count = entries->Count();
               while (count--) {
                   const TMsvId id = (*entries)[count];
                   if (aEvent == EMsvEntriesDeleted) {
                       notification(aEvent, TUid(), *(static_cast<TMsvId*>(aArg2)), id);
                   } else {
                       CMsvEntry* pReceivedEntry = NULL;
                       TRAPD(err, pReceivedEntry = ipMsvSession->GetEntryL(id));
                       if (err == KErrNone) {
                           const TMsvEntry& entry = pReceivedEntry->Entry();
                           if (entry.iType == KUidMsvMessageEntry) {
                               notification(aEvent, entry.iMtm, *(static_cast<TMsvId*>(aArg2)), id);
                           }
                           delete pReceivedEntry;
                       }
                   }
               }
            }
        }
        break;
    
    default:
        break;
    }
}

CMessagesFindOperation::CMessagesFindOperation(CMTMEngine& aOwner, CMsvSession* apMsvSession, int aOperationId)
    : CActive(CActive::EPriorityStandard), iOwner(aOwner), ipMsvSession(apMsvSession), iOperationId(aOperationId),
        iResultCorrectlyOrdered(false)
{
    CActiveScheduler::Add(this);
    iTimer.CreateLocal();
}

CMessagesFindOperation::~CMessagesFindOperation()
{
    Cancel();
    iTimer.Close();
    delete ipMsvFindOperation;
    delete ipEntrySelection;
}

void CMessagesFindOperation::DoCancel()
{
    ipMsvFindOperation->Cancel();
}

void CMessagesFindOperation::filterAndOrderMessages(const QMessageFilter &filter, const QMessageSortOrder& sortOrder,
                                                    QString body, QMessageDataComparator::MatchFlags matchFlags)
{
    iFilterList.clear();
    iFilterList.append(filter);
    filterAndOrderMessages(iFilterList, sortOrder, body, matchFlags);
}

void CMessagesFindOperation::filterAndOrderMessages(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                                    const QMessageSortOrder& sortOrder,
                                                    QString body,
                                                    QMessageDataComparator::MatchFlags matchFlags)
{
    delete ipMsvFindOperation;
    ipMsvFindOperation = NULL;
    delete ipEntrySelection;
    ipEntrySelection = NULL;
    iNumberOfHandledFilters = 0;

    if (filters.count() == 0) {
        iIdList = QMessageIdList();
        iTimer.After(iStatus, 1);
        if (!IsActive()) {
            SetActive();
        }
        return;
    }
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filters[iNumberOfHandledFilters]);
    if ((filters.count() == 1) &&
        (pf->_field == QMessageFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => return empty QMessageIdList 
            iIdList = QMessageIdList();
        } else {
            // There is only one filter: empty QMessageFilter()
            // => return all messages
            ipEntrySelection = new(ELeave)CMsvEntrySelection;
            getAllMessagesL(iOrdering);
            iIdList = QMessageIdList();
            for (int i=0; i < ipEntrySelection->Count(); i++) {
                iIdList.append(QMessageId(SymbianHelpers::addIdPrefix(QString::number((*ipEntrySelection)[i]),SymbianHelpers::EngineTypeMTM)));
            }
        }
        iNumberOfHandledFilters++;
        iTimer.After(iStatus, 1);
        if (!IsActive()) {
            SetActive();
        }
        return;
    }
    
    // Set sortOrder
    if (!sortOrder.isEmpty() ) {
        QMessageSortOrderPrivate* privateMessageOrdering = QMessageSortOrderPrivate::implementation(sortOrder);
        iOrdering.SetShowInvisibleEntries(EFalse);
        QPair<QMessageSortOrderPrivate::Field, Qt::SortOrder> fieldOrder = privateMessageOrdering->_fieldOrderList.at(0);
        switch (fieldOrder.first) {
        case QMessageSortOrderPrivate::Type:
            iOrdering.SetGroupByMtm(true);
            break;
        case QMessageSortOrderPrivate::Sender:
            if (fieldOrder.second == Qt::AscendingOrder) {
                iOrdering.SetSorting(EMsvSortByDetails); // To/From (A-Z folded)
            } else {
                iOrdering.SetSorting(EMsvSortByDetailsReverse); // To/From (Z-A folded)
            }
            break;
        case QMessageSortOrderPrivate::Recipients:
            if (fieldOrder.second == Qt::AscendingOrder) {
                iOrdering.SetSorting(EMsvSortByDetails); // To/From (A-Z folded)
            } else {
                iOrdering.SetSorting(EMsvSortByDetailsReverse); // To/From (Z-A folded)
            }
            break;
        case QMessageSortOrderPrivate::Subject:
            if (fieldOrder.second == Qt::AscendingOrder) {
                iOrdering.SetSorting(EMsvSortByDescription); // Description (A-Z folded)
            } else {
                iOrdering.SetSorting(EMsvSortByDescriptionReverse); // Description (Z-A folded)
            }
            break;
        case QMessageSortOrderPrivate::TimeStamp:
            if (fieldOrder.second == Qt::AscendingOrder) {
                iOrdering.SetSorting(EMsvSortByDate); // Date (earliest-latest) 
            } else {
                iOrdering.SetSorting(EMsvSortByDateReverse); // Date (latest-earliest)
            }
            break;
        case QMessageSortOrderPrivate::ReceptionTimeStamp:
            if (fieldOrder.second == Qt::AscendingOrder) {
                iOrdering.SetSorting(EMsvSortByDate); // Date (earliest-latest) 
            } else {
                iOrdering.SetSorting(EMsvSortByDateReverse); // Date (latest-earliest)
            }
            break;
        case QMessageSortOrderPrivate::Read:
            //TODO:
            break;
        case QMessageSortOrderPrivate::HasAttachments:
            //TODO:
            break;
        case QMessageSortOrderPrivate::Incoming:
            //TODO:
            break;
        case QMessageSortOrderPrivate::Removed:
            //TODO:
            break;
        case QMessageSortOrderPrivate::Priority:
            //iOrdering.SetGroupByPriority(true);
            break;
        case QMessageSortOrderPrivate::Size:
            if (fieldOrder.second == Qt::AscendingOrder) {
                iOrdering.SetSorting(EMsvSortBySize); // (smallest-largest) 
            } else {
                iOrdering.SetSorting(EMsvSortBySizeReverse); // (largest-smallest)
            }
            break;
        }
    }

    switch (pf->_field) {
    case QMessageFilterPrivate::Id:
        {
        iNumberOfHandledFilters++;
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageId
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (!pf->_value.isNull() && pf->_value.toString().length() > QString(SymbianHelpers::mtmPrefix).length()) {
                if (cmp == QMessageDataComparator::Equal) {
                    long int messageId = SymbianHelpers::stripIdPrefix(pf->_value.toString()).toLong();
                    CMsvEntry* pEntry = iOwner.retrieveCMsvEntryAndPushToCleanupStack(messageId);
                    if (pEntry) {
                        const TMsvEntry& entry = pEntry->Entry();
                        if (entry.iType == KUidMsvMessageEntry) {
                            ipEntrySelection = new(ELeave)CMsvEntrySelection;
                            ipEntrySelection->AppendL(messageId);
                        }
                        iOwner.releaseCMsvEntryAndPopFromCleanupStack(pEntry);
                    }
                    iResultCorrectlyOrdered = true;
                } else { // NotEqual
                    ipEntrySelection = new(ELeave)CMsvEntrySelection;
                    getAllMessagesL(iOrdering);
                    long int messageId = SymbianHelpers::stripIdPrefix(pf->_value.toString()).toLong();
                    for (int i=0; i < ipEntrySelection->Count(); i++) {
                        if (ipEntrySelection->At(i) == messageId) {
                            ipEntrySelection->Delete(i);
                            break;
                        }
                    }
                }
            } else {
                ipEntrySelection = new(ELeave)CMsvEntrySelection;
                if (cmp == QMessageDataComparator::NotEqual) {
                    getAllMessagesL(iOrdering);
                }
            }
        } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (pf->_ids.count() > 0) { // QMessageIdList
                if (cmp == QMessageDataComparator::Includes) {
                    ipEntrySelection = new(ELeave)CMsvEntrySelection;
                    for (int i=0; i < pf->_ids.count(); i++) {
                        long int messageId = SymbianHelpers::stripIdPrefix(pf->_ids[i].toString()).toLong();
                        CMsvEntry* pEntry = iOwner.retrieveCMsvEntryAndPushToCleanupStack(messageId);
                        if (pEntry) {
                            const TMsvEntry& entry = pEntry->Entry();
                            if (entry.iType == KUidMsvMessageEntry) {
                                ipEntrySelection->AppendL(messageId);
                            }
                            iOwner.releaseCMsvEntryAndPopFromCleanupStack(pEntry);
                        }
                    }
                } else { // Excludes
                    ipEntrySelection = new(ELeave)CMsvEntrySelection;
                    getAllMessagesL(iOrdering);
                    for (int i=0; i < pf->_ids.count(); i++) {
                        long int messageId = SymbianHelpers::stripIdPrefix(pf->_ids[i].toString()).toLong();
                        for (int i=0; i < ipEntrySelection->Count(); i++) {
                            if (ipEntrySelection->At(i) == messageId) {
                                ipEntrySelection->Delete(i);
                                break;
                            }
                        }
                    }
                }
            } else {
                ipEntrySelection = new(ELeave)CMsvEntrySelection;
                if (cmp == QMessageDataComparator::Excludes) {
                    getAllMessagesL(iOrdering);
                }
                /*// QMessageFilter
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }*/
            }
        }
        break;
        }
    case QMessageFilterPrivate::ParentAccountId:
        {
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageAccountId
            iNumberOfHandledFilters++;
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                ipEntrySelection = new(ELeave)CMsvEntrySelection;
                
                QMessageAccount messageAccount = iOwner.account(pf->_value.toString());
                QMessageFilterPrivate* privateFilter = NULL;
                if (filters.count() > iNumberOfHandledFilters) {
                    privateFilter = QMessageFilterPrivate::implementation(filters[iNumberOfHandledFilters]);
                    if (privateFilter->_field != QMessageFilterPrivate::StandardFolder) {
                        privateFilter = NULL;
                    } else {
                        iNumberOfHandledFilters++;
                    }
                }
                getAccountSpecificMessagesL(messageAccount, iOrdering, privateFilter);
            } else { // NotEqual
                ipEntrySelection = new(ELeave)CMsvEntrySelection;
                
                QStringList exludedAccounts;
                exludedAccounts << pf->_value.toString();
                
                QMessageFilterPrivate* privateFilter = NULL;
                for (int i=iNumberOfHandledFilters; i < filters.count(); i++) {
                    privateFilter = QMessageFilterPrivate::implementation(filters[i]);
                    if (privateFilter->_field == QMessageFilterPrivate::ParentAccountId &&
                        privateFilter->_comparatorType == QMessageFilterPrivate::Equality) {
                        cmp = static_cast<QMessageDataComparator::EqualityComparator>(privateFilter->_comparatorValue);
                        if (cmp == QMessageDataComparator::NotEqual) {
                            exludedAccounts << privateFilter->_value.toString();
                            iNumberOfHandledFilters++;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                privateFilter = NULL;
                if (filters.count() > iNumberOfHandledFilters) {
                    privateFilter = QMessageFilterPrivate::implementation(filters[iNumberOfHandledFilters]);
                    if (privateFilter->_field == QMessageFilterPrivate::StandardFolder &&
                        privateFilter->_comparatorType == QMessageFilterPrivate::Equality) {
                        cmp = static_cast<QMessageDataComparator::EqualityComparator>(privateFilter->_comparatorValue);
                        if (cmp == QMessageDataComparator::Equal) {
                            iNumberOfHandledFilters++;
                        }
                    } else {
                        privateFilter = NULL;
                    }
                }
                
                foreach (QMessageAccount value, iOwner.iAccounts) {
                    if (!exludedAccounts.contains(value.id().toString())) {
                        getAccountSpecificMessagesL(value, iOrdering, privateFilter);
                    }
                }
            }
        } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) { // QMessageAccountFilter
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                //TODO:
            } else { // Excludes
                //TODO:
            }
        }
        break;
        }
    case QMessageFilterPrivate::ParentFolderId:
        {
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageFolderId
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                long int folderId = iOwner.folderIdFromQMessageFolderId(QMessageFolderId(pf->_value.toString()));
                long int serviceEntryId = iOwner.serviceEntryIdFromQMessageFolderId(QMessageFolderId(pf->_value.toString()));
                QMessageAccount messageAccount = iOwner.account(iOwner.accountIdByServiceId(serviceEntryId));
                if (messageAccount.messageTypes() == QMessage::Email) {
                    iNumberOfHandledFilters++;
                    CMsvEntryFilter* pFilter = CMsvEntryFilter::NewLC();
                    pFilter->SetOrder(iOrdering);
                    pFilter->SetType(KUidMsvMessageEntry);
                    // Get POP3 or IMAP messages from folder 
                    CMsvEntrySelection* pEntrySelection1 = new(ELeave)CMsvEntrySelection;
                    CleanupStack::PushL(pEntrySelection1);
                    pFilter->SetService(messageAccount.d_ptr->_service1EntryId);
                    ipMsvSession->GetChildIdsL(folderId, *pFilter, *pEntrySelection1);
                    // Get SMTP messages from folder 
                    CMsvEntrySelection* pEntrySelection2 = new(ELeave)CMsvEntrySelection;
                    CleanupStack::PushL(pEntrySelection2);
                    pFilter->SetService(messageAccount.d_ptr->_service2EntryId);
                    ipMsvSession->GetChildIdsL(folderId, *pFilter, *pEntrySelection2);
                    if (pEntrySelection1->Count() > 0 && pEntrySelection2->Count() > 0) {
                        ipEntrySelection = pEntrySelection1;
                        for (int i = 0; i < pEntrySelection2->Count(); i++) {
                            ipEntrySelection->AppendL(pEntrySelection2->At(i));
                        }
                        CleanupStack::PopAndDestroy(pEntrySelection2);
                        CleanupStack::Pop(pEntrySelection1);
                    } else if (pEntrySelection1->Count() > 0) {
                        ipEntrySelection = pEntrySelection1;
                        CleanupStack::PopAndDestroy(pEntrySelection2);
                        CleanupStack::Pop(pEntrySelection1);
                        iResultCorrectlyOrdered = true;
                    } else {
                        // (pEntrySelection2->Count() > 0) or both selections are empty
                        ipEntrySelection = pEntrySelection2;
                        CleanupStack::Pop(pEntrySelection2);
                        CleanupStack::PopAndDestroy(pEntrySelection1);
                        iResultCorrectlyOrdered = true;
                    }
                    CleanupStack::PopAndDestroy(pFilter);
                } else if (messageAccount.messageTypes() == QMessage::Sms || messageAccount.messageTypes() == QMessage::Mms) {
                    iNumberOfHandledFilters++;
                    CMsvEntry* pEntry = iOwner.retrieveCMsvEntryAndPushToCleanupStack(serviceEntryId);
                    if (pEntry) {
                        TUid mtm = pEntry->Entry().iMtm;
                        iOwner.releaseCMsvEntryAndPopFromCleanupStack(pEntry);
                        CMsvEntryFilter* pFilter = CMsvEntryFilter::NewLC();
                        pFilter->SetMtm(mtm);
                        pFilter->SetOrder(iOrdering);
                        pFilter->SetType(KUidMsvMessageEntry);
                        ipEntrySelection = new(ELeave)CMsvEntrySelection;
                        ipMsvSession->GetChildIdsL(folderId, *pFilter, *ipEntrySelection);
                        CleanupStack::PopAndDestroy(pFilter);
                    }
                    iResultCorrectlyOrdered = true;
                }
            } else { // NotEqual
                // TODO:
            }
        } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) { // QMessageFolderFilter
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                // TODO:
            } else { // Excludes
                // TODO:
            }
        }
        break;
        }
    case QMessageFilterPrivate::AncestorFolderIds:
        {
        iNumberOfHandledFilters++;
        if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (!pf->_value.isNull()) { // QMessageFolderId
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            } else { // QMessageFolderFilter
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Type:
        {
        iNumberOfHandledFilters++;
        QMessageFilterPrivate* privateFilter = NULL;
        // Check if next filter is StandardFolder filter 
        if (filters.count() > iNumberOfHandledFilters) {
            privateFilter = QMessageFilterPrivate::implementation(filters[iNumberOfHandledFilters]);
            if (privateFilter->_field != QMessageFilterPrivate::StandardFolder) {
                privateFilter = NULL;
            } else {
                iNumberOfHandledFilters++;
            }
        }
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessage::Type
            QMessage::Type type = static_cast<QMessage::Type>(pf->_value.toInt()); 
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                ipEntrySelection = new(ELeave)CMsvEntrySelection;
                QMessageAccountIdList accountIds = iOwner.accountsByType(type);
                for (int i = 0; i < accountIds.count(); i++) {
                    QMessageAccount messageAccount = iOwner.account(accountIds[i]);
                    getAccountSpecificMessagesL(messageAccount, iOrdering, privateFilter);
                }
            } else { // NotEqual
                foreach (QMessageAccount value, iOwner.iAccounts) {
                    if (!(value.messageTypes() & type)) {
                        getAccountSpecificMessagesL(value, iOrdering, privateFilter);
                    }
                }
            }
        } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) { // QMessage::TypeFlags
            QMessage::TypeFlags typeFlags = static_cast<QMessage::TypeFlags>(pf->_value.toInt());
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                foreach (QMessageAccount value, iOwner.iAccounts) {
                    if (value.messageTypes() | typeFlags) {
                        getAccountSpecificMessagesL(value, iOrdering, privateFilter);
                    }
                }
            } else { // Excludes
                foreach (QMessageAccount value, iOwner.iAccounts) {
                    if (!(value.messageTypes() & typeFlags)) {
                        getAccountSpecificMessagesL(value, iOrdering, privateFilter);
                    }
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::StandardFolder:
        {
        iNumberOfHandledFilters++;
        QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
        QMessage::StandardFolder standardFolder = static_cast<QMessage::StandardFolder>(pf->_value.toInt());
        TMsvId stdFolderId = iOwner.standardFolderId(standardFolder);
        if (cmp == QMessageDataComparator::Equal) {
            CMsvEntry* pStandardFolderContext = iOwner.retrieveCMsvEntryAndPushToCleanupStack(stdFolderId);
            if (pStandardFolderContext) {
                pStandardFolderContext->SetSortTypeL(iOrdering);
                ipEntrySelection = pStandardFolderContext->ChildrenL();
                iOwner.releaseCMsvEntryAndPopFromCleanupStack(pStandardFolderContext);
            }
            iResultCorrectlyOrdered = true;
        } else { // NotEqual
            ipEntrySelection = new(ELeave)CMsvEntrySelection;
            QMessage::StandardFolder i = QMessage::InboxFolder;
            while (i <= QMessage::TrashFolder) {
                if (i != standardFolder) {
                    CMsvEntry* pStandardFolderContext = iOwner.retrieveCMsvEntryAndPushToCleanupStack(iOwner.standardFolderId(i));
                    if (pStandardFolderContext) {
                        pStandardFolderContext->SetSortTypeL(iOrdering);
                        CMsvEntrySelection* pEntries = pStandardFolderContext->ChildrenL();
                        CleanupStack::PushL(pEntries);
                        for (int i = 0; i < pEntries->Count(); i++) {
                            ipEntrySelection->AppendL(pEntries->At(i));
                        }
                        CleanupStack::PopAndDestroy(pEntries);
                        iOwner.releaseCMsvEntryAndPopFromCleanupStack(pStandardFolderContext);
                    }
                }
                i = static_cast<QMessage::StandardFolder>(static_cast<int>(i) + 1);
            }
        }
        break;
        }
    case QMessageFilterPrivate::ParentAccountIdFilter:
    case QMessageFilterPrivate::ParentFolderIdFilter:
    case QMessageFilterPrivate::TimeStamp:
    case QMessageFilterPrivate::ReceptionTimeStamp:
    case QMessageFilterPrivate::Sender:
    case QMessageFilterPrivate::Recipients:
    case QMessageFilterPrivate::Subject:
    case QMessageFilterPrivate::Status:
    case QMessageFilterPrivate::Priority:
    case QMessageFilterPrivate::Size:
    case QMessageFilterPrivate::None:
        break;
    }
    
    TMsvPartList partlist = 0;
    TPtrC16 value(KNullDesC);
    if (body.isEmpty()) {
        if (iNumberOfHandledFilters < filters.count()) {    
            pf = QMessageFilterPrivate::implementation(filters[iNumberOfHandledFilters]);
            
            if (pf->_matchFlags & QMessageDataComparator::MatchCaseSensitive) {
                partlist |= KMsvFindCaseSensitive;
            }
            if (pf->_matchFlags & QMessageDataComparator::MatchFullWord) {
                partlist |= KMsvFindWholeWord;
            }
    
            switch (pf->_field) {
            case QMessageFilterPrivate::Sender:
                {
                if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                    QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Equal) {
                        if (pf->_value.toString().length() > 0) {
                            value.Set(reinterpret_cast<const TUint16*>(pf->_value.toString().utf16()));
                            partlist |= KMsvMessagePartOriginator;
                            iNumberOfHandledFilters++;
                        }
                    } else { // NotEqual
                        // TODO:
                    }
                } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                }
                break;
                }
            case QMessageFilterPrivate::Recipients: // to, cc & bcc fields
                {
                if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Includes) {
                        if (pf->_value.toString().length() > 0) {
                            value.Set(reinterpret_cast<const TUint16*>(pf->_value.toString().utf16()));
                            partlist |= KMsvMessagePartRecipient;
                            iNumberOfHandledFilters++;
                        }
                    } else { // Excludes
                        //TODO:
                    }
                }
                break;
                }
            case QMessageFilterPrivate::Subject:
                {
                if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                    QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Equal) {
                        if (pf->_value.toString().length() > 0) {
                            value.Set(reinterpret_cast<const TUint16*>(pf->_value.toString().utf16()));
                            partlist |= KMsvMessagePartDescription;
                            iNumberOfHandledFilters++;
                        }
                    } else { // NotEqual
                        // TODO:
                    }
                } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                }
                break;
                }
            case QMessageFilterPrivate::TimeStamp:
                {
                if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                    QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Equal) {
                        // TODO:
                    } else { // NotEqual
                        // TODO:
                    }
                } else if (pf->_comparatorType == QMessageFilterPrivate::Relation) {
                    QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::LessThan) {
                        // TODO:
                    } else if (cmp == QMessageDataComparator::LessThanEqual) {
                        // TODO:
                    } else if (cmp == QMessageDataComparator::GreaterThan) {
                        // TODO:
                    } else { // GreaterThanEqual
                        // TODO:
                    }
                }
                break;
                }
            case QMessageFilterPrivate::ReceptionTimeStamp:
                {
                if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                    QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Equal) {
                        value.Set(reinterpret_cast<const TUint16*>(pf->_value.toString().utf16()));
                        partlist |= KMsvMessagePartDate;
                        iNumberOfHandledFilters++;
                    } else { // NotEqual
                        // TODO:
                    }
                } else if (pf->_comparatorType == QMessageFilterPrivate::Relation) {
                    QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::LessThan) {
                        // TODO:
                    } else if (cmp == QMessageDataComparator::LessThanEqual) {
                        // TODO:
                    } else if (cmp == QMessageDataComparator::GreaterThan) {
                        // TODO:
                    } else { // GreaterThanEqual
                        // TODO:
                    }
                }
                break;
                }
            case QMessageFilterPrivate::Status:
                {
                if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                    QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Equal) {
                        // TODO:
                    } else { // NotEqual
                        // TODO:
                    }
                } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                }
                break;
                }
            case QMessageFilterPrivate::Priority:
                {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    // TODO:
                } else { // NotEqual
                    // TODO:
                }
                break;
                }
            case QMessageFilterPrivate::Size:
                {
                if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                    QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Equal) {
                        // TODO:
                    } else { // NotEqual
                        // TODO:
                    }
                } else if (pf->_comparatorType == QMessageFilterPrivate::Relation) {
                    QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::LessThan) {
                        // TODO:
                    } else if (cmp == QMessageDataComparator::LessThanEqual) {
                        // TODO:
                    } else if (cmp == QMessageDataComparator::GreaterThan) {
                        // TODO:
                    } else { // GreaterThanEqual
                        // TODO:
                    }
                }
                break;
                }
            case QMessageFilterPrivate::ParentAccountIdFilter:
            case QMessageFilterPrivate::ParentFolderIdFilter:
            case QMessageFilterPrivate::Id:
            case QMessageFilterPrivate::ParentFolderId:
            case QMessageFilterPrivate::AncestorFolderIds:
            case QMessageFilterPrivate::ParentAccountId:
            case QMessageFilterPrivate::Type:
            case QMessageFilterPrivate::StandardFolder:
            case QMessageFilterPrivate::None:
                break;
            }
        }
    } else {
        // Body 
        if (matchFlags & QMessageDataComparator::MatchCaseSensitive) {
            partlist |= KMsvFindCaseSensitive;
        }
        if (matchFlags & QMessageDataComparator::MatchFullWord) {
            partlist |= KMsvFindWholeWord;
        }
        
        value.Set(reinterpret_cast<const TUint16*>(body.utf16()));
        partlist |= KMsvMessagePartBody;
    }
    
    if (iResultCorrectlyOrdered && iOrdering.Sorting() == EMsvSortByNone && !iOrdering.GroupingOn()) {
        iResultCorrectlyOrdered = false;
    }

    if (!ipEntrySelection) {
        ipEntrySelection = new(ELeave)CMsvEntrySelection;
        getAllMessagesL(iOrdering);
    }

    if (partlist != 0 && ipEntrySelection->Count() > 0) {
        ipMsvFindOperation = CMsvFindOperation::FindInSelectionL(*ipMsvSession, value, *ipEntrySelection, partlist, iStatus);
        if (!IsActive()) {
            SetActive();
        }
    } else {
        iIdList = QMessageIdList();
        for (int i=0; i < ipEntrySelection->Count(); i++) {
            iIdList.append(QMessageId(SymbianHelpers::addIdPrefix(QString::number((*ipEntrySelection)[i]),SymbianHelpers::EngineTypeMTM)));
        }
        iTimer.After(iStatus, 100);
        if (!IsActive()) {
            SetActive();
        }
    }
}

void CMessagesFindOperation::RunL()
{
    if (iStatus.Int() != KErrNone) {
        iOwner.filterAndOrderMessagesReady(false, iOperationId, QMessageIdList(), 0, false);
    } else {
        if (ipMsvFindOperation) {
            const CMsvFindResultSelection& findResultSelection = ipMsvFindOperation->GetFindResult();
            QMessageIdList msgIds;
            for (int i=0; i < findResultSelection.Count(); i++) {
                msgIds.append(QMessageId(SymbianHelpers::addIdPrefix(QString::number(findResultSelection[i].iId),SymbianHelpers::EngineTypeMTM)));
            }
            iOwner.filterAndOrderMessagesReady(true, iOperationId, msgIds, iNumberOfHandledFilters, iResultCorrectlyOrdered);
        } else {
            iOwner.filterAndOrderMessagesReady(true, iOperationId, iIdList, iNumberOfHandledFilters, iResultCorrectlyOrdered);
        }
    }
    
}

void CMessagesFindOperation::getAllMessagesL(const TMsvSelectionOrdering sortOrder)
{
    // Get all messages from every known account
    foreach (QMessageAccount value, iOwner.iAccounts) {
        getAccountSpecificMessagesL(value, sortOrder);
    }
}

void CMessagesFindOperation::getAccountSpecificMessagesL(QMessageAccount& messageAccount, const TMsvSelectionOrdering sortOrder, QMessageFilterPrivate* privateFolderFilter)
{
    CMsvEntry* pService = iOwner.retrieveCMsvEntryAndPushToCleanupStack(messageAccount.d_ptr->_service1EntryId);
    if (pService) {
        TUid mtmUid = pService->Entry().iMtm;
        iOwner.releaseCMsvEntryAndPopFromCleanupStack(pService);
        getServiceSpecificMessagesL(messageAccount.d_ptr->_service1EntryId, sortOrder, privateFolderFilter);
    }

    TMsvId serviceId = messageAccount.d_ptr->_service2EntryId;
    if (serviceId != 0) {
        CMsvEntry* pService = iOwner.retrieveCMsvEntryAndPushToCleanupStack(serviceId);
        if (pService) {
            TUid mtmUid = pService->Entry().iMtm;
            iOwner.releaseCMsvEntryAndPopFromCleanupStack(pService);
            int count = ipEntrySelection->Count();
            getServiceSpecificMessagesL(messageAccount.d_ptr->_service2EntryId, sortOrder, privateFolderFilter);
            if (ipEntrySelection->Count() > count) {
                iResultCorrectlyOrdered = false;
            }
        }
    }
}

void CMessagesFindOperation::getServiceSpecificMessagesL(TMsvId serviceId, const TMsvSelectionOrdering sortOrder, QMessageFilterPrivate* privateFolderFilter)
{
    if (privateFolderFilter) {
        QMessageDataComparator::EqualityComparator cmp2(static_cast<QMessageDataComparator::EqualityComparator>(privateFolderFilter->_comparatorValue));
        QMessage::StandardFolder standardFolder = static_cast<QMessage::StandardFolder>(privateFolderFilter->_value.toInt()); 
        if (cmp2 == QMessageDataComparator::Equal) {
            iResultCorrectlyOrdered = true;
            getServiceSpecificMessagesFromFolderL(serviceId, sortOrder, iOwner.standardFolderId(standardFolder));
        } else { // NotEqual
            // Loop through all standard folders
            QMessage::StandardFolder i = QMessage::InboxFolder;
            while (i <= QMessage::TrashFolder) {
                if (i != standardFolder) {
                    getServiceSpecificMessagesFromFolderL(serviceId, sortOrder, iOwner.standardFolderId(i));
                }
                i = static_cast<QMessage::StandardFolder>(static_cast<int>(i) + 1);
            }
        }
    }
    else {
        // List all service specific messages from Standard Folders
        QMessage::StandardFolder i = QMessage::InboxFolder;
        while (i <= QMessage::TrashFolder) {
            getServiceSpecificMessagesFromFolderL(serviceId, sortOrder, iOwner.standardFolderId(i));
            i = static_cast<QMessage::StandardFolder>(static_cast<int>(i) + 1);
        }
        
        // List all service specific messages from user created folders
        CMsvEntry* pEntry = iOwner.retrieveCMsvEntryAndPushToCleanupStack(serviceId);
        if (pEntry) {
            TUid mtmUid = pEntry->Entry().iMtm;
            if (mtmUid == KUidMsgTypeSMS || mtmUid == KUidMsgTypeMultimedia || mtmUid == KUidMsgTypeSMTP) {
                pEntry->SetEntryL(KDocumentsEntryIdValue);
                CMsvEntrySelection* pSelection = pEntry->ChildrenWithTypeL(KUidMsvFolderEntry);
                CleanupStack::PushL(pSelection);
                for(TInt i = 0; i < pSelection->Count(); i++) {
                    CMsvEntryFilter* pFilter = CMsvEntryFilter::NewLC();
                    if (mtmUid == KUidMsgTypeSMS || mtmUid == KUidMsgTypeMultimedia) {
                        pFilter->SetMtm(mtmUid);
                    } else {
                        pFilter->SetService(serviceId);
                    }
                    pFilter->SetOrder(sortOrder);
                    pFilter->SetType(KUidMsvMessageEntry);
                    CMsvEntrySelection* pEntries = new(ELeave) CMsvEntrySelection;;
                    CleanupStack::PushL(pEntries);
                    ipMsvSession->GetChildIdsL(pSelection->At(i), *pFilter, *pEntries);
                    for(TInt j = 0; j < pEntries->Count(); j++) {
                        ipEntrySelection->AppendL(pEntries->At(j));
                    }
                    CleanupStack::PopAndDestroy(pEntries);
                    CleanupStack::PopAndDestroy(pFilter);
                }
                CleanupStack::PopAndDestroy(pSelection);
            }
            iOwner.releaseCMsvEntryAndPopFromCleanupStack(pEntry);
        }
    }
}

void CMessagesFindOperation::getServiceSpecificMessagesFromFolderL(TMsvId serviceId, const TMsvSelectionOrdering sortOrder, TMsvId standardFolderId)
{
    CMsvEntry* pEntry = iOwner.retrieveCMsvEntryAndPushToCleanupStack(serviceId);
    if (pEntry) {
        pEntry->SetSortTypeL(sortOrder);
        TUid mtmUid = pEntry->Entry().iMtm;
        if (mtmUid == KUidMsgTypePOP3) {
            if (standardFolderId == KMsvGlobalInBoxIndexEntryIdValue) {
                // POP3 service has (Inbox) messages in service root
                CMsvEntrySelection* pMessageEntries = pEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
                CleanupStack::PushL(pMessageEntries);
                for (int i = 0; i < pMessageEntries->Count(); i++) {
                    ipEntrySelection->AppendL(pMessageEntries->At(i));
                }
                CleanupStack::PopAndDestroy(pMessageEntries);
            }
        } else if (mtmUid == KUidMsgTypeIMAP4) {
            if (standardFolderId == KMsvGlobalInBoxIndexEntryIdValue) {
                // All IMAP4 folders are treated as Inbox folders
                // IMAP4 service has folders in service root
                CMsvEntrySelection* pFolderEntries = pEntry->ChildrenWithTypeL(KUidMsvFolderEntry);
                CleanupStack::PushL(pFolderEntries);
                for (int i = 0; i < pFolderEntries->Count(); i++) {
                    pEntry->SetEntryL(pFolderEntries->At(i));
                    CMsvEntrySelection* pMessageEntries = pEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
                    CleanupStack::PushL(pMessageEntries);
                    for (int j = 0; j < pMessageEntries->Count(); j++) {
                        ipEntrySelection->AppendL(pMessageEntries->At(j));
                    }
                    CleanupStack::PopAndDestroy(pMessageEntries);
                }
                CleanupStack::PopAndDestroy(pFolderEntries);
            }
        } else {
            // Handle SMS, MMS & SMTP Standard Folders
            CMsvEntryFilter* pFilter = CMsvEntryFilter::NewLC();
            if (mtmUid == KUidMsgTypeSMTP) {
                // There maybe multiple SMTP Services
                // => Messages must be queried using ServiceId
                pFilter->SetService(serviceId);
            } else {
                // There is only one service per SMS and per MMS
                // => Messages can be queried using MTM Uid
                pFilter->SetMtm(mtmUid);
            }
            pFilter->SetOrder(sortOrder);
            pFilter->SetType(KUidMsvMessageEntry);
            CMsvEntrySelection* pEntries = new(ELeave) CMsvEntrySelection;;
            CleanupStack::PushL(pEntries);
            ipMsvSession->GetChildIdsL(standardFolderId, *pFilter, *pEntries);
            for (int i = 0; i < pEntries->Count(); i++) {
                ipEntrySelection->AppendL(pEntries->At(i));
            }
            CleanupStack::PopAndDestroy(pEntries);
            CleanupStack::PopAndDestroy(pFilter);
        }
        iOwner.releaseCMsvEntryAndPopFromCleanupStack(pEntry);
    }
}


QTM_END_NAMESPACE
