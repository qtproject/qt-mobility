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
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <e32math.h>
#include <phbksync.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntsyncecom.h"
#endif

#include "t_utils.h"
#include "cntsyncchecker.h"
#include "ccontactvieweventqueue.h"
#include "ccontactdbeventqueue.h"
#include <ecom/ecom.h>
// include templates for CleanupResetAndDestroyPushL(T)
#include "cntviewprivate.h"

_LIT(KTestName,"@SYMTestCaseID:PIM-T-ICCVIEW-0001 t_iccview");

_LIT(KTestDbName, "c:T_ICCView.cdb");
_LIT(KPluginName,"phone book synchronizer Implementation");
_LIT(KTestPluginName,"Test phone book synchronizer Implementation");

LOCAL_D RTest test(KTestName);

LOCAL_D CContactSyncChecker* syncChecker;

class CICCViewTest : public CBase
	{
public:
	static CICCViewTest* NewLC(const RArray<TUid>& aPhonebookList);
	~CICCViewTest();

	void TestOneOrFiveL(TBool aCreateViewBeforeSync);
	void TestTwoL();
	void TestThreeOrSixL(TBool aCreateViewBeforeSync);
	void TestFourL();
	void TestSevenL();
	void TestConcurrenceL();
	
private:
	enum TSyncStatus 
		{
		ENotSynchronised,
		ESynchronised,
		ESyncIccLocked,
		};
	CICCViewTest(const RArray<TUid>& aPhonebookList);
	void ConstructL();

	void CreateViewsL();
	void CheckViewCreateAndSyncEventsL(TInt aSyncCompletionError, TInt aIsSyncError);
	void CheckEventsForTestFourL();
	void SyncThenCreateViewCheckEventsL(TInt aSyncCompletionError, TInt aIsSyncError);
	void CheckEventsForTestSevenL();

	void CreateEntriesL(TInt aNumberOfEntries, TBool aIsIccEntry=EFalse);
	void CreateEntriesNoVerifyL(TInt aNumberOfEntries, TBool aIsIccEntry=EFalse);
	void DeleteEntriesL(TInt aNumberOfEntries, TBool aIccEntry=EFalse);
	void CreateEntryL(TBool aIsIccEntry=EFalse);
	void VerifyNumberOfEntriesInEachViewL(TSyncStatus aSyncStatus);

	void SetRandomAlphaString(TDes& aBuf,TInt aLength);
	TText RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException);
	TText RandomCharCode(TText aLowerBound,TText aUpperBound);

private:
	const RArray<TUid>& iPhbkList;

	CContactDatabase* iDb;
	CContactItem* iEntry;
	CContactViewEventQueue* iViewEventQueue;
	RContactViewSortOrder iViewSortOrder;
	CContactLocalView* iContactsView;
	CContactLocalView* iIccView;
	CContactLocalView* iContactsAndIccView;
	CContactViewEventQueue* iFilteredViewEventQueue;
	CContactItem* iTemplate;
	RArray<TContactItemId>	iIccContacts;
	RArray<TContactItemId>	iNonIccContacts;
	TInt64 iRandSeed;

	TInt iInitialIccCount;
	};

CICCViewTest* CICCViewTest::NewLC(const RArray<TUid>& aPhonebookList)
    {
    CICCViewTest* self = new(ELeave) CICCViewTest(aPhonebookList);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CICCViewTest::CICCViewTest(const RArray<TUid>& aPhonebookList) : iPhbkList(aPhonebookList)
	{
	}

void CICCViewTest::ConstructL()
    {
	// reset method call count & phonebook sync state
	// Set-up responses from the phonebook plug-in 
	syncChecker->ResetEverythingL();

	// tell phonebook sync simulator what phonebooks to use
	test(syncChecker->SetPhonebookListL(iPhbkList) == KErrNone);
	// put one contact in the first ICC Phonebook
	test(syncChecker->AddContactToPhonebookL(iPhbkList[0], _L("John Brown"), _L("1234")) == KErrNone);
	// put one contact in the last ICC Phonebook
	test(syncChecker->AddContactToPhonebookL(iPhbkList[iPhbkList.Count()-1], _L("Al Capone"), _L("9876")) == KErrNone);
	// number of ICC contacts expected in Views after Synchronisation
	iInitialIccCount = 2;

    // start test with empty database
	iDb = CContactDatabase::ReplaceL(KTestDbName);

    iViewEventQueue = CContactViewEventQueue::NewL();

    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
	}

// Create 3 views: non-icc contacts, icc contacts, and both
void CICCViewTest::CreateViewsL()
	{
    iContactsView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EContactsOnly);
	iIccView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EICCEntriesOnly);
	iContactsAndIccView = CContactLocalView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EICCEntriesAndContacts);
	}

/**
 Test View notifications when Phonebook Synch finishes
 Takes parameters to stimulate error conditions in the synchronisation

 Method: Creates 3 views for Contacts, ICC Entries and Mixed Contacts & ICC Entries
 Count/check the View events.
 */
