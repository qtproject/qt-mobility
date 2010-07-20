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
* ViewSort Test module
* NB the Reverse View tests use the CntSimpleSortPlugin DLL
*
*/



#include <e32test.h>
#include <cntviewbase.h>
#include <cntitem.h>
#include <cntfldst.h>

#include "ccontactvieweventqueue.h"

_LIT(KTestName, "T_ViewSort");

_LIT(KTestDbName, "c:T_ViewSort.cdb");

// data to find SimpleSortPlugin DLL
_LIT8(KReverseSortPlugin, "application.vnd.symbian.com/contacts-reverse-sort");

LOCAL_D RTest test(KTestName);


class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC();
        ~CTestResources();

		TContactItemId AddNewContactL(const TDesC& aGivenName, const TDesC& aFamilyName=KNullDesC, const TDesC& aCompanyName=KNullDesC);
		void CreateTestOneContactsL();
		void CreateTestTwoContactsL();
		void CreateTestThreeContactsL();
		void CreateTestFourContactsL();
		void PrintViewSortOrderL();
		void PrintReverseViewSortOrderL();

		void StartTestOneL();
		void StartTestTwoL();
		void StartTestThreeL();
		void StartTestFourL();
		void VerifySortOrderL();
		// for verifying reverse order view
		void VerifyReverseSortOrderL();

        CContactDatabase* iDb;
        CContactViewEventQueue* iViewEventQueue;
		CContactViewEventQueue* iViewEventQueueReverse;
        RContactViewSortOrder iViewSortOrder;
        CContactLocalView* iLocalView;
        CContactLocalView* iLocalReverseView;
		RArray<TContactItemId> iIdsInViewSortOrder;
		RArray<TContactItemId> iIdsInViewReverseSortOrder;
    };

CTestResources* CTestResources::NewLC()
    {
    CTestResources* self = new(ELeave) CTestResources;
    CleanupStack::PushL(self);
    return self;
    }

void CTestResources::StartTestOneL()
	{
	// Create an empty database
	iDb = CContactDatabase::ReplaceL(KTestDbName);
	CreateTestOneContactsL();
	
    test.Next(_L("Starting Test 1"));

	
	iViewEventQueue = CContactViewEventQueue::NewL();
	iViewEventQueueReverse = CContactViewEventQueue::NewL();

	iViewSortOrder.AppendL(KUidContactFieldGivenName);
	iViewSortOrder.AppendL(KUidContactFieldFamilyName);
	iViewSortOrder.AppendL(KUidContactFieldCompanyName);

	iLocalView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);
	TRAPD(err, iLocalReverseView = CContactLocalView::NewL(*iViewEventQueueReverse, *iDb, iViewSortOrder, EContactsOnly, KReverseSortPlugin));
	// KErrNotFound means Sort Plug-in was not found
	test(err == KErrNone);

	// Wait 5 seconds for the view to get ready
	TContactViewEvent event;
	test(iViewEventQueue->ListenForEvent(5,event));
	test(event.iEventType == TContactViewEvent::EReady);

    // Wait 5 seconds for the reverse view to get ready
    test(iViewEventQueueReverse->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);
    }

void CTestResources::StartTestTwoL()
	{
	// Create an empty database
    iDb = CContactDatabase::ReplaceL(KTestDbName);
	CreateTestTwoContactsL();

	test.Next(_L("Starting Test 2"));

	
    iViewEventQueue = CContactViewEventQueue::NewL();
	iViewEventQueueReverse = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

    iLocalView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);
	TRAPD(err, iLocalReverseView = CContactLocalView::NewL(*iViewEventQueueReverse, *iDb, iViewSortOrder, EContactsOnly, KReverseSortPlugin));
	// KErrNotFound means Sort Plug-in was not found
	test(err == KErrNone);

    // Wait 5 seconds for the view to get ready
    TContactViewEvent event;
    test(iViewEventQueue->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // Wait 5 seconds for the reverse view to get ready
    test(iViewEventQueueReverse->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);
    }

void CTestResources::StartTestThreeL()
	{
	// Create an empty database
    iDb = CContactDatabase::ReplaceL(KTestDbName);
	CreateTestThreeContactsL();

	test.Next(_L("Starting Test 3"));


    iViewEventQueue = CContactViewEventQueue::NewL();
	iViewEventQueueReverse = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

    iLocalView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);
	TRAPD(err, iLocalReverseView = CContactLocalView::NewL(*iViewEventQueueReverse, *iDb, iViewSortOrder, EContactsOnly, KReverseSortPlugin));
	// KErrNotFound means Sort Plug-in was not found
	test(err == KErrNone);

    // Wait 5 seconds for the view to get ready
    TContactViewEvent event;
    test(iViewEventQueue->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // Wait 5 seconds for the reverse view to get ready
    test(iViewEventQueueReverse->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);
    }

