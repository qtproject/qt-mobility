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


#include "teststartupserver.h"
#include "testgetsetviewsettingstep.h"
#include "teststartupexestep.h"
#include "testcontactsdbstep.h"

_LIT(KServerName,"TestStartUpSuite");

/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
CTestStartUpServer* CTestStartUpServer::NewL()
	{
	CTestStartUpServer* server = new (ELeave) CTestStartUpServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.

	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


/**
 * Uses the new Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestStartUpServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestStartUpServer::NewL());
	if(KErrNone == err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


/**
 * @return - Standard Epoc error code on exit
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	
	if (err != KErrNone)
	    {
	    // Error	
	    }	
	    
	delete cleanup;
	return KErrNone;
    }
    

/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CTestStartUpServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

    if (aStepName == KTestGetSetViewSettingStep)
        {
	    testStep = new CTestGetSetViewSettingStep();
        }  
    else if (aStepName == KTestStartUpExeStep)
        {
        testStep = new CTestStartUpExeStep(*this);
        }
    else if (aStepName == KTestContactsDbStep)
        {
        testStep = new CTestContactsDbStep();
        }
                
	return testStep;
	}
	

/**
 * @return - err
 * Implements a method to launch the contacts server.
 * The method is called from a test step but the handle to
 * the new process is stores in the CTestStartUpServer so that
 * other test steps can either terminate or retieve information
 * regarding the startup process.
 * @param aExeName
 * @param aServerMode
 */ 
TInt CTestStartUpServer::LaunchCntsStartUpExe(TPtrC aExeName, TPtrC aServerMode)
    {	
	TInt err=iStartUpProcess.Create(aExeName,aServerMode);

	if (err!=KErrNone)
		{
		return err;
		}

	TRequestStatus stat;
	iStartUpProcess.Rendezvous(stat);
	if (stat!=KRequestPending)
		{
		iStartUpProcess.Kill(0);    // abort startup
		}
	else
		{
		iStartUpProcess.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	return (err);
    }


/**
 * Implements a method to terminate the contacts server.
 * The method is called from a test step.
 */
void CTestStartUpServer::TermCntsStartUpExe()
    {
	iStartUpProcess.Terminate(0);
    }   


/**
 * @return  TBool
 * @param aExitTypeFromIni
 * @param aExitReasonFromIni
 * Implements a method to check status of the contacts server.
 * The method is called from a test step. The exit type and
 * exit reason are retieved for the startup exe and compared
 * against expected values read from an ini file.
 */  
TBool CTestStartUpServer::CheckCntsStartUpExe(TExitType aExitTypeFromIni, TInt aExitReasonFromIni)
    {	
	TExitType exitType = iStartUpProcess.ExitType();
	TInt exitReason = iStartUpProcess.ExitReason();
	
	_LIT(KExitTypeFromIni,"Expected Exit Type From ini, ExitTypeValue = %d");
	INFO_PRINTF2(KExitTypeFromIni,aExitTypeFromIni);
		
	_LIT(KExitReasonFromIni,"Expected ExitReason from ini, ExitReasonValue = %d");
	INFO_PRINTF2(KExitReasonFromIni,aExitReasonFromIni);
	
	if (exitType == aExitTypeFromIni && exitReason == aExitReasonFromIni)
	    {
	    _LIT(KReasonOK,"Exit Type and reason for process as expected");
	    INFO_PRINTF1(KReasonOK);
	    _LIT(KExitTypeOk,"Exit Type Returned From Server, ExitType = %d");
	    _LIT(KExitReasonOk,"Exit Reason Returned Frrom Server, ExitReason = %d");
	    INFO_PRINTF2(KExitTypeOk,exitType);
	    INFO_PRINTF2(KExitReasonOk,exitReason);
		return (ETrue);
	    }
	else
	    {
	    _LIT(KReasonNotOK,"Exit Type and reason for process not as expected");
	    ERR_PRINTF1(KReasonNotOK);
	    _LIT(KExitTypeNotOk,"Exit Type Returned From Server, ExitType = %d");
	    _LIT(KExitReasonNotOk,"Exit Reason Returned Frrom Server, ExitReason = %d");
	    ERR_PRINTF2(KExitTypeNotOk,exitType);
	    ERR_PRINTF2(KExitReasonNotOk,exitReason);
		return (EFalse);
	    }
    }


