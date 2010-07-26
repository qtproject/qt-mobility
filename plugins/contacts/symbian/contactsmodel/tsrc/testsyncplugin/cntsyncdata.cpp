/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cntsyncdata.h"
#include "cntphonecontact.h"
#include <cntdef.h>
#include <cntdb.h>
#include "cntsyncphonedata.h"

#include <centralrepository.h>  // crepository.
#include <e32property.h>
#include <phbksync.h>

CContactSyncData::~CContactSyncData()
	{
    iPerPhonebookData.ResetAndDestroy();
	iPerPhonebookData.Close();
    delete iRepository;	
	}



void CContactSyncData::ReadConfigurationDataL()
	{
	// Connect to the Central Repository and internalize.
	TUint32 crKey = ERefCount;	
	TInt temp;
	TInt numberOfPhoneBooks = 0;
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iRefCount, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iValidateCallCount, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iSyncCallCount, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iPluginCallCount, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iValidateWriteResponse, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iDeleteContactResponse, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iReadValidateResponse, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iEditValidateResponse, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iSearchValidateResponse, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, iVerboseLoggingEnabled, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, temp, *iRepository);
	CContactSyncStaticRepository::ReadIntFromCentralRepositoryL(crKey, numberOfPhoneBooks, *iRepository);
	iLastCommitedPhonebookUid.iUid = temp;
    iPerPhonebookData.ResetAndDestroy();
    for (TInt i = 0 ; i < numberOfPhoneBooks ; i++)
	    {
	    CContactSyncPhonebookData* temp = CContactSyncPhonebookData::NewL(crKey, iRepository);
	    iPerPhonebookData.Append(temp);
	    }
	}
	
void CContactSyncData::WriteConfigurationDataL()
	{
	TUint32 crKey = ERefCount;	
	// Connect to the Central Repository and externalize.
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iRefCount, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iValidateCallCount, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iSyncCallCount, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iPluginCallCount, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iValidateWriteResponse, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iDeleteContactResponse, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iReadValidateResponse, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iEditValidateResponse, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iSearchValidateResponse, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iVerboseLoggingEnabled, *iRepository);
	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iLastCommitedPhonebookUid.iUid, *iRepository);
 	CContactSyncStaticRepository::WriteIntToCentralRepositoryL(crKey, iPerPhonebookData.Count(), *iRepository);


    for (TInt i = 0 ; i < iPerPhonebookData.Count() ; i++)
	    {
	    CContactSyncPhonebookData* temp = iPerPhonebookData[i];
		temp->ExternaliseToCRL(crKey);
	    }
	}



CContactSyncData& CContactSyncData::DataL()
	{
	ReadConfigurationDataL();
	return *this;	
	}



CContactSyncData* CContactSyncData::NewL()
	{
	CContactSyncData* self = new (ELeave) CContactSyncData;
	CleanupStack::PushL(self);	
	self->ConstructL();
	CleanupStack::Pop();		
	return self;
	}

void  CContactSyncData::ConstructL()
	{
	// Connect to the Central Repository and get the current configuration.
    iRepository = CRepository::NewL(KPropertyCategory);
   // ReadConfigurationDataL();
    iRefCount++;
	}

void CContactSyncData::ResetCallCountAndValidateResponsesL()
	{
	iSyncCallCount = 0;
	iValidateCallCount = 0;
	iPluginCallCount = 0;
	iValidateWriteResponse = 0;
	iDeleteContactResponse = 0;
	iReadValidateResponse = 0;
	iEditValidateResponse = 0;
	iSearchValidateResponse = 0;
	WriteConfigurationDataL();
	}
	
RPointerArray<CContactSyncPhonebookData>& CContactSyncData::PerPhonebookData()
	{
	return iPerPhonebookData;
	}

CContactSyncPhonebookData* CContactSyncData::PhonebookData(TUid aPhonebookUid)
	{
	RPointerArray<CContactSyncPhonebookData>& perPhbkData = PerPhonebookData();
	const TInt phbkCount = perPhbkData.Count();

	// search through phonebook list
	for (TInt k = 0; k < phbkCount; ++k)
		{
		if (perPhbkData[k]->PhonebookUid() == aPhonebookUid)
			{
			return perPhbkData[k];
			}
		}
	return (CContactSyncPhonebookData*) NULL;
	}
	
CContactSyncPhonebookData* CContactSyncData::PhonebookDataFromTemplateId(TContactItemId aTemplateId)
	{
	if (aTemplateId == KNullContactId)
		{
		return (CContactSyncPhonebookData*) NULL;
		}

	RPointerArray<CContactSyncPhonebookData>& perPhbkData = PerPhonebookData();
	const TInt phbkCount = perPhbkData.Count();

	// search through phonebook list
	for (TInt k = 0; k < phbkCount; ++k)
		{
		if (perPhbkData[k]->TemplateId() == aTemplateId)
			{
			return perPhbkData[k];
			}
		}

	return (CContactSyncPhonebookData*) NULL;
	}


void CContactSyncData::AllocPhonebookDataL(TUid aPhonebookUid)
	{
	iPerPhonebookData.AppendL(CContactSyncPhonebookData::NewLC(aPhonebookUid, iRepository));
	CleanupStack::Pop();
	}


void CContactSyncData::DeleteAllPhonebooks()
	{
	iPerPhonebookData.ResetAndDestroy();
	}