void CICCViewTest::CheckViewCreateAndSyncEventsL(TInt aSyncCompletionError, TInt aIsSyncError)
	{
    // Wait for local views to get ready
	TInt readyCount(0);
	TInt sortErrorCount(0);
    TContactViewEvent event;

	// create the view to test
	CreateViewsL();

	// Contact only view should Ready
	test(iViewEventQueue->ListenForEvent(10,event));

	// Expecting EReady event
	test.Printf(_L("Event received (%i)\n"), event.iEventType);
	test(event.iEventType == TContactViewEvent::EReady);
	readyCount++;

	// there should be 1 Synch Change Notification request outstanding per phonebook
	TInt notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());

	// Phbk Watchers should be created when views were created - check number of plug-in calls
	test(syncChecker->SyncMethodCallCountL() == (2 + 4 * iPhbkList.Count()));


	// synchronise all phonebooks
	test(syncChecker->SynchroniseIccAllPhonebooksL(*iDb, aSyncCompletionError, aIsSyncError) == KErrNone);


	// views with ICC entries should become ready now
	while (((readyCount + sortErrorCount) < 3) && iViewEventQueue->ListenForEvent(10,event))
		{
		// Expecting EReady events
		switch (event.iEventType)
			{
		case TContactViewEvent::EItemAdded:
			test.Printf(_L("Event received - EItemAdded\n"));
			// ignore events from adding contacts
			break;

		case TContactViewEvent::EGroupChanged:
			test.Printf(_L("Event received - EGroupChanged (group Id %i)\n"), event.iContactId);
			// ignore events from adding contacts
			break;

		case TContactViewEvent::EReady:
			test.Printf(_L("Event received - EReady\n"));
			readyCount++;
			break;

		case TContactViewEvent::ESortError:
			test.Printf(_L("Event received - ESortError\n"));
			if (aSyncCompletionError != KErrNone)
				{
				test(event.iInt == aSyncCompletionError);
				}
			else
				{
				test(event.iInt == aIsSyncError);
				}
			sortErrorCount++;
			break;

		default:
			test.Printf(_L("Unexpected event received (%i)\n"), event.iEventType);
			test(0);
			}
		}

	// either received all events or timed out waiting
	if (aSyncCompletionError == KErrNone && aIsSyncError == KErrNone)
		{
		// all 3 views should be ready
		test(readyCount == 3);

		// check number of entries in views (ICC is synchronised)
		VerifyNumberOfEntriesInEachViewL(ESynchronised);
		}
	else
		{
		// ICC views should have had sort errors
		test(sortErrorCount == 2);

		// check number of entries in views (ICC is not synchronised)
		VerifyNumberOfEntriesInEachViewL(ENotSynchronised);
		}


	// there should be 1 Synch Change Notification request outstanding per phonebook
	notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());

	// Check the number of requests made to the plug-in
	const TInt methodCount = syncChecker->SyncMethodCallCountL();
	if (aSyncCompletionError == KErrNone)
		{
		// There should be 1 call plus 4 per phonebook to the Phonebook Sync plug-in
		// PhonebookList + N * 2 * (NotifySyncStateChange, IsSynchronised)
		test(methodCount == 2 + (8 * iPhbkList.Count()));  
		}
	else
		{
		// There should be 1 call plus 2 per phonebook to the Phonebook Sync plug-in
		// PhonebookList + N * (NotifySyncStateChange, IsSynchronised, NotifySyncStateChange)
		test(methodCount == 2 + (6 * iPhbkList.Count())); 
		}


    // Flush all other events
	iViewEventQueue->Flush();
	}


/**
 Test Objective: Test Phonebook Synch for SIM card that starts Locked and becomes Unlocked.
 - tests View events and number of contacts in views

 Method: Creates 3 views for Contacts, ICC Entries and Mixed Contacts & ICC Entries.
   Simulated Phonebook Synch fails with ICC card locked, subsequently card becomes unlocked
   and Phonebook Synch succeeds.
 
 Test steps:
 1. Create some non-ICC Contacts.
 2. Contacts only view becomes ready, while PhoneBook Synch waits to Synchronise.
 3. PhoneBook Synch then indicates an error due to SIM card (ICC) locked.
 4. The two ICC views become available without ICC entries.
 5. The PhoneBook Synch then indicates that the Cache is Valid.
 6. The two ICC views are updated (re-sorted) with the ICC entries.
 7. Cleanup (deletes all entries)
 */
