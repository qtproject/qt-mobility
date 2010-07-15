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
*
*/


/**
 @file 
 @publishedAll
 @released
*/

#include "performancefunctionalityserver.h"
#include "requestmixstep.h"
#include "transactionsstep.h"
#include "lowdiskspacestep.h"
#include "concurrentnotifierstep.h"
#include "concurrentlistenerstep.h"
#include "crudoomstep.h"
#include "timeoutstep.h"
#include "concurrenttimeoutnotifierstep.h"
#include "concurrenttimeoutlistenerstep.h"
#include "randomblobstep.h"
#include "notificationstep.h"

#include "compactandrecoverstep.h"

#include "statemachinestep.h"

_LIT(KServerName,"Te_PerformanceFunctionalitySuite");
CPerformanceFunctionalityTestsSuite* CPerformanceFunctionalityTestsSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CPerformanceFunctionalityTestsSuite * server = new (ELeave) CPerformanceFunctionalityTestsSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.
	server->iFs = new(ELeave) RFs();
	//server->StartL(KServerName);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CPerformanceFunctionalityTestsSuite::~CPerformanceFunctionalityTestsSuite()
	{
	delete iFs;
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CPerformanceFunctionalityTestsSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CPerformanceFunctionalityTestsSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
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
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CPerformanceFunctionalityTestsSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
	if(aStepName == KRequestMixStep)
		{
		testStep = new CRequestMixStep();
		}
	else if(aStepName == KTransactionsStep)
		{
		testStep = new CTransactionsStep( *this );
		}
	else if(aStepName == KLowDiskSpaceStep)
		{
		testStep = new CLowDiskSpaceStep( *this );
		}
	else if(aStepName == KConcurrentNotifierStep)
		{
		testStep = new CConcurrentNotifierStep( );
		}
	else if(aStepName == KConcurrentListenerStep)
		{
		testStep = new CConcurrentListenerStep( );
		}
	else if(aStepName == KConcurrentInitilizerStep)
		{
		testStep = new CConcurrentInitializerStep( );
		}
	else if(aStepName == KCRUDOOMStep)
		{
		testStep = new CCRUDOOMStep();
		}
	else if(aStepName == KTimeoutStep)
		{
		testStep = new CTimeoutStep();
		}
	else if(aStepName == KConcurrentTimeoutNotifierStep)
		{
		testStep = new CConcurrentTimeOutNotifierStep();
		}
	else if(aStepName == KConcurrentTimeOutListenerStep)
		{
		testStep = new CConcurrentTimeOutListenerStep();
		}
	else if(aStepName == KRandomBlobStep)
		{
		testStep = new CRandomBlobStep();
		}
	else if(aStepName == KNotificationStep)
		{
		testStep = new CNotificationStep();
		}
	else if(aStepName == KCompactAndRecoverStep)
		{
		testStep = new CCompactAndRecoverStep();
		}
	else if(aStepName == KStateMachineStep)
		{
		testStep = new CStateMachineStep();
		}
	else
	    {
    	_LIT(KInvalid,"Invalid Test Step");
    	User::Panic(KInvalid, 2204);
	    }

	if (!testStep)
		{
		_LIT(KAllocationFailed, 
			"Could not allocate teststep in CPerformanceFunctionalityTestsSuite::CreateTestStep()\n");
		ERR_PRINTF1(KAllocationFailed);
		User::Invariant();
		}
	
	return testStep;
	}
