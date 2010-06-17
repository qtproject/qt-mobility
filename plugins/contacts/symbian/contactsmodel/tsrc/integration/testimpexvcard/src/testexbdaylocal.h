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




#if (!defined __TESTEXBDAYLOCAL_H__)
#define __TESTEXBDAYLOCAL_H__
#include <testexecutestepbase.h>
#include "testimpexvcardsuitestepbase.h"
#include "testimpexvcardsuitedefs.h"

class CTestExBDayLocal : public CTestImpExvCardSuiteStepBase
	{
public:
	CTestExBDayLocal();
	~CTestExBDayLocal();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	TBool CheckImportedBDay();
	TPtrC GetBDayL(TInt numberOfCases);
	TPtrC GetNameL(TInt numberOfCases);
	TPtrC GetPhoneL(TInt numberOfCases);
	CActiveScheduler* iScheduler;
	RFs iFsSession;
	TTime iTimeFromImport;
	TBuf<MAX_TEXT_MESSAGE> iName;
	TBuf<MAX_TEXT_MESSAGE> iPhone;
	};

_LIT(KTestExBDayLocal,"TestExBDayLocal");

#endif
