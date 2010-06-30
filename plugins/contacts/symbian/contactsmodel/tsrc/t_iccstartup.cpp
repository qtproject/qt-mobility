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
* This test harness simulates aspects of the phbksync test plugin behaviour which
* starts up the phbksync server if it is not running. The phbksync server configures itself
* and needs to access the contacts DB. Accessing the contacts DB from within the contacts
* server can cause deadlocks. The contacts client will load the test plugin and try to access
* the contacts DB on first use only, this will avoid the problem of the server being deadlocked.
* **This test only works with Contacts the test plugin and not the original phonebooksync plugin.**
*/


#include <e32test.h>
#include "t_utils.h"
#include "./testsyncplugin/cnttestsynchroniser.h"

_LIT(KICCStartupTest, "ICC Test plugin startup test");
_LIT(KPhoneGroupId, "PhoneGroupId test");
_LIT(KICCTemplateId, "ICCTemplateId test");
_LIT(KIccStartupDB, "c:IccStartupDB");

RTest test(KICCStartupTest);
CCntTest* CntTestPtr;
CContactSyncChecker* syncChecker;

void DoPhoneGroupIdTest()
    {
    /*
	get contacts database and get the contacts phone group id.
	This should load the test plugin, which should make a call back
	into the contacts DB
	*/
	CntTestPtr->CreateDatabaseL();	
	CntTestPtr->OpenDatabaseL();
	
	
	CContactDatabase& Db = *CntTestPtr->Db();
	
	Db.PhonebookGroupIdL(); 
	
	test (syncChecker->PluginLoadedCountL() == 2); //one for the client & one for server
	
	CntTestPtr->CloseDatabase();
	CntTestPtr->DeleteDatabaseL(); //close & delete database at end of test
    }

void DoICCTemplateIdTest()
    {
    /*
	get contacts database and get the contacts ICCTemplate id.
	This should load the test plugin, which should make a call back
	into the contacts DB
	*/
    CntTestPtr->CreateDatabaseL();	
	CntTestPtr->OpenDatabaseL();
	
	
	CContactDatabase& Db = *CntTestPtr->Db();
	
	Db.ICCTemplateIdL(); 
	
	test (syncChecker->PluginLoadedCountL() == 2); //one for the client & one for server
	
	CntTestPtr->CloseDatabase();
	CntTestPtr->DeleteDatabaseL(); //close & delete database at end of test
	
    }

/**

@SYMTestCaseID     PIM-T-ICCSTARTUP-0001

*/
 
void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-ICCSTARTUP-0001 ICC Test plugin startup test"));

	syncChecker = new (ELeave) CContactSyncChecker;
	syncChecker->ResetEverythingL(); //set the counters to zero
	
	test.Next(_L("PhoneGroupId test"));

	DoPhoneGroupIdTest();
	
	
	syncChecker->ResetEverythingL(); //set the counters to zero
	test.Next(_L("ICCTemplateId test"));

	DoICCTemplateIdTest();
	
	
	delete syncChecker;
    syncChecker = NULL;
	
    }


/** Standard E32Main method */

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
		
	CntTestPtr=new(ELeave) CCntTest;
	
	CntTestPtr->ConstructL(test,KIccStartupDB); //KIccStartupDB defined in cnttestsynchroniser.h
	
    TRAPD(err,DoTestsL()); 
    
	CntTestPtr->EndTestLib(err); // is responsible for deleting the memory for itself 
	
	__UHEAP_MARKEND;
	return KErrNone;
    }
