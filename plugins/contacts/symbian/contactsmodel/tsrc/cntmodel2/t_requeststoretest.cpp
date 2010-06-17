/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
@SYMTestCaseID          PIM-APPENG-CNTMODEL-CNTSRV-CT-106277-0001
@SYMTestCaseDesc        Request store testing.  Ensure the request store can handle failed requests.
@SYMTestType            CIT
@SYMTestPriority        High
@SYMTestActions         1. Create thread 1 signaling a start of transaction to lock the 
                           default database. 
                        2. Create thread 2 to continuously sends a large number of invalid delete 
                           requests to the database.
                        3. After a specific amount of time elapsed, signaling a commit from thread 1
                           to release the database.                          
@SYMTestExpectedResults Test should completed without error or panic.
@SYMTestStatus          Implemented
@SYMDEF                 DEF106277 - CntModel - CRequestStore attempts to double-delete a CCntRequest

Note: T_RequestStoreTest is created base on T_DBTransactionTest with minor modifications to suit 
      this test.  
*/

#include <e32base.h>

#include "nbcnttestlib.h"
#include "t_requeststoretest.h"

_LIT(KTestName, "T_RequestStoreTest"); // Used by Testhelper - gives an warning under ARMv5

#include "testhelpers.h"

// Adjust KNumContacts to allow pre-population of contact database if
// pre-existing contact items are needed in the database.
const TInt KNumContacts = 10;

// Number of delete request to be made 
const TInt KDeleteRequest = 500;

// adjust KLockPeriod to ensure there's enough lock-down time
// to force requests from other threads be stored in the Request
// store.
const TInt KLockPeriod = 10000000;

//===========================================================
// CLockDatabase Class
//===========================================================

CLockDatabase* CLockDatabase::NewLC()
    {
    CLockDatabase* self = new (ELeave) CLockDatabase();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CLockDatabase::~CLockDatabase()
    {
    if (iContactDatabase)
        delete iContactDatabase;
    iTest->End();
    iTest->Close();
    delete iTest;
    }

CLockDatabase::CLockDatabase()
    : CActive(EPriorityIdle)
    {
    CActiveScheduler::Add(this);
    }

void CLockDatabase::ConstructL()
    {
    TPtrC name(RThread().Name());
    iTest = new(ELeave) RTest(name);
    iTest->Start(_L("T_LockDatabase starts ..."));
    OpenDatabaseL();
    }

void CLockDatabase::OpenDatabaseL()
    {
    iContactDatabase = CContactDatabase::OpenL(CContactDatabase::EMultiThread);

    TRequestStatus *pS = &iStatus;
    User::RequestComplete(pS, KErrNone);
    SetActive();
    CActiveScheduler::Start();
    }

void CLockDatabase::DoCancel()
    {
    CActiveScheduler::Stop();
    }

TInt CLockDatabase::RunError(TInt aError)
    {
    return aError;
    }

void CLockDatabase::RunL()
    {
    if (!iLocked)
        {
        iTest->Printf(_L("T_LockDatabase - Lock database ..."));
        iContactDatabase->DatabaseBeginL(EFalse);

        // When requests are sent to the Contact database server during transaction(lock-down)
        // state, all the requests sent from other threads are being held in the Request Store
        // waiting to be processed upon the very next change of state.
        //
        // Therefore, in order to ensure the request store gets any request, ensure there's
        // enough lock-down time before unlocking the contact database again.
        User::After(KLockPeriod);

        TRequestStatus *pS = &iStatus;
        User::RequestComplete(pS, KErrNone);
        SetActive();
        iLocked = ETrue;
        }
    else
        {
        iTest->Printf(_L("T_LockDatabase - Unlock database ..."));
        iContactDatabase->DatabaseCommitL(ETrue);
        iLocked = EFalse;
        CActiveScheduler::Stop();
        }
    }


//===========================================================
// CDeleteInvalidCnt Class
//===========================================================

CDeleteInvalidCnt* CDeleteInvalidCnt::NewLC()
    {
    CDeleteInvalidCnt* self = new (ELeave) CDeleteInvalidCnt();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CDeleteInvalidCnt::~CDeleteInvalidCnt()
    {
    if (iContactDatabase)
        delete iContactDatabase;
    iTest->End();
    iTest->Close();
    delete iTest;
    }

CDeleteInvalidCnt::CDeleteInvalidCnt()
    : CActive(EPriorityIdle),
      iStep(-1)
    {
    CActiveScheduler::Add(this);
    }

void CDeleteInvalidCnt::ConstructL()
    {
    TPtrC name(RThread().Name());
    iTest = new(ELeave) RTest(name);
    iTest->Start(_L("T_DeleteInvalidCnt starts"));

    OpenDatabaseL();
    }

void CDeleteInvalidCnt::OpenDatabaseL()
    {
    iContactDatabase = CContactDatabase::OpenL(CContactDatabase::EMultiThread);

    iStep++;
    iTest->Printf(_L("T_DeleteInvalidCnt - Proceed to CContactDatabase::DeleteContactL() in bulk..."));

    TRequestStatus *pS = &iStatus;
    User::RequestComplete(pS, KErrNone);
    SetActive();
    CActiveScheduler::Start();
    }

void CDeleteInvalidCnt::DoCancel()
    {
    CActiveScheduler::Stop();
    }

TInt CDeleteInvalidCnt::RunError(TInt aError)
    {
    return aError;
    }

void CDeleteInvalidCnt::RunL()
    {
    if (iStep < KDeleteRequest)
        {
        // always delete a non-existent contact item to force a failure
        TContactItemId invalidCnt = iStep + iContactDatabase->CountL() + 1;
        TRAP_IGNORE(iContactDatabase->DeleteContactL(invalidCnt));
        /*
        CContactItem* item = NULL;
        TRAP_IGNORE(item = iContactDatabase->OpenContactL(1));
        if (item)
            delete item;        */
        
        iTest->Printf(_L("T_DeleteInvalidCnt - Issued request CContactDatabase::DeleteContactL(%d)..."),
                      invalidCnt);
//        User::After(200000);
        iStep++;

        TRequestStatus *pS = &iStatus;
        User::RequestComplete(pS, KErrNone);
        SetActive();
        }
    else
        {
        iTest->Printf(_L("T_DeleteInvalidCnt - All requests completed..."));
        CActiveScheduler::Stop();
        }
    }


//===========================================================
// CConcurrentController Class
//===========================================================

CConcurrentController::~CConcurrentController()
    {
    iStore.ResetAndDestroy();
    }

CConcurrentController* CConcurrentController::NewLC()
    {
    CConcurrentController* self = new (ELeave) CConcurrentController();
    CleanupStack::PushL(self);
    return self;
    }

CConcurrentController::CConcurrentController()
    {}

void CConcurrentController::StartTestL()
    {
    // Create some entries into the contact database for use
    CreateContactsL(KNumContacts);

    // test to verify the RequestStore is properly handling a failed
    // request
    StartRequestStoreFailureTestL();

    // In the future, more tests should be added to verify the handling of
    // requests within the Request Store under different situations
    // 
    // In order to add new tests:
    // 
    // 1. Use CLockDatabase as the thread to force requests to be stored
    //    within the Request Store
    // 2. Create a new class(es) using CDeleteInvalidCnt as a template for 
    //    send requests which satisfy the desired test condition (eg.
    //    leave, panic, success, timeout)
    // 3. Create a new function using StartRequestStoreFailureTestL to
    //    trigger the threads to run
    // 4. Add the new function in Step 3 here to include it as one of the 
    //    the Request Store test 
    }

void CConcurrentController::StartRequestStoreFailureTestL()
    {
    test.Printf(_L("Starting Request Store Failure Test ..."));

    CreateTestersL(2);

    // Create a thread for locking the database while the other
    // thread attempts to delete contact items that does not exists

    // Thread 1. Start sending delete requests which are guaranteed to fail
    iStore[0]->RunTestThreadL(EDeleteInvalidCnt);

    // Thread 2. Trigger a lock-down of the database and unlock it after a while
    iStore[1]->RunTestThreadL(ELockDatabase);

    // While Thread 1 continues to send delete requests, the database server
    // will be locked down temporily by Thread 2.  During lock-down time,
    // the database server will store all incoming requests, from a different thread
    // compare to the one locking down the server, into the request store.  After a
    // specific time elapsed, Thread 2 will unlock the database, causing a state change
    // in the database server.  Upon this very first state change, all requests stored
    // in the Request Store will be retrieved and processed within the store's own
    // Active Loop and they will fail.
    while (!Completed())
        User::WaitForAnyRequest();
    }

void CConcurrentController::CreateTestersL(TInt aNoOfTesters)
    {
    // flush all pre-existing CConcurrentTester instances
    iStore.ResetAndDestroy();

    if (iStore.Count() == 0)
        {
        _LIT(KThreadName,"Thread_%d");
        TBuf<256> buf;

        for (TInt ii = 0; ii < aNoOfTesters; ++ii)
            {
            buf.Format(KThreadName, ii);
            iStore.AppendL(CConcurrentTester::NewL(CEventResponse::NewL(*this), buf));
            }
        }
    }

TBool CConcurrentController::Completed()
    {
    TInt noOfTesters = iStore.Count();
    for (TInt ii = 0; ii < noOfTesters; ++ii)
        {
        if (iStore[ii]->IsCompleted())
            return EFalse;
        }
    return ETrue;
    }

void CConcurrentController::PublishError(TInt aError)
    {
    test.Printf(_L(" ->  TEST FAILED - Error %d was reported during execution"), aError);
    }

void CConcurrentController::CreateContactsL(TInt aNumContacts)
    {
    // flush all existing entries, and create a blank new one
    CContactDatabase* db = CContactDatabase::ReplaceL();
    CleanupStack::PushL(db);

    CContactItemViewDef* matchAll
        = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,
                                     CContactItemViewDef::EIncludeHiddenFields);
    matchAll->AddL(KUidContactFieldMatchAll);

    CContactItem* item = db->ReadContactL(db->TemplateId(), *matchAll);

    CleanupStack::PopAndDestroy(matchAll);

    CContactTemplate* cntTemplate = static_cast<CContactTemplate*>(item);
    CleanupStack::PushL(cntTemplate);

    CCntItemBuilder* builder = CCntItemBuilder::NewLC(*cntTemplate);

    for (TInt i = 0; i < aNumContacts; i++)
        {
        // Create the contact
        CContactItem* cntItem = builder->GetCntItemLC();

        TRAP_IGNORE(db->AddNewContactL(*cntItem));

        CleanupStack::PopAndDestroy(cntItem);
        }

    CleanupStack::PopAndDestroy(builder);
    CleanupStack::PopAndDestroy(cntTemplate);
    CleanupStack::PopAndDestroy(db);
    }

//===========================================================
// CEventResponse Class
//===========================================================

CEventResponse* CEventResponse::NewL(CConcurrentController& aController)
    {
    CEventResponse* self = new (ELeave) CEventResponse(aController);
    return self;
    }


CEventResponse::CEventResponse(CConcurrentController& aController) : CActive(EPriorityIdle), iController(aController)
    {
    CActiveScheduler::Add(this);
    }


CEventResponse::~CEventResponse()
    {}

void CEventResponse::RunL()
    {
    test.Next(_L("-> Thread Finished"));

    }

void CEventResponse::DoCancel()
    {
    Cancel();
    }

TInt CEventResponse::RunError(TInt aError)
    {
    iController.PublishError(aError);
    return aError;
    }

void CEventResponse::CompleteRequest(TInt aError)
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aError);
    }

