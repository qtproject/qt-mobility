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
* Listener step / thread waits for messages from notifier thread
* also listens to messages from the contacts model, and compares the two
* set of messages to see if they are identical.
* Prints out a list of expected messages ( those from notifier ) and
* database messages (from cntmodel). If comparison fails or timeout occurs,
* test fails.
* Sending messages:
* Append shared string length signifying the number of listner threads 
* that have finished from this test case. Once all notifications have
* been processed append single charcter to shared string.(AAAAAA)
* Receiving Messages:
* Read shared string containing the number of notifications to expect. iMessageCount. (10)
* Read shared string containing events of notification. iSharedContacts. (1 2 3 4)
* Read shared string with value of contact id in notification. iSharedEvents. (1 2 3 4)
* iSharedContacts and iSharedEvents are synchronized so that at index X lies the contact and
* event value for notification X+1.
* Read shared string (KSharedNextTest), length of string signifies current test case number
* when value inscreases signifies new test case has started. iLastCount. (AAAA)
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#include "concurrentlistenerstep.h"
#include "performancefunctionalitydefs.h"

const static TInt KTimeout = 5000000;//5 seconds timeout
const static TInt KMaxShareStringSize = 300; //Shared string used by TEF is < 300 charcters long
	
CConcurrentListenerStep::CConcurrentListenerStep() : 	
					CPerformanceFunctionalityBase(KConcurrentContacts),
					iMessageCount( -1 )
	{
	iConcurrent = ETrue;
	SetTestStepName(KConcurrentListenerStep);
	}
	
void CConcurrentListenerStep::PreTestL()
	{
	iActive = new (ELeave) CConcurrentListenerStep::CConcurrentListenerActive( this );
	}
	
CConcurrentListenerStep::~CConcurrentListenerStep()
	{
	CLEAR(iActive);
	}
	
void CConcurrentListenerStep::CConcurrentListenerActive::RunL()
	{
	_LIT(KDatabaseGenerated,"Database Generated");
	_LIT(KDatabaseExpected, "Expected");
	iStep->GetNextTestL();
	
	//if new test case, clear arrays and message count
	if(!iStep->iSharedNext)
		{	
		iStep->ResetArrays();
		iStep->iMessageCount = -1;
		}
		
	//if continuing with current test case, continue waiting for notifications
	else
		{
		iStep->GetMessageCountL();
		iStep->GetEventsL();
		iStep->GetContactsL();
		}
		
	//if expecting messages	
	if( iStep->iMessageCount >= 0 )
		{
		iStep->iEnd.UniversalTime();
		}

	//check test has timed out, if time limit exceeded fail test step
	if( iStep->iEnd.MicroSecondsFrom( iStep->iStart ).Int64() > KTimeout ) 
		{
		_LIT(KTimedOut, "CConcurrentListenerStep listener has timed out");
		iStep->ERR_PRINTF1( KTimedOut );
		iStep->TEST1( EFalse, ETrue );//fail test
		iStep->PrintNotifications(KDatabaseGenerated, *iStep->iDBEvents, *iStep->iDBContacts);
		iStep->PrintNotifications(KDatabaseExpected, *iStep->iSharedEvents, *iStep->iSharedContacts);		
		// go to next test
		iStep->iSharedNext = EFalse;
		iStep->iMessageCount = -1;
		iStep->ResetArrays();
		iStep->NotifyReceivedL();
		CActiveScheduler::Stop();
		}
		
	//if test has not ended and message received
	else if( iStep->iSharedNext && ( iStep->iMessageCount >= 0 ) )
		{
		/*if expected message count (shared*) matches the
		db generated notifications (DB*)
		*/
		if( iStep->iMessageCount == iStep->iSharedEvents->Count() 
			&& iStep->iMessageCount == iStep->iSharedContacts->Count()
			&& iStep->iMessageCount == iStep->iDBEvents->Count() 
			&& iStep->iMessageCount == iStep->iDBContacts->Count() )
			{
			iStep->iSharedNext = EFalse;
			iStep->iMessageCount = -1;
			//compare message content
			const TBool chk = iStep->CompareNotifications();
			iStep->TEST1( chk , ETrue );
			//if comparison fails, print content of expected and received notifications
			if(!chk)
				{
				iStep->PrintNotifications(KDatabaseGenerated, *iStep->iDBEvents, *iStep->iDBContacts);
				iStep->PrintNotifications(KDatabaseExpected, *iStep->iSharedEvents, *iStep->iSharedContacts);
				}
			iStep->ResetArrays();
			iStep->NotifyReceivedL();//send message to notifier step thread that message has been received/processed
			}
		Activate();	//wait for all messages to get propogated
		}
		
	//test has ended (last count is greater than current count)
	else if( iStep->iEndTest )
		{
		CActiveScheduler::Stop();
		}
		
	//waiting for next event/test
	else
		{
		Activate();
		}
	}

CConcurrentListenerStep::CConcurrentListenerActive::CConcurrentListenerActive(CConcurrentListenerStep *aStep) 
													: 	CActive(EPriorityIdle),
														iStep(aStep){}
	
void CConcurrentListenerStep::CConcurrentListenerActive::DoCancel()
	{
	//nothing to cleanup
	}

TInt CConcurrentListenerStep::CConcurrentListenerActive::RunError(TInt aError)
	{
	_LIT(KActiveError, "CConcurrentListenerStep:: Error in CConcurrentListenerActive runL: %d");
	iStep->ERR_PRINTF2( KActiveError, aError );
	return aError;
	}