void CICCViewTest::CheckEventsForTestFourL()
	{
    // Wait for local views to get ready
    TContactViewEvent event;
	TInt readyCount(0);
	TInt unavailableCount(0);
	TInt sortOrderChangedCount(0);

	// create the view to test
	CreateViewsL();


	// Test Step 1. Create some non-ICC Contacts.
	test.Printf(_L("Creating 5 Contacts\n"));
	CreateEntriesNoVerifyL(5);

	// This While loop tests for steps:
	// 2. Contacts only view becomes ready, while PhoneBook Synch waits to Synchronise.
	// 3. PhoneBook Synch then indicates an error due to SIM card (ICC) locked.
	// 4. The two ICC views become available without ICC entries.

	/* Expected events:
	 * a) EReady (1) => non-ICC Contacts view is ready
	 * b) [listen times out] test must notify sync SIM lock error
	 * c&d) EReady (1) => ICC Views become Ready empty or with contacts only
	 */

	// Contact only view should Ready
	test(iViewEventQueue->ListenForEvent(10,event));

	// Expecting EReady event
	test.Printf(_L("Event received (%i)\n"), event.iEventType);
	test(event.iEventType == TContactViewEvent::EReady);
	readyCount++;

	// there should be 1 Synch Change Notification request outstanding per phonebook
	TInt notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());


	// synchronise all phonebooks, with SIM card "access denied" condition
	test(syncChecker->SynchroniseIccAllPhonebooksL(*iDb, KErrNone, KErrAccessDenied) == KErrNone);


	// views with ICC entries should become ready now
	while ((readyCount < 3) && iViewEventQueue->ListenForEvent(10,event))
		{
		// Expecting EReady events
		switch (event.iEventType)
			{
		case TContactViewEvent::EItemAdded:
			test.Printf(_L("Event received - EItemAdded\n"));
			// ignore events from adding contacts
			break;

		case TContactViewEvent::EGroupChanged:
			test.Printf(_L("Event received - EGroupChanged (group Id %i)\n"), event.iContactId);
			// ignore events from adding contacts
			break;

		case TContactViewEvent::EReady:
			test.Printf(_L("Event received - EReady\n"));
			readyCount++;
			break;

		default:
			test.Printf(_L("Unexpected event received (%i)\n"), event.iEventType);
			test(0);
			}
		}


	test(readyCount == 3);

	// there should be 1 Synch Change Notification request outstanding per phonebook
	notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());

	// check number of entries in views (ICC locked)
	VerifyNumberOfEntriesInEachViewL(ESyncIccLocked);

	// Test Step 5. The PhoneBook Synch then indicates that the Cache is Valid.
	test(syncChecker->SynchroniseIccAllPhonebooksL(*iDb, KErrNone, KErrNone) == KErrNone);
	test.Printf(_L("PhoneBook Synch completed, cache now valid\n"));

	// Test Step 6. The two ICC views are updated (re-sorted) with the ICC entries.

	/* The 2 Views with ICC entries will each have 2 events:
	 * a) EUnavailable (0) => view is unavailable
	 * b) ESortOrderChanged (2) => view is Ready again
	 *
	 * For the 2 Views these can be interleaved
	 */
	while ((unavailableCount < 2) || (sortOrderChangedCount < 2))
		{
		TBool eventReceived = iViewEventQueue->ListenForEvent(1,event);

		if (eventReceived)
			{
			switch(event.iEventType)
				{
				case TContactViewEvent::EItemAdded:
					test.Printf(_L("Event received - EItemAdded (Id %i)\n"), event.iContactId);
					// ignore events from adding contacts
					break;

				case TContactViewEvent::EGroupChanged:
					test.Printf(_L("Event received - EGroupChanged (group Id %i)\n"), event.iContactId);
					// ignore events from adding contacts
					break;

				case TContactViewEvent::ESortOrderChanged:
					test.Printf(_L("Event received - ESortOrderChanged\n"));
					sortOrderChangedCount++;
					test(sortOrderChangedCount <= unavailableCount);
					break;

				case TContactViewEvent::EUnavailable:
					test.Printf(_L("Event received - EUnavailable\n"));
					unavailableCount++;
					test(unavailableCount <= 2);
					break;

				default:
					test.Printf(_L("Unexpected event received (%i)\n"), event.iEventType);
					test(EFalse);
					break;
				}
			}	
		}

	// there should be 1 Synch Change Notification request outstanding per phonebook
	notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());

	// check number of entries in views (ICC synchronised)
	VerifyNumberOfEntriesInEachViewL(ESynchronised);

	// Check the number of requests made to the plug-in
	// Expecting 6 calls in total from PhbkSync Watcher
	// (NotifySyncStateChange + IsSynchronised) * 3
	//

	// Test Step 7. Cleanup (deletes all entries)
	// delete non-Icc entries
	DeleteEntriesL(iNonIccContacts.Count());
	// delete Icc entries
	DeleteEntriesL(iIccContacts.Count(), ETrue);

	// no entries left
	test((iNonIccContacts.Count() == 0) && (iIccContacts.Count() == 0));

    // Flush all other events
	iViewEventQueue->Flush();
	}


/**
 Test View notifications when Phonebook Synch finishes
 Takes parameters to stimulate error conditions in the synchronisation

 Method: Start synchronisation then creates 3 views for Contacts, ICC Entries and Mixed Contacts & ICC Entries
 Count/check the View events.
 */
