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
#include <cntview.h>

#include "cfixedqueue.h"
#include "ccontactvieweventqueue.h"

_LIT(KTestName, "T_FilteredViewUpdate");

_LIT(KTestDbName, "c:T_FilteredViewUpdate.cdb");

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

        static const TInt KTestContacts;
        CContactDatabase* iDb;
        CContactIdArray* iContacts;
        TContactItemId iGroupId;
        CDbEventListener* iDbEventQueue;
        CContactViewEventQueue* iViewEventQueue;
        RContactViewSortOrder iViewSortOrder;
        CContactRemoteView* iRemoteView;
        CContactViewEventQueue* iFilteredViewEventQueue;
        CContactFilteredView* iFilteredView;
    };

const TInt CTestResources::KTestContacts = 8;


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

    CreateTestContactsL();

    iViewEventQueue = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    
    iRemoteView = CContactRemoteView::NewL
        (*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);

    iFilteredViewEventQueue = CContactViewEventQueue::NewL();
    iFilteredView = CContactFilteredView::NewL
        (*iFilteredViewEventQueue, *iDb, *iRemoteView, CContactDatabase::EMailable);

    // Wait for filtered view to get ready
    TContactViewEvent event;
	TBool eventReady = iFilteredViewEventQueue->ListenForEvent(10,event);
	__ASSERT_ALWAYS(eventReady,User::Invariant());
    ASSERT(event.iEventType == TContactViewEvent::EReady);

    // Flush all other events
    iDbEventQueue->Flush();
    iViewEventQueue->Flush();
    iFilteredViewEventQueue->Flush();
    }

void CTestResources::CreateTestContactsL()
    {
    TInt i;
    iContacts = CContactIdArray::NewL();

    // Create contacts
    for (i=1; i <= KTestContacts; ++i)
        {
        CContactCard* card = CContactCard::NewLC();
        CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
        TBuf<30> name;
        name.Format(_L("Contact%02d"), i);
        field->TextStorage()->SetTextL(name);
        card->AddFieldL(*field);
        CleanupStack::Pop(field);
        if (i % 2 == 0)
            {
            // Add an email field to every other contact
            CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldEMail);
            field->TextStorage()->SetTextL(_L("email"));
            card->AddFieldL(*field);
            CleanupStack::Pop(field);
            }
        const TContactItemId contactId = iDb->AddNewContactL(*card);
        iContacts->AddL(contactId);
        CleanupStack::PopAndDestroy(card);

        // Eat away contact db events
        TContactDbObserverEvent event;
        do 
            {
			TBool eventReady = iDbEventQueue->ListenForEvent(10,event);
            __ASSERT_ALWAYS(eventReady,User::Invariant());
            }
        while (event.iType != EContactDbObserverEventContactAdded ||
               event.iContactId != contactId);
        }
    }

