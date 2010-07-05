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
#include <e32math.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_utils.h"
#include "t_groupview_unfiled_profile.h"

//
// Macro's
//
#define CLEAR_ALL_CLOCKS CCntTest::ProfileReset(0, 30)
#define START_CLOCK CCntTest::ProfileReset(macroTimer, 1); CCntTest::ProfileStart(macroTimer++)
#define STOP_CLOCK CCntTest::ProfileEnd(--macroTimer)
#define STOP_CLOCK_TIME( profile ) CCntTest::ProfileEnd(--macroTimer); CCntTest::ProfileResult(&(profile), macroTimer, 1)
#define GET_LAST_CLOCK_TIME( profile ) CCntTest::ProfileResult(&profile, macroTimer, 1)

//
// Constants.
//

_LIT(KTestName,"t_groupview_unfiled_profile");
_LIT(KLogFileName,"t_groupview_unfiled_profile.log");

_LIT(KDbFileName,"c:t_groupview_unfiled_profile.cdb");

_LIT(KRemoteViewName,"RemoteView");
_LIT(KTextDefSeparator,"");

RTest test(KTestName);


const TInt KNumContacts=500;
const TInt KNumContactsInGroupOne=250;
const TInt KMaxContactsToAddInGroupTest = 50;
const TInt KMaxContactsToRemoveInGroupTest = 50;

_LIT(KGroupOneName,"GroupOne");

//
// File scope variable
//
TInt macroTimer = 1;

//
// CTestConductor.
//

CTestConductor* CTestConductor::NewL()
	{
	CTestConductor* self=new(ELeave) CTestConductor();
	CleanupStack::PushL(self);
	self->ConstructL();
	self->RunTestsL();
	CleanupStack::Pop();
	return self;
	}

CTestConductor::~CTestConductor()
	{
	delete iLog;
	delete iDb;
	delete iRandomGenerator;
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbFileName));
	iFs.Close();	
	}

CTestConductor::CTestConductor() {}

void CTestConductor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iLog=CLog::NewL(test,KLogFileName);
	iDb=CContactDatabase::ReplaceL(KDbFileName);
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);
	AddContactsL();
	}

void CTestConductor::AddContactsL()
	{
	iTotalContacts=0;
	TInt ii;
	for (ii=0;ii<KNumContacts;ii++)
		{
		iRandomGenerator->AddTypicalRandomContactL();
		iTotalContacts++;
		test.Printf(_L("Adding %d \n"),ii);
		}
	CreateGroupTestDataL();
	}

void CTestConductor::CreateGroupTestDataL()
	{
	CContactGroup* groupOne = static_cast<CContactGroup*>(iDb->CreateContactGroupL(KGroupOneName));
	CContactIdArray* idsInGroupViewOne = NULL;

	CleanupStack::PushL(groupOne);

	iGroupOneId = groupOne->Id();
	for (TInt ii=1;ii<=KNumContactsInGroupOne;++ii)
		{
		test.Printf(_L("Making Group Contact %d \n"),ii);
		iDb->AddContactToGroupL(ii,iGroupOneId);
		}

	CleanupStack::PopAndDestroy(groupOne);

	groupOne = static_cast<CContactGroup*>(iDb->ReadContactL(iGroupOneId));
	CleanupStack::PushL(groupOne);

	idsInGroupViewOne = groupOne->ItemsContainedLC();
	CleanupStack::PopAndDestroy(idsInGroupViewOne);
	CleanupStack::PopAndDestroy(groupOne);
	}

void CTestConductor::AddSingleContactL()
	{
	test.Printf(_L("Adding Contact [%d] "), ++iTotalContacts);
	iRandomGenerator->AddTypicalRandomContactL();
	test.Printf(_L(" \n"));
	}


void CTestConductor::PrintTimeTaken(TInt aTime)
	{
	iLog->LogLine(_L(" Time Taken [%d.%dus] \n"), aTime/1000000, aTime%1000000);
	}

void CTestConductor::RunTestsL()
	{
	CGroupViewTester* tester=CGroupViewTester::NewL(*iLog,*iDb, *this);
	CleanupStack::PushL(tester);
	CActiveScheduler::Start();
	CleanupStack::Pop(tester);
	// error from CGroupViewTester?
	User::LeaveIfError(iTestError);
	}

void CTestConductor::SetTestError(TInt aTestError)
	{
	iTestError = aTestError;
	}


//
// CGroupViewTester.
//

