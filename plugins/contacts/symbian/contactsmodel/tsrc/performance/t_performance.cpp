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


/**
@SYMTestCaseID PIM-APPENG-CNTMODEL-CNTDB-U-0001-HP

@SYMPREQ					PREQ1132 PREQ1192 PREQ1187
@SYMComponent				app-engines_cntmodel
@SYMAuthor					Simon Mellor, JamesCl
@SYMTestStatus				Implemented
@SYMTestType				CT

@SYMTestCaseDesc			Tests the performance of contacts model operations to provide benchmark data

@SYMTestActions				This test harness performs performance benchmarking for operations on 
							a corporate-profile database of 1000 contacts, for the following use 
							cases:
							-- Initial Sync: import a contacts db from a vcard file to mimic a sync
							-- Startup: open database, create view, open some contacts
							-- Bulk Delete: 1, 20 and 1000 contacts
							-- Resort View: by combinations of given name, family name, company and phone number
							-- Match Phone Number 
							-- Create, Read, Update and Delete operations on 1 Contact


@SYMTestExpectedResults		Test cases will run without leaving and will output timing information.

@SYMTestCaseDependencies	This test case relies on the PIM test server to run. 
	
*/

#include "t_perfinitialsync.h"
#include "t_perfbulkdelete.h"
#include "t_perfstartup.h"
#include "t_perfdbcreator.h"
#include "t_perfresortview.h"
#include "t_perfcrudoperations.h"

#include <coreappstest/testserver.h>

_LIT(KTestName,"T_Performance");
_LIT(KContactsDBFileNameC,"c:\\private\\10003A73\\SQLite__Contacts.cdb");
_LIT(KContactsDBSaveFileNameC,"c:\\private\\10003A73\\SQLite__ContactsSave.cdb");
_LIT(KVCardLoc, "Z:\\core-apps\\app-engines\\cntmodel\\performance\\corporate-1000-0.vcf");
_LIT(KDbFileName, "C:Contacts.cdb");
_LIT(KDbFileSaveName, "C:ContactsSave.cdb");

// The number of contacts in the database can be changed to see 
// the effect of varying the DB size on deletion speed.
const TInt KNumContacts  = 1000;
const TInt K1Contact     = 1;
const TInt K20Contacts   = 20;
const TInt K1000Contacts = 1000;


LOCAL_D RTest test(KTestName);

/**
	Copy one database file to another.
*/
static void CopyFileL(const TDesC& aSource, const TDesC& aDest)
	{	
	RPIMTestServer serv;	
	CleanupClosePushL(serv);
	User::LeaveIfError(serv.Connect());
	serv.CopyFileL(aSource, aDest);
	CleanupStack::PopAndDestroy(&serv);
	}


/**
	Main test procedure.
*/
static void DoTestsL(TInt aEntryCount)
	{
    // Performance Test Initial Sync
	CAddBench* addTest = CAddBench::NewLC(test);
	addTest->DoSyncL(aEntryCount, KVCardLoc);
	CleanupStack::PopAndDestroy(addTest);
	User::After(10000000);

	CopyFileL(KContactsDBFileNameC, KContactsDBSaveFileNameC);
	
	// Performance Test Startup
	CStartUp* startupTest = CStartUp::NewLC(test);
	CopyFileL(KContactsDBSaveFileNameC, KContactsDBFileNameC);
	// contacts view creation
	TInt ms20FromView(0);
	startupTest->DoTestL(KDbFileName, CStartUp::ERemoteContactsView, ms20FromView);
	TInt microSecsToGet20ItemsFromView(ms20FromView);
	// 'groups only' view creaton
	startupTest->DoTestL(KDbFileName, CStartUp::ERemoteGroupsView, ms20FromView);
	CleanupStack::PopAndDestroy(startupTest);

	User::After(1000000);

	// Performance Test Bulk Delete
	_LIT(KBulkDeleteTitle, "\nBeginning Bulk Delete testing...\n");
	_LIT(KBulkDeleteNote, "[Figures include (fetch 20 items in view) * (num items deleted / transaction size).]\n\n");
	test.Printf(KBulkDeleteTitle);
	test.Printf(KBulkDeleteNote);
	CDeleteMany* deleteMany = NULL;
	// Delete 1 contact
	CopyFileL(KContactsDBSaveFileNameC, KContactsDBFileNameC);
	deleteMany = CDeleteMany::NewLC(test);
	deleteMany->DelDataL(K1Contact, aEntryCount, microSecsToGet20ItemsFromView);
	CleanupStack::PopAndDestroy(deleteMany);
	deleteMany = NULL;
	// Delete 20 contacts
	CopyFileL(KContactsDBSaveFileNameC, KContactsDBFileNameC);
	deleteMany = CDeleteMany::NewLC(test);
	deleteMany->DelDataL(K20Contacts, aEntryCount, microSecsToGet20ItemsFromView);
	CleanupStack::PopAndDestroy(deleteMany);
	deleteMany = NULL;
	// Delete 1000 contacts
	CopyFileL(KContactsDBSaveFileNameC, KContactsDBFileNameC);
	deleteMany = CDeleteMany::NewLC(test);
	deleteMany->DelDataL(K1000Contacts, aEntryCount, microSecsToGet20ItemsFromView);
	CleanupStack::PopAndDestroy(deleteMany);
	deleteMany = NULL;

	// Performance Test Resort View
	CopyFileL(KContactsDBSaveFileNameC, KContactsDBFileNameC);
	CViewResort* resortTest = CViewResort::NewLC(test);
	resortTest->DoTestL(KDbFileName);
	CleanupStack::PopAndDestroy(resortTest);

	// Performance Test Match Phone Number and Update 1 Contact
	CopyFileL(KContactsDBSaveFileNameC, KContactsDBFileNameC);
	CCrudOperations* findAndUpdateTest = CCrudOperations::NewLC(test);
	findAndUpdateTest->DoTestsL(microSecsToGet20ItemsFromView);
	CleanupStack::PopAndDestroy(findAndUpdateTest);
	}

LOCAL_C void CleanupFilesL()
	{
    // delete the database files
	if (CContactDatabase::ContactDatabaseExistsL(KDbFileName) )
		{
		CContactDatabase::DeleteDatabaseL(KDbFileName);
		}
	if (CContactDatabase::ContactDatabaseExistsL(KDbFileSaveName) )
		{
		CContactDatabase::DeleteDatabaseL(KDbFileSaveName);
		}
	}

/*
@SYMTestCaseID PIM-TPERFORMANCE-0001
*/
	
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	
	test.Title();
	test.Start(_L("@SYMTestCaseID PIM-TPERFORMANCE-0001 T_Performance - Performance Tests"));
	
	CActiveScheduler* rootScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(rootScheduler);
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	if (!theCleanup)
		{
		return KErrNoMemory;
		}
		
	TRAPD(ret, DoTestsL(KNumContacts));
	if (ret != KErrNone)
		{
		_LIT(KOutputRes,"Performance Harness T_Performance Returned with %d \n");
		TBuf<64> formattable;
		formattable.Format(KOutputRes, ret);
		test.Printf(formattable);
		}

	// Make sure database files are deleted even if DoTestsL() leaves 
	TRAP_IGNORE(CleanupFilesL());
	
	test(ret==KErrNone);
	
	delete theCleanup;
	delete rootScheduler;
	
	test.End();
	test.Close();
	
	__UHEAP_MARKEND;
	return(ret);
    }