void CTestResources::StartTestFourL()
	{
	// Create an empty database
    iDb = CContactDatabase::ReplaceL(KTestDbName);
	CreateTestFourContactsL();

	test.Next(_L("Starting Test 4"));


    iViewEventQueue = CContactViewEventQueue::NewL();
	iViewEventQueueReverse = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

    iLocalView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);
	TRAPD(err, iLocalReverseView = CContactLocalView::NewL(*iViewEventQueueReverse, *iDb, iViewSortOrder, EContactsOnly, KReverseSortPlugin));
	// KErrNotFound means Sort Plug-in was not found
	test(err == KErrNone);

    // Wait 5 seconds for the view to get ready
    TContactViewEvent event;
    test(iViewEventQueue->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);

    // Wait 5 seconds for the reverse view to get ready
    test(iViewEventQueueReverse->ListenForEvent(5,event));
    test(event.iEventType == TContactViewEvent::EReady);
    }

CTestResources::~CTestResources()
    {
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    if (iLocalReverseView) iLocalReverseView->Close(*iViewEventQueueReverse);
    iViewSortOrder.Close();
    delete iViewEventQueue;
	delete iViewEventQueueReverse;
	iIdsInViewSortOrder.Close();
	iIdsInViewReverseSortOrder.Close();

	delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

TContactItemId CTestResources::AddNewContactL(const TDesC& aGivenName, const TDesC& aFamilyName, const TDesC& aCompanyName)
 	{
	CContactItem* card=CContactCard::NewLC();

	// Add first, last and company name fields
	CContactItemField* field1 = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	field1->TextStorage()->SetTextL(aGivenName);
	card->AddFieldL(*field1);
	CleanupStack::Pop(field1);
	CContactItemField* field2 = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
	field2->TextStorage()->SetTextL(aFamilyName);
	card->AddFieldL(*field2);
	CleanupStack::Pop(field2);
	CContactItemField* field3 = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCompanyName);
	field3->TextStorage()->SetTextL(aCompanyName);
	card->AddFieldL(*field3);
	CleanupStack::Pop(field3);

	TContactItemId id = iDb->AddNewContactL(*card);

	CleanupStack::PopAndDestroy(card);
	return id;
	}

void CTestResources::CreateTestOneContactsL()
	{
	// these must be in the same order as the view ordering
	iIdsInViewSortOrder.Append(AddNewContactL(_L(" a")));	// Check that space before is dealt with properly


	iIdsInViewSortOrder.Append(AddNewContactL(_L(" a"),		_L(" y")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L(" a"),		_L("x")));		
	iIdsInViewSortOrder.Append(AddNewContactL(_L(" aa"),	_L("x")));	// Check that space before is dealt with properly
	iIdsInViewSortOrder.Append(AddNewContactL(_L(" aa"),	_L("y")));	
	iIdsInViewSortOrder.Append(AddNewContactL(_L(" c")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L(" c"),		_L("x")));
	
	
	iIdsInViewSortOrder.Append(AddNewContactL(_L("a")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("a"),		_L("z")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("aa")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("aa"),		_L("x")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("aa"),		_L("x")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("aa"),		_L("z")));
	
	iIdsInViewSortOrder.Append(AddNewContactL(_L("b"),		_L(" xx")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("b"),		_L(" y")));	
	iIdsInViewSortOrder.Append(AddNewContactL(_L("b"),		_L("x")));

	iIdsInViewSortOrder.Append(AddNewContactL(_L("b"),		_L("z    z")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("b"),		_L("z x")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("b"),		_L("zx")));

	iIdsInViewSortOrder.Append(AddNewContactL(_L("c"),		_L("X")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("c"),		_L("z")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("d"),		_L("x")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("D"),		_L("y")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("d"),		_L("z")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("e")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L(""),		_L(""),		_L("f")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L(""),		_L("g")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("h"),		_L(""),		_L("a")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("h"),		_L("b")));
	iIdsInViewSortOrder.Append(AddNewContactL(_L("h"),		_L("b"),	_L("a")));

	// Contacts 11 & 12 have the same text
	// so reverse order sort will put these in Id order
	const TInt count = iIdsInViewSortOrder.Count();
	for (TInt i = 0; i < count; i++)
		{
		TContactItemId id = iIdsInViewSortOrder[count - i - 1];

		if (id == 11)
			{
			iIdsInViewReverseSortOrder.Append(12);
			}
		else if (id == 12)
			{
			iIdsInViewReverseSortOrder.Append(11);
			}
		else
			{
			iIdsInViewReverseSortOrder.Append(id);
			}
		}
	}

