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
* Generic import step to import the contact items into the database
*
*/



 
#ifndef TESTGENERICIMPORT_H
#define TESTGENERICIMPORT_H

// System includes
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <f32file.h>
#include <testexecutestepbase.h>
#include <cntdb.h>
#include <vcard.h>
#include <cntitem.h>
#include <cntfldst.h>

// User includes
#include "cntvcardutils.h"
#include "testimpexvcardsuitedefs.h"

class CTestGenericImportStep : public CTestStep
	{
public:
	CTestGenericImportStep();
	~CTestGenericImportStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void ImportContactsL(void);
	void GetInputFromIni();
	void ImportWithPBAP(CContactDatabase* aDBase, RFileReadStream& aReadStream, TBool& aSuccess);

private:
	CActiveScheduler* iScheduler;
	TPtrC iOOM;
	TPtrC iImportFile;
	TPtrC iDatabaseFile;
	TInt  iNumberOfContacts;
	TBool iEnableImportPBAP;
	};

_LIT(KTestGenericImportStep,	"TestGenericImportStep");
_LIT(KImportFile, 				"importfile");
_LIT(KDatabaseFile, 			"databasefile");
_LIT(KNumberOfContacts,			"NumberOfContacts");
_LIT(KEnableImportPBAP,			"ImportPBAP");

#endif
