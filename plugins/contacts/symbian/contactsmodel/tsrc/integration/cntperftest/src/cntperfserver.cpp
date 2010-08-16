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

#include "cntperfserver.h"
#include "clientserver.h"
#include "customlabeluid.h"
#include "unifiedsingular.h"
#include "viewdefallfieldstest.h"
#include "simpleviewdeftests.h"
#include "viewdefanyfieldstest.h"
#include "viewdefcustomfieldstest.h"
#include "customtemplatetest.h"
#include "accesscounttests.h"
#include "contactviewtest.h"
#include "ccontactvieweventqueue.h"
#include "contactutilitiescollection.h"
#include "testcontactviewaccessstep.h"
#include "preparedatateststep.h"
#include "testcontactviewunderlyingviewupdatestep.h"
#include "testcontactviewcrudoperationsstep.h"



CCntPerfServer* CCntPerfServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CCntPerfServer * server = new (ELeave) CCntPerfServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.
	TParsePtrC serverName(RProcess().FileName());
	server->ConstructL( serverName.Name() );
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CCntPerfServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CCntPerfServer::NewL());
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
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	return err;
    }

CTestStep* CCntPerfServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if(aStepName == SharedConstants::KCustomLabelUid)
		{
		testStep = new CCustomLabelUid();
		}
	else if(aStepName == SharedConstants::KUnifiedSingular)
		{
		testStep = new CUnifiedSingular();
		}
	else if(aStepName == SharedConstants::KViewDefAllFieldsTest)
		{
		testStep = new CViewDefAllFieldsTest();
		}
	else if(aStepName == SharedConstants::KSimpleViewDefTest)
		{
		testStep = new CSimpleViewDefTest();
		}
	else if(aStepName == SharedConstants::KViewDefAnyFieldsTest)
		{
		testStep = new CViewDefAnyFieldsTest();
		}
	else if(aStepName == SharedConstants::KViewDefCustomFieldsTest)
		{
		testStep = new CViewDefCustomFieldsTest();
		}
	else if(aStepName == SharedConstants::KCustomTemplateTest)
		{
		testStep = new CCustomTemplateTest();
		}
	else if(aStepName == SharedConstants::KAccessCountTests)
		{
		testStep = new CAccessCountTests( *this );
		}
	else if(aStepName == SharedConstants::KContactViewTest)
		{
		testStep = new CContactViewTest(*this);
		}	
	else if(aStepName == KTestContactViewAccessStep)
		{
		testStep = new CTestContactViewAccessStep();
		}
	else if(aStepName == KPrepareDataTestStep)
		{
		testStep = new CPrepareDataTestStep();
		}
	else if(aStepName == KTestContactViewUnderLyingViewUpdateStep)
		{
		testStep = new CTestContactViewUnderlyingViewUpdateStep();
		}	
	else if(aStepName == KTestContactViewCRUDOperationsStep)
		{
		testStep = new CTestContactViewCRUDOperationsStep();
		}
	else
	    {
    	_LIT(KInvalid,"Invalid Test Step");
    	User::Panic(KInvalid, 2204);
	    }

	return testStep;
	}

