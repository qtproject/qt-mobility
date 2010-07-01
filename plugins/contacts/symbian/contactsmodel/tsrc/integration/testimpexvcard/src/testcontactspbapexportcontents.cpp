/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 
#include "testcontactspbapexportcontents.h"

CTestContactsPBAPExportContents::~CTestContactsPBAPExportContents()
/**
 * Destructor
 */
	{
	}

CTestContactsPBAPExportContents::CTestContactsPBAPExportContents()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestContactsPBAPExportContents);
	}

TVerdict CTestContactsPBAPExportContents::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Check if the exported file has any content"));

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestContactsPBAPExportContents::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TestContentsL();
	return TestStepResult();
	}

// Tests the contents of a file
void CTestContactsPBAPExportContents::TestContentsL()
	{
	// Tests that when a leave occurs, the exported file doesn't contain any data
	RFs fsSession;
	TInt returnVal;
	RFile file;
	TBuf8<20> buf;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	TFindFile findFile(fsSession);
	fsSession.SetSessionPath(KPath);
	returnVal = findFile.FindByDir(KExportLeaveFile, KPath);
	
	if(returnVal == KErrNone)
		{
		User::LeaveIfError(file.Open(fsSession, KExportLeaveFile, EFileWrite));
		file.Read(buf);
		// test that the exported file has no content
		if(buf.Length() == 0)
			{
			SetTestStepResult(EPass);	
			}
		else
			{
			SetTestStepResult(EFail);	
			}
		}
	else
		{
		SetTestStepResult(EFail);		
		}
		
	file.Close();
	CleanupStack::PopAndDestroy(&fsSession);
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CTestContactsPBAPExportContents::doTestStepPostambleL()
 	{
	return TestStepResult();
	}

