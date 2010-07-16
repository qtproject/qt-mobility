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


#include "testimprevlocaltz.h"

CTestImpRevLocalTZ::~CTestImpRevLocalTZ()
/**
 * Destructor
 */
	{
	}

CTestImpRevLocalTZ::CTestImpRevLocalTZ()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestImpRevLocalTZ);
	}

TVerdict CTestImpRevLocalTZ::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(KCITFSIE4);
	INFO_PRINTF1(_L("Start Import of vCards (REV with TZ property) Tests"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestImpRevLocalTZ::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
    SetTestStepResult(EFail);	       
	
	TInt numberOfCases = 0;

	while(ETrue)
		{
		TBuf<90> config(KImportRevLocalTZ);
		TPtrC ptrexpUTC = GetExpectedUTCFromIniL(numberOfCases, config, EFalse);
		if(ptrexpUTC==KNullDesC)
			{
			break;	
			}
			
		INFO_PRINTF2(_L("TEST: %d"), numberOfCases+1);
		iExpectedUTC = FormatDateTime(ptrexpUTC);
		TBuf<90> pathVCF(KPathImportRevLocalTZ);
		OpenVCFAndImportItemL(pathVCF, iFsSession, numberOfCases); // Imports vcf 

		TDateTime t = iTimeFromImport.DateTime();
		TDateTime t1 = iExpectedUTC.DateTime();
		INFO_PRINTF7(_L("Imported Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), t.Year(), t.Month(), t.Day(), t.Hour(), t.Minute(), t.Second());
		INFO_PRINTF7(_L("Expected Date Year: %d, Month: %d, Day: %d, Expected Time Hr: %d, Min: %d, Sec: %d "), t1.Year(), t1.Month(), t1.Day(), t1.Hour(), t1.Minute(), t1.Second());
	
		if (iExpectedUTC==iTimeFromImport) // checks if imported time is correct
			{
			INFO_PRINTF1(_L("Imported Time is correct"));
			SetTestStepResult(EPass);	
			}
		else
			{
			INFO_PRINTF1(_L("Imported Time is NOT correct"));
			SetTestStepResult(EFail);
			return TestStepResult();
			}
			
		numberOfCases++;
		}

	return TestStepResult();
	}

TVerdict CTestImpRevLocalTZ::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Import of vCards (REV with TZ property) Tests")); 
	return TestStepResult();
	}


