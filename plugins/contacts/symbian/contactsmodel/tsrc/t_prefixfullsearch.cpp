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

_LIT(KTestName, "T_PreFixFullSearch");

_LIT(KTestDbName, "c:T_PreFixFullSearch.cdb");

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
    __ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady ,User::Invariant());
  
    }

void CTestResources::CreateTestContactsL()
    {

    // Create a few test contacts in the db
	// - Creating 'prefix' results for 'an'
    CContactCard* card;
    CContactItemField* field;
    card = CContactCard::NewLC();
    field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(_L("Annie"));
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);

    card = CContactCard::NewLC();
    field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(_L("Anne"));
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);

    card = CContactCard::NewLC();
    field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(_L("Andrew"));
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);

	// - Creating 'fullsearch' results for 'an'
    card = CContactCard::NewLC();
    field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(_L("Christan"));
    card->AddFieldL(*field);
    CleanupStack::Pop(field);
    iDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);

    card = CContactCard::NewLC();
    field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(_L("Adan"));
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


LOCAL_C TInt FullSearchL(const TDesC& aDes, CTestResources* aTestResources)
    {
	CTestResources* res = NULL;
	HBufC* dummy = NULL;
	CFindText* newFindText = NULL;
	TContactViewEvent event;
	TInt retval = 0;

	test.Next(_L("Preform a full search only"));

	if (aTestResources == NULL)
		{
		res = CTestResources::NewLC();
		}
	else
		{
		res = aTestResources;
		}

    dummy = HBufC::NewLC(aDes.Length() * aDes.Length());
    newFindText = CFindText::NewLC(aDes);
    res->iFindViewEventQueue = CContactViewEventQueue::NewL();
    res->iFindView = CContactFindView::NewL(*res->iDb, *res->iLocalView, *res->iFindViewEventQueue, newFindText);

	do
		{
		__ASSERT_ALWAYS(res->iFindViewEventQueue->ListenForEvent(10,event),User::Invariant());
		}
	while (event.iEventType != TContactViewEvent::EReady);
	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady,User::Invariant());

    retval = res->iFindView->CountL();


    CleanupStack::PopAndDestroy(newFindText);
	CleanupStack::PopAndDestroy(dummy);
	if (aTestResources == NULL)
		{
		CleanupStack::PopAndDestroy(res);
		}
	return retval;
    }

LOCAL_C TInt PreFixSearchL(const TDesC& aDes, CTestResources* aTestResources)
    {
	CTestResources* res = NULL;
	HBufC* dummy = NULL;
	CFindText* newFindText = NULL;
	TContactViewEvent event;
	TInt retval = 0;

	test.Next(_L("Preform a prefix search only"));

	if (aTestResources == NULL)
		{
    	res = CTestResources::NewLC();
		}
	else
		{
		res = aTestResources;
		}
    dummy = HBufC::NewLC(aDes.Length() * aDes.Length());
    newFindText = CFindText::NewLC(aDes);
    res->iFindViewEventQueue = CContactViewEventQueue::NewL();
    res->iFindView = CContactFindView::NewL(*res->iDb, *res->iLocalView, *res->iFindViewEventQueue, newFindText, CContactViewBase::EPrefixSearch);

	do
		{
		__ASSERT_ALWAYS(res->iFindViewEventQueue->ListenForEvent(10,event),User::Invariant());
		}
	while (event.iEventType != TContactViewEvent::EReady);
	__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady,User::Invariant());

    //__ASSERT_ALWAYS(res->iFindViewEventQueue->ListenForEvent(10,event),User::Invariant());
	//__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady,User::Invariant());

    retval = res->iFindView->CountL();


    CleanupStack::PopAndDestroy(newFindText);
	CleanupStack::PopAndDestroy(dummy);

	if (aTestResources == NULL)
		{
		CleanupStack::PopAndDestroy(res);
		}
	return retval;
    }

/** Profile the speed of CContactLocalView::ContactsMatchingCriteriaL */
LOCAL_C void AddNewRandomContactsL(CTestResources* res)
	{
	//CTestResources* res = CTestResources::NewLC();
	res->CreateMoreTestContactsL();

	while (res->iLocalView->CountL()<KMaxNumContacts)
		{
		TContactViewEvent event;
		__ASSERT_ALWAYS(res->iViewEventQueue->ListenForEvent(10,event), User::Invariant());

		if (event.iEventType == TContactViewEvent::EUnavailable)
			{
			// In the event of queue over flow, the view becomes unavailable
			// this is valid behaviour, as view will be refreshed completely
			break;
			}
		
		__ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EItemAdded , User::Invariant());
		}
	}

LOCAL_C void TimeSearchesL(const TDesC& aDes)
	{
	CTestResources* res = CTestResources::NewLC();

	AddNewRandomContactsL(res);


	CCntTest::ProfileReset(0,2);
	CCntTest::ProfileStart(0);
	FullSearchL(aDes, res);
	CCntTest::ProfileEnd(0);

	if (res->iFindView != NULL)
		{
		delete res->iFindView;
		res->iFindView = NULL;
		}
	if (res->iFindViewEventQueue != NULL)
		{
		delete res->iFindViewEventQueue;
		res->iFindViewEventQueue = NULL;
		}

	CCntTest::ProfileStart(1);
	PreFixSearchL(aDes, res);
	CCntTest::ProfileEnd(1);


	TCntProfile profile[2];
	CCntTest::ProfileResult(profile,0,2);
	_LIT(KFullSearch,"FullSearch %d.%03d sec (%d hits)\n");
	_LIT(KPrefixSearch,"PrefixSearch %d.%03d sec (%d hits)\n");
	test.Printf(KFullSearch,profile[0].iTime/1000000, profile[0].iTime%1000000, profile[0].iCount);
	RDebug::Print(KFullSearch,profile[0].iTime/1000000, profile[0].iTime%1000000, profile[0].iCount);
	test.Printf(KPrefixSearch,profile[1].iTime/1000000, profile[1].iTime%1000000, profile[1].iCount);
	RDebug::Print(KPrefixSearch,profile[1].iTime/1000000, profile[1].iTime%1000000, profile[1].iCount);
	CleanupStack::PopAndDestroy(res);
	}

/**

@SYMTestCaseID     PIM-T-PREFIXFULLSEARCH-0001

*/

void DoTestsL()
    {
	_LIT(KSearchString, "an");
	test.Start(_L("@SYMTESTCaseID:PIM-T-PREFIXFULLSEARCH-0001 T_PreFixFullSearch"));


	__UHEAP_MARK;
    FullSearchL(KSearchString, NULL);
    __UHEAP_MARKEND;

    __UHEAP_MARK;
    PreFixSearchL(KSearchString, NULL);
    __UHEAP_MARKEND;

	TimeSearchesL(KSearchString);

    test.End();
    }

GLDEF_C TInt E32Main()
	{
    // Init
	__UHEAP_MARK;
	RProcess().SetPriority(EPriorityBackground);
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
