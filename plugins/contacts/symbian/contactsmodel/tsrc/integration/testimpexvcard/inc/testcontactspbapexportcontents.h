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
* Checks if the exported file is present with no content
*
*/



 
#ifndef __TESTCONTACTSPBAPEXPORTCONTENTS_H__
#define __TESTCONTACTSPBAPEXPORTCONTENTS_H__

// System includes
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <f32file.h>
#include <testexecutestepbase.h>

// User includes
#include "testimpexvcardsuiteserver.h"

class CTestContactsPBAPExportContents : public CTestStep
	{
public:
	CTestContactsPBAPExportContents();
	~CTestContactsPBAPExportContents();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void TestContentsL();
	
	};

_LIT(KTestContactsPBAPExportContents,	"TestContactsPBAPExportContents");
_LIT(KExportLeaveFile, 					"Expleave.vcf");
_LIT(KPath, 							"\\CITExImvCard\\testdata\\PBAPExport\\");

#endif
