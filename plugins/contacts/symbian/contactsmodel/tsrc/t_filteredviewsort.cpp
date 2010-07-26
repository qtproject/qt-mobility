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
* FilteredViewSort Test module
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


_LIT(KTestName, "T_FilteredViewSorting");

_LIT(KTestDbName, "c:T_FilteredViewSorting.cdb");

_LIT(KLinefeed, " \n");
_LIT(KNoContacts, "No contacts\n");
_LIT(KLocalViewOrder, "CContactLocalView ordering:");
_LIT(KFilteredViewOrder, "CContactFilteredView ordering:");

// Following lists the names used in the test,
// constants are named alphabetically (the first being KName1)
_LIT(KName1, "Aho");
_LIT(KName2, "Alho");
_LIT(KName3, "Granholm");
_LIT(KName4, "Jokinen");
_LIT(KName5, "Kontulainen");
_LIT(KName6, "Koskinen");
_LIT(KName7, "Lampinen");
_LIT(KName8, "Rosendahl");
_LIT(KName9, "Smolander");
_LIT(KName10, "Tanabe");
_LIT(KName11, "Tukiainen");



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
        CContactViewEventQueue* iFilteredViewEventQueue;
        CContactFilteredView* iFilteredView;
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
    
    iLocalView = CContactLocalView::NewL
        (*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);

    iFilteredViewEventQueue = CContactViewEventQueue::NewL();
    // Create a filtered view which contains contacts with phone numbers
    iFilteredView = CContactFilteredView::NewL
        (*iFilteredViewEventQueue, *iDb, *iLocalView, CContactDatabase::EPhonable);

    // Wait for filtered view to get ready
    TContactViewEvent event;
    test(iFilteredViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EReady); // this line is failing.

    // Flush all other events
    iDbEventQueue->Flush();
    iViewEventQueue->Flush();
    iFilteredViewEventQueue->Flush();
    }

TContactItemId CTestResources::CreateTestContactL
        (const TDesC& aFamilyName, TBool aWithPhoneNumber)
    {
    // Create a contact card
    CContactCard* card = CContactCard::NewLC();
    // Create a name field
    CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(aFamilyName);
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    // Create a phone number field
    if (aWithPhoneNumber)
        {
        // Create a phone number field
        CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPhoneNumber);
        field->TextStorage()->SetTextL(_L("555"));
        card->AddFieldL(*field);
        CleanupStack::Pop(field);
        }
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
    if (iFilteredView) iFilteredView->Close(*iFilteredViewEventQueue);
    delete iFilteredViewEventQueue;
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    iViewSortOrder.Close();    
    delete iViewEventQueue;
    delete iDbEventQueue;
    delete iDb;

    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

// Adds some contacts to the db
LOCAL_C void AddContactsL(CTestResources* aRes, TBool aPhonable)
    {
	aRes->CreateTestContactL(KName4,  aPhonable);
	aRes->CreateTestContactL(KName7,  aPhonable);
	aRes->CreateTestContactL(KName1,  aPhonable);
	aRes->CreateTestContactL(KName5,  aPhonable);
	aRes->CreateTestContactL(KName2,  aPhonable);
	aRes->CreateTestContactL(KName11, aPhonable);
	aRes->CreateTestContactL(KName9,  aPhonable);
	aRes->CreateTestContactL(KName3,  aPhonable);
	aRes->CreateTestContactL(KName6,  aPhonable);
	aRes->CreateTestContactL(KName8,  aPhonable);
	aRes->CreateTestContactL(KName10, aPhonable);
    }

// Helper function
LOCAL_C TPtrC GetContactNameL(CTestResources* aRes, TInt aIndex)
	{
	const CViewContact& viewContact	= aRes->iFilteredView->ContactAtL(aIndex);
	return viewContact.Field(0);
	}

// Test are the contacts in order
LOCAL_C void TestFilteredContactsOrderL(CTestResources* aRes)
	{
	// Should write a better tester which compares the results
	// of contacts ordering using localview and filteredview, but
	// this one have to do for now.. this fails also and that's
	// the point

	// Test are there exact number of matches
	test(aRes->iFilteredView->CountL() == 11);		// pass

	// Test that contacts are in order
	TInt i=0;
	test (GetContactNameL(aRes, i) == KName1);		// fails!

	++i;
	test (GetContactNameL(aRes, i) == KName2);

	++i;
	test (GetContactNameL(aRes, i) == KName3);

	++i;
	test (GetContactNameL(aRes, i) == KName4);

	++i;
	test (GetContactNameL(aRes, i) == KName5);

	++i;
	test (GetContactNameL(aRes, i) == KName6);

	++i;
	test (GetContactNameL(aRes, i) == KName7);

	++i;
	test (GetContactNameL(aRes, i) == KName8);

	++i;
	test (GetContactNameL(aRes, i) == KName9);

	++i;
	test (GetContactNameL(aRes, i) == KName10);

	++i;
	test (GetContactNameL(aRes, i) == KName11);
	}

// Shows the contacts in order
LOCAL_C void ShowFilteredContactsL(CTestResources* aRes)
	{
	if (aRes->iFilteredView->CountL() < 1)
		{
		test.Console()->Printf(KNoContacts);
		}
	else
		{
		// Show the local view ordering first
		test.Console()->Printf(KLinefeed);
		test.Console()->Printf(KLocalViewOrder);
		test.Console()->Printf(KLinefeed);
		for (TInt i=0; i<aRes->iLocalView->CountL(); ++i)
			{
			const CViewContact& viewContact = aRes->iLocalView->ContactAtL(i);

			// Assume that the name is in the first field
			test.Console()->Printf(viewContact.Field(0));
			test.Console()->Printf(KLinefeed);
			}

		// Pause execution, until user presses a key
		//test.Console()->Getch();

		// Then show filtered view ordering
		test.Console()->Printf(KLinefeed);
		test.Console()->Printf(KFilteredViewOrder);
		test.Console()->Printf(KLinefeed);
		for (TInt j=0; j<aRes->iFilteredView->CountL(); ++j)
			{
			const CViewContact& viewContact = aRes->iFilteredView->ContactAtL(j);

			test.Console()->Printf(viewContact.Field(0));
			test.Console()->Printf(KLinefeed);
			}
		}

		// Pause execution, until user presses a key
		//test.Console()->Getch();
	}

/**

@SYMTestCaseID     PIM-T-FILTEREDVIEWSORT-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-FILTEREDVIEWSORT-0001 T_FilteredViewSorting"));


	// Create test resources
	CTestResources* res = CTestResources::NewLC();

	// First, create phonable contacts
	AddContactsL(res, ETrue);

	// Show the contacts ordering (see that the results are wrong)
	ShowFilteredContactsL(res);

	// Do the test (prove that the results are wrong)
	TestFilteredContactsOrderL(res);	// Fails!

	// Cleanup
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
