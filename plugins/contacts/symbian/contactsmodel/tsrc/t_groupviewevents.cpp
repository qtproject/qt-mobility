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
#include "t_groupviewevents.h"

// Test Macro
LOCAL_D RTest test(_L("t_GroupViewEvents"));

//
// Constants.
//
_LIT(KTestName,    "t_GroupViewEvents" );
_LIT(KLogFileName, "t_GroupViewEvents.log" );
_LIT(KDbFileName,  "c:t_GroupViewEvents.cdb" );
_LIT(KTextDefSeparator, "");

const TInt KNumContacts = 100;
const TInt KNumContactsInGroupOne = 11;
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
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL( KDbFileName ) );
	iFs.Close();
	}

CTestConductor::CTestConductor()
	{
	}

void CTestConductor::ConstructL()
	{
	User::LeaveIfError( iFs.Connect() );
	iLog = CLog::NewL( test, KLogFileName );
	iDb = CContactDatabase::ReplaceL( KDbFileName );
	iRandomGenerator = CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL( *iDb );
	AddContactsL();
	}

TContactItemId CTestConductor::AddRandomContactL()
    {
	TContactItemId id = iRandomGenerator->AddTypicalRandomContactL();

	iTotalContacts++;
    return id;
    }

void CTestConductor::AddContactsL()
	{
	iTotalContacts=0;
	TInt ii;
	for (ii=0; ii < KNumContacts; ii++)
		{
		iRandomGenerator->AddTypicalRandomContactL();
		iTotalContacts++;
		test.Printf(_L("Adding %d "),ii);
		}
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
    iGroupViewTwo->Close(*this);
	iGroupViewOne->Close(*this);
	iLocalView->Close(*this);
	iSortOrder_1.Close();
	delete iTextDef;
	delete iIdsInGroupViewOne;
	delete iGroupOne;
	}

CGroupViewTester::CGroupViewTester( CLog& aLog ,CTestConductor* aTestConductor,
    CContactDatabase& aDb) :
	CActive(EPriorityStandard), 
    iLog( aLog ), 
    iTestConductor( aTestConductor ),
    iDb( aDb ), 
    iCurrentTest( -1 ),
	iLastContactID(-1),
	iSecondLastContactID(-1),
	iThirdLastContactID(-1)     
	{
	CActiveScheduler::Add( this );
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
	iGroupOne = static_cast<CContactGroup*>( iDb.CreateContactGroupL( 
        KGroupOneName ) );

	for (TInt ii=1;ii<=KNumContactsInGroupOne;++ii)
		{
		iDb.AddContactToGroupL(ii,iGroupOne->Id());
		}

	iGroupOneId = iGroupOne->Id();
	delete iGroupOne;
	iGroupOne = NULL;
	iGroupOne = STATIC_CAST(CContactGroup*,iDb.ReadContactL(iGroupOneId));
	iIdsInGroupViewOne = iGroupOne->ItemsContainedLC();
	CleanupStack::Pop( iIdsInGroupViewOne );
	}

void CGroupViewTester::RunL()
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iLog.LogLine(_L("Creating local view"));
			iLocalView = CContactLocalView::NewL( *this, iDb, iSortOrder_1, 
                EContactAndGroups );
			break;

		case ECreateGroupOneView:
			iLog.LogLine(_L("Creating group view one."));
            
            // Create a version 2 object.
			iGroupViewOne = CContactGroupView::NewL( *iLocalView, iDb, *this,
                iGroupOne->Id(), CContactGroupView::EShowContactsInGroup );
			break;

        case ECreateGroupTwoView:
            {
			iLog.LogLine(_L("Creating group view two."));
			iGroupViewTwo = CContactGroupView::NewL( *iLocalView, iDb, *this,
                KNullContactId, CContactGroupView::EShowContactsNotInAnyGroup );
            }
            break;

        case EDeleteItemsFromView:
            {
            iLog.LogLine(_L("Deleting contacts from group one."));

            TInt id1 = 1;
            TInt index = iGroupViewOne->FindL( id1 );
            iLog.LogLine( _L("Deleting contact id %d. index = %d"), id1, index );

            TInt id2 = 2;
            index = iGroupViewOne->FindL( 2 );
            iLog.LogLine( _L("Deleting contact id %d. index = %d"), id2, index );

            // Delete two contacts using a bulk delete.
            TInt numContactsInGroup = iGroupViewOne->CountL();
            CContactIdArray* itemsToDelete = CContactIdArray::NewL();
            CleanupStack::PushL( itemsToDelete );
            itemsToDelete->AddL( id1 );
            itemsToDelete->AddL( id2 );
            iDb.DeleteContactsL( *itemsToDelete );
            CleanupStack::PopAndDestroy( itemsToDelete );

            break;
            }

        case EAddItemToGroup:
            {
            iLog.LogLine(_L("Adding contact to database and group one."));
            // Create a new item and add it to group one.
            iIdAdded = iTestConductor->AddRandomContactL();
            iDb.AddContactToGroupL( iIdAdded, iGroupOne->Id() );
            break;
            }

		case ENumTests:
            // Tests complete.
            iLog.LogLine(_L("All tests complete."));
			CActiveScheduler::Stop();
			delete this;
			break;

		default:
			ASSERT( EFalse );
			break;
		}
	}

