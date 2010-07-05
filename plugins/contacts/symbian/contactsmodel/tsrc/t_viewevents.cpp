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
#include "t_viewevents.h"

//
// Constants.
//
_LIT8(KSortPlugin, "application.vnd.symbian.com/contacts-reverse-sort"); //SimpleSortPlugin DLL Unique name

_LIT(KTestName,"t_viewevents");
_LIT(KLogFileName,"t_viewevents.log");

_LIT(KDbFileName,"c:t_viewevents.cdb");

_LIT(KRemoteViewName,"RemoteView");
_LIT(KGroupOneName,"GroupOne");
_LIT(KGroupTwoName,"GroupTwo");

const TInt KContactIdToNotAssignToGroup2 = 5;

LOCAL_D RTest test(KTestName);


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

void CTestConductor::AddContactL(TInt aBitwiseFilterType)
	{
	test.Printf(_L("Adding Contact"));
	iRandomGenerator->AddTypicalContactForFilterL(aBitwiseFilterType);
	iTotalContacts++;
	}

void CTestConductor::AddContactsL()
	{
	iTotalContacts=0;
	test.Printf(_L("Adding "));
	TInt ii;
	for (ii=0;ii<5;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::ESmsable;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}


	for (ii=0;ii<5;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::ELandLine;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}

	}

void CTestConductor::RunTestsL()
	{
	CViewTester* tester=CViewTester::NewL(*iLog,*iDb,this);
	CleanupStack::PushL(tester);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(); // tester.
	}

void CTestConductor::SetTestError(TInt aTestError)
	{
	// error from tester
	iTestError = aTestError;
	}

//
// CViewTester.
//

