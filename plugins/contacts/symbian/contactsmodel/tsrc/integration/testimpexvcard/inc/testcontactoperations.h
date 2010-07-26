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
* Performs delete operation on the database
*
*/



 
#ifndef __TESTCONTACTOPERATIONS_H__
#define __TESTCONTACTOPERATIONS_H__

// System includes
#include <cntfilt.h>
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <f32file.h>
#include <cntdb.h>
#include <vcard.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <testexecutestepbase.h>

// User includes
#include "cntvcardutils.h"
#include "testimpexvcardsuiteserver.h"

class CTestContactOperations : public CTestStep
	{
public:
	CTestContactOperations();
	~CTestContactOperations();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void DeleteContactsL(void);

private:
	CActiveScheduler* iScheduler;
	
	};

_LIT(KTestContactOperations,	"TestContactOperations");
_LIT(KNumberOfCont,				"NumberOfContacts");

#endif
