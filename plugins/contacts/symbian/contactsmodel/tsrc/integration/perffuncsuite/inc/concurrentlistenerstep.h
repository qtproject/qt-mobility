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
#ifndef __CONCURRENT_LISTENER_STEP_H__
#define __CONCURRENT_LISTENER_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

class CConcurrentListenerStep : public CPerformanceFunctionalityBase
	{	
class CConcurrentListenerActive;
friend class CConcurrentListenerActive;		
public:
	CConcurrentListenerStep();
	~CConcurrentListenerStep();
	virtual TVerdict doTestStepL();

private:
	void PreTestL();
	void InitializeL();
	void Cleanup();
	void NotifyReceivedL();
	void GetEventsL();
	void GetContactsL();
	void GetNextTestL();
	void GetSharedArrayL(const TDesC &aShare, RArray<TInt> &aArray, RArray<TInt> &aDBArray);
	TBool CompareNotifications();
	void ResetArrays();
	void GetMessageCountL();
	
private: 
	TTime 	iStart;//store test start time
	TTime 	iEnd;//store current time
	TBool iSharedNext;//new test case
	TBool iEndTest;//test has ended (last count is greater than current count)
	TInt iLastCount;//current test case being testsed
	TInt iMessageCount;//number of notifications to expect
	CContactChangeNotifier	*iNotifier;
	
	RArray<TInt> *iSharedContacts;//contact ids of expected notifications
	RArray<TInt> *iSharedEvents;//event ids of expected notifications
	RArray<TInt> *iDBContacts;//contact ids of database generated notifications
	RArray<TInt> *iDBEvents;//event ids of database generated notifications
	CConcurrentListenerStep::CConcurrentListenerActive *iActive;

private:
	class CConcurrentListenerActive : public CActive, MContactDbObserver
		{
	friend class CConcurrentListenerStep;
	public:
		CConcurrentListenerActive(CConcurrentListenerStep *aStep);
	private:
		void Activate();		
	private: // From CActive.
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);

	private: // from MContactDbObserver
		void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
	private:
		CConcurrentListenerStep *iStep;
		};
	};

_LIT(KConcurrentListenerStep,"ConcurrentListenerStep");

#endif
