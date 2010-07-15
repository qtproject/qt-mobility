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




#if (!defined __TESTEXPORTCONTACT_H__)
#define __TESTEXPORTCONTACT_H__
#include <testexecutestepbase.h>
#include "testexportcontactstep.h"
#include "testimpexvcardsuitedefs.h"

class CTestExportContactStep : public CTestStep
	{
public:
	CTestExportContactStep();
	~CTestExportContactStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler* iScheduler;
	RFs iFsSession;
	void ExportFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid avCardMap, TBuf<80> aPath);
	void ExportAllFieldsL(void);
	static TInt iStorage[];
	static TUid iFieldUid[];
	static TUid iVcardUid[];
	};

_LIT(KTestExportContactStep,"TestExportContactStep");

#endif
