/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Summary:
* A test to exercise the contacts model subview object and associated 
* functionality. The test includes creating, resorting and searching a view. 
* Actions:
* -- Create a new database;
* -- Populate database with randomly generated data;
* -- Create a local named view sorted by lname, fname, company and telephone;
* -- Create a subview based on the local view;
* -- Verify the subview has been created properly;
* -- Resort the local view;
* -- Verify the subview has been updated properly after resort;
* -- Clean up.
* Associated defect:
* DEF125393 PIM Sphinx QP: Improve test coverage of Cntmodel Subviews
* Written by:
* James Clarke
*
*/


#include "t_subview.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
// CSubViewTest methods
//////////////////////////////////////////////////////////////////////////////////////////////////

CSubViewTest* CSubViewTest::NewLC()
	{
	CSubViewTest* self = new (ELeave) CSubViewTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CSubViewTest::CSubViewTest() :
	CActive(CActive::EPriorityStandard)
	{
	}
	
	
void CSubViewTest::ConstructL()
	{
	CActiveScheduler::Add(this);

	// create a database populated with randomly generated contacts 
	iContactsDb = DbCreator::CreateDbL(KDbName, KNumContacts);
	}


CSubViewTest::~CSubViewTest()
	{
	Cancel();
	Cleanup();
	}


void CSubViewTest::Cleanup()
	{
	if (iNamedLocalView)
		{
		iNamedLocalView->Close(*this);
		iNamedLocalView = NULL;
		}
	if (iSubView)
		{
		iSubView->Close(*this);
		iSubView = NULL;
		}
	delete iContactsDb;
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbName));
	}


void CSubViewTest::RunL()
	{
	switch (iState)
		{
		case ETestStarted:
			CreateNamedLocalViewL();
			iState = ENamedLocalViewCreated;
			break;
		case ENamedLocalViewCreated: 
			test.Printf(_L("Local view created"));
			CreateSubViewL();
			iState = ESubViewCreated;
			break;
		case ESubViewCreated:
			test.Printf(_L("Sub view created"));
			PreResortTestL();
			ResortNamedLocalViewL();
			iState = EResorting;
			break;
		case ENamedLocalViewResorted: 
		case ESubViewUpdated: 
			break;
		case ESortingComplete:
			test.Printf(_L("Local view resorted"));
			test.Printf(_L("Sub view updated after local view resort."));
			PostResortTestL();
			CActiveScheduler::Stop();
			break;
		default:
			test.Printf(_L("Invalid state."));
			test(EFalse);
		}
	}



void CSubViewTest::CreateNamedLocalViewL()
	{
	test.Next(_L("Creating local view."));

	
	RContactViewSortOrder sortOrder;
	CleanupClosePushL(sortOrder);
	sortOrder.AppendL(KUidContactFieldFamilyName);
	sortOrder.AppendL(KUidContactFieldGivenName);
	sortOrder.AppendL(KUidContactFieldCompanyName);
	sortOrder.AppendL(KUidContactFieldPhoneNumber);

	// set the field indexes to reflect the order above
	iLnameIdx = 0;
	iFnameIdx = 1;
	iCnameIdx = 2;
	iNumIdx   = 3;
	
	iNamedLocalView = CContactNamedLocalView::NewL(*this, KTestName(), *iContactsDb, sortOrder, EContactsOnly);
	
	CleanupStack::PopAndDestroy(&sortOrder);
	}


void CSubViewTest::DoCancel()
	{
	}


void CSubViewTest::CreateSubViewL()
	{
	test.Next(_L("Creating subview."));


	iSubView = CContactSubView::NewL(*iNamedLocalView, *iContactsDb, *this, KLowBoundary(), KHighBoundary());
	}


void CSubViewTest::HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent)
	{

	if (aEvent.iEventType == TContactViewEvent::ESortOrderChanged)
		{
			if (iState == EResorting)
				{
				if (&aView == iNamedLocalView)
					{
					iState = ENamedLocalViewResorted;
					}
				else
					{
					iState = ESubViewUpdated; 
					}
				}
			else if (iState == ENamedLocalViewResorted || iState == ESubViewUpdated)
				{
				iState = ESortingComplete;
				}
			else
				{
				test.Printf(_L("Ooops! Something's gone wrong with the view-resorting states..."));
				test(EFalse);
				}
		}

	if (aEvent.iEventType == TContactViewEvent::EReady || iState == ESortingComplete)
		{
		TRequestStatus* pStat = &iStatus;
	  	User::RequestComplete(pStat, KErrNone);
	   	SetActive();
		}
	}