void CTestResources::CreateTestTwoContactsL()
	{
	// Content and order specified in original defect report
	// THY-5D9KH7
	TContactItemId additionOrder[4];

	additionOrder[0] = AddNewContactL(_L("ibbb"));
	additionOrder[1] = AddNewContactL(_L("Ibbb"));
	additionOrder[2] = AddNewContactL(_L("ibbb"));

	iIdsInViewSortOrder.Append(additionOrder[0]);
	iIdsInViewSortOrder.Append(additionOrder[2]);
	iIdsInViewSortOrder.Append(additionOrder[1]);

	// this view has duplicate names
	// so we need to explicitly list the reverse order we expect
	iIdsInViewReverseSortOrder.Append(additionOrder[1]);	// upper case
	iIdsInViewReverseSortOrder.Append(additionOrder[0]);	// lower case
	iIdsInViewReverseSortOrder.Append(additionOrder[2]);	// lower case
	}

void CTestResources::CreateTestThreeContactsL()
	{
	// Content and order specified in original defect report
	// DEF022410
	TContactItemId additionOrder[8];
	
	additionOrder[0] = AddNewContactL(_L("Sunshine"));
	additionOrder[1] = AddNewContactL(_L("Symbian"));
	additionOrder[2] = AddNewContactL(_L("S ymbian"));
	additionOrder[3] = AddNewContactL(_L("Symmetrical"));
	additionOrder[4] = AddNewContactL(_L(" Symbian"));
	additionOrder[5] = AddNewContactL(_L("Symbian "));
	additionOrder[6] = AddNewContactL(_L("_Sunshine"));
	additionOrder[7] = AddNewContactL(_L("S_unshine"));

	iIdsInViewSortOrder.Append(additionOrder[6]);
	iIdsInViewSortOrder.Append(additionOrder[4]);
	iIdsInViewSortOrder.Append(additionOrder[7]);
	iIdsInViewSortOrder.Append(additionOrder[2]);
	iIdsInViewSortOrder.Append(additionOrder[0]);
	iIdsInViewSortOrder.Append(additionOrder[1]);
	iIdsInViewSortOrder.Append(additionOrder[5]);
	iIdsInViewSortOrder.Append(additionOrder[3]);
	}
	
	//	test to check that special characters
	//	are sorted properly (INC039675)
	void CTestResources::CreateTestFourContactsL()
	{
	TContactItemId additionOrder[8];

	additionOrder[0] = AddNewContactL(_L(".alli"));
	additionOrder[1] = AddNewContactL(_L("ally"));
	additionOrder[2] = AddNewContactL(_L("alli"));
	additionOrder[3] = AddNewContactL(_L(".ally"));
	additionOrder[4] = AddNewContactL(_L("alli ."));
	additionOrder[5] = AddNewContactL(_L("al .ly"));
	additionOrder[6] = AddNewContactL(_L("all .i"));
	additionOrder[7] = AddNewContactL(_L(" .ally"));

	//	correct behaviour of view is in this order
	iIdsInViewSortOrder.Append(additionOrder[7]);
	iIdsInViewSortOrder.Append(additionOrder[0]);
	iIdsInViewSortOrder.Append(additionOrder[3]);
	iIdsInViewSortOrder.Append(additionOrder[5]);
	iIdsInViewSortOrder.Append(additionOrder[6]);
	iIdsInViewSortOrder.Append(additionOrder[2]);
	iIdsInViewSortOrder.Append(additionOrder[4]);
	iIdsInViewSortOrder.Append(additionOrder[1]);

	}

void CTestResources::VerifySortOrderL()
	{
	const TInt count = iLocalView->CountL();
	for(TInt i=0; i<count; ++i)
		{
		TInt viewId = iLocalView->AtL(i);
		TInt expectedId=iIdsInViewSortOrder[i];
		// diagnostic info
		if (viewId != expectedId)
			{
			test.Printf(_L("* line number %d fails\n"), i+1);
			}
		test (viewId == expectedId);
		}
	}

void CTestResources::VerifyReverseSortOrderL()
	{
	const TInt count = iLocalReverseView->CountL();
	
	// explicit reverse orde to test?
	if (iIdsInViewReverseSortOrder.Count() > 0)
		{
		for(TInt i=0; i<count; ++i)
			{
			TInt viewId = iLocalReverseView->AtL(i);
			TInt expectedId=iIdsInViewReverseSortOrder[i];
			// diagnostic info
			if (viewId != expectedId)
				{
				test.Printf(_L("* line number %d fails\n"), i+1);
				}
			test (viewId == expectedId);
			}
		}
	else
		{
		// check for reverse of normal order
		for(TInt i=0; i<count; ++i)
			{
			TInt viewId = iLocalReverseView->AtL(i);
			TInt expectedId=iIdsInViewSortOrder[count - i - 1];
			// diagnostic info
			if (viewId != expectedId)
				{
				test.Printf(_L("* line number %d fails\n"), i+1);
				}
			test (viewId == expectedId);
			}
		}
	}

