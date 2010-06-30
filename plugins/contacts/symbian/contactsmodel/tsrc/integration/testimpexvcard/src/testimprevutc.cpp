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


#include "testimprevutc.h"

CTestImpRevUTC::~CTestImpRevUTC()
/**
 * Destructor
 */
	{
	}

CTestImpRevUTC::CTestImpRevUTC()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestImpRevUTC);
	}

TVerdict CTestImpRevUTC::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Import of vCards (REV in UTC) Tests"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestImpRevUTC::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EFail);
	
	TInt numberOfCases = 0;
	
	while(ETrue)
		{
		TBuf<90> config(KImportRevUTC);
		TPtrC ptrexpUTC = GetExpectedUTCFromIniL(numberOfCases, config, EFalse);
		if(ptrexpUTC==KNullDesC)
			{
			break;	
			}
			
		INFO_PRINTF2(_L("TEST: %d"), numberOfCases+1);
		iExpectedUTC = FormatDateTime(ptrexpUTC);
		TBuf<90> pathVCF(KPathImportRevUTC);
		OpenVCFAndImportItemL(pathVCF, iFsSession, numberOfCases); // Imports vcf 
	
		TDateTime t = iTimeFromImport.DateTime();
		TDateTime t1 = iExpectedUTC.DateTime();
		INFO_PRINTF7(_L("Imported Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), t.Year(), t.Month(), t.Day(), t.Hour(), t.Minute(), t.Second());
		INFO_PRINTF7(_L("Expected Date Year: %d, Month: %d, Day: %d, Expected Time Hr: %d, Min: %d, Sec: %d "), t1.Year(), t1.Month(), t1.Day(), t1.Hour(), t1.Minute(), t1.Second());
	
		if (iExpectedUTC==iTimeFromImport) // checks if imported time is correct
			{
			INFO_PRINTF1(_L("Imported Time as UTC (correct)"));
			SetTestStepResult(EPass);	
			}
		else
			{
			INFO_PRINTF1(_L("Imported Time not imported as UTC (NOT CORRECT)"));
			SetTestStepResult(EFail);
			return TestStepResult();
			}
			
		numberOfCases++;
		}
	
	return TestStepResult();
	}

TVerdict CTestImpRevUTC::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Import of vCards (REV in UTC) Tests"));
	return TestStepResult();
	}
	