void CSubViewTest::PrintSubViewL() const
	{
	const TInt KLocalCount(iNamedLocalView->CountL());
	const TInt KSubCount(iSubView->CountL());
	test.Printf(_L("Subview contains %d of the %d contacts in the NamedLocalView."), KSubCount, KLocalCount);
	
	for(TInt i = 0; i < KSubCount; ++i)
		{
		const TPtrC fname = iSubView->ContactAtL(i).Field(iFnameIdx);
		const TPtrC lname = iSubView->ContactAtL(i).Field(iLnameIdx);
		const TPtrC cname = iSubView->ContactAtL(i).Field(iCnameIdx);
		const TPtrC num = iSubView->ContactAtL(i).Field(iNumIdx);
		test.Printf(_L("Contact #%d -- Person: %S %S; Company: %S; Phone number: %S."), 
			i + 1, &fname, &lname, &cname, &num);
		}
	}


void CSubViewTest::PreResortTestL() const
	{
	test.Next(_L("Verifying subview is correct after creation."));

	SubAndLocalViewCompareTestL();
	CheckSortOrderL();
	}

void CSubViewTest::PostResortTestL() const
	{
	test.Next(_L("Verifying subview is correct after resorting underlying view."));

	SubAndLocalViewCompareTestL();
	CheckSortOrderL();
	}


void CSubViewTest::SubAndLocalViewCompareTestL() const
	{
	test.Printf(_L("Testing subview is accurate compared to the local view."));

	const TInt KSubViewCount = iSubView->CountL();
	const TInt KLocalViewCount = iNamedLocalView->CountL();
	const TInt KFirstFieldIdx(0);
	
	TInt subViewIdx(0);
	TBool foundFirstInSubView(EFalse);
	TBool foundLastInSubView(EFalse);
	
	TInt subCntItemId(iSubView->AtL(subViewIdx));
	TInt localCntItemId(-1); // initialise with a value that can't be a real Id 
	for(TInt localViewIdx = 0; localViewIdx < KLocalViewCount && !foundLastInSubView; ++localViewIdx)
		{
		localCntItemId = iNamedLocalView->AtL(localViewIdx);

		// already found the first item in the sub view within
		// the underlying local view.
		if (foundFirstInSubView)
			{
			++subViewIdx;
			subCntItemId = iSubView->AtL(subViewIdx);
			// check that the contact ids in the sub view and local view match
			if (subCntItemId != localCntItemId)
				{
				test.Printf(_L("Mismatch of contacts between subview and local view"));
				test(EFalse);
				}

			// if we've found the last sub view item, do boundary checking to 
			// ensure we have the right things from the local view in the sub view. 
			if (subViewIdx == KSubViewCount - 1)
				{
				foundLastInSubView = ETrue;

				// Haven't checked localViewIdx < KLocalViewCount but it's not
				// likely that the last contact in the sub view will legitimately 
				// be the same as the last one in the local view.    
				const TPtrC nextLocalName = iNamedLocalView->ContactAtL(localViewIdx + 1).Field(KFirstFieldIdx);
				const TPtrC subName = iSubView->ContactAtL(subViewIdx).Field(KFirstFieldIdx);

				// check that the first character of the last contact in the subview
				// is within the proper range and that the next contact in the
				// local view is outside the range.
				test.Printf(_L("Found last contact from subview in local view. Testing for errors."));
				test(subName.Left(1).CompareF(KHighChar) <= 0 ||
						nextLocalName.Left(1).CompareF(KHighChar) > 0);
				test.Printf(_L("-- check okay."));
				}
			}

		// we've found the first contact item from the sub view in the local
		// view. check 
		else if (localCntItemId == subCntItemId && !foundFirstInSubView)
			{
			foundFirstInSubView = ETrue;

			// Haven't checked localViewIdx != 0 but it is not likely that the 
			// first contact in the local view will legitimately be the same as 
			// the first one in the sub view.
			const TPtrC prevLocalName = iNamedLocalView->ContactAtL(localViewIdx - 1).Field(KFirstFieldIdx);
			const TPtrC subName = iSubView->ContactAtL(subViewIdx).Field(KFirstFieldIdx);
						
			// check that the first character of the first contact in the subview
			// is within the proper range and that the previous contact in the
			// local view is outside the range.
			test.Printf(_L("Found first contact from subview in local view. Testing for errors."));
			test(subName.Left(1).CompareF(KLowChar) >= 0 &&
					prevLocalName.Left(1).CompareF(KLowChar) < 0);
			test.Printf(_L("-- check okay."));
			
			// check that we can find an expected contact item in the subview
			// and cannot find something that should not be there.
			test.Printf(_L("Testing FindL() functionality."));
			test(iSubView->FindL(subCntItemId) == subViewIdx &&
					iSubView->FindL(iNamedLocalView->AtL(localViewIdx - 1)) == KErrNotFound);
			test.Printf(_L("-- check okay."));
			
			// check the AllFieldsLC functionality
			test.Printf(_L("Testing AllFieldsLC() functionality."));
			
			_LIT(KDelimiter, ";");
			HBufC* allFields = iSubView->AllFieldsLC(subViewIdx, KDelimiter());
			TPtrC allFieldsPtr = allFields->Des();

			TBufC<16> field0 = iSubView->ContactAtL(subViewIdx).Field(0);
			TBufC<16> field1 = iSubView->ContactAtL(subViewIdx).Field(1);
			TBufC<16> field2 = iSubView->ContactAtL(subViewIdx).Field(2);
			TBufC<16> field3 = iSubView->ContactAtL(subViewIdx).Field(3);
			
			TBuf<48> fieldsBuf;
			_LIT(KFormat, "%S;%S;%S;%S");
			fieldsBuf.AppendFormat(KFormat(), &field0, &field1, &field2, &field3);
			
			test(fieldsBuf.Compare(allFieldsPtr) == 0); // they should be the same
			test.Printf(_L("-- check okay."));
			CleanupStack::PopAndDestroy(allFields);
			}
		}
	}