void CICCViewTest::SyncThenCreateViewCheckEventsL(TInt aSyncCompletionError, TInt aIsSyncError)
	{
    // Wait for local views to get ready
	TInt readyCount(0);
	TInt sortErrorCount(0);
    TContactViewEvent event;

	// synchronise all phonebooks
	test(syncChecker->SynchroniseIccAllPhonebooksL(*iDb, aSyncCompletionError, aIsSyncError) == KErrNone);


	// create the view to test
	CreateViewsL();

	// Contact only view should Ready
	test(iViewEventQueue->ListenForEvent(10,event));

	// Expecting EReady event
	test.Printf(_L("Event received (%i)\n"), event.iEventType);
	test(event.iEventType == TContactViewEvent::EReady);
	readyCount++;

	// there should be 1 Synch Change Notification request outstanding per phonebook
	TInt notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());

	// Phbk Watchers should be created when views were created - check number of plug-in calls
	//test(syncChecker->SyncMethodCallCountL() == (1 + 2 * iPhbkList.Count())); Johan


	// views with ICC entries should become ready now
	while (((readyCount + sortErrorCount) < 3) && iViewEventQueue->ListenForEvent(10,event))
		{
		// Expecting EReady events
		switch (event.iEventType)
			{
		case TContactViewEvent::EItemAdded:
			test.Printf(_L("Event received - EItemAdded\n"));
			// ignore events from adding contacts
			break;

		case TContactViewEvent::EGroupChanged:
			test.Printf(_L("Event received - EGroupChanged (group Id %i)\n"), event.iContactId);
			// ignore events from adding contacts
			break;

		case TContactViewEvent::EReady:
			test.Printf(_L("Event received - EReady\n"));
			readyCount++;
			break;

		case TContactViewEvent::ESortError:
			test.Printf(_L("Event received - ESortError\n"));
			if (aSyncCompletionError != KErrNone)
				{
				test(event.iInt == aSyncCompletionError);
				}
			else
				{
				test(event.iInt == aIsSyncError);
				}
			sortErrorCount++;
			break;

		default:
			test.Printf(_L("Unexpected event received (%i)\n"), event.iEventType);
			test(0);
			}
		}

	// either received all events or timed out waiting
	if (aSyncCompletionError == KErrNone && aIsSyncError == KErrNone)
		{
		// all 3 views should be ready
		test(readyCount == 3);

		// check number of entries in views (ICC is synchronised)
		VerifyNumberOfEntriesInEachViewL(ESynchronised);
		}
	else
		{
		// ICC views should have had sort errors
		test(sortErrorCount == 2);

		// check number of entries in views (ICC is not synchronised)
		VerifyNumberOfEntriesInEachViewL(ENotSynchronised);
		}


	// there should be 1 Synch Change Notification request outstanding per phonebook
	notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());

	// Check the number of requests made to the plug-in
	const TInt methodCount = syncChecker->SyncMethodCallCountL();
	if (aSyncCompletionError == KErrNone)
		{
		// There should be 1 call plus 2 per phonebook to the Phonebook Sync plug-in
		// PhonebookList + N * (NotifySyncStateChange, IsSynchronised)
		test(methodCount == 2 + (4 * iPhbkList.Count())); 
		}
	else
		{
		// There should be 1 call plus 2 per phonebook to the Phonebook Sync plug-in
		// PhonebookList + N * NotifySyncStateChange + N * 2 * (IsSynchronised, NotifySyncStateChange)
		test(methodCount == 1 + (5 * iPhbkList.Count())); 
		}


    // Flush all other events
	iViewEventQueue->Flush();
	}


/**
 Test Objective: Test Phonebook Synch for SIM card that starts Locked and becomes Unlocked.
 - tests View events and number of contacts in views

 Differs from test 4 in that the initial Phonebook Sync state (SIM locked) is indicated
 before the views are created.

 Method: Creates 3 views for Contacts, ICC Entries and Mixed Contacts & ICC Entries.
   Simulated Phonebook Synch fails with ICC card locked, subsequently card becomes unlocked
   and Phonebook Synch succeeds.
 
 Test steps:
 1. PhoneBook Synch fails with an error due to SIM card (ICC) locked.
 2. Create Views and some non-ICC Contacts.
 2. Contacts only view becomes ready
 4. The two ICC views become available without ICC entries.
 5. The PhoneBook Synch then indicates that the Cache is Valid.
 6. The two ICC views are updated (re-sorted) with the ICC entries.
 7. Cleanup (deletes all entries)
 */