void CConcurrentListenerStep::CConcurrentListenerActive::Activate()
	{
	if(!IsActive())
		{
		TRequestStatus *pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	}

void CConcurrentListenerStep::CConcurrentListenerActive::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{  
	iStep->iDBContacts->Append(aEvent.iContactId);
	iStep->iDBEvents->Append(aEvent.iType);
	Activate();

	}

void CConcurrentListenerStep::InitializeL()
	{
	CPerformanceFunctionalityBase::InitializeL();
	iNotifier = CContactChangeNotifier::NewL(*iContactsDatabase, iActive);
	iSharedContacts = new (ELeave)RArray<TInt>();
	iSharedEvents = new (ELeave)RArray<TInt>();
	iDBContacts = new (ELeave)RArray<TInt>();
	iDBEvents = new (ELeave)RArray<TInt>();
	}

void CConcurrentListenerStep::Cleanup()
	{
	if( iSharedEvents )
		{
		iSharedEvents->Reset();
		CLEAR(iSharedEvents);
		}
	if( iSharedContacts )
		{
		iSharedContacts->Reset();
		CLEAR(iSharedContacts);
		}
	if( iDBEvents )
		{
		iDBEvents->Reset();
		CLEAR(iDBEvents);
		}
	if( iDBContacts )
		{
		iDBContacts->Reset();
		CLEAR(iDBContacts);
		}
	CLEAR( iNotifier );
	CPerformanceFunctionalityBase::Cleanup();
	}

TVerdict CConcurrentListenerStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KStartTest, "CTransactionsStep::doTestStepL()");
	INFO_PRINTF1(KStartTest);  //Block start 
	iIterate->Reset();
	
	CActiveScheduler::Add(iActive);
	iActive->Activate();
	CActiveScheduler::Start();

	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}
/**
this listener sends message stating that it has received and prcoessed a notification
*/
void CConcurrentListenerStep::NotifyReceivedL()
	{
	_LIT(KSingleChar,"A");
	SetSharedTextL( KSharedClients, KSingleChar , ETrue);	
	}

/**
receive an array of ints from notifier
*/
void CConcurrentListenerStep::GetSharedArrayL(const TDesC &aShare, RArray<TInt> &aArray, RArray<TInt> &aDBArray)
	{
	TInt val = -1;//invalid value, no negative values should be stored in shared array
	HBufC *fval = HBufC::NewLC( KMaxShareStringSize );
	
	TPtr txtReadData = fval->Des();
	ReadSharedDataL(aShare, txtReadData);
	if ( txtReadData.Length() > 0 )
		{		
		TLex lex( txtReadData );
		while( !lex.Eos() )
			{
			TLex token( lex.NextToken() );
			User::LeaveIfError( token.Val( val ) );
			aArray.Append(val);
			if( val == KNullEvent )
				{
				aDBArray.Append(val);
				}
			}
		}
	CleanupStack::PopAndDestroy( fval );
	}

/**
get array of events from notifier
*/
void CConcurrentListenerStep::GetEventsL()
	{
	if(iSharedEvents->Count() < iMessageCount)
		{
		GetSharedArrayL(KSharedEvents, *iSharedEvents, *iDBEvents);
		}
		
	}

/**
get array of contacts from notifier
*/	
void CConcurrentListenerStep::GetContactsL()
	{
	if(iSharedContacts->Count() < iMessageCount)
		{
		GetSharedArrayL(KSharedContact, *iSharedContacts, *iDBContacts);
		}
	}

/**
waits to receive notification that another test has started
*/
void CConcurrentListenerStep::GetNextTestL()
	{
	if(!iSharedNext)
		{
		TInt testcount = SharedCountL(KSharedNextTest);
		iSharedNext = testcount > iLastCount;
		iEndTest = testcount < iLastCount;
		if( iSharedNext )
			{
			iLastCount = testcount;
			}
		}
	}

/**
get the number of messages sent by notifer
*/
void CConcurrentListenerStep::GetMessageCountL()
	{
	if( iMessageCount < 0)
		{
		iMessageCount = GetSharedIntL(KSharedMessageCount);
		if(iMessageCount >= 0)
			{
			iStart.UniversalTime();
			}
		}
	}

/**
clears all arrays storing recived notifications
*/
void CConcurrentListenerStep::ResetArrays()
	{		
	iSharedEvents->Reset();
	iSharedContacts->Reset(); 
	iDBContacts->Reset();
	iDBEvents->Reset();
	}

/**
compare expected notifications to notifications received from the database
*/
TBool CConcurrentListenerStep::CompareNotifications()
	{
	TBool OverallTestResult = EFalse;
	TBool SingleTestResult = EFalse;
	
	TESTPRINT( SingleTestResult = ( iSharedContacts->Count() == iDBContacts->Count() ) );
	OverallTestResult = SingleTestResult;
	TESTPRINT( SingleTestResult = ( iSharedEvents->Count() == iDBEvents->Count() ) );
	OverallTestResult = OverallTestResult && SingleTestResult;
	TESTPRINT( SingleTestResult = ( iSharedEvents->Count() == iSharedContacts->Count() ) ); 
	OverallTestResult = OverallTestResult && SingleTestResult;
	
	TInt length = iSharedContacts->Count();
	TInt i = 0;
	for(; i < length; ++i )
		{
		TESTPRINTI( SingleTestResult = ( iSharedContacts->Find( (*iDBContacts)[i] ) > KErrNotFound ), i );
		OverallTestResult = OverallTestResult && SingleTestResult;
		
		TESTPRINTI( SingleTestResult = ( iSharedEvents->Find( (*iDBEvents)[i] ) > KErrNotFound ), i );
		OverallTestResult = OverallTestResult && SingleTestResult;
		}
	
	return OverallTestResult;
	}

