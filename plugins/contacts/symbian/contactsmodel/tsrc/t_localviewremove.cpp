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
* LocalViewRemove Test module
*
*/



#include <e32test.h>
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntviewbase.h>

#include "ccontactvieweventqueue.h"
#include "ccontactdbeventqueue.h"

_LIT(KTestName, "T_LocalViewRemove");

_LIT(KTestDbName, "c:T_LocalViewRemove.cdb");

LOCAL_D RTest test(KTestName);

class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC();
        void ConstructL();
        TContactItemId CreateTestContactL(const TDesC& aFamilyName, TBool aWithPhoneNumber);
        ~CTestResources();

        CContactDatabase* iDb;
        TContactItemId iGroupId;
        CContactDbEventQueue* iDbEventQueue;
        CContactViewEventQueue* iViewEventQueue;
        RContactViewSortOrder iViewSortOrder;
        CContactLocalView* iLocalView;
        TContactItemId iTestContactId;
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
    iDbEventQueue = CContactDbEventQueue::NewL(iDb);

    iViewEventQueue = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

	// Create contact view preferences, UNSORTED AT THE END!
	TContactViewPreferences preferences =  static_cast<TContactViewPreferences>
        (EContactsOnly | EUnSortedAtEnd | ESingleWhiteSpaceIsEmptyField);

    // Create the view
    iLocalView = CContactLocalView::NewL
        (*iViewEventQueue, *iDb, iViewSortOrder, preferences);

    // Wait for view to get ready
    TContactViewEvent event;
    test(iViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // Flush all other events
    iDbEventQueue->Flush();
    iViewEventQueue->Flush();
    }

TContactItemId CTestResources::CreateTestContactL
        (const TDesC& aFamilyName, TBool aWithName)
    {
    // Create a contact card
    CContactCard* card = CContactCard::NewLC();
    
	// Create a name field
	if (aWithName)
		{
		CContactItemField* field = CContactItemField::NewLC(KStorageTypeText,
			KUidContactFieldFamilyName);
		field->TextStorage()->SetTextL(aFamilyName);
		card->AddFieldL(*field);
		CleanupStack::Pop(field);
		}

    // Create a phone number field
	CContactItemField* field = CContactItemField::NewLC(KStorageTypeText,
		KUidContactFieldPhoneNumber);
    field->TextStorage()->SetTextL(_L("1013"));
    card->AddFieldL(*field);
    CleanupStack::Pop(field);

    // Add the contact to the DB
    const TContactItemId contactId = iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);

    // Eat away contact db events
    TContactDbObserverEvent event;
    while 
        (iDbEventQueue->ListenForEvent(10,event) && 
        (event.iType != EContactDbObserverEventContactAdded ||
        event.iContactId != contactId))
        {
        }
    iTestContactId = contactId;
    return contactId;
    }

CTestResources::~CTestResources()
    {
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    iViewSortOrder.Close();
    delete iViewEventQueue;
    delete iDbEventQueue;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

LOCAL_C void DoAddContactL(CTestResources* aRes, const TDesC& aFamilyName)
    {
	if (aFamilyName.Compare(KNullDesC) == 0)
		{
		aRes->CreateTestContactL(aFamilyName, EFalse);
		}
	else
		{
		aRes->CreateTestContactL(aFamilyName, ETrue);
		}
    }

LOCAL_C void AddContactWithNameL(CTestResources* aRes)
    {
    DoAddContactL(aRes, _L("Mulder"));
    }

LOCAL_C void AddContactWithoutNameL(CTestResources* aRes)
    {
	DoAddContactL(aRes, KNullDesC);
    }

// This test passes

LOCAL_C void TestRemoveContactWithNameL(CTestResources* aRes)
    {
    test.Next(_L("Remove a contact with name"));


	// First add the contact which will be soon deleted.
	// Let's use name Scully, which will be ordered to
	// be the 11th name in the view (there are 10 Mulders and
	// 5 unnamed contacts)
    DoAddContactL(aRes, _L("Scully"));

	// Flush the view event queue
    aRes->iViewEventQueue->Flush();

    // Now delete the added contact
    aRes->iDb->DeleteContactL(aRes->iTestContactId);

    // Then wait for the local view event
    TContactViewEvent viewEvent;
    test(aRes->iViewEventQueue->ListenForEvent(10,viewEvent));

    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == aRes->iTestContactId);
	// Now the important test: was the deleted contact 11th contact
	// in the view (the iInt index of the contact should be 10)?
    test(viewEvent.iInt == 10);								// <---- success!
    }