CViewTester* CViewTester::NewL(CLog& aLog,CContactDatabase& aDb,CTestConductor* aTestConductor)
	{
	CViewTester* self=new(ELeave) CViewTester(aLog,aDb,aTestConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}



CViewTester::CViewTester(CLog& aLog,CContactDatabase& aDb,CTestConductor* aTestConductor)
	: CActive(EPriorityStandard),iLog(aLog),iDb(aDb),iTestConductor(aTestConductor)
	{
	CActiveScheduler::Add(this);
	}

CViewTester::~CViewTester()
	{
	DestructRemoteView();
	DestructFilterView();
	DestructGroupView();
	DestructGroupView2();
	delete iGroup;
	iGroup = NULL;
    delete iGroup2;
    iGroup2 = NULL;	
	iViewSortOrder.Close();
	iLocalViewResults.Close();
	iReceivedResults.Close();
	}

void CViewTester::ConstructL()
	{
	iViewSortOrder.AppendL(KUidContactFieldGivenName);
	iViewSortOrder.AppendL(KUidContactFieldFamilyName);
	iViewSortOrder.AppendL(KUidContactFieldCompanyName);
	iGroup = static_cast<CContactGroup*>(iDb.CreateContactGroupL(KGroupOneName));
	AssignAllContactsToGroupL();
    iGroup2 = static_cast<CContactGroup*>(iDb.CreateContactGroupL(KGroupTwoName));
   	AssignContactsToGroup2L();

	iTestState = EStartingTests;
	NextTest();
	}


void CViewTester::NextTest()
	{
	iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}


void CViewTester::CompareRecordedResultWithExpected()
	{
	TInt counter;
	TInt max;
	TContactViewEvent::TEventType expected;
	TContactViewEvent::TEventType actual;

	test( iLocalViewResults.Count() == iReceivedResults.Count() );
	max = iLocalViewResults.Count();

	for (counter = 0; counter < max; counter++)
		{
		expected = static_cast<TContactViewEvent::TEventType>(iLocalViewResults[counter]);
		actual = static_cast<TContactViewEvent::TEventType>(iReceivedResults[counter]);
		test( expected == actual );
		}

	iReceivedResults.Reset();
	}




void CViewTester::RunL()
	{
	switch (iTestState)
		{
		//
		// Remote View
		//
		case EConstructRemoteView:
			iLog.LogLine ( _L("=== Creating remote view"));
			ConstructRemoteViewL();
			break;
		case ERemoteViewSortOrderChange:
			iLog.LogLine ( _L("=== Changing sort order and recording application messages"));
			RemoteViewSortOrderChangeL();
			break;

		//
		// Filter View
		//
		case EConstructFilterView:
			iLog.LogLine ( _L("=== Creating Filter View"));
			ConstructFilterViewL();
			break;
		case EFilterViewSortOrderChange:
			iLog.LogLine ( _L("=== Changing sort order and processing application messages"));
			FilterViewSortOrderChangeL();
			break;

		//
		// Group View
		//
		case EConstructGroupView:
			iLog.LogLine ( _L("=== Creating Group View"));
			ConstructGroupViewL();
			break;
		case EGroupViewSortOrderChange:
			iLog.LogLine ( _L("=== Changing sort order and processing application messages"));
			GroupViewSortOrderChangeL();
			break;

        //
        // Group2 View
        //
        case EConstructGroupView2:
            iLog.LogLine(_L("=== Creating Group2 View"));
            ConstructGroupView2L();
            break;
        case EGroupView2DeleteContactEvent:
            iLog.LogLine(_L("=== Deleting the only contact in group 2"));
            GroupView2DeleteOnlyContactL();
            break;

        //
        // Remote View
        //
		case ERemoteViewPlugin2:
		case ERemoteViewPlugin:
			iLog.LogLine ( _L("=== Creating remote view plugin"));
			ConstructRemoteViewL(KSortPlugin);
			break;
			
		//
		// End Tests
		//
		case EEndTest:

			iLog.LogLine(_L("==== Finished Tests"));
			CActiveScheduler::Stop();
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

	switch (iTestState)
		{
		case EConstructRemoteView: test.Printf(_L("Test failed at step ConstructRemoteView (%i), with error %i"), iTestState, aError); break;
		case ERemoteViewSortOrderChange: test.Printf(_L("Test failed at step RemoteViewSortOrderChange (%i), with error %i"), iTestState, aError); break;
		case EConstructFilterView: test.Printf(_L("Test failed at step ConstructFilterView (%i), with error %i"), iTestState, aError); break;
		case EFilterViewSortOrderChange: test.Printf(_L("Test failed at step FilterViewSortOrderChange (%i), with error %i"), iTestState, aError); break;
		case EConstructGroupView: test.Printf(_L("Test failed at step ConstructGroupView (%i), with error %i"), iTestState, aError); break;
		case EGroupViewSortOrderChange: test.Printf(_L("Test failed at step GroupViewSortOrderChange (%i), with error %i"), iTestState, aError); break;
        case EConstructGroupView2: test.Printf(_L("Test failed at step ConstructGroupView2 (%i), with error %i"), iTestState, aError); break;
        case EGroupView2DeleteContactEvent: test.Printf(_L("Test failed at step EGroupView2DeleteContactEvent (%i), with error %i"), iTestState, aError); break;
		case ERemoteViewPlugin2: test.Printf(_L("Test failed at step RemoteViewPlugin2 (%i), with error %i"), iTestState, aError); break;
		case ERemoteViewPlugin: test.Printf(_L("Test failed at step RemoteViewPlugin (%i), with error %i"), iTestState, aError); break;
		case EEndTest: test.Printf(_L("Test failed at step EndTest (%i), with error %i"), iTestState, aError); break;

		default: test.Printf(_L("Test failed at step %i, with error %i"), iTestState, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}


void CViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	static TInt NumChanged = 0;
	switch (iTestState)
		{
		//
		// Remote View
		//
		case EConstructRemoteView:
			if ( (&aView == iRemoteView) && (aEvent.iEventType == TContactViewEvent::EReady) )
				{
				NextTest();
				}
			break;

		case ERemoteViewSortOrderChange:
			if ( &aView == iRemoteView )
				{
				iLocalViewResults.Append( aEvent.iEventType );
				if ( aEvent.iEventType == TContactViewEvent::ESortOrderChanged )
					{
					NextTest();
					}
				}
			break;

		//
		// Filter View
		//
		case EConstructFilterView:
			if ( (&aView == iFilteredView) && (aEvent.iEventType == TContactViewEvent::EReady) )
				{
				NextTest();
				}
			break;

		case EFilterViewSortOrderChange:
			if ( &aView == iFilteredView )
				{
				iReceivedResults.Append( aEvent.iEventType );
				if ( aEvent.iEventType == TContactViewEvent::ESortOrderChanged )
					{
					CompareRecordedResultWithExpected();
					NextTest();
					}
				}
			break;

		//
		// Group View
		//
		case EConstructGroupView:
			if ( (&aView == iGroupView) && (aEvent.iEventType == TContactViewEvent::EReady) )
				{
				NumChanged = 0;
				NextTest();
				}
			break;

		case EGroupViewSortOrderChange:
			if ( aEvent.iEventType == TContactViewEvent::ESortOrderChanged)
				{
				++NumChanged;
				}
			if ( &aView == iGroupView )
				{
				iReceivedResults.Append( aEvent.iEventType );
				}
			if ( NumChanged == 3 )
				{
				CompareRecordedResultWithExpected();
				NextTest();
				}
			break;

        //
        // Group2 View
        //
        case EConstructGroupView2:
            if ((&aView == iGroupView2) && (aEvent.iEventType == TContactViewEvent::EReady))
                {
                NextTest();
                }
            break;
       
        case EGroupView2DeleteContactEvent:
            if ((&aView == iGroupView2))
                {
                // When a contact is deleted, we expect a EGroupChanged event
                // followed by a EItemRemoved. The iInt in the EItemRemoved
                // should be the index of the contact in the group view and
                // not its index in the underlying view.
                switch (aEvent.iEventType)
                    {
                    case TContactViewEvent::EGroupChanged:
                        break;
                    case TContactViewEvent::EItemRemoved:
                        {
                        // Check that the index in the event is the index of
                        // the contact in the group view and NOT the index
                        // of the contact in the global database. There is
                        // only one item in the group, so its index is 0.
                        test(aEvent.iInt == iContactToDeleteMapping.iMapping);
                        test(aEvent.iContactId == iContactToDeleteMapping.iId);
                        NextTest();                       
                        }
                        break;
                    default:
                        test(EFalse);
                    }
                }
            break;

        //
        // Remote View Plugin
        //
		case ERemoteViewPlugin:
			if ( aEvent.iEventType == TContactViewEvent::EReady)
				{
				NextTest();
				}
			break;
			
		case ERemoteViewPlugin2:
			if (aEvent.iEventType == TContactViewEvent::EReady)
				{
				/* test to make sure both these two view2 use the same remote view
					We can't *really* test this without a lot of painful effort. So we test what we can:
					that the two plugins are identical. 
					To really confirm this, check the logs to ensure that two example sort
					plugins were created.
					
					There should be two instantiated on the client side and one on the server side.
					Thus a total of three.
				*/
				TUid firstID = iRemoteView2->GetViewSortPluginImplUid();
				TUid secondID = iRemoteView3->GetViewSortPluginImplUid();
				test(firstID == secondID); // make sure both use the same plugin.


				CArrayFix<TContactItemId >* ids(NULL);
				CArrayFix<TContactItemId >* ids2(NULL);
				TRAPD(err, 
					ids = OrderOfContactsLC(*iRemoteView2);
					ids2 = OrderOfContactsLC(*iRemoteView3);
					// Must pop these here because they're on this trap's cleanup stack.
					// Don't need to re-push them as nothing below can leave.
					CleanupStack::Pop(2, ids); // and ids2
					)
				test(err == KErrNone);
				
				TInt size = ids->Count();
				test(ids2->Count() == size);
				for (TInt i =0; i < size; ++i) 
					{
					test(ids->At(i) == ids2->At(i));
					}

				// cleanup id arrays -- don't need Reset() as only holding TClasses
				delete ids;
				delete ids2;
	
				NextTest();

				}
			break;

		//
		// End Tests
		//
		case EEndTest:
		default:
			test(EFalse);
			break;
		}
	}

void CViewTester::AssignAllContactsToGroupL()
	{
	TInt counter = 0;
	for ( counter = 0; counter < 10; counter++)
		{
		iDb.AddContactToGroupL(counter, iGroup->Id());
		}
	}

void CViewTester::AssignContactsToGroup2L()
    {
    TInt counter = 0;
    for ( counter = 0; counter < 10; counter++)
        {
        if (counter != KContactIdToNotAssignToGroup2)
            {
            iDb.AddContactToGroupL(counter, iGroup2->Id());
            }
        }
    }

void CViewTester::ConstructFilterViewL()
	{
	iFilteredView = CContactFilteredView::NewL(*this, iDb, *iRemoteView, EContactsOnly);
	}

void CViewTester::FilterViewSortOrderChangeL()
	{
	iRemoteView->ChangeSortOrderL(iViewSortOrder);
	}

void CViewTester::DestructFilterView()
	{
	if (iFilteredView) iFilteredView->Close( *this );
	// filtered view should delete it's self when it runs out of observers
	iFilteredView = NULL;
	}

void CViewTester::ConstructRemoteViewL(const TDesC8& aSortPluginName)
	{
	if(aSortPluginName.Length() == 0)
		{
		iRemoteView = CContactNamedRemoteView::NewL(*this, KRemoteViewName, iDb, iViewSortOrder, EContactsOnly);
		}
	else 
		{
		if(iRemoteView2) 
			iRemoteView3 = CContactRemoteView::NewL(*this, iDb, iViewSortOrder, EContactsOnly, aSortPluginName);
		else
			iRemoteView2 = CContactRemoteView::NewL(*this, iDb, iViewSortOrder, EContactsOnly, aSortPluginName);
		}
	}

CContactRemoteView* CViewTester::ExtracttRemoteViewLC()
	{
	CContactRemoteView* remoteView = iRemoteView2;
	iRemoteView2 = NULL;
	CleanupStack::PushL(remoteView);
	return remoteView;
	}

void CViewTester::RemoteViewSortOrderChangeL()
	{
	iRemoteView->ChangeSortOrderL(iViewSortOrder);
	}

void CViewTester::DestructRemoteView()
	{
	if (iRemoteView) iRemoteView->Close( *this );
	// remote view should delete it's self when it runs out of observers
	iRemoteView = NULL;
	if (iRemoteView2) iRemoteView2->Close( *this );
	// remote view should delete it's self when it runs out of observers
	iRemoteView2 = NULL;
	if (iRemoteView3) iRemoteView3->Close( *this );
	// remote view should delete it's self when it runs out of observers
	iRemoteView3 = NULL;

	}

void CViewTester::ConstructGroupViewL()
	{
	iGroupView = CContactGroupView::NewL(iDb,*iRemoteView,*this,iGroup->Id(),CContactGroupView::EShowContactsInGroup);
	}

void CViewTester::GroupViewSortOrderChangeL()
	{
	iRemoteView->ChangeSortOrderL(iViewSortOrder);
	}

void CViewTester::DestructGroupView()
	{
	if(iGroupView) iGroupView->Close(*this);
	// group view should delete it's self when it runs out of observers
	iGroupView = NULL;
	}

void CViewTester::ConstructGroupView2L()
    {
    iGroupView2 = CContactGroupView::NewL(iDb,*iRemoteView,*this,iGroup2->Id(),CContactGroupView::EShowContactsNotInGroup);
    }
   
void CViewTester::GroupView2DeleteOnlyContactL()
    {
    // Set up the mapping between the contact Id that
    // will be deleted and its index in the group view.
    iContactToDeleteMapping.iId = KContactIdToNotAssignToGroup2;
    iContactToDeleteMapping.iMapping = iGroupView2->FindL(KContactIdToNotAssignToGroup2);
   
    iDb.DeleteContactL(KContactIdToNotAssignToGroup2);
    }
   
void CViewTester::DestructGroupView2()
    {
    if(iGroupView2) iGroupView2->Close(*this);
    // group view should delete it's self when it runs out of observers
    iGroupView2 = NULL;
    }

CArrayFix<TContactItemId >* CViewTester::OrderOfContactsLC(const CContactViewBase& aView)
	{ // aView must be ready
	CArrayFix<TContactItemId >* ids=new(ELeave) CArrayFixFlat<TContactItemId >(4);
	CleanupStack::PushL(ids);

	TInt size = aView.CountL();
	for(TInt i=0;i<size;i++)
		{
		ids->AppendL(aView.AtL(i));
		}
	return ids;
	}


void CViewTester::DoCancel()
	{
	}

//
// Main.
//

/**

@SYMTestCaseID     PIM-T-VIEWEVENTS-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEWEVENTS-0001 Testing ViewEvents"));

	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* testConductor=NULL;
			TRAP_IGNORE(testConductor = CTestConductor::NewL());
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
