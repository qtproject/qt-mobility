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


#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>
#include "t_utils.h"

_LIT(KTestName,"T_ServerStart");
_LIT(KTest1,"Simultaneous startup");

LOCAL_D RTest test(KTestName);


static void CheckOnlyOneServerLaunched()
	{
	TInt count=0;
	TFullName fullName;
	TFindProcess find(_L("cntsrv*"));
	while(find.Next(fullName)==KErrNone)
		{
		count++;
		}
	__ASSERT_DEBUG(count==1,User::Invariant());
	}


/** Thread function */
static TInt LaunchThread(TAny* /*aAny*/)
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanup=CTrapCleanup::New();

	User::After(1000000);
	CContactDatabase* db =NULL;

	TRAPD(err, db = CContactDatabase::OpenL());
	__ASSERT_ALWAYS(err==KErrNone,User::Invariant());
	CheckOnlyOneServerLaunched();
	delete db;

	delete cleanup;	
	delete CActiveScheduler::Current();
	return KErrNone;
	}


/** Create two threads which simultaneously attempt to open the database */
LOCAL_C void simultaneousStartupL()
	{
	RThread thread1;
	RThread thread2;
	// Create the threads
	User::LeaveIfError(thread1.Create(_L("Thread1"), LaunchThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	User::LeaveIfError(thread2.Create(_L("Thread2"), LaunchThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	// Let them run
	thread1.Resume();
	thread2.Resume();
	User::After(5000000);

	thread1.Close();
	thread2.Close();
	}


/** 
 * Create default contacts file ready for test
 * Run simultaneous startup test
 */

/**

@SYMTestCaseID     PIM-T-SERVERSTART-0001

*/

void doMainL()
	{
	test.Start(_L("@SYMTESTCaseID:PIM-T-SERVERSTART-0001 Simultaneous startup"));


	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());
	CContactDatabase* db = CContactDatabase::CreateL();
	delete db;
	simultaneousStartupL();
	test.End();
	test.Close();
	}


/** Standard E32 entry point */
GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	CActiveScheduler::Install(new(ELeave) CActiveScheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());	
	
	test(ret==KErrNone);
	delete theCleanup;	
	delete CActiveScheduler::Current();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