TRequestStatus& CEventResponse::RequestStatus()
    {
    return iStatus;
    }


//===========================================================
// CConcurrentTester Class
//===========================================================

CConcurrentTester::~CConcurrentTester()
    {
    delete iResponse;
    iThread.Close();
    }

CConcurrentTester* CConcurrentTester::NewL(CEventResponse* aResponse, const TDesC& aThreadName)
    {
    CConcurrentTester* self = new (ELeave) CConcurrentTester(aResponse, aThreadName);
    return self;
    }

TInt CConcurrentTester::ThreadFunction(TAny* aTester)
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if (!cleanup)
    	{
        return KErrNoMemory;
		}

    CActiveScheduler*  scheduler = new CActiveScheduler;
    CActiveScheduler::Install(scheduler);

    TRAPD(err, static_cast<CConcurrentTester*>(aTester)->RunTestL());

    delete scheduler;
    scheduler = NULL;
    delete cleanup;
    cleanup   = NULL;

    return err;
    }

TBool CConcurrentTester::IsCompleted()
    {
    return (iResponse->RequestStatus() == KRequestPending);
    }

// Wrapper method for Test class RunTestL()
// This method is called from within the ThreadFunction() method
// and runs in a new thread. Apart from this function all the concurrent
// logic is in the CTransactionThreadTest class.

void CConcurrentTester::RunTestL()
    {
    switch(iTestCode)
        {
        case ELockDatabase:
            {
            CLockDatabase* lockTest = CLockDatabase::NewLC();
            CleanupStack::PopAndDestroy(lockTest);
            lockTest = NULL;
            break;
            }
        case EDeleteInvalidCnt:
            {
            CDeleteInvalidCnt* deleteTest = CDeleteInvalidCnt::NewLC();
            CleanupStack::PopAndDestroy(deleteTest);
            deleteTest = NULL;
            break;
            }
        default:
            {
            User::Leave(KErrNotFound);
            }
        }
    }


void CConcurrentTester::RunTestThreadL(TInt aTestCode)
    {
    test.Next(_L("-> %s Started"));


    iTestCode = aTestCode;
    User::LeaveIfError(iThread.Create(iThreadName,
                                      CConcurrentTester::ThreadFunction,
                                      KDefaultStackSize,
                                      0x2000,
                                      0x200000,
                                      this,
                                      EOwnerThread));

    iThread.SetPriority(EPriorityMuchLess);
    iThread.Logon (iResponse->RequestStatus());
    iThread.Resume();
    }
   

LOCAL_C void DoTestsL()
    {
    CleanupClosePushL(test);

    test.Start(_L("@SYMTESTCaseID:PIM-APPENG-CNTMODEL-CNTSRV-CT-106277-0001 ----------- Request Store tests begins ----------"));


    CConcurrentController* testController = CConcurrentController::NewLC();
    testController->StartTestL();

    test.Printf(_L (" ----------- Request Store tests ends ---------- "));
    test.End();

    CleanupStack::PopAndDestroy(2);     // testController & test
    }

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    CActiveScheduler* scheduler=new CActiveScheduler;
    if (scheduler)
        {
        CActiveScheduler::Install(scheduler);
        CTrapCleanup* cleanup = CTrapCleanup::New();
        if (cleanup)
            {
            TRAPD(err,DoTestsL());
            __ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("DBDumper Failed"),err) );
            delete cleanup;
            }
        delete scheduler;
        }

    __UHEAP_MARKEND;
    return KErrNone;
    }
