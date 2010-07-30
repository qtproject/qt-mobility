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
* Checks the exported vCard
*
*/



 
#ifndef __TESTCOMPARECNTFILES_H__
#define __TESTCOMPARECNTFILES_H__

// System includes
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <f32file.h>
#include <testexecutestepbase.h>

// User includes
#include "testimpexvcardsuitedefs.h"
#include "contactspbapexportutilityclass.h"

// Constants
const TInt KMaxLengthProperty = 15;
const TChar KLinefeedChar = TChar(0xa);


class CTestCompareCntFiles : public CTestStep
	{
public:
	CTestCompareCntFiles();
	~CTestCompareCntFiles();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void CompareFileL(const TDesC& aExpectedFile, const TDesC& aFile, TDes8& aProperty);
	void CompareLinesL(const TDesC8& aBufExpected, const TDesC8& aBufOutput);
	TBool CheckForNoFieldL(const TDesC& aFile, const TDesC& aProperty);
	TBool IsPropertyPresent(const TDesC8& aLine, TDes8& aProperty);
	void TokenizeAndCompareL();
	void TokenizeAndCheckNoFieldL();
	TBool CompareWholeFileL(const TDesC& aExpectedFile, const TDesC& aFile);
	TBool CompareLine(const TDesC8& aLine1, const TDesC8& aLine2);
	
private:
	CContactsPBAPExportUtilityClass* iExportObj;
	TPtrC iExportFilePath;
	TPtrC iFieldToVerify;
	TPtrC iExpectedExportFilePath;
	RFs iFsSession;
	TPtrC iNoField;
	};

_LIT(KTestCompareCntFiles,					"TestCompareCntFiles");
_LIT(KCompareFile,							"compareFile");
_LIT8(KRevision,							"REV");

_LIT(KExportFile,							"ExportFile");
_LIT(KExpectedExportFile,					"ExpectedExportFile");
_LIT(KFieldToVerify,						"FieldToVerify");
_LIT(KNoField,								"NoField");

#endif
