/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <s32mem.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include "t_utils.h"
#include <cntfilt.h>


_LIT(KDatabaseFileName,"c:t_connec");
_LIT(KCntTestName,"Connectivity functionality tests");
_LIT(KCntContactName,"Contact %d");

const TInt KTotalNumRecords=10;

GLDEF_D TLocale OriginalLocale;
GLDEF_D CCntTest* CntTest = NULL;
GLDEF_D RTest test(KCntTestName);




/** Set the contents of a text field */
LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	}


/** Populate the database */
LOCAL_C void PopulateDatabaseL(TInt aNumberToPopulate)
	{
	for (TInt i=0;i<aNumberToPopulate;i++)
		{
		CContactItem* item=CContactCard::NewLC(); 
		TBuf<20> name;
		name.Format(KCntContactName,i);
		SetNameL(*item,KUidContactFieldGivenName,name);     
		CntTest->Db()->AddNewContactL(*item); 
		CleanupStack::PopAndDestroy(item);
		}
	CntTest->Db()->CompactL();
	}





LOCAL_C void TestFilteringL()
	{
	_LIT(KProgressIndicator,".");
	test.Printf(KProgressIndicator);

	CCntFilter* filter = CCntFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetContactFilterTypeCard(ETrue);
	TTime time;
	time.UniversalTime();
	time-=TTimeIntervalMinutes(20); // changes in the last 20 mins
	filter->SetFilterDateTime(time);
	filter->SetIncludeNewContacts(ETrue);
	CntTest->Db()->FilterDatabaseL(*filter);
	test(filter->iIds->Count()==KTotalNumRecords);

// modified contact
	User::After(4000000);	// Let time pass to distinguish between mod  / new
	// this may still fail if the device is running too slow (e.g. under a heavy load)
	
	filter->Reset();
	time.UniversalTime();
	filter->SetFilterDateTime(time);
	filter->SetIncludeModifiedContacts(ETrue);
	CntTest->Db()->FilterDatabaseL(*filter);
	test(filter->iIds->Count()==0);
	
	CContactItem* contact = CntTest->Db()->OpenContactLX(5);
	CleanupStack::PushL(contact);
	contact->IncAccessCount();
	CntTest->Db()->CommitContactL(*contact);
	CleanupStack::PopAndDestroy(contact);
	CleanupStack::Pop(); // contact.Close()

	CntTest->Db()->FilterDatabaseL(*filter);
	test(filter->iIds->Count()==1);

// deleted contact
	filter->Reset();
	filter->SetFilterDateTime(time); // same time as for modified
	filter->SetIncludeDeletedContacts(ETrue);
	CntTest->Db()->FilterDatabaseL(*filter);
	test(filter->iIds->Count()==0);

	CntTest->Db()->DeleteContactL(5);
	CntTest->Db()->FilterDatabaseL(*filter);
	TInt deletedContactCount = filter->iIds->Count();
	test(deletedContactCount ==1);
	
#ifdef _DEBUG
	CContactIdArray* deletedCntArray = CntTest->Db()->DeletedContactsLC();
	test(deletedCntArray->Count()==deletedContactCount);
	CleanupStack::PopAndDestroy(deletedCntArray ); 
#endif	

	CleanupStack::PopAndDestroy(filter);
	}

LOCAL_C void SimpleFilterTestL()
	{
	CCntFilter* filter = CCntFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetContactFilterTypeCard(ETrue);
	TTime time;
	time.UniversalTime();
	time-=TTimeIntervalMinutes(20); // changes in the last 20 mins
	filter->SetFilterDateTime(time);
	filter->SetIncludeNewContacts(ETrue);
	CntTest->Db()->FilterDatabaseL(*filter);
	test(filter->iIds->Count()==KTotalNumRecords);
	CleanupStack::PopAndDestroy(filter);
	}

LOCAL_C void NonDateBasedFilterTest()
	{
	//This test has been created because, formerly,
	//CContactDatabase::FilterDatabase(...) could not
	//filter on contact types ONLY.
	//Previously, the CCntFilter passed in had to 
	//have specified filtering based on the 
	//time-based states as well (e.g. new, modified, deleted).
	//Addresses DEF036345
	CCntFilter* theFilter = CCntFilter::NewLC();
	theFilter->SetContactFilterTypeGroup(EFalse);
	theFilter->SetContactFilterTypeTemplate(EFalse);
	theFilter->SetContactFilterTypeOwnCard(EFalse);
	theFilter->SetContactFilterTypeCard(ETrue);
	//Get a filtered version of the database
	CntTest->Db()->FilterDatabaseL(*theFilter);
	test(theFilter->iIds->Count()==KTotalNumRecords);
	CleanupStack::PopAndDestroy(theFilter);
	}

