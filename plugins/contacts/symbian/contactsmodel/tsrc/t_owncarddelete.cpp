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


#include <e32std.h>
#include <e32test.h>
#include <e32const.h>
#include <collate.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_utils.h"
#include <badesca.h>

LOCAL_D TInt ThreadFunction(TAny*);

_LIT(KDatabaseFileName, "C:T_OwnCardDelete");
_LIT(KOwnCardMutex, "OwnCardMutex");
_LIT(KOwnCard, "OwnCard");
_LIT(KThreadFunc, "ThreadFunc");

const TInt KTotalContacts  = 4;
const TInt KTimeout = 2000000; 

LOCAL_D TInt gEventCounter = 0;
LOCAL_D TInt gUnknownEventCounter = 0;
// Add*KTotalContacts + 1*OwnCardChanged + 1*UnknownChanges + OwnCardDeleted
const TInt KMaxNumOfEvents = KTotalContacts + 3; 
const TInt KMaxUnknownEvents = 1;


class RTestPtr
    {
public:
    RTestPtr(RTest* aPtr) : iPtr(aPtr) 
        {
        }       
    ~RTestPtr()
        { 
        iPtr->Close();
        delete iPtr; 
        }              
    RTest* operator->()
        {
        return iPtr; 
        }                
    RTest& Ref()
        {
        return *iPtr;
        }                      
private:
    RTest* iPtr;
    };              


class CMyObserver :  public CActive, public MContactDbObserver
    {
public:
    CMyObserver() : CActive(EPriorityIdle)
        {
        iStart.UniversalTime();
        }
    void Activate();
            
private: // From CActive.
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private: // From MContactDbObserver
    void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
    
private:
    TTime     iStart;
    TTime     iEnd;
    };

void CMyObserver::RunL()
    {
    iEnd.UniversalTime();
    // Timeout has been excedeed, stop waiting
    if( iEnd.MicroSecondsFrom(iStart.Int64()) > KTimeout )
        {        
        CActiveScheduler::Stop();
        }
    else // Waiting for next event
        {
        Activate();
        }
    }

void CMyObserver::DoCancel()
    {   
    }

TInt CMyObserver::RunError(TInt aError)
    {
    return aError;
    }

void CMyObserver::Activate()
    {
    if(!IsActive())
        {
        TRequestStatus* pS = &iStatus;
        User::RequestComplete(pS, KErrNone);
        SetActive();
        }
    }


void CMyObserver::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
    {      
    switch( aEvent.iType )
        {
        case EContactDbObserverEventUnknownChanges:
            {
            gUnknownEventCounter++;              
            }break;
        default:
            {                        
            Activate();
            break;
            }
        }            
    gEventCounter++;
    }
    
    
LOCAL_C void WaitForNotificationsL(CContactDatabase* aDb)
    {                
    CMyObserver* observer = new (ELeave) CMyObserver();
    CleanupStack::PushL(observer);
    CContactChangeNotifier* notifier = CContactChangeNotifier::NewL(*aDb, observer);
    CleanupStack::PushL(notifier);
    CActiveScheduler::Add(observer);
    observer->Activate();
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy(notifier);
    CleanupStack::PopAndDestroy(observer);
    }
                
    
LOCAL_C void DeleteMultipleContactsL(CContactDatabase* aDb)
    {
    CContactIdArray* idArray = aDb->ContactsChangedSinceL(TTime(0));
    CleanupStack::PushL(idArray);
    aDb->DeleteContactsL(*idArray);        
    CleanupStack::PopAndDestroy(idArray);
    }

LOCAL_C void DeleteOwnContactL(CContactDatabase* aDb)
    {
    TContactItemId contactId = aDb->OwnCardId();
    aDb->DeleteContactL(contactId);
    }
    
    
LOCAL_C void TestSetOwnCardL(CContactDatabase* aDb)
    {
    CContactIdArray* idArray = aDb->ContactsChangedSinceL(TTime(0));
    CleanupStack::PushL(idArray);
    TInt count = idArray->Count();
    if (count > 0)
        {
        TInt someIndex(0); //Make the first contact own contact           
        TContactItemId id = (*idArray)[someIndex];
        CContactItem* item = aDb->ReadContactLC(id); // take ownership of item
        aDb->SetOwnCardL(*item);
        CleanupStack::PopAndDestroy(item);
        }        
    CleanupStack::PopAndDestroy(idArray);
    }
    
    
LOCAL_C void PopulateDatabaseL(CContactDatabase* aDb, TInt aCntCount)
    {        
    CRandomContactGenerator* randomCnt = CRandomContactGenerator::NewL();
    CleanupStack::PushL(randomCnt);
    randomCnt->SetDbL(*aDb);    
    for (TInt i = 0; i < aCntCount; ++i)
        {
        randomCnt->AddTypicalRandomContactL();        
        }          
    CleanupStack::PopAndDestroy(randomCnt);       
    }
    
    
LOCAL_D void SwitchToMainThread(RMutex& aMutex)
    {    
    RThread::Rendezvous(KErrNone);         
    aMutex.Wait();     
    aMutex.Signal();
    User::After(10);
    }
 
    
LOCAL_D void SwitchToWorkerThread(RMutex& aMutex, RThread& aThread, TBool aSignal, TBool aWait)
    {
    if (aSignal)
        {
        aMutex.Signal();             
        User::After(10);         
        }
        
    if (aWait)
        {            
        aMutex.Wait(); 
        }
        
    TRequestStatus status;            
    aThread.Rendezvous(status);         


    User::WaitForRequest(status);            
    }    
    
LOCAL_D void ThreadFirstTestL(RTest& aTest)
    {
    aTest.Next(_L("-- Running worker thread --"));


    RMutex mutex;
    TInt errMutex = mutex.OpenGlobal(KOwnCardMutex);   
    User::LeaveIfError(errMutex);
    CleanupClosePushL(mutex);
    
    aTest.Printf(_L("Replacing the database \n"));
    CContactDatabase* db = CContactDatabase::ReplaceL(KDatabaseFileName);
    CleanupStack::PushL(db); 
    
    // get a second database object
    CContactDatabase* dbAnother = CContactDatabase::OpenL(KDatabaseFileName);
    CleanupStack::PushL(dbAnother);    
        
    SwitchToMainThread(mutex); // 1) Back to MT-a
    
    ///////////////////////////////////////////////
    // Main thread has now opened the database    
    ///////////////////////////////////////////////
                        
    aTest.Printf(_L("Populating the database \n"));
    PopulateDatabaseL(db, KTotalContacts);    
    WaitForNotificationsL(db);        
                    
    aTest.Printf(_L("Set the Own card id \n"));                    
    TestSetOwnCardL(db);                
    WaitForNotificationsL(db);
    
    aTest.Printf(_L("Checking count value \n"));
    TInt count = db->CountL();    
    TInt countAnother = dbAnother->CountL();
    aTest.Printf(_L("Count: %d \n"), count );
    aTest(count == KTotalContacts && countAnother == KTotalContacts, __LINE__);
    
    aTest.Printf(_L("Checking the id \n"));
    TContactItemId id = db->OwnCardId();
    TContactItemId idCopy = dbAnother->OwnCardId();
    aTest.Printf(_L("Id: %d \n"), id);    
    aTest(id != KNullContactId && idCopy != KNullContactId, __LINE__);                
        
    SwitchToMainThread(mutex); // 2) Back to MT-b 
    
    ///////////////////////////////////////////////
    // Main thread has now checked the added values    
    ///////////////////////////////////////////////    
    
    DeleteMultipleContactsL(db);    
    WaitForNotificationsL(db);         
        
    aTest.Printf(_L("Checking deleted count value \n"));
    count = db->CountL();
    countAnother = dbAnother->CountL();    
    aTest.Printf(_L("Count: %d \n"), count );
    aTest(count == 0 && countAnother == 0, __LINE__);
    
    aTest.Printf(_L("Checking the deleted id \n"));
    id = db->OwnCardId();
    idCopy = dbAnother->OwnCardId();
    aTest.Printf(_L("Id: %d \n"), id);    
    aTest(id == KNullContactId && idCopy == KNullContactId, __LINE__);
            
    SwitchToMainThread(mutex); // 3) Back to MT-c
    
    ///////////////////////////////////////////////
    // Main thread has now checked the deleted values    
    ///////////////////////////////////////////////    
    
    CleanupStack::PopAndDestroy(dbAnother);        
    CleanupStack::PopAndDestroy(db);
    CleanupStack::PopAndDestroy(); // close mutex handle
                
    RThread::Rendezvous(KErrNone); // Finish) back to MT-d
    }          
    
LOCAL_D void TestSecondL(RTest& aTest)
    {
    aTest.Next(_L("-- Second Owncard Test --"));

    
    RMutex mtx;
    TInt errMutex = mtx.CreateGlobal(KOwnCardMutex);
    User::LeaveIfError(errMutex);
    CleanupClosePushL(mtx);    
    
    RThread thd;  
    aTest.Printf(_L("Creating worker thread \n"));  
    TInt errThread = thd.Create(KThreadFunc, ThreadFunction, KDefaultStackSize, KDefaultStackSize*20, KDefaultStackSize*40, NULL);  
    User::LeaveIfError(errThread);
    CleanupClosePushL(thd);
    thd.Resume();    
         
    SwitchToWorkerThread(mtx, thd, EFalse, ETrue); // a) Run: WT-start to WT-1
                  
    ///////////////////////////////////////////////
    // Worker thread has now replaced the database    
    ///////////////////////////////////////////////
        
    aTest.Printf(_L("Open existing database \n"));
    CContactDatabase* db = CContactDatabase::OpenL(KDatabaseFileName);
    CleanupStack::PushL(db);
        
    SwitchToWorkerThread(mtx, thd, ETrue, ETrue); // b) Run: WT-1 to WT-2                    
    
    ///////////////////////////////////////////////
    // Worker thread has now added the contacts
    ///////////////////////////////////////////////
        
    WaitForNotificationsL(db); 
    
    aTest.Printf(_L("Checking db count \n"));          
    TInt count = db->CountL();
    aTest.Printf(_L("Count: %d \n"), count);
    aTest(count == KTotalContacts, __LINE__);
    
    aTest.Printf(_L("Checking owncard id \n"));      
    TContactItemId id = db->OwnCardId();  
    aTest.Printf(_L("id: %d \n"), id);
    aTest(id != KNullContactId, __LINE__);
    
    SwitchToWorkerThread(mtx, thd, ETrue, ETrue); // c) Run: WT-2 to WT-3      
    
    ///////////////////////////////////////////////
    // Worker thread has now deleted the contacts
    ///////////////////////////////////////////////
    
    aTest.Printf(_L("Checking deleted db count \n"));      
        
    WaitForNotificationsL(db); 
    
    count = db->CountL();
    aTest.Printf(_L("Count: %d \n"), count);
    aTest(count == 0, __LINE__);
    
    aTest.Printf(_L("Checking deleted owncard id \n"));      
    id = db->OwnCardId();  
    aTest.Printf(_L("id: %d \n"), id);
    aTest(id == KNullContactId, __LINE__);
                 
    CleanupStack::PopAndDestroy(db);           
        
    SwitchToWorkerThread(mtx, thd, ETrue, EFalse); // d) Run: WT-3 to end
    
    CleanupStack::PopAndDestroy(); // close thd handle
    CleanupStack::PopAndDestroy(); // close mtx handle        
    }
  
    
LOCAL_D void TestFirstL(RTest& aTest)
    {
    aTest.Next(_L("@SYMTestCaseID:PIM-T-OWNCARDDELETE-0001 -- First Owncard Test --"));

    aTest.Printf(_L("Replacing database \n"));
    CContactDatabase* db = CContactDatabase::ReplaceL(KDatabaseFileName);
    CleanupStack::PushL(db); 
    
    CContactDatabase* dbAnother = CContactDatabase::OpenL(KDatabaseFileName);
    CleanupStack::PushL(dbAnother);
        
    PopulateDatabaseL(db, KTotalContacts);        
    WaitForNotificationsL(db);        
                    
    TestSetOwnCardL(db);                
    WaitForNotificationsL(db);        
    
    aTest.Printf(_L("Checking the count value \n"));
    TInt count = dbAnother->CountL();    
    aTest.Printf(_L("Count: %d \n"), count);
    aTest(count == KTotalContacts, __LINE__);
      
    aTest.Printf(_L("Checking the id \n"));    
    TContactItemId id = dbAnother->OwnCardId();    
    aTest.Printf(_L("id: %d \n"), id);
    aTest(id != KNullContactId, __LINE__);
    
    TContactIter iter(*db); //Test that the iterator is updated
    TContactItemId firstContactId;
    firstContactId = iter.FirstL(); //should be own contactId
    
    DeleteOwnContactL(dbAnother);
    WaitForNotificationsL(dbAnother);
    
    TContactItemId secondContactId;
    TRAPD (err,  secondContactId = iter.FirstL());
    aTest(err == KErrNone && firstContactId != secondContactId, __LINE__);
    
    
    DeleteMultipleContactsL(db);    
    WaitForNotificationsL(db); 
    
    aTest.Printf(_L("Checking the deleted count value \n"));
    count = dbAnother->CountL();    
    aTest.Printf(_L("Count: %d \n"), count);
    aTest(count == 0, __LINE__);
      
    aTest.Printf(_L("Checking the deleted id \n"));    
    id = dbAnother->OwnCardId();    
    aTest.Printf(_L("id: %d \n"), id);
    aTest(id == KNullContactId, __LINE__);     
    
    aTest(gEventCounter == KMaxNumOfEvents, __LINE__);
    aTest(gUnknownEventCounter == KMaxUnknownEvents, __LINE__);                             
    
    CleanupStack::PopAndDestroy(2,db);    
    } 
    
    
LOCAL_D void ThreadDoTestsL()
    {
    RTestPtr myTest(new(ELeave) RTest(KThreadFunc));
    
    myTest->Start(KThreadFunc);        
    ThreadFirstTestL(myTest.Ref());    
    myTest->End();    
    }
     

/**
@SYMTestCaseID PIM-T-OWNCARDDELETE-0001
@SYMTestType UT
@SYMTestPriority Medium
@SYMDEF PDEF107246
@SYMTestCaseDependencies CntModel CoreAppsTest
@SYMTestCaseDesc Check that updates to one CContactDatabase-instance will propagate to a second instance
@SYMTestActions Add some contacts. Set one as own card. Check that the count is the same for all instances. 
                Check that the own card id is the same for all instances. Delete all contacts and check that 
                the count is zero and own card id is KNullContactId (-1) for all instances.                
@SYMTestExpectedResults see above.
*/
LOCAL_D void DoTestsL()
    {  
    RTestPtr myTest(new(ELeave) RTest(KOwnCard));
        
    myTest->Start(KOwnCard);          
    TestFirstL(myTest.Ref());                          
    TestSecondL(myTest.Ref());         
    
    CContactDatabase::DeleteDatabaseL(KDatabaseFileName);   
    myTest->Printf(_L("Completed OK \n"));        
    myTest->End();  
    }
    
    
LOCAL_D TInt ThreadFunction(TAny*)
    {      
    __UHEAP_MARK;                     
    CActiveScheduler* scheduler = new CActiveScheduler;
    if (scheduler)
        {        
        CActiveScheduler::Install(scheduler);    
        CTrapCleanup* cleanup = CTrapCleanup::New();
        if (cleanup)
            {                            
            TRAPD(err, ThreadDoTestsL());
            __ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("Failure in Worker Thread"),err));
            delete cleanup;
            }
        delete scheduler;        
        }             
    __UHEAP_MARKEND;                 
    return KErrNone;
    }    
    
           
GLDEF_C TInt E32Main()
    {            
    __UHEAP_MARK;
    CActiveScheduler* scheduler = new CActiveScheduler;
    if (scheduler)
        {    
        CActiveScheduler::Install(scheduler);    
        CTrapCleanup* cleanup = CTrapCleanup::New();
        if (cleanup)
            {
            TRAPD(err, DoTestsL());                            
            __ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("Failure in Main Thread"),err));            
            delete cleanup;
            }
        delete scheduler;
        }    
    __UHEAP_MARKEND;
    return KErrNone;
    }
