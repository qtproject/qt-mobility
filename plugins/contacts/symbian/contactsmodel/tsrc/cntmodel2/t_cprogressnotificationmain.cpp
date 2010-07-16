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
#include "t_cprogressnotification.h"

LOCAL_C void DoTestsL()
	{

  	__UHEAP_MARK;
	CProgressNotificationTest* cntTest = CProgressNotificationTest::NewLC();

	cntTest->AddContactsL(100);
	
#if defined(_DEBUG)
	//Only valid in udeb build
	cntTest->RunRecoverNotifyTestL();

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	cntTest->RunRecoverNotifyCancelTestL();
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__

#endif //_DEBUG
	
	cntTest->RunCompressNotifyTestL();
	cntTest->RunCompressNotifyCancelTestL();
	cntTest->DeleteContactsL(100);

	CleanupStack::PopAndDestroy(cntTest);
	cntTest	   = NULL;
    __UHEAP_MARKEND;	
	
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
    User::LeaveIfError(err);
    __UHEAP_MARKEND;

		
    // Cleanup
    delete activeScheduler;
 	delete cleanupStack;
	activeScheduler = NULL;
	cleanupStack = NULL;
	
	return err;
    }
