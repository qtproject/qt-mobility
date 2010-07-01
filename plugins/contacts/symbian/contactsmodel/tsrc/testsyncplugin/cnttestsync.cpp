/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <cntsync.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdef.h>
#include <cntitem.h>
#include <cntdb.h>
#include "cnttestsynchroniser.h"
#include <implementationproxy.h>


CContactActiveTestSync::CContactActiveTestSync(CContactSyncChecker& aSyncChecker, TUid aPhoneBookUid) 
: CActive(EPriorityMuchMore), iStatusArray(3), iSyncChecker(aSyncChecker), iPhoneBookUid(aPhoneBookUid)
	{
	CActiveScheduler::Add(this);
	}

CContactActiveTestSync::~CContactActiveTestSync()
	{	
	Cancel();	
	}

void CContactActiveTestSync::RunL()
	{
	const TInt result = iSyncChecker.FetchNotificationErrorL(iPhoneBookUid);
	for(TInt i = 0; i < iStatusArray.Count();  ++i)
		{
		User::RequestComplete(iStatusArray[i], result);
		}
	iStatusArray.Reset();
	}

TInt CContactActiveTestSync::RunError(TInt /*aError*/)
	{
	TRAP_IGNORE(RunL() );
	return KErrNone;
	}

void CContactActiveTestSync::DoCancel()
	{
	iSyncChecker.CompleteNotificationRequestL(KErrCancel, iPhoneBookUid);
	for(TInt i = 0; i < iStatusArray.Count();  ++i)
		{
		User::RequestComplete(iStatusArray[i], KErrCancel);
		}
	iStatusArray.Reset();	
	}
	
void CContactActiveTestSync::StartL(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iStatusArray.AppendL(&aStatus);
	if (!IsActive())
		{
		__ASSERT_ALWAYS(iStatusArray.Count() == 1, User::Invariant());
		iStatus = KRequestPending;
		SetActive();
		iSyncChecker.StoreNotificationRequestL(iStatus, iPhoneBookUid);
		}
	}
	
TUid CContactActiveTestSync::PhoneBookUid()
{
	return iPhoneBookUid;
}




TInt CContactTestSynchroniser::ValidateContact(TValidateOperation aOp, TContactItemId /*aId*/)
	{
	iContactSyncChecker->ValidateMethodCalledL();
	TInt ret=iContactSyncChecker->ValidateResponseL(aOp);
	if (iContactSyncChecker->VerboseLoggingEnabledL())
		{
		switch (aOp)
			{
		case ERead:		
			User::InfoPrint(_L("Validate ERead"));
			break;
		case ESearch:
			User::InfoPrint(_L("Validate ESearch"));
			break;
		case EEdit:
			User::InfoPrint(_L("Validate EEdit"));
			break;
		default:
			break;
			}
		}
	return ret;
	}
	
TInt CContactTestSynchroniser::ValidateWriteContact(CContactICCEntry& aContactItem)
	{
	TRAPD(err, iContactSyncChecker->ValidateMethodCalledL() );
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant() );
	
	TBool verboseLoggingEnabled(EFalse);
	TRAP(err, verboseLoggingEnabled = iContactSyncChecker->VerboseLoggingEnabledL() );
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant() );
	if (verboseLoggingEnabled)
		{
		User::InfoPrint(_L("ValidateWriteContact"));
		}

	// Set the phonebook field, if the field doesn't exist, it should be added
	CContactItemFieldSet& fieldset = aContactItem.CardFields();
	TInt n = fieldset.Find(KUidContactFieldICCPhonebook);
	if(n == KErrNotFound)
		{
		CContactItemField* field(NULL);
		TRAP_IGNORE(field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldICCPhonebook) );
		TRAP(err, aContactItem.AddFieldL(*field) );
		CContactItemFieldSet& fieldset = aContactItem.CardFields();
		n = fieldset.Find(KUidContactFieldICCPhonebook);
		__ASSERT_ALWAYS(err == KErrNone && n != KErrNotFound, User::Invariant() );
		}

	CContactItemField& field = fieldset[n];
	CContactTextField* textField = field.TextStorage();
	TUid phonebookUid(TUid::Null() );
	TRAP(err, phonebookUid = iContactSyncChecker->PhonebookUidL(aContactItem.TemplateRefId() ) );
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant() );

	TBuf<128> buf;
	if(phonebookUid != KUidIccPhonebookNotFound)
		{
		TInt phonebookUidNum = phonebookUid.iUid;
		buf.AppendNum(phonebookUidNum);
		TRAP(err, textField->SetTextL(buf));
		__ASSERT_ALWAYS(err == KErrNone, User::Invariant() );
		}

	TBool validateWriteResponse(EFalse);
	TRAP(err, validateWriteResponse = iContactSyncChecker->ValidateWriteResponseL() );
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant() );

	return validateWriteResponse;
	}

TInt CContactTestSynchroniser::DeleteContact(TContactItemId /*aId*/)
	{
	iContactSyncChecker->ValidateMethodCalledL();
	if (iContactSyncChecker->VerboseLoggingEnabledL())
		User::InfoPrint(_L("DeleteContact"));
	return iContactSyncChecker->DeleteContactResponseL();
	}


