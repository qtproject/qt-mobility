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
* These tests attempt to overload the transaction / event queue
* by calling many operations before the active scheduler is started
* or by calling the operations within the active scheduler.
* Unknownchanges event should be generated when queue is overloaded
*
*/


/**
 @file 
 @publishedAll
 @released
*/
#include "notificationstep.h"
#include "performancefunctionalitydefs.h"
#include "ccnteventqueue.h"

_LIT(KRun1,"UnkownChange");
_LIT(KRun2,"TransactionUnkown");
_LIT(KRun3,"TransactionUnkownPlus");
_LIT(KRun4,"RevertTransactionUnkown");
_LIT(KRun5,"RevertTransactionUnkownPlus");

_LIT(KTest1, "unknown changes notification test");
_LIT(KTest2, "Transaction unknown changes notification test");
_LIT(KTest3, "Transaction unknown changes plus additional changes notification test");

const TInt KTimeout = 5000000;//5 seconds timeout
const TInt KPlusContacts = 10;
const TInt KInitialCount = -1;

#define KNumberOfContacts 10

CNotificationStep::CNotificationStep() 
					: 	CPerformanceFunctionalityBase( KNumberOfContacts ),
						iMessageCount( KInitialCount )
	{
	SetTestStepName(KNotificationStep);
	}
	
void CNotificationStep::PreTestL()
	{
	iMyActive = new (ELeave) CNotificationStep::CNotificationActive( this );
	}
	
CNotificationStep::~CNotificationStep()
	{
	CLEAR(iMyActive);
	}
/**
checks notifications are received and makes sure that they're correct,
times out if notification is not received after timeout period
*/	
void CNotificationStep::CNotificationActive::RunL()
	{
	_LIT(KDatabaseGenerated,"Database Generated");
	_LIT(KExpected,"Expected");
	iStep->iEnd.UniversalTime();

	
	if( iStep->iEnd.MicroSecondsFrom( iStep->iStart ).Int64() > KTimeout ) //test has timed out
		{
		 _LIT(KTimedOut,"CNotificationStep listener has timed out");
		iStep->ERR_PRINTF1(KTimedOut );
		iStep->PrintNotifications(KDatabaseGenerated, *iStep->iDBEvents, *iStep->iDBContacts);
		iStep->PrintNotifications(KExpected, *iStep->iSharedEvents, *iStep->iSharedContacts);
		iStep->TEST1( EFalse, ETrue );//fail test
		iStep->ResetArrays();
		iStep->iMessageCount = KInitialCount;
		iStep->iStart.UniversalTime();
		CActiveScheduler::Stop();
		}
	else if( iStep->iMessageCount > KInitialCount )// if zero or more notifications are expected
		{
		//all expected messages have been received
		if( iStep->iMessageCount == iStep->iSharedEvents->Count() 
			&& iStep->iMessageCount == iStep->iSharedContacts->Count()
			&& iStep->iMessageCount == iStep->iDBEvents->Count() 
			&& iStep->iMessageCount == iStep->iDBContacts->Count() )
			{
			const TBool chk = (iStep->*iCheck)();
			iStep->TEST1( chk  , ETrue );
			//if test fails
			if(!chk)
				{
				iStep->PrintNotifications(KDatabaseGenerated, *iStep->iDBEvents, *iStep->iDBContacts);
				iStep->PrintNotifications(KExpected, *iStep->iSharedEvents, *iStep->iSharedContacts);
				}
			iStep->iMessageCount = KInitialCount;
			iStep->ResetArrays();
			iStep->iStart.UniversalTime();
			CActiveScheduler::Stop();	
			}
		else//wait for rest of db message to be propogated
			{
			Activate();
			}
		}
	else //test hasnt timed out and no notifications are expected
		{
		if( !iClear )//not expecting to receive notifications for contacts added in preamble, fail test.
			{			
			_LIT(KEmptyTestError,"CNotificationStep::CNotificationActive::RunL: Notifications have not been queued up");
			iStep->ERR_PRINTF1(KEmptyTestError );
			iStep->TEST1( EFalse, ETrue );//fail test
			iStep->ResetArrays();
			iStep->iMessageCount = KInitialCount;
			CActiveScheduler::Stop();
			}
		//expecting to receive notifications for contacts added in preamble, have received all expected notifications
		else if( iStep->iDBContacts->Count() >= KNumberOfContacts )
			{
			iStep->ResetArrays();
			iStep->iMessageCount = KInitialCount;
			CActiveScheduler::Stop();
			}
		else//have not received all expected notifications from contacts added in preamble
			{
			Activate();
			}
		}
	}
	
