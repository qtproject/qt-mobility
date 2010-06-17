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


#include "testexbdaylocal.h"

CTestExBDayLocal::~CTestExBDayLocal()
/**
 * Destructor
 */
	{
	}

CTestExBDayLocal::CTestExBDayLocal()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestExBDayLocal);
	}

TVerdict CTestExBDayLocal::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Export of vCards (birthday in Local) Tests"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TBool CTestExBDayLocal::CheckImportedBDay()
	{
	TDateTime importedDateTime = iBDayFromImport.DateTime();
	TDateTime exportedDateTime = iBDayLocal.DateTime();
	
	//checks if exported and imported birthday macthes, which it should, otherwise test failed
	if((importedDateTime.Year() != exportedDateTime.Year()) ||
	 (importedDateTime.Month() != exportedDateTime.Month()) ||
	 (importedDateTime.Day() != exportedDateTime.Day()) )
		{
		INFO_PRINTF1(_L("Export and Import of birthday does not match (incorrect)"));
		return EFalse;
		}
	else
		{
		INFO_PRINTF1(_L("Export and Import of birthday macthes (correct)"));
		return ETrue;
		}	
	}
	
TVerdict CTestExBDayLocal::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EFail);
	
	TInt numberOfCases = 0;
	
	while (ETrue)
		{
		TPtrC ptrBDay = GetBDayL(numberOfCases);
		if(ptrBDay==KNullDesC)
			{
			break;	
			}
		
		INFO_PRINTF2(_L("TEST: %d"), numberOfCases+1);
				
		iBDayLocal = FormatDateTime(ptrBDay);
		TDateTime t = iBDayLocal.DateTime();
		INFO_PRINTF7(_L("Birthday to be exported, Year: %d, Month: %d, Day: %d, Hr: %d, Min: %d, Sec: %d"), t.Year(), t.Month(), t.Day(), t.Hour(), t.Minute(), t.Second());
				
		iName = GetNameL(numberOfCases);
		
		iPhone = GetPhoneL(numberOfCases);
		
		TBuf<90> pathVCF(KExportBDayFile);
		ExportItemL(pathVCF, ETrue);
		// read from the disk.
		ImportItemL(pathVCF, ETrue);
		
		if(!CheckImportedBDay())
			{
			SetTestStepResult(EFail);
			return TestStepResult();
			}
			
		numberOfCases++;
		}// End Of While Loop
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestExBDayLocal::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Export of vCards (birthday in Local) Tests"));
	return TestStepResult();
	}

TPtrC CTestExBDayLocal::GetBDayL(TInt numberOfCases)
	{
	TPtrC bDay;
	
	TPtrC config = KVCard();
	HBufC *configval = HBufC::NewLC(config.Length()+2);
	
	TPtr configSection = configval->Des();
	
	configSection.Append(config);
	configSection.AppendNum(numberOfCases);
	
	GetStringFromConfig(configSection,KBDayLocal,bDay);
	
	CleanupStack::PopAndDestroy(configval);
	
	return bDay;
	}
	
TPtrC CTestExBDayLocal::GetNameL(TInt numberOfCases)
	{
	TPtrC name;
	
	TPtrC config = KVCard();
	HBufC *configval = HBufC::NewLC(config.Length()+2);
	
	TPtr configSection = configval->Des();
	
	configSection.Append(config);
	configSection.AppendNum(numberOfCases);
	
	GetStringFromConfig(configSection,KName,name);
	
	CleanupStack::PopAndDestroy(configval);
	
	return name;
	}
	
TPtrC CTestExBDayLocal::GetPhoneL(TInt numberOfCases)
	{
	TPtrC phone;
	
	TPtrC config = KVCard();
	HBufC *configval = HBufC::NewLC(config.Length()+2);
	
	TPtr configSection = configval->Des();
	
	configSection.Append(config);
	configSection.AppendNum(numberOfCases);
	
	GetStringFromConfig(configSection,KPhone,phone);
	
	CleanupStack::PopAndDestroy(configval);
	
	return phone;
	}

