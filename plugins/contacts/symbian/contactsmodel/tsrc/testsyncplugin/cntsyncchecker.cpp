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


#include "../testsyncplugin/cntsyncchecker.h"
#include "../testsyncplugin/cntsyncdata.h"
#include "../testsyncplugin/cntsyncphonedata.h"

#include <cntdef.h>
#include <cntdb.h>

#include <centralrepository.h>  // crepository.
#include <e32property.h>
#include <phbksync.h>

class CContactSyncPhonebookData;
class CPhonebookContact;





EXPORT_C CContactSyncChecker* CContactSyncChecker::NewL()
	{
	CContactSyncChecker* self = new(ELeave) CContactSyncChecker();
	return self;
	}
	
	

EXPORT_C CContactSyncChecker::~CContactSyncChecker()	
	{
	//TRAP_IGNORE(ResetL() );
	}

EXPORT_C TInt CContactSyncChecker::ValidateResponseL(MContactSynchroniser::TValidateOperation aOp)
	{

	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TInt ret;
	switch (aOp)
		{
	case MContactSynchroniser::ERead:
		ret = settings.iReadValidateResponse;
		break;
	case MContactSynchroniser::ESearch:
		ret = settings.iSearchValidateResponse;
		break;
	case MContactSynchroniser::EEdit:
		ret = settings.iEditValidateResponse;
		break;
	default:
		ret = KErrNone;
		break;
		}
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

EXPORT_C TInt CContactSyncChecker::ValidateWriteResponseL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TInt ret(settings.iValidateWriteResponse);
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

EXPORT_C TInt CContactSyncChecker::DeleteContactResponseL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TInt ret(settings.iDeleteContactResponse);
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

EXPORT_C TContactItemId CContactSyncChecker::ICCTemplateIdL(TUid aPhonebookUid)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TContactItemId ret = KNullContactId;
	CContactSyncPhonebookData* tempData = settings.PhonebookData(aPhonebookUid);	
	if (tempData)
		{
		ret = tempData->TemplateId();
		}
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

EXPORT_C TContactItemId CContactSyncChecker::GroupIdL(TUid aPhonebookUid)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TContactItemId ret = KNullContactId;
	CContactSyncPhonebookData* tempData = settings.PhonebookData(aPhonebookUid);	
	if (tempData)
		{
		ret = tempData->GroupId();
		}
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

EXPORT_C TUid CContactSyncChecker::PhonebookUidL(TContactItemId aTemplateId)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TUid ret = KUidIccPhonebookNotFound;
	CContactSyncPhonebookData* tempData = settings.PhonebookDataFromTemplateId(aTemplateId);	
	if (tempData)
		{
		ret = tempData->PhonebookUid();
		}
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}
	
EXPORT_C void CContactSyncChecker::ResetEverythingL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	// all Validate Responses set to an error
	settings.ResetCallCountAndValidateResponsesL();
	// delete all phonebooks
	settings.DeleteAllPhonebooks();
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}
	
/**
 Current count of Phonebook Synch State change notifications outstanding - for all phonebooks
 (Note this is across all Views using the test DLL.)
 */
EXPORT_C TInt CContactSyncChecker::TotalNotificationPendingCountL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	RPointerArray<CContactSyncPhonebookData>& perPhbkData = settings.PerPhonebookData();
	const TInt phbkCount = perPhbkData.Count();
	TInt total = 0;

	for (TInt k = 0; k < phbkCount; ++k)
		{
		total += perPhbkData[k]->NotificationPendingCount();
		}
	CleanupStack::PopAndDestroy(temp);
	return total;
	}

EXPORT_C void CContactSyncChecker::SetValidateResponseL(MContactSynchroniser::TValidateOperation aOp, TInt aError)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	switch (aOp)
		{
	case MContactSynchroniser::ERead:
		settings.iReadValidateResponse=aError;
		break;
	case MContactSynchroniser::ESearch:
		settings.iSearchValidateResponse=aError;
		break;
	case MContactSynchroniser::EEdit:
		settings.iEditValidateResponse=aError;
		break;
	default:
		break;
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}

EXPORT_C void CContactSyncChecker::SetValidateWriteResponseL(TInt aError)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iValidateWriteResponse=aError;
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}

EXPORT_C void CContactSyncChecker::SetDeleteContactResponseL(TInt aError)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iDeleteContactResponse=aError;
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}

