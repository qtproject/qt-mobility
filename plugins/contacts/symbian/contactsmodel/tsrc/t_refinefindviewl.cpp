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

#include "ccontactvieweventqueue.h"
#include "cfindtext.h"
#include "t_utils2.h"
#include "t_utils.h"

_LIT(KTestName, "T_RefineFindViewL");

_LIT(KTestDbName, "c:T_RefineFindViewL.cdb");

const TInt KMaxNumContacts=1000;

LOCAL_D RTest test(KTestName);


class CTestResources : public CBase
    {
    public:
        static CTestResources* NewLC();
        void ConstructL();
        void CreateTestContactsL();
		void CreateMoreTestContactsL();
        ~CTestResources();

        CContactDatabase* iDb;
        CContactViewEventQueue* iViewEventQueue;
        RContactViewSortOrder iViewSortOrder;
        CContactLocalView* iLocalView;
        CContactViewEventQueue* iFindViewEventQueue;
        CContactFindView* iFindView;
        CFindText* iFindText;
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

    CreateTestContactsL();

    iViewEventQueue = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
    
    const TContactViewPreferences prefs = 
        static_cast<TContactViewPreferences>(EContactsOnly);
    iLocalView = CContactLocalView::NewL
        (*iViewEventQueue, *iDb, iViewSortOrder, prefs);

    // Wait for view to get ready
    TContactViewEvent event;
    __ASSERT_ALWAYS(iViewEventQueue->ListenForEvent(10,event),User::Invariant());
    __ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady,User::Invariant());
    }

void CTestResources::CreateTestContactsL()
    {
    // Create a few test contacts in the db
    CContactCard* card;
    CContactItemField* field;
    card = CContactCard::NewLC();
    field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(_L("Graham"));
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);

    card = CContactCard::NewLC();
    field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(_L("Sami"));
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);

    card = CContactCard::NewLC();
    field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(_L("Sarah"));
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);
    }


void CTestResources::CreateMoreTestContactsL()
	{
	CRandomContactGenerator* generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL(generator);
	generator->SetDbL(*iDb);
	for (TInt i=0;i<KMaxNumContacts;i++)
		{
		generator->AddTypicalRandomContactL();
		if (i%100==0)
			{
			iDb->CompactL();
			test.Printf(_L("Added %d contacts\n"),i);
			}
		}
	CleanupStack::PopAndDestroy(generator);

	}

CTestResources::~CTestResources()
    {
    if (iFindView) iFindView->Close(*iFindViewEventQueue);
    delete iFindViewEventQueue;
    delete iFindText;
    if (iLocalView) iLocalView->Close(*iViewEventQueue);
    iViewSortOrder.Close();
    delete iViewEventQueue;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

/**
 * Simulates using CContactFindView to filter contacts as the user types 
 * characters to an UI "find box".
 */

LOCAL_C void TestRefineFindViewL()
    {
	__UHEAP_MARK;

	test.Next(_L("Repeatedly call CContactFindView::RefineFindViewL()"));

    CTestResources* res = CTestResources::NewLC();

    // Simulate the user typing 's', 'a', 'm', 'i' into a find box
    _LIT(KFindText, "sami");
    for (TInt c=1; c <= KFindText().Length(); ++c)
        {
        // Allocate a dummy buffer to keep the heap from recycling a previous
        // CFindText's address to newFindText below.
        HBufC* dummy = HBufC::NewLC(c*c);

        // Create a find word array
        CFindText* newFindText = CFindText::NewLC(KFindText().Left(c));

        if (!res->iFindView)
            {
            // create find view when first find character is entered
            res->iFindViewEventQueue = CContactViewEventQueue::NewL();
			RDebug::Print(_L("=== %S\n"),&newFindText->Text());
            res->iFindView = CContactFindView::NewL
                (*res->iDb, *res->iLocalView, *res->iFindViewEventQueue, newFindText);
            // Wait for the find view to get ready
            TContactViewEvent event;
            __ASSERT_ALWAYS(res->iFindViewEventQueue->ListenForEvent(10,event),User::Invariant());
            __ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady,User::Invariant());
            }
        else
            {
            // Find view exists -> just refine search
            // (Note: I also noticed a memory leak if you comment out the line
            //  below and execute the test. The leaked object is a CViewContact
            //  and it seems to be allocated in CContactLocalView::InsertL().)
			RDebug::Print(_L("=== %S\n"),&newFindText->Text());
            res->iFindView->RefineFindViewL(newFindText);
            }

        // keep the find words array because RefineFindViewL accesses the previous
        // array on the next cycle
        CleanupStack::Pop(newFindText);
        delete res->iFindText;
        res->iFindText = newFindText;

        CleanupStack::PopAndDestroy(dummy);
        }

    CleanupStack::PopAndDestroy(res);

	__UHEAP_MARKEND;
    }