/** 
 * Update the lookup tables in the server.
 * This method is called by the Contacts model when a new CContactICCEntry
 * has been added to the database. The plug-in should extract the 
 * appropriate details from the item (eg. item UID and slot number) 
 * and call the sync server to update the lookup tables.
 */
void CContactTestSynchroniser::UpdatePostWriteL(const CContactICCEntry& /*aContactItem*/)
	{
	iContactSyncChecker->ValidateMethodCalledL();
	if (iContactSyncChecker->VerboseLoggingEnabledL())
		User::InfoPrint(_L("UpdateLookupTable"));
	}	

TContactItemId CContactTestSynchroniser::ICCTemplateIdL(TUid aPhonebookUid)
	{
	iContactSyncChecker->ValidateMethodCalledL();
	User::InfoPrint(_L("TemplateId"));
	return iContactSyncChecker->ICCTemplateIdL(aPhonebookUid);
	}

TContactItemId CContactTestSynchroniser::GroupIdL(TUid aPhonebookUid)
	{
	
	iContactSyncChecker->ValidateMethodCalledL();
	User::InfoPrint(_L("GroupId"));
	return iContactSyncChecker->GroupIdL(aPhonebookUid);
	}

void CContactTestSynchroniser::NotifySyncStateChange(TRequestStatus& aStatus, TUid aPhonebookUid)
	{
	TRAPD(err, iContactSyncChecker->SyncMethodCalledL() );
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant() );
	
	TBool exist(EFalse);
	CContactActiveTestSync* temp(NULL);
	TInt i(0);
	TRAP(err, 
		for (i = 0 ; i < iActiveTestSync.Count() ; ++i)
			{
			temp = iActiveTestSync[i];		
			if (temp->PhoneBookUid().iUid == aPhonebookUid.iUid)
				{
				exist = ETrue;	
				temp->StartL(aStatus);
				break;	
				}
			}
	);
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant() );

	if (!exist)
		{
		temp = new CContactActiveTestSync(*iContactSyncChecker, aPhonebookUid);
		__ASSERT_ALWAYS(temp != NULL, User::Invariant() );
		iActiveTestSync.Append(temp);
		TRAP(err, temp->StartL(aStatus) );	
		__ASSERT_ALWAYS(err == KErrNone, User::Invariant() );
		}
	}

void CContactTestSynchroniser::CancelNotifyRequest(TUid aPhonebookUid)
	{
	iContactSyncChecker->SyncMethodCalledL();
	if (iContactSyncChecker->VerboseLoggingEnabledL())
		{
		RDebug::Print(_L("PhonebookSync - CancelNotifyRequest(Phonebook Uid = 0x%08X)\r\n"),
			aPhonebookUid.iUid);
		}
	for (TInt i = 0 ; i < iActiveTestSync.Count() ; ++i)
		{
		CContactActiveTestSync* temp = iActiveTestSync[i];		
		if (temp->PhoneBookUid().iUid == aPhonebookUid.iUid)
			{
  			temp->Cancel();
			break;	
			}
		}
	}

TBool CContactTestSynchroniser::IsSynchronisedL(TUid aPhonebookUid)
	{
	iContactSyncChecker->SyncMethodCalledL();
	if (iContactSyncChecker->VerboseLoggingEnabledL())
		{
		RDebug::Print(_L("PhonebookSync - IsSynchronisedL(Phonebook Uid = 0x%08X)\r\n"),
			aPhonebookUid.iUid);
		}
	return iContactSyncChecker->IsSynchronisedL(aPhonebookUid);
	}
	
TInt CContactTestSynchroniser::PhonebookList(RArray<TUid>& aPhonebookList) //Lots to do here
	{
	iContactSyncChecker->SyncMethodCalledL();
	if (iContactSyncChecker->VerboseLoggingEnabledL())
		{
		RDebug::Print(_L("PhonebookSync - PhonebookList()\r\n"));
		}
	return iContactSyncChecker->PhonebookListL(aPhonebookList);
	}
	
CContactTestSynchroniser* CContactTestSynchroniser::NewL()
	{
	RDebug::Print(_L("**********CContactTestSynchroniser::NewL\n"));
	CContactTestSynchroniser* self = new(ELeave) CContactTestSynchroniser();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CContactTestSynchroniser::ConstructL()
	{
	iContactSyncChecker = iContactSyncChecker->NewL();
	//iActiveTestSync = new (ELeave) CContactActiveTestSync(*iContactSyncChecker);
	iContactSyncChecker->PluginLoadedL();
	}


void CContactTestSynchroniser::Release()
	{
	User::InfoPrint(_L("Release"));
	TRAP_IGNORE(iContactSyncChecker->PluginUnloadedL());
	delete this;
	}
	
CContactTestSynchroniser::~CContactTestSynchroniser()
	{
	RDebug::Print(_L("**********CContactTestSynchroniser::~CContactTestSynchroniser\n"));
	delete iContactSyncChecker;
	iContactSyncChecker = NULL;
	iActiveTestSync.ResetAndDestroy();
	}

// Export the implementation collection function
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102035FF, 	CContactTestSynchroniser::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
/**
 * Standard Epoc32 Dll Entry point
 */
