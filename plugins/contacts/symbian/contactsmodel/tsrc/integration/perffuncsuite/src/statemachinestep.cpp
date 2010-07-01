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

#include "statemachinestep.h"
#include "performancefunctionalitydefs.h"
#include <cntfilt.h>

const static TInt KNumberOfContacts = 10;

_LIT(KTestCase1,"OpeningRecovery");
_LIT(KTestCase2,"OpeningAsync");
_LIT(KTestCase3,"Writable");
_LIT(KTestCase4,"DefaultRecovery");
_LIT(KTestCase5,"DefaultCompress");
_LIT(KTestCase6,"AsyncInteg");
_LIT(KTestCase7,"TablesClosed");
_LIT(KTestCase8,"WritableSpeed");

_LIT(KTestCasePrint1, "Opening State statemachine Recovery tests");
_LIT(KTestCasePrint2, "Opening State statemachine Asynchronous open tests");
_LIT(KTestCasePrint3, "Writable State statemachine tests");
_LIT(KTestCasePrint4, "Default State recovery statemachine tests");
_LIT(KTestCasePrint5, "Default State compression statemachine tests");
_LIT(KTestCasePrint6, "Asynchronous Integrity tests");
_LIT(KTestCasePrint7, "Tables Closed State statemachine tests");
_LIT(KTestCasePrint8, "Writable State statemachine speed dial tests");

/**
 * This class tests the internal implementation of the contact model server
 * specifically it is designed to stress test the state machine
 * so that all the major functionality of the state machine is exercised
 * each test function usually encompasses one test case, the description for
 * the test case is defined in the related test script file (StateMachineStep.script)
 * each test step in the test script file usually call this test class with a non
 * existent ini file section name, the section name correspond to the test case
 * name stored within this class. The reasoning behind this is that each test case
 * does not need to store any additional data within the ini file and as such a reference
 * to an actual ini file section would be redundent.
 */
CStateMachineStep::CStateMachineStep()
                        : CPerformanceFunctionalityBase( KNumberOfContacts )
	{
	SetTestStepName(KStateMachineStep);
	}

TVerdict CStateMachineStep::doTestStepPostambleL()
	{
	return CPerformanceFunctionalityBase::doTestStepPostambleL();
	}

void CStateMachineStep::InitializeL()
	{
	CPerformanceFunctionalityBase::InitializeL();
	//set timeout to 0, to minimize waits within the test
	iContactsDatabase->SetOperationTimeOutL(0);
	}

void CStateMachineStep::Cleanup()
	{
    //unload async opens
    CLEAR( iContactOpenOperation1 );
    CLEAR( iContactOpenOperation2 );
	CPerformanceFunctionalityBase::Cleanup();
	}

void CStateMachineStep::PreTestL()
	{
	//does nothing
	}

TVerdict CStateMachineStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KPrintStart, "CStateMachineStep::doTestStepL()");
	INFO_PRINTF1(KPrintStart);  //Block start
	iIterate->Reset();

	const TDesC &KSection = ConfigSection();

	if( KSection ==  KTestCase1 )
		{
		INFO_PRINTF1( KTestCasePrint1 );
		OpeningStateRecoverTestsL();
		}
	else if( KSection ==  KTestCase2 )
		{
		INFO_PRINTF1( KTestCasePrint2 );
		OpeningStateAsyncOpenTestsL();
		}
	else if( KSection ==  KTestCase3 )
		{
		INFO_PRINTF1( KTestCasePrint3 );
		WritableStateTestsL();
		}
	else if( KSection ==  KTestCase4 )
		{
		INFO_PRINTF1( KTestCasePrint4 );
		DefaultStateCompressionTestsL();
		}
	else if( KSection ==  KTestCase5 )
		{
		INFO_PRINTF1( KTestCasePrint5 );
		DefaultStateRecoveryTestsL();
		}
	else if( KSection ==  KTestCase6 )
		{
		INFO_PRINTF1( KTestCasePrint6 );
		AsynchronousIntegrityTestL();
		}
	else if( KSection ==  KTestCase7 )
		{
		INFO_PRINTF1( KTestCasePrint7 );
		TablesClosedStateTestsL();
		}
	else if( KSection ==  KTestCase8 )
		{
		INFO_PRINTF1( KTestCasePrint8 );
		WritableStateSpeedDialTestsL();
		}
	else
		{
		MissingTestPanic();
		}

	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();

	}