void CTestResources::PrintViewSortOrderL()
	{
	test.Printf(_L("View Sort order is:\n"));
	TBuf<128> buf;
	for(TInt i=0; i<iLocalView->CountL(); ++i)
		{
		TContactItemId id=iLocalView->AtL(i);
		CContactItem* contact=iDb->ReadMinimalContactLC(id);
	 	CContactItemFieldSet& fieldSet=contact->CardFields();
	 	buf.Format(_L("Id %d : \""), id);
	 	TInt pos=fieldSet.Find(KUidContactFieldGivenName);
		if (pos!=KErrNotFound)
		 	buf.Append(fieldSet[pos].TextStorage()->Text());
		buf.Append(_L("\" "));
	 	pos=fieldSet.Find(KUidContactFieldFamilyName);
	 	buf.Append(_L("\""));
		if (pos!=KErrNotFound)
	 		buf.Append(fieldSet[pos].TextStorage()->Text());
		buf.Append(_L("\" "));
	 	pos=fieldSet.Find(KUidContactFieldCompanyName);
	 	buf.Append(_L("\""));
		if (pos!=KErrNotFound)
		 	buf.Append(fieldSet[pos].TextStorage()->Text());
		buf.Append(_L("\""));
		RDebug::Print(_L("%S\n"),&buf);
		CleanupStack::PopAndDestroy(contact);
		buf.Zero();
		}
	}

void CTestResources::PrintReverseViewSortOrderL()
	{
	test.Printf(_L("View Sort reverse order is:\n"));
	TBuf<128> buf;
	for(TInt i=0; i<iLocalReverseView->CountL(); ++i)
		{
		TContactItemId id=iLocalReverseView->AtL(i);
		CContactItem* contact=iDb->ReadMinimalContactLC(id);
	 	CContactItemFieldSet& fieldSet=contact->CardFields();
	 	buf.Format(_L("Id %d : \""), id);
	 	TInt pos=fieldSet.Find(KUidContactFieldGivenName);
		if (pos!=KErrNotFound)
		 	buf.Append(fieldSet[pos].TextStorage()->Text());
		buf.Append(_L("\" "));
	 	pos=fieldSet.Find(KUidContactFieldFamilyName);
	 	buf.Append(_L("\""));
		if (pos!=KErrNotFound)
	 		buf.Append(fieldSet[pos].TextStorage()->Text());
		buf.Append(_L("\" "));
	 	pos=fieldSet.Find(KUidContactFieldCompanyName);
	 	buf.Append(_L("\""));
		if (pos!=KErrNotFound)
		 	buf.Append(fieldSet[pos].TextStorage()->Text());
		buf.Append(_L("\""));
		RDebug::Print(_L("%S\n"),&buf);

		CleanupStack::PopAndDestroy(contact);
		buf.Zero();
		}
	}

LOCAL_C void TestViewOrderOfContactsL()
    {
    test.Next(_L("Test the view sort order"));

	CTestResources* res = CTestResources::NewLC();
	TRAPD(err, res->StartTestOneL());
	if (err != KErrNone)
		{
		test.Printf(_L("Error Code: %d:\n"), err);
		}
	res->PrintViewSortOrderL();
	res->VerifySortOrderL();
	res->PrintReverseViewSortOrderL();
	res->VerifyReverseSortOrderL();
	CleanupStack::PopAndDestroy(res);

	res = CTestResources::NewLC();
	res->StartTestTwoL();
	res->PrintViewSortOrderL();
	res->VerifySortOrderL();
	res->PrintReverseViewSortOrderL();
	res->VerifyReverseSortOrderL();
	CleanupStack::PopAndDestroy(res);

	res = CTestResources::NewLC();
	res->StartTestThreeL();
	res->PrintViewSortOrderL();
	res->VerifySortOrderL();
	res->PrintReverseViewSortOrderL();
	res->VerifyReverseSortOrderL();
	CleanupStack::PopAndDestroy(res);
	
	res = CTestResources::NewLC();
	res->StartTestFourL();
	res->PrintViewSortOrderL();
	res->VerifySortOrderL();
	res->PrintReverseViewSortOrderL();
	res->VerifyReverseSortOrderL();
	CleanupStack::PopAndDestroy(res);
	}

/**

@SYMTestCaseID     PIM-T-VIEWSORT-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEWSORT-0001 T_ViewSort"));


	TestViewOrderOfContactsL();

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
