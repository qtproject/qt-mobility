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


#include "testexnobday.h"

CTestExNoBDay::~CTestExNoBDay()
/**
 * Destructor
 */
	{
	}

CTestExNoBDay::CTestExNoBDay()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestExNoBDay);
	}

TVerdict CTestExNoBDay::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Export of vCards (No birthday) Tests"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestExNoBDay::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EFail);
    
    // test codes
	// Export an vCard with the current universal time
    TBuf<90> pathVCF(KExportNoBDayFile);
	ExportItemL(pathVCF, EFalse);
    
    // read from the disk.
    ImportItemL(pathVCF, ETrue);
	
	if(iBDayFromImport == NULL) 
			{
			INFO_PRINTF1(_L("Imported Birthday is NULL (correct), as exported without BDAY"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF1(_L("Imported Birthday is NOT NULL (incorrect)"));
			SetTestStepResult(EFail);
			}	
			
	return TestStepResult();
	}

TVerdict CTestExNoBDay::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Export of vCards (No birthday) Tests"));
	INFO_PRINTF1(KFCITFSIE4);
	return TestStepResult();
	}


