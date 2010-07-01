/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @publishedAll
 @released
*/
#include "timeoutbasestep.h"
#include "performancefunctionalitydefs.h"

CTimeoutBaseStep::CTimeoutBaseStep(const TInt aContacts, const TInt aDeviation) 
		:	CPerformanceFunctionalityBase( aContacts ), 
			iTimeOut(KStartTime),
			iDeviation(aDeviation)
	{
	}
/**
Checks that the relevant function call waits for the expected amount of time
also checks that the timeout error matches the expected error
*/	
TBool CTimeoutBaseStep::CheckTimeOutL(	TInt (CTimeoutBaseStep::*aFunc)(CContactDatabase *, const TContactItemId),
										CContactDatabase *aContactsDatabase, 
										const TContactItemId aCid,
										const TInt aLockErr,
										const TInt aTimeout,
										const TBool aSetTime)
	{
	TBool SingleCheck = EFalse;
	TBool OverallResult = EFalse;
	TInt deviation = 0;
	if( aSetTime )
		{
		SetTimeOutL( aContactsDatabase );
		}
	iStart.UniversalTime();
	TInt err = (this->*aFunc)(aContactsDatabase, aCid);
	iEnd.UniversalTime();
	deviation = static_cast<TInt>( iEnd.MicroSecondsFrom( iStart ).Int64() - aTimeout );
	TESTPRINT( SingleCheck = (aLockErr == err) ); 
	OverallResult = SingleCheck;
	TESTPRINT( SingleCheck = ( Abs( deviation ) <= iDeviation ) );
	OverallResult = OverallResult && SingleCheck;
	if(!OverallResult)
		{
		_LIT(KTimeoutError,"Timeout check failed with following deviance %d <= %d and err %d == %d");
		ERR_PRINTF5(KTimeoutError, Abs(deviation), iDeviation, aLockErr, err);
		}
	return OverallResult;
	}

//set the timeout for this session
void CTimeoutBaseStep::SetTimeOutL(CContactDatabase *aContactsDatabase)
	{
	if( aContactsDatabase == NULL )
		{
		aContactsDatabase = iContactsDatabase;
		}
	aContactsDatabase->SetOperationTimeOutL( iTimeOut );
	}
//the following methods attemp to call a specific contact api and return any errors generated. 
TInt CTimeoutBaseStep::CheckOpenL(CContactDatabase *aContactsDatabase, const TContactItemId aCid)
	{
	CContactItem *contactItem = NULL;
	TRAPD(err, contactItem = aContactsDatabase->OpenContactL( aCid, *iViewAll ));
	if( KErrNone == err )
		{
		iContactsDatabase->CloseContactL(contactItem->Id()); // this method *CANNOT* leave, depsite the trailing 'L'
		CLEAR( contactItem ); 
		}
	return err;
	}

TInt CTimeoutBaseStep::CheckUpdateL(CContactDatabase *aContactsDatabase, const TContactItemId aCid)
	{
	CContactItem *contactItemR = iContactsDatabase->ReadContactLC( aCid, *iViewAll );
	TRAPD(err, 
				CContactItem *contactItem = aContactsDatabase->UpdateContactLC( aCid, contactItemR );
				CleanupStack::PopAndDestroy( contactItem ); 
		);
	CleanupStack::PopAndDestroy( contactItemR );
	contactItemR = NULL;
	return err;
	}
	
TInt CTimeoutBaseStep::CheckRead(CContactDatabase *aContactsDatabase, const TContactItemId aCid)
	{
	TRAPD(err, 
				CContactItem *contactItem = aContactsDatabase->ReadContactL( aCid, *iViewAll );
				CLEAR( contactItem ); 
		);
	return err;
	}
	
TInt CTimeoutBaseStep::CheckAddL(CContactDatabase *aContactsDatabase, const TContactItemId /*aCid*/)
	{
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	TContactItemId cid = 0;
	TRAPD(err, 
				cid = aContactsDatabase->doAddNewContactL(*contact, EFalse, iTransaction);
				iIterate->AddL( cid ); 
		);
	CleanupStack::PopAndDestroy(contact);
	return err;
	}
	
TInt CTimeoutBaseStep::CheckDelete(CContactDatabase *aContactsDatabase, const TContactItemId aCid)
	{
	TRAPD(err, 
				aContactsDatabase->doDeleteContactL( aCid, ETrue, iTransaction );
				iIterate->RemoveL(aCid);
		);
	return err;
	}

