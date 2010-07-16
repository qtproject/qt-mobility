/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
@SYMPREQ                  PREQ1132 PREQ1187
@SYMComponent             app-engines_cntmodel
@SYMAuthor                Simon Mellor, JamesCl
@SYMTestStatus            Implemented
@SYMTestType              CT

@SYMTestCaseDesc          Tests the performance of a bulk delete operation on a corporate-profile 
						  database of 1000 contacts.
						  
@SYMTestActions           Measures the time taken to perform a bulk delete on the contacts database
						  by passing a CContactIdArray to:
						  -- CContactDatabase::DeleteContactsL()

@SYMTestExpectedResults   Test case will run without leaving and will output timing information.

*/

#include "t_perfbulkdelete.h"
#include "../t_utils.h"
#include <cntitem.h>

// Constants   
_LIT(KDbPathName,"C:Contacts.cdb");
const TPtrC KDbPath( (KDbPathName) );

_LIT(KOutputFormat,"Bulk Delete,  deleting %d of %d contacts, took: %d s %03d\n");
_LIT(KOutputFormat2, "Db size before/after delete: %d / %d bytes\n");


CDeleteMany::CDeleteMany()
	{
	} 

CDeleteMany::~CDeleteMany()
	{
	delete iDb;
	} 
	
CDeleteMany* CDeleteMany::NewLC(RTest& aTest)
	{
	CDeleteMany* self = new(ELeave) CDeleteMany();
	CleanupStack::PushL(self);
	self->ConstructL(aTest);
	return(self);
	}  

void CDeleteMany::ConstructL(RTest& aTest)
	{
	iTest = &aTest;
	iDb = CContactDatabase::OpenL(KDbPath);
	CCntTest::ProfileReset(0, 59);
	} 

/*
	Creates a random array of numbers corresponding to the Contacts Id in the 
	Contacts model.  No Contact Id is repeated  
*/
void CDeleteMany::CreateRandomIdArrayL(CContactIdArray& aUids, TInt aNumIds, TInt aNumEntriesInDb)
	{
	TTime now;
	now.UniversalTime();
	TInt64 KRandomSeed = now.DateTime().MicroSecond();

	TInt uid(0);
	for(TInt i = 0; i < aNumIds; ++i)
		{
		uid = (Math::Rand(KRandomSeed) % aNumEntriesInDb) + 1;
		while(aUids.Find(uid) != KErrNotFound)
			{
			uid = (Math::Rand(KRandomSeed) % aNumEntriesInDb) + 1;
			}
		aUids.AddL(uid);		
		}
	}
	
/**
	Static convenience cleanup method copied from CSmlContactsDba  
*/
void CDeleteMany::CleanupPtrArray(TAny* aCArrayPtr)
	{ 
	CArrayPtr<CContactItem>* array = reinterpret_cast<CArrayPtr<CContactItem>*>(aCArrayPtr);
	array->ResetAndDestroy();
	delete array;
	}	
	
	
/**
	Main test routine.  Calls and times CContactDatabase::DeleteContactsL
*/
void CDeleteMany::DelDataL(TInt aEntryCount, TInt aNumEntriesInDb, TInt aNumMsToGet20ItemsFromView)
	{
	TInt dbSizeBefore = iDb->FileSize();
	
	CContactIdArray* idArray = CContactIdArray::NewLC();
	
	// create an array of random, non-repeating contact ids
	CreateRandomIdArrayL(*idArray, aEntryCount, aNumEntriesInDb);	

	TCntPerfTimer timer;
	timer.StartTimer();
	iDb->DeleteContactsL(*idArray);
	timer.StopTimer();

	CleanupStack::PopAndDestroy(); // idArray
	
	const TInt KTransactionSize(50); // num records deleted in a transaction

 	// add in number of screen updates need in UI: (time to fetch 20 view items) * number of transactions
 	TInt result = timer.Result() +
 		(aNumMsToGet20ItemsFromView * ( (aEntryCount / KTransactionSize) + (aEntryCount % 20 ? 1 : 0) ) );


	TBuf<64> formattable;
	formattable.Format(KOutputFormat, aEntryCount, aNumEntriesInDb, result / 1000000, (result / 1000) % 1000);
	iTest->Printf(formattable);
    formattable.Format(KOutputFormat2, dbSizeBefore, iDb->FileSize());
	iTest->Printf(formattable);
	}