void CNotificationStep::CNotificationActive::DoCancel()
	{
	}

TInt CNotificationStep::CNotificationActive::RunError(TInt aError)
	{
	 _LIT(KActiveError,"CNotificationStep:: Error in doTest runL: %d");
	iStep->ERR_PRINTF2(KActiveError, aError );
	return aError;
	}

void CNotificationStep::CNotificationActive::Activate()
	{
	if(!IsActive())
		{
		TRequestStatus *pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	}
	
//receives notificaiton from database
void CNotificationStep::CNotificationActive::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{  
	iStep->iDBContacts->Append(aEvent.iContactId);
	iStep->iDBEvents->Append(aEvent.iType);
	Activate();

	}

void CNotificationStep::InitializeL()
	{
	CPerformanceFunctionalityBase::InitializeL();
	iNotifier = CContactChangeNotifier::NewL(*iContactsDatabase, iMyActive);
	iSharedContacts = new (ELeave)RArray<TInt>();
	iSharedEvents = new (ELeave)RArray<TInt>();
	iDBContacts = new (ELeave)RArray<TInt>();
	iDBEvents = new (ELeave)RArray<TInt>();
	}

void CNotificationStep::Cleanup()
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

TVerdict CNotificationStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	
	_LIT(KDoStepPrint,"CNotificationStep::doTestStepL()");
	INFO_PRINTF1(KDoStepPrint);  //Block start 
	iIterate->Reset();
	
	CActiveScheduler::Add(iMyActive);
	ClearNotifications();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		UnkownChangeTestL();
		}
	else if( run == KRun2 )
		{
		TransactionUnkownChangeTestL( EFalse );
		}
	else if( run == KRun3 )
		{
		PlusTransactionUnkownChangeTestL( EFalse );
		}
	else if( run == KRun4 )
		{
		TransactionUnkownChangeTestL( ETrue );
		}
	else if( run == KRun5 )
		{
		PlusTransactionUnkownChangeTestL( ETrue );
		}
	else
		{
		MissingTestPanic();
		}

	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}
	
void CNotificationStep::ClearNotifications()
	{
	//clear any prior notifications, due to adding contacts...
	iMyActive->iClear = ETrue;
	iMyActive->iCheck = &CNotificationStep::UnkownChangeCheck;
	iMyActive->Activate();
	iStart.UniversalTime();
	CActiveScheduler::Start();
	iMyActive->iClear = EFalse;
	}

void CNotificationStep::ResetArrays()
	{		
	iSharedEvents->Reset();
	iSharedContacts->Reset(); 
	iDBContacts->Reset();
	iDBEvents->Reset();
	}
/**
add event to array and increment message count
*/
void CNotificationStep::AddEventL(const TInt aEvent)
	{
	iSharedEvents->AppendL(aEvent);
	//increment message count, if message count uninitialized, set to 1.
	( ( iMessageCount < 0 ) ? ( iMessageCount=1 ) : ++iMessageCount );
	}
	
/**
add contact to array
*/
void CNotificationStep::AddContactL(const TInt aCid)
	{
	iSharedContacts->AppendL( aCid );
	//message count will be incremented in addevent
	}

/**
commit or rollback transaction as necessary, recover database if damage by rollback
*/
void CNotificationStep::EndTransactionL(const TBool aRevert)
	{
	if(aRevert)
		{
		iContactsDatabase->DatabaseRollback();
		AddEventL( EContactDbObserverEventRollback );
		AddContactL( KNullContactId );
		CleanupStack::Pop();
		if( iContactsDatabase->IsDamaged() )
			{
			iContactsDatabase->RecoverL();
			AddEventL( EContactDbObserverEventTablesClosed );
			AddContactL( 0 );
			AddEventL( EContactDbObserverEventTablesOpened );
			AddContactL( 0 );
			}
		}
	else
		{
		iContactsDatabase->DatabaseCommitLP(EFalse);
		}
	}
	
void CNotificationStep::UnkownChangeTestL()
	{
	INFO_PRINTF1(KTest1);
	TContactItemId cid = iIterate->NextL();
	TInt i = 0;
	for( ; i < KMaxNumberOfEventsInEventQueue + 10; ++i )
		{
		OpenL( cid );
		CommitL( EFalse );
		}
	AddEventL( EContactDbObserverEventContactChanged );
	AddContactL( cid );
	AddEventL( EContactDbObserverEventUnknownChanges );
	AddContactL( -1 );
	iMyActive->iCheck = &CNotificationStep::UnkownChangeCheck;
	iMyActive->Activate();
	iStart.UniversalTime();
	CActiveScheduler::Start();
	}	
				
