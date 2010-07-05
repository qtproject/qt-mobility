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


#include "testimpbday.h"

CTestImpBDay::~CTestImpBDay()
/**
 * Destructor
 */
	{
	}

CTestImpBDay::CTestImpBDay()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestImpBDay);
	}

TVerdict CTestImpBDay::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Import of vCards (Birthday present) Tests"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}
			
TVerdict CTestImpBDay::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EFail);
	
	TInt numberOfCases = 0;
	
	while(ETrue)
		{
		TBuf<90> config(KImportBDay);
		TPtrC ptrexpUTC = GetExpectedUTCFromIniL(numberOfCases, config, ETrue);
		if(ptrexpUTC==KNullDesC)
			{
			break;	
			}
			
		INFO_PRINTF2(_L("TEST: %d"), numberOfCases+1);
		iExpectedBDay = FormatDateTime(ptrexpUTC);
		TBuf<90> pathVCF(KPathImportBDay);
		OpenBDAYVCFAndImportItemL(pathVCF, numberOfCases); // Imports vcf 
	
		TDateTime importedDateTime = iBDayFromImport.DateTime();
		TDateTime expectedDateTime = iExpectedBDay.DateTime();
	
		// If birthday does not match, test will fail
		if((importedDateTime.Year() != expectedDateTime.Year()) ||     
		   (importedDateTime.Month() != expectedDateTime.Month()) ||
		   (importedDateTime.Day() != expectedDateTime.Day()) )
			{
			INFO_PRINTF1(_L("Imported Birthday not correct"));
		   	SetTestStepResult(EFail);
		   	return TestStepResult();
			}
		else
			{
			INFO_PRINTF1(_L("Imported Birthday as expected"));
			SetTestStepResult(EPass);
			}
			
		numberOfCases++;
		}
		
	return TestStepResult();
	}

TVerdict CTestImpBDay::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Import of vCards (Birthday present) Tests"));
	return TestStepResult();
	}
