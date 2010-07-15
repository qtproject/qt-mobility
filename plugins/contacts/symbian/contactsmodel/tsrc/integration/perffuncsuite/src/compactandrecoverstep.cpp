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
#include "compactandrecoverstep.h"
#include "performancefunctionalitydefs.h"
#include <cntfilt.h>

#define KNumberOfContacts 10

_LIT(KRun1,"CRUD");
_LIT(KRun2,"Transaction");
_LIT(KRun3,"Simple");
_LIT(KRun4,"AsyncOpen");

_LIT(KTest1, "CRUD tests");
_LIT(KTest2, "Transaction tests");
_LIT(KTest3, "Simple operations test");
_LIT(KTest4, "Asynchronous Open recovery test");


_LIT(KCompleteSteps, "Complete all active steps Test");
_LIT(KCancelSteps, "Cancel active steps Test");

_LIT(KRecover, "Recover_" );
_LIT(KComplete, "Complete_" );
_LIT(KCompleteAndRecover, "CNR_" );



CCompactAndRecoverStep::CCompactAndRecoverStep()  : CPerformanceFunctionalityBase(KNumberOfContacts)
	{
	SetTestStepName(KCompactAndRecoverStep);
	}
	
TVerdict CCompactAndRecoverStep::doTestStepPostambleL()
	{
	CLEAR( iContactOpenOperation );	
	return CPerformanceFunctionalityBase::doTestStepPostambleL();
	}
	
void CCompactAndRecoverStep::InitializeL()
	{
	CPerformanceFunctionalityBase::InitializeL();
	iContactsDatabase2 = CContactDatabase::OpenL();
	//set timeout to 0, to minimize waits within the test
	iContactsDatabase->SetOperationTimeOutL(0);
	iContactsDatabase2->SetOperationTimeOutL(0);
	}

void CCompactAndRecoverStep::Cleanup()
	{
	CLEAR( iContactsDatabase2 );
	CPerformanceFunctionalityBase::Cleanup();
	}
	
void CCompactAndRecoverStep::PreTestL()
	{
	const TDesC &KSection = ConfigSection();
	//read recover flag passed in section paramter
	iRecoverFlag = ( KSection.Find( KRecover ) == 0 );
	//read complete flag passed in section paramter
	iCompleteSteps = ( KSection.Find( KComplete ) == 0 );
	
	if( !iRecoverFlag && !iCompleteSteps )
		{
		iRecoverFlag = iCompleteSteps = ( KSection.Find( KCompleteAndRecover ) == 0 );
		}
	}

TVerdict CCompactAndRecoverStep::doTestStepL()
	{
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
	return EPass;
#else	
	__UHEAP_MARK;
	InitializeL();
	_LIT(KPrintStart, "CCompactAndRecoverStep::doTestStepL()");
	INFO_PRINTF1(KPrintStart);  //Block start 
	iIterate->Reset();

	const TDesC &run = ConfigSection();
	
	if( run.Find( KRun1 ) > KErrNotFound )
		{
		INFO_PRINTF1( KTest1 );
		TestWrapperL( &CCompactAndRecoverStep::CRUDOperationsL, KErrNotReady, KErrNotReady, KErrNone, KErrNotReady );
		}
	else if( run.Find( KRun2 ) > KErrNotFound )
		{
		INFO_PRINTF1( KTest2 );
		TestWrapperL(  &CCompactAndRecoverStep::TransactionOperations, KErrNotReady, KErrNotReady, KErrNone, KErrNotReady );
		}
	else if( run.Find( KRun3 ) > KErrNotFound )
		{
		INFO_PRINTF1( KTest3 );
		TestWrapperL(  &CCompactAndRecoverStep::SimpleOperationsL, KErrAccessDenied, KErrNone, KErrNone, KErrNone );
		}
	else if( run.Find( KRun4 ) > KErrNotFound )
		{
		INFO_PRINTF1( KTest4 );
		OpenAsyncTestL();
		}
	else
		{
		MissingTestPanic();
		}
		
	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
#endif
	}
	
/**
Test wrapper for performing all comapct and recover tests
start recover/compress
perfrom database operations, expecting recover/compress errors
end recover compress
perform database operations again, expecting no errors
*/
void CCompactAndRecoverStep::TestWrapperL(
									void (CCompactAndRecoverStep::*aOperations)(const TInt ), 
									const TInt aErr, 
									const TInt aErr2, 
									const TInt aErr3, 
									const TInt aErr4 )
	{
	if(iCompleteSteps)
		{
		INFO_PRINTF1(KCompleteSteps);
		}
	else
		{
		INFO_PRINTF1(KCancelSteps);
		}
	
	//start compress/recover	
	if( iRecoverFlag )
		{
		StartRecoverL();
		}
	else
		{
		StartCompressL();
		}
	
	//perfrom database operations	
	#ifdef _DEBUG
	
	(this->*aOperations)( iRecoverFlag ? aErr : aErr2 );
	(void)aErr3;
	(void)aErr4;
	
	#else
	
	(this->*aOperations)( iRecoverFlag? aErr3 : aErr4 );
	(void)aErr;
	(void)aErr2;
	
	#endif
	
	//end recover / compress
	if( iRecoverFlag )
		{
		EndRecoverL();
		}
	else
		{
		EndCompressL();
		}
		
	CContactDatabase *temp = iContactsDatabase2;
	iContactsDatabase2 = iContactsDatabase;
	iContactsDatabase = temp;
	
	//perfrom database operations expecting no errors
	(this->*aOperations)( KErrNone );
	
	temp = iContactsDatabase2;
	iContactsDatabase2 = iContactsDatabase;
	iContactsDatabase = temp;
	}

