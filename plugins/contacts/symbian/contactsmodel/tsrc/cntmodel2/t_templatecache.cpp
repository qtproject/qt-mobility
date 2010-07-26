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

_LIT(KTestName, "T_TemplateCache Test"); // Used by Testhelper - gives an warning under ARMv5


#include "..\..\..\cntserver2\client\inc\rcntmodel.h"
#include <nbcnttestlib.h>
#include <cntdef.h>
#include <cntitem.h>

#include "testhelpers.h"
#include "ccontactprivate.h"

/**

@SYMTestCaseID     PIM-T-TEMPLATECACHE-0001

*/

LOCAL_C void DoTestsL()
	{
    CleanupClosePushL(test);
	test.Start(_L("@SYMTESTCaseID:PIM-T-TEMPLATECACHE-0001 ----------- State Tests ----------"));


   	__UHEAP_MARK;

	RCntModel cntClient; 
	cntClient.ConnectL();
	cntClient.OpenDatabase();
	
	CCntTemplateCache* templateCache = CCntTemplateCache::NewL(cntClient);
	CleanupStack::PushL(templateCache);
	
	CContactIdArray* cardTemplateIds = CContactIdArray::NewLC();
	// Add 15 templates
	CContactTemplate* contactTemplate = NULL;
	TInt ii = 0;
	for (; ii < 15; ++ii)
		{
		contactTemplate = CContactTemplate::NewLC();
		cardTemplateIds->AddL(cntClient.CreateContactL(*contactTemplate));
		CleanupStack::PopAndDestroy(); // contactTemplate
		}
		
	// Create a contactItem
	CCntItemBuilder* cntItemBldr = CCntItemBuilder::NewLC(static_cast<CContactTemplate&>(templateCache->SystemTemplateL()));
	CContactItem* cntItem 		 = cntItemBldr->GetCntItemLC();
	/*TContactItemId cntId  		 = */cntClient.CreateContactL(*cntItem);

	ii = 0;
	for (; ii < 15; ++ii)
		{
		cntItem->SetTemplateRefId((*cardTemplateIds)[ii]);
		templateCache->MergeWithTemplateL(*cntItem);
		// Merge a second time... the template should now be cached
		templateCache->MergeWithTemplateL(*cntItem);
		}
	
	
	CleanupStack::PopAndDestroy(cntItem);
	CleanupStack::PopAndDestroy(cntItemBldr);
	CleanupStack::PopAndDestroy(cardTemplateIds);
	CleanupStack::PopAndDestroy(templateCache);
	cntClient.Close();
	
	__UHEAP_MARKEND;

  	test.Next(_L("---- All Tests Completed OK ----"));


	test.End();
    CleanupStack::PopAndDestroy(1); // test.Close
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

	test(__NB_failures == 0);
		
    // Cleanup
    delete activeScheduler;
 	delete cleanupStack;
	activeScheduler = NULL;
	cleanupStack = NULL;
	
	return err;
    }
