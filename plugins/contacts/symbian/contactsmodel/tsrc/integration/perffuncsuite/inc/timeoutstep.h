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
*
*/




/**
 @file 
 @publishedAll
 @released
*/
#if (!defined __TIMEOUT_STEP_H__)
#define __TIMEOUT_STEP_H__
#include <testexecutestepbase.h>
#include "timeoutbasestep.h"

class CTimeoutStep : public CTimeoutBaseStep
	{
public:
	CTimeoutStep();
	virtual TVerdict doTestStepL();

private:
	void PreTestL()
		{
		}
	TBool ContactLockingTestL(CContactDatabase *aContactsDatabase);
	TBool TransactionLockingTestL(CContactDatabase *aContactsDatabase);
	TBool TransactionInternalLockingTestL();
	TBool TemplateLockingTestL(CContactDatabase *aContactsDatabase);
	void TimeoutValuesTestL(CContactDatabase *aContactsDatabase);
	};

_LIT(KTimeoutStep,"TimeoutStep");

#endif
