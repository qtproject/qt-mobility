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


#include "teststartupexestep.h"

CTestStartUpExeStep::~CTestStartUpExeStep()
/**
 * Destructor
 */
	{
	}

CTestStartUpExeStep::CTestStartUpExeStep(CTestStartUpServer& aTestServerPtr) 
/**
 * Constructor
 */
	{ 
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestStartUpExeStep);
	iTestServerPtr = &aTestServerPtr;
	}

TVerdict CTestStartUpExeStep::doTestStepPreambleL()
/**Step
 * @return - TVerdict code
 * Override of base class virtual
 */
 	{
 	SetTestStepResult(EPass);	
 	GetStringFromConfig(ConfigSection(),KStartupnameTag,iCntsStarupExeName);
 	GetStringFromConfig(ConfigSection(),KStartupActionTag,iStartupAction); 	 
 	GetIntFromConfig(ConfigSection(),KExitReasonTag,iExitReasonFromIni); 
 	GetStringFromConfig(ConfigSection(),KModeTag,iServerMode);
 	
 	//Fetch the expected exit type for the cnts server process and convert
 	//to the correct type
 	TInt exitTypeFromIni;
 	if (GetIntFromConfig(ConfigSection(),KExitTypeTag,exitTypeFromIni))
 	    {
 		switch (exitTypeFromIni)
 	        {
 	        case (0):
 	        iExitTypeFromIni = EExitKill;
 	        break;
 	    
 	        case (1):
 	        iExitTypeFromIni = EExitTerminate;
 	        break;
 	    
 	        case (2):
 	        iExitTypeFromIni = EExitPanic;
 	        break;
 	    
 	        case (3):
 	        iExitTypeFromIni = EExitPending;
 	        break;
 	    
 	        default:
 	        _LIT(KinvalidExitType,"Invalid Exit Type Read From ini File");
 	        ERR_PRINTF1(KinvalidExitType);
 	        SetTestStepResult(EFail);
 	        }
 	    }    	
 	return TestStepResult(); 	
 	}
 	

TVerdict CTestStartUpExeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class virtual. Implements the main control
 * for the test step. Depending on the action requested the stored
 * TestServer pointer is used to either launch, terminate or check
 * the contacts server.
 */
    {
    SetTestStepResult(EPass);
    
    if (iStartupAction.Compare(KLaunch) == KErrNone)
 	    {
 		TInt err = iTestServerPtr->LaunchCntsStartUpExe(iCntsStarupExeName,iServerMode);
        if(err != KErrNone)
            {
        	SetTestStepResult(EFail);
            }
 	    } 	    
 
 	else if (iStartupAction.Compare(KCheck) == KErrNone)
 	    {
 		TBool res = iTestServerPtr->CheckCntsStartUpExe(iExitTypeFromIni,iExitReasonFromIni);
        if(!res)
            {
        	SetTestStepResult(EFail);
            } 
 	    }
 	    
 	else if (iStartupAction.Compare(KTerminate) == KErrNone)
 	    {
 		iTestServerPtr->TermCntsStartUpExe();
 	    } 	  
 	    
    else
        {
    	//illegal action
    	SetTestStepResult(EFail);
        } 	    
    
 	return TestStepResult();
    };
  
 
 TVerdict CTestStartUpExeStep::doTestStepPostambleL()
 /**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
	return TestStepResult();
	}
