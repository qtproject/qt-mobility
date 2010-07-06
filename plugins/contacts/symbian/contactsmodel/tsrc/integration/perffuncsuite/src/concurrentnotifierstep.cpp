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
* This step performs operations that generate notifications
* It sends messages to the concurrently running listner step / thread
* informing it what notifications it should be listening for, when it
* should be listening when it should stop listening, also it waits for
* messages from all the listeners infroming it that all mesasges have
* been received.
* Sending Messages:
* Set shared string to contain the number of notifications to expect. (10)
* Append shared string with valued of event in notification. KSharedEvents (1 2 3 4)
* Append shared string with value of contact id in notification. KSharedContact. (1 2 3 4)
* KSharedContact and KSharedEvents are synchronized so that at index X lies the contact and
* event value for notification X+1.
* Append shared string (KSharedNextTest) with single character. Signifies new
* test case has started. (AAAA)
* Receiving messages:
* Read shared string length signifying the number of listner threads
* that have finished from this test case. If this number equals number of
* listener threads, stop this test case and start next test case. (AAAAAA)
*
*/


/**
 @file
 @publishedAll
 @released
*/

#include "concurrentnotifierstep.h"
#include "performancefunctionalitydefs.h"
#include "ccnteventqueue.h"

_LIT(KRun1,"Update");
_LIT(KRun2,"Commit");
_LIT(KRun3,"Add");
_LIT(KRun4,"Delete");
_LIT(KRun5,"CloseOpenTables");
_LIT(KRun7,"OwncardAddDelete");
_LIT(KRun9,"Speeddial");
_LIT(KRun10,"TransactionCommit");
_LIT(KRun11,"TransactionRevert");
_LIT(KRun12,"CUDTransactionCommit");
_LIT(KRun13,"CUDTransactionRevert");
_LIT(KRun14,"EmptyTransactionCommit");
_LIT(KRun15,"EmptyTransactionRevert");
_LIT(KRun16,"UnknownChange");

_LIT(KTest1, "Update notification test");
_LIT(KTest2, "Commit notification test");
_LIT(KTest3, "Add notification test");
_LIT(KTest4, "Delete notification test");
_LIT(KTest5, "Open tables notification test");
_LIT(KTest6, "Close tables notification test");
_LIT(KTest7, "Own Card notification test");
_LIT(KTest14, "Change Own Card notification test");
_LIT(KTest9, "Own Card Deletion notification test");
_LIT(KTest8, "Speed dial notification test");
_LIT(KTest15, "Delete Speed dial Card notification test");
_LIT(KTest16, "Remove Speed dial Card notification test");
_LIT(KTest10, "Single commit transaction notification test");
_LIT(KTest102, "Single commit revert transaction notification test");
_LIT(KTest11, "Multiple Cud transaction notification test");
_LIT(KTest112, "Multiple Cud revert transaction notification test");
_LIT(KTest12, "Empty transaction notification test");
_LIT(KTest122, "Empty revert transaction notification test");
_LIT(KTest13, "unknown changes notification test");

_LIT(KClients,"Clients");
_LIT(KNotifier,"Notifier");
_LIT(KSperator," ");
_LIT(KSingleCharacter,"A");

const TInt KTimeout = 5000000;//5 seconds timeout

//start initlizer step

CConcurrentInitializerStep::CConcurrentInitializerStep() : CPerformanceFunctionalityBase(KConcurrentContacts)
	{
	iUnclean = ETrue;
	SetTestStepName(KConcurrentInitilizerStep);
	}

TVerdict CConcurrentInitializerStep::doTestStepL()
	{
	__UHEAP_MARK;
	_LIT(KStartTest,"CConcurrentInitializerStep::doTestStepL()");
	INFO_PRINTF1(KStartTest);  //Block start

	InitializeL();
	iIterate->Reset();
	TInt j = 0;
	for(; j < iContacts; ++j)
		{
		OpenL(iIterate->NextL());
		TInt fieldId=iFields->Find(KUidContactFieldPhoneNumber);
		SetFieldL(fieldId,GetFieldL(KStringFields, KField, 1));// if this is not set, speeddial cant be set
		CommitL(EFalse);
		}
	iIterate->Reset();
	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}
//end initlizer step


CConcurrentNotifierStep::CConcurrentNotifierStep() :
					CPerformanceFunctionalityBase(KConcurrentContacts)
	{
	iConcurrent = ETrue;
	SetTestStepName(KConcurrentNotifierStep);
	}

