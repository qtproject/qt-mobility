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

//  Include Files  
#include <e32base.h>
#include <e32std.h>
#include <e32test.h>	// rtest
#include <bacline.h> 	// ccommandlinearguments
#include "fuzztest.h"
#include "ipcfuzztest.h"

//  Constants

_LIT(KTextTestTitle, "FuzzTest");
const TInt KMinIPCFuncRange = 0;
const TInt KMaxIPCFuncRange = 300;

//  Global Variables

LOCAL_D RTest gTest(KTextTestTitle());  // write all messages to this


//  Local Functions

LOCAL_C void DoTestL()
	{
	//
	// Checking command line arguements and create fuzz test instance.
	//
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	
	TInt minRange = KMinIPCFuncRange;
	TInt maxRange = KMaxIPCFuncRange;
	
	const TInt argsCount = args->Count();
	if(argsCount == 1)
		{
		gTest.Printf(_L("Please give target server name and IPC funtion ranges"));
		User::Leave(KErrArgument);
		}
	
	TPtrC targetNamePtr = args->Arg(1);
	if(argsCount == 3)
		{
		TLex maxRangeLex(args->Arg(2));
		maxRangeLex.Val(maxRange);
		}
	else if(argsCount > 3)
		{
		TLex minRangeLex(args->Arg(2));
		TLex maxRangeLex(args->Arg(3));
		maxRangeLex.Val(maxRange);
		minRangeLex.Val(minRange);
		}
	
	if(minRange > maxRange) 
		{
		TInt temp = minRange;
		minRange = maxRange;
		maxRange = temp;
		}
	
	RIpcFuzzTest ipcFuzzTest(gTest);
	
	ipcFuzzTest.RunTestL(targetNamePtr, minRange, maxRange);
	CleanupStack::PopAndDestroy(args);
	}


//  Global Functions
/**

@SYMTestCaseID     PIM-FUZZTEST-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	gtest.Start(_L("@SYMTESTCaseID:PIM-FUZZTEST-0001 FuzzTest"));

	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,DoTestL());
			gTest(err == KErrNone);
			delete cleanup;
			}
		delete scheduler;
		}

	gTest.Close();
	__UHEAP_MARKEND;
	return KErrNone;
	}

