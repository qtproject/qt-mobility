/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CNTSYNCDATA_H
#define CNTSYNCDATA_H

#include "cntsyncchecker.h"
#include <cntdef.h>
#include <cntdb.h>
#include "cntsyncphonedata.h"

#include <centralrepository.h>  // crepository.
#include <e32property.h>
#include <phbksync.h>




class CContactSyncData : public CBase
	{

public:
	static CContactSyncData* NewL();
	~CContactSyncData();
	
	 CContactSyncData& DataL();
	 void ReadConfigurationDataL();
	 void WriteConfigurationDataL();
	 void ConstructL();
	 RPointerArray<CContactSyncPhonebookData>& PerPhonebookData();
	 CContactSyncPhonebookData* PhonebookData(TUid aPhonebookUid);
	 CContactSyncPhonebookData* PhonebookDataFromTemplateId(TContactItemId aTemplateId);

	 void AllocPhonebookDataL(TUid aPhonebookUid);
	
	void ResetCallCountAndValidateResponsesL();
	void DeleteAllPhonebooks();

public:
	TUid iLastCommitedPhonebookUid;
enum TRepositoryKeys
        {
        ERefCount = 0x00000001,
        EValidateCallCount,
        EValidateSyncCallCount,
        EValidateWriteResponse,
        EValidateDeleteResponse,
        EValidateReadResponse,
        EValidateEditResponse,
        EValidateSearchResponse,
        EVerboseLoggingEnabled       
        };

public:
	// reference count - number of open connections
	TInt iRefCount;
	// general data
	TInt iValidateCallCount;
	TInt iSyncCallCount;
	TInt iPluginCallCount;
	TInt iValidateWriteResponse;
	TInt iDeleteContactResponse;
	TInt iReadValidateResponse;
	TInt iEditValidateResponse;
	TInt iSearchValidateResponse;
	TBool iVerboseLoggingEnabled;
private:
	RPointerArray<CContactSyncPhonebookData> iPerPhonebookData;
	CRepository* iRepository;
	};	
#endif

