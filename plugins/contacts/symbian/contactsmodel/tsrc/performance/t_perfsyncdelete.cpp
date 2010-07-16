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
* This is a performance test harness measuring a sync deletion of 
* a proportion of Contacts in the default database.
*
*/


#include <cntvcard.h>
#include <cntitem.h>
#include <f32file.h>
#include <s32mem.h>
#include "t_perfsyncdelete.h"
#include "../t_utils.h"

// Constants

_LIT(KDbName,"C:Contacts.cdb");

const TPtrC KDbPath((KDbName)); 
const TInt KCompactResolution = 16;
_LIT(KOutputFormat,"Sync Delete took: %d s %03d deleting %d of %d Contacts\n");

CSyncDelete::CSyncDelete(TInt64 aSeed)
	{
	iRandSeed = aSeed;
	} 

CSyncDelete::~CSyncDelete()
	{
	delete iDb;
	iFs.Close();
	} 
	
CSyncDelete* CSyncDelete::NewLC(RTest& aTest, TInt64 ranSeed)
	{;
	CSyncDelete* self = new (ELeave) CSyncDelete(ranSeed);
	CleanupStack::PushL(self);
	self->ConstructL(aTest);
	return(self);
	}  

void CSyncDelete::ConstructL(RTest& aTest)
	{
	iTest = &aTest;
	iDb = CContactDatabase::OpenL(KDbPath);
	User::LeaveIfError(iFs.Connect());
	CCntTest::ProfileReset(0, 59);
	} 

TBool CSyncDelete::ExistsInArray(const CArrayFix<TInt>& aArr, TInt aSize, TInt aNum)
/**
    Returns whether aNum exists in aArr from 0 to aSize in the aArr array
*/
	{
	for(TInt i = 0; i < aSize; ++i)
		{
		if (aNum == aArr[i])
			{
			return ETrue;
			}
		}
	return EFalse;
	}
	
void CSyncDelete::CreateRandomIdArray(CArrayFix<TInt>& aUids, TInt aNumIds, TInt aNumEntriesInDb)
/*
	Creates a random array of numbers corresponding to the uids in the 
	Contacts model.  No UID is repeated  
*/
	{
	for(TInt i = 0; i < aNumIds; ++i)
		{
		TInt uid = Math::Rand(iRandSeed)%(aNumEntriesInDb-1)+1;
		while(ExistsInArray(aUids,i,uid))
			{
			uid = Math::Rand(iRandSeed)%(aNumEntriesInDb-1)+1;
			}
		aUids[i] = uid;		
		}
	}
	
void CSyncDelete::CleanupPtrArray(TAny* aCArrayPtr)
/**
	Static convenience cleanup method copied from CSmlContactsDba  
*/
	{ 
	CArrayPtr<CContactItem>* array = reinterpret_cast<CArrayPtr<CContactItem>*>(aCArrayPtr);
	array->ResetAndDestroy();
	delete array;
	}	

void CSyncDelete::DoSyncL(TInt aEntryCount, TInt aNumEntriesInDb)
/** This routine deleted aEntryCount of aNumEntriesInDb where the Ids to delete are 
    generated at random.  This harness does not tests Sync Update
*/	
	{
	// create an array of random, non-repeating contact ids
	CArrayFixFlat<TInt>* uids = new(ELeave) CArrayFixFlat<TInt>(aEntryCount);
	CleanupStack::PushL(uids);
	uids->ResizeL(aEntryCount);	
	CreateRandomIdArray(*uids,aEntryCount,aNumEntriesInDb);
	
	// Most SML servers perform all the delete operations followed by all the
	// update operations (or vice verca) rather than intermingling the 2
	// operations. We are only interested in the deletion aspect at this state.
	//
	CCntTest::ProfileStart(0);
	for (TInt i = 0; i < aEntryCount; ++i)
		{		;
		const TContactItemId KEntryId = static_cast<TContactItemId>((*uids)[i]); 
		iDb->DeleteContactL(KEntryId);
		
		if ((i > 0) && ((i % KCompactResolution) == 0)) 
			{
			iDb->CompactL();
			}
		}
	CleanupStack::PopAndDestroy(uids);	
	CCntTest::ProfileEnd(0);
	
	TCntProfile p[1];
	TBuf<64> formattable;
	TInt sec;
	TInt msec; 
	
	CCntTest::ProfileResult(&p[0],0,1);	
	sec = p[0].iTime/1000000;
	msec = (p[0].iTime/1000)%1000;
	formattable.Format(KOutputFormat, sec, msec, aEntryCount, aNumEntriesInDb);
	iTest->Printf(formattable);
	}