CGroupViewTester* CGroupViewTester::NewL(CLog& aLog,CContactDatabase& aDb, CTestConductor& aTestConductor)
	{
	CGroupViewTester* self=new(ELeave) CGroupViewTester(aLog,aDb, aTestConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CGroupViewTester::~CGroupViewTester()
	{
	iLocalView->Close(*this);
	iNamedRemoteView->Close(*this);
	iSortOrder_1.Close();
	delete iTextDef;
	}

CGroupViewTester::CGroupViewTester(CLog& aLog,CContactDatabase& aDb, CTestConductor& aTestConductor)
	: CActive(EPriorityStandard),iLog(aLog),iDb(aDb),iTestConductor(aTestConductor),iCurrentTest(EPreStartTest)
	{
	CActiveScheduler::Add(this);
	}

void CGroupViewTester::ConstructL()
	{
	iSortOrder_1.AppendL(KUidContactFieldGivenName);
	iSortOrder_1.AppendL(KUidContactFieldFamilyName);
	iSortOrder_1.AppendL(KUidContactFieldCompanyName);

	iTextDef=CContactTextDef::NewL();
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));

	NextTest();
	}

void CGroupViewTester::RunL()
	{
	TCntProfile profile;

	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iLog.LogLine(_L("=== Create local view \n"));
			iLocalView=CContactLocalView::NewL(*this,iDb,iSortOrder_1,EContactAndGroups);
			break;

		case ECreateRemoteNamedView:
			iLog.LogLine(_L("=== Create Remote named view \n"));
			iNamedRemoteView = CContactNamedRemoteView::NewL(*this,KRemoteViewName,iDb,iSortOrder_1,EContactAndGroups);
			break;

		case EStartLocalTests:
			iLog.LogLine(_L("=== Start Local Tests \n"));
			CLEAR_ALL_CLOCKS;
			iLog.LogLine(_L(">> Start Clock [%d] \n"), macroTimer);
			START_CLOCK;
			iTester = CUnfiledGroupViewTester::NewL(iLog, iDb, iTestConductor, *iLocalView, iStatus);
			SetActive();
			iCurrentTest = EEndLocalTests;
			iTester->StartL();
			break;

		case EEndLocalTests:
			STOP_CLOCK_TIME( profile );
			iLog.LogLine(_L("<< Stop  Clock [%d] \n"), macroTimer);
			iLog.LogLine(_L("== End Local Tests Time to complete \n"));
			delete iTester; iTester = NULL;
			iTestConductor.PrintTimeTaken(profile.iTime);
			NextTest();
			break;

		case EStartRemoteTests:
			iLog.LogLine(_L("=== Start Remote Tests \n"));
			CLEAR_ALL_CLOCKS;
			iLog.LogLine(_L(">> Start Clock [%d] \n"), macroTimer);
			START_CLOCK;
			iTester = CUnfiledGroupViewTester::NewL(iLog, iDb, iTestConductor, *iNamedRemoteView, iStatus);
			SetActive();
			iCurrentTest = EEndRemoteTests;
			iTester->StartL();
			break;

		case EEndRemoteTests:
			STOP_CLOCK_TIME( profile );
			iLog.LogLine(_L("<< Stop  Clock [%d] \n"), macroTimer);
			iLog.LogLine(_L("== End Remote Tests Time to complete\n"));
			delete iTester; iTester = NULL;
			iTestConductor.PrintTimeTaken(profile.iTime);
			NextTest();
			break;


		case ENumTests:
			iLog.LogLine(_L("==== No Time to report, tests over ! \n"));
			iLog.LogLine(_L("==== Group View Tests Finished, All Passed...\n"));
			CActiveScheduler::Stop();
			delete this;
			break;
		default:
			ASSERT(EFalse);
			break;
		}
	}