TInt CGroupViewTester::RunError(TInt aError)
	{
	// propagate error
	iTestConductor->SetTestError(aError);

	switch (iCurrentTest)
		{
		case ECreateLocalView: 
            test.Printf(_L("Test failed at step CreateLocalView (%i) with error %i"), 
                iCurrentTest, aError); 
            break;

		case ECreateGroupOneView: 
            test.Printf(_L("Test failed at step ECreateGroupOneView (%i) with error %i"), 
                iCurrentTest, aError); 
            break;


		case ECreateGroupTwoView: 
            test.Printf(_L("Test failed at step ECreateGroupTwoView (%i) with error %i"), 
                iCurrentTest, aError); 
            break;

        case EDeleteItemsFromView:
            test.Printf(_L("Test failed at step EDeleteItemsFromView (%i) with error %i"), 
                iCurrentTest, aError);     
            break;

        case EAddItemToGroup:
            test.Printf(_L("Test failed at step EAddItemToGroup (%i) with error %i"), 
                iCurrentTest, aError);     
            break;

		default: 
            test.Printf(_L("Test failed at step %i with error %i"), 
                iCurrentTest, aError); 
            break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}

void CGroupViewTester::HandleContactViewEvent( const CContactViewBase& aView,
    const TContactViewEvent& aEvent )
	{
    // Log events from both underlying view and group view.
    LogEvent( const_cast<TContactViewEvent&>( aEvent ) );

	switch (iCurrentTest)
		{
		case ECreateLocalView:
			test(iLocalView==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
			break;

		case ECreateGroupOneView:
			{
			test(iGroupViewOne == &aView);
			test(aEvent.iEventType == TContactViewEvent::EReady);

			//Reserve the IDs of last three contacts in the group
			TInt index = KNumContactsInGroupOne-1;
            TRAPD(err, iLastContactID = iGroupViewOne->ContactAtL(index).Id() );
			if (err)
				{
				_LIT(KErrMsg, "Contact view error in CGroupViewTester::HandleContactViewEvent() assigning iLastContactID\n");
				test.Printf(KErrMsg);
				User::Invariant();
				}

            --index;
            TRAP(err, iSecondLastContactID = iGroupViewOne->ContactAtL(index).Id() );
			if (err)
				{
				_LIT(KErrMsg, "Contact view error in CGroupViewTester::HandleContactViewEvent() assigning iSecondLastContactID\n");
				test.Printf(KErrMsg);
				User::Invariant();
				}

            --index;
            TRAP(err, iThirdLastContactID = iGroupViewOne->ContactAtL(index).Id() );
			if (err)
				{
				_LIT(KErrMsg, "Contact view error in CGroupViewTester::HandleContactViewEvent() assigning iThirdLastContactID\n");
				test.Printf(KErrMsg);
				User::Invariant();
				}

			}
			break;

        case ECreateGroupTwoView:
			test( iGroupViewTwo == &aView );
			test( aEvent.iEventType == TContactViewEvent::EReady );
            break;

        case EDeleteItemsFromView:
            {
            // Bulk delete complete when the last Ready is received from the 
            // base view. This comes after the group view ready event. Break to 
            // run next test.
            TBool lastEvent = (aEvent.iEventType == TContactViewEvent::EReady) &&
                (&aView == iLocalView);
            if (lastEvent) 
                { 
                break; 
                }

            if (aEvent.iEventType == TContactViewEvent::EItemRemoved)
                {
                // Only proces events from group view.
                if (&aView == iGroupViewOne)
                    {
                    // Ensure that only 1 contact is removed per notification.
                     iNumRemovedEvents++;
                    TInt numItemsExpected = KNumContactsInGroupOne - iNumRemovedEvents;
		            
		            TInt numItems(0);
		            TRAPD(err, numItems = iGroupViewOne->CountL() );
					if (err)
						{
						_LIT(KErrMsg, "Contact view error in CGroupViewTester::HandleContactViewEvent() assigning numItems\n");
						test.Printf(KErrMsg);
						User::Invariant();
						}
		            test(numItems == numItemsExpected );

                    // Verify the contacts were deleted as expected.
                    TBool testVal(EFalse);
                    TInt index = numItemsExpected - 1;
					TRAP(err, testVal = (iGroupViewOne->ContactAtL(index).Id() == iLastContactID) );
					if (err)
						{
						_LIT(KErrMsg, "Contact view error in CGroupViewTester::HandleContactViewEvent() assigning testVal for iLastContactID\n");
						test.Printf(KErrMsg);
						User::Invariant();
						}
					test(testVal);

                    --index;
					TRAP(err, testVal = (iGroupViewOne->ContactAtL(index).Id() == iSecondLastContactID) );
					if (err)
						{
						_LIT(KErrMsg, "Contact view error in CGroupViewTester::HandleContactViewEvent() assigning testVal for iSecondLastContactID\n");
						test.Printf(KErrMsg);
						User::Invariant();
						}
					test(testVal);

                    --index;
					TRAP(err, testVal = (iGroupViewOne->ContactAtL(index).Id() == iThirdLastContactID) );
					if (err)
						{
						_LIT(KErrMsg, "Contact view error in CGroupViewTester::HandleContactViewEvent() assigning testVal for iThirdLastContactID\n");
						test.Printf(KErrMsg);
						User::Invariant();
						}
					test(testVal);
                    }

                // More events expected, don't run next test.
                return;
                }
            else
                {
                // Ignore all other events.
                return;
                }
            }

        case EAddItemToGroup:
            {
            // Break to run next test if this is the last expected event. The last
            // event is the group being added from the base view. The local view
            // removes and then adds the group id whenever the group changes. These
            // events are meaningless.
            TBool lastEvent = (&aView == iLocalView) && 
                (aEvent.iEventType == TContactViewEvent::EItemAdded) &&
                (aEvent.iContactId == iGroupOneId);
            if (lastEvent)
                {
                break; // Run next test.
                }

            // Verify contact has been added.
            if (aEvent.iEventType == TContactViewEvent::EItemAdded && 
                &aView == iGroupViewOne)
                {
                // Check if contact was added.
                test( aEvent.iContactId == iIdAdded );

                TInt index = iGroupViewOne->FindL( iIdAdded );
                test (index != KErrNotFound);

                // More events expected, don't run next test.
                return;
                }
            else
                {
                // Ignore all other events.
                return;
                }
            }

		default:
			test( EFalse );
			break;
		}

    // Run the next test.
	if (--iNumNotificationExpected <= 0)
		{
		NextTest();
		iNumNotificationExpected = 0;
		}
	}

void CGroupViewTester::NextTest()
	{
	++iCurrentTest;
	TRequestStatus *pS = &iStatus;
	User::RequestComplete( pS, KErrNone );
	SetActive();
	}

void CGroupViewTester::DoCancel()
	{
	}

void CGroupViewTester::LogEvent( TContactViewEvent& aEvent )
    {
    _LIT( KLogLine, "Event occurred. Type: %S, iInt: %d, contact id: %d" );

    TBuf<256> buf; 
    _LIT( KUnavailable,      "EUnavailable" );
    _LIT( KReady,            "EReady" );
    _LIT( KSortOrderChanged, "ESortOrderChanged" );
    _LIT( KSortError,        "ESortError" );
    _LIT( KServerError,      "EServerError" );
    _LIT( KIndexingError,    "EIndexingError" );
    _LIT( KItemAdded,        "EItemAdded" );
    _LIT( KItemRemoved,      "EItemRemoved" );
    _LIT( KGroupChanged,     "EGroupChanged" );
    _LIT( KUnknown,          "Unknown" );

    const TDesC16* eventString = 0;
    switch (aEvent.iEventType)
        {
        case TContactViewEvent::EUnavailable:
            eventString = &KUnavailable;
            break;
        case TContactViewEvent::EReady:
            eventString = &KReady;
            break;
        case TContactViewEvent::ESortOrderChanged:
            eventString = &KSortOrderChanged;
            break;
        case TContactViewEvent::ESortError:
            eventString = &KSortError;
            break;
        case TContactViewEvent::EServerError:
            eventString = &KServerError;
            break;
        case TContactViewEvent::EIndexingError:
            eventString = &KIndexingError;
            break;
        case TContactViewEvent::EItemAdded:
            eventString = &KItemAdded;
            break;
        case TContactViewEvent::EItemRemoved:
            eventString = &KItemRemoved;
            break;
        case TContactViewEvent::EGroupChanged:
            eventString = &KGroupChanged;
            break;
        default:
            eventString = &KUnknown;
            break;
        };

    buf.Format( KLogLine, eventString, aEvent.iInt, aEvent.iContactId );
    test.Printf( buf );
    }

//
// Main.
//

/**

@SYMTestCaseID     PIM-T-GROUPVIEWEVENTS-0001

*/

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
			test.Start(_L("@SYMTESTCaseID:PIM-T-GROUPVIEWEVENTS-0001 t_GroupViewEvents"));

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