void CConcurrentNotifierStep::CConcurrentNotifierActive::RunL()
	{
	const TInt many = iStep->iTests->Count();
	if( iStep->iNextTest >= many )
		{
		if( many > 1 )
			{
			iStep->SetSharedTextL(KSharedContact,KNullDesC, EFalse);
			iStep->SetSharedTextL(KSharedEvents,KNullDesC, EFalse);
			iStep->SetSharedTextL(KSharedNextTest, KNullDesC, EFalse);
			}
		CActiveScheduler::Stop();
		iStep->iNextTest = 0;
		}
	else
		{
		if( iStep->iWaiting )
			{
			iStep->ProcessWaitL();
			}
		else
			{
			RArray< void (CConcurrentNotifierStep::*)() > &Tests = *( iStep->iTests );
			//select test to run
			void (CConcurrentNotifierStep::*TestFunction)() = Tests[ iStep->iNextTest ];
			//execute test
			( iStep->*TestFunction )();
			}
		Activate();
		}
	}

CConcurrentNotifierStep::CConcurrentNotifierActive::CConcurrentNotifierActive(CConcurrentNotifierStep *aStep)
								:	CActive( EPriorityStandard ),
									iStep ( aStep )
	{
	}

void CConcurrentNotifierStep::CConcurrentNotifierActive::DoCancel()
	{
	//nothing to cleanup
	}

TInt CConcurrentNotifierStep::CConcurrentNotifierActive::RunError(TInt aError)
	{
	_LIT(KActiveError,"CConcurrentNotifierStep:: Error in doTest runL: %d");
	iStep->ERR_PRINTF2( KActiveError , aError );
	return aError;
	}

void CConcurrentNotifierStep::CConcurrentNotifierActive::Activate()
	{
	if(!IsActive())
		{
		TRequestStatus *pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	}

CConcurrentNotifierStep::~CConcurrentNotifierStep()
	{
	if( iTests )
		{
		iTests->Close();
		CLEAR(iTests);
		}
	CLEAR(iMyActive);
	}

void CConcurrentNotifierStep::PreTestL()
	{
	_LIT(KPretestPrint,"I am in CConcurrentNotifierStep PreTestL()");
	INFO_PRINTF1(KPretestPrint);
	GetIntFromConfig( KNotifier, KClients, iManyListeners );
	iMyActive = new (ELeave) CConcurrentNotifierStep::CConcurrentNotifierActive( this );
	iTests = new(ELeave) RArray< void (CConcurrentNotifierStep::*)() >();

	ClearSharedL();
	SetSharedTextL(KSharedNextTest, KNullDesC, EFalse);

	const TDesC &run = ConfigSection();

	if( run == KRun1 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::UpdateTestL );
		}
	else if( run == KRun2 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::CommitTestL );
		}
	else if( run == KRun3 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::AddTestL );
		}
	else if( run == KRun4 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::DeleteTestL );
		}
	else if( run == KRun5 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::CloseTablesTestL );
		iTests->AppendL( &CConcurrentNotifierStep::OpenTablesTestL );
		}
	else if( run == KRun7 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::OwnCardTestL );
		iTests->AppendL( &CConcurrentNotifierStep::ChangeOwnCardTestL );
		iTests->AppendL( &CConcurrentNotifierStep::OwnCardDeleteTestL );
		}
	else if( run == KRun9 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::SpeedDialTestL );
		iTests->AppendL( &CConcurrentNotifierStep::RemoveSpeedDialTestL );
		iTests->AppendL( &CConcurrentNotifierStep::DeleteSpeedDialTestL );
		}
	else if( run == KRun10 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::SingleCommitTransactionTestL );
		}
	else if( run == KRun11 )
		{
		iRevert = ETrue;
		iTests->AppendL( &CConcurrentNotifierStep::SingleCommitTransactionTestL );
		}
	else if( run == KRun12 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::MultipleChangeTransactionTestL );
		}
	else if( run == KRun13 )
		{
		RProcess().SetPriority(EPriorityBackground);
		iRevert = ETrue;
		iTests->AppendL( &CConcurrentNotifierStep::MultipleChangeTransactionTestL );
		RProcess().SetPriority(EPriorityForeground);
		}
	else if( run == KRun14 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::EmptyTransactionTestL );
		}
	else if( run == KRun15 )
		{
		RProcess().SetPriority(EPriorityBackground);
		iRevert = ETrue;
		iTests->AppendL( &CConcurrentNotifierStep::EmptyTransactionTestL );
		RProcess().SetPriority(EPriorityForeground);
		}
	else if( run == KRun16 )
		{
		iTests->AppendL( &CConcurrentNotifierStep::UnkownChangeTestL );
		}
	else
		{
		MissingTestPanic();
		}

	}

