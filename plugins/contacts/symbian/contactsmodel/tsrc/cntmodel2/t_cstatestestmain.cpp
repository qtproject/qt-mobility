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


#include <e32test.h>
#include "t_cstatestest.h"

LOCAL_C void DoTestsL()
	{

	COpenCommitTest* cntTest = COpenCommitTest::NewLC();
   	
  	__UHEAP_MARK;	
	cntTest->RunOpenCommitTestL();
	cntTest->RunGroupTestL();
    __UHEAP_MARKEND;
	
	CleanupStack::PopAndDestroy(cntTest);
	cntTest	   = NULL;


	}




GLDEF_C TInt E32Main()
	{
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
  	if (!cleanupStack)
      return KErrNoMemory;
        
    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        return KErrNoMemory;
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    __UHEAP_MARK;
    TRAPD(err, DoTestsL());
    __UHEAP_MARKEND;

		
    // Cleanup
    delete activeScheduler;
 	delete cleanupStack;
	activeScheduler = NULL;
	cleanupStack = NULL;
	
	return err;
    }
