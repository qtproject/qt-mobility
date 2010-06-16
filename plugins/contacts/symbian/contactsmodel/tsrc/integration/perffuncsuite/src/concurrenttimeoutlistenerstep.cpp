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
#include "concurrenttimeoutlistenerstep.h"
#include "performancefunctionalitydefs.h"

_LIT(KRun1,"Open");
_LIT(KRun2,"Update");
_LIT(KRun3,"Read");
_LIT(KRun4,"Delete");
_LIT(KRun5,"Add");

_LIT(KTest1, "Contacts Open timeout test");
_LIT(KTest2, "Contacts Update timeout test");
_LIT(KTest3, "Contacts Read timeout test");
_LIT(KTest4, "Contacts Delete timeout test");
_LIT(KTest5, "Contacts Add/Create timeout test");

const TInt KTimeout = 50000000;//50 seconds timeout
const TInt KOneSecond = 1000000;
const TInt KLocalTimeOut = 10;

_LIT(KDoubleChar,"AA");

	
CConcurrentTimeOutListenerStep::CConcurrentTimeOutListenerStep() : 	CTimeoutBaseStep(KConcurrentTimeoutContacts, KDeviation), 
																	iCid( -1 ),
																	iSharedTime( -1 )
	{
	iConcurrent = ETrue;
	SetTestStepName(KConcurrentTimeOutListenerStep);
	}

/**
runs the next timeout test after receiving message specifying that the relevant
resources have been locked for said amount of time, then sends message back stating that
countdown should take place.
timesout if no messages are received from the notifier
*/
void CConcurrentTimeOutListenerStep::CConcurrentTimeOutListenerMyActive::RunL()
	{
	const TInt KTestCount = iStep->iTests->Count();
	iStep->iEnd.UniversalTime();
	iStep->GetTimerL();
	iStep->GetContactL();
	iStep->GetNextTestL();
	TInt64 alldiff = iStep->iEnd.MicroSecondsFrom( iStep->iTestStart ).Int64();
	if( iStep->iNextTest >= KTestCount )
		{
		CActiveScheduler::Stop();
		iStep->iNextTest = 0;
		}
	else if( alldiff > KTimeout )
		{
		_LIT(KTimedOut,"Listener test %d has timed out");
		iStep->ERR_PRINTF2(KTimedOut, ++iStep->iNextTest);
		iStep->iCid = -1;
		iStep->iSharedTime = -1;
		
		iStep->TEST1( EFalse, ETrue );
		iStep->iNextTest = iStep->iTests->Count();
		Activate();
		}
	else if( iStep->iSharedNext && iStep->iCid > 0 && iStep->iSharedTime > 0)
		{
		iStep->iSharedNext = EFalse;
		iStep->RemoteLockL();
		iStep->TEST1( (iStep->*(*(iStep->iTests))[iStep->iNextTest++] )(), ETrue);
		iStep->iTestStart.UniversalTime();
		Activate();
		}
	else
		{
		Activate();
		}
	}
	
void CConcurrentTimeOutListenerStep::CConcurrentTimeOutListenerMyActive::DoCancel()
	{
	}

TInt CConcurrentTimeOutListenerStep::CConcurrentTimeOutListenerMyActive::RunError(TInt aError)
	{
	_LIT(KActiveError,"CConcurrentTimeOutListenerStep:: Error in doTest runL: %d");
	iStep->ERR_PRINTF2(KActiveError, aError );
	return aError;
	}
/**
activates the active object
*/
void CConcurrentTimeOutListenerStep::CConcurrentTimeOutListenerMyActive::Activate()
	{
	if(!IsActive())
		{	
		TRequestStatus *pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	}

CConcurrentTimeOutListenerStep::~CConcurrentTimeOutListenerStep()
	{
	if( iTests )
		{
		iTests->Close();
		CLEAR(iTests);
		}
	CLEAR(iMyActive);
	}
	
void CConcurrentTimeOutListenerStep::PreTestL()
	{
	iMyActive = new (ELeave) CConcurrentTimeOutListenerStep::CConcurrentTimeOutListenerMyActive( this );
	iTests = new(ELeave) RArray< TBool (CConcurrentTimeOutListenerStep::*)() >();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		iTests->AppendL( &CConcurrentTimeOutListenerStep::OpenContactTestL );
		}
	else if( run == KRun2 )
		{
		iTests->AppendL( &CConcurrentTimeOutListenerStep::UpdateContactTestL );
		}
	else if( run == KRun3 )
		{
		iTests->AppendL( &CConcurrentTimeOutListenerStep::ReadContactTestL );
		}
	else if( run == KRun4 )
		{
		iTests->AppendL( &CConcurrentTimeOutListenerStep::DeleteContactTestL );
		}
	else if( run == KRun5 )
		{
		iTests->AppendL( &CConcurrentTimeOutListenerStep::AddContactTestL );
		}
	else
		{
		MissingTestPanic();
		}
	
	}

TVerdict CConcurrentTimeOutListenerStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KDoStepPrint,"CConcurrentTimeOutListenerStep::doTestStepL()");
	INFO_PRINTF1(KDoStepPrint);  //Block start 
	iIterate->Reset();
	
	//RemoteLockL();
	CActiveScheduler::Add(iMyActive);
	iMyActive->Activate();
	iTestStart.UniversalTime();
	CActiveScheduler::Start();
	
	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
get message from notifier specifying epxected timeout
then clear message
*/
void CConcurrentTimeOutListenerStep::GetTimerL()
	{
	if( iSharedTime <= 0)
		{
		iSharedTime = GetSharedIntL(KSharedEvents);
		}
	if( iSharedTime > 0)
		{
		ShareIntL(KSharedEvents, -1);
		}
	}

/**
get message from notifier specifying the locked resource
then clear mesasge
*/	
void CConcurrentTimeOutListenerStep::GetContactL()
	{
	if( iCid <= 0)
		{
		iCid = GetSharedIntL(KSharedContact);
		}
	if( iCid > 0)
		{
		ShareIntL(KSharedContact, -1);
		}
	}

/**
get message from notifier specifying that next test has started
then clear mesasge
*/	
void CConcurrentTimeOutListenerStep::GetNextTestL()
	{
	if( !iSharedNext )
		{
		iSharedNext = SharedCountL(KSharedNextTest) > 0;
		}
	if( iSharedNext )
		{
		SetSharedTextL(KSharedNextTest, KNullDesC, EFalse);
		}
	}
/**
specify that countdown should being on notifier
*/	
void CConcurrentTimeOutListenerStep::RemoteLockL()
	{
	SetSharedTextL(KSharedClients, KDoubleChar, EFalse );
	}
		
TBool CConcurrentTimeOutListenerStep::OpenContactTestL()
	{
	TBool ret = EFalse;
	INFO_PRINTF1(KTest1);
	iTimeOut = ( (KLocalTimeOut + iSharedTime) * KOneSecond );
	ret = CheckTimeOutL(&CConcurrentTimeOutListenerStep::CheckOpenL, iContactsDatabase, iCid, KErrNone, ( iSharedTime * KOneSecond ) );
	return ret;
	}
		
TBool CConcurrentTimeOutListenerStep::UpdateContactTestL()
	{
	TBool ret = EFalse;
	INFO_PRINTF1(KTest2);
	iTimeOut = ( (KLocalTimeOut + iSharedTime) * KOneSecond );
	ret = CheckTimeOutL(&CConcurrentTimeOutListenerStep::CheckUpdateL, iContactsDatabase, iCid, KErrNone, ( iSharedTime * KOneSecond ) );
	return ret;
	}
	
TBool CConcurrentTimeOutListenerStep::ReadContactTestL()
	{
	TBool ret = EFalse;
	INFO_PRINTF1(KTest3);
	iTimeOut = ( (KLocalTimeOut + iSharedTime) * KOneSecond );
	ret = CheckTimeOutL(&CConcurrentTimeOutListenerStep::CheckRead, iContactsDatabase, iCid, KErrNone, 0 );
	return ret;
	}
		
TBool CConcurrentTimeOutListenerStep::DeleteContactTestL()
	{
	TBool ret = EFalse;
	INFO_PRINTF1(KTest4);
	iTimeOut = ( (KLocalTimeOut + iSharedTime) * KOneSecond );
	ret = CheckTimeOutL(&CConcurrentTimeOutListenerStep::CheckDelete, iContactsDatabase, iCid, KErrNone, ( iSharedTime * KOneSecond ) );
	return ret;
	}
		
TBool CConcurrentTimeOutListenerStep::AddContactTestL()
	{
	TBool ret = EFalse;
	INFO_PRINTF1(KTest5);
	iTimeOut = ( (KLocalTimeOut + iSharedTime) * KOneSecond );
	ret = CheckTimeOutL(&CConcurrentTimeOutListenerStep::CheckAddL, iContactsDatabase, iCid, KErrNone, ( iSharedTime * KOneSecond ) );
	return ret;
	}


		