TVerdict CConcurrentNotifierStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KDoStepPrint,"CTransactionsStep::doTestStepL()");
	INFO_PRINTF1(KDoStepPrint);  //Block start
	iIterate->Reset();

	SetSharedTextL(KSharedEvents,KNullDesC, EFalse);
	CActiveScheduler::Add(iMyActive);
	iMyActive->Activate();
	CActiveScheduler::Start();

	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}

TVerdict CConcurrentNotifierStep::doTestStepPostambleL()
	{
	_LIT(KPostamblePrint,"I am in CConcurrentNotifierStep doTestStepPostambleL()");
	INFO_PRINTF1(KPostamblePrint);
	ClearSharedL();
	SetSharedTextL(KSharedNextTest, KNullDesC, EFalse);
	return CPerformanceFunctionalityBase::doTestStepPostambleL();
	}


/**
tell listener to expect aEvent type notification
increase message count
*/
void CConcurrentNotifierStep::AddEventL(const TInt aEvent)
	{
	if( !iRevert )
		{
		ShareIntL(KSharedEvents, aEvent, ETrue, KSperator);
		++iMessageCount;//either here or in addcontact
		}
	}

/**
tell listener to expect notification for aCid contact
don't
*/
void CConcurrentNotifierStep::AddContactL(const TInt aCid)
	{
	if( !iRevert )
		{
		ShareIntL(KSharedContact, aCid, ETrue, KSperator);
		}
	}
/**
tell listener to expect iMessageCount many messages / notifications
sets the shared string to contain the value iMessageCount
*/
void CConcurrentNotifierStep::SetMessageCountL()
	{
	ShareIntL(KSharedMessageCount, iMessageCount);
	iMessageCount = 0;
	}

/**
each listener appends a charcter to shared memory KSharedClients
once it has received all notifications from this thread,
this returns length of string
*/
TInt CConcurrentNotifierStep::ReceivedCountL()
	{
	return SharedCountL(KSharedClients);
	}

/**
Sets the value of all shared memmories to Null
used at beginning/end of test case
*/
void CConcurrentNotifierStep::ClearSharedL()
	{
	SetSharedTextL(KSharedEvents, KNullDesC, EFalse);
	SetSharedTextL(KSharedContact, KNullDesC, EFalse);
	SetSharedTextL(KSharedClients, KNullDesC, EFalse);
	SetSharedTextL(KSharedMessageCount, KNullDesC, EFalse);
	}

/**
if in revert mode rollback transaction, otherwise commit,
tell listner to expect an notifications that may be generated
*/
void CConcurrentNotifierStep::EndTransactionL( )
	{
	if(iRevert)
		{
		iRevert = EFalse;
		iContactsDatabase->DatabaseRollback();
		AddEventL( EContactDbObserverEventRollback );
		AddContactL( KNullContactId );
		CleanupStack::Pop();
		TBool damaged = iContactsDatabase->IsDamaged();

		#ifdef _DEBUG
			#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
		//It maybe necessary to explicitly damage the database more than once
		//for it to be registered as damaged
		while( !iContactsDatabase->IsDamaged() )
			{
			//will give consistent behaviour on debug builds
			iContactsDatabase->DamageDatabaseL(0x666);
			}

		if( !damaged )
			{
			AddEventL( EContactDbObserverEventRollback );
			AddContactL( KNullContactId );
			}
			#endif
		#endif

		if( iContactsDatabase->IsDamaged() )
			{
			iContactsDatabase->RecoverL();
			AddEventL( EContactDbObserverEventRecover );
			AddContactL( 0 );
			if( !damaged )
				{
				AddEventL( EContactDbObserverEventTablesClosed );
				AddContactL( 0 );
				}
			AddEventL( EContactDbObserverEventTablesOpened );
			AddContactL( 0 );
			}
		iRevert = ETrue;
		}
	else
		{
		iContactsDatabase->DatabaseCommitLP(EFalse);
		}
	iRevert = !iRevert;
	}
