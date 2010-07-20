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
* Notifier step / thread, locks a contact / database, sends message to
* concurrent running listener thread / step informing it what has been locked
* waits for message from  listner to start countdown to release of lock
* listener will send message and wait for specified lock to be released
* or for timeout to be invoked by cntmodel.
* Sending Messages:
* Set shared string to contain length of time resource will be locked for in seconds. KSharedEvents. (6)
* Set shared string to contain  id of contact that is locked. KSharedContact. (1)
* Set shared string to contain a value, specify that new test case can start. KSharedNextTest. (aa)
* Receiving messages:
* Read shared string length signifying that thread should start countdown 
* until realease of resource. KSharedClients. (AA)
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#include "concurrenttimeoutnotifierstep.h"
#include "performancefunctionalitydefs.h"

_LIT(KRun1,"NonTrans");
_LIT(KRun2,"Transaction");

_LIT(KTest1, "Contacts timeout test");
_LIT(KTest2, "Transaction timeout test");

const static TInt KTimeout = 50000000;//50 seconds timeout
const static TInt KOneSecond = 1000000;

_LIT(KDoubleChar,"AA");
	
CConcurrentTimeOutNotifierStep::CConcurrentTimeOutNotifierStep() : 	
									CPerformanceFunctionalityBase(KConcurrentTimeoutContacts),
									iSeconds( 10 )
	{
	iConcurrent = ETrue;
	SetTestStepName(KConcurrentTimeoutNotifierStep);
	}
/**
runs the next test that will lock a particular resource
*/
void CConcurrentTimeOutNotifierStep::CConcurrentTimeOutNotifierMyActive::RunL()
	{
	const static TInt KTestCount = iStep->iTests->Count();
	if( iStep->iNextTest >= KTestCount )
		{
		CActiveScheduler::Stop();
		iStep->iNextTest = 0;
		}
	else
		{
		RArray< void (CConcurrentTimeOutNotifierStep::*)() > &Tests = *( iStep->iTests );
		//select test to run
		void (CConcurrentTimeOutNotifierStep::*TestFunction)() = Tests[ iStep->iNextTest ];
		//execute test
		( iStep->*TestFunction )();
		Activate();
		}
	}
	
void CConcurrentTimeOutNotifierStep::CConcurrentTimeOutNotifierMyActive::DoCancel()
	{
	//nothing to cleanup, does nothing.
	}

TInt CConcurrentTimeOutNotifierStep::CConcurrentTimeOutNotifierMyActive::RunError(TInt aError)
	{
	 _LIT(KActiveError,"CConcurrentTimeOutNotifierStep:: Error in doTest runL: %d");
	iStep->ERR_PRINTF2(KActiveError, aError );
	return aError;
	}

void CConcurrentTimeOutNotifierStep::CConcurrentTimeOutNotifierMyActive::Activate()
	{
	if(!IsActive())
		{	
		TRequestStatus *pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	}

CConcurrentTimeOutNotifierStep::~CConcurrentTimeOutNotifierStep()
	{
	if( iTests )
		{
		iTests->Close();
		CLEAR(iTests);
		}
	CLEAR(iMyActive);
	}

/**
create active object and test array
append contact lock or transaction lock test as necessary
*/	
void CConcurrentTimeOutNotifierStep::PreTestL()
	{
	iMyActive = new (ELeave) CConcurrentTimeOutNotifierStep::CConcurrentTimeOutNotifierMyActive( this );
	iTests = new(ELeave) RArray< void (CConcurrentTimeOutNotifierStep::*)() >();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		iTests->AppendL( &CConcurrentTimeOutNotifierStep::LockContactTestL );
		}
	else if( run == KRun2 )
		{
		iTests->AppendL( &CConcurrentTimeOutNotifierStep::LockDatabaseTestL );
		}
	else
		{
		MissingTestPanic();
		}
	}

TVerdict CConcurrentTimeOutNotifierStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KDoStepPrint,"CConcurrentTimeOutNotifierStep::doTestStepL()");
	INFO_PRINTF1(KDoStepPrint);  //Block start 
	iIterate->Reset();
	SetTimerL(-1);
	SetContactL( -1 );
	SetSharedTextL(KSharedNextTest, KDoubleChar, EFalse);
	CActiveScheduler::Add(iMyActive);
	iMyActive->Activate();
	CActiveScheduler::Start();
	
	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
sends message to listener specifying the length of time resource will be locked for
*/
void CConcurrentTimeOutNotifierStep::SetTimerL(const TInt aTime)
	{
	
	ShareIntL(KSharedEvents, aTime);
	
	}

/**
sends message to listener specifying the contact that will be locked
*/
void CConcurrentTimeOutNotifierStep::SetContactL(const TInt aCid)
	{
	
	ShareIntL(KSharedContact, aCid);
	
	}

/**
wait for message from listener specifying that countdown to release of resource should commence
*/
void CConcurrentTimeOutNotifierStep::StartCountdownL()
	{
	if ( iCountdown )
		{
		return;
		}
	iCountdown = SharedCountL(KSharedClients) > 0;
	if (iCountdown)
		{
		iStart.UniversalTime();
		SetSharedTextL(KSharedClients, KNullDesC, EFalse);
		}
	}
	
/**
either release resource after countdown has reached the necessary value
or timeout test if countdown is not started after the timeout period has passed
*/
void CConcurrentTimeOutNotifierStep::ProcessWaitL(const TBool aTransaction)
	{
	StartCountdownL();
	iEnd.UniversalTime();
	const TInt64 KLockTime = iEnd.MicroSecondsFrom( iStart ).Int64();
	const TInt64 KTestTime = iEnd.MicroSecondsFrom( iTestStart ).Int64();
	if( iCountdown && ( KLockTime >= ((iSeconds - KTimeDiff) * KOneSecond) ) )
		{
		iWaiting = EFalse;
		iCountdown = EFalse;
		if(aTransaction)
			{
			iContactsDatabase->DatabaseCommitL( EFalse );//unlock database
			}
		else
			{
			CloseL( ETrue );//unlock contact
			}
		++iNextTest;
		SetSharedTextL(KSharedNextTest, KDoubleChar, EFalse);
		}
	else if( KTestTime > KTimeout ) //test has timed out
		{
		_LIT(KTimedOut,"Notifier test %d has timed out");
		ERR_PRINTF2(KTimedOut, ++iNextTest);
		iWaiting = EFalse;
		iCountdown = EFalse;
		TESTPRINT( EFalse );
		SetSharedTextL(KSharedNextTest, KDoubleChar, EFalse);
		iNextTest = iTests->Count();
		}
	//else test is not complete and has not timed out... continue waiting
	}
	
/*
open a contact, this causes that contact to be locked
*/
void CConcurrentTimeOutNotifierStep::LockContactTestL()
	{
	if(iWaiting)
		{
		ProcessWaitL( EFalse );
		}
	else
		{
		INFO_PRINTF1(KTest1);
		iWaiting = ETrue;
		TContactItemId cid = iIterate->NextL();
		OpenL( cid );
		//notify listener that database will be unlocked after x amount of time
		SetTimerL( iSeconds - KTimeDiff );
		SetContactL( cid );
		iTestStart.UniversalTime();
		}
	}
	
/*
starts a transactions
this causes the database to be locked	
*/
void CConcurrentTimeOutNotifierStep::LockDatabaseTestL()
	{
	if(iWaiting)
		{
		ProcessWaitL( ETrue );
		}
	else
		{
		INFO_PRINTF1(KTest2);
		iWaiting = ETrue;
		TContactItemId cid = iIterate->NextL();
		iContactsDatabase->DatabaseBeginLC(EFalse);
		//notify listener that contact will be unlocked after x amount of time
		SetTimerL( iSeconds - KTimeDiff );
		SetContactL( cid );
		CleanupStack::Pop(); // cleanup item from the CContactDatabase::DatabaseBeginLC() 
		iTestStart.UniversalTime();
		}
	}
		



