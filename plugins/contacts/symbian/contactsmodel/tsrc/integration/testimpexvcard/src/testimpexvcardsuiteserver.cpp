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


#include "testimpexvcardsuiteserver.h"
#include "testimprevlocaltz.h"
#include "testimprevlocal.h"
#include "testimpnorev.h"
#include "testimprevutc.h"
#include "testimpbday.h"
#include "testimpnobday.h"
#include "testimpoom.h"
#include "testexrevutc.h"
#include "testexbdaylocal.h"
#include "testexnobday.h"
#include "testimpexvcardsuitedefs.h"
#include "testexportcontactstep.h"
#include "testimportcontactinsertmodestep.h"
#include "testimportcontactmergemodestep.h"
#include "testcontactspbapexport.h"
#include "testcomparecntfiles.h"
#include "testcontactoperations.h"
#include "testcontactspbapexportperformance.h"
#include "testcontactspbapexportcontents.h"
#include "testgenericimportstep.h"

_LIT(KServerName,"TestImpExvCardSuite");
CTestImpExvCardSuite* CTestImpExvCardSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestImpExvCardSuite * server = new (ELeave) CTestImpExvCardSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestImpExvCardSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestImpExvCardSuite::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}

	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CTestImpExvCardSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below
              if(aStepName == KTestImpRevLocalTZ)
              	{
              	testStep = new CTestImpRevLocalTZ();
              	}
              else if (aStepName == KTestImpRevLocal)
	              	   {
              		   testStep = new CTestImpRevLocal();
              		   }
              else if (aStepName == KTestImpNoRev)
	              	   {
              		   testStep = new CTestImpNoRev();
              		   }
              else if (aStepName == KTestImpRevUTC)
	              	   {
              		   testStep = new CTestImpRevUTC();
              		   }
              else if (aStepName == KTestImpBDay)
	              	   {
              		   testStep = new CTestImpBDay();
              		   }
              else if (aStepName == KTestImpNoBDay)
	              	   {
              		   testStep = new CTestImpNoBDay();
              		   }
              else if (aStepName == KTestExRevUTC)
	              	   {
              		   testStep = new CTestExRevUTC();
              		   }
              else if (aStepName == KTestExBDayLocal)
	              	   {
              		   testStep = new CTestExBDayLocal();
              		   }
              else if (aStepName == KTestExNoBDay)
	              	   {
              		   testStep = new CTestExNoBDay();
              		   }
              else if (aStepName == KTestExportContactStep)
	              	   {
              		   testStep = new CTestExportContactStep();
              		   }
              else if (aStepName == KTestImportContactInsertModeStep)
	              	   {
              		   testStep = new CTestImportContactInsertModeStep();
              		   }
              else if (aStepName == KTestImportContactMergeModeStep)
	              	   {
              		   testStep = new CTestImportContactMergeModeStep();
              		   }
              else if (aStepName == KTestImpOOM)
	              	   {
              		   testStep = new CTestImpOOM();
              		   }
              else if (aStepName == KTestGenericImportStep)
	              	   {
              		   testStep = new CTestGenericImportStep();
              		   }
              else if (aStepName == KTestContactsPBAPExport)
	              	   {
              		   testStep = new CTestContactsPBAPExport();
              		   }
              else if (aStepName == KTestCompareCntFiles)
	              	   {
              		   testStep = new CTestCompareCntFiles();
              		   }
              else if (aStepName == KTestContactOperations)
	              	   {
              		   testStep = new CTestContactOperations();
              		   }
              else if (aStepName == KTestContactsPBAPExportContents)
	              	   {
              		   testStep = new CTestContactsPBAPExportContents();
              		   }
              else if (aStepName == KTestContactsPBAPExportPerformance)
	              	   {
              		   testStep = new CTestContactsPBAPExportPerformance();
              		   }
	return testStep;
	}
