/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <cntitem.h>
#include <cntfldst.h>
#include <cntviewbase.h>

#include "ccontactvieweventqueue.h"


// Constants
_LIT(KTestName, "T_ViewNotificationError");

_LIT(KTestDbName, "c:T_ViewNotificationError.cdb");


/// Global RTest object
LOCAL_D RTest test(KTestName);


/**
 * A helper class for holding the resources needed in the tests.
 */
class CTestResources :
        public CBase, public MContactViewObserver
    {
    public:
        static CTestResources* NewLC();
        void ConstructL();
        void CreateViewL();
        void RemoveFirstContactL();
        ~CTestResources();

        CContactDatabase* iDb;
        CContactViewEventQueue* iViewObserver1;
        CContactViewEventQueue* iViewObserver2;
        CContactViewEventQueue* iViewObserver3;
        RContactViewSortOrder iViewSortOrder;
        CContactLocalView* iAllContactsView;

    private: // From MContactViewObserver.
	    void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);
    };

/**
 * Creates new test resources object.
 */
CTestResources* CTestResources::NewLC()
    {
    CTestResources* self = new(ELeave) CTestResources;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CTestResources::ConstructL()
    {
    // Open test DB
    iDb = CContactDatabase::OpenL(KTestDbName);
    }

void CTestResources::CreateViewL()
    {
    // Create the sort order
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

    // Create a view, assign this class to it's observer
    iAllContactsView = CContactLocalView::NewL
        (*this, *iDb, iViewSortOrder, EContactsOnly);

    // Create three additional view observers
    iViewObserver1 = CContactViewEventQueue::NewL(iAllContactsView);
    iViewObserver2 = CContactViewEventQueue::NewL(iAllContactsView);
    iViewObserver3 = CContactViewEventQueue::NewL(iAllContactsView);
	iViewObserver1->CloseOnItemRemoved();
	iViewObserver2->CloseOnItemRemoved();
	iViewObserver3->CloseOnItemRemoved();
    }

void CTestResources::RemoveFirstContactL()
    {
    iDb->DeleteContactL(1);
    }

void CTestResources::HandleContactViewEvent
        (const CContactViewBase& /*aView*/, const TContactViewEvent& /*aEvent*/)
    {
    // Do nothing
    }

CTestResources::~CTestResources()
    {
    if (iAllContactsView) iAllContactsView->Close(*this);
    iViewSortOrder.Close();
    delete iViewObserver3;
    delete iViewObserver2;
    delete iViewObserver1;
	User::After(1000000);
    delete iDb;
    }


LOCAL_C TContactItemId CreateTestContactL
        (CContactDatabase& aDb, const TDesC& aFamilyName)
    {
    // Create a contact card
    CContactCard* card = CContactCard::NewLC();
    // Create a name field
    CContactItemField* field = CContactItemField::NewLC
        (KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(aFamilyName);
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    // Add the contact to the DB
    const TContactItemId contactId = aDb.AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);
    return contactId;
    }


LOCAL_C CContactIdArray* PopulateTestDbLC(TInt aNumContacts)
    {
    CContactIdArray* ids = CContactIdArray::NewLC();
    CContactDatabase* db = CContactDatabase::ReplaceL(KTestDbName);
    CleanupStack::PushL(db);
    TBuf<50> name;
    _LIT(KNameFormat, "Test%d");
    for (TInt i=1; i <= aNumContacts; ++i)
        {
        name.Format(KNameFormat,i);
        const TContactItemId id = CreateTestContactL(*db,name);
        ids->AddL(id);
        }
    CleanupStack::PopAndDestroy(db);
    return ids;
    }


/**
 * This test demonstrates how a contact view fails to notify its observers
 * if some of the observers deregisters for receiving events. It does not
 * only fail to do that, it also panics the application.
 * There is a design error in CContactViewBase::NotifyObservers.
 */

LOCAL_C void TestViewNotificationL()
    {
    test.Next(_L("Test contact view notification"));


    // Open the populated test DB
    CTestResources* res = CTestResources::NewLC();

    // Create a contact view and three additional
    // observers (please see CTestResources::CreateViewL
    // implementation for extra comments)
    res->CreateViewL();

    // Wait for the first observer to report the view is ready
    TContactViewEvent event;
    test(res->iViewObserver1->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // ..then the second observer...
    test(res->iViewObserver2->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // ..and finally the last observer reports it's ready.
    test(res->iViewObserver3->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // Remove one contact from the db
    res->RemoveFirstContactL();

    // Wait for the first observer to receive the remove event...
    test(res->iViewObserver1->ListenForEvent(10,event));
/** 
 *  After the above line Contact Model crashes to
 *  USER 130 panic, EBadArrayIndex.
 *
 *  The reason is that the observer deregistered itself
 *  from receiving contact view events when it got
 *  EItemRemoved event. The for loop in
 *  CContactViewBase::NotifyObservers can't handle that
 *  situation.
 */
    test(event.iEventType == TContactViewEvent::EItemRemoved);

    // Then second observer should receive the event
    test(res->iViewObserver2->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EItemRemoved);

    // And finally the third observer should receive the event
    test(res->iViewObserver3->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EItemRemoved);
    CleanupStack::PopAndDestroy(res);
    }

/**

@SYMTestCaseID     PIM-T-VIEWNOTIFICATIONERROR-0001

*/


LOCAL_C void DoTestsL()
    {
    // Init
    CleanupClosePushL(test);
	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEWNOTIFICATIONERROR-0001 T_ViewNotificationError"));


    // Populate a test DB
    const TInt KNumTestContacts = 100;
    CContactIdArray* testContactIds = PopulateTestDbLC(KNumTestContacts);

    // Execute the tests
    TestViewNotificationL();

    // Cleanup
    CleanupStack::PopAndDestroy(testContactIds);
    test.End();
    CleanupStack::PopAndDestroy();  // test.Close()
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
