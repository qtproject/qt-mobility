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


#include "testexrevutc.h"

CTestExRevUTC::~CTestExRevUTC()
/**
 * Destructor
 */
	{
	}

CTestExRevUTC::CTestExRevUTC()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestExRevUTC);
	}

TVerdict CTestExRevUTC::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Export of vCards (Rev in UTC) Tests"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}
		
TVerdict CTestExRevUTC::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EFail);
	
	TBuf<90> pathVCF(KExportRevUTCFile);
	ExportItemL(pathVCF, EFalse);
	
	// read from the disk.
	ImportItemL(pathVCF,EFalse);
	TDateTime t = iRecordedTime.DateTime();
	TDateTime t1 = iTimeFromImport.DateTime();
	
	INFO_PRINTF7(_L("Recorded Last Modified Date Year: %d, Month: %d, Day: %d, Recorded Time Hr: %d, Min: %d, Sec: %d "), t.Year(), t.Month(), t.Day(), t.Hour(), t.Minute(), t.Second());
	INFO_PRINTF7(_L("Imported Last Modified Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), t1.Year(), t1.Month(), t1.Day(), t1.Hour(), t1.Minute(), t1.Second());
	
	TTimeIntervalSeconds secondsDifference;
	User::LeaveIfError(iTimeFromImport.SecondsFrom(iRecordedTime, secondsDifference));
	TInt difference = secondsDifference.Int();
	
	if (difference < 2 && difference > -2)
		{
		INFO_PRINTF1(_L("Recorded and Imported DateTime match"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF1(_L("Recorded and Imported DateTime does not match"));
		SetTestStepResult(EFail);
		}
		
	return TestStepResult();
	}

TVerdict CTestExRevUTC::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Export of vCards (Rev in UTC) Tests"));
	return TestStepResult();
	}

