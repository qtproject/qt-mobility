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




#if (!defined __TESTIMPORTCONTACTINSERTMODE_H__)
#define __TESTIMPORTCONTACTINSERTMODE_H__
#include <testexecutestepbase.h>
#include "testimportcontactinsertmodestep.h"
#include "testimpexvcardsuitedefs.h"

class CTestImportContactInsertModeStep : public CTestStep
	{
public:
	CTestImportContactInsertModeStep();
	~CTestImportContactInsertModeStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler* iScheduler;
	RFs iFsSession;
	void ImportFieldsL(TBuf<50> aPath, TInt aStorageType,TUid aContFieldUid, TUid avCardMap);
	void ImportAllFieldsL();
	static TInt iStorage[];
	static TUid iFieldUid[];
	static TUid iVcardUid[];
	};

_LIT(KTestImportContactInsertModeStep,"TestImportContactInsertModeStep");

#endif
