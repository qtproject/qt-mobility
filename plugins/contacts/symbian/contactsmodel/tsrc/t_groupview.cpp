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
#include "t_utils2.h"
#include "t_groupview.h"
#include "ccontactdbeventqueue.h"

// Test Macro
LOCAL_D RTest test(_L("T_GROUPVIEW"));

//
// Constants.
//

_LIT(KTestName,"t_groupview");
_LIT(KLogFileName,"t_groupview.log");

_LIT(KDbFileName,"c:t_groupview.cdb");

_LIT(KTextDefSeparator,"");

const TInt KNumContacts = 100;
const TInt KNumGroups = 1;
const TInt KNumContactsInGroupOne = 25;
const TInt KNumMaxEventRequests = 1000;
_LIT(KGroupOneName,"GroupOne");


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

CTestConductor::CTestConductor()
	{
	}

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
	iTotalContacts = 0;
	TInt loop;
	for (loop = 0;loop < KNumContacts; ++loop)
		{
		iRandomGenerator->AddTypicalRandomContactL();
		++iTotalContacts;
		test.Printf(_L("Adding %d "),loop);
		}

	// consume any outstanding database events before creating
	// the views to stop these events later being sent to them.
	CContactDbEventQueue* dbEventQueue = CContactDbEventQueue::NewL(iDb, KNumMaxEventRequests);
	TContactDbObserverEvent dbEvent;
	TBool expectingEvent = ETrue;
	const TInt KTimeOutInSeconds = 5;
	
	for(loop = 0; expectingEvent; ++loop)
		{
		expectingEvent = dbEventQueue->ListenForEvent(KTimeOutInSeconds, dbEvent);
		if (expectingEvent) // meaning if we've just received one
			{
			test.Printf(_L("Consumed db event #%d: %d\n"), loop, dbEvent.iType);
			}
		}
	delete dbEventQueue;
	}