EXPORT_C void CContactSyncChecker::SetPhonebookEntryL(TUid /*aPhonebookUid*/,TInt /*aPhonebookTemplateId*/,TInt /*aPhonebookGroupId*/)
	{
/*	TLookupPhonebookEntry phonebookEntry(aPhonebookUid,aPhonebookTemplateId,aPhonebookGroupId);
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iPhonebookEntries.Append(phonebookEntry);
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);*/
	}
	
EXPORT_C TInt CContactSyncChecker::UpdatePhonebookEntryL(TUid aPhonebookUid, TInt aPhonebookTemplateId, TInt aPhonebookGroupId)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	TInt ret = KErrNotFound;
	if (phbkData)
		{
		phbkData->SetTemplateId(aPhonebookTemplateId);
		phbkData->SetGroupId(aPhonebookGroupId);
		ret = KErrNone;
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);

	return ret;
	}


EXPORT_C void CContactSyncChecker::EnableVerboseLoggingL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iVerboseLoggingEnabled=ETrue;
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}

EXPORT_C TBool CContactSyncChecker::VerboseLoggingEnabledL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TBool ret = settings.iVerboseLoggingEnabled;
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

EXPORT_C void CContactSyncChecker::SyncMethodCalledL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iSyncCallCount++;
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}
	


EXPORT_C void CContactSyncChecker::ValidateMethodCalledL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iValidateCallCount++;
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}

EXPORT_C void CContactSyncChecker::ResetMethodCallCountsL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iSyncCallCount = 0;
	settings.iValidateCallCount = 0;
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}
	
EXPORT_C void CContactSyncChecker::PluginLoadedL()
    {
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iPluginCallCount++;
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
    }
    
EXPORT_C void CContactSyncChecker::PluginUnloadedL()
    {
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	settings.iPluginCallCount--;
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
    }

EXPORT_C TInt CContactSyncChecker::ValidateMethodCallCountL()
	{
	TInt ret;
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	ret = settings.iValidateCallCount;
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

EXPORT_C TInt CContactSyncChecker::SyncMethodCallCountL()
	{
	TInt ret;
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	ret = settings.iSyncCallCount;
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}
	
EXPORT_C TInt CContactSyncChecker::PluginLoadedCountL()
    {
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TInt ret = settings.iPluginCallCount;
	CleanupStack::PopAndDestroy(temp);
	return ret;
    }

EXPORT_C void CContactSyncChecker::ResetL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	temp->WriteConfigurationDataL();
	CleanupStack::PopAndDestroy(temp);
	}


EXPORT_C void CContactSyncChecker::SetSynchronisedL(TBool aIsSynchronised, TUid aPhonebookUid)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	if (!phbkData)
		{
		User::Invariant();
		}
	phbkData->SetSynchronised(aIsSynchronised);
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}

EXPORT_C TBool CContactSyncChecker::IsSynchronisedL(TUid aPhonebookUid)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	if (!phbkData)
		{
		User::Leave(KErrNotSupported);
		}
	TBool ret = phbkData->IsSynchronisedL();;
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}





 EXPORT_C void CContactSyncChecker::StoreNotificationRequestL(TRequestStatus& aStatus, TUid aPhonebookUid)
 	{
	RProperty property;
 	CContactSyncData* temp = CContactSyncData::NewL();
 	CleanupStack::PushL(temp);
 	CContactSyncData& settings = temp->DataL();
 	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	TInt error(KErrNone);
	if (!phbkData)
		{
		error = KErrNotSupported;
		}
	else
		{
		aStatus=KRequestPending;
		//Store any value, the value does not mean anything, the reason is to let the vector grow
		phbkData->StoreNotificationRequest(100);
 		settings.WriteConfigurationDataL();
	 	// Define a property using Publish and Subscribe.
	    RProperty::Define(
	    	SyncNotification::KPropertyCategory, //Category
	        aPhonebookUid.iUid, //key
	        RProperty::EInt); //Attr
 
		// Subscribe for the dst event
		User::LeaveIfError(property.Attach(SyncNotification::KPropertyCategory,aPhonebookUid.iUid));
		property.Subscribe(aStatus);
		User::After(1000);	
		}
	// some problem encountered
	if (error)
		{
		TRequestStatus* pS = &aStatus;
		User::RequestComplete(pS, error);
		}
 	CleanupStack::PopAndDestroy(temp);
 	}
 
 EXPORT_C void CContactSyncChecker::CompleteNotificationRequestL(TInt aError, TUid aPhonebookUid)
 	{
 	CContactSyncData* temp = CContactSyncData::NewL();
 	CleanupStack::PushL(temp);
 	CContactSyncData& settings = temp->DataL();
  	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	if (!phbkData)
		{
		User::Invariant();
		}
	phbkData->CompleteNotificationRequest(aError);			
	settings.WriteConfigurationDataL();	
 	CleanupStack::PopAndDestroy(temp);
 	}
 
