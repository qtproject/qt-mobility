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
#include "qmessagestore_symbian_p.h"
#include "qmtmengine_symbian_p.h"
#include "qmessage_symbian_p.h"
#include "qmessageordering_p.h"
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageaccountordering_p.h"
#include "qmessagefolderordering_p.h"
#include "qmessageordering_p.h"
#include "qmessagefolder.h"
#include "qmessagefolder_p.h"
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"
#include "qmessageserviceaction_symbian_p.h"

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
#include <MMsvAttachmentManager.h>
#include <CMsvMimeHeaders.h> // Attachemt mimeheader
#include <eikenv.h>
#include <smut.h>
#include <smuthdr.h>
#include <mtuireg.h> // CMtmUiRegistry
#include <mtmuibas.h> // CBaseMtmUi
#include <senduiconsts.h>
#include <SendUi.h>	// SendUi API
#include <cmessagedata.h> //CMessageData
#include <APGCLI.H>
#include <rsendas.h>
#include <rsendasmessage.h>
#include <cmsvrecipientlist.h>

#include <QMessageBox.h>


QTM_BEGIN_NAMESPACE

const TInt KWaitAfterReceivedMessage = 100000; // = 0.1 seconds

Q_GLOBAL_STATIC(CMTMEngine,mtmEngine);

CMTMEngine::CMTMEngine()
 : CActive(EPriorityStandard)
{
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

    // Create & Add SMS Account
    TRAPD(accountError,
    	QMessageAccount smsAcc = QMessageAccountPrivate::from(QMessageAccountId(QString::number(mtmServiceEntryIdL(CMTMEngine::MTMTypeSMS))),
                                                          QString("SMS"),
                                                          mtmServiceEntryIdL(CMTMEngine::MTMTypeSMS),
                                                          0,
                                                          QMessage::Sms);
		iAccounts.append(smsAcc);


		// Create & Add MMS Account
		QMessageAccount mmsAcc = QMessageAccountPrivate::from(QMessageAccountId(QString::number(mtmServiceEntryIdL(CMTMEngine::MTMTypeMMS))),
                                                          QString("MMS"),
                                                          mtmServiceEntryIdL(CMTMEngine::MTMTypeMMS),
                                                          0,
                                                          QMessage::Mms);
		iAccounts.append(mmsAcc);
		updateEmailAccountsL();
		);
    
    TRAPD(err2,
		RFs fileServer;
		User::LeaveIfError(fileServer.Connect());
		CleanupClosePushL(fileServer);
		TBuf<KMaxPath> privatePath;
		fileServer.CreatePrivatePath(EDriveC);
		fileServer.PrivatePath(privatePath);
		iPath.Append(_L("c:"));
		iPath.Append(privatePath);
		iPath.Append(_L("tempattachments\\"));                         
		CFileMan* pFileMan = CFileMan::NewL(fileServer);
		CleanupStack::PushL(pFileMan);
		pFileMan->RmDir(iPath);
		fileServer.MkDirAll(iPath);
		CleanupStack::PopAndDestroy(pFileMan);
		CleanupStack::PopAndDestroy(&fileServer);
	);
}

CMTMEngine::~CMTMEngine()
{
    delete ipPop3Mtm;
    delete ipImap4Mtm;
    delete ipSmtpMtm;
    delete ipMmsMtm;
    delete ipSmsMtm;

    delete ipClientMtmReg;
    delete ipMsvSession;
    
    TRAPD(error,
    	RFs fileServer;
        if (fileServer.Connect() == KErrNone){
			CleanupClosePushL(fileServer);
			TBuf<KMaxPath> privatePath;
			fileServer.CreatePrivatePath(EDriveC);
			fileServer.PrivatePath(privatePath);
			TBuf<KMaxPath> path;
			path.Append(_L("c:"));
			path.Append(privatePath);
			path.Append(_L("tempattachments\\"));                         
			CFileMan* pFileMan=CFileMan::NewL(fileServer);
			CleanupStack::PushL(pFileMan);
			pFileMan->RmDir(path);
			CleanupStack::PopAndDestroy(pFileMan);
			CleanupStack::PopAndDestroy(&fileServer);
            }
        );
}

CMTMEngine* CMTMEngine::instance()
{
    return mtmEngine();
}

bool CMTMEngine::accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2)
{
    CMTMEngine* pMTMEngine = mtmEngine();
    return QMessageAccountOrderingPrivate::lessThan(pMTMEngine->iCurrentAccountOrdering,
                                                    pMTMEngine->account(accountId1),
                                                    pMTMEngine->account(accountId2));
}

void CMTMEngine::orderAccounts(QMessageAccountIdList& accountIds, const QMessageAccountOrdering &ordering) const
{
    iCurrentAccountOrdering = ordering;
    qSort(accountIds.begin(), accountIds.end(), CMTMEngine::accountLessThan);
}

bool CMTMEngine::folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2)
{
    CMTMEngine* pMTMEngine = mtmEngine();
    return QMessageFolderOrderingPrivate::lessThan(pMTMEngine->iCurrentFolderOrdering,
                                                   pMTMEngine->folder(folderId1),
                                                   pMTMEngine->folder(folderId2));
}

void CMTMEngine::orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderOrdering &ordering) const
{
    iCurrentFolderOrdering = ordering;
    qSort(folderIds.begin(), folderIds.end(), CMTMEngine::folderLessThan);
}

bool CMTMEngine::messageLessThan(const QMessageId messageId1, const QMessageId messageId2)
{
    CMTMEngine* pMTMEngine = mtmEngine();
    return QMessageOrderingPrivate::lessThan(pMTMEngine->iCurrentMessageOrdering,
                                             pMTMEngine->message(messageId1),
                                             pMTMEngine->message(messageId2));
}

void CMTMEngine::orderMessages(QMessageIdList& messageIds,  const QMessageOrdering &ordering) const
{
    iCurrentMessageOrdering = ordering;
    qSort(messageIds.begin(), messageIds.end(), CMTMEngine::messageLessThan);
}

QMessageAccountIdList CMTMEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountOrdering &ordering, uint limit, uint offset) const
{
    QMessageAccountIdList accountIds;

    TRAPD(err, updateEmailAccountsL());
    
    int count = iAccounts.count();
    if (limit > 0 && (offset+limit < count)) {
        count = offset+limit;
    }
    
    QMessageAccountFilterPrivate* privateMessageAccountFilter = QMessageAccountFilterPrivate::implementation(filter);
    if (privateMessageAccountFilter->_valid) {
        for (int i=offset; i < count; i++) {
            if (privateMessageAccountFilter->filter(iAccounts[i])) {
                accountIds.append(iAccounts[i].id());
            }
        }
    } else {
        for (int i=offset; i < count; i++) {
            accountIds.append(iAccounts[i].id());
        }
    }
    
    if (!ordering.isEmpty()) {
        orderAccounts(accountIds, ordering);
    }
        
    return accountIds;
}

int CMTMEngine::countAccounts(const QMessageAccountFilter &filter) const
{
    TRAPD(err, updateEmailAccountsL());

    int count = 0;
    QMessageAccountFilterPrivate* privateMessageAccountFilter = QMessageAccountFilterPrivate::implementation(filter);
    if (privateMessageAccountFilter->_valid) {
        for (int i=0; i < iAccounts.count(); i++) {
            if (privateMessageAccountFilter->filter(iAccounts[i])) {
                count++;
            }
        }
    } else {
        count = iAccounts.count();
    }
    
    return count;
}

QMessageAccount CMTMEngine::account(const QMessageAccountId &id) const
{
    for (int i=0; i < iAccounts.count(); i++) {
        if (iAccounts[i].id() == id) {
            return iAccounts[i];
        }
    }
    return QMessageAccount();
}

QMessageAccountId CMTMEngine::defaultAccount(QMessage::Type type) const
{
    TRAPD(err, updateEmailAccountsL());

    for (int i=0; i < iAccounts.count(); i++) {
        if ((iAccounts[i].messageTypes() & type) == type) {
            return iAccounts[i].id();
        }
    }
    return QMessageAccountId();
}

QMessageAccountIdList CMTMEngine::accountsByType(QMessage::Type type) const
{
    QMessageAccountIdList accountIds;
    
    for (int i=0; i < iAccounts.count(); i++) {
        if ((iAccounts[i].messageTypes() & type) == type) {
            accountIds.append(iAccounts[i].id());
        }
    }
    
    return accountIds;
}

void CMTMEngine::updateEmailAccountsL() const
{
    for (int i=2; i < iAccounts.count();) {
        iAccounts.removeAt(i);
    }
    
    CEmailAccounts* pEmailAccounts = CEmailAccounts::NewLC();

    // Default Email account will be the first Email account in the iAccounts list
    TSmtpAccount defaultAccount;
    TInt err = pEmailAccounts->DefaultSmtpAccountL(defaultAccount);    
    if (err == KErrNone) {
        QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(QString::number(defaultAccount.iSmtpService)),
                                                               QString::fromUtf16(defaultAccount.iSmtpAccountName.Ptr(), defaultAccount.iSmtpAccountName.Length()),
                                                               defaultAccount.iRelatedService,
                                                               defaultAccount.iSmtpService,
                                                               QMessage::Email);
        iAccounts.append(account);
    }

    bool found = false;
    RArray<TImapAccount> imapAccounts(10);
    pEmailAccounts->GetImapAccountsL(imapAccounts);
    for (int i=0; i < imapAccounts.Count(); i++) {
        for (int j=0; j < iAccounts.count(); j++) {
            if (imapAccounts[i].iImapService == iAccounts[j].d_ptr->_service1EntryId) {
                found = true;
                break;
            }
        }
        if (!found) {
            QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(QString::number(imapAccounts[i].iImapService)),
                                                                   QString::fromUtf16(imapAccounts[i].iImapAccountName.Ptr(), imapAccounts[i].iImapAccountName.Length()),
                                                                   imapAccounts[i].iImapService,
                                                                   imapAccounts[i].iSmtpService,
                                                                   QMessage::Email);
            iAccounts.append(account);
        } else {
            found = false;
        }
    }
    
    RArray<TPopAccount> popAccounts(10);
    pEmailAccounts->GetPopAccountsL(popAccounts);
    for (int i=0; i < popAccounts.Count(); i++) {
        for (int j=0; j < iAccounts.count(); j++) {
            if (popAccounts[i].iPopService == iAccounts[j].d_ptr->_service1EntryId) {
                found = true;
                break;
            }
        }
        if (!found) {
            QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(QString::number(popAccounts[i].iPopService)),
                                                                   QString::fromUtf16(popAccounts[i].iPopAccountName.Ptr(), popAccounts[i].iPopAccountName.Length()),
                                                                   popAccounts[i].iPopService,
                                                                   popAccounts[i].iSmtpService,
                                                                   QMessage::Email);
            iAccounts.append(account);
        } else {
            found = false;
        }
    }

    RArray<TSmtpAccount> smtpAccounts(10);
    pEmailAccounts->GetSmtpAccountsL(smtpAccounts);
    for (int i=0; i < smtpAccounts.Count(); i++) {
        for (int j=0; j < iAccounts.count(); j++) {
            if (smtpAccounts[i].iSmtpService == iAccounts[j].d_ptr->_service2EntryId) {
                found = true;
                break;
            }
        }
        if (!found) {
            QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(QString::number(smtpAccounts[i].iSmtpService)),
                                                                   QString::fromUtf16(smtpAccounts[i].iSmtpAccountName.Ptr(), smtpAccounts[i].iSmtpAccountName.Length()),
                                                                   smtpAccounts[i].iRelatedService,
                                                                   smtpAccounts[i].iSmtpService,
                                                                   QMessage::Email);
            iAccounts.append(account);
        } else {
            found = false;
        }
    }
    
    CleanupStack::PopAndDestroy(pEmailAccounts);
}