void CTestConductor::RunTestsL()
	{
	CGroupViewTester* tester=CGroupViewTester::NewL(*iLog,this,*iDb);
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

CGroupViewTester* CGroupViewTester::NewL(CLog& aLog,CTestConductor* aTestConductor,CContactDatabase& aDb)
	{
	CGroupViewTester* self=new(ELeave) CGroupViewTester(aLog,aTestConductor,aDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CGroupViewTester::~CGroupViewTester()
	{
	iGroupViewOne->Close(*this);
	iGroupViewOneByName->Close(*this);
	iGroupViewOneNotInGroup->Close(*this);
	iGroupViewUnfiled->Close(*this);
	iLocalView->Close(*this);
	iSortOrder_1.Close();
	delete iTextDef;
	delete iGroupView_One_Name;
	delete iIdsInGroupViewOne;
	delete iGroupOne;
	delete iEventConsumer;
	}

CGroupViewTester::CGroupViewTester(CLog& aLog,CTestConductor* aTestConductor,CContactDatabase& aDb)
	: CActive(EPriorityStandard),iLog(aLog),iTestConductor(aTestConductor),iDb(aDb),iCurrentTest(-1)
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
	CreateGroupTestDataL();
	NextTest();
	}

void CGroupViewTester::CreateGroupTestDataL()
	{
	TInt loop;
	iGroupOne = STATIC_CAST(CContactGroup*,iDb.CreateContactGroupL(KGroupOneName));
	for (loop = 1;loop <= KNumContactsInGroupOne;++loop)
		{
		iDb.AddContactToGroupL(loop,iGroupOne->Id());
		}
	iGroupOneId = iGroupOne->Id();
	delete iGroupOne;
	iGroupOne = NULL;
	iGroupOne = STATIC_CAST(CContactGroup*,iDb.ReadContactL(iGroupOneId));
	iIdsInGroupViewOne = iGroupOne->ItemsContainedLC();
	CleanupStack::Pop();//iIdsInGroupViewOne:(
	
	// consume any outstanding database events before creating
	// the views to stop these events later being sent to them.
	CContactDbEventQueue* dbEventQueue = CContactDbEventQueue::NewL(&iDb, KNumMaxEventRequests);
	TContactDbObserverEvent dbEvent;
	TBool expectingEvent = ETrue;
	const TInt KTimeOutInSeconds = 5;
	for(loop = 0; expectingEvent; ++loop)
		{
		expectingEvent = dbEventQueue->ListenForEvent(KTimeOutInSeconds, dbEvent);
		if (expectingEvent) // meaning if we've just received one
			{
			test.Printf(_L("Consumed db event #%d: %d\n"), loop, dbEvent.iType);
			}
		}
	delete dbEventQueue;
	}

void CGroupViewTester::RunL()
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iLog.LogLine(_L("=== Create local view"));
			iLocalView=CContactLocalView::NewL(*this,iDb,iSortOrder_1,EContactAndGroups/*EContactsOnly*/);
			break;
		case EExerciseLocalView:
			iLog.LogLine(_L("=== Exercise local view"));			
			ExceriseViewL(*iLocalView);
			NextTest();
			break;
		case ECreateGroupOneView:
			{
			iLog.LogLine(_L("=== GroupOneView"));
			iGroupViewOne=CContactGroupView::NewL(iDb,*iLocalView,*this,iGroupOne->Id(),CContactGroupView::EShowContactsInGroup);
			}
			break;
		case ETestGroupOneView:
			{
			iLog.LogLine(_L("==== Exercise ETestGroupOneView"));
			TInt groupCount = iGroupOne->ItemsContained()->Count();
			test(iGroupViewOne->CountL()==groupCount);
			const CContactIdArray* array= iGroupOne->ItemsContained();
			for (TInt ii=0;ii<groupCount;ii++)
				{
				test(iGroupViewOne->FindL((*array)[ii])!=KErrNotFound);
				}
			TestGroupViewSortOrderL(*iGroupViewOne);
			NextTest();
			}
			break;
		case ECreateGroupOneViewByName:
			{
			iLog.LogLine(_L("=== Create GroupOneView By Name"));
			iGroupViewOneByName=CContactGroupView::NewL(iDb,*iLocalView,*this,KGroupOneName,CContactGroupView::EShowContactsInGroup);
			}
			break;
		case ETestGroupOneViewByName:
			{
			iLog.LogLine(_L("==== Exercise ETestGroupOneView By Name"));
			TInt groupCount = iGroupOne->ItemsContained()->Count();
			test(iGroupViewOneByName->CountL()==groupCount);
			const CContactIdArray* array= iGroupOne->ItemsContained();
			for (TInt ii=0;ii<groupCount;ii++)
				{
				test(iGroupViewOneByName->FindL((*array)[ii])!=KErrNotFound);
				}
			TestGroupViewSortOrderL(*iGroupViewOneByName);
			NextTest();
			break;
			}
		case ECreateGroupOneViewNotInGroup:
			{
			iLog.LogLine(_L("=== Create GroupOneViewNotInGroup By Name"));
			iGroupViewOneNotInGroup=CContactGroupView::NewL(iDb,*iLocalView,*this,KGroupOneName,CContactGroupView::EShowContactsNotInGroup);
			}
			break;
		case ETestGroupOneViewNotInGroup:
			{
			iLog.LogLine(_L("==== Exercise GroupOneViewNotInGroup By Name"));
			TInt totalContacts = iLocalView->CountL();
			TInt totalNotInGroup =  totalContacts - KNumContactsInGroupOne;
			test(iGroupViewOneNotInGroup->CountL()==totalNotInGroup);
			const CContactIdArray* array= iGroupOne->ItemsContained();
			TInt groupCount = array->Count();
			for (TInt ii=0;ii<groupCount;ii++)
				{
				test(iGroupViewOneNotInGroup->FindL((*array)[ii])==KErrNotFound);
				}
			TestGroupViewSortOrderL(*iGroupViewOneNotInGroup);
			NextTest();
			break;
			}
		case EAllViewsOutOfBoundsAccess:
			{
			//Views depend on their underlying views being in a good state, however
			//as some base views are potentially in other processes they must be resistant
			//to out of date views accessesing out of bound members, views, should not
			//panic but should leave with KErrNotFound;
			//local view
			TInt err=0;
			iLog.LogLine(_L("=== Test views for out of bounds access"));
			TInt outCount = iGroupViewOneByName->CountL();
			TRAP(err,iGroupViewOneByName->AtL(outCount));
			test(err==KErrNotFound);
			TRAP(err,iGroupViewOneByName->ContactAtL(outCount));
			test(err==KErrNotFound);
			NextTest();
			}
			break;
		case ECreateUnfiledGroupView:
			{
			iLog.LogLine(_L("=== Create Unfiled group view"));
			iGroupViewUnfiled=CContactGroupView::NewL(iDb,*iLocalView,*this,KNullContactId,CContactGroupView::EShowContactsNotInAnyGroup);
			}
			break;
		case ETestUnfiledGroupView:
			{
			iLog.LogLine(_L("==== Exercise Unfiled group"));
			const TInt totalContacts = iLocalView->CountL();
			const TInt totalUnfiled =  totalContacts - KNumContactsInGroupOne - KNumGroups;
			test(iGroupViewUnfiled->CountL()==totalUnfiled);			
			test(iGroupViewUnfiled->FindL(iGroupOneId));
			TestGroupViewSortOrderL(*iGroupViewUnfiled);

			CRandomContactGenerator* generator = CRandomContactGenerator::NewL();
			CleanupStack::PushL(generator);
			generator->SetDbL(iDb);
			generator->AddTypicalRandomContactL();
			CleanupStack::PopAndDestroy(generator);
			iNumNotificationExpected=5;
			}
			break;
		case ETestUnfiledGroupAddition:
			{
			iLog.LogLine(_L("==== Exercise Unfiled group addition"));
			TInt revisedCount = iGroupViewUnfiled->CountL();
			CContactIdArray* unfiled = iDb.UnfiledContactsL();
			test(revisedCount == unfiled->Count());
			delete unfiled;
			ExceriseViewL(*iGroupViewUnfiled);
			// Test that adding contact which currently forms part of
			// iGroupViewUnfiled to iGroupOne causes update of
			// iGroupViewUnfiled such that the contact no longer forms
			// part of iGroupViewUnfiled (i.e. by adding the contact to the
			// group it is no longer unfiled and should not appear in the
			// unfiled view).  The update to iGroupViewUnfiled will not take
			// place until the view receives the change event so wait until the
			// view observer method HandleContactViewEvent() is called before
			// testing that the expected change to iGroupViewUnfiled has taken
			// place.
			iDb.AddContactToGroupL(KNumContacts,iGroupOne->Id());
			// Expect (ESortOrderChanged x 4) + EGroupChanged + (EItemRemoved x
			// 4) + (EItemAdded x 4).
			iNumNotificationExpected=13;
			}
			break;
		case ENumTests:
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
	iTestConductor->SetTestError(aError);

	switch (iCurrentTest)
		{
		case ECreateLocalView: test.Printf(_L("Test failed at step CreateLocalView (%i) with error %i"), iCurrentTest, aError); break;
		case EExerciseLocalView: test.Printf(_L("Test failed at step ExerciseLocalView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateGroupOneView: test.Printf(_L("Test failed at step CreateGroupOneView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestGroupOneView: test.Printf(_L("Test failed at step TestGroupOneView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateGroupOneViewByName: test.Printf(_L("Test failed at step CreateGroupOneViewByName (%i) with error %i"), iCurrentTest, aError); break;
		case ETestGroupOneViewByName: test.Printf(_L("Test failed at step TestGroupOneViewByName (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateGroupOneViewNotInGroup: test.Printf(_L("Test failed at step CreateGroupOneViewNotInGroup (%i) with error %i"), iCurrentTest, aError); break;
		case ETestGroupOneViewNotInGroup: test.Printf(_L("Test failed at step TestGroupOneViewNotInGroup (%i) with error %i"), iCurrentTest, aError); break;
		case EAllViewsOutOfBoundsAccess: test.Printf(_L("Test failed at step AllViewsOutOfBoundsAccess (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateUnfiledGroupView: test.Printf(_L("Test failed at step CreateUnfiledGroupView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestUnfiledGroupView: test.Printf(_L("Test failed at step TestUnfiledGroupView (%i) with error %i"), iCurrentTest, aError); break;
		case ETestUnfiledGroupAddition: test.Printf(_L("Test failed at step TestUnfiledGroupAddition (%i) with error %i"), iCurrentTest, aError); break;

		case ENumTests: test.Printf(_L("Test failed at step NumTests (%i) with error %i"), iCurrentTest, aError); break;

		default: test.Printf(_L("Test failed at step %i with error %i"), iCurrentTest, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}

void CGroupViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	//LOGGING///////////////////////////////////////////////////////////////////////////////////
				if(&aView == iLocalView)
					test.Printf(_L("LocalView "));
				else if(&aView == iGroupViewOne)
					test.Printf(_L("GroupViewOne "));
				else if(&aView == iGroupViewOneByName)
					test.Printf(_L("GroupViewOneByName "));
				else if(&aView == iGroupViewOneNotInGroup)
					test.Printf(_L("GroupViewOneNotInGroup "));
				else if(&aView == iGroupViewUnfiled)
					test.Printf(_L("GroupViewUnfiled "));
				else
					test.Printf(_L("Er, none of the known views... "));

				switch (aEvent.iEventType)
					{
					case TContactViewEvent::EUnavailable:
						test.Printf(_L("EUnavailable\n"));
						break;
					case TContactViewEvent::EReady:
						test.Printf(_L("EReady\n"));
						break;
					case TContactViewEvent::ESortOrderChanged:
						test.Printf(_L("ESortOrderChanged\n"));
						break;
					case TContactViewEvent::ESortError:
						test.Printf(_L("ESortError\n"));
						break;
					case TContactViewEvent::EServerError:
						test.Printf(_L("EServerError\n"));
						break;
					case TContactViewEvent::EIndexingError:
						test.Printf(_L("EIndexingError\n"));
						break;
					case TContactViewEvent::EItemAdded:
						test.Printf(_L("EItemAdded\n"));
						break;
					case TContactViewEvent::EItemRemoved:
						test.Printf(_L("EItemRemoved\n"));
						break;
					case TContactViewEvent::EGroupChanged:
						test.Printf(_L("EGroupChanged\n"));
						break;
					default:
						test.Printf(_L("Er, none of the known event types... "));
						break;
					}
	////////////////////////////////////////////////////////////////////////////////////////////
	
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			test(iLocalView==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
			break;
		case EExerciseLocalView:
			test(ETrue);
			break;
		case ECreateGroupOneView:
			test(iGroupViewOne==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
			break;
		case ETestGroupOneView:
			test(EFalse);
			break;
		case ECreateGroupOneViewByName:
			test(iGroupViewOneByName==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
			break;
		case ETestGroupOneViewByName:
			test(EFalse);
			break;
		case ECreateGroupOneViewNotInGroup:
			test(iGroupViewOneNotInGroup==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
			break;
		case ETestGroupOneViewNotInGroup:
			test(EFalse);
			break;
		case EAllViewsOutOfBoundsAccess:
			test(EFalse);
			break;
		case ECreateUnfiledGroupView:
			test(iGroupViewUnfiled==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
			break;
		case ETestUnfiledGroupView:
			break;
		case ETestUnfiledGroupAddition:
			{
			// No point testing for each and every notification: the test will
			// pass for all notifications.
			if (iNumNotificationExpected > 1)
				{
				break;
				}
			// Test that adding contact which formed part of iGroupViewUnfiled
			// to iGroupOne caused update of iGroupViewUnfiled such that the
			// contact no longer forms part of iGroupViewUnfiled (i.e. by adding
			// the contact to the group it is no longer unfiled is not a member
			// of the unfiled view).
			TInt ret(KErrNone);
			TRAPD(err1, ret = iGroupViewUnfiled->FindL(KNumContacts) );
			test(err1 == KErrNone && ret == KErrNotFound);
			// Double check - make sure the number of items in the unfiled
			// group view agrees with the number of unfiled items reported by
			// the database.
			TInt groupViewUnfiledCount(0);
			TRAPD(err2, groupViewUnfiledCount = iGroupViewUnfiled->CountL() );
			CContactIdArray* dbUnfiled = NULL;
			TRAPD(err3, dbUnfiled = iDb.UnfiledContactsL() );
			test(err2 == KErrNone && err3 == KErrNone && groupViewUnfiledCount==dbUnfiled->Count() );
			delete dbUnfiled;
			}
			break;
		case ENumTests:
		default:
			test(EFalse);
			break;
		}
	if (--iNumNotificationExpected <= 0)
		{
		iNumNotificationExpected=0;
		NextTest();
		}
	}

void CGroupViewTester::NextTest()
	{
	++iCurrentTest;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CGroupViewTester::ExceriseViewL(CContactViewBase& aView)
	{
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	for (TInt loop = 0;loop < numItems;++loop)
		{
		if (loop == numItems-1)
			{
			lastId=aView.AtL(loop);
			}
		iDb.ReadContactTextDefL(aView.AtL(loop),iScratchBuf,iTextDef);
		iLog.LogLineNoEcho(iScratchBuf);
		iScratchBuf.SetLength(0);
		}

	test(aView.FindL(lastId)==numItems-1);
	}


void CGroupViewTester::TestGroupViewSortOrderL(CContactGroupView& aView)
	{
	const TInt numItems=aView.CountL();
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
// Callback from event consumer class instance
TInt CGroupViewTester::EventConsumerCallBack(TAny* aThis)
	{
	// Just go to the next test in the RunL()
	static_cast<CGroupViewTester*>(aThis)->NextTest();
	return KErrNone;
	}

///////

//
// Main.
//

/**

@SYMTestCaseID     PIM-T-GROUPVIEW-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	RProcess().SetPriority(EPriorityBackground);
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* testConductor=NULL;
			test.Start(_L("@SYMTESTCaseID:PIM-T-GROUPVIEW-0001 t_groupview"));

			TRAPD(err,testConductor=CTestConductor::NewL());
			test(err == KErrNone);
			test.End();
			delete testConductor;
			test.Close();
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