TInt CGroupViewTester::RunError(TInt aError)
	{
	// propagate error
	iTestConductor.SetTestError(aError);

	switch (iCurrentTest)
		{
		case ECreateLocalView: test.Printf(_L("Test failed at step CreateLocalView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateRemoteNamedView: test.Printf(_L("Test failed at step CreateRemoteNamedView (%i) with error %i"), iCurrentTest, aError); break;
		case EStartLocalTests: test.Printf(_L("Test failed at step StartLocalTests (%i) with error %i"), iCurrentTest, aError); break;
		case EEndLocalTests: test.Printf(_L("Test failed at step EndLocalTests (%i) with error %i"), iCurrentTest, aError); break;
		case EStartRemoteTests: test.Printf(_L("Test failed at step StartRemoteTests (%i) with error %i"), iCurrentTest, aError); break;
		case EEndRemoteTests: test.Printf(_L("Test failed at step EndRemoteTests (%i) with error %i"), iCurrentTest, aError); break;

		case ENumTests: test.Printf(_L("Test failed at step NumTests (%i) with error %i"), iCurrentTest, aError); break;

		default: test.Printf(_L("Test failed at step %i with error %i"), iCurrentTest, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}

void CGroupViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			test(iLocalView==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
				NextTest();
			break;
		case ECreateRemoteNamedView:
			test(iNamedRemoteView==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
				NextTest();
			break;
		case EStartLocalTests:
		case EEndLocalTests:
		case EStartRemoteTests:
		case EEndRemoteTests:
		case ENumTests:
			break;
		default:
			test(EFalse);
			break;
		}

	}

void CGroupViewTester::NextTest()
	{
	iCurrentTest = static_cast<TTest>( static_cast<TInt>(iCurrentTest) + 1 ) ;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CGroupViewTester::ExceriseViewL(CContactViewBase& aView)
	{
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	for (TInt ii=0;ii<numItems;++ii)
		{
		if (ii==numItems-1)
			{
			lastId=aView.AtL(ii);
			}
		iDb.ReadContactTextDefL(aView.AtL(ii),iScratchBuf,iTextDef);
		iLog.LogLineNoEcho(iScratchBuf);
		iScratchBuf.SetLength(0);
		}

	test(aView.FindL(lastId)==numItems-1);
	}


void CGroupViewTester::TestGroupViewSortOrderL(CContactGroupView& aView)
	{
	iLog.LogLine(_L("<TestGroupViewSortOrderL> "));
	const TInt numItems=aView.CountL();
	iLog.LogLine(_L("aView.CountL = [%d] \n"), numItems );
	for (TInt ii=0;ii<numItems;++ii)
		{
		if(ii!=numItems-1)//not last item
			{
			TContactIdWithMapping first = (aView.iGroupContacts)[ii];
			TContactIdWithMapping second = (aView.iGroupContacts)[ii+1];
			test(first.iMapping < second.iMapping);
			}
		}
	}


void CGroupViewTester::DoCancel()
	{
	}


//
//  CUnfiledGroupViewTester
//
CUnfiledGroupViewTester* CUnfiledGroupViewTester::NewL(CLog& aLog, CContactDatabase& aDb, CTestConductor& aTestConductor, CContactViewBase& aView, TRequestStatus& aClientStatus)
	{
	CUnfiledGroupViewTester* self = new (ELeave) CUnfiledGroupViewTester(aLog, aDb, aTestConductor, aView, aClientStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CUnfiledGroupViewTester::CUnfiledGroupViewTester(CLog& aLog, CContactDatabase& aDb, CTestConductor& aTestConductor, CContactViewBase& aView, TRequestStatus& aClientStatus)
	: CActive(EPriorityStandard),
	iLog(aLog),
	iDb(aDb),
	iView(aView),
	iTestConductor(aTestConductor),
	iClientStatus(aClientStatus)
	{
	CActiveScheduler::Add(this);
	iCurrentTest = EPreStartTest;
	}

CUnfiledGroupViewTester::~CUnfiledGroupViewTester()
	{
	delete iGroupViewUnfiled;
	}

void CUnfiledGroupViewTester::ConstructL()
	{
	}

void CUnfiledGroupViewTester::DoCancel()
	{
	}

void CUnfiledGroupViewTester::StartL()
	{
	iClientStatus = KRequestPending;
	NextTest();
	}

void CUnfiledGroupViewTester::NextTest()
	{
	iCurrentTest = static_cast<TTest>( static_cast<TInt>(iCurrentTest) + 1 ) ;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CUnfiledGroupViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	TCntProfile profile;
	TInt sizeOfView = 0;

	switch (iCurrentTest)
		{
		case ECreateUnfiledGroupView:
			test(iGroupViewUnfiled==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
			// Stop timer.
			STOP_CLOCK_TIME( profile );
			iLog.LogLine(_L("<< Stop  Clock [%d] \n"), macroTimer);
			iLog.LogLine(_L("=== Time Taken to create view "));
			iTestConductor.PrintTimeTaken(profile.iTime);
			NextTest();
		break;

		case ETestAddUnfiledContacts:
		case ETestRemoveUnfiledContacts:
			TRAPD(err, sizeOfView = iGroupViewUnfiled->CountL() );
			test(err == KErrNone);
			if (sizeOfView == iSizeOfViewToExpect)
				{
				// Stop timer.
				STOP_CLOCK_TIME( profile );
				iLog.LogLine(_L("=== Time Taken handle all Messages "));
				iLog.LogLine(_L("<< Stop  Clock [%d] \n"), macroTimer);
				iTestConductor.PrintTimeTaken(profile.iTime);
				iSizeOfViewToExpect = 0;
				NextTest();
				}
		break;


		case ENumTests:
			break;
		default:
			test(EFalse);
			break;
		}

	}

void CUnfiledGroupViewTester::RunL()
	{
	TContactItemId deletedId;
	TCntProfile profile;
	TRequestStatus* clientStatus = &iClientStatus;
	TInt counter = 0;
	TInt currentSizeOfView = 0;

	switch (iCurrentTest)
		{
		case ECreateUnfiledGroupView:
			iLog.LogLine(_L("=== Create Unfiled group view"));
			iLog.LogLine(_L(">> Start Clock [%d] \n"), macroTimer);
			START_CLOCK;
			iGroupViewUnfiled=CContactGroupView::NewL(iDb,iView,*this,KNullContactId,CContactGroupView::EShowContactsNotInAnyGroup);
		break;

		case ETestAddUnfiledContacts:
			iLog.LogLine(_L("==== Add Un-filed Contacts to group"));

			// Contact added here.
			// Start timer
			iLog.LogLine(_L(">> Start Clock [%d] \n"), macroTimer);
			iSizeOfViewToExpect = iGroupViewUnfiled->CountL() + KMaxContactsToAddInGroupTest;

			START_CLOCK;
			for (counter= 0; counter< KMaxContactsToAddInGroupTest; counter++)
			{
				iTestConductor.AddSingleContactL();
			}
			// Stop timer.
			STOP_CLOCK_TIME( profile );
			iLog.LogLine(_L("<< Stop  Clock [%d] \n"), macroTimer);
			iTestConductor.PrintTimeTaken(profile.iTime);
			// Start timer
			iLog.LogLine(_L(">> Start Clock [%d] \n"), macroTimer);
			START_CLOCK;
		break;

		case ETestRemoveUnfiledContacts:
			iLog.LogLine(_L("==== Remove Un-filed Contacts to group"));

			// Contact added here.
			// Start timer
			currentSizeOfView = iGroupViewUnfiled->CountL();
			iSizeOfViewToExpect = currentSizeOfView - KMaxContactsToRemoveInGroupTest;
			currentSizeOfView--;

			iLog.LogLine(_L(">> Start Clock [%d] \n"), macroTimer);
			START_CLOCK;

			for (counter = 0; counter < KMaxContactsToRemoveInGroupTest; counter++)
			{
				deletedId = iGroupViewUnfiled->AtL( currentSizeOfView - counter );
				iDb.DeleteContactL(deletedId);
				iLog.LogLine(_L("==== Removed contact from DB [%d]"), currentSizeOfView - counter);
			}
			// Stop timer.
			STOP_CLOCK_TIME( profile );
			iLog.LogLine(_L("<< Stop  Clock [%d] \n"), macroTimer);
			iTestConductor.PrintTimeTaken(profile.iTime);
			// Start timer
			iLog.LogLine(_L(">> Start Clock [%d] \n"), macroTimer);
			START_CLOCK;
		break;

		case ENumTests:
			iLog.LogLine(_L("==== Group View Tests Finished, All Passed...\n"));
			User::RequestComplete(clientStatus,KErrNone);
			break;
		default:
			ASSERT(EFalse);
			break;
		}
	}

TInt CUnfiledGroupViewTester::RunError(TInt aError)
	{
	// propagate error
	iTestConductor.SetTestError(aError);

	switch (iCurrentTest)
		{
		case ECreateUnfiledGroupView: test.Printf(_L("Test failed at step CreateUnfiledGroupView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestAddUnfiledContacts: test.Printf(_L("Test failed at step TestAddUnfiledContacts (%i) with error %i"), iCurrentTest, aError); break;
		case ETestRemoveUnfiledContacts: test.Printf(_L("Test failed at step TestRemoveUnfiledContacts (%i) with error %i"), iCurrentTest, aError); break;

		case ENumTests: test.Printf(_L("Test failed at step NumTests (%i) with error %i"), iCurrentTest, aError); break;

		default: test.Printf(_L("Test failed at step %i with error %i"), iCurrentTest, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}





//
// Main.
//
/**

@SYMTestCaseID     PIM-T-GROUPVIEW-UNFILED-PROFILE-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	RProcess().SetPriority(EPriorityBackground);
	CActiveScheduler* scheduler=new CActiveScheduler;
	test.Start(_L("@SYMTESTCaseID:PIM-T-GROUPVIEW-UNFILED-PROFILE-0001 Test Groupview unfiled profile"));

	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* testConductor=NULL;
			TRAPD(err,testConductor=CTestConductor::NewL());
			test(err == KErrNone);
			delete testConductor;
			delete cleanup;
			}
		delete scheduler;
		}
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }
