/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntviewbase.h>

#include "cfixedqueue.h"
#include "ccontactvieweventqueue.h"

_LIT(KTestName, "T_LocalViewDuplicates");

_LIT(KTestDbName, "c:T_LocalViewDuplicates.cdb");

LOCAL_D RTest test(KTestName);


class CDbEventListener : public CTimer, public MContactDbObserver
    {
    public:
        static CDbEventListener* NewL
            (CContactDatabase& aDb, TInt aMaxQueueSize=128);
        ~CDbEventListener();

        /**
         * Waits for an event from the database.
         * @param aTimeOut  max time to wait for an event.
         * @param aEvent    the received event, undefined if this function returns false.
         * @return true if an event was received, false if timeout expired first.
         */
        TBool ListenForEvent(TTimeIntervalSeconds aTimeOut, TContactDbObserverEvent& aEvent);

        /**
         * Removes all previously arrvied events from the queue.
         */
        void Flush()
            {
            iEventQueue.Reset();
            }

    private:  // from CTimer
        void RunL();

    private:  // from MContactDbObserver
        void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

    private:  // Implementation
        CDbEventListener();
        void ConstructL(CContactDatabase& aDb, TInt aMaxQueueSize);

    private:  // Data
        CContactChangeNotifier* iContactChangeNotifier;
        CFixedQueue<TContactDbObserverEvent> iEventQueue;
    };

CDbEventListener* CDbEventListener::NewL
        (CContactDatabase& aDb, TInt aMaxQueueSize/*=128*/)
    {
    CDbEventListener* self = new(ELeave) CDbEventListener;
    CleanupStack::PushL(self);
    self->ConstructL(aDb,aMaxQueueSize);
    CleanupStack::Pop(self);
    return self;
    }

CDbEventListener::~CDbEventListener()
    {
    delete iContactChangeNotifier;
    CTimer::Cancel();
    }

TBool CDbEventListener::ListenForEvent(TTimeIntervalSeconds aTimeOut, TContactDbObserverEvent& aEvent)
    {
    CTimer::Cancel();

    if (iEventQueue.IsEmpty())
        {
        CTimer::After(aTimeOut.Int() * 1000000);
        // Keep execution here until the timer expires
        do  {
            CActiveScheduler::Start();
            }
        while (CTimer::IsActive());
        }

    if (!iEventQueue.IsEmpty())
        {
        aEvent = iEventQueue.Head();
        iEventQueue.PopHead();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

void CDbEventListener::RunL()
    {
    // Timer expired
    CActiveScheduler::Stop();
    }

void CDbEventListener::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
    {
    const TBool timerWasActive = CTimer::IsActive();
    CTimer::Cancel();
	TBool eventPushed = iEventQueue.Push(aEvent);
    __ASSERT_ALWAYS(eventPushed,User::Invariant());
    if (timerWasActive)
        {
        CActiveScheduler::Stop();
        }
    }

CDbEventListener::CDbEventListener()
    : CTimer(CActive::EPriorityStandard)
    {
    }

void CDbEventListener::ConstructL
        (CContactDatabase& aDb, TInt aMaxQueueSize)
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    iEventQueue.ConstructL(aMaxQueueSize);
    iContactChangeNotifier = CContactChangeNotifier::NewL(aDb, this);
    }

class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC();
        void ConstructL();
        void CreateTestContactsL();
        ~CTestResources();

        CContactDatabase* iDb;
        CDbEventListener* iDbEventQueue;
        RContactViewSortOrder iViewSortOrder;
    };

CTestResources* CTestResources::NewLC()
    {
    CTestResources* self = new(ELeave) CTestResources;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CTestResources::ConstructL()
    {
    iDb = CContactDatabase::ReplaceL(KTestDbName);
    iDbEventQueue = CDbEventListener::NewL(*iDb);
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    }

CTestResources::~CTestResources()
    {
    iViewSortOrder.Close();
    delete iDbEventQueue;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

LOCAL_C void CreateTestContactsL(CTestResources& aRes, TInt aCount)
    {
    for (TInt i=1; i <= aCount; ++i)
        {
        CContactCard* card = CContactCard::NewLC();
        CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
        TBuf<30> name;
        name.Format(_L("Contact%02d"), i);
        field->TextStorage()->SetTextL(name);
        card->AddFieldL(*field);
        CleanupStack::Pop(field);
        aRes.iDb->AddNewContactL(*card);
        CleanupStack::PopAndDestroy(card);
        }
    }

LOCAL_C void EatDbEvents(CTestResources& aRes)
    {
    TContactDbObserverEvent event;
    while (aRes.iDbEventQueue->ListenForEvent(2,event))
        {
        }
    }

LOCAL_C void TestNoDuplicatesL(const CContactViewBase& aView)
    {
    const TInt count = aView.CountL();
    for (TInt i=0; i < count; ++i)
        {
        const TContactItemId id = aView.AtL(i);
        for (TInt i2=i+1; i2 < count; ++i2)
            {
            test(aView.AtL(i2) != id);
            }
        }
    }

LOCAL_C void TestOpenLocalViewL
        (CTestResources& aRes, TInt aCount)
    {
    // Create a local view on the DB
    CContactViewEventQueue* viewEvents = CContactViewEventQueue::NewL();
    CleanupStack::PushL(viewEvents);
    CContactLocalView* testView = CContactLocalView::NewL
        (*viewEvents, *aRes.iDb, aRes.iViewSortOrder, EContactsOnly);
    CleanupStack::PushL(testView);

    // Wait for the view to initialize
    TContactViewEvent event;
    test(viewEvents->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // Check view count is correct
    test(testView->CountL() == aCount);

    // Check view doesn't contain any duplicates
    TestNoDuplicatesL(*testView);

    CleanupStack::PopAndDestroy(2,viewEvents);
    }

// This test runs fine

LOCAL_C void TestOpenLocalViewWithoutPendingEventsL()
    {
    test.Next(_L("Open local view with pending events from the DB FLUSHED"));


    CTestResources* res = CTestResources::NewLC();
    const TInt KContactCount = 10;

    // Create the test contacts
    CreateTestContactsL(*res, KContactCount);

    // Eat all outstanding DB events
    EatDbEvents(*res);

    // Open local view on the DB
    TestOpenLocalViewL(*res, KContactCount);

    CleanupStack::PopAndDestroy(res);
    }

// This test fails: view contact count is 2*DB contact count, each contact is 
// twice in the view!!!

LOCAL_C void TestOpenLocalViewWithPendingEventsL()
    {
    test.Next(_L("Open local view with pending events from the DB NOT FLUSHED"));


    CTestResources* res = CTestResources::NewLC();
    const TInt KContactCount = 10;

    // Create the test contacts
    CreateTestContactsL(*res, KContactCount);

    // Don't Eat all outstanding DB events
    //EatDbEvents(*res);

    // Open local view on the DB
    TestOpenLocalViewL(*res, KContactCount);

    CleanupStack::PopAndDestroy(res);
    }

/**

@SYMTestCaseID     PIM-T-LOCALVIEWDUPLICATES-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-LOCALVIEWDUPLICATES-0001 T_LocalViewDuplicates"));


    TestOpenLocalViewWithoutPendingEventsL();
    TestOpenLocalViewWithPendingEventsL();

    test.End();
    test.Close(); 
    }

GLDEF_C TInt E32Main()
	{
    // Init
    __UHEAP_MARK; 

    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }

    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        {
        return KErrNoMemory;
        }
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    TRAPD(err, DoTestsL());

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
    __UHEAP_MARKEND;
	return err;
    }
