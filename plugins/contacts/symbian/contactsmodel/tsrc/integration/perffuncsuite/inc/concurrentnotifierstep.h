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
#ifndef __CONCURRENT_NOTIFIER_STEP_H__
#define __CONCURRENT_NOTIFIER_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

class CConcurrentNotifierStep : public CPerformanceFunctionalityBase 
	{	
class CConcurrentNotifierActive;
friend class CConcurrentNotifierActive;		
public:
	CConcurrentNotifierStep();
	~CConcurrentNotifierStep();
	virtual TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void PreTestL();
	TInt ReceivedCountL();
	void AddEventL(const TInt aEvent);
	void AddContactL(const TInt aCid);
	void SetMessageCountL();
	void ClearSharedL();
	void EndTransactionL();
	void ProcessWaitL();
	void UpdateTestL();
	void CommitTestL();
	void AddTestL();
	void DeleteTestL();
	void OpenTablesTestL();
	void CloseTablesTestL();
	void OwnCardTestL();
	void ChangeOwnCardTestL();
	void OwnCardDeleteTestL();
	void SpeedDialTestL();
	void RemoveSpeedDialTestL();
	void DeleteSpeedDialTestL();
	void SingleCommitTransactionTestL();
	void MultipleChangeTransactionTestL();
	void EmptyTransactionTestL();
	void UnkownChangeTestL();
	
private:
	TInt iManyListeners;//the number of listener threads
	TInt iNextTest;//index of next consecutive test
	TInt iMessageCount;//number of expected notifications for this test case
	TInt iSpeedPosition;//position of speed dial 1
	TInt iSpeedPosition2;//position of speed dial 2
	TBool iWaiting;//waiting for listener threads to process expected notifications
	TBool iRevert; 	//signifies that transaction will be rolledback/reverted, 
					//otherwised it will be commited, applies only to transaction tests
	TBool iDamage;
	RArray< void (CConcurrentNotifierStep::*)() > *iTests;//list of tests to be run consecutively
	TTime iStart;//time test case started
	TTime iEnd;//current time
	CConcurrentNotifierStep::CConcurrentNotifierActive *iMyActive;
	
private:
	class CConcurrentNotifierActive : public virtual CActive
		{
	public:
		CConcurrentNotifierActive(CConcurrentNotifierStep *aStep);
		void Activate();
	private: // From CActive.
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	private:
		CConcurrentNotifierStep *iStep;
		};
	};

class CConcurrentInitializerStep : public CPerformanceFunctionalityBase
	{
public:
	CConcurrentInitializerStep();
	virtual TVerdict doTestStepL();
private:
	void PreTestL(){}
	};

_LIT(KConcurrentNotifierStep,"ConcurrentNotifierStep");
_LIT(KConcurrentInitilizerStep,"ConcurrentInitilizerStep");

#endif
