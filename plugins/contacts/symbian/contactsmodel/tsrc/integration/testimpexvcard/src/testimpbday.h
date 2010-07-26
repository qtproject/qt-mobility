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




#if (!defined __TESTIMPBDAY_H__)
#define __TESTIMPBDAY_H__
#include <testexecutestepbase.h>
#include "testimpexvcardsuitestepbase.h"
#include "testimpexvcardsuitedefs.h"

class CTestImpBDay : public CTestImpExvCardSuiteStepBase
	{
public:
	CTestImpBDay();
	~CTestImpBDay();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	RFs iFsSession;
	CActiveScheduler* iScheduler;
	TTime iExpectedBDay;
	};

_LIT(KTestImpBDay,"TestImpBDay");

#endif