void CSubViewTest::CheckSortOrderL() const
	{
	test.Printf(_L("Checking sort order"));
	RContactViewSortOrder sortOrder = iSubView->SortOrderL(); // don't take ownership
	test(
			sortOrder[iLnameIdx] == KUidContactFieldFamilyName &&
			sortOrder[iFnameIdx] == KUidContactFieldGivenName &&
			sortOrder[iCnameIdx] == KUidContactFieldCompanyName &&
			sortOrder[iNumIdx]   == KUidContactFieldPhoneNumber &&
			iSubView->ContactViewPreferences() == EContactsOnly
		);
	}


void CSubViewTest::ResortNamedLocalViewL()
	{
	test.Next(_L("Resorting local view."));


	RContactViewSortOrder sortOrder;
	CleanupClosePushL(sortOrder);
	sortOrder.AppendL(KUidContactFieldCompanyName);
	sortOrder.AppendL(KUidContactFieldGivenName);
	sortOrder.AppendL(KUidContactFieldFamilyName);
	sortOrder.AppendL(KUidContactFieldPhoneNumber);

	// set the field indexes to reflect the order above
	iCnameIdx = 0;
	iFnameIdx = 1;
	iLnameIdx = 2;
	iNumIdx   = 3;
	
	iNamedLocalView->ChangeSortOrderL(sortOrder);

	CleanupStack::PopAndDestroy(&sortOrder);
	}


void CSubViewTest::DoTestL()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	iState = ETestStarted;
	CActiveScheduler::Start();
	}


//////////////////////////////////////////////////////////////////////////////////////////////////
// main test functions
//////////////////////////////////////////////////////////////////////////////////////////////////

/**

@SYMTestCaseID     PIM-T-SUBVIEW-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-SUBVIEW-0001 "));

	CSubViewTest* myTest = CSubViewTest::NewLC();
	myTest->DoTestL();

	// do some stuff here...

	CleanupStack::PopAndDestroy(myTest);
    test.End();
    test.Close();
    }

GLDEF_C TInt E32Main()
	{
    // Init
    __UHEAP_MARK; 
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
    if (err)
    	{
    	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbName) );
    	test(EFalse);
    	}
    
    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
    __UHEAP_MARKEND;
	return err;
    }