/**
Used by all tests to wait for listener to recive notification and send confirmation back.
timesout if no confirmation received
*/
void CConcurrentNotifierStep::ProcessWaitL()
	{
	iEnd.UniversalTime();
	if(ReceivedCountL() == iManyListeners)
		{
		if( iTests->Count() > 1 )
			{
			ClearSharedL();
			}
		iWaiting = EFalse;
		++iNextTest;
		}
	else if( iEnd.MicroSecondsFrom( iStart ).Int64() > KTimeout ) //test has timed out
		{
		_LIT(KTimedOut,"Notifier test %d has timed out");
		ERR_PRINTF2(KTimedOut, ++iNextTest);

		if( iTests->Count() > 1 )
			{
			ClearSharedL();
			}
		iWaiting = EFalse;
		TESTPRINT( EFalse );
		iNextTest = iTests->Count();
		}
	//else test is not complete and has not timed out... continue waiting
	}

void CConcurrentNotifierStep::UpdateTestL()
	{
	INFO_PRINTF1(KTest1);
	SetSharedTextL(KSharedNextTest,KSingleCharacter , ETrue);
	iWaiting = ETrue;
	TContactItemId cid = iIterate->NextL();
	ReadL( cid );
	iContactsDatabase->UpdateContactLC( cid, iContactItem);
	CleanupStack::PopAndDestroy();
	CloseL( ETrue );
	AddEventL(EContactDbObserverEventContactChanged);
	AddContactL( cid );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::CommitTestL()
	{
	INFO_PRINTF1(KTest2);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	TContactItemId cid = iIterate->NextL();
	OpenL( cid );
	CommitL( EFalse );
	AddEventL(EContactDbObserverEventContactChanged);
	AddContactL( cid );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::AddTestL()
	{
	INFO_PRINTF1(KTest3);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	TContactItemId cid = iContactsDatabase->AddNewContactL(*contact);
	iIterate->AddL( cid );
	CleanupStack::PopAndDestroy(contact);
	AddEventL(EContactDbObserverEventContactAdded);
	AddContactL( cid );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::DeleteTestL()
	{
	INFO_PRINTF1(KTest4);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	TContactItemId cid = iIterate->NextL();
	DeleteContactL( cid, EFalse );
	AddEventL(EContactDbObserverEventContactDeleted);
	AddContactL( cid );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::OpenTablesTestL()
	{
	INFO_PRINTF1(KTest5);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	iContactsDatabase->OpenTablesL();
	AddEventL( EContactDbObserverEventTablesOpened );
	AddContactL( 0 );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::CloseTablesTestL()
	{
	INFO_PRINTF1(KTest6);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	iContactsDatabase->CloseTables();
	AddEventL( EContactDbObserverEventTablesClosed );
	AddContactL( 0 );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::OwnCardTestL()
	{
	INFO_PRINTF1(KTest7);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	CContactItem *item = iContactsDatabase->CreateOwnCardLC();
	iIterate->AddL( item->Id() );
	AddEventL( EContactDbObserverEventContactAdded );
	AddContactL( item->Id() );
	AddEventL( EContactDbObserverEventOwnCardChanged );
	AddContactL( item->Id() );
	CleanupStack::PopAndDestroy( item );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::ChangeOwnCardTestL()
	{
	INFO_PRINTF1(KTest14);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	TContactItemId cid = iIterate->NextL();
	OpenL( cid );
	iContactsDatabase->SetOwnCardL(*iContactItem);
	CloseL( ETrue );
	AddEventL( EContactDbObserverEventOwnCardChanged );
	AddContactL( cid );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::OwnCardDeleteTestL()
	{
	INFO_PRINTF1(KTest9);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	TContactItemId cid = iContactsDatabase->OwnCardId();
	ReadL( cid );
	CloseL( EFalse );
	DeleteContactL( cid, EFalse );
	AddEventL( EContactDbObserverEventOwnCardDeleted );
	AddContactL( cid);
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::SpeedDialTestL()
	{
	INFO_PRINTF1(KTest8);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	TContactItemId cid = iIterate->PreviousL();//start at last contact, so that other tests are not corrupted
	OpenL( cid );
	TInt fieldId=iFields->Find(KUidContactFieldPhoneNumber);
	iSpeedPosition = 1;
	iContactsDatabase->SetFieldAsSpeedDialL(*iContactItem, fieldId, iSpeedPosition);
	CloseL( EFalse );
	OpenL( cid );
	iSpeedPosition2 = 2;
	iContactsDatabase->SetFieldAsSpeedDialL(*iContactItem, fieldId, iSpeedPosition2);
	CloseL( EFalse );
	AddEventL( EContactDbObserverEventSpeedDialsChanged );
	AddContactL( cid );
	AddEventL( EContactDbObserverEventSpeedDialsChanged );
	AddContactL( cid );
	AddEventL( EContactDbObserverEventSpeedDialsChanged );
	AddContactL( cid );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::RemoveSpeedDialTestL()
	{
	INFO_PRINTF1(KTest16);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	TBuf<200> ptr;
	TContactItemId cid = iContactsDatabase->GetSpeedDialFieldL(iSpeedPosition2, ptr);
	iContactsDatabase->RemoveSpeedDialFieldL(cid, iSpeedPosition2);

	AddEventL( EContactDbObserverEventSpeedDialsChanged );
	AddContactL( cid );
	SetMessageCountL();
	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::DeleteSpeedDialTestL()
	{
	INFO_PRINTF1(KTest15);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	TBuf<200> ptr;
	TContactItemId cid = iContactsDatabase->GetSpeedDialFieldL(iSpeedPosition, ptr);
	DeleteContactL( cid, EFalse );
	AddEventL( EContactDbObserverEventSpeedDialsChanged );
	AddContactL( cid );
	AddEventL( EContactDbObserverEventContactDeleted );
	AddContactL( cid);
	SetMessageCountL();
	iStart.UniversalTime();
	}


void CConcurrentNotifierStep::SingleCommitTransactionTestL()
	{
	if( iRevert )
		{
		INFO_PRINTF1(KTest102);
		}
	else
		{
		INFO_PRINTF1(KTest10);
		}
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	TContactItemId cid = iIterate->NextL();

	iContactsDatabase->DatabaseBeginLC(EFalse);
	OpenL( cid );
	CommitL( ETrue );
	AddEventL(EContactDbObserverEventContactChanged);
	AddContactL( cid );
	EndTransactionL();
	SetMessageCountL();

	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::MultipleChangeTransactionTestL()
	{
	if( iRevert )
		{
		INFO_PRINTF1(KTest112);
		}
	else
		{
		INFO_PRINTF1(KTest11);
		}
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	TContactItemId cid = iIterate->NextL();

	iContactsDatabase->DatabaseBeginLC(EFalse);
	OpenL( cid );
	CommitL( ETrue );
	AddEventL(EContactDbObserverEventContactChanged);
	AddContactL( cid );

	cid = iIterate->NextL();
	ReadL( cid );
	iContactsDatabase->UpdateContactLC( cid, iContactItem);
	CleanupStack::PopAndDestroy();
	CloseL( ETrue );
	AddEventL(EContactDbObserverEventContactChanged);
	AddContactL( cid );

	CContactCard* contact = CContactCard::NewLC(iTemplate);
	cid = iContactsDatabase->doAddNewContactL(*contact, EFalse, ETrue);
	if(!iRevert)
		{
		iIterate->AddL( cid );
		}
	CleanupStack::PopAndDestroy(contact);
	AddEventL(EContactDbObserverEventContactAdded);
	AddContactL( cid );

	cid = iIterate->NextL();
	DeleteContactL( cid, ETrue );
	AddEventL(EContactDbObserverEventContactDeleted);
	AddContactL( cid );

	EndTransactionL();
	SetMessageCountL();

	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::EmptyTransactionTestL()
	{
	if( iRevert )
		{
		INFO_PRINTF1(KTest122);
		}
	else
		{
		INFO_PRINTF1(KTest12);
		}
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iContactsDatabase->DatabaseBeginLC(EFalse);
	AddEventL(KNullEvent);
	AddContactL( KNullEvent );
	EndTransactionL();
	SetMessageCountL();

	iStart.UniversalTime();
	}

void CConcurrentNotifierStep::UnkownChangeTestL()
	{
	INFO_PRINTF1(KTest13);
	SetSharedTextL(KSharedNextTest, KSingleCharacter, ETrue);
	iWaiting = ETrue;
	iMyActive->Activate();
	TContactItemId cid = iIterate->NextL();
	TInt i = 0;
	iContactsDatabase->DatabaseBeginLC(EFalse);
	for( ; i < KMaxNumberOfEventsInEventQueue + 1; ++i )
		{
		OpenL( cid );
		CommitL( EFalse );
		}
	AddEventL( EContactDbObserverEventUnknownChanges );
	AddContactL( -1 );
	EndTransactionL();
	SetMessageCountL();
	iStart.UniversalTime();
	}



