/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ccontactvieweventqueue.h"
#include "ccontactdbeventqueue.h"

_LIT(KTestName, "T_FilteredViewEvents");

_LIT(KTestDbName, "c:T_FilteredViewEvents.cdb");
_LIT(KDbName,"c:ViewTest.db");
_LIT(KViewName, "TestView");
const TInt KNumOfContacts = 3;
const TInt KTimeout = 60*1000000; // 60 seconds
const TInt KTestBufSize = 64;
const TContactViewPreferences KContactViewPrefs =
static_cast<TContactViewPreferences>
    ( EContactsOnly | EUnSortedAtEnd | ESingleWhiteSpaceIsEmptyField );


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
    __ASSERT_ALWAYS(iFilteredViewEventQueue->ListenForEvent(10,event), User::Invariant());
    __ASSERT_ALWAYS(event.iEventType == TContactViewEvent::EReady, User::Invariant());

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

LOCAL_C CTestResources* DoTestAddContactLC(TBool aWithPhoneNumber)
    {
    CTestResources* res = CTestResources::NewLC();
    
    const TContactItemId contactId = res->CreateTestContactL(_L("Joplin"), aWithPhoneNumber);

    // Check that iLocalView sends an event
    TContactViewEvent event;
    test(res->iViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EItemAdded);
    test(event.iContactId == contactId);
    test(event.iInt >= 0 && event.iInt < res->iLocalView->CountL());
    test(res->iLocalView->AtL(event.iInt) == res->iTestContactId);

    return res;
    }
    
    
//////////////////////////////////////////////////////////////////////////////////////////
//
class CFilteredViewTester : public CTimer, public MContactViewObserver
{
public:
	static CFilteredViewTester* NewL();
	~CFilteredViewTester();
	void StartTest();
	TBool TestCompletedOk();
	
private:
	CFilteredViewTester();
	void ConstructL();				
	void NextTestStepL();
	void CreateViewsL();
	void AddContactsL(TInt aCount);
	void DeleteContactsDescL(TInt aCount);
	TContactItemId DoAddContactL(TInt n);
	CContactItemField* CreateFieldLC(const TUid aMappingUId, TFieldType aFieldType, const TDesC &aText);
	
private: // from CTimer
	void RunL();
	void DoCancel();
	
private: // from MContactViewObserver	
	void HandleContactViewEvent(const CContactViewBase &aView, const TContactViewEvent &aEvent);	

private:
	enum TestStates{
		EStateIdle = 0,
		ECreateView,	
		EAddContacts,
		EDeleteContacts,
		ETestComplete,
	};
	
private:
	TestStates iState;
	CContactDatabase* iContactDb;
	CContactViewBase* iBaseView;
    CContactViewBase* iFilteredView;
    TBool iBaseReady;
    TBool iFilteredReady;
    TInt iBaseContactsCount;
    TInt iFilteredContactsCount;
    RContactViewSortOrder iSortOrder;
    CContactIdArray* iIdArray;
    TBool iTimerStarted;
    TInt iContactArrayIndex;
};

CFilteredViewTester* CFilteredViewTester::NewL()
{
	CFilteredViewTester* self = new (ELeave) CFilteredViewTester();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

CFilteredViewTester::CFilteredViewTester()
: CTimer(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

CFilteredViewTester::~CFilteredViewTester()
	{	
	iSortOrder.Close();
	delete iIdArray;
	if (iFilteredView)
	{
		iFilteredView->Close(*this);
		}
	if (iBaseView)
		{
		iBaseView->Close(*this);
		}
	delete iContactDb;
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbName) );
	}


void CFilteredViewTester::ConstructL()
	{
	CTimer::ConstructL();
	iContactDb = CContactDatabase::CreateL(KDbName);
	iSortOrder.AppendL(KUidContactFieldFamilyName);
	iSortOrder.AppendL(KUidContactFieldGivenName);
	iIdArray = CContactIdArray::NewL ();
	}

void CFilteredViewTester::StartTest()
	{
	iState = ECreateView;
	TRequestStatus* s = &iStatus;
	User::RequestComplete(s, KErrNone);	
	SetActive();
	}

void CFilteredViewTester::NextTestStepL()
	{
	switch (iState)
		{
		case ECreateView:
			CreateViewsL();
			break;
		case EAddContacts:
			AddContactsL(KNumOfContacts);
			break;
		case EDeleteContacts:
			DeleteContactsDescL(KNumOfContacts);
			break;
		case ETestComplete:
		default:
		TRequestStatus* s = &iStatus;
		User::RequestComplete(s, KErrNone);					
			break;
		}
	}

void CFilteredViewTester::RunL()
	{
	if (!iTimerStarted)
		{
		iTimerStarted = ETrue;
		After(KTimeout);
		NextTestStepL();
		}
	else
		{
		CActiveScheduler::Stop ();
		}	
	}

void CFilteredViewTester::DoCancel()
	{
	}

TBool CFilteredViewTester::TestCompletedOk()
	{
	return iState == ETestComplete;	
	}

void CFilteredViewTester::HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent)
	{	
	switch (aEvent.iEventType)
		{
		case TContactViewEvent::EReady:
			{
			if (&aView == iBaseView)
				{
				iBaseReady = ETrue;
				}
			else if (&aView == iFilteredView)
				{
				iFilteredReady = ETrue;
				}

			if (iFilteredReady && iBaseReady && iState == ECreateView)
				{
				iState = EAddContacts;
				TRAPD(err, NextTestStepL() );
				test(err == KErrNone);
				}
			break;
			}

		case TContactViewEvent::EItemAdded:
			{
			if (&aView == iBaseView)
				{
				test.Printf(_L("Add event received: Base view\r\n"));
				iBaseContactsCount++;
				}
			else if (&aView == iFilteredView)
				{
				test.Printf(_L("Add event received: Filtered view\r\n"));
				iFilteredContactsCount++;
				}
			if (iBaseContactsCount == KNumOfContacts && iFilteredContactsCount == KNumOfContacts && iState == EAddContacts)
				{
				iState = EDeleteContacts;
				NextTestStepL();
				}
			break;
			}

		case TContactViewEvent::EItemRemoved:
			{
			if (&aView == iBaseView)
				{
				test.Printf(_L("Remove event received: Base view\r\n"));
				iBaseContactsCount--;
				}
			else if (&aView == iFilteredView)
				{
				test.Printf(_L("Remove event received: Filtered view\r\n"));
				iFilteredContactsCount--;
				//The contacts are deleted in descending order but the id and position should be reported correctly 
				//The reported positions are 
				//             contact 1 deleted at position 0 
				//             contact 2 deleted at position 0 
				//             contact 3 deleted at position 0 
				test (aEvent.iInt == 0); 
				test (aEvent.iContactId == (*iIdArray)[iContactArrayIndex++]); 
				}

			if (iBaseContactsCount == 0 && iFilteredContactsCount == 0 && iState == EDeleteContacts)
				{
				iState = ETestComplete;
				NextTestStepL();
				}
			break;
			}

		default:
			{
			// do nothing...
			break;
			}
		}	
	}

void CFilteredViewTester::CreateViewsL()
	{
	iBaseView = CContactNamedRemoteView::NewL(*this, KViewName, *iContactDb, iSortOrder, KContactViewPrefs);
    iFilteredView = CContactFilteredView::NewL(*this, *iContactDb, *iBaseView, CContactDatabase::EPhonable); 
	}

void CFilteredViewTester::AddContactsL(TInt aCount)
	{
	for (TInt i = 0; i < aCount; ++i)
		{		
		iIdArray->AddL(DoAddContactL(i));
		}    
	}

void CFilteredViewTester::DeleteContactsDescL(TInt aCount)
    {
    for (TInt i = aCount - 1; i >= 0; --i)
        {
        iContactDb->DeleteContactL((*iIdArray)[i]);
        }       
    }

TContactItemId CFilteredViewTester::DoAddContactL(TInt aOrdinal)
	{
	TBuf<KTestBufSize> name(_L("MyContact"));
	TBuf<KTestBufSize> num1(_L("123456"));
	TBuf<KTestBufSize> num2(_L("123457"));
	name.AppendNum (aOrdinal);
	num1.AppendNum (aOrdinal);
	num2.AppendNum (aOrdinal);

	CContactCard* card = CContactCard::NewLC();

	card->AddFieldL(*CreateFieldLC(KUidContactFieldVCardMapUnusedN,	KUidContactFieldGivenName, _L("ContactField")));
	CleanupStack::Pop();
	card->AddFieldL (*CreateFieldLC(KUidContactFieldVCardMapUnusedN, KUidContactFieldFamilyName, name));
	CleanupStack::Pop();
	card->AddFieldL (*CreateFieldLC(KUidContactFieldVCardMapTEL, KUidContactFieldPhoneNumber, num1));
	CleanupStack::Pop();
	card->AddFieldL (*CreateFieldLC(KUidContactFieldVCardMapTELFAX, KUidContactFieldPhoneNumber, num2));
	CleanupStack::Pop();

	TContactItemId id = iContactDb->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(card);
	return id;
	}

CContactItemField* CFilteredViewTester::CreateFieldLC(const TUid aMappingUId, TFieldType aFieldType, const TDesC &aText)
	{
	CContactItemField * field =CContactItemField::NewLC(KStorageTypeText, aFieldType);
	field->SetMapping(aMappingUId);
	field->TextStorage()->SetTextL(aText);
	return field;
	}
// CFilteredViewTester
////////////////////////////////////////////////////////////////////////    

LOCAL_C void TestAddContactWithPhoneNumberL()
    {
    test.Next(_L("Add a contact with phone number"));

    CTestResources* res = DoTestAddContactLC(ETrue);

    // There should come an event from iFilteredView because the added
    // contact matches the filtering criteria
    TContactViewEvent event;
    test(res->iFilteredViewEventQueue->ListenForEvent(10,event));
    test(event.iEventType == TContactViewEvent::EItemAdded);
    test(event.iContactId == res->iTestContactId);
    test(event.iInt >= 0 && event.iInt < res->iFilteredView->CountL());
    test(res->iFilteredView->AtL(event.iInt) == res->iTestContactId);

    CleanupStack::PopAndDestroy(res);
    }

LOCAL_C void TestRemoveContactWithPhoneNumberL()
    {
    test.Next(_L("Remove a contact with phone number"));

    CTestResources* res = DoTestAddContactLC(ETrue);

    // There should come an event from iFilteredView because the added
    // contact matches the filtering criteria
    TContactViewEvent viewEvent;
    test(res->iFilteredViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemAdded);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt >= 0 && viewEvent.iInt < res->iFilteredView->CountL());
    test(res->iFilteredView->AtL(viewEvent.iInt) == res->iTestContactId);

    // Delete the added contact
    res->iDb->DeleteContactL(res->iTestContactId);

    // Wait for a DB event
    TContactDbObserverEvent dbEvent;
    test(res->iDbEventQueue->ListenForEvent(10,dbEvent));
    test(dbEvent.iType == EContactDbObserverEventContactDeleted);
    test(dbEvent.iContactId == res->iTestContactId);

    // Check that iLocalView sends an event
    test(res->iViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt >= 0 && viewEvent.iInt <= res->iLocalView->CountL());
    test(res->iLocalView->FindL(viewEvent.iContactId) == KErrNotFound);

    // There should come an event from iFilteredView because the added
    // contact matches the filtering criteria
    test(res->iFilteredViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt >= 0 && viewEvent.iInt <= res->iFilteredView->CountL());
    test(res->iFilteredView->FindL(viewEvent.iContactId) == KErrNotFound);

    CleanupStack::PopAndDestroy(res);
    }


LOCAL_C void TestAddContactWithoutPhoneNumberL()
    {
    test.Next(_L("Add a contact without phone number"));

    CTestResources* res = DoTestAddContactLC(EFalse);

    // There shouldn't come any event from iFilteredView because the added
    // contact does not match the filtering criteria. 
    // However, if an event is received check that it is valid
    TContactViewEvent event;
    if (res->iFilteredViewEventQueue->ListenForEvent(10,event))
        {
        test(event.iEventType == TContactViewEvent::EItemAdded);
        test(event.iContactId == res->iTestContactId);
        test(event.iInt >= 0 && event.iInt < res->iFilteredView->CountL());
        test(res->iFilteredView->AtL(event.iInt) == res->iTestContactId);
        }
    CleanupStack::PopAndDestroy(res);
    }

LOCAL_C void TestRemoveContactWithoutPhoneNumberL()
    {
    test.Next(_L("Remove a contact without phone number"));

    CTestResources* res = DoTestAddContactLC(EFalse);

    // There shouldn't come any event from iFilteredView because the added
    // contact does not match the filtering criteria. 
    // However, if an event is received check that it is valid
    TContactViewEvent viewEvent;
    if (res->iFilteredViewEventQueue->ListenForEvent(10,viewEvent))
        {
        // Already tested in TestAddContactWithoutPhoneNumberL
        /*
        test(viewEvent.iEventType == TContactViewEvent::EItemAdded);
        test(viewEvent.iContactId == res->iTestContactId);
        test(viewEvent.iInt >= 0 && viewEvent.iInt < res->iFilteredView->CountL());
        test(res->iFilteredView->AtL(viewEvent.iInt) == res->iTestContactId);
        */
        }

    // Delete the added contact
    res->iDb->DeleteContactL(res->iTestContactId);

    // Wait for a DB event
    TContactDbObserverEvent dbEvent;
    test(res->iDbEventQueue->ListenForEvent(10,dbEvent));
    test(dbEvent.iType == EContactDbObserverEventContactDeleted);
    test(dbEvent.iContactId == res->iTestContactId);

    // Check that iLocalView sends an event
    test(res->iViewEventQueue->ListenForEvent(10,viewEvent));
    test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
    test(viewEvent.iContactId == res->iTestContactId);
    test(viewEvent.iInt >= 0 && viewEvent.iInt <= res->iLocalView->CountL());
    test(res->iLocalView->FindL(viewEvent.iContactId) == KErrNotFound);

    // There shouldn't come any event from iFilteredView because the removed
    // contact does not match the filtering criteria. 
    // However, if an event is sent check that it is valid
    if (res->iFilteredViewEventQueue->ListenForEvent(10,viewEvent))
        {
        test(viewEvent.iEventType == TContactViewEvent::EItemRemoved);
        test(viewEvent.iContactId == res->iTestContactId);
        test(viewEvent.iInt >= 0 && viewEvent.iInt <= res->iFilteredView->CountL());
        test(res->iFilteredView->FindL(viewEvent.iContactId) == KErrNotFound);
        }
    CleanupStack::PopAndDestroy(res);
    }


LOCAL_C void TestListenForRemoveContactEventsL()
	{
    test.Next(_L("Check events received when deleting contacts"));

	
    CFilteredViewTester* viewTest = CFilteredViewTester::NewL();
	CleanupStack::PushL(viewTest);

	viewTest->StartTest();
	CActiveScheduler::Start();
	
	test(viewTest->TestCompletedOk());
	
	CleanupStack::PopAndDestroy(viewTest);	
	}

/**

@SYMTestCaseID     PIM-T-FILTEREDVIEWEVENTS-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-FILTEREDVIEWEVENTS-0001 T_FilteredViewEvents"));


    TestAddContactWithPhoneNumberL();
    TestRemoveContactWithPhoneNumberL();
    TestAddContactWithoutPhoneNumberL();
    TestRemoveContactWithoutPhoneNumberL();
	TestListenForRemoveContactEventsL();

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