// This test fails

LOCAL_C void TestRemoveContactWithoutNameL(CTestResources* aRes)
    {
    test.Next(_L("Remove a contact without name"));


	// First add the contact which will be soon deleted.
	// The names list contains now 10 Mulders and 5 unnamed
	// contacts. There's no real sort order for unnamed contacts,
	// this contact will be 11th - 16th contact in the view,
	// because unsorted contacts are placed to the end of the
	// view (see CTestResources::ConstructL)
    DoAddContactL(aRes, KNullDesC);

	// Flush the view event queue
    aRes->iViewEventQueue->Flush();

    // Now delete the added contact
    aRes->iDb->DeleteContactL(aRes->iTestContactId);

    // Then wait for the local view event
    TContactViewEvent viewEvent;
    test(aRes->iViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == aRes->iTestContactId);
	// Now the important test: was the deleted contact 11th, 12th,
	// 13th, 14th, 15th, or 16th contact in the view? Each one of
	// these values is acceptable, since unnamed contacts are not
	// sorted but are placed at the end of the view
	// (EUnSortedAtEnd preference is in use). This means, that
	// the index of the deleted contact should be in range of 10-15.
    test((viewEvent.iInt >= 10) && (viewEvent.iInt <= 15));		// <---- fail!

	// Test fails, because the iInt index is 5. So according to the
	// received event the deleted unnamed contact was 6th contact
	// in a view which contained 10 named contacts and 6 unnamed
	// contacts and where the unnamed contacts were at the end
	// of the view (EUnSortedAtEnd). Which is not true, the event
	// is wrong.
    }

LOCAL_C void TestUnsortedAtBeginningL(CTestResources* aRes)
	{
	// Now, switch the unsorted to the beginning, and make sure that we
	// get a good response when we remove a name (both sorted and unsorted).
	// Create contact view preferences, UNSORTED AT THE BEGINNING!
	TContactViewPreferences preferences =  static_cast<TContactViewPreferences>
        (EContactsOnly | EUnSortedAtBeginning | ESingleWhiteSpaceIsEmptyField);
    // Create the view
    if (aRes->iLocalView) aRes->iLocalView->Close(*(aRes->iViewEventQueue));
    aRes->iLocalView = CContactLocalView::NewL
        (*(aRes->iViewEventQueue), *(aRes->iDb), aRes->iViewSortOrder, preferences);
    // Wait for view to get ready
    TContactViewEvent event;
    test(aRes->iViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady);
    // Flush all other events
    aRes->iDbEventQueue->Flush();
    aRes->iViewEventQueue->Flush();
	// Now remove a Mulder.
    DoAddContactL(aRes, _L("Scully"));

	// Flush the view event queue
    aRes->iViewEventQueue->Flush();

    // Now delete the added contact
    aRes->iDb->DeleteContactL(aRes->iTestContactId);

    // Then wait for the local view event
    TContactViewEvent viewEvent;
    test(aRes->iViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == aRes->iTestContactId);
    test(viewEvent.iInt > 4);

	// Now remove one without a name
    DoAddContactL(aRes, KNullDesC);

	// Flush the view event queue
    aRes->iViewEventQueue->Flush();

    // Now delete the added contact
    aRes->iDb->DeleteContactL(aRes->iTestContactId);

    // Then wait for the local view event
    test(aRes->iViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == aRes->iTestContactId);
	}

/**

@SYMTestCaseID     PIM-T-LOCALVIEWREMOVE-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-LOCALVIEWREMOVE-0001 T_LocalViewRemove"));


	// Create test resources
    CTestResources* res = CTestResources::NewLC();

	// First add 10 contacts with name Mulder
	for (TInt i=0; i<10; ++i)
	{
		AddContactWithNameL(res);
	}

	// Then add 5 contacts without name
	for (TInt j=0; j<5; ++j)
	{
	    AddContactWithoutNameL(res);
	}

	// Do the tests
    TestRemoveContactWithNameL(res);	// passes
    TestRemoveContactWithoutNameL(res);	// fails
	TestUnsortedAtBeginningL(res);

	CleanupStack::PopAndDestroy(res);

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