// Regression testcode for EDNJKAA-4WLDAY 
// "Cannot synchronise Outlook contacts with German device"
//
// Tests filtering under the following conditions 
// Time & date separator '.' 
// Time separator ':', date separator '.' 
// Time & date separator ':'
// Time separator '.', date separator ':'
// 
LOCAL_C void TestFilterLocalesL()
	{
	SimpleFilterTestL();
	NonDateBasedFilterTest();
	TInt i=0;
	TLocale example;
	example.SetDateFormat(EDateEuropean);
	for (i=0; i<4; i++)
		{
		example.SetTimeSeparator(TChar('.'),i);
		example.SetDateSeparator(TChar('.'),i);
		}
	example.Set();
	SimpleFilterTestL();

	example.SetDateFormat(EDateAmerican);
	for (i=0; i<4; i++)
		{
		example.SetTimeSeparator(TChar(':'),i);
		}
	example.Set();
	SimpleFilterTestL();

	example.SetDateFormat(EDateJapanese);
	for (i=0; i<4; i++)
		{
		example.SetDateSeparator(TChar(':'),i);
		}
	example.Set();
	SimpleFilterTestL();

	for (i=0; i<4; i++)
		{
		example.SetTimeSeparator(TChar('.'),i);
		}
	example.Set();
	SimpleFilterTestL();
	}


LOCAL_C void TestSyncUid()
	{
	// test sync / connectivity

	TTime time;
	time.UniversalTime();
	TContactSyncId newSyncId = CntTest->Db()->SetLastSyncDateL(time);
	TTime newTime;
	CntTest->Db()->GetLastSyncDateL(newSyncId,newTime);
	test(newTime==time);
	User::After(1000000);
	time.UniversalTime();
	CntTest->Db()->SetLastSyncDateL(newSyncId, time);
	TTime latestTime;
	CntTest->Db()->GetLastSyncDateL(newSyncId,latestTime);
	test(latestTime > newTime);
	test(latestTime==time);
	test(newTime!=time);
	CntTest->Db()->SetLastSyncDateL(newSyncId++, time);
	}

// SetInclude* are exclusive
// SetContactFilterType* is cumulative
//
LOCAL_C void CheckFilterSettingsL(TBool aSetting)
	{
	CCntFilter* filter = CCntFilter::NewLC();

	filter->Reset();
	filter->SetIncludeModifiedContacts(aSetting);
	if (aSetting)
		{
		test(filter->IncludeModifiedContacts());
		test(!filter->IncludeNewContacts());
		test(!filter->IncludeDeletedContacts());
		}

	filter->SetIncludeNewContacts(aSetting); 
	if (aSetting)
		{
		test(filter->IncludeNewContacts());
		test(!filter->IncludeModifiedContacts());
		test(!filter->IncludeDeletedContacts());
		}


	filter->SetIncludeDeletedContacts(aSetting);
	if (aSetting)
		{
		test(filter->IncludeDeletedContacts());
		test(!filter->IncludeModifiedContacts());
		test(!filter->IncludeNewContacts());
		}

	filter->SetContactFilterTypeCard(aSetting);
 	filter->SetContactFilterTypeGroup(aSetting);
	filter->SetContactFilterTypeTemplate(aSetting);
	filter->SetContactFilterTypeOwnCard(aSetting);
	if (aSetting)
		{
		test(filter->ContactFilterTypeGroup());
		test(filter->ContactFilterTypeCard());
		test(filter->ContactFilterTypeTemplate());
		test(filter->ContactFilterTypeOwnCard());
		}
	else
		{
		test(!filter->ContactFilterTypeGroup());
		test(!filter->ContactFilterTypeCard());
		test(!filter->ContactFilterTypeTemplate());
		test(!filter->ContactFilterTypeOwnCard());	
		}
	
	filter->SetContactFilterTypeALL(aSetting);
	if (aSetting)
		{
		test(filter->ContactFilterTypeALL());
		test(filter->ContactFilterTypeGroup());
		test(filter->ContactFilterTypeCard());
		test(filter->ContactFilterTypeTemplate());
		test(filter->ContactFilterTypeOwnCard());
		}

	TTime time;
	time.UniversalTime();
	filter->SetFilterDateTime(time);
	test(filter->GetFilterDateTime()==time);
		
	CleanupStack::PopAndDestroy(filter);
	}


// Construction tests 
// NewL methods are called by NewLC variants so there's no tests for that
//
LOCAL_C void TestFilterConstructionL()
	{
	CCntFilter* filter = CCntFilter::NewLC();
	TTime time;
	time.UniversalTime();
	filter->SetFilterDateTime(time);

	CCntFilter* clone = CCntFilter::NewLC(filter);
	test(clone->GetFilterDateTime()==time);

	CleanupStack::PopAndDestroy(2); //filter,clone
	}

LOCAL_C void TestFilterL()
	{
	CheckFilterSettingsL(ETrue);
	CheckFilterSettingsL(EFalse);
	TestFilterConstructionL();
	TestFilteringL();
	}

LOCAL_C void RetrieveLocale(TAny *)
	{
	OriginalLocale.Set();
	}

/**

@SYMTestCaseID     PIM-T-CONNEC-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-CONNEC-0001 Connectivity functionality tests"));


	OriginalLocale.Refresh();
	CleanupStack::PushL(TCleanupItem(RetrieveLocale, 0));

	CntTest->CreateDatabaseL();
	CntTest->OpenDatabaseL();
	PopulateDatabaseL(KTotalNumRecords);

	
	TestFilterL();

	TestFilterLocalesL();

	TRAPD(err,TestSyncUid());
	test(err==KErrNone);

	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CleanupStack::PopAndDestroy(); // locale
    }

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CntTest = new CCntTest;
	if (!CntTest)
		{
		return KErrNoMemory;
		}

	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	test.Printf(_L("\n"));
	CntTest->EndTestLib(err);
	__UHEAP_MARKEND;
	return KErrNone;
    }
