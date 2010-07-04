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


#include "testimpnobday.h"

CTestImpNoBDay::~CTestImpNoBDay()
/**
 * Destructor
 */
	{
	}

CTestImpNoBDay::CTestImpNoBDay()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestImpNoBDay);
	}

TVerdict CTestImpNoBDay::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Import of vCards (BDay not present) Tests"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestImpNoBDay::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EFail);
		
	for (TInt i =0; i<2; i++)
		{
		INFO_PRINTF2(_L("TEST: %d"), i+1);
		// test codes
		TBuf<90> pathVCF(KPathImportNoBDay);
		OpenBDAYVCFAndImportItemL(pathVCF, i); // Imports vcf 

		// checks if Imported Birthday is NULL
		if(iBDayFromImport == NULL) 
			{
			INFO_PRINTF1(_L("Imported Birthday is NULL (correct)"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF1(_L("Imported Birthday is NOT NULL (incorrect)"));
			SetTestStepResult(EFail);
			return TestStepResult();
			}	
		}
	
	return TestStepResult();
	}

TVerdict CTestImpNoBDay::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Import of vCards (BDay not present) Tests"));
	return TestStepResult();
	}

