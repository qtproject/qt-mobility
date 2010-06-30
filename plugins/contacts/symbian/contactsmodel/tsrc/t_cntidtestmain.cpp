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


#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>

#include "t_cntidtest.h"


//--------------------- LOCAL DEFINITIONS ----------------------------------
_LIT(KCntTest,"===== Cntmodel contact ID Test =====\n");
_LIT(KCntTestOK,"ok\n");
_LIT(KCntTestError,"Error: %d");
_LIT(KCntTestDone," [Done]\n");
_LIT(KTestName,"t_CntIdTest");

RTest test(KTestName);

//---------------------- LOCAL FUNTIONS ------------------------------------
LOCAL_C void InitExampleL();
LOCAL_C void DoExampleL();

//--------------------------------------------------------------------------
// E32Main
//--------------------------------------------------------------------------
//
/**

@SYMTestCaseID     PIM-T-CNTIDTESTMAIN-0001

*/

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-CNTIDTESTMAIN-0001 Testing contact IDs"));

	
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(error,InitExampleL());
	__ASSERT_ALWAYS(!error,User::Panic(KCntTest,error));
	

	delete cleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }

//--------------------------------------------------------------------------
// InitExampleL
//--------------------------------------------------------------------------
//
LOCAL_C void InitExampleL()
    {
    // Create and install active scheduler
    CActiveScheduler* acticeScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL( acticeScheduler );
    CActiveScheduler::Install(acticeScheduler);    
    
	TRAPD(error,DoExampleL()); // perform example function
	if (error)
		{
		test.Printf(KCntTestError, error);
		User::Leave(error);
		}
	else
		{
		test.Printf(KCntTestOK);	
		}
	test.Printf(KCntTestDone);
	CleanupStack::PopAndDestroy(); // activescheduler
    }

//--------------------------------------------------------------------------
// DoExampleL
//--------------------------------------------------------------------------
//
LOCAL_C void DoExampleL()
    {
	
	CCntTest* cntTest = CCntTest::NewL();
	cntTest->RunTest();
	CActiveScheduler::Start();
	delete cntTest; cntTest = NULL;
	}
