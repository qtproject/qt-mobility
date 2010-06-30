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


#include "testimpnorev.h"

CTestImpNoRev::~CTestImpNoRev()
/**
 * Destructor
 */
	{
	}

CTestImpNoRev::CTestImpNoRev()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestImpNoRev);
	}

TVerdict CTestImpNoRev::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Import of vCards (No REV) Tests"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestImpNoRev::doTestStepL()
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
		TBuf<90> pathVCF(KPathImportNoREv);
		OpenVCFAndImportItemL(pathVCF, iFsSession, i); // Imports vcf 

		if(iTimeFromImport == NULL) 
			{
			INFO_PRINTF1(_L("Imported Time is NULL (correct)"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF1(_L("Imported Time is NOT NULL (incorrect)"));
			SetTestStepResult(EFail);
			return TestStepResult();
			}	
		}
	
	return TestStepResult();
	}

TVerdict CTestImpNoRev::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Import of vCards (No REV) Tests"));
	return TestStepResult();
	}
