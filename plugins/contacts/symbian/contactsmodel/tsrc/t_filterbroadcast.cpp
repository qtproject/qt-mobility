/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <f32file.h>
#include "t_utils2.h"
#include "t_filterbroadcast.h"

//
// Constants.
//

_LIT(KTestName,"t_filterbroadcast");

_LIT(KDbFileName,"c:t_filterbroadcast.cdb");

_LIT(KViewName, "viewname");
const TInt KNumSmsContacts = 111;
// Time to wait for Views to be updated - 5s is very generous for any platform
const TInt KViewWaitTimeout = 5000000;


LOCAL_C void CleanupFilesL()
	{
    // delete the database file
	if (CContactDatabase::ContactDatabaseExistsL(KDbFileName) )
		{
		CContactDatabase::DeleteDatabaseL(KDbFileName);
		}
	}



//
// CTestConductor.
//

CTestConductor* CTestConductor::NewL(RTest& aTest)
	{
	CTestConductor* self=new(ELeave) CTestConductor(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->RunTestsL();
	CleanupStack::Pop();
	return self;
	}

CTestConductor::~CTestConductor()
	{
	delete iDb;
	delete iRandomGenerator;
	}

CTestConductor::CTestConductor(RTest& aTest) : iTest(aTest)
	{
	}

void CTestConductor::ConstructL()
	{
	iDb=CContactDatabase::ReplaceL(KDbFileName);
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);
	AddContactsL();
	}

void CTestConductor::AddContactL(TInt aBitwiseFilterType)
	{
	iTest.Printf(_L("Adding Contact"));
	iRandomGenerator->AddTypicalContactForFilterL(aBitwiseFilterType);
	}

void CTestConductor::AddContactsThatMatchFilterL(TInt aNumberOfContacts, TInt aFilter)
	{
	TInt counter = 0;
	for (counter = 0; counter < aNumberOfContacts; counter++)
		{
		iRandomGenerator->AddTypicalContactForFilterL(aFilter);
		}
	}
void CTestConductor::AddContactsL()
	{
	TInt bit=0;
	iTest.Printf(_L("Adding "));

	bit = CContactDatabase::ESmsable;
	AddContactsThatMatchFilterL(KNumSmsContacts, bit);
	}

void CTestConductor::RunTestsL()
	{
	CViewTester* tester = CViewTester::NewL(iTest, *iDb, *this);
	CleanupStack::PushL(tester);
	tester->StartTest();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(tester);

	// error from CViewTester ?
	User::LeaveIfError(iTestError);
	}


void CTestConductor::SetTestError(TInt aTestError)
	{
	iTestError = aTestError;
	}


//
// CViewTester.
//

CViewTester* CViewTester::NewL(RTest& aTest,CContactDatabase& aDb, CTestConductor& aTestConductor)
	{
	CViewTester* self=new(ELeave) CViewTester(aTest,aDb, aTestConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CViewTester::~CViewTester()
	{
	iTimer.Cancel();
	iFilteredView->Close(*this);
	iNamedRemoteView->Close(*this);
	iSortOrder[0].Close();
	iSortOrder[1].Close();
	}

CViewTester::CViewTester(RTest& aTest,CContactDatabase& aDb,CTestConductor& aTestConductor)
	: CActive(EPriorityStandard),iTest(aTest),iDb(aDb),iTestConductor(aTestConductor)
	{
	CActiveScheduler::Add(this);
	}

void CViewTester::ConstructL()
	{
	iSortOrder[0].AppendL(KUidContactFieldGivenName);
	iSortOrder[0].AppendL(KUidContactFieldFamilyName);
	iSortOrder[0].AppendL(KUidContactFieldCompanyName);

	iSortOrder[1].AppendL(KUidContactFieldCompanyName);
	iSortOrder[1].AppendL(KUidContactFieldFamilyName);
	iSortOrder[1].AppendL(KUidContactFieldGivenName);

	iTimer.CreateLocal();
	}



void CViewTester::StartTest()
	{
	iState = ECreate_RemoteView;
	iRemoteViewEvent = TContactViewEvent::EUnavailable;
	iFilteredViewEvent = TContactViewEvent::EUnavailable;
	CallRunLAgain();
	}

void CViewTester::RunL()
	{
	switch (iState)
		{
		case ECreate_RemoteView:
			iTest.Next(_L("Named Remote View creation View Ready event"));

			iNamedRemoteView = CContactNamedRemoteView::NewL( *this, KViewName, iDb, iSortOrder[0], EContactsOnly );
			// Wait for view to be created
			iTimer.After( iStatus, KViewWaitTimeout );
			SetActive();
			NextState();
		break;


		// It is an error for the RTimer to trigger before the state has changed.
		case EWait_RemoteView:
			iTest.Printf(_L("Timeout waiting for Remote View to be created"));
			TRAP_IGNORE(CleanupFilesL() );
			iTest( EFalse); // failed
		break;

		case ECreate_FilteredView:
			iTest.Next(_L("Filtered View creation View Ready event"));

			iFilteredView = CContactFilteredView::NewL( *this, iDb, *iNamedRemoteView, CContactDatabase::ESmsable );
			// Wait for view to be created
			iTimer.After( iStatus, KViewWaitTimeout );
			SetActive();
			NextState();
		break;


		// It is an error for the RTimer to trigger before the state has changed.
		case EWait_FilteredView:
			iTest.Printf(_L("Timeout waiting for Filtered View to be created"));
			TRAP_IGNORE(CleanupFilesL() );
			iTest( EFalse ); // failed
		break;

		case EChangeSortOrder:
			iTest.Next(_L("Re-sort Remote View and Filtered View, wait for view events"));

			iNamedRemoteView->ChangeSortOrderL( iSortOrder[1] );
			// Wait for views to be resorted
			iTimer.After( iStatus, KViewWaitTimeout );
			SetActive();
			NextState();
		break;


		// It is an error for the RTimer to trigger before the state has changed.
		case EWait_SortViewEvents:
			// Give information on what went wrong:
			iTest.Printf(_L("Test Timeout waiting for Filtered and Remote View to be resorted:\n"));
			switch(iRemoteViewEvent)
				{
				case TContactViewEvent::EReady:
					iTest.Printf(_L("  Named Remote View; sort not started\n"));
				break;

				case TContactViewEvent::EUnavailable:
					iTest.Printf(_L("  Named Remote View; sort not finished\n"));
				break;

				case TContactViewEvent::ESortOrderChanged:
					// Remote View Sorted & Ready
				break;

				default:
					// error in test
					User::Invariant();
				}

			switch(iFilteredViewEvent)
				{
				case TContactViewEvent::EReady:
					iTest.Printf(_L("  Filtered View; sort not started\n"));
				break;

				case TContactViewEvent::EUnavailable:
					iTest.Printf(_L("  Filtered View; sort not finished\n"));
				break;

				case TContactViewEvent::ESortOrderChanged:
					// Filtered View Sorted & Ready
				break;

				default:
					// error in test
					User::Invariant();
				}
			TRAP_IGNORE(CleanupFilesL() );
			iTest( EFalse ); // failed
		break;

		case EFinished:
			CActiveScheduler::Stop(); // we're done.
		break;

		default:
			ASSERT(EFalse);
		break;
		}
	}


TInt CViewTester::RunError(TInt aError)
	{
	iTestConductor.SetTestError(aError);

	switch (iState)
		{
		case ECreate_RemoteView: iTest.Printf(_L("Test failed at step Create_RemoteView (%i) with error %i"), iState, aError); break;
		case EWait_RemoteView: iTest.Printf(_L("Test failed at step Wait_RemoteView (%i) with error %i"), iState, aError); break;
		case ECreate_FilteredView: iTest.Printf(_L("Test failed at step Create_FilteredView (%i) with error %i"), iState, aError); break;
		case EWait_FilteredView: iTest.Printf(_L("Test failed at step Wait_FilteredView (%i) with error %i"), iState, aError); break;
		case EChangeSortOrder: iTest.Printf(_L("Test failed at step ChangeSortOrder (%i) with error %i"), iState, aError); break;
		case EWait_SortViewEvents: iTest.Printf(_L("Test failed at step Wait_SortViewEvents (%i) with error %i"), iState, aError);
			// Give information on what went wrong:
			iTest.Printf(_L("Test Timeout waiting for Filtered and Remote View to be resorted:\n"));
			switch(iRemoteViewEvent)
				{
				case TContactViewEvent::EReady:
					iTest.Printf(_L("  Named Remote View; sort not started\n"));
					break;

				case TContactViewEvent::EUnavailable:
					iTest.Printf(_L("  Named Remote View; sort not finished\n"));
					break;

				case TContactViewEvent::ESortOrderChanged:
					// Remote View Sorted & Ready
					break;

				default:
					break;
				}

			switch(iFilteredViewEvent)
				{
				case TContactViewEvent::EReady:
					iTest.Printf(_L("  Filtered View; sort not started\n"));
					break;

				case TContactViewEvent::EUnavailable:
					iTest.Printf(_L("  Filtered View; sort not finished\n"));
					break;

				case TContactViewEvent::ESortOrderChanged:
					// Filtered View Sorted & Ready
					break;

				default:
					break;
				}
			break;

		case EFinished: iTest.Printf(_L("Test failed at step Finished (%i) with error %i"), iState, aError); break;

		default: iTest.Printf(_L("Test failed at step %i with error %i"), iState, aError);
		}

	// stop test
	CActiveScheduler::Stop();
	return KErrNone;
	}



void CViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
/**
 * for each of the Remote and Filtered Views we wait for certain events
 * iState				  Event
 * EWait_RemoteView		  Named Remote View to become EReady
 * EWait_FilteredView	  Filtered View to become EReady
 * EWait_SortViewEvents   Named Remote View to become EUnavailable, and then ESortOrderChanged
 *                        Filtered View to become EUnavailable, and then ESortOrderChanged
 * The last expected event in each state calls NextState() for the test state machine.
 * Any other event, or any event in another test state, is an error.
 */
	{
	// view reference is correct?
	if(&aView != iNamedRemoteView && &aView != iFilteredView)
		{
		iTest.Printf(_L("test error: event for unknown View\n"));
		User::Invariant();
		}

	TBool expectedEvent = EFalse; // assume this is not the event we are looking for

	// is this the/an event we are expecting now?
	switch(aEvent.iEventType)
		{
		case TContactViewEvent::EReady:
			if(iState == EWait_RemoteView && &aView == iNamedRemoteView)
				{
				// Remote View is now ready, go to next test state
				iRemoteViewEvent = aEvent.iEventType;
				iTest.Printf(_L("event: Remote View ready\n"));
				expectedEvent = ETrue;
				NextState();
				}
			else if(iState == EWait_FilteredView && &aView == iFilteredView)
				{
				// Filter View is now ready, go to next test state
				iFilteredViewEvent = aEvent.iEventType;
				iTest.Printf(_L("event: Filtered View ready\n"));
				expectedEvent = ETrue;
				NextState();
				}
		break;

		case TContactViewEvent::EUnavailable:
			if(iState == EWait_SortViewEvents && &aView == iNamedRemoteView)
				{
				// Remote View, but correct event?
				if(iRemoteViewEvent == TContactViewEvent::EReady)
					{
					iRemoteViewEvent = aEvent.iEventType;
					iTest.Printf(_L("event: Remote View unavailable\n"));
					expectedEvent = ETrue;
					}
				}
			else if(iState == EWait_SortViewEvents && &aView == iFilteredView)
				{
				if(iFilteredViewEvent == TContactViewEvent::EReady)
					{
					iFilteredViewEvent = aEvent.iEventType;
					iTest.Printf(_L("event: Filtered View unavailable\n"));
					expectedEvent = ETrue;
					}
				}
		break;

		case TContactViewEvent::ESortOrderChanged:
			if(iState == EWait_SortViewEvents)
				{
				if(&aView == iNamedRemoteView && iRemoteViewEvent == TContactViewEvent::EUnavailable)
					{
					iRemoteViewEvent = aEvent.iEventType;
					iTest.Printf(_L("event: Remote View sort order changed\n"));
					expectedEvent = ETrue;
					}
				else if(&aView == iFilteredView && iFilteredViewEvent == TContactViewEvent::EUnavailable)
					{
					iFilteredViewEvent = aEvent.iEventType;
					iTest.Printf(_L("event: Filtered View sort order changed\n"));
					expectedEvent = ETrue;
					}

				// both views now Re-Sorted ?
				if(iRemoteViewEvent == TContactViewEvent::ESortOrderChanged &&
						iFilteredViewEvent == TContactViewEvent::ESortOrderChanged)
					{
					NextState();
					}
				}
		break;

		default:
			// unexpected event
		break;
		}


	if(!expectedEvent)
		{
		// valid View, but unexpected event
		iTest.Printf(_L("test error: unexpected View event\n"));
		TRAP_IGNORE(CleanupFilesL() );
		iTest( EFalse );
		}

	return;
	}



void CViewTester::NextState()
	{
	TState oldState(iState);

	iState = static_cast<TState>( static_cast<TInt>(iState) + 1 );

	// moving from an event with timeout?
	if(oldState == EWait_RemoteView || oldState == EWait_FilteredView || oldState == EWait_SortViewEvents)
		{
		// Cancel view wait timer, causes RunL to run
		iTimer.Cancel();
		}
	}

void CViewTester::CallRunLAgain()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CViewTester::DoCancel()
	{
	}

/**

@SYMTestCaseID     PIM-T-FILTERBROADCAST-0001

*/

void doMainL()
	{
	RTest test( KTestName );
	CTestConductor* testConductor=NULL;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDbFileName, EFileTypeCnt);

	test.Start(_L("@SYMTESTCaseID:PIM-T-FILTERBROADCAST-0001 "));


	__UHEAP_MARK;
	testConductor = CTestConductor::NewL(test);

	delete testConductor;
	__UHEAP_MARKEND;

	CleanupStack::PopAndDestroy(2);

	test.Printf(_L("\n"));
	test.End();
	test.Close();
	}
//
// Main.
//

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	// The dummy is just to get rid of linker warning LNK4089 when building for WINS
	TVolumeInfo Dummy;
	Dummy.iSize = 1;
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			TRAP_IGNORE(doMainL() );
			// cleanup files even if doMainL() leaves
			TRAP_IGNORE(CleanupFilesL() );
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