void CICCViewTest::CheckEventsForTestSevenL()
	{
    // Wait for local views to get ready
    TContactViewEvent event;
	TInt readyCount(0);
	TInt unavailableCount(0);
	TInt sortOrderChangedCount(0);

	// Test step 1. synchronise all phonebooks, with SIM card "access denied" condition
	test(syncChecker->SynchroniseIccAllPhonebooksL(*iDb, KErrNone, KErrAccessDenied) == KErrNone);


	// Test Step 2. Create views and some non-ICC Contacts.
	CreateViewsL();
	test.Printf(_L("Creating 5 Contacts\n"));
	CreateEntriesNoVerifyL(5);

	// This While loop tests for steps:
	// 2. Contacts only view becomes ready, and 
	// 3. The two ICC views become available without ICC entries.

	/* Expected events:
	 * a) EReady (1) => non-ICC Contacts view is ready
	 * b&c) EReady (1) => ICC Views become Ready empty or with contacts only
	 */

	// views with ICC entries should become ready now
	while ((readyCount < 3) && iViewEventQueue->ListenForEvent(10,event))
		{
		// Expecting EReady events
		switch (event.iEventType)
			{
		case TContactViewEvent::EItemAdded:
			test.Printf(_L("Event received - EItemAdded\n"));
			// ignore events from adding contacts
			break;

		case TContactViewEvent::EGroupChanged:
			test.Printf(_L("Event received - EGroupChanged (group Id %i)\n"), event.iContactId);
			// ignore events from adding contacts
			break;

		case TContactViewEvent::EReady:
			test.Printf(_L("Event received - EReady\n"));
			readyCount++;
			break;

		default:
			test.Printf(_L("Unexpected event received (%i)\n"), event.iEventType);
			test(0);
			}
		}

	test(readyCount == 3);

	// there should be 1 Synch Change Notification request outstanding per phonebook
	TInt notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());

	// check number of entries in views (ICC locked)
	VerifyNumberOfEntriesInEachViewL(ESyncIccLocked);

	// Test Step 5. The PhoneBook Synch then indicates that the Cache is Valid.
	test(syncChecker->SynchroniseIccAllPhonebooksL(*iDb, KErrNone, KErrNone) == KErrNone);
	test.Printf(_L("PhoneBook Synch completed, cache now valid\n"));

	// Test Step 6. The two ICC views are updated (re-sorted) with the ICC entries.

	/* The 2 Views with ICC entries will each have 2 events:
	 * a) EUnavailable (0) => view is unavailable
	 * b) ESortOrderChanged (2) => view is Ready again
	 *
	 * For the 2 Views these can be interleaved
	 */
	while ((unavailableCount < 2) || (sortOrderChangedCount < 2))
		{
		TBool eventReceived = iViewEventQueue->ListenForEvent(1,event);

		if (eventReceived)
			{
			switch(event.iEventType)
				{
				case TContactViewEvent::EItemAdded:
					test.Printf(_L("Event received - EItemAdded (Id %i)\n"), event.iContactId);
					// ignore events from adding contacts
					break;

				case TContactViewEvent::EGroupChanged:
					test.Printf(_L("Event received - EGroupChanged (group Id %i)\n"), event.iContactId);
					// ignore events from adding contacts
					break;

				case TContactViewEvent::ESortOrderChanged:
					test.Printf(_L("Event received - ESortOrderChanged\n"));
					sortOrderChangedCount++;
					test(sortOrderChangedCount <= unavailableCount);
					break;

				case TContactViewEvent::EUnavailable:
					test.Printf(_L("Event received - EUnavailable\n"));
					unavailableCount++;
					test(unavailableCount <= 2);
					break;

				default:
					test.Printf(_L("Unexpected event received (%i)\n"), event.iEventType);
					test(EFalse);
					break;
				}
			}	
		}

	// there should be 1 Synch Change Notification request outstanding per phonebook
	notificationCount = syncChecker->TotalNotificationPendingCountL();
	test(notificationCount == iPhbkList.Count());

	// check number of entries in views (ICC synchronised)
	VerifyNumberOfEntriesInEachViewL(ESynchronised);

	// Check the number of requests made to the plug-in
	// Expecting 6 calls in total from PhbkSync Watcher
	// (NotifySyncStateChange + IsSynchronised) * 3
	//
	TInt methodCount = syncChecker->SyncMethodCallCountL();
	// Number of expected calls to syncChecker->MethodCalledL()
	test(methodCount = 6); 

	// Test Step 7. Cleanup (deletes all entries)
	// delete non-Icc entries
	DeleteEntriesL(iNonIccContacts.Count());
	// delete Icc entries
	DeleteEntriesL(iIccContacts.Count(), ETrue);

	// no entries left
	test((iNonIccContacts.Count() == 0) && (iIccContacts.Count() == 0));

    // Flush all other events
	iViewEventQueue->Flush();
	}


CICCViewTest::~CICCViewTest()
    {
	iIccContacts.Reset();
	iIccContacts.Close();
	iNonIccContacts.Reset();
	iNonIccContacts.Close();
    if (iContactsView)
		iContactsView->Close(*iViewEventQueue);
    if (iIccView)
		iIccView->Close(*iViewEventQueue);
    if (iContactsAndIccView)
		iContactsAndIccView->Close(*iViewEventQueue);

	delete iViewEventQueue;
    iViewSortOrder.Close();
	delete iEntry;
	delete iTemplate;
    delete iDb;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestDbName));
    }

void CICCViewTest::CreateEntriesL(TInt aNumberOfEntries, TBool aIsIccEntry)
	{
	iViewEventQueue->Flush();
	for	(TInt i=0; i<aNumberOfEntries; i++)
		{
		CreateEntryL(aIsIccEntry);
		}

	// wait for views to be updated
	TContactViewEvent event;
	test(iViewEventQueue->ListenForEvent(10,event));
	test((event.iEventType == TContactViewEvent::EItemAdded) || (event.iEventType == TContactViewEvent::EGroupChanged));

    // Eat away events
    do{}
	while (iViewEventQueue->ListenForEvent(1,event));

	// Check views
	VerifyNumberOfEntriesInEachViewL(ESynchronised);
	}