CTestResources::~CTestResources()
    {
    if (iFilteredView) iFilteredView->Close(*iFilteredViewEventQueue);
    delete iFilteredViewEventQueue;
    if (iRemoteView) iRemoteView->Close(*iViewEventQueue);
    iViewSortOrder.Close();
    delete iViewEventQueue;
    delete iDbEventQueue;
    delete iContacts;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

LOCAL_C void CheckConsistentL
        (CContactDatabase& aDb,
        const CContactFilteredView& aFilteredView, 
        const CContactViewBase& aBaseView,
        TFieldType aFieldType)
    {
    const TInt filteredViewCount = aFilteredView.CountL();
    test(filteredViewCount <= aBaseView.CountL());

    // Check that filtered view contains all the contacts it should
    // from the base view
    const TInt count = aBaseView.CountL();
    TInt i;
    for (i=0; i < count; ++i)
        {
        const TContactItemId id = aBaseView.AtL(i);
        CContactItem* item = aDb.ReadContactLC(id);
        if (item->CardFields().Find(aFieldType) != KErrNotFound)
            {
			TInt pos = aFilteredView.FindL(id);
            test(pos != KErrNotFound);
            }
        CleanupStack::PopAndDestroy(item);
        }

    // Check that filtered view doesn't contain any contacts it shouldn't contain
    for (i=0; i < filteredViewCount; ++i)
        {
        const TContactItemId id = aFilteredView.AtL(i);
        CContactItem* item = aDb.ReadContactLC(id);
        test(item->CardFields().Find(aFieldType) != KErrNotFound);
        CleanupStack::PopAndDestroy(item);
        }
    }

LOCAL_C void CheckConsistentL(CTestResources& aRes)
    {
    CheckConsistentL(*aRes.iDb, *aRes.iFilteredView, *aRes.iRemoteView, KUidContactFieldEMail);
    }

// Removes the last contact with an email address from the DB. The contact happens
// also to be the last contact in the underlying view
//
// This test crashes because the filtered view accesses its underlying view with an
// out of bounds index.

LOCAL_C void TestDeleteLastEmailContactL()
    {
    test.Next(_L("Delete last email contact"));


    CTestResources* res = CTestResources::NewLC();
    CheckConsistentL(*res);

    const TInt lastEmailContactIndex = res->iFilteredView->CountL() - 1;
    const TContactItemId lastEmailContactId = res->iFilteredView->AtL(lastEmailContactIndex);

    res->iDb->DeleteContactL(lastEmailContactId);

    // Wait for the update event from the filtered view
    TContactViewEvent event;
    test(res->iFilteredViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType==TContactViewEvent::EItemRemoved && event.iInt==lastEmailContactIndex && event.iContactId==lastEmailContactId);
    CleanupStack::PopAndDestroy(res);
	}

// Renames last of the email contacts so that it will move to the beginning of 
// sort order.
//
// This test fails because CContactFilteredView::HandleRemoveEventL messes up the removal
// and sends back an event with incorrect information. With some luck, however, the filtered
// view is consistent after the EItemAdded event arrives.


LOCAL_C void TestRenameLastEmailContactL()
    {
	test.Next(_L("Rename last email contact"));


    CTestResources* res = CTestResources::NewLC();
    CheckConsistentL(*res);

    const TInt lastEmailContactIndex = res->iFilteredView->CountL() - 1;
    const TContactItemId lastEmailContactId = res->iFilteredView->AtL(lastEmailContactIndex);
    CContactItem* item = res->iDb->OpenContactLX(lastEmailContactId);
    CleanupStack::PushL(item);
    CContactItemField& nameField = item->CardFields()[item->CardFields().Find(KUidContactFieldFamilyName)];
    nameField.TextStorage()->SetTextL(_L("AAAFirst"));
    res->iDb->CommitContactL(*item);
    CleanupStack::PopAndDestroy(2);

    // Wait for the update events from the filtered view
    TContactViewEvent event;

	test(res->iFilteredViewEventQueue->ListenForEvent(10,event));
	test(event.iEventType==TContactViewEvent::EItemRemoved && event.iInt==lastEmailContactIndex && event.iContactId==lastEmailContactId);

	test(res->iFilteredViewEventQueue->ListenForEvent(10,event));
	test(event.iEventType==TContactViewEvent::EItemAdded && event.iInt==0 && event.iContactId==lastEmailContactId);

    // Check consistency
    CheckConsistentL(*res);
    
    CleanupStack::PopAndDestroy(res);
    }

LOCAL_C void TestChangeContactL()
    {
	test.Next(_L("Commit and delete"));


    CTestResources* res = CTestResources::NewLC();
    CheckConsistentL(*res);

	// step1 - Open a contact
    const TInt lastEmailContactIndex = res->iFilteredView->CountL() - 1;
    const TContactItemId lastEmailContactId = res->iFilteredView->AtL(lastEmailContactIndex);
    CContactItem* item = res->iDb->OpenContactLX(lastEmailContactId);
    CleanupStack::PushL(item);
	// step2 - Edit a contact field 
    CContactItemField& nameField = item->CardFields()[item->CardFields().Find(KUidContactFieldFamilyName)];
    nameField.TextStorage()->SetTextL(_L("AAAFirst"));
	// step3 - Commit the contact (remove and add in one time)
    res->iDb->CommitContactL(*item);
	// step4 - Delete the new contact
	res->iDb->CloseContactL(lastEmailContactId);
	res->iDb->DeleteContactL(lastEmailContactId);

    CleanupStack::PopAndDestroy(2);

    // Wait for the remove event from the filtered view
    TContactViewEvent event;
   
	test(res->iFilteredViewEventQueue->ListenForEvent(10,event));
	test(event.iEventType==TContactViewEvent::EItemRemoved && event.iInt==lastEmailContactIndex && event.iContactId==lastEmailContactId);

	// Check there are no more events 
	test(!res->iFilteredViewEventQueue->ListenForEvent(10,event));
    // Check consistency
    CheckConsistentL(*res);
    CleanupStack::PopAndDestroy(res);
	}

/**

@SYMTestCaseID     PIM-T-FILTEREDVIEWUPDATE-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-FILTEREDVIEWUPDATE-0001 T_FilteredViewUpdate"));

	TestDeleteLastEmailContactL();
	TestRenameLastEmailContactL();
	TestChangeContactL();
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
	__ASSERT_ALWAYS(err==KErrNone,User::Invariant());

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
    
	__UHEAP_MARKEND;
	
	return err;
    }
