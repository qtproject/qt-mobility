/*
* Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
* Contact: http://www.qt-project.org/legal
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




#if (!defined __TESTEXNOBDAY_H__)
#define __TESTEXNOBDAY_H__
#include <test/testexecutestepbase.h>
#include "testimpexvcardsuitestepbase.h"
#include "testimpexvcardsuitedefs.h"

class CTestExNoBDay : public CTestImpExvCardSuiteStepBase
	{
public:
	CTestExNoBDay();
	~CTestExNoBDay();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler* iScheduler;
	};

_LIT(KTestExNoBDay,"TestExNoBDay");

_LIT (KFCITFSIE4, "Finished Executing Integration Tests FS App-Engines/Cntmodel IE.4");

#endif