LOCAL_C void CleanupContactArray(TAny* aArray)	
	{
	RPointerArray<CViewContact>* array=REINTERPRET_CAST(RPointerArray<CViewContact>*,aArray);
	if (array)
		array->ResetAndDestroy();
	array->Close();
	}

/** Profile the speed of CContactLocalView::ContactsMatchingCriteriaL */
LOCAL_C void ProfileContactMatchingCriteriaL()
	{
	CTestResources* res = CTestResources::NewLC();
	res->CreateMoreTestContactsL();
	
	while (res->iLocalView->CountL()<KMaxNumContacts)
		{
		TContactViewEvent event;
		__ASSERT_ALWAYS(res->iViewEventQueue->ListenForEvent(10,event), User::Invariant());
		
		if (event.iEventType == TContactViewEvent::EUnavailable)
			{
			// It is likely that when adding 1000 contacts to the database, the view
			// event queue will have overflowed at the server side, and instead
			// the client view will be totally recreated.
			break;
			}
		
		__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EItemAdded, User::Invariant());
		}

	CFindText* findText = CFindText::NewLC(_L("S"));
	RPointerArray<CViewContact> contactArray;
	CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
	CCntTest::ProfileReset(0,2);
	CCntTest::ProfileStart(0);
	res->iLocalView->ContactsMatchingPrefixL(*findText,contactArray);
	CCntTest::ProfileEnd(0);
	CleanupStack::PopAndDestroy(&contactArray);

	res->iViewEventQueue->Flush();
	const TContactViewPreferences prefs = static_cast<TContactViewPreferences>(EContactsOnly);
	CContactRemoteView* rv = CContactRemoteView::NewL(*res->iViewEventQueue, *res->iDb, res->iViewSortOrder, prefs);
	CleanupStack::PushL(rv);
	TContactViewEvent event(TContactViewEvent::EUnavailable);
	while (event.iEventType != TContactViewEvent::EReady)
		{
		__ASSERT_ALWAYS(res->iViewEventQueue->ListenForEvent(10,event), User::Invariant());
		}
	__ASSERT_ALWAYS(rv->CountL()>=KMaxNumContacts, User::Invariant());

	RPointerArray<CViewContact> contactArray2;
	CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray2));
	CCntTest::ProfileStart(1);
	rv->ContactsMatchingPrefixL(*findText,contactArray2);
	CCntTest::ProfileEnd(1);
	CleanupStack::PopAndDestroy(&contactArray2);

	TCntProfile profile[2];
	CCntTest::ProfileResult(profile,0,2);
	_LIT(KProfileLocal,"LocalView PrefixSearch %d.%03d sec (%d hits)\n");
	_LIT(KProfileRemote,"RemoteView PrefixSearch %d.%03d sec (%d hits)\n");
	test.Printf(KProfileLocal,profile[0].iTime/1000000, profile[0].iTime%1000000, profile[0].iCount);
	RDebug::Print(KProfileLocal,profile[0].iTime/1000000, profile[0].iTime%1000000, profile[0].iCount);
	test.Printf(KProfileRemote,profile[1].iTime/1000000, profile[1].iTime%1000000, profile[1].iCount);
	RDebug::Print(KProfileLocal,profile[1].iTime/1000000, profile[1].iTime%1000000, profile[1].iCount);

	CleanupStack::PopAndDestroy(rv);
	CleanupStack::PopAndDestroy(findText);
	CleanupStack::PopAndDestroy(res);
	}

/**

@SYMTestCaseID     PIM-T-REFINEFINDVIEWL-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-REFINEFINDVIEWL-0001 T_RefineFindViewL"));

    TestRefineFindViewL();
	ProfileContactMatchingCriteriaL();
    test.End();
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

	//Get rid of the warnings.
	//T_utils2 uses efsrv.dll, but we don't use this functions in this test code.
	//So we pretend to use efsrv to keep T_utils2.obj and linker happy
	RFs fs;
	fs.Connect();
	fs.Close();

    // Run the tests
    TRAPD(err, DoTestsL());

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
	test.Close();
	__UHEAP_MARKEND;
	return err;
    }
