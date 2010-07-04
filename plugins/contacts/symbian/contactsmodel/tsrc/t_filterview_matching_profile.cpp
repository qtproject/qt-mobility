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
#include <cntviewbase.h>
#include <cntview.h>
#include "t_utils2.h"
#include "t_utils.h"
#include "t_filterview_matching_profile.h"
#include "cfindtext.h"

_LIT(KTestName,"t_filterview");
_LIT(KLogFileName,"t_filterview.log");

_LIT(KDbFileName,"c:t_filterview.cdb");

_LIT(KRemoteViewName,"RemoteView");
_LIT(KTextDefSeparator,"");

const TInt KNumSmsContacts=250;
const TInt KNumWorkEmailContacts=250;
const TInt KNumHomeEmailContacts=250;
const TInt KNumLandlineContacts=250;
const TInt KNumFaxContacts=250;


LOCAL_C void CleanupContactArray(TAny* aArray)
	{
	RPointerArray<CViewContact>* array=REINTERPRET_CAST(RPointerArray<CViewContact>*,aArray);
	if (array)
		array->ResetAndDestroy();
	array->Close();
	}

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
	iTest.End();
	iTest.Close();
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbFileName));
	iFs.Close();
	}

CTestConductor::CTestConductor() : iTest(KTestName)
	{
	}

void CTestConductor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iLog=CLog::NewL(iTest,KLogFileName);
	iDb=CContactDatabase::ReplaceL(KDbFileName);
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);
	AddContactsL();
	}

void CTestConductor::AddContactL(TInt aBitwiseFilterType)
	{
	iTest.Printf(_L("Adding Contact"));
	iRandomGenerator->AddTypicalContactForFilterL(aBitwiseFilterType);
	iTotalContacts++;
	}

