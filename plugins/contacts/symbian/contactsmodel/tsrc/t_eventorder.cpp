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
#include "t_eventorder.h"

_LIT8(KSortPlugin, "application.vnd.symbian.com/contacts-reverse-sort"); //SimpleSortPlugin DLL Unique name

_LIT(KTestName,"t_eventorder");
_LIT(KLogFileName,"t_eventorder.log");

_LIT(KDbFileName,"c:t_eventorder.cdb");

_LIT(KRemoteViewName,"RemoteView");


LOCAL_D RTest test(KTestName);

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
	const TInt KCount(contacts->Count() );

	TRAP_IGNORE(
		for(TInt i = 0; i < KCount; ++i)
			{
			iDb->DeleteContactL((*contacts)[i]);
			}
	);
	delete iDb;
	delete iRandomGenerator;
	delete contacts;
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
	contacts = CContactIdArray::NewLC();
	iTotalContacts=0;
	test.Printf(_L("Adding Contact for Phone number test "));
	TInt ii;
	for (ii=0;ii<3;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::ESmsable;
		TContactItemId itemId =iRandomGenerator->AddTypicalContactForFilterL(bit);
		contacts->AddL(itemId);
		iTotalContacts++;
		}
	CleanupStack::Pop(contacts); // tester.
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
   
	iViewSortOrder.Close();
	iLocalViewResults.Close();
	iReceivedResults.Close();
	iAddNumberRemoteViewResults1.Close();
	iAddNumberRemoteViewResults2.Close();
	iAddNumberRemoteViewResults3.Close();
	iAddNumberRemoteViewResults4.Close();	
	iAddNumberReceivedResults.Close();
	}

void CViewTester::ConstructL()
	{
	iViewSortOrder.AppendL(KUidContactFieldGivenName);
	iViewSortOrder.AppendL(KUidContactFieldFamilyName);
	iViewSortOrder.AppendL(KUidContactFieldCompanyName);


	iAddNumberRemoteViewResults1.Append( TContactViewEvent::EItemRemoved);
	iAddNumberRemoteViewResults1.Append( 2 );
	iAddNumberRemoteViewResults1.Append( TContactViewEvent::EItemRemoved);
	iAddNumberRemoteViewResults1.Append( 0 );
	iAddNumberRemoteViewResults1.Append( TContactViewEvent::EItemAdded);
	iAddNumberRemoteViewResults1.Append( 0 );
	iAddNumberRemoteViewResults1.Append( TContactViewEvent::EItemAdded);
	iAddNumberRemoteViewResults1.Append( 2 );
	
	iAddNumberRemoteViewResults2.Append( TContactViewEvent::EItemRemoved);
	iAddNumberRemoteViewResults2.Append( 1 );
	iAddNumberRemoteViewResults2.Append( TContactViewEvent::EItemRemoved);
	iAddNumberRemoteViewResults2.Append( 0 );
	iAddNumberRemoteViewResults2.Append( TContactViewEvent::EItemAdded);
	iAddNumberRemoteViewResults2.Append( 0 );
	iAddNumberRemoteViewResults2.Append( TContactViewEvent::EItemAdded);
	iAddNumberRemoteViewResults2.Append( 1 );
	
	iAddNumberRemoteViewResults3.Append( TContactViewEvent::EItemRemoved);
	iAddNumberRemoteViewResults3.Append( 0 );
	iAddNumberRemoteViewResults3.Append( TContactViewEvent::EItemAdded);
	iAddNumberRemoteViewResults3.Append( 0 );
	iAddNumberRemoteViewResults3.Append( TContactViewEvent::EItemRemoved);
	iAddNumberRemoteViewResults3.Append( 2 );
	iAddNumberRemoteViewResults3.Append( TContactViewEvent::EItemAdded);
	iAddNumberRemoteViewResults3.Append( 2 );
	
	iAddNumberRemoteViewResults4.Append( TContactViewEvent::EItemRemoved);
	iAddNumberRemoteViewResults4.Append( 0 );
	iAddNumberRemoteViewResults4.Append( TContactViewEvent::EItemAdded);
	iAddNumberRemoteViewResults4.Append( 0 );
	iAddNumberRemoteViewResults4.Append( TContactViewEvent::EItemRemoved);
	iAddNumberRemoteViewResults4.Append( 1 );
	iAddNumberRemoteViewResults4.Append( TContactViewEvent::EItemAdded);
	iAddNumberRemoteViewResults4.Append( 1 );			
	iTestState = EStartingTests;
	NextTest();
	}

