/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@SYMPREQ                  PREQ1187
@SYMComponent             app-engines_cntmodel
@SYMAuthor                Simon Mellor, JamesCl
@SYMTestStatus            Implemented
@SYMTestType              CT

@SYMTestCaseDesc          Tests the performance of resorting a remote view on a corporate-profile
						  database of 1000 contacts.

@SYMTestActions           Creates a named remote view on the contacts database and then times
						  resorting, first with a the original three sort fields (given name,
						  family name and company name) from the identity table and then with the
						  telephone number in place of the company name so as to have a field not
						  in the original view and not in the same table as the other fields. Uses:
						  -- CContactDatabase::OpenL()
						  -- CContactNamedRemoteView::NewL()
						  -- CContactNamedRemoteView::ChangeSortOrderL()

@SYMTestExpectedResults   Test cases will run without leaving and will output timing information.

*/

#include "t_perfresortview.h"

/**
 Factory function for CViewResort
*/
CViewResort* CViewResort::NewLC(RTest& aTest)
	{
	CViewResort* self = new (ELeave) CViewResort(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CViewResort::CViewResort(RTest& aTest) :
	CActive(CActive::EPriorityStandard),
	iTest(aTest)
	{
	}

void CViewResort::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CViewResort::~CViewResort()
	{
	Cancel();
	Cleanup();
	}

/**
 This will get called after the view is ready to be used.
 The resorts are performed and then later the clean up of
 any resources is done and the active scheduler is stopped.
*/
void CViewResort::RunL()
	{
	TInt result;
	switch 	(iViewState)
		{
		case ECreatedView:
			// View created so start resort by name
			_LIT(KNameStartMsg, "\nBeginning View Resort by Name testing...\n");
			iTest.Printf(KNameStartMsg);
			iTimer.ResetTimer();
			iTimer.StartTimer();
			ResortL();
			break;
		case EResortedViewByName:
			// Resort by name completed so output results
			iTimer.StopTimer();
			result = iTimer.Result();
			iTimer.ResetTimer();
			_LIT(KNameSortResultsFormat, "Remote contacts view resort by name took: %d s %03d\n");
			iTest.Printf(KNameSortResultsFormat, (result / 1000000), ((result / 1000) % 1000));

			// Start resort by number
			_LIT(KNumStartMsg, "\nBeginning View Resort by Phone Number testing...\n");
			iTest.Printf(KNumStartMsg);
			iTimer.StartTimer();
			ResortL();
			break;
		case EResortedViewByNumber:
			// Resort by number completed so output results
			iTimer.StopTimer();
			result = iTimer.Result();
			iTimer.ResetTimer();
			_LIT(KNumberSortResultsFormat, "Remote contacts view resort by phone number took: %d s %03d\n");
			iTest.Printf(KNumberSortResultsFormat, (result / 1000000), ((result / 1000) % 1000));

			// Close the database and remote view and stop the active scheduler
			Cleanup();
			CActiveScheduler::Stop();
		}
	}

/**
 Implementation of CActive::DoCancel for CViewResort.
*/
void CViewResort::DoCancel()
	{
	}


/**
 Begin the resorting test case.
*/
void CViewResort::DoTestL(const TDesC& aDbName)
	{
	iContactsDb = CContactDatabase::OpenL(aDbName);
	CreateRemoteViewL(); //starts the testing. See RunL() for rest of the test control.
	CActiveScheduler::Start();
	}

/**
 Callback from the remote view when an operation is completed.
*/
void CViewResort::HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent)
	{
	if ((aEvent.iEventType == TContactViewEvent::EReady ||
		 aEvent.iEventType == TContactViewEvent::ESortOrderChanged) && &aView == iNamedRemoteView)
		{
		// Called when the view is ready to use; complete the request status.
		TRequestStatus* pStat = &iStatus;
	  	User::RequestComplete(pStat, KErrNone);
	   	SetActive();
		}
	}


/**
 Creates a named remote view
*/
void CViewResort::CreateRemoteViewL()
	{
	RContactViewSortOrder viewSortOrder;
	CleanupClosePushL(viewSortOrder);
	viewSortOrder.AppendL(KUidContactFieldFamilyName);
	viewSortOrder.AppendL(KUidContactFieldGivenName);
	viewSortOrder.AppendL(KUidContactFieldCompanyName);

	_LIT(KViewName, "resortTestRemoteNamedView");
	iNamedRemoteView = CContactNamedRemoteView::NewL(*this, KViewName, *iContactsDb, viewSortOrder, EContactsOnly);

	CleanupStack::PopAndDestroy(&viewSortOrder);
	iViewState = ECreatedView;
	}

/**
 Deletes any resources used by this class.
*/
void CViewResort::Cleanup()
	{
	if (iNamedRemoteView)
		{
		iNamedRemoteView->Close(*this);
		iNamedRemoteView = NULL;
		}

	if (iContactsDb)
		{
		delete iContactsDb;
		iContactsDb = NULL;
		}
	}

/**
 Resorts the remote view.
*/
void CViewResort::ResortL()
	{
	RContactViewSortOrder viewSortOrder;
	CleanupClosePushL(viewSortOrder);

	switch 	(iViewState)
		{
		case ECreatedView:
			// sort in a different order from the original view sort order
			// but with the same three fields from the identity table
			viewSortOrder.AppendL(KUidContactFieldCompanyName);
			viewSortOrder.AppendL(KUidContactFieldFamilyName);
			viewSortOrder.AppendL(KUidContactFieldGivenName);
			iNamedRemoteView->ChangeSortOrderL(viewSortOrder);
			iViewState = EResortedViewByName;
			break;
		case EResortedViewByName:
			// sorting in a different order from the original view sort order
			// but with the phonenumber field from the phone table
			viewSortOrder.AppendL(KUidContactFieldPhoneNumber);
			viewSortOrder.AppendL(KUidContactFieldGivenName);
			viewSortOrder.AppendL(KUidContactFieldFamilyName);
			iNamedRemoteView->ChangeSortOrderL(viewSortOrder);
			iViewState = EResortedViewByNumber;
			break;
		}
    CleanupStack::PopAndDestroy(&viewSortOrder);
	}

