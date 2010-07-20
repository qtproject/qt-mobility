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
#ifndef __CONCURRENT_TIMEOUT_NOTIFIER_STEP_H__
#define __CONCURRENT_TIMEOUT_NOTIFIER_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

class CConcurrentTimeOutNotifierStep : public CPerformanceFunctionalityBase 
	{	
class CConcurrentTimeOutNotifierMyActive;
friend class CConcurrentTimeOutNotifierMyActive;		
public:
	CConcurrentTimeOutNotifierStep();
	~CConcurrentTimeOutNotifierStep();
	virtual TVerdict doTestStepL();
	
private:
	void PreTestL();
	void SetTimerL(const TInt aTime);
	void SetContactL(const TInt aCid);
	void StartCountdownL();
	void ProcessWaitL(const TBool aTransaction);
	void LockContactTestL();
	void LockDatabaseTestL();
	
private:
	TInt iNextTest;
	TBool iWaiting;
	TBool iCountdown;
	RArray< void (CConcurrentTimeOutNotifierStep::*)() > *iTests;
	TTime iStart;
	TTime iTestStart;
	TTime iEnd;
	TInt iSeconds;
	CConcurrentTimeOutNotifierStep::CConcurrentTimeOutNotifierMyActive *iMyActive;
	
private:
	class CConcurrentTimeOutNotifierMyActive : public virtual CActive
		{
	public:
		CConcurrentTimeOutNotifierMyActive(CConcurrentTimeOutNotifierStep *aStep) : CActive( EPriorityStandard ),
													iStep ( aStep )
			{}
		void Activate();
	private: // From CActive.
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	private:
		CConcurrentTimeOutNotifierStep *iStep;
		};
	};

_LIT(KConcurrentTimeoutNotifierStep,"ConcurrentTimeoutNotifierStep");

#endif
