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




#if (!defined __TESTIMPNOREV_H__)
#define __TESTIMPNOREV_H__
#include <test/testexecutestepbase.h>
#include "testimpexvcardsuitestepbase.h"
#include "testimpexvcardsuitedefs.h"

class CTestImpNoRev : public CTestImpExvCardSuiteStepBase
	{
public:
	CTestImpNoRev();
	~CTestImpNoRev();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	RFs iFsSession;
	CActiveScheduler* iScheduler;
	};

_LIT(KTestImpNoRev,"TestImpNoRev");

#endif
