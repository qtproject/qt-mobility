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
#ifndef __CONCURRENT_TIMEOUT_LISTENER_STEP_H__
#define __CONCURRENT_TIMEOUT_LISTENER_STEP_H__
#include <testexecutestepbase.h>
#include "timeoutbasestep.h"

class CConcurrentTimeOutListenerStep : public CTimeoutBaseStep 
	{	
class CConcurrentTimeOutListenerMyActive;
friend class CConcurrentTimeOutListenerMyActive;		
public:
	CConcurrentTimeOutListenerStep();
	~CConcurrentTimeOutListenerStep();
	virtual TVerdict doTestStepL();
	
// Please add/modify your class members here:
private:
	void PreTestL();
	void GetTimerL();
	void GetContactL();
	void RemoteLockL();
	void GetNextTestL();
	TBool OpenContactTestL();
	TBool UpdateContactTestL();
	TBool ReadContactTestL();
	TBool DeleteContactTestL();
	TBool AddContactTestL();
	
private:
	TInt iNextTest;
	RArray< TBool (CConcurrentTimeOutListenerStep::*)() > *iTests;
	TTime iTestStart;
	TContactItemId iCid;
	TInt iSharedTime; 
	TBool iSharedNext;
	CConcurrentTimeOutListenerStep::CConcurrentTimeOutListenerMyActive *iMyActive;
	
private:
	class CConcurrentTimeOutListenerMyActive : public virtual CActive
		{
	public:
		CConcurrentTimeOutListenerMyActive(CConcurrentTimeOutListenerStep *aStep) : CActive( EPriorityStandard ),
													iStep ( aStep )
			{}
		void Activate();
	private: // From CActive.
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	private:
		CConcurrentTimeOutListenerStep *iStep;
		};
	};

_LIT(KConcurrentTimeOutListenerStep,"ConcurrentTimeOutListenerStep");

#endif