/**
Performs and Traps database crud(Create, read, update, delete) operations
all operations should leave with aError
*/
void CCompactAndRecoverStep::CRUDOperationsL(const TInt aError)
	{
	_LIT(KPrintErrors,"Expected error: %d and Observed error: %d");
	TContactItemId con1 = iIterate->NextL();
	CContactItem *contactItem1 = NULL;

	TRAPD( err, 
			contactItem1 = iContactsDatabase->ReadContactLC(con1);
			CleanupStack::PopAndDestroy( contactItem1 );
			);
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);
	
	TRAP( err, 
		contactItem1 = iContactsDatabase->OpenContactLX(con1);
		CleanupStack::PushL(contactItem1);
		iContactsDatabase->CommitContactL(*contactItem1);
		CleanupStack::PopAndDestroy( contactItem1 );
		CleanupStack::Pop( );//lock
		);
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);
	
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	TRAP(err, contactItem1 = iContactsDatabase->UpdateContactLC(con1, contact); 
		  CleanupStack::PopAndDestroy(contactItem1);
	);
	CleanupStack::PopAndDestroy( contact );
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);
	
	TRAP( err, 
			AddEmptyContactsL(1, EFalse);
			);
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);
	
	TRAP( err, 
			DeleteContactL(con1, EFalse);
			);
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);
	}

/**
Performs and Traps database transaction operations
all operations should leave with aError
*/
void CCompactAndRecoverStep::TransactionOperations(const TInt aError)
	{
	_LIT(KPrintErrors,"Expected error: %d and Observed error: %d");
	
	//begin transaction	
	TRAPD(err,	iContactsDatabase->DatabaseBeginL(EFalse); );
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);
	
	//perfrom crud operation
	TRAP_IGNORE( AddEmptyContactsL(1, EFalse) );
	
	//commit transaction
	TRAP(err,	iContactsDatabase->DatabaseCommitL(EFalse); );
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);	
	
	//begin transaction	
	TRAP(err,	iContactsDatabase->DatabaseBeginL(EFalse); );
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);
	
	//perfrom crud operation
	TRAP_IGNORE( AddEmptyContactsL(1, EFalse) );
	
	//rollback transaction
	TRAP(err,	
	        iContactsDatabase->DatabaseRollback();
	        User::After( 100000 );
	         );
	//rollback can't leave
	TESTPRINT( KErrNone == err);
	INFO_PRINTF3(KPrintErrors, KErrNone, err);
	}
	
/**
Performs and Traps simple database operations
all operations should leave with aError
*/
void CCompactAndRecoverStep::SimpleOperationsL(const TInt aError)
	{
	_LIT(KPrintErrors,"Expected error: %d and Observed error: %d");
	
	const TContactItemId KOwnCardId = iContactsDatabase->OwnCardId();
	TESTPRINT( KOwnCardId == -1 );
	
	CCntFilter *filter = CCntFilter::NewL();
	TRAPD(err,	iContactsDatabase->FilterDatabaseL( *filter ); );
	CLEAR( filter );
	TESTPRINT(aError == err);
	INFO_PRINTF3(KPrintErrors, aError, err);
	}

/**
Start asynchronous open while in compress/recover state
end compress/recover state
wait for open to be successfully completed
*/	
void CCompactAndRecoverStep::OpenAsyncTestL()
	{
	_LIT(KOpenAsync, "Open Async");
	_LIT(KEndAsync, "End Async");
	iCompleteSteps = EFalse;
	
	StartRecoverL();
	ALLPRINT(KOpenAsync);
	iContactOpenOperation = CContactDatabase::Open(iStatus);
	EndRecoverL();	
	User::WaitForRequest(iStatus);
	CLEAR( iContactOpenOperation );
	ALLPRINT( KEndAsync );	
	
	StartCompressL();
	ALLPRINT(KOpenAsync);
	iContactOpenOperation = CContactDatabase::Open(iStatus);
	EndCompressL();	
	User::WaitForRequest(iStatus);
	CLEAR( iContactOpenOperation );
	ALLPRINT( KEndAsync );	
	}
	