TUid CMTMEngine::mtmUidByType(MTMType aMTMType)
{
    switch(aMTMType)
    {
    case CMTMEngine::MTMTypeSMS:  return KUidMsgTypeSMS;
    case CMTMEngine::MTMTypeMMS:  return KUidMsgTypeMultimedia;
    case CMTMEngine::MTMTypeSMTP: return KUidMsgTypeSMTP;
    case CMTMEngine::MTMTypeIMAP: return KUidMsgTypeIMAP4;
    case CMTMEngine::MTMTypePOP3: return KUidMsgTypePOP3;
    }
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
}

TMsvId CMTMEngine::mtmServiceEntryIdL(MTMType aMTMType)
{
    CBaseMtm* pMTM;
    TUid mtmUID;
    switch(aMTMType)
    {
    case CMTMEngine::MTMTypeSMS:
        mtmUID = KUidMsgTypeSMS;
        pMTM = ipSmsMtm;
        break;
    case CMTMEngine::MTMTypeMMS:
        mtmUID = KUidMsgTypeMultimedia;
        pMTM = ipMmsMtm;
        break;
    case CMTMEngine::MTMTypeSMTP:
        mtmUID = KUidMsgTypeSMTP;
        pMTM = ipSmtpMtm;
        break;
    case CMTMEngine::MTMTypeIMAP:
        mtmUID = KUidMsgTypeIMAP4;
        pMTM = ipImap4Mtm;
        break;
    case CMTMEngine::MTMTypePOP3:
        mtmUID = KUidMsgTypePOP3;
        pMTM = ipPop3Mtm;
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
    switch(aMTMType)
    {
    case CMTMEngine::MTMTypeSMS: return ipSmsMtm;
    case CMTMEngine::MTMTypeMMS: return ipMmsMtm;
    case CMTMEngine::MTMTypeSMTP: return ipSmtpMtm;
    case CMTMEngine::MTMTypeIMAP: return ipImap4Mtm;
    case CMTMEngine::MTMTypePOP3: return ipPop3Mtm;
    }
}

TMsvId CMTMEngine::standardFolderId(QMessage::StandardFolder standardFolder)
{
    switch(standardFolder)
    {
    case QMessage::InboxFolder:  return KMsvGlobalInBoxIndexEntryIdValue;
    case QMessage::OutboxFolder: return KMsvGlobalOutBoxIndexEntryIdValue;
    case QMessage::DraftsFolder: return KMsvDraftEntryIdValue;
    case QMessage::SentFolder:   return KMsvSentEntryIdValue;
    case QMessage::TrashFolder:  return KMsvDeletedEntryFolderEntryIdValue;
    }
}

bool CMTMEngine::addMessage(QMessage* m)
{
	bool retVal = true;
	if (m->type() == QMessage::NoType){
		QMessageAccount account = QMessageAccount(m->parentAccountId());
		QMessage::TypeFlags types = account.messageTypes();
		if (types == QMessage::Email){
			m->setType(QMessage::Email);
		}
		else if (types == QMessage::Mms){
			m->setType(QMessage::Mms);
		}
		if (types == QMessage::Sms){
			m->setType(QMessage::Sms);
		}
	} 
	
	if (m->type() == QMessage::Sms){
		TRAPD(err, storeSMSL(*m, KMsvDraftEntryId));
		if (err != KErrNone)
			retVal = false;
	}
	else if (m->type() == QMessage::Mms){
		TRAPD(err, storeMMSL(*m, KMsvDraftEntryId));
		if (err != KErrNone)
			retVal = false;
	}
	else if (m->type() == QMessage::Email){
		TRAPD(err, storeEmailL(*m, KMsvDraftEntryId));
		if (err != KErrNone)
			retVal = false;
	}
	
	return retVal;
}

bool CMTMEngine::updateMessage(QMessage* m)
{
	bool retVal = true;
	
	if (m->type() == QMessage::NoType){
		QMessageAccount account = QMessageAccount(m->parentAccountId());
		QMessage::TypeFlags types = account.messageTypes();
		if (types == QMessage::Email){
			m->setType(QMessage::Email);
		}
		else if (types == QMessage::Mms){
			m->setType(QMessage::Mms);
		}
		if (types == QMessage::Sms){
			m->setType(QMessage::Sms);
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

bool CMTMEngine::removeMessage(const QMessageId &id, QMessageStore::RemovalOption option)
{
    if (!iSessionReady)
        return false;
    
    bool retVal;
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
    long int messageId = id.toString().toLong();
    
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

    return retVal;
}

bool CMTMEngine::composeSMSL(const QMessage &message)
{
	Q_UNUSED(message); // TODO: fill the messageData
	CSendUi *sendUi = CSendUi::NewL();
	CleanupStack::PushL(sendUi);
	CMessageData* messageData = CMessageData::NewLC();	
	sendUi->CreateAndSendMessageL(KSenduiMtmSmsUid, messageData, KNullUid, ETrue);
	CleanupStack::PopAndDestroy(2); //messageData and sendUi
	return true;
}

bool CMTMEngine::composeMMSL(const QMessage &message)
{
	Q_UNUSED(message); // TODO: fill the messageData
	CSendUi *sendUi = CSendUi::NewL();
	CleanupStack::PushL(sendUi);
	CMessageData* messageData = CMessageData::NewLC();
	sendUi->CreateAndSendMessageL(KSenduiMtmMmsUid, messageData, KNullUid, ETrue);
	CleanupStack::PopAndDestroy(2); //messageData and sendUi
	return true;
}

bool CMTMEngine::composeEmailL(const QMessage &message)
{
	Q_UNUSED(message); // TODO: fill the messageData
	CSendUi *sendUi = CSendUi::NewL();
	CleanupStack::PushL(sendUi);
	CMessageData* messageData = CMessageData::NewLC();
	sendUi->CreateAndSendMessageL(KSenduiMtmSmtpUid, messageData, KNullUid, ETrue);
	CleanupStack::PopAndDestroy(2); //messageData and sendUi
	return true;
}

bool CMTMEngine::retrieve(const QMessageContentContainerId& id)
{
	TRAPD(err, retrieveL(id));
		if (err != KErrNone)
			return false;
		else
			return true;
}

void CMTMEngine::retrieveL(const QMessageContentContainerId& id)
{
	Q_UNUSED(id)
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
	long int messageId = id.toString().toLong();
	
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
		wait->Start();
		CActiveScheduler::Start();	
		
		TImImap4GetPartialMailInfo info;
		info.iPartialMailOptions = EBodyTextOnly;
		TPckg<TImImap4GetPartialMailInfo> bodyInfo(info);

		CMsvOperation* opPopulate = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMPopulate,
													*sel, bodyInfo, wait->iStatus);
		wait->Start();
		CActiveScheduler::Start();
		
		CMsvOperation* opDisconnect = ipImap4Mtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,
													*sel, parameter, wait->iStatus);
		
		wait->Start();
		CActiveScheduler::Start();			
	}
	
	if (pEntry->Entry().iMtm == KUidMsgTypeSMTP){	
		// TODO:
	}
	
	if (pEntry->Entry().iMtm == KUidMsgTypePOP3){
		TPckgBuf<TInt> parameter;
		
		ipPop3Mtm->SwitchCurrentEntryL(pEntry->OwningService());	

		sel->AppendL(pEntry->EntryId());
		
		CMsvOperation* opConnect = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMConnect,
													*sel, parameter, wait->iStatus);
		wait->Start();
		CActiveScheduler::Start();	

		CMsvOperation* opPopulate = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMPopulate,
													*sel, parameter, wait->iStatus);
		wait->Start();
		CActiveScheduler::Start();
		
		CMsvOperation* opDisconnect = ipPop3Mtm->InvokeAsyncFunctionL(KPOP3MTMDisconnect,
													*sel, parameter, wait->iStatus);
		
		wait->Start();
		CActiveScheduler::Start();	
		
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

void CMTMEngine::retrieveHeaderL(const QMessageId& id)
{
	long int messageId = id.toString().toLong();
	CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
	CleanupStack::PushL(pEntry);
	
	CleanupStack::PopAndDestroy(pEntry);
}

bool CMTMEngine::removeMessageL(const QMessageId &id, QMessageStore::RemovalOption /*option*/)
{
    long int messageId = id.toString().toLong();
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
        ipImap4Mtm->Entry().DeleteL(messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypePOP3) {
        if (!ipPop3Mtm)
            return false;
        ipPop3Mtm->SwitchCurrentEntryL(messageId);
        TMsvId parent = ipPop3Mtm->Entry().Entry().Parent();
        ipPop3Mtm->SwitchCurrentEntryL(parent);
        ipPop3Mtm->Entry().DeleteL(messageId);
    } 
    
    CleanupStack::PushL(pEntry);

    return true;
}

bool CMTMEngine::removeMessages(const QMessageFilter& /*filter*/, QMessageStore::RemovalOption /*option*/)
{
    return false;
}

bool CMTMEngine::queryMessages(QMessageServiceActionPrivate& privateAction, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
	Q_UNUSED(limit);
	Q_UNUSED(offset);
    MessageQueryInfo queryInfo;
    queryInfo.operationId = ++iOperationIds;
    queryInfo.filter = filter;
    queryInfo.ordering = ordering;
    queryInfo.findOperation = new CMessagesFindOperation((CMTMEngine&)*this, ipMsvSession, queryInfo.operationId);
    queryInfo.privateAction = &privateAction;
    queryInfo.currentFilterListIndex = 0;
    iMessageQueries.append(queryInfo);
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(iMessageQueries[iMessageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(iMessageQueries[iMessageQueries.count()-1].filter,
                                                        iMessageQueries[iMessageQueries.count()-1].ordering);
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0], iMessageQueries[iMessageQueries.count()-1].ordering);
    }
}

