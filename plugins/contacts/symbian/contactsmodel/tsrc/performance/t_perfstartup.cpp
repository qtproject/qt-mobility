/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
@SYMPREQ                  PREQ1192 PREQ1187
@SYMComponent             app-engines_cntmodel
@SYMAuthor                Simon Mellor, JamesCl
@SYMTestStatus            Implemented
@SYMTestType              CT

@SYMTestCaseDesc          Tests the performance of opening a corporate-profile database of 
						  1000 contacts, creating a remote view and retrieving some contacts.

@SYMTestActions           Measures the time taken to:
						  -- Open the database file with CContactDatabase::OpenL().
						  -- Create a RContactViewSortOrder and pass it to CContactRemoteView::NewL() 
						     to create a new remote view.
						  -- Retrieve contacts from the remote view using CContactRemoteView::ContactAtL().

@SYMTestExpectedResults   Test cases will run without leaving and will output timing information.

*/

#include "t_perfstartup.h"
	
/**
 Factory function for CStartUp
*/
CStartUp* CStartUp::NewLC(RTest& aTest)
	{
	CStartUp* self = new (ELeave) CStartUp(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CStartUp::CStartUp(RTest& aTest) : 
	CActive(CActive::EPriorityStandard),
	iTest(aTest)
	{
	}
	
void CStartUp::ConstructL()
	{
	_LIT(KStartUpTestTitle, "\nBeginning Startup Time testing...\n");
	iTest.Printf(KStartUpTestTitle);

	// This creates KNumTimings timing slots in iTimes
	// which are then accessed with the TTimings enums
	for (TInt i = 0; i < KNumTimings; ++i)
		{
		iTimes.AppendL(0);
		}

	CActiveScheduler::Add(this);
	}

CStartUp::~CStartUp()
	{
	Cancel();
	Cleanup();
	iTimes.Close();
	}
	
/**
 This will get called after the view is ready to be used. The only things left to do are clean up
 any resources and stop the active scheduler.
*/
void CStartUp::RunL()
	{
	if (iViewType == ERemoteContactsView) // groups view is empty so don't try to retrieve contacts
		{
		// Get the first 8 contacts like an app would in order to display them in the UI
		// and then get the ninth contacts like an app would if scrolling down one.
		RetrieveContactsL();
		}
	
	// Print results
	_LIT(KStartupTotalText, "Startup Total: %d s %03d\nof which:\n");
	_LIT(KOpenDbText, "    Open database: %d s %03d\n");
	_LIT(KCreateViewText, "    Create view: %d s %03d\n");
	_LIT(KGetFirstTwentyContactsText, "Get first 20 contacts: %d s %03d\n");
	_LIT(KGetTwentyFirstContactText, "Get 21st (scrolldown): %d s %03d\n");

	TBuf<255> buf;
	TInt startupTotal = iTimes[EOpenDatabase]  + iTimes[ECreateView];
	buf.AppendFormat(KStartupTotalText, (startupTotal / 1000000), ((startupTotal / 1000) % 1000));
	buf.AppendFormat(KOpenDbText, (iTimes[EOpenDatabase] / 1000000), ((iTimes[EOpenDatabase] / 1000) % 1000));
	buf.AppendFormat(KCreateViewText, (iTimes[ECreateView] / 1000000), ((iTimes[ECreateView] / 1000) % 1000));
	if (iViewType == ERemoteContactsView)
		{
		buf.AppendFormat(KGetFirstTwentyContactsText, (iTimes[EGetFirstTwenty] / 1000000), ((iTimes[EGetFirstTwenty] / 1000) % 1000));
		buf.AppendFormat(KGetTwentyFirstContactText, (iTimes[EGetTwentyFirst] / 1000000), ((iTimes[EGetTwentyFirst] / 1000) % 1000));

		// Put the time for retrieving the first twenty in the variable passed in 
		// to the DoTestL function to which iMs20FromViewPtr is pointing. Horrid.
		*iNumMsToGet20ItemsFromViewPtr = iTimes[EGetFirstTwenty];
		}
	iTest.Printf(buf);


	// Cleanup: Close the database and remote view
	Cleanup();
		
	// Stop the active scheduler
	CActiveScheduler::Stop();
	}

/**
 Implementation of CActive::DoCancel for CStartUp.
*/
void CStartUp::DoCancel()
	{
	}
	
void CStartUp::CompleteSelf()
	{
	TRequestStatus* pStat = &iStatus;
  	User::RequestComplete(pStat, KErrNone);
   	SetActive();
	}

/**
 Begin the start-up test case. Opening the database is synchcronous as is the first
 part of creating the remote view. The second part (where the view is populated
 with contacts) is asynchronous and finishes when CStartUp::HandleContactViewEvent
 is called. CStartUp::RunL then just cleans up once the test case is finished.
*/
void CStartUp::DoTestL(const TDesC& aDbName, TViewType aViewType, TInt& aNumMsToGet20ItemsFromView)
	{
	iViewType = aViewType;
	iNumMsToGet20ItemsFromViewPtr = &aNumMsToGet20ItemsFromView;
	_LIT(KRemoteContactViewText, "\n[Remote Contacts view]\n");
	_LIT(KRemoteGroupViewText,   "\n[Remote Groups view]\n");
	TBuf<32> buf;
	if (iViewType == ERemoteContactsView)
		{
		buf.Append(KRemoteContactViewText);
		}
	else
		{
		buf.Append(KRemoteGroupViewText); 
		}

	iTest.Printf(buf);

	// open database
	OpenDatabaseL(aDbName);
	
	// create view
	CreateRemoteViewL();
	CActiveScheduler::Start();
	}

/**
 Called when the remote contact view is ready to be used. Records the time taken to 
 asynchronously create the remote view.
*/
void CStartUp::HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent)
	{
	if (aEvent.iEventType == TContactViewEvent::EReady && &aView == iContactRemoteView)
		{
		// Called when the view is ready to use; complete the request status.
		iTimer.StopTimer();
		iTimes[ECreateView] = iTimer.Result();
		iTimer.ResetTimer();
		CompleteSelf();
		}
	}

/**
 Times the opening of the database.
*/
void CStartUp::OpenDatabaseL(const TDesC& aDbName)
	{	
	_LIT(KOpenDatabaseTitleFormat, "Opening Database: %S... \n");
	iTest.Printf(KOpenDatabaseTitleFormat, &aDbName);

	iTimer.ResetTimer();
	iTimer.StartTimer();
	iContactsDb = CContactDatabase::OpenL(aDbName);
	iTimer.StopTimer();

	iTimes[EOpenDatabase] = iTimer.Result();
	iTimer.ResetTimer();
	}

/**
 Times the synchronous part of remote view creation.
*/
void CStartUp::CreateRemoteViewL()
	{
	iTimer.ResetTimer();
	iTimer.StartTimer();

	RContactViewSortOrder viewSortOrder;
	CleanupClosePushL(viewSortOrder);
	
	viewSortOrder.AppendL(KUidContactFieldFamilyName);
	viewSortOrder.AppendL(KUidContactFieldGivenName);
	viewSortOrder.AppendL(KUidContactFieldCompanyName);

	if (iViewType == ERemoteContactsView)
		{
		iContactRemoteView = CContactRemoteView::NewL(*this, *iContactsDb, viewSortOrder, EContactsOnly);
		}
	else if (iViewType == ERemoteGroupsView)
		{
		iContactRemoteView = CContactRemoteView::NewL(*this, *iContactsDb, viewSortOrder, EGroupsOnly);
		}
	
	CleanupStack::PopAndDestroy(&viewSortOrder);
	}

/**
 Deletes any resources used by this class.
*/
void CStartUp::Cleanup()
	{
	if (iContactRemoteView) 
		{
		iContactRemoteView->Close(*this);
		iContactRemoteView = NULL;
		}

	if (iContactsDb) 
		{
		delete iContactsDb;
		iContactsDb = NULL;
		}   
	}
	
/**
 This gets the first 20 contacts from the remote view in the same way that 
 an app would to populate the UI view. Then it gets the 21st contacts from 
 the remote view in the same way that an app would when scrolling down one.
*/
void CStartUp::RetrieveContactsL()
	{
	iTimer.ResetTimer();
	iTimer.StartTimer();
	const TInt K20CViewContactsToGet(20);
	for (TInt ii = 0; ii < K20CViewContactsToGet; ++ii)
		{
		const CViewContact& viewContact = iContactRemoteView->ContactAtL(ii);
		}
	iTimer.StopTimer();
	iTimes[EGetFirstTwenty] = iTimer.Result();
	iTimer.ResetTimer();
	
	iTimer.StartTimer();
	const TInt K21stCViewContact(K20CViewContactsToGet + 1);
	const CViewContact& viewContact = iContactRemoteView->ContactAtL(K21stCViewContact);
	iTimes[EGetTwentyFirst] = iTimer.Result();
	iTimer.ResetTimer();
	}