/**
check for unkownchange notification cause by the session queue being overloaded
*/	
TBool CNotificationStep::UnkownChangeCheck()
	{
	TBool SingleCheck = EFalse;
	TBool OverallResult = ETrue;
	
	TESTPRINT( SingleCheck = ( iSharedContacts->Count() == iDBContacts->Count()) );
	OverallResult = OverallResult && SingleCheck;
	
	TESTPRINT( SingleCheck = ( iSharedEvents->Count() == iDBEvents->Count() ) );
	OverallResult = OverallResult && SingleCheck;
	
	TESTPRINT( SingleCheck = ( iSharedEvents->Count() == iSharedContacts->Count() ) );
	OverallResult = OverallResult && SingleCheck;
	
	TInt length = iSharedContacts->Count();
	TInt i = 0;
	for(; i < length; ++i )
		{
		TESTPRINTI( SingleCheck = ( iSharedContacts->Find( (*iDBContacts)[i] ) > KErrNotFound ), i );
		OverallResult = OverallResult && SingleCheck;
		
		TESTPRINTI( SingleCheck = ( iSharedEvents->Find( (*iDBEvents)[i] ) > KErrNotFound ) , i );
		OverallResult = OverallResult && SingleCheck;
		}
	
	return OverallResult;
	}

/**
check for unknownchange notification cause by the transaction queue being overloaded
*/	
void CNotificationStep::TransactionUnkownChangeTestL(const TBool aRevert)
	{
	INFO_PRINTF1(KTest2);
	
	TContactItemId cid = iIterate->NextL();
	TInt i = 0;
	iContactsDatabase->DatabaseBeginLC(EFalse);
	for( ; i < KMaxNumberOfEventsInEventQueue + 1; ++i )
		{
		OpenL( cid );
		CommitL( ETrue );
		}
	EndTransactionL( aRevert );
	
	if( !aRevert )
		{
		AddEventL( EContactDbObserverEventUnknownChanges );
		AddContactL( -1 );
		}
	iMyActive->iCheck = &CNotificationStep::TransactionUnkownChangeCheck;
	iMyActive->Activate();
	iStart.UniversalTime();
	CActiveScheduler::Start();
	}					
	
TBool CNotificationStep::TransactionUnkownChangeCheck()
	{
	TBool SingleCheck = EFalse;
	TBool OverallResult = ETrue;
	
	TESTPRINT( SingleCheck = ( iMessageCount == iDBContacts->Count() ) );
	OverallResult = OverallResult && SingleCheck;
	
	TESTPRINT( SingleCheck = ( iMessageCount == iDBEvents->Count() ) );
	OverallResult = OverallResult && SingleCheck;
	
	TInt length = iSharedContacts->Count();
	TInt i = 0;
	for(; i < length; ++i )
		{
		TESTPRINTI( SingleCheck = ( iSharedContacts->Find( (*iDBContacts)[i] ) > KErrNotFound ), i );
		OverallResult = OverallResult && SingleCheck;
	
		TESTPRINTI( SingleCheck = ( iSharedEvents->Find( (*iDBEvents)[i] ) > KErrNotFound ), i );
		OverallResult = OverallResult && SingleCheck;	
		}

	return OverallResult;
	}	

void CNotificationStep::PlusTransactionUnkownChangeTestL(const TBool aRevert)
	{
	INFO_PRINTF1(KTest3);
	
	TContactItemId cid = iIterate->NextL();
	TInt i = 0;
	for( ; i < KPlusContacts; ++i )
		{
		OpenL( cid );
		CommitL( EFalse );
		}
		
	TInt many = aRevert ? KPlusContacts : 1;
	for( i = 0; i < many; ++i )
		{
		AddEventL( EContactDbObserverEventContactChanged );
		AddContactL( cid );
		}
	
	cid = iIterate->NextL();
	iContactsDatabase->DatabaseBeginLC(EFalse);
	for( i = 0; i < KMaxNumberOfEventsInEventQueue + 1; ++i )
		{
		OpenL( cid );
		CommitL( ETrue );
		}
	EndTransactionL( aRevert );
	
	if( !aRevert )
		{
		AddEventL( EContactDbObserverEventUnknownChanges );
		AddContactL( -1 );
		iMyActive->iCheck = &CNotificationStep::UnkownChangeCheck;
		}
	else
		{
		iMyActive->iCheck = &CNotificationStep::TransactionUnkownChangeCheck;
		}
	iMyActive->Activate();
	iStart.UniversalTime();
	CActiveScheduler::Start();
	}					
	