bool CMTMEngine::queryMessages(QMessageServiceActionPrivate& privateAction, const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const
{
	Q_UNUSED(offset);
	Q_UNUSED(limit);
    MessageQueryInfo queryInfo;
    queryInfo.operationId = ++iOperationIds;
    queryInfo.filter = filter;
    queryInfo.ordering = ordering;
    queryInfo.findOperation = new CMessagesFindOperation((CMTMEngine&)*this, ipMsvSession, queryInfo.operationId);
    queryInfo.privateAction = &privateAction;
    queryInfo.currentFilterListIndex = 0;
    iMessageQueries.append(queryInfo);
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(iMessageQueries[iMessageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(iMessageQueries[iMessageQueries.count()-1].filter,
                                                        iMessageQueries[iMessageQueries.count()-1].ordering,
                                                        body,
                                                        options);
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0],
                                                        iMessageQueries[iMessageQueries.count()-1].ordering,
                                                        body,
                                                        options);
    }
}

bool CMTMEngine::countMessages(const QMessageFilter &filter, uint limit) const
{
	Q_UNUSED(filter);
	Q_UNUSED(limit);
	return false;
}

bool CMTMEngine::countMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, uint limit) const
{
	Q_UNUSED(filter);
	Q_UNUSED(body);
	Q_UNUSED(options);
	Q_UNUSED(limit);
	return false;
}

void CMTMEngine::filterAndOrderMessagesReady(bool success, int operationId, QMessageIdList ids, int numberOfHandledFilters)
{
    int index=0;
    for (; index < iMessageQueries.count(); index++) {
        if (iMessageQueries[index].operationId == operationId) {
            break;
        }
    }

    if (success) {
        QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(iMessageQueries[index].filter);
        if (pf->_filterList.count() > 0) {
            if (pf->_filterList[0].count() > numberOfHandledFilters) {
                for (int i=0; i < ids.count(); i++) {
                    QMessage msg = message(ids[i]);
                    for (int j=numberOfHandledFilters; j < pf->_filterList[0].count(); j++) {
                        QMessageFilterPrivate* pf2 = QMessageFilterPrivate::implementation(pf->_filterList[0][j]);
                        if (!pf2->filter(msg)) {
                            ids.removeAt(i);
                            i--;
                            break;
                        }
                    }
                }
            }
        }
        
        emit iMessageQueries[index].privateAction->messagesFound(ids);
    }

    delete iMessageQueries[index].findOperation;
    iMessageQueries.removeAt(index);
}

QMessageAccountIdList CMTMEngine::filterAccounts(const QMessageAccountFilter& filter) const
{
	Q_UNUSED(filter);
}

QMessageFolderIdList CMTMEngine::filterMessageFolders(const QMessageFolderFilter& filter) const
{
    QMessageFolderFilterPrivate* pf = QMessageFolderFilterPrivate::implementation(filter);
    if (!pf->_valid) {
        return QMessageFolderIdList();
    }
    
    QMessageFolderIdList ids;
    
    Qt::CaseSensitivity caseSensitivity = (pf->_options & QMessageDataComparator::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive; 
    
    switch (pf->_field) {
    case QMessageFolderFilterPrivate::Id:
        {
        if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                long int folderId = pf->_value.toString().toLong();
                CMsvEntry* pEntry = ipMsvSession->GetEntryL(folderId);
                CleanupStack::PushL(pEntry);
                const TMsvEntry& entry = pEntry->Entry();
                if (entry.iType == KUidMsvFolderEntry) {
                    ids.append(QMessageFolderId(pf->_value.toString()));
                }
                CleanupStack::PopAndDestroy(pEntry);
            } else { // NotEqual
                // TODO:
            }
        } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (pf->_ids.count() > 0) { // QMessageIdList
                if (cmp == QMessageDataComparator::Includes) {
                    for (int i=0; i<pf->_ids.count(); i++) {
                        long int folderId = pf->_ids[i].toString().toLong();
                        CMsvEntry* pEntry = ipMsvSession->GetEntryL(folderId);
                        CleanupStack::PushL(pEntry);
                        const TMsvEntry& entry = pEntry->Entry();
                        if (entry.iType == KUidMsvFolderEntry) {
                            ids.append(QMessageFolderId(pf->_value.toString()));
                        }
                        CleanupStack::PopAndDestroy(pEntry);
                    }
                } else { // Excludes
                    // TODO:
                }
            } else { // QMessageFilter
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::DisplayName:
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
                // TODO:
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
                // TODO:
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::ParentAccountId:
        {
        if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                QMessageAccountId accountId = QMessageAccountId(pf->_value.toString());
                QMessageAccount messageAccount = account(accountId);
                CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageAccount.d_ptr->_service1EntryId);
                CleanupStack::PushL(pEntry);
                CMsvEntrySelection* pSelection = pEntry->ChildrenWithTypeL(KUidMsvFolderEntry);
                CleanupStack::PushL(pSelection);
                if (pSelection->Count() > 0) {
                    for(TInt i = 0; i < pSelection->Count(); i++) {
                        ids.append(QMessageFolderId(QString::number(pSelection->At(i))));
                    }
                }
                CleanupStack::PopAndDestroy(pSelection);
                CleanupStack::PopAndDestroy(pEntry);
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
    case QMessageFolderFilterPrivate::ParentFolderId:
        {
        if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                long int folderId = pf->_value.toString().toLong();
                CMsvEntry* pEntry = ipMsvSession->GetEntryL(folderId);
                CleanupStack::PushL(pEntry);
                const TMsvEntry& entry = pEntry->Entry();
                if (entry.iType == KUidMsvFolderEntry) {
                    CMsvEntrySelection* pSelection = pEntry->ChildrenWithTypeL(KUidMsvFolderEntry);
                    CleanupStack::PushL(pSelection);
                    if (pSelection->Count() > 0) {
                        for(TInt i = 0; i < pSelection->Count(); i++) {
                            ids.append(QMessageFolderId(QString::number(pSelection->At(i))));
                        }
                    }
                    CleanupStack::PopAndDestroy(pSelection);
                }
                CleanupStack::PopAndDestroy(pEntry);
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
    }

    return ids;
}

QMessageFolderIdList CMTMEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderOrdering &ordering, uint limit, uint offset) const
{
	Q_UNUSED(offset);
	Q_UNUSED(limit);
    QMessageFolderIdList ids;
    
    QMessageFolderFilterPrivate* pMFFilter = QMessageFolderFilterPrivate::implementation(filter);

    QList<QMessageFolderIdList> resultSets;
    if (pMFFilter->_filterList.count() > 0) {
        for (int i=0; i < pMFFilter->_filterList.count(); i++) {
            QMessageFolderIdList ids;
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                if (j == 0) {
                    ids = filterMessageFolders(pMFFilter->_filterList[i][j]);
                } else {
                    for (int x=0; x < ids.count(); x++) {
                        QMessageFolder mf = folder(ids[x]);
                        if (!QMessageFolderFilterPrivate::implementation(pMFFilter->_filterList[i][j])->filter(mf)) {
                            ids.removeAt(x);
                            x--;
                        }
                    }
                }
                if (ids.count() == 0) {
                    break;
                }
            }
            if (ids.count() > 0) {
                resultSets.append(ids);
            }
        }
        for (int i=0; i < resultSets.count(); i++) {
            if (i == 0) {
                ids = resultSets[i];
            } else {
                for (int j=0; j<ids.count(); j++) {
                    if (!resultSets[i].contains(ids[j])) {
                        ids.removeAt(j);
                        j--;
                    }
                }
            }
        }
    } else {
        ids = filterMessageFolders(filter);
    }
    
    if (!ordering.isEmpty()) {
        orderFolders(ids, ordering);
    }    
    
    return ids;
}

int CMTMEngine::countFolders(const QMessageFolderFilter &filter) const
{
	Q_UNUSED(filter);
	return 0;
}

QMessageFolder CMTMEngine::folder(const QMessageFolderId &id) const
{
	if (!iSessionReady)
		return QMessageFolder();
	
	QMessageFolder folder;
	TRAPD(err, folder = folderL(id));
	   
    return folder;
}

QMessageFolder CMTMEngine::folderL(const QMessageFolderId &id) const
{
    QMessageFolder folder;

    long int folderId = id.toString().toLong();
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(folderId);
    CleanupStack::PushL(pEntry);
    const TMsvEntry& entry = pEntry->Entry();
    if (entry.iType == KUidMsvFolderEntry) {
        QMessageAccountId accountId(QString::number(entry.iServiceId));
        QMessageFolderId parentId;
        QString name = QString::fromUtf16(entry.iDetails.Ptr(), entry.iDetails.Length());
        QString path;
        folder = QMessageFolderPrivate::from(id, accountId, parentId, name, path);
    }
    CleanupStack::PopAndDestroy(pEntry);
    
    return folder;
}

QMessage CMTMEngine::message(const QMessageId& id) const
{
    if (!iSessionReady)
        return QMessage();
    
    QMessage message;
    TRAPD(err, message = messageL(id));

    return message;
}