EXPORT_C TInt CContactSyncChecker::FetchNotificationErrorL(TUid aPhoneBookUid)
	{
 	CContactSyncData* temp = CContactSyncData::NewL();
 	CleanupStack::PushL(temp);
 	CContactSyncData& settings = temp->DataL();
  	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhoneBookUid);
	if (!phbkData)
		{
		User::Invariant();
		}
	TInt ret = phbkData->ErrorCode();
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}	

EXPORT_C TInt CContactSyncChecker::NotificationPendingCountL(TUid aPhonebookUid)
/**
 * Current count of Phonebook Synch State change notifications outstanding
 * (Note this is across all Views using the test DLL.)
 */
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
  	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	if (!phbkData)
		{
		User::Invariant();
		}

	TInt ret = phbkData->NotificationPendingCount();
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}


EXPORT_C void CContactSyncChecker::SetSynchronisedLeaveCodeL(TInt aLeave, TUid aPhonebookUid)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
  	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	if (!phbkData)
		{
		User::Invariant();
		}
	phbkData->SetSynchronisedLeaveCode(aLeave);
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	}


EXPORT_C TInt CContactSyncChecker::UpdatePhonebookTemplateIdL(TUid aPhonebookUid, TInt aPhonebookTemplateId)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	TInt ret = KErrNotFound;
	if (phbkData)
		{
		phbkData->SetTemplateId(aPhonebookTemplateId);
		ret = KErrNone;
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}
	
EXPORT_C TInt CContactSyncChecker::UpdatePhonebookGroupIdL(TUid aPhonebookUid, TInt aPhonebookGroupId)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	TInt ret = KErrNotFound;

	if (phbkData)
		{
		phbkData->SetGroupId(aPhonebookGroupId);
		ret = KErrNone;
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

EXPORT_C TInt CContactSyncChecker::SynchroniseIccPhonebookL(CContactDatabase& aDb, TUid aPhonebookUid, TInt aCompletionError, TInt aSyncError)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	TInt ret = KErrNotSupported;

	if (phbkData)
		{
		ret = phbkData->SynchronisePhonebook(aDb, aCompletionError, aSyncError);
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);
	return ret;
	}

/**
 Synchronise all waiting phonebooks.
 Also sets all validate responses to KErrNone.

  @param aDb connection to the databse to sync to.
  @param aCompletionError System error to send to all observers
  @param aSyncError System error to leave with when Phonebook Sync IsSynchronisedL() API is called.
 */
EXPORT_C TInt CContactSyncChecker::SynchroniseIccAllPhonebooksL(CContactDatabase& aDb, TInt aCompletionError, TInt aSyncError)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	RPointerArray<CContactSyncPhonebookData>& perPhbkData = settings.PerPhonebookData();
	const TInt phbkCount = perPhbkData.Count();
	TInt error(KErrNone);

	if ((aCompletionError == KErrNone) && (aSyncError == KErrNone))
		{
		SetValidateResponseL(MContactSynchroniser::ERead, KErrNone);
		SetValidateResponseL(MContactSynchroniser::EEdit, KErrNone);
		SetValidateResponseL(MContactSynchroniser::ESearch, KErrNone);
		SetValidateWriteResponseL(KErrNone);
		SetDeleteContactResponseL(KErrNone);
		}

	// search through phonebook list
	for (TInt k = 0; (k < phbkCount) && (error == KErrNone); ++k)
		{
		// synchronise all waiting phonebooks
		if (perPhbkData[k]->SynchroniseRequired())
			{
			error = perPhbkData[k]->SynchronisePhonebook(aDb, aCompletionError, aSyncError);
			}
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);

	return error;
	}


