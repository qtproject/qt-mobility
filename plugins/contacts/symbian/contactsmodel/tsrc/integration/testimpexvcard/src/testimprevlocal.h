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




#if (!defined __TESTIMPREVLOCAL_H__)
#define __TESTIMPREVLOCAL_H__
#include <test/testexecutestepbase.h>
#include "testimpexvcardsuitestepbase.h"
#include "testimpexvcardsuitedefs.h"

class CTestImpRevLocal : public CTestImpExvCardSuiteStepBase
	{
public:
	CTestImpRevLocal();
	~CTestImpRevLocal();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler* iScheduler;
	RFs iFsSession;
	TTime iExpectedUTC;
	};

_LIT(KTestImpRevLocal,"TestImpRevLocal");

#endif