QMessage CMTMEngine::messageL(const QMessageId& id) const
{
    QMessage message;

    long int messageId = id.toString().toLong();
    CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(pEntry);
    
    if (pEntry->Entry().iMtm == KUidMsgTypeSMS) {
        if (!ipSmsMtm)
            return message;
        message = smsMessageL(*pEntry, messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypeMultimedia) {
        if (!ipMmsMtm)
            return message;
        message = mmsMessageL(*pEntry, messageId);
    }  else if (pEntry->Entry().iMtm == KUidMsgTypeSMTP) {
        if (!ipSmtpMtm)
            return message;
        message = emailMessageL(*pEntry, messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypeIMAP4) {
        if (!ipImap4Mtm)
            return message;
        message = emailMessageL(*pEntry, messageId);
    } else if (pEntry->Entry().iMtm == KUidMsgTypePOP3) {
        if (!ipPop3Mtm)
            return message;
        message = emailMessageL(*pEntry, messageId);
    }

    CleanupStack::PopAndDestroy(pEntry);
    
    return message;
}

bool CMTMEngine::storeMMS(QMessage &message, TMsvId dest)
{
	if (!iSessionReady)
		return false;
	
	TRAPD(err, storeMMSL(message, dest));
	if (err != KErrNone)
		return false;
	else
		return true;
}

void CMTMEngine::sendMMS()
{
	if (!iSessionReady)
		return;

	TRAPD(err, sendMMSL());
}

bool CMTMEngine::storeEmail(QMessage &message, TMsvId dest)
{
	if (!iSessionReady)
		return false;
	
	TRAPD(err, storeEmailL(message, dest));
	if (err != KErrNone)
		return false;
	else
		return true;
}

void CMTMEngine::sendEmail(QMessage &message)
{
	if (!iSessionReady)
		return;

	TRAPD(err, sendEmailL(message));
}

bool CMTMEngine::storeSMS(QMessage &message, TMsvId dest)
{
	if (!iSessionReady)
		return false;
	
	TRAPD(err, storeSMSL(message, dest));
	if (err != KErrNone)
		return false;
	else
		return true;
}

void CMTMEngine::storeSMSL(QMessage &message, TMsvId dest)
{
	Q_UNUSED(dest);
	RSendAs sendAs;
    TInt err = sendAs.Connect();
    if (err)
		return;
    CleanupClosePushL(sendAs);

    RSendAsMessage sendAsMessage;
    // creates message in the draft folder
    sendAsMessage.CreateL(sendAs, KUidMsgTypeSMS);
    CleanupClosePushL(sendAsMessage);

    // Add receivers
    QList<QMessageAddress> list(message.to());
    TPtrC16 receiver(KNullDesC);
    QString qreceiver;
    for (int i = 0; i < list.size(); ++i) {
		qreceiver = list.at(i).recipient();
		receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
		sendAsMessage.AddRecipientL(receiver, RSendAsMessage::ESendAsRecipientTo);
    }
    
    // Set Body text
    QString body = message.textContent();
    TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
    HBufC* bd = msg.AllocL();
    sendAsMessage.SetBodyTextL(*bd);
    sendAsMessage.SaveMessageAndCloseL();
    
    CleanupStack::Pop(); // sendAsMessage
    CleanupStack::PopAndDestroy(); // sendAs
}

bool CMTMEngine::sendSMS(QMessage &message)
{
	if (!iSessionReady)
		return false;
	
	TRAPD(err, sendSMSL(message));
	if (err != KErrNone)
		return false;
	else
		return true;
}

void CMTMEngine::sendSMSL(QMessage &message)
{
	RSendAs sendAs;
    TInt err = sendAs.Connect();
    if (err) {
		return;
    }
    CleanupClosePushL(sendAs);

    RSendAsMessage sendAsMessage;
    sendAsMessage.CreateL(sendAs, KUidMsgTypeSMS);
    CleanupClosePushL(sendAsMessage);

    // Prepare the message
    
    // Add receivers
    QList<QMessageAddress> list(message.to());
    TPtrC16 receiver(KNullDesC);
    QString qreceiver;
    for (int i = 0; i < list.size(); ++i) {
		qreceiver = list.at(i).recipient();
		receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
		sendAsMessage.AddRecipientL(receiver, RSendAsMessage::ESendAsRecipientTo);
    }
    
    // Set Body text
    QString body = message.textContent();
    TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
    HBufC* bd = msg.AllocL();
    sendAsMessage.SetBodyTextL(*bd);
    
    // Send the message
    sendAsMessage.SendMessageAndCloseL();

    CleanupStack::Pop(); // sendAsMessage (already closed)
    CleanupStack::PopAndDestroy(); // sendAs	
}

void CMTMEngine::storeMMSL(QMessage &message, TMsvId dest)
{
	if (!iSessionReady)
		return;
		
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();
	CMsvOperation* operation = ipMmsMtm->CreateNewEntryL(dest, wait->iStatus);
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
			
	bool retVal = true;
    // Add receivers
    QList<QMessageAddress> list(message.to());
    TPtrC16 receiver(KNullDesC);
    QString qreceiver;
    for (int i = 0; i < list.size(); ++i) {
		qreceiver = list.at(i).recipient();
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
    ipMmsMtm->Entry().ChangeL(ent); 
    // Save the changes    
    ipMmsMtm->SaveMessageL();
    
    CMsvStore* store = ipMmsMtm->Entry().EditStoreL();    
    CleanupStack::PushL(store); 
    
    // attachments.
    QByteArray filePath;
    QMessageContentContainerIdList contentIds = message.attachmentIds();
    int count = contentIds.count();
    foreach (QMessageContentContainerId id, contentIds){
		QMessageContentContainer container = message.find(id);
		filePath = container.suggestedFileName();
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
        User::LeaveIfError(attachment.Open(CEikonEnv::Static()->FsSession(),attachmentFile, EFileShareReadersOnly | EFileRead));    
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
		User::LeaveIfError(file2.Open(CEikonEnv::Static()->FsSession(),tempFileName, EFileShareAny|EFileRead));
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

    // Add body
    QString body = message.textContent();
    if (!body.isEmpty()){
		TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
		TMsvAttachmentId attachmentid = KMsvNullIndexEntryId;
		TFileName bodyFile( _L("body.txt") );
		ipMmsMtm->CreateTextAttachmentL(*store, attachmentid, msg, bodyFile, ETrue); 
		store->CommitL();
    }        
	CleanupStack::PopAndDestroy(); // store
	
	// Save the changes
	ipMmsMtm->SaveMessageL();       
}

void CMTMEngine::updateSMSL(QMessage &message)
{
	if (!iSessionReady)
		return;
	
	QMessageId id = message.id();
    long int messageId = id.toString().toLong();
    if (messageId == 0)
    	return;
    
    CMsvEntry* entry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(entry);
    
	ipSmsMtm->SwitchCurrentEntryL(messageId);
	ipSmsMtm->LoadMessageL();
	
	QList<QMessageAddress> list(message.to());
	if (!list.empty()){
		TPtrC16 receiver(KNullDesC);
		QString qreceiver;
		for (int i = 0; i < list.size(); ++i) {
			qreceiver = list.at(i).recipient();
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
}

void CMTMEngine::updateMMSL(QMessage &message)
{
	if (!iSessionReady)
		return;
	
	QMessageId id = message.id();
    long int messageId = id.toString().toLong();
    if (messageId == 0)
    	return;
    
	ipMmsMtm->SwitchCurrentEntryL(messageId);
	ipMmsMtm->LoadMessageL();
			
	bool retVal = true;
    // Add receivers
    QList<QMessageAddress> list(message.to());
    if (!list.empty()){
		TPtrC16 receiver(KNullDesC);
		QString qreceiver;
		for (int i = 0; i < list.size(); ++i) {
			qreceiver = list.at(i).recipient();
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
    ipMmsMtm->Entry().ChangeL(ent); 
    // Save the changes    
    ipMmsMtm->SaveMessageL();
    
    CMsvStore* store = ipMmsMtm->Entry().EditStoreL();    
    CleanupStack::PushL(store); 
    
    // attachments.
    QByteArray filePath;
    QMessageContentContainerIdList contentIds = message.attachmentIds();
    int count = contentIds.count();
    foreach (QMessageContentContainerId id, contentIds){
		QMessageContentContainer container = message.find(id);
		filePath = container.suggestedFileName();
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
        User::LeaveIfError(attachment.Open(CEikonEnv::Static()->FsSession(),attachmentFile, EFileShareReadersOnly | EFileRead));    
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
		User::LeaveIfError(file2.Open(CEikonEnv::Static()->FsSession(),tempFileName, EFileShareAny|EFileRead));
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
    // Add body
    QString body = message.textContent();
    if (!body.isEmpty()){
		TPtrC16 msg(reinterpret_cast<const TUint16*>(body.utf16()));
		TMsvAttachmentId attachmentid = KMsvNullIndexEntryId;
		TFileName bodyFile( _L("body.txt") );
		ipMmsMtm->CreateTextAttachmentL(*store, attachmentid, msg, bodyFile, ETrue); 
		store->CommitL();
    }        
	CleanupStack::PopAndDestroy(); // store	
	// Save the changes
	ipMmsMtm->SaveMessageL();	       
}

void CMTMEngine::updateEmailL(QMessage &message)
{
	if (!iSessionReady)
    	return;  
	
	QMessageId id = message.id();
    long int messageId = id.toString().toLong();
    if (messageId == 0)
    	return;
    
    ipSmtpMtm->SwitchCurrentEntryL(messageId);
    ipSmtpMtm->LoadMessageL();
    
    CMsvEntry* entry = ipMsvSession->GetEntryL(messageId);
    CleanupStack::PushL(entry);
    
    CImEmailMessage* mailMsg = CImEmailMessage::NewLC(*entry); 
    
    CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
    
    if (!message.textContent().isEmpty()) {
		CParaFormatLayer* paragraphFormatLayer = CParaFormatLayer::NewL();
		CleanupStack::PushL(paragraphFormatLayer);
	
		CCharFormatLayer* characterFormatLayer = CCharFormatLayer::NewL(); 
		CleanupStack::PushL(characterFormatLayer);
	
		CRichText* bodyText = CRichText::NewL(paragraphFormatLayer, characterFormatLayer, CEditableText::EFlatStorage, 256);
		CleanupStack::PushL(bodyText);
	
		// Insert the contents of a buffer into the document at specified position
		bodyText->InsertL(0, TPtrC(reinterpret_cast<const TUint16*>(message.textContent().utf16())));
		mailMsg->StoreBodyTextL(messageId, *bodyText, waiter->iStatus);
	    waiter->Start();
	    CActiveScheduler::Start();
		CleanupStack::PopAndDestroy(3); // bodyText, characterFormatLayer, paragraphFormatLayer
    }
    
    
    QByteArray filePath;
	QMessageContentContainerIdList contentIds = message.attachmentIds();
	int count = contentIds.count();
	foreach (QMessageContentContainerId id, contentIds){
		QMessageContentContainer container = message.find(id);
		filePath = container.suggestedFileName();
		filePath.replace(QByteArray("/"), QByteArray("\\"));
		QString temp_path = QString(filePath);
		TPtrC16 attachmentPath(KNullDesC);
		attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
		TFileName attachmentFile;
		attachmentFile.Append(attachmentPath);    
	
		RFile attachment;    
		User::LeaveIfError(attachment.Open(CEikonEnv::Static()->FsSession(),attachmentFile, EFileShareReadersOnly | EFileRead));    
		CleanupClosePushL(attachment);   
		
        CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
        QByteArray filePath = container.suggestedFileName();
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
	}  

    CMsvStore* store = entry->EditStoreL();
    CleanupStack::PushL(store);
    CImHeader* emailEntry = CImHeader::NewLC();
    emailEntry->RestoreL(*store);
    emailEntry->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    
    QList<QMessageAddress> toList(message.to());
    if (toList.count() > 0){
		TPtrC16 receiver(KNullDesC);
		QString qreceiver;
		for (int i = 0; i < toList.size(); ++i) {
			qreceiver = toList.at(i).recipient();
			receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
			emailEntry->ToRecipients().AppendL(receiver); 
			}
    }
    
	QList<QMessageAddress> ccList(message.cc());
	if (ccList.count() > 0){
		TPtrC16 receiver(KNullDesC);
		QString qreceiver;
		for (int i = 0; i < ccList.size(); ++i) {
			qreceiver = ccList.at(i).recipient();
			receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
			emailEntry->CcRecipients().AppendL(receiver); 
			}
	}
		
	QList<QMessageAddress> bccList(message.bcc());
	if (bccList.count() > 0){
		TPtrC16 receiver(KNullDesC);
		QString qreceiver;
		for (int i = 0; i < bccList.size(); ++i) {
			qreceiver = bccList.at(i).recipient();
			receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
			emailEntry->BccRecipients().AppendL(receiver); 
			}
	}
     
    emailEntry->StoreL(*store);
    // Store the changes permanently
    store->CommitL();

    CleanupStack::PopAndDestroy(5, entry); 
    // mailMsg, emailEntry, store, waiter, entry   
}

void CMTMEngine::sendMMSL()
{
	// Start sending the message via the Server MTM to the MMS server    
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();    
	wait->iStatus = KRequestPending;    
	CMsvOperation* op = NULL;    
	op = ipMmsMtm->SendL(wait->iStatus);    
	wait->Start();    
	CleanupStack::PushL(op);    
	CActiveScheduler::Start();     
   
	while(wait->iStatus == KRequestPending)        
		 CActiveScheduler::Start();     
	CleanupStack::PopAndDestroy(2); // op, wait	
}

void CMTMEngine::storeEmailL(QMessage &message, TMsvId dest)
{
    if (!iSessionReady)
    {
    	return;   
    } 
        
    // Set the context to the folder in which message has to be created
    CMsvEntry* entry = CMsvEntry::NewL(*ipMsvSession, dest, TMsvSelectionOrdering());
    CleanupStack::PushL(entry);
    entry->SetEntryL(dest);
    
    CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
    
    TMsvEmailTypeList msvEmailTypeList = 0;
    TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
        
    CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(waiter->iStatus, *ipMsvSession, dest, 
																		partList, msvEmailTypeList, KUidMsgTypeSMTP);
    CleanupStack::PushL(emailOperation);
    waiter->Start();
    CActiveScheduler::Start();
    
    TMsvId temp;
    TPckgC<TMsvId> paramPack(temp);
    const TDesC8& progBuf = emailOperation->ProgressL();
    paramPack.Set(progBuf);
    TMsvId newMessageId;
    newMessageId = paramPack();

    entry->SetEntryL(newMessageId);

    CParaFormatLayer* paragraphFormatLayer = CParaFormatLayer::NewL();
    CleanupStack::PushL(paragraphFormatLayer);

    CCharFormatLayer* characterFormatLayer = CCharFormatLayer::NewL(); 
    CleanupStack::PushL(characterFormatLayer);

    CRichText* bodyText = CRichText::NewL(paragraphFormatLayer, characterFormatLayer, CEditableText::EFlatStorage, 256);
    CleanupStack::PushL(bodyText);

    // Insert the contents of a buffer into the document at specified position
    bodyText->InsertL(0, TPtrC(reinterpret_cast<const TUint16*>(message.textContent().utf16())));
    
    CImEmailMessage* emailMessage = CImEmailMessage::NewL(*entry);
    CleanupStack::PushL(emailMessage);
    emailMessage->StoreBodyTextL(newMessageId, *bodyText, waiter->iStatus);
    waiter->Start();
    CActiveScheduler::Start();
    
    QByteArray filePath;
	QMessageContentContainerIdList contentIds = message.attachmentIds();
	int count = contentIds.count();
	foreach (QMessageContentContainerId id, contentIds){
		QMessageContentContainer container = message.find(id);
		filePath = container.suggestedFileName();
		filePath.replace(QByteArray("/"), QByteArray("\\"));
		QString temp_path = QString(filePath);
		TPtrC16 attachmentPath(KNullDesC);
		attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
		TFileName attachmentFile;
		attachmentFile.Append(attachmentPath);    
	
		RFile attachment;    
		User::LeaveIfError(attachment.Open(CEikonEnv::Static()->FsSession(),attachmentFile, EFileShareReadersOnly | EFileRead));    
		CleanupClosePushL(attachment);   
		
        CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
        QByteArray filePath = container.suggestedFileName();
		int last = filePath.lastIndexOf("/");
		int count = filePath.count();
		QByteArray name = filePath.right(count-last-1);
		QString fileName = QString(name);
        CleanupStack::PushL(attachmentInfo);  
        attachmentInfo->SetAttachmentNameL(TPtrC(reinterpret_cast<const TUint16*>(fileName.utf16())));
        emailMessage->AttachmentManager().AddAttachmentL(attachment, attachmentInfo, waiter->iStatus);
        waiter->Start();
        CActiveScheduler::Start();
        CleanupStack::Pop(attachmentInfo);
        CleanupStack::Pop(&attachment); // close file
	}  

    CMsvStore* store = entry->EditStoreL();
    CleanupStack::PushL(store);
    CImHeader* emailEntry = CImHeader::NewLC();
    emailEntry->RestoreL(*store);
    emailEntry->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    
    QList<QMessageAddress> toList(message.to());
    if (toList.count() > 0){
		TPtrC16 receiver(KNullDesC);
		QString qreceiver;
		for (int i = 0; i < toList.size(); ++i) {
			qreceiver = toList.at(i).recipient();
			receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
			emailEntry->ToRecipients().AppendL(receiver); 
			}
    }
    
	QList<QMessageAddress> ccList(message.cc());
	if (ccList.count() > 0){
		TPtrC16 receiver(KNullDesC);
		QString qreceiver;
		for (int i = 0; i < ccList.size(); ++i) {
			qreceiver = ccList.at(i).recipient();
			receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
			emailEntry->CcRecipients().AppendL(receiver); 
			}
	}
		
	QList<QMessageAddress> bccList(message.bcc());
	if (bccList.count() > 0){
		TPtrC16 receiver(KNullDesC);
		QString qreceiver;
		for (int i = 0; i < bccList.size(); ++i) {
			qreceiver = bccList.at(i).recipient();
			receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
			emailEntry->BccRecipients().AppendL(receiver); 
			}
	}
     
    emailEntry->StoreL(*store);
    // Store the changes permanently
    store->CommitL();
    
    CleanupStack::PopAndDestroy(9, entry); 
    // emailmessage, bodyText, characterFormatLayer, paragraphFormatLayer, emailEntry, store, waiter, emailOperation, entry   
}

void CMTMEngine::sendEmailL(QMessage &message)
{
	QMessageAccountId accountId = message.parentAccountId();
	QMessageAccount messageAccount;
	if (accountId.isValid()) {
		messageAccount = account(accountId);
	}
	else {
		// TODO: use default account
	}
	CMsvEntry* entry = ipMsvSession->GetEntryL(messageAccount.d_ptr->_service2EntryId);
	QString name = messageAccount.name();
    CleanupStack::PushL(entry);

    TMsvId outboxId = KMsvGlobalOutBoxIndexEntryId;
    
    // creating Childrenselection
    TMsvSelectionOrdering order;
    order.SetShowInvisibleEntries(ETrue);
    entry->SetSortTypeL(order);
    entry->SetEntryL(outboxId);
    CMsvEntrySelection* selection = entry->ChildrenWithMtmL(KUidMsgTypeSMTP);

    // Fetch the Id of the first entry
    TMsvId entryId = KMsvNullIndexEntryId;
    entryId = (*selection)[0];   
    delete selection;

    CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
    TMsvId id = messageAccount.d_ptr->_service2EntryId;
    CMsvOperation* operation = entry->CopyL(entryId, id, waiter->iStatus); 
    CleanupStack::PushL(operation);
    
    waiter->Start();
    CActiveScheduler::Start(); 
    CleanupStack::PopAndDestroy(3, entry); //operation,waiter,entry
}

QDateTime CMTMEngine::symbianTTimetoQDateTime(const TTime& time) const
{
    TDateTime dateTime = time.DateTime();
    QDate qdate = QDate(dateTime.Year(), static_cast<int>(dateTime.Month())+1, dateTime.Day());
    QTime qtime = QTime(dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()/1000 );
	return QDateTime(qdate, qtime);
}

TTime CMTMEngine::qDateTimeToSymbianTTime(const QDateTime& date) const
{
    TDateTime dateTime;
    dateTime.SetYear(date.date().year());
    dateTime.SetMonth(static_cast<TMonth>(date.date().month()-1));
    dateTime.SetDay(date.date().day());
    dateTime.SetHour(date.time().hour());
    dateTime.SetMinute(date.time().minute());
    dateTime.SetSecond(date.time().second());
    dateTime.SetMicroSecond(date.time().msec()*1000);
	return TTime(dateTime);
}

QMessage CMTMEngine::smsMessageL(CMsvEntry& receivedEntry, long int messageId) const
{
    QMessage message;
    message.setType(QMessage::Sms);

    CMsvStore* pStore = receivedEntry.ReadStoreL();
    CleanupStack::PushL(pStore);
    
    const TMsvEntry& entry = receivedEntry.Entry();
    message.setDate(symbianTTimetoQDateTime(entry.iDate));
    
    // Read message sender
    ipSmsMtm->SwitchCurrentEntryL(messageId);
    ipSmsMtm->LoadMessageL();
    CSmsHeader& header = ipSmsMtm->SmsHeader();
    message.setFrom(QMessageAddress(QString::fromUtf16(header.FromAddress().Ptr(), header.FromAddress().Length()),
                                    QMessageAddress::Phone));
    
    // Read message recipients
    const CArrayPtrFlat<CSmsNumber>&  array = header.Recipients();
    QMessageAddressList messageAddresslist;
    for (int i=0; i < array.Count(); i++) {
        CSmsNumber* smsNumber = array.At(i);
        TPtrC recipientNumber = smsNumber->Address();
        messageAddresslist.append(QMessageAddress(QString::fromUtf16(recipientNumber.Ptr(),
                                                                     recipientNumber.Length()),
                                                  QMessageAddress::Phone));
    }
    message.setTo(messageAddresslist);

    // Read message body
    if (pStore->HasBodyTextL()) {
        if (!ipRichText) {
            ipCharFormatLayer = CCharFormatLayer::NewL();
            ipParaFormatLayer = CParaFormatLayer::NewL();
            ipRichText=CRichText::NewL(ipParaFormatLayer,ipCharFormatLayer);
        }
        ipRichText->Reset();
        pStore->RestoreBodyTextL(*ipRichText);
        HBufC* pMessage = HBufC::NewLC(ipRichText->DocumentLength());
        TPtr ptr2(pMessage->Des());
        ipRichText->Extract(ptr2);
        message.setBody(QString::fromUtf16(pMessage->Ptr(),pMessage->Length()));
        CleanupStack::PopAndDestroy(pMessage);
    }
    
    CleanupStack::PopAndDestroy(pStore);
    
    return message;
}

QMessage CMTMEngine::mmsMessageL(CMsvEntry& receivedEntry, long int messageId) const
{
    QMessage message;
    message.setType(QMessage::Mms);
    
    CMsvStore* pStore = receivedEntry.ReadStoreL();
    CleanupStack::PushL(pStore);

    const TMsvEntry& entry = receivedEntry.Entry();
    message.setDate(symbianTTimetoQDateTime(entry.iDate));
    
    // Read message sender
    ipMmsMtm->SwitchCurrentEntryL(messageId);
    ipMmsMtm->LoadMessageL();
    message.setFrom(QMessageAddress(QString::fromUtf16(ipMmsMtm->Sender().Ptr(), ipMmsMtm->Sender().Length()),
                                    QMessageAddress::Phone));
    
    // Read message subject
    if (receivedEntry.Entry().iDescription.Length() > 0)  {
        message.setSubject(QString::fromUtf16(receivedEntry.Entry().iDescription.Ptr(),
                                              receivedEntry.Entry().iDescription.Length()));
    }
    
    // Read message body & attachments
    TInt count = pStore->AttachmentManagerL().AttachmentCount();
    TBool textAdded = EFalse;
    for (TInt i = 0; i < count; i++) 
    {
        CMsvAttachment* pAttachment = pStore->AttachmentManagerL().GetAttachmentInfoL(i);
        CleanupStack::PushL(pAttachment);
        if (pAttachment->MimeType() == _L8("text/plain") && !textAdded) {
            // Read message body
            textAdded = ETrue;
            RFile file = pStore->AttachmentManagerL().GetAttachmentFileL(i);
            CleanupClosePushL(file);
            TInt fileSize;
            file.Size(fileSize);
            HBufC8* pFileContent = HBufC8::NewLC(fileSize);
            TPtr8 fileContent(pFileContent->Des());
            file.Read(fileContent);
            HBufC* pMsg = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*pFileContent);
            CleanupStack::PopAndDestroy(pFileContent);
            CleanupStack::PopAndDestroy(&file);
            message.setBody(QString::fromUtf16(pMsg->Ptr(), pMsg->Length()));
            delete pMsg;
        } else {
            // Read attachment
            RFile file = pStore->AttachmentManagerL().GetAttachmentFileL(i);
            CleanupClosePushL(file);
            TInt fileSize;
            file.Size(fileSize);
            HBufC8* pFileContent = HBufC8::NewL(fileSize);
            TPtr8 fileContent(pFileContent->Des());
            file.Read(fileContent);
            CleanupStack::PopAndDestroy(&file);
            CleanupStack::PushL(pFileContent);
            
            // write tempFile to private folder //TODO: Remove temp file usage
			RFs fileServer;
			User::LeaveIfError(fileServer.Connect());
			CleanupClosePushL(fileServer);
			
		    TBuf<KMaxPath> privatePath;
		    fileServer.CreatePrivatePath(EDriveC);
		    fileServer.PrivatePath(privatePath);
		    TBuf<KMaxPath> path;
		    path.Append(_L("c:"));
		    path.Append(privatePath);
		    path.Append(_L("tempattachments\\"));                         
		    CFileMan* pFileMan = CFileMan::NewL(fileServer);
		    CleanupStack::PushL(pFileMan);
		    pFileMan->RmDir(path);
		    fileServer.MkDirAll(path);
		    CleanupStack::PopAndDestroy(pFileMan);
		    
			RFile file2;
			TFileName tempFileName;
			TInt err = file2.Temp(fileServer,path,tempFileName,EFileWrite);
			if (err == KErrNone) {
				CleanupClosePushL(file2);
				file2.Write(*pFileContent);
				CleanupStack::PopAndDestroy(&file2);
				QStringList fileNames;
				fileNames.append(QString::fromUtf16(tempFileName.Ptr(),tempFileName.Length()));
				message.appendAttachments(fileNames);
			}
			CleanupStack::PopAndDestroy(&fileServer);

            //TODO:attachment.setContent(QByteArray((const char *)pFileContent->Ptr(),pFileContent->Length()));
            //TODO: message.appendContent(attachment);
            
            CleanupStack::PopAndDestroy(pFileContent);
        }
        CleanupStack::PopAndDestroy(pAttachment);
    }

    CleanupStack::PopAndDestroy(pStore);
    
    return message;
}

QMessage CMTMEngine::emailMessageL(CMsvEntry& receivedEntry, long int messageId) const
{
	QMessage message;
    message.setType(QMessage::Email);

    const TMsvEntry& entry = receivedEntry.Entry();
    message.setDate(symbianTTimetoQDateTime(entry.iDate));
    
    CImHeader* emailEntry = CImHeader::NewLC();
    CImEmailMessage* emailMessage = CImEmailMessage::NewLC(receivedEntry);
    if (receivedEntry.HasStoreL()) {
        CMsvStore* msvStore = receivedEntry.ReadStoreL();
        CleanupStack::PushL(msvStore);
        TRAP_IGNORE(emailEntry->RestoreL(*msvStore));
        CleanupStack::PopAndDestroy(); // store
        }
    
    if (!ipRichText) {
        ipCharFormatLayer = CCharFormatLayer::NewL();
        ipParaFormatLayer = CParaFormatLayer::NewL();
        ipRichText = CRichText::NewL(ipParaFormatLayer, ipCharFormatLayer);
    }
    ipRichText->Reset();
    emailMessage->GetBodyTextL(
				messageId,
                CImEmailMessage::EThisMessageOnly,
                *ipRichText,
                *ipParaFormatLayer,
                *ipCharFormatLayer);
	
    /*if (ipRichText->DocumentLength() < 1){
		retrieveBodyL(QMessageId(QString::number(messageId)));
	    emailMessage->GetBodyTextL(
					messageId,
	                CImEmailMessage::EThisMessageOnly,
	                *ipRichText,
	                *ipParaFormatLayer,
	                *ipCharFormatLayer);
    }*/
    	   
    HBufC* pMessage = HBufC::NewLC(ipRichText->DocumentLength());
	TPtr ptr2(pMessage->Des());
	ipRichText->Extract(ptr2);
	QString text = QString::fromUtf16(pMessage->Ptr(),pMessage->Length());
	message.setBody(QString::fromUtf16(pMessage->Ptr(),pMessage->Length()));
	CleanupStack::PopAndDestroy(pMessage);
    
    //from
    TPtrC from = emailEntry->From();
    message.setFrom(QMessageAddress(QString::fromUtf16(from.Ptr(), from.Length()), QMessageAddress::Email));
    QString f = QString::fromUtf16(from.Ptr(), from.Length());
    
    //to
    CDesCArray& toArray = emailEntry->ToRecipients();
    QList<QMessageAddress> toList;
    for (TInt i = 0; i < toArray.Count(); i++)
	{
		TPtrC16 to(toArray.MdcaPoint(i));
		toList.append(QMessageAddress(QString::fromUtf16(to.Ptr(), to.Length()), QMessageAddress::Email));			
	}
    message.setTo(toList);
    
    //cc
    CDesCArray& ccArray = emailEntry->CcRecipients();
    QList<QMessageAddress> ccList;
    for (TInt i = 0; i < ccArray.Count(); i++)
	{
		TPtrC16 cc(ccArray.MdcaPoint(i));
		ccList.append(QMessageAddress(QString::fromUtf16(cc.Ptr(), cc.Length()), QMessageAddress::Email));			
	}
    message.setCc(ccList);
    
    //bcc
    CDesCArray& bccArray = emailEntry->BccRecipients();
    QList<QMessageAddress> bccList;
    for (TInt i = 0; i < bccArray.Count(); i++)
	{
		TPtrC16 bcc(bccArray.MdcaPoint(i));
		bccList.append(QMessageAddress(QString::fromUtf16(bcc.Ptr(), bcc.Length()), QMessageAddress::Email));			
	}
    message.setBcc(bccList);
    
    CleanupStack::PopAndDestroy(emailMessage);
    CleanupStack::PopAndDestroy(emailEntry);
	
	// Read message subject
	if (receivedEntry.Entry().iDescription.Length() > 0)  {
		message.setSubject(QString::fromUtf16(receivedEntry.Entry().iDescription.Ptr(),
											  receivedEntry.Entry().iDescription.Length()));
	}
	
	return message;
    
}

QMessage CMTMEngine::pop3MessageL(CMsvEntry& /*receivedEntry*/, long int /*messageId*/) const
{
    return QMessage();
}

QMessage CMTMEngine::imap4MessageL(CMsvEntry& /*receivedEntry*/, long int /*messageId*/) const
{
    return QMessage();
}

QMessageStore::NotificationFilterId CMTMEngine::registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
                                                                           const QMessageFilter &filter)
{
    ipMessageStorePrivate = &aPrivateStore;
    iListenForNotifications = true;    

    int filterId = ++_filterId;
    _filters.insert(filterId, filter);
    return filterId;
}

void CMTMEngine::unregisterNotificationFilter(QMessageStore::NotificationFilterId notificationFilterId)
{
    _filters.remove(notificationFilterId);
    if (_filters.count() == 0) {
        iListenForNotifications = false;
    }
}

void CMTMEngine::notification(TMsvSessionEvent aEvent, TUid aMsgType, TMsvId aFolderId, TMsvId aMessageId)
{
    QMessageStore::NotificationFilterIdSet matchingFilters;

    // Copy the filter map to protect against modification during traversal
    QMap<int, QMessageFilter> filters(_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();
    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());

        if (filter.isEmpty()) {
            // Empty filter matches to all messages
            matchingFilters.insert(it.key());
        } else {
            QMessageFilterPrivate* privateMessageFilter = QMessageFilterPrivate::implementation(filter);
            QMessage message;
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
                
                if (privateMessageFilter->filter(message)) {
                    matchingFilters.insert(it.key());
                }
            } else if (privateMessageFilter->_field == QMessageFilterPrivate::StandardFolder) {
                if (aFolderId == KMsvGlobalInBoxIndexEntryId) {
                    QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
                } else if (aFolderId == KMsvDraftEntryId) {
                    QMessagePrivate::setStandardFolder(message,QMessage::DraftsFolder);
                } else if (aFolderId == KMsvSentEntryId) {
                    QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
                } else if (aFolderId == KMsvDeletedEntryFolderEntryId) {
                    QMessagePrivate::setStandardFolder(message,QMessage::TrashFolder);
                }       

                if (privateMessageFilter->filter(message)) {
                    matchingFilters.insert(it.key());
                }
            }
        }
    }

    if (matchingFilters.count() > 0) {
        MessageEvent event;
        if (aEvent == EMsvEntriesCreated) {
            event.notificationType = QMessageStorePrivate::Added; 
            //ipMessageStorePrivate->messageNotification(QMessageStorePrivate::Added, QMessageId(QString::number(aMessageId)), matchingFilters);
        } else if (aEvent == EMsvEntriesChanged || aEvent == EMsvEntriesMoved) {
            event.notificationType = QMessageStorePrivate::Updated; 
            //ipMessageStorePrivate->messageNotification(QMessageStorePrivate::Updated, QMessageId(QString::number(aMessageId)), matchingFilters);
        } if (aEvent == EMsvEntriesDeleted) {
            event.notificationType = QMessageStorePrivate::Removed; 
            //ipMessageStorePrivate->messageNotification(QMessageStorePrivate::Removed, QMessageId(QString::number(aMessageId)), matchingFilters);
        }
        event.messageId = aMessageId;
        event.matchingFilters = matchingFilters;
        if (iUndeliveredMessageEvents.count() == 0) {
            iDeliveryTriesCounter = 0;
        }
        iUndeliveredMessageEvents.append(event);
        tryToDeliverMessageNotifications();
    }
}

void CMTMEngine::tryToDeliverMessageNotifications()
{
    if (!IsActive()) {
        int count = iUndeliveredMessageEvents.count();
        while (count--) {
            // Try to deliver the oldest message event notification first
            MessageEvent event = iUndeliveredMessageEvents[0];
            TInt error = KErrNone;
            if (event.notificationType != QMessageStorePrivate::Removed) {
                TRAP(error, 
                    // Check if new message entry is ready to be read
                    CMsvEntry* pReceivedEntry = ipMsvSession->GetEntryL(event.messageId);
                    delete pReceivedEntry;
                    );
            }
    
            if (error == KErrNone) {
                // New message entry was ready to be read
                // Remove message event from queue
                iUndeliveredMessageEvents.removeFirst();
                iDeliveryTriesCounter = 0;
                // Deliver message event notification
                ipMessageStorePrivate->messageNotification(event.notificationType,
                                                           QMessageId(QString::number(event.messageId)),
                                                           event.matchingFilters);
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
    switch (aEvent) 
    {
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
                           CMsvEntry* pReceivedEntry;
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
    : CActive(CActive::EPriorityStandard), iOwner(aOwner), ipMsvSession(apMsvSession), iOperationId(aOperationId)
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

void CMessagesFindOperation::filterAndOrderMessages(const QMessageFilter &filter, const QMessageOrdering& ordering,
                                                    QString body, QMessageDataComparator::Options options)
{
    iFilterList.clear();
    iFilterList.append(filter);
    filterAndOrderMessages(iFilterList, ordering, body, options);
}

void CMessagesFindOperation::filterAndOrderMessages(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                                    const QMessageOrdering& ordering,
                                                    QString body,
                                                    QMessageDataComparator::Options options)
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
    }
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filters[iNumberOfHandledFilters]);

    // Handle simple id filtering cases first
    if (pf->_field == QMessageFilterPrivate::Id) { 
        iNumberOfHandledFilters++;
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageId
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                QMessageIdList ids;
                long int messageId = pf->_value.toString().toLong();
                CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
                CleanupStack::PushL(pEntry);
                const TMsvEntry& entry = pEntry->Entry();
                if (entry.iType == KUidMsvMessageEntry) {
                    ids.append(QMessageId(pf->_value.toString()));
                }
                CleanupStack::PopAndDestroy(pEntry);
                iIdList = ids;
                iTimer.After(iStatus, 1);
                if (!IsActive()) {
                    SetActive();
                }
                return;
            } else { // NotEqual
                // TODO:
            }
        } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (pf->_ids.count() > 0) { // QMessageIdList
                if (cmp == QMessageDataComparator::Includes) {
                    QMessageIdList ids;
                    for (int i=0; i<pf->_ids.count(); i++) {
                        long int messageId = pf->_ids[i].toString().toLong();
                        CMsvEntry* pEntry = ipMsvSession->GetEntryL(messageId);
                        CleanupStack::PushL(pEntry);
                        const TMsvEntry& entry = pEntry->Entry();
                        if (entry.iType == KUidMsvFolderEntry) {
                            ids.append(QMessageId(pf->_value.toString()));
                        }
                        CleanupStack::PopAndDestroy(pEntry);
                    }
                    iIdList = ids;
                    iTimer.After(iStatus, 1);
                    if (!IsActive()) {
                        SetActive();
                    }
                    return;
                } else { // Excludes
                    // TODO:
                }
            } else { // QMessageFilter
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
        }
    }

    // Set ordering
    TMsvSelectionOrdering sort;
    if (!ordering.isEmpty() ) {
        QMessageOrderingPrivate* privateMessageOrdering = QMessageOrderingPrivate::implementation(ordering);
        sort.SetShowInvisibleEntries(EFalse);
        QPair<QMessageOrderingPrivate::Field, Qt::SortOrder> fieldOrder = privateMessageOrdering->_fieldOrderList.at(0);
        switch (fieldOrder.first) {
        case QMessageOrderingPrivate::Type:
            sort.SetGroupByMtm(true);
            break;
        case QMessageOrderingPrivate::Sender:
            if (fieldOrder.second == Qt::AscendingOrder) {
                sort.SetSorting(EMsvSortByDetails); // To/From (A-Z folded)
            } else {
                sort.SetSorting(EMsvSortByDetailsReverse); // To/From (Z-A folded)
            }
            break;
        case QMessageOrderingPrivate::Recipients:
            if (fieldOrder.second == Qt::AscendingOrder) {
                sort.SetSorting(EMsvSortByDetails); // To/From (A-Z folded)
            } else {
                sort.SetSorting(EMsvSortByDetailsReverse); // To/From (Z-A folded)
            }
            break;
        case QMessageOrderingPrivate::Subject:
            if (fieldOrder.second == Qt::AscendingOrder) {
                sort.SetSorting(EMsvSortByDescription); // Description (A-Z folded)
            } else {
                sort.SetSorting(EMsvSortByDescriptionReverse); // Description (Z-A folded)
            }
            break;
        case QMessageOrderingPrivate::TimeStamp:
            if (fieldOrder.second == Qt::AscendingOrder) {
                sort.SetSorting(EMsvSortByDate); // Date (earliest-latest) 
            } else {
                sort.SetSorting(EMsvSortByDateReverse); // Date (latest-earliest)
            }
            break;
        case QMessageOrderingPrivate::ReceptionTimeStamp:
            if (fieldOrder.second == Qt::AscendingOrder) {
                sort.SetSorting(EMsvSortByDate); // Date (earliest-latest) 
            } else {
                sort.SetSorting(EMsvSortByDateReverse); // Date (latest-earliest)
            }
            break;
        case QMessageOrderingPrivate::Read:
            //TODO:
            break;
        case QMessageOrderingPrivate::HasAttachments:
            //TODO:
            break;
        case QMessageOrderingPrivate::Incoming:
            //TODO:
            break;
        case QMessageOrderingPrivate::Removed:
            //TODO:
            break;
        case QMessageOrderingPrivate::Priority:
            sort.SetGroupByPriority(true);
            break;
        case QMessageOrderingPrivate::Size:
            if (fieldOrder.second == Qt::AscendingOrder) {
                sort.SetSorting(EMsvSortBySize); // (smallest-largest) 
            } else {
                sort.SetSorting(EMsvSortBySizeReverse); // (largest-smallest)
            }
            break;
        }
    }
    
    switch (pf->_field) {
    case QMessageFilterPrivate::ParentAccountId:
        {
        iNumberOfHandledFilters++;
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageAccountId
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
                getAccountSpecificMessagesL(messageAccount, sort, privateFilter);
            } else { // NotEqual
                ipEntrySelection = new(ELeave)CMsvEntrySelection;

                QMessageFilterPrivate* privateFilter = NULL;
                if (filters.count() > iNumberOfHandledFilters) {
                    privateFilter = QMessageFilterPrivate::implementation(filters[iNumberOfHandledFilters]);
                    if (privateFilter->_field != QMessageFilterPrivate::StandardFolder) {
                        privateFilter = NULL;
                    } else {
                        iNumberOfHandledFilters++;
                    }
                }
                
                for (int i=0; i < iOwner.iAccounts.count(); i++) {
                    if (iOwner.iAccounts[i].id().toString() != pf->_value.toString()) {
                        getAccountSpecificMessagesL(iOwner.iAccounts[i], sort, privateFilter);
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
        iNumberOfHandledFilters++;
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageFolderId
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            long int folderId = pf->_value.toString().toLong(); 
            if (cmp == QMessageDataComparator::Equal) {
                CMsvEntry* pEntry = ipMsvSession->GetEntryL(folderId);
                pEntry->SetSortTypeL(sort);
                CleanupStack::PushL(pEntry);
                ipEntrySelection = pEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
                CleanupStack::PopAndDestroy(pEntry);
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
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
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
        QMessage::Type type = static_cast<QMessage::Type>(pf->_value.toInt()); 
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessage::Type
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                ipEntrySelection = new(ELeave)CMsvEntrySelection;
                QMessageAccountIdList accountIds = iOwner.accountsByType(type);
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
                for (int i = 0; i < accountIds.count(); i++) {
                    QMessageAccount messageAccount = iOwner.account(accountIds[i]);
                    getAccountSpecificMessagesL(messageAccount, sort, privateFilter);
                }
            } else { // NotEqual
                // TODO:
            }
        } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) { // QMessage::TypeFlags
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                // TODO:
            } else { // Excludes
                // TODO:
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
			CMsvEntry* pStandardFolderContext = CMsvEntry::NewL(*ipMsvSession, stdFolderId, sort);
			CleanupStack::PushL(pStandardFolderContext);
			ipEntrySelection = pStandardFolderContext->ChildrenL();
			CleanupStack::PopAndDestroy(pStandardFolderContext);
        } else { // NotEqual
			ipEntrySelection = new(ELeave)CMsvEntrySelection;
			QMessage::StandardFolder i = QMessage::InboxFolder;
			while (i <= QMessage::TrashFolder) {
				if (i != standardFolder) {
					CMsvEntry* pStandardFolderContext = CMsvEntry::NewL(*ipMsvSession, iOwner.standardFolderId(i), sort);
					CleanupStack::PushL(pStandardFolderContext);
					CMsvEntrySelection* pEntries = pStandardFolderContext->ChildrenL();
					CleanupStack::PushL(pEntries);
					for (int i = 0; i < pEntries->Count(); i++) {
						ipEntrySelection->AppendL(pEntries->At(i));
					}
					CleanupStack::PopAndDestroy(pEntries);
					CleanupStack::PopAndDestroy(pStandardFolderContext);
				}
				i = static_cast<QMessage::StandardFolder>(static_cast<int>(i) + 1);
			}
        }
        break;
        }
    }
    
    TMsvPartList partlist = 0;
    TPtrC16 value(KNullDesC);
    if (body.isEmpty()) {
        if (iNumberOfHandledFilters < filters.count()) {    
            pf = QMessageFilterPrivate::implementation(filters[iNumberOfHandledFilters]);
            
            if (pf->_options & QMessageDataComparator::CaseSensitive) {
                partlist |= KMsvFindCaseSensitive;
            }
            if (pf->_options & QMessageDataComparator::FullWord) {
                partlist |= KMsvFindWholeWord;
            }
    
            switch (pf->_field) {
            case QMessageFilterPrivate::Sender:
                {
                if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                    QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                    if (cmp == QMessageDataComparator::Equal) {
                        value.Set(reinterpret_cast<const TUint16*>(pf->_value.toString().utf16()));
                        partlist |= KMsvMessagePartOriginator;
                        iNumberOfHandledFilters++;
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
                        value.Set(reinterpret_cast<const TUint16*>(pf->_value.toString().utf16()));
                        partlist |= KMsvMessagePartRecipient;
                        iNumberOfHandledFilters++;
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
                        value.Set(reinterpret_cast<const TUint16*>(pf->_value.toString().utf16()));
                        partlist |= KMsvMessagePartDescription;
                        iNumberOfHandledFilters++;
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
            }
        }
    } else {
        // Body 
        if (options & QMessageDataComparator::CaseSensitive) {
            partlist |= KMsvFindCaseSensitive;
        }
        if (options & QMessageDataComparator::FullWord) {
            partlist |= KMsvFindWholeWord;
        }
        
        value.Set(reinterpret_cast<const TUint16*>(body.utf16()));
        partlist |= KMsvMessagePartBody;
    }

    if (partlist != 0) {
        if (ipEntrySelection) {
            ipMsvFindOperation = CMsvFindOperation::FindInSelectionL(*ipMsvSession, value, *ipEntrySelection, partlist, iStatus);
        } else {
            ipMsvFindOperation = CMsvFindOperation::FindInChildrenL(*ipMsvSession, value, KMsvRootIndexEntryId, partlist, iStatus);
        }
        if (!IsActive()) {
            SetActive();
        }
    } else {
        iIdList = QMessageIdList();
        for (int i=0; i < ipEntrySelection->Count(); i++) {
            iIdList.append(QMessageId(QString::number((*ipEntrySelection)[i]))); 
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
        iOwner.filterAndOrderMessagesReady(false, iOperationId, QMessageIdList(), iNumberOfHandledFilters);
    } else {
        if (ipMsvFindOperation) {
            const CMsvFindResultSelection& findResultSelection = ipMsvFindOperation->GetFindResult();
            QMessageIdList msgIds;
            for (int i=0; i < findResultSelection.Count(); i++) {
                msgIds.append(QMessageId(QString::number(findResultSelection[i].iId))); 
            }
            iOwner.filterAndOrderMessagesReady(true, iOperationId, msgIds, iNumberOfHandledFilters);
        } else {
            iOwner.filterAndOrderMessagesReady(true, iOperationId, iIdList, iNumberOfHandledFilters);
        }
    }
}

void CMessagesFindOperation::getAccountSpecificMessagesL(QMessageAccount& messageAccount, const TMsvSelectionOrdering ordering, QMessageFilterPrivate* privateFolderFilter)
{
    CMsvEntry* pService = ipMsvSession->GetEntryL(messageAccount.d_ptr->_service1EntryId);
    TUid mtmUid = pService->Entry().iMtm;
    delete pService;
    getMTMSpecificMessagesL(mtmUid, ordering, privateFolderFilter);

    TMsvId serviceId = messageAccount.d_ptr->_service2EntryId;
    if (serviceId != 0) {
        pService = ipMsvSession->GetEntryL(serviceId);
        TUid mtmUid = pService->Entry().iMtm;
        delete pService;
        getMTMSpecificMessagesL(mtmUid, ordering, privateFolderFilter);
    }
}

void CMessagesFindOperation::getMTMSpecificMessagesL(TUid mtmUid, const TMsvSelectionOrdering ordering, QMessageFilterPrivate* privateFolderFilter)
{
    bool allFolders = true;
    if (privateFolderFilter) {
        QMessageDataComparator::EqualityComparator cmp2(static_cast<QMessageDataComparator::EqualityComparator>(privateFolderFilter->_comparatorValue));
        QMessage::StandardFolder standardFolder = static_cast<QMessage::StandardFolder>(privateFolderFilter->_value.toInt()); 
        if (cmp2 == QMessageDataComparator::Equal) {
            getMTMSpecificMessagesFromFolderL(mtmUid, ordering, iOwner.standardFolderId(standardFolder));
        } else { // NotEqual
            // Loop through all standard folders
            QMessage::StandardFolder i = QMessage::InboxFolder;
            while (i <= QMessage::TrashFolder) {
                if (i != standardFolder) {
                    getMTMSpecificMessagesFromFolderL(mtmUid, ordering, iOwner.standardFolderId(i));
                }
                i = static_cast<QMessage::StandardFolder>(static_cast<int>(i) + 1);
            }
        }
        allFolders = false;
    }
    if (allFolders) {
        ipEntrySelection = new(ELeave)CMsvEntrySelection;
        QMessage::StandardFolder i = QMessage::InboxFolder;
        while (i <= QMessage::TrashFolder) {
            getMTMSpecificMessagesFromFolderL(mtmUid, ordering, iOwner.standardFolderId(i));
            i = static_cast<QMessage::StandardFolder>(static_cast<int>(i) + 1);
        }
    }
}

void CMessagesFindOperation::getMTMSpecificMessagesFromFolderL(TUid mtmUid, const TMsvSelectionOrdering ordering, TMsvId standardFolderId)
{
    if (mtmUid == KUidMsgTypePOP3) {
        if (standardFolderId == KMsvGlobalInBoxIndexEntryIdValue) {
            CMsvEntry* pCurrentEntry = ipMsvSession->GetEntryL(KMsvRootIndexEntryId);
            CleanupStack::PushL(pCurrentEntry);
            pCurrentEntry->SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,EMsvSortByNone, ETrue));
            for(TInt i = 0; i < pCurrentEntry->Count(); i++) {
                const TMsvEntry& child = (*pCurrentEntry)[i];
                if (child.iMtm == mtmUid) {
                    CMsvEntry* pEntry = ipMsvSession->GetEntryL(child.Id());
                    CleanupStack::PushL(pEntry);
                    pEntry->SetSortTypeL(ordering);
                    CMsvEntrySelection* pEntries = pEntry->ChildrenL();
                    for (int i = 0; i < pEntries->Count(); i++) {
                        ipEntrySelection->AppendL(pEntries->At(i));
                    }
                    CleanupStack::PopAndDestroy(pEntry);
                    break;
                }
            }
            CleanupStack::PopAndDestroy(pCurrentEntry);
        }
    } else {
        CMsvEntry* pStandardFolderContext = CMsvEntry::NewL(*ipMsvSession, standardFolderId, ordering);
        CleanupStack::PushL(pStandardFolderContext);
        pStandardFolderContext->SetSortTypeL(ordering);
        CMsvEntrySelection* pEntries = pStandardFolderContext->ChildrenWithMtmL(mtmUid);
        CleanupStack::PushL(pEntries);
        for (int i = 0; i < pEntries->Count(); i++) {
            ipEntrySelection->AppendL(pEntries->At(i));
        }
        CleanupStack::PopAndDestroy(pEntries);
        CleanupStack::PopAndDestroy(pStandardFolderContext);
    }
}


QTM_END_NAMESPACE