void CViewTester::CompleteRequest()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();	
	}
void CViewTester::NextTest()
	{
	iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
	CompleteRequest();
	}


void CViewTester::AddNumberCompareRecordedResultWithExpected(const RArray<TInt>& aAddNumberRemoteViewResults)
	{
	TInt counter;
	TInt max;
	TContactViewEvent::TEventType expectedEvent;
	TContactViewEvent::TEventType actualEvent;
	TInt expectedInt;
	TInt actualInt;

	test( aAddNumberRemoteViewResults.Count() == iAddNumberReceivedResults.Count() );
	max = aAddNumberRemoteViewResults.Count();

	for (counter = 0; counter < max; counter++)
		{
		expectedEvent = static_cast<TContactViewEvent::TEventType>(aAddNumberRemoteViewResults[counter]);
		actualEvent = static_cast<TContactViewEvent::TEventType>(iAddNumberReceivedResults[counter]);
		test( expectedEvent == actualEvent );
		counter++;
		expectedInt = static_cast<TInt>(aAddNumberRemoteViewResults[counter]);
		actualInt = static_cast<TInt>(iAddNumberReceivedResults[counter]);
		test( expectedInt == actualInt );
		}
		
	iAddNumberReceivedResults.Reset();

	}
	
void CViewTester::AddPhoneNumberL(const TDesC& aWorkPhone,TContactItemId& ItemId)
	{
	CContactItem *contactItem=iDb.OpenContactL(ItemId);
	CleanupStack::PushL(contactItem);
	CContactItemFieldSet& fields=contactItem->CardFields();
	TInt fieldIndex = fields.Find(KUidContactFieldPhoneNumber);

	if ( fieldIndex > KErrNotFound )
		{
		CContactItemField& field = fields[ fieldIndex ];
		ASSERT(field.StorageType()==KStorageTypeText);
		STATIC_CAST(CContactTextField*,field.Storage())->SetText(aWorkPhone.AllocL());
		}
	
	iDb.CommitContactL(*contactItem); 
	CleanupStack::PopAndDestroy(contactItem);
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

		case ERemoteViewPlugin2:
		case ERemoteViewPlugin:
			iLog.LogLine ( _L("=== Creating remote view plugin"));
			ConstructRemoteViewL(KSortPlugin);
			break;			
			
		//Add Phone Numbers	
		case EAddPhoneNumber1:
			iLog.LogLine ( _L("=== Add Phone Number to contact item 2 and 1"));
			AddPhoneNumberL(_L("222"), (*(iTestConductor->contacts))[1]);
			AddPhoneNumberL(_L("111"), (*(iTestConductor->contacts))[0]);
			iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
			break;
		case EAddPhoneNumber2:
			iLog.LogLine ( _L("=== Add Phone Number to contact item 3 and 1"));
			AddPhoneNumberL(_L("333"), (*(iTestConductor->contacts))[2]);
			AddPhoneNumberL(_L("111"), (*(iTestConductor->contacts))[0]);
			iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
			break;
		case EAddPhoneNumber3:
			iLog.LogLine ( _L("=== Add Phone Number to contact item 1 and 2"));
			AddPhoneNumberL(_L("111"), (*(iTestConductor->contacts))[0]);
			AddPhoneNumberL(_L("222"), (*(iTestConductor->contacts))[1]);
			iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
			break;
		case EAddPhoneNumber4:
			iLog.LogLine ( _L("=== Add Phone Number to contact item 1 and 3"));
			AddPhoneNumberL(_L("111"), (*(iTestConductor->contacts))[0]);
			AddPhoneNumberL(_L("333"), (*(iTestConductor->contacts))[2]);
			iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
			break;
		
		case EPhoneNumberChangedEvent1:
		case EPhoneNumberChangedEvent2:
		case EPhoneNumberChangedEvent3:
		case EPhoneNumberChangedEvent4:
			iLog.LogLine ( _L("=== Capture Phone Number Changed Event"));
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
        // Remote View Plugin
        //
		case ERemoteViewPlugin:
			if ( aEvent.iEventType == TContactViewEvent::EReady)
				{
				NextTest();
				}
			break;
			
		case ERemoteViewPlugin2:
			if ( aEvent.iEventType == TContactViewEvent::EReady)
				{
				/* test to make sure both these two view2 use the same remote view
					We can't *really* test this wihout alot of painful effort. So we test what we can:
					that the two plugins are identical. 
					To really confirm this, check the logs to ensure that two example sort
					plugins were created.
					
					There should be two instantiated on the client side and one on the server side.
					Thus a total of three.
				*/
				TUid firstID = iRemoteView2->GetViewSortPluginImplUid();
				TUid secondID = iRemoteView3->GetViewSortPluginImplUid();
				test(firstID==secondID); // make sure both use the same plugin.

				CArrayFix<TContactItemId >* ids(NULL);
				CArrayFix<TContactItemId >* ids2(NULL);
				TRAPD(err, ids = OrderOfContactsLC(*iRemoteView2) );
				if (err)
					{
					_LIT(KErrMsg, "Contact view error in CViewTester::HandleContactViewEvent() assigning ids\n");
					test.Printf(KErrMsg);
					User::Invariant();
					}
				TRAP(err, ids2 = OrderOfContactsLC(*iRemoteView3) );
				if (err)
					{
					_LIT(KErrMsg, "Contact view error in CViewTester::HandleContactViewEvent() assigning ids2\n");
					test.Printf(KErrMsg);
					User::Invariant();
					}
				
				TInt size = ids->Count();
				test(ids2->Count()==size);
				for (TInt i =0;i<size;i++) 
					{
					test(ids->At(i)==ids2->At(i));
					}

				CleanupStack::PopAndDestroy(2,ids); // +ids2
	
				NextTest();

				}
			break;
					
		//Phone Number Changed events	
		case EPhoneNumberChangedEvent1:	//Pass through
		case EPhoneNumberChangedEvent2:	//Pass through
		case EPhoneNumberChangedEvent3:	//Pass through
		case EPhoneNumberChangedEvent4:	
		 	if ( &aView == iRemoteView ) 
		 	{
			if (aEvent.iEventType == TContactViewEvent::EItemRemoved )
				{
				iLog.LogLine ( _L("=== Capature EItemRemoved"));
				}
			else if ( (&aView == iRemoteView) && (aEvent.iEventType == TContactViewEvent::EItemAdded) )
				{
				iLog.LogLine ( _L("=== Capature EItemAdded"));
				}
			iAddNumberReceivedResults.Append( aEvent.iEventType );
			iAddNumberReceivedResults.Append( aEvent.iInt );
			}
			
			if (iAddNumberReceivedResults.Count() == 8 )
				{
				if (iTestState == EPhoneNumberChangedEvent1)
					{
					AddNumberCompareRecordedResultWithExpected(iAddNumberRemoteViewResults1);
					iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
					}
				else if (iTestState == EPhoneNumberChangedEvent2)
					{
					AddNumberCompareRecordedResultWithExpected(iAddNumberRemoteViewResults2);
					iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
					}
				else if (iTestState == EPhoneNumberChangedEvent3)
					{
					AddNumberCompareRecordedResultWithExpected(iAddNumberRemoteViewResults3);
					iTestState = static_cast<TTest>( static_cast<TInt>(iTestState) + 1 );
					}
				else
					{
					AddNumberCompareRecordedResultWithExpected(iAddNumberRemoteViewResults4);
					iTestState = EEndTest;
					}	
				}
		 	
			CompleteRequest();
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

@SYMTestCaseID     PIM-T-EVENTORDER-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	test.Start(_L("@SYMTESTCaseID:PIM-T-EVENTORDER-0001 Testing EventOrder"));

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