void CTestConductor::AddContactsL()
	{
	iTotalContacts=0;
	iTest.Printf(_L("Adding "));
	TInt ii;
	for (ii=0;ii<KNumSmsContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::ESmsable;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	for (ii=0;ii<KNumWorkEmailContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::EMailable;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	for (ii=0;ii<KNumHomeEmailContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::EMailable;
		bit |= CContactDatabase::EHome;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	for (ii=0;ii<KNumLandlineContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::ELandLine;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	for (ii=0;ii<KNumFaxContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::EFaxable;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	}

void CTestConductor::RunTestsL()
	{
	CViewTester* tester=CViewTester::NewL(*iLog,iTest,*iDb,this);
	CleanupStack::PushL(tester);
	CActiveScheduler::Start();
	CleanupStack::Pop(tester);

	// error from active object
	iTest(iTestError == KErrNone);
	}

void CTestConductor::SetTestError(TInt aTestError)
	{
	iTestError = aTestError;
	}

//
// CViewTester.
//

CViewTester* CViewTester::NewL(CLog& aLog,RTest& aTest,CContactDatabase& aDb,CTestConductor* aTestConductor)
	{
	CViewTester* self=new(ELeave) CViewTester(aLog,aTest,aDb,aTestConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CViewTester::~CViewTester()
	{
	iLandlineFilterView->Close(*this);
	iUnfilteredView->Close(*this);
	iNamedRemoteView->Close(*this);
	iSortOrder_1.Close();
	iSortOrder_2.Close();
	delete iTextDef;
	}

CViewTester::CViewTester(CLog& aLog,RTest& aTest,CContactDatabase& aDb,CTestConductor* aTestConductor)
	: CActive(EPriorityStandard),iLog(aLog),iTest(aTest),iDb(aDb),iCurrentTest(-1),iTestConductor(aTestConductor)
	{
	CActiveScheduler::Add(this);
	}

/**

@SYMTestCaseID     PIM-T-FILTERVIEW-MATCHING-PROFILE-0001

*/

void CViewTester::ConstructL()
	{
	iSortOrder_1.AppendL(KUidContactFieldGivenName);
	iSortOrder_1.AppendL(KUidContactFieldFamilyName);
	iSortOrder_1.AppendL(KUidContactFieldCompanyName);

	iTextDef=CContactTextDef::NewL();
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));

	iSortOrder_2.AppendL(KUidContactFieldCompanyName);
	iSortOrder_2.AppendL(KUidContactFieldFamilyName);
	iSortOrder_2.AppendL(KUidContactFieldGivenName);

	iTest.Start(_L("@SYMTESTCaseID:PIM-T-FILTERVIEW-MATCHING-PROFILE-0001 "));

	NextTest();
	}

void CViewTester::PrintTimeTaken(TInt aTime)
	{
	iScratchBuf.Format(_L("**  Test Took [%ds%dus]  **\n"), aTime/1000000, aTime%1000000);
	iLog.LogLine(iScratchBuf);
	//TBuf<100> buffer;
	//buffer.Format(_L("  Test Took [%ds%dus]\n"), aProfile.iTime/1000000, aProfile.iTime%1000000);
	//iLog.LogLine(buffer);
	}


void CViewTester::PrintTotalTimeTaken()
	{
	TInt counter;
	TInt totalTime = 0;

	for (counter = 0; counter < ENumTests; counter++)
		{
		totalTime += iTimeForTestsToComplete[counter];
		}

	iScratchBuf.Format(_L("**  Total amount of time taken [%ds%dus]  **"), totalTime/1000000, totalTime%1000000);
	iLog.LogLine(iScratchBuf);

	}

void CViewTester::RunL()
	{
	TInt counter = 0;
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iLog.LogLine(_L("=== Create Name Remote local view"));
			CCntTest::ProfileReset(0,1);
			CCntTest::ProfileStart(0);
			iNamedRemoteView=CContactNamedRemoteView::NewL(*this,KRemoteViewName,iDb,iSortOrder_1,EContactsOnly);
			break;

		case ECreateLandlineFilterView:
			iLog.LogLine(_L("=== Create Land line Filter View {CContactFilteredView}"));
			iFilter=0;
			iFilter|=CContactDatabase::ELandLine;
			CCntTest::ProfileReset(0,1);
			CCntTest::ProfileStart(0);
			iLandlineFilterView=CContactFilteredView::NewL(*this,iDb,*iNamedRemoteView,iFilter);
			break;

		case ECreateUnfilteredView:
			iLog.LogLine(_L("=== Create Unfilter View {CContactFilteredView}"));
			iFilter=0;
			CCntTest::ProfileReset(0,1);
			CCntTest::ProfileStart(0);
			iUnfilteredView=CContactFilteredView::NewL(*this,iDb,*iNamedRemoteView,iFilter);
			break;

		case ETestContactsMatchingPrefixL_LandLine:
			iLog.LogLine(_L("=== Testing ContactsMatchPrefixL - LandLine"));
			iTimeForTestsToComplete[ETestContactsMatchingPrefixL_LandLine] = 0;
			for (counter = 1; counter < 7; counter++)
				{
				CCntTest::ProfileReset(0,1);
				CCntTest::ProfileStart(0);
				ProfileContactMatchingPrefixL(counter, iLandlineFilterView);
				CCntTest::ProfileEnd(0);
				CCntTest::ProfileResult(&iProfile,0,1);
				iTimeForTestsToComplete[ETestContactsMatchingPrefixL_LandLine] += iProfile.iTime;
				PrintTimeTaken(iProfile.iTime);
				}
			// do stuff
			NextTest();
		break;

		case ETestContactsMatchingPrefixL_Unfiltered:
			iLog.LogLine(_L("=== Testing ContactsMatchPrefixL - Unfiltered"));
			iTimeForTestsToComplete[ETestContactsMatchingPrefixL_Unfiltered] = 0;
			for (counter = 1; counter < 7; counter++)
				{
				CCntTest::ProfileReset(0,1);
				CCntTest::ProfileStart(0);
				ProfileContactMatchingPrefixL(counter, iUnfilteredView);
				CCntTest::ProfileEnd(0);
				CCntTest::ProfileResult(&iProfile,0,1);
				iTimeForTestsToComplete[ETestContactsMatchingPrefixL_Unfiltered] += iProfile.iTime;
				PrintTimeTaken(iProfile.iTime);
				}
			// do stuff
			NextTest();
		break;

		case ETestContactsMatchingCriteriaL_LandLine:
			iLog.LogLine(_L("=== Testing ContactsMatchCriteriaL - LandLine"));
			iTimeForTestsToComplete[ETestContactsMatchingPrefixL_LandLine] = 0;
			for (counter = 1; counter < 7; counter++)
				{
				CCntTest::ProfileReset(0,1);
				CCntTest::ProfileStart(0);
				ProfileContactMatchingCriteriaL(counter, iLandlineFilterView);
				CCntTest::ProfileEnd(0);
				CCntTest::ProfileResult(&iProfile,0,1);
				iTimeForTestsToComplete[ETestContactsMatchingCriteriaL_LandLine] += iProfile.iTime;
				PrintTimeTaken(iProfile.iTime);
				}
			// do stuff
			NextTest();
		break;

		case ETestContactsMatchingCriteriaL_Unfiltered:
			iLog.LogLine(_L("=== Testing ContactsMatchCriteriaL - Unfiltered"));
			iTimeForTestsToComplete[ETestContactsMatchingPrefixL_Unfiltered] = 0;
			for (counter = 1; counter < 7; counter++)
				{
				CCntTest::ProfileReset(0,1);
				CCntTest::ProfileStart(0);
				ProfileContactMatchingCriteriaL(counter, iUnfilteredView);
				CCntTest::ProfileEnd(0);
				CCntTest::ProfileResult(&iProfile,0,1);
				iTimeForTestsToComplete[ETestContactsMatchingCriteriaL_Unfiltered] += iProfile.iTime;
				PrintTimeTaken(iProfile.iTime);
				}
			// do stuff
			NextTest();
		break;

		case ENumTests:
			iLog.LogLine(_L("==== Filter View Finished"));
			PrintTotalTimeTaken();
			CActiveScheduler::Stop();
			delete this;
			break;

		default:
			ASSERT(EFalse);
			break;
		}
	}

TInt CViewTester::RunError(TInt aError)
	{
	// propagate error
	iTestConductor->SetTestError(aError);

	switch (iCurrentTest)
		{
		case ECreateLocalView: iTest.Printf(_L("test failed at step CreateLocalView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateLandlineFilterView: iTest.Printf(_L("test failed at step CreateLandlineFilterView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateUnfilteredView: iTest.Printf(_L("test failed at step CreateUnfilteredView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestContactsMatchingPrefixL_LandLine: iTest.Printf(_L("test failed at step TestContactsMatchingPrefixL_LandLine (%i) with error %i"), iCurrentTest, aError); break;
		case ETestContactsMatchingPrefixL_Unfiltered: iTest.Printf(_L("test failed at step TestContactsMatchingPrefixL_Unfiltered (%i) with error %i"), iCurrentTest, aError); break;
		case ETestContactsMatchingCriteriaL_LandLine: iTest.Printf(_L("test failed at step TestContactsMatchingCriteriaL_LandLine (%i) with error %i"), iCurrentTest, aError); break;
		case ETestContactsMatchingCriteriaL_Unfiltered: iTest.Printf(_L("test failed at step TestContactsMatchingCriteriaL_Unfiltered (%i) with error %i"), iCurrentTest, aError); break;
		case ENumTests: iTest.Printf(_L("test failed at step NumTests (%i) with error %i"), iCurrentTest, aError); break;

		default: iTest.Printf(_L("test failed at step %i with error %i"), iCurrentTest, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}

void CViewTester::ProfileContactMatchingPrefixL(TInt aNumberOfS, CContactFilteredView* aContactFilterView)
	{
	CFindText* findText = NULL;
	RPointerArray<CViewContact> contactArray;
	CleanupStack::PushL(TCleanupItem(CleanupContactArray, &contactArray));
	switch(aNumberOfS)
		{
		case 1:
		findText = CFindText::NewLC(_L("S"));
		break;
		case 2:
		findText = CFindText::NewLC(_L("SS"));
		break;
		case 3:
		findText = CFindText::NewLC(_L("SSS"));
		break;
		case 4:
		findText = CFindText::NewLC(_L("SSSS"));
		break;
		case 5:
		findText = CFindText::NewLC(_L("SSSSS"));
		break;
		case 6:
		findText = CFindText::NewLC(_L("SSSSSS"));
		break;
		};

	aContactFilterView->ContactsMatchingPrefixL(*findText, contactArray);

	CleanupStack::PopAndDestroy(findText);
	CleanupStack::PopAndDestroy(&contactArray);
	}

void CViewTester::ProfileContactMatchingCriteriaL(TInt aNumberOfS, CContactFilteredView* aContactFilterView)
	{
	//_LIT(KFindString, "S");
	//CPtrCArray* array = new (ELeave) CPtrC16Array(1);
	//CleanupStack::PushL(array);
	//array->AppendL(KFindString());
	CFindText* findText = NULL;
	RPointerArray<CViewContact> contactArray;
	CleanupStack::PushL(TCleanupItem(CleanupContactArray, &contactArray));
	switch(aNumberOfS)
		{
		case 1:
		findText = CFindText::NewLC(_L("S"));
		break;
		case 2:
		findText = CFindText::NewLC(_L("SS"));
		break;
		case 3:
		findText = CFindText::NewLC(_L("SSS"));
		break;
		case 4:
		findText = CFindText::NewLC(_L("SSSS"));
		break;
		case 5:
		findText = CFindText::NewLC(_L("SSSSS"));
		break;
		case 6:
		findText = CFindText::NewLC(_L("SSSSSS"));
		break;
		};
	aContactFilterView->ContactsMatchingCriteriaL(*findText, contactArray);

	CleanupStack::PopAndDestroy(findText);
	CleanupStack::PopAndDestroy(&contactArray);
	}

// Should always return true for a correct implementation of
// CContactViewBase
TBool CViewTester::ContactIdsEqualL(const CContactViewBase& aView, TInt aIndex)
  {
  TContactItemId viewId = aView.AtL(aIndex);
  RDebug::Print(_L("UnderLyingView viewId : %d"),viewId);
  TContactItemId contactId = aView.ContactAtL(aIndex).Id();
  RDebug::Print(_L("Sub View Id contactId : %d"),contactId);
  return (contactId == viewId);
  }

void CViewTester::TestViewIndiciesL(const CContactViewBase& aView)
	{
	for (TInt i=0; i < aView.CountL(); ++i)
	  {
	  iTest(ContactIdsEqualL(aView,i)!=EFalse);
	  }
	}

void CViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iTest(iNamedRemoteView==&aView);
			iTest(aEvent.iEventType==TContactViewEvent::EReady);
			CCntTest::ProfileEnd(0);
			CCntTest::ProfileResult(&iProfile,0,1);
			iTimeForTestsToComplete[ECreateLocalView] = iProfile.iTime;
			PrintTimeTaken(iProfile.iTime);
			break;
		case ECreateLandlineFilterView:
			iTest(iLandlineFilterView==&aView);
			iTest(aEvent.iEventType==TContactViewEvent::EReady);
			CCntTest::ProfileEnd(0);
			CCntTest::ProfileResult(&iProfile,0,1);
			iTimeForTestsToComplete[ECreateLandlineFilterView] = iProfile.iTime;
			PrintTimeTaken(iProfile.iTime);
			break;
		case ECreateUnfilteredView:
			iTest(iUnfilteredView==&aView);
			iTest(aEvent.iEventType==TContactViewEvent::EReady);
			CCntTest::ProfileEnd(0);
			CCntTest::ProfileResult(&iProfile,0,1);
			iTimeForTestsToComplete[ECreateUnfilteredView] = iProfile.iTime;
			PrintTimeTaken(iProfile.iTime);
			break;

		case ENumTests:
		default:
			iTest(EFalse);
			break;
		}
	if (--iNumNotificationExpected<=0)
		{
		NextTest();
		iNumNotificationExpected=0;
		}
	}

void CViewTester::TestViewNotificationsL(CContactViewBase& /*aView*/)
	{
	//Test just add some contacts for now...
/*	CRandomContactGenerator* generator=CRandomContactGenerator::NewL();
	CleanupStack::PushL(generator);
	generator->SetDbL(iDb);
	for (TInt ii=0;ii<KInitialNumContacts;++ii)
		{
		generator->AddTypicalRandomContactL();
		}
	CleanupStack::PopAndDestroy(); // generator.
*/	}

void CViewTester::NextTest()
	{
	++iCurrentTest;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CViewTester::ExceriseViewL(CContactViewBase& aView)
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

	iTest(aView.FindL(lastId)==numItems-1);
	}

void CViewTester::DoCancel()
	{
	}

//
// Main.
//


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* testConductor=NULL;
			TRAP_IGNORE(testConductor=CTestConductor::NewL());
			delete testConductor;
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