void CICCViewTest::CreateEntriesNoVerifyL(TInt aNumberOfEntries, TBool aIsIccEntry)
	{
	// add Contacts entries, but don't verify now
	for	(TInt i=0; i<aNumberOfEntries; i++)
		{
		if(aIsIccEntry)
			{
			test.Printf(_L("  Adding SIM contact - - -\n"));
			}
		CreateEntryL(aIsIccEntry);
		}
	}

void CICCViewTest::CreateEntryL(TBool aIsIccEntry)
	{
	TBool iccEntry=EFalse;
	if	(aIsIccEntry)
		{
		if (iTemplate == NULL)
			{
			TContactItemId templateId = iDb->ICCTemplateIdL(iPhbkList[0]);
			iTemplate = iDb->ReadContactL(templateId);
			}

		iEntry=CContactICCEntry::NewL(*iTemplate);
		iccEntry=ETrue;
		}
	else
		{
		iEntry=CContactCard::NewL();
		}

	HBufC* buf=HBufC::NewLC(32);
	TPtr bufPtr=buf->Des();
	SetRandomAlphaString(bufPtr,16);

	CContactItemFieldSet& fieldSet=iEntry->CardFields();
	const TInt pos=fieldSet.Find(KUidContactFieldGivenName);
	if (pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(bufPtr);

	iDb->AddNewContactL(*iEntry);
	if(iccEntry)
		{
		iIccContacts.Append(iEntry->Id());
		}
	else
		{
		iNonIccContacts.Append(iEntry->Id());
		}
	CleanupStack::PopAndDestroy(buf);

	delete iEntry;
	iEntry=NULL;
	}

void CICCViewTest::DeleteEntriesL(TInt aNumberOfEntries, TBool aIccEntry)
	{
	if(aIccEntry)
		{
		const TInt max = iIccContacts.Count();
		if(aNumberOfEntries > max)
			{
			aNumberOfEntries = max;
			}

		for(TInt i=0; i<aNumberOfEntries; i++)
			{
			iDb->DeleteContactL(iIccContacts[0]);
			iIccContacts.Remove(0);
			}
		}
	else
		{
		const TInt max = iNonIccContacts.Count();
		if(aNumberOfEntries > max)
			{
			aNumberOfEntries = max;
			}

		for(TInt i=0; i<aNumberOfEntries; i++)
			{
			iDb->DeleteContactL(iNonIccContacts[0]);
			iNonIccContacts.Remove(0);
			}
		}

    // Eat away events
	TContactViewEvent event;
    do{}
	while (iViewEventQueue->ListenForEvent(1,event));

	// Check views
	VerifyNumberOfEntriesInEachViewL(ESynchronised);
	}

void CICCViewTest::VerifyNumberOfEntriesInEachViewL(TSyncStatus aSyncStatus)
	{
	// any sync status -> check non Icc View
	TInt nonIcc = iContactsView->CountL();
	// number items in view matches the number of contacts added?
	test(nonIcc == iNonIccContacts.Count());


	// should ICC views be ready?
	if (aSyncStatus == ESynchronised || aSyncStatus == ESyncIccLocked)
		{
		TInt Icc=iIccView->CountL();
		TInt both=iContactsAndIccView->CountL();
		
		if (aSyncStatus == ESynchronised)
			{
			test(Icc == iInitialIccCount + iIccContacts.Count());
			test(both == iInitialIccCount + iNonIccContacts.Count() + iIccContacts.Count());
			}
		else // ESyncIccLocked -> no sync of ICC entries
			{
			test(Icc == iIccContacts.Count());
			test(both == iNonIccContacts.Count() + iIccContacts.Count());
			}
		}
	}

void CICCViewTest::SetRandomAlphaString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);

	for (TInt ii=0;ii<aLength;++ii)
		{
		aBuf[ii]=RandomCharCode('A','z',' ');
		}
	}

TText CICCViewTest::RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException)
	{
	TText charCode=0;

	do
		{
		charCode=RandomCharCode(aLowerBound,aUpperBound);
		}
		while (charCode==aException);

	return charCode;
	}

TText CICCViewTest::RandomCharCode(TText aLowerBound,TText aUpperBound)
	{
	TText charCode=STATIC_CAST(TText,(Math::Rand(iRandSeed)%(aUpperBound-aLowerBound))+aLowerBound);
	ASSERT(charCode>=aLowerBound && charCode<=aUpperBound);
	return charCode;
	}
	
void CheckForPhbkSyncPluginL()
	{
	test.Next(_L("Check for PhbkSync test plug-in"));

	RImplInfoPtrArray	implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);
	REComSession::ListImplementationsL(KUidEcomCntPhBkSyncInterface, implInfoArray);
	//Find implementations of KUidEcomCntPhBkSyncInterface
	TInt availCount = implInfoArray.Count(); 
	TInt count;
	for(count = 0; count < availCount; count++)
		{
		const TUid firstImplementationFound = implInfoArray[count]->ImplementationUid();
		CImplementationInformation *info = implInfoArray[count];
		test.Printf(_L("\n"));
		test.Printf(_L("PhbkSync plugin #%i, Implementation UID 0x%08X version %i\n"),
			count + 1, info->ImplementationUid(), info->Version());
		test.Printf(_L("Plugin name = \"%S\"\n"), &(info->DisplayName()));
		}

	// is telephony's plug-in in the list?
	for(count = 0; count < availCount; count++)
		{
		const TUid firstImplementationFound = implInfoArray[count]->ImplementationUid();
		CImplementationInformation *info = implInfoArray[count];
		if(info->DisplayName() == KTestPluginName)
			{
			test.Printf(_L("\n"));
			test.Printf(_L("This test has now loaded the test plugin"));
			test.Printf(_L("\n"));
			availCount = 1;
			break;
			}		
		
		if(info->DisplayName() == KPluginName)
			{
			test.Printf(_L("\n"));
			test.Printf(_L("This test only works with Contacts the test plugin and not the original phonebooksync plugin."));
			test.Printf(_L("Depending on the build to removed the plugin in different ways:"));
			test.Printf(_L("hardware - delete the line \"ECOM_PLUGIN(phbksyncplugin.dll,1020428C.rsc)\" from phbksync.iby"));
			test.Printf(_L("winscw - delete phbksyncplugin.dll from %epocroot%/epoc32/release/winscw/udeb or similarly named directory"));
			test.Printf(_L("\n"));
			test(0);  // stop
			break;
			}
		}

	// only continue test if there is exactly one plug-in present
	test(availCount == 1);	

	CleanupStack::PopAndDestroy(&implInfoArray);

	}

	
void CICCViewTest::TestOneOrFiveL(TBool aCreateViewBeforeSync)
    {	
	// Tests succesful Phonebook Synchronisation
    if (aCreateViewBeforeSync)
    	{
		CheckViewCreateAndSyncEventsL(KErrNone, KErrNone);


		test.Next(_L("Test creation and deletion of ICC entries"));
		// create some icc and non-icc entries
		CreateEntriesL(5);
		CreateEntriesL(4, ETrue);
		CreateEntriesL(2);

		// delete some icc and non-icc entries
		DeleteEntriesL(3);
		DeleteEntriesL(iIccContacts.Count(), ETrue);
		DeleteEntriesL(iNonIccContacts.Count());
    	}
    else
    	{
    	SyncThenCreateViewCheckEventsL(KErrNone, KErrNone);
    	}
	}

void CICCViewTest::TestTwoL()
    {
    // NB test only valid for creating view before Sync happens
    // test condition when notification request completes with an error
	CheckViewCreateAndSyncEventsL(KErrGeneral, KErrNone);
	}

void CICCViewTest::TestThreeOrSixL(TBool aCreateViewBeforeSync)
    {
    // test condition where notification completes okay, but IsSynchronisedL() leaves with an error
    if (aCreateViewBeforeSync)
    	{
		CheckViewCreateAndSyncEventsL(KErrNone, KErrGeneral);
    	}
    else
    	{
    	SyncThenCreateViewCheckEventsL(KErrNone, KErrGeneral);
    	}
	}

void CICCViewTest::TestFourL()
    {	
	CheckEventsForTestFourL();
	}

void CICCViewTest::TestSevenL()
    {	
	CheckEventsForTestSevenL();
	}

void CICCViewTest::TestConcurrenceL()
    {
    test(syncChecker->SynchroniseIccAllPhonebooksL(*iDb, KErrNone, KErrNone) == KErrNone);
    
    CContactRemoteView* iccRemoteView = CContactRemoteView::NewL(*iViewEventQueue, *iDb, iViewSortOrder, EICCEntriesOnly);
    
    TContactViewEvent event;
	test(iViewEventQueue->ListenForEvent(10,event));

	// Expecting EReady event
	test.Printf(_L("Event received (%i)\n"), event.iEventType);
	test(event.iEventType == TContactViewEvent::EReady);
	
	//Create and close another contact client session
	CContactDatabase* newDb = CContactDatabase::OpenL(KTestDbName);
    delete newDb;    	
    
    //Close the view to see if there is concurrence resource conflication.
    iccRemoteView->Close(*iViewEventQueue);

	//delete all icc and non-icc entries
	DeleteEntriesL(iIccContacts.Count(), ETrue);
    }
	
void RunTestOneL(const RArray<TUid> aPhonebookList)
	{
	test.Next(_L("Test view construction (create view before sync)"));

	CICCViewTest* viewTest=CICCViewTest::NewLC(aPhonebookList);
	viewTest->TestOneOrFiveL(ETrue);
	
	CleanupStack::PopAndDestroy(viewTest); 
	}

void RunTestTwoL(const RArray<TUid> aPhonebookList)
	{
	test.Next(_L("Test error handling: NotifySyncStateChange (create view before sync)"));

	CICCViewTest* viewTest=CICCViewTest::NewLC(aPhonebookList);
	viewTest->TestTwoL();
	CleanupStack::PopAndDestroy(viewTest);
	}

void RunTestThreeL(const RArray<TUid> aPhonebookList)
	{
	test.Next(_L("Test error handling: IsICCSynchronisedL (create view before sync)"));

	CICCViewTest* viewTest=CICCViewTest::NewLC(aPhonebookList);
	viewTest->TestThreeOrSixL(ETrue);
	CleanupStack::PopAndDestroy(viewTest);
	}

void RunTestFourL(const RArray<TUid> aPhonebookList)
	{
	test.Next(_L("Test error handling: SIM Locked and then Unlocked (create view before sync)"));

	CICCViewTest* viewTest=CICCViewTest::NewLC(aPhonebookList);
	viewTest->TestFourL();
	CleanupStack::PopAndDestroy(viewTest);
	}

void RunTestFiveL(const RArray<TUid> aPhonebookList)
	{
	test.Next(_L("Test view construction (sync before view creation)"));

	CICCViewTest* viewTest=CICCViewTest::NewLC(aPhonebookList);
	viewTest->TestOneOrFiveL(EFalse);
	
	CleanupStack::PopAndDestroy(viewTest); 
	}

void RunTestSixL(const RArray<TUid> aPhonebookList)
	{
	test.Next(_L("Test error handling: IsICCSynchronisedL (sync before view creation)"));

	CICCViewTest* viewTest=CICCViewTest::NewLC(aPhonebookList);
	viewTest->TestThreeOrSixL(EFalse);
	CleanupStack::PopAndDestroy(viewTest);
	}

void RunTestSevenL(const RArray<TUid> aPhonebookList)
	{
	test.Next(_L("Test error handling: SIM Locked and then Unlocked (sync before view creation)"));

	CICCViewTest* viewTest=CICCViewTest::NewLC(aPhonebookList);
	viewTest->TestSevenL();
	CleanupStack::PopAndDestroy(viewTest);
	}

void RunConcurrenceTestL(const RArray<TUid> aPhonebookList)
	{
	test.Next(_L("Test error handling: SIM Locked and then Unlocked (sync before view creation)"));

	CICCViewTest* viewTest = CICCViewTest::NewLC(aPhonebookList);
    viewTest->TestConcurrenceL();
	
	CleanupStack::PopAndDestroy(viewTest);
	}

/**

@SYMTestCaseID PIM-T-ICCVIEW-0001

*/
	
void DoTestsL()
	{
	test.Start(KTestName);

	CheckForPhbkSyncPluginL();

	syncChecker = CContactSyncChecker::NewL();
	syncChecker->ResetEverythingL();
	//syncChecker->EnableVerboseLoggingL();

	RArray<TUid>	phbkList;
	CleanupClosePushL(phbkList);

	for(TInt loop = 0; loop < 3; ++loop)
		{
		switch (loop)
			{
		case 0:
			phbkList.Reset();
			phbkList.AppendL(KUidIccGlobalAdnPhonebook);
			test.Next(_L("Test with GSM Global ADN phonebook"));
			break;

		case 1:
			phbkList.Reset();
			phbkList.AppendL(KUidUsimAppAdnPhonebook);
			test.Next(_L("Test with USIM App ADN phonebook"));
			break;

		case 2:
			phbkList.Reset();
			phbkList.AppendL(KUidIccGlobalAdnPhonebook);
			phbkList.AppendL(KUidUsimAppAdnPhonebook);
			test.Next(_L("Test with GSM Global and USIM App ADN phonebooks"));
			break;

		default:
			test(0);
			}

		test.Start(_L("Phonebook Sync tests"));
			// all tests work with any number of phonebooks
		RunTestOneL(phbkList);
		RunTestTwoL(phbkList);
		RunTestThreeL(phbkList);
		RunTestFourL(phbkList);
		
		RunTestFiveL(phbkList);
		RunTestSixL(phbkList);
		RunTestSevenL(phbkList);

		test.End();
		}
	
#ifdef CNTTEST_ICCREMOTEVIEW
	//The concurrence test is marked off because currently the cntsyncchecker has
	//problem in remoteview which will block the contact server. -- See INC112963
	test.Next(_L("Phonebook Sync Concurrence Test"));
	phbkList.Reset();
	phbkList.AppendL(KUidUsimAppAdnPhonebook);
	RunConcurrenceTestL(phbkList);
#endif //CNTTEST_ICCREMOTEVIEW
	
	test.End();
    test.Close();

	CleanupStack::PopAndDestroy(); // phbkList

	delete syncChecker;
	syncChecker = NULL;
	}

GLDEF_C TInt E32Main()
	{
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

    // Run the tests
    TRAPD(err, DoTestsL());

	if (syncChecker)
		{
		delete syncChecker;
		syncChecker = NULL;
		}

	test(err == KErrNone);

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
	return err;
    }
