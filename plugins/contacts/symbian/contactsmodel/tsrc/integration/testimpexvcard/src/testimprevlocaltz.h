/*
* Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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




#if (!defined __TESTIMPREVLOCALTZ_H__)
#define __TESTIMPREVLOCALTZ_H__
#include <test/testexecutestepbase.h>
#include "testimpexvcardsuitestepbase.h"
#include "testimpexvcardsuitedefs.h"

class CTestImpRevLocalTZ : public CTestImpExvCardSuiteStepBase
	{
public:
	CTestImpRevLocalTZ();
	~CTestImpRevLocalTZ();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler* iScheduler;
	RFs iFsSession;
	TTime iExpectedUTC;
	};

_LIT(KTestImpRevLocalTZ,"TestImpRevLocalTZ");

_LIT (KCITFSIE4, "Executing Integration Tests FS App-Engines/Cntmodel IE.4");

#endif