EXPORT_C TInt CContactSyncChecker::AddContactToPhonebookL(TUid aPhonebookUid, const TDesC& aName, const TDesC& aNumber)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	TInt ret = KErrNotSupported;
	if (phbkData)
		{
		TRAP(ret, phbkData->AddPhonebookContactL(aName, aNumber));
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);	
	return ret;
	}
	
EXPORT_C TInt CContactSyncChecker::ResetSyncIccPhonebookL(TUid aPhonebookUid, TBool aResetTemplateAndGroupIds)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	CContactSyncPhonebookData* phbkData = settings.PhonebookData(aPhonebookUid);
	TInt ret = KErrNotSupported;

	if (phbkData)
		{
		phbkData->ResetSynchronised(aResetTemplateAndGroupIds);
		ret = KErrNone;
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);	
	return ret;
	}
	
EXPORT_C TInt CContactSyncChecker::ResetSyncIccAllPhonebooksL(TBool aResetTemplateAndGroupIds)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	RPointerArray<CContactSyncPhonebookData>& perPhbkData = settings.PerPhonebookData();
	const TInt phbkCount = perPhbkData.Count();

	// search through phonebook list
	for (TInt k = 0; k < phbkCount; ++k)
		{
		// synchronise all waiting phonebooks
		perPhbkData[k]->ResetSynchronised(aResetTemplateAndGroupIds);
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);	
	return KErrNone;
	}
	
// Configure simulation for GSM Phonebook
EXPORT_C TInt CContactSyncChecker::SetGsmPhonebookL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TInt error = KErrNone;
	if (settings.PerPhonebookData().Count() != 0)
		{
		// too late - the phonebook list has already been set
		error = KErrGeneral;
		}
		else
		{
		TRAP(error, settings.AllocPhonebookDataL(KUidIccGlobalAdnPhonebook));		
		}
	settings.WriteConfigurationDataL();	
	// create object for GSM ADN phonebook
	CleanupStack::PopAndDestroy(temp);	
	return error;
	}
	
// Configure simulation for USIM (UMTS/WCDMA) Phonebook
EXPORT_C TInt CContactSyncChecker::SetUsimPhonebookL()
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TInt error = KErrNone;

	if (settings.PerPhonebookData().Count() != 0)
		{
		// too late - the phonebook list has already been set
		error = KErrGeneral;
		}
		else
		{
		// create object for USIM App ADN phonebook
		TRAP(error, settings.AllocPhonebookDataL(KUidUsimAppAdnPhonebook));	
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);	
	return error;
	}
	
// Configure custom Phonebook simulation
EXPORT_C TInt CContactSyncChecker::SetPhonebookListL(const RArray<TUid> aNewPhonebookList)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	TInt error = KErrNone;
	if (settings.PerPhonebookData().Count() != 0)
		{
		// too late - the phonebook list has already been set
		error = KErrGeneral;
		}
		else
		{
		TInt const phbkCount = aNewPhonebookList.Count();
		error = KErrNone;

		for (TInt k = 0; (k < phbkCount) && !error; ++k)
			{
			// create object for requested phonebook
			TRAP(error, settings.AllocPhonebookDataL(aNewPhonebookList[k]));
			}			
		}
	settings.WriteConfigurationDataL();	
	CleanupStack::PopAndDestroy(temp);		
	return error;
	}

// Get phonebook list
EXPORT_C TInt CContactSyncChecker::PhonebookListL(RArray<TUid>& aPhonebookList)
	{
	CContactSyncData* temp = CContactSyncData::NewL();
	CleanupStack::PushL(temp);
	CContactSyncData& settings = temp->DataL();
	RPointerArray<CContactSyncPhonebookData>& perPhbkData = settings.PerPhonebookData();
	const TInt phbkCount = perPhbkData.Count();
	TInt ret = KErrNone;
	if (phbkCount == 0)
		{
		// too early - the phonebook list has not been set
		ret = KErrNotReady;
		}
		else
		{
		// copy/create phonebook list
		aPhonebookList.Reset();
		for (TInt k = 0; k < phbkCount; ++k)
			{
			if (aPhonebookList.Append(perPhbkData[k]->PhonebookUid()) != KErrNone)
				{
				// error
				aPhonebookList.Reset();
				ret = KErrNoMemory;
				break;
				}
			}
			
		}
	CleanupStack::PopAndDestroy(temp);		
	return ret;
	}