void CStateMachineStep::OpeningStateRecoverTestsL()
    {
	TContactItemId con1 = iIterate->NextL();
    CContactDatabase *database = CContactDatabase::OpenL();

    iCompleteSteps = ETrue;
    TInt error = KErrNone;
    
    //transition to opening state
    TRAP( error, StartRecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    
    //open tables while in opening state
    TRAP( error, database->OpenTablesL() );
    #ifdef _DEBUG
    	#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
    	INTCOMPARE( KErrNone, ==, error, 0, 0 );
    	#else
      	INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
     	#endif     
    #else
    INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
    #endif
    
    //transition out of opening state
    TRAP( error, EndRecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );
    CLEAR( database );

    iCompleteSteps = ETrue;
    //transition to opening state
    TRAP( error, StartRecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    //transition out of opening state, completing all recovery steps
    TRAP( error, EndRecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );

    iCompleteSteps = EFalse;
    //transition to opening state
    TRAP( error, StartRecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    //transition out of opening state, cancelling recovery
    TRAP( error, EndRecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );

    iCompleteSteps = EFalse;
    //transition to opening state
    TRAP( error, StartRecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    //can't access contacts as database is in recovery state
    TRAP( error,
            ReadL(con1);
            CloseL( EFalse );
        );

    #ifdef _DEBUG
    	#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	    INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
    	#else
	    INTCOMPARE( KErrNone, ==, error, 0, 0 );
     	#endif     
    #else
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    #endif

    TRAP( error,
            OpenL(con1);
            CloseL( ETrue );
        );
    #ifdef _DEBUG
    	#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	    INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
    	#else
	    INTCOMPARE( KErrNone, ==, error, 0, 0 );
     	#endif         
    #else
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    #endif
    
    //transition out of opening state, cancelling recovery
    TRAP( error, EndRecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );

    //after recovery is complete it should be possible to access contact normally
    TRAP( error,
            ReadL(con1);
            CloseL( EFalse );
        );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    TRAP( error,
            OpenL(con1);
            CloseL( ETrue );
        );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );

    }

void CStateMachineStep::OpeningStateAsyncOpenTestsL()
    {
    CContactDatabase *database1 = NULL;
    CContactDatabase *database2 = NULL;

    CLEAR( iContactsDatabase );
    WaitForServerToExitL();

    //open databases asynchronously *2
    iContactOpenOperation1 = CContactDatabase::Open(iStatus1);
    iContactOpenOperation2 = CContactDatabase::Open(iStatus2);
    //cancel second async open first
    CLEAR( iContactOpenOperation2 );
    CLEAR( iContactOpenOperation1 );
    //check that async opens were cancelled
    if( iStatus1.Int() != KErrNone )
        {
        INTCOMPARE( KErrCancel, ==, iStatus1.Int(), 0, 0 );
        }
    if( iStatus2.Int() != KErrNone )
        {
        INTCOMPARE( KErrCancel, ==, iStatus2.Int(), 0, 0 );
        }

    //open databases asynchronously *2
    iContactOpenOperation1 = CContactDatabase::Open(iStatus1);
    iContactOpenOperation2 = CContactDatabase::Open(iStatus2);
    //cancel first async open
    CLEAR( iContactOpenOperation1 );
    User::WaitForRequest(iStatus2);
    //get opened database sessions
    database1 = iContactOpenOperation2->TakeDatabase();
    //check that databases have been opened
    INTCOMPARE( reinterpret_cast<TInt>(database1), !=, NULL, 0, 0 );
    //check that first async open was cancelled
    INTCOMPARE( KErrCancel, ==, iStatus1.Int(), 0, 0 );
    //check that second async opened was successful
    INTCOMPARE( KErrNone, ==, iStatus2.Int(), 0, 0 );
    //unload opened database sessions
    CLEAR( database1 );
    CLEAR( iContactOpenOperation2 );

    WaitForServerToExitL();
    //open databases asynchronously *2
    iContactOpenOperation1 = CContactDatabase::Open(iStatus1);
    iContactOpenOperation2 = CContactDatabase::Open(iStatus2);
    //wait for async opens to complete
    User::WaitForRequest(iStatus1);
    User::WaitForRequest(iStatus2);

    //get opened database sessions
    database1 = iContactOpenOperation1->TakeDatabase();
    database2 = iContactOpenOperation2->TakeDatabase();

    //check that databases have been opened
    INTCOMPARE( reinterpret_cast<TInt>(database1), !=, NULL, 0, 0 );
    INTCOMPARE( reinterpret_cast<TInt>(database2), !=, NULL, 0, 0 );
    //unload async opens
    CLEAR( iContactOpenOperation1 );
    CLEAR( iContactOpenOperation2 );
    //unload opened database sessions
    CLEAR( database1 );
    CLEAR( database2 );
    //check that async opens completed successfully
    INTCOMPARE( KErrNone, ==, iStatus1.Int(), 0, 0 );
    INTCOMPARE( KErrNone, ==, iStatus2.Int(), 0, 0 );


    iContactsDatabase = CContactDatabase::OpenL();
   	#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
   		iContactsDatabase->DamageDatabaseL(0x666);
 	#endif
    CLEAR( iContactsDatabase );

    WaitForServerToExitL();
    iActiveRecoverObserver.iCompressOrRecoverError = KErrNone;

    //open databases asynchronously
    iContactOpenOperation1 = CContactDatabase::Open(iStatus1);
    //get partially opened database session
    database1= iContactOpenOperation1->TakeDatabase();
    //start async recovery
    CContactActiveRecover*  recover = database1->CreateRecoverLC();
	recover->SetObserver( &iActiveRecoverObserver );
	//complete async recovery
	while(recover->Step()){}
	CleanupStack::PopAndDestroy( recover );
	recover = NULL;

    //unload async opens
    CLEAR( iContactOpenOperation1 );
    //unload opened database sessions
    CLEAR( database1 );

    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );

    iContactsDatabase = CContactDatabase::OpenL();
    }

void CStateMachineStep::WritableStateTestsL()
    {
	const TContactItemId KContactId = iIterate->NextL();
	const TContactItemId KContactId2 = iIterate->NextL();
    TInt error = KErrNone;

    //set contact as own card
    OpenL( KContactId );
	TRAP( error, iContactsDatabase->SetOwnCardL( *iContactItem ) );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    CommitL( ETrue );
    //check contact is set as own card
    INTCOMPARE( iContactsDatabase->OwnCardId(), ==, KContactId, 0, 0 );

    //set contact as own card again, to test internal functionality of state machine
    OpenL( KContactId );
	TRAP( error, iContactsDatabase->SetOwnCardL( *iContactItem ) );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    CommitL( ETrue );
    //contact is still marked as own card
    INTCOMPARE( iContactsDatabase->OwnCardId(), ==, KContactId, 0, 0 );

    //read contact
    ReadL( KContactId2 );
    //set contact as own card using read contact
	TRAP( error, iContactsDatabase->SetOwnCardL( *iContactItem ) );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    CloseL( EFalse );
    //check that contact is set as own card
    INTCOMPARE( iContactsDatabase->OwnCardId(), ==, KContactId2, 0, 0 );

    //Perform recovery while in writeable state and database not damaged
    TRAP( error,
            //start async recovery
            CContactActiveRecover*  recover = iContactsDatabase->CreateRecoverLC();
        	recover->SetObserver( &iActiveRecoverObserver );
        	//complete async recovery
        	while(recover->Step()){}
        	CleanupStack::PopAndDestroy( recover );
        	recover = NULL;
        );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );

    TRAP( error, iContactsDatabase->RecoverL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    }

void CStateMachineStep::WritableStateSpeedDialTestsL()
    {
    _LIT(KFieldValue, "FieldValue");
	const TContactItemId KContactId = iIterate->NextL();
    TInt error = KErrNone;
    iActiveRecoverObserver.iCompressOrRecoverError = KErrNone;

    OpenL( KContactId );
    const TInt KFieldCount = iFields->Count();
    CloseL( ETrue );


	OpenL( KContactId );
    
    //try setting an empty field as a SpeedDial
    //no error should ensue
    TRAP( error, iContactsDatabase->SetFieldAsSpeedDialL(*iContactItem, 1, 1) );

    INTCOMPARE( KErrNone, ==, error, 0, 0 );

    //write some test data into all fields of the contact item
    //parameter is used only for Text and Store type fields - others are created on the fly
    SetAllFieldsL(KFieldValue);
    CommitL( EFalse );    

    for( TInt i = 0; i < KFieldCount; ++i)
        {
        OpenL( KContactId );
        TRAP( error, iContactsDatabase->SetFieldAsSpeedDialL(*iContactItem, i, 1) );

        //if field is text, set speed dial should be successful
        if( (*iFields)[i].StorageType() == KStorageTypeText)
            {
            CloseL( ETrue );
            INTCOMPARE( KErrNone, ==, error, i, 0 );
            ReadL( KContactId );
            TESTPRINTI( (*iFields)[i].IsSpeedDial(), i );
            CloseL( EFalse );
            }
        //if field not text, set speed dial should fail
        else
            {
            CloseL( ETrue );
            INTCOMPARE( KErrArgument, ==, error, i, 0 );
            ReadL( KContactId );
            TESTPRINTI( !(*iFields)[i].IsSpeedDial(),  i );
            CloseL( EFalse );
            }
        }

    ReadL( KContactId );
    //set speed dial for unopened/read contact
    TRAP( error, iContactsDatabase->SetFieldAsSpeedDialL(*iContactItem, 1, 1) );
    CloseL( EFalse );
    //set speed dial should fail
    INTCOMPARE( KErrAccessDenied, ==, error, 0, 0 );
    ReadL( KContactId );
    TESTPRINT( !(*iFields)[1].IsSpeedDial() );
    CloseL( EFalse );

    //set speed dial for system template - expect error here
    CContactCard* contact = CContactCard::NewLC( iTemplate );
    TRAP( error, iContactsDatabase->SetFieldAsSpeedDialL(*contact, 1, 1) );
    INTCOMPARE( KErrArgument, ==, error, 0, 0 );
    CleanupStack::PopAndDestroy(contact);

    //set speed dial for contact with empty field set - expect error here too
    contact = CContactCard::NewLC();
    TRAP( error, iContactsDatabase->SetFieldAsSpeedDialL(*contact, 1, 1) );
    INTCOMPARE( KErrArgument, ==, error, 0, 0 );
    CleanupStack::PopAndDestroy(contact);
    }

/**
Perform operations that are not supported by current state, so that default state is used
all operations should be unsuccessfull
*/
void CStateMachineStep::DefaultStateCompressionTestsL()
    {
	const TContactItemId KContactId = iIterate->NextL();
    TInt error = KErrNone;

	OpenL( KContactId );
    //transition into compress state
    StartCompressL();

    //perform unsupported operations
    TRAP( error, iContactsDatabase->CloseTables() );//non leaving function
    INTCOMPARE( KErrNone, ==, error, 0, 0 );

    TRAP( error, iContactsDatabase->OpenTablesL() );
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
   	INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
#else
   	INTCOMPARE( KErrNone, ==, error, 0, 0 );
#endif
   	
    TRAP( error, iContactsDatabase->RecoverL() );
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
   	INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
#else
   	INTCOMPARE( KErrNone, ==, error, 0, 0 );
#endif

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	// only do this with the DBMS version of the contacts model as DamageDatabaseL
	// doesn't do anything in the SQLite version and so the test will always fail.
    TRAP( error, iContactsDatabase->DamageDatabaseL(0x666); );
    INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
#endif

    TRAP( error,
        CContactActiveRecover*  recover = iContactsDatabase->CreateRecoverLC();
        CleanupStack::PopAndDestroy(recover);
        );
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
   	INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
#else
   	INTCOMPARE( KErrNone, ==, error, 0, 0 );
#endif
    
	TRAP( error, iContactsDatabase->SetOwnCardL( *iContactItem ) );
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
   	INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
#else
   	INTCOMPARE( KErrNone, ==, error, 0, 0 );
#endif
      	
    TRAP( error, iContactsDatabase->SetFieldAsSpeedDialL(*iContactItem, 1, 1) );
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
   	INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
#else
   	INTCOMPARE( KErrInUse, ==, error, 0, 0 );
#endif
   	
    EndCompressL();
    //end compress state

    CloseL( ETrue );


    }

/**
Perform operations that are not supported by current state, so that default state is used
all operations should be unsuccessfull
*/
void CStateMachineStep::DefaultStateRecoveryTestsL()
    {
    TInt error = KErrNone;

    //transition into opening state
    StartRecoverL();

    //perform usupported operations
    TRAP( error, iContactsDatabase->CompactL() );

    #ifdef _DEBUG
    	#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
    	INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
    	#else
      	INTCOMPARE( KErrNone, ==, error, 0, 0 );
     	#endif     
    #else
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    #endif


    TRAP( error,
            CContactActiveCompress*  compress = iContactsDatabase->CreateCompressorLC();
            CleanupStack::PopAndDestroy( compress );
     );

    #ifdef _DEBUG
    	#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
    	INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
    	#else
      	INTCOMPARE( KErrNone, ==, error, 0, 0 );
     	#endif     
    #else
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    #endif

    EndRecoverL();
    //end opening stat
    }

void CStateMachineStep::AsynchronousIntegrityTestL()
    {
	const TContactItemId KContactId = iIterate->NextL();
    TInt error = KErrNone;

    //test that it is possible to create multiple active compressor
    iActiveRecoverObserver.iCompressOrRecoverError = KErrNone;
    CContactActiveCompress*  compress1 = iContactsDatabase->CreateCompressorLC();
	compress1->SetObserver( &iActiveRecoverObserver );
    CContactActiveCompress*  compress2 = iContactsDatabase->CreateCompressorLC();
	compress2->SetObserver( &iActiveRecoverObserver );
    while(compress1->Step()){}
    while(compress2->Step()){}
    CleanupStack::PopAndDestroy( compress2 );
    CleanupStack::PopAndDestroy( compress1 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );


    //test that it is possible to create multiple active recoverers
    iActiveRecoverObserver.iCompressOrRecoverError = KErrNone;
    CContactActiveRecover*  recover1 = iContactsDatabase->CreateRecoverLC();
	recover1->SetObserver( &iActiveRecoverObserver );
    CContactActiveRecover*  recover2 = iContactsDatabase->CreateRecoverLC();
	recover2->SetObserver( &iActiveRecoverObserver );
    while(recover1->Step()){}
    while(recover2->Step()){}
    CleanupStack::PopAndDestroy( recover2 );
    CleanupStack::PopAndDestroy( recover1 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );

    //transition into compress state

    iCompleteSteps = EFalse;
    StartCompressL();
    TRAP( error,
            CContactActiveRecover*  recover = iContactsDatabase->CreateRecoverLC();
            CleanupStack::PopAndDestroy( recover );
        );
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
#else    
    INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
#endif   

    EndCompressL();
    //end compress state

    //test that we have transitioned out of compress state
    TRAP( error, ReadL( KContactId ) );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    CloseL( EFalse );

    iCompleteSteps = EFalse;
    //transition into opening state
    StartRecoverL();
    TRAP( error,
            CContactActiveCompress*  compress = iContactsDatabase->CreateCompressorLC();
            CleanupStack::PopAndDestroy( compress );
        );

    #ifdef _DEBUG
   	    #ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
   		INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
   		#else
   		INTCOMPARE( KErrNone, ==, error, 0, 0 );
   		#endif     
    #else
    	INTCOMPARE( KErrNone, ==, error, 0, 0 );
    #endif
    
    EndRecoverL();
    //transition out of opening state

    //test that we have transitioned out of compress state
    TRAP( error, ReadL( KContactId ) );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    CloseL( EFalse );
    }

void CStateMachineStep::TablesClosedStateTestsL()
    {
	const TContactItemId KContactId = iIterate->NextL();
    TInt error = KErrNone;

    iActiveRecoverObserver.iCompressOrRecoverError = KErrNone;
    //transition to tables closed state
    iContactsDatabase->CloseTables();


    TRAP( error,
            CContactActiveCompress*  compress = iContactsDatabase->CreateCompressorLC();
            compress->SetObserver( &iActiveRecoverObserver );
            CleanupStack::PopAndDestroy( compress );//dont complete all steps
     );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );

    //test that we are still in tables closed state
    TRAP( error, ReadL( KContactId ) );
    INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
    CloseL( EFalse );

    TRAP( error,
            CContactActiveCompress*  compress = iContactsDatabase->CreateCompressorLC();
            compress->SetObserver( &iActiveRecoverObserver );
            while(compress->Step()){}//complete all steps
            CleanupStack::PopAndDestroy( compress );
     );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    INTCOMPARE( KErrNone, ==, iActiveRecoverObserver.iCompressOrRecoverError, 0, 0 );

    //test that we are still in tables closed state
    TRAP( error, ReadL( KContactId ) );
    INTCOMPARE( KErrNotReady, ==, error, 0, 0 );
    CloseL( EFalse );

    //transition to writeable state
    CContactDatabase *database = NULL;
    TRAP( error, database = CContactDatabase::OpenL() );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );

    //test that we have transitioned out of tables closed state
    TRAP( error, ReadL( KContactId ) );
    INTCOMPARE( KErrNone, ==, error, 0, 0 );
    CloseL( EFalse );

    CLEAR( database );

    }
