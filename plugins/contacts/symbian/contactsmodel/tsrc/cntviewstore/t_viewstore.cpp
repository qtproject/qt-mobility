/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* RTest based unit test to verify the operation of the functions
* to get/set 'default' view definitions in cntmodel. The functions are:
* TContactViewStore::DefaultViewsL()
* TContactViewStore::SetDefaultViewsL()
*
*/


#include <e32base.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntviewbase.h>
#include <cntviewstore.h>
#include <centralrepository.h>
#include "t_viewstore.h"
#include "cntviewprivate.h"
#include "cntviewstoreprivate.h"

/*
  List of test cases
  
  Need to mainly test the set function, so test various
  combinations of setting view defs. e.g.
  
  - Set the default techview settings
  - Set multiple (valid) view settings
  - Set invalid view settings
  - Set mixed valid & invalid view settings
  
  Other tests that may be of use:
  
  - Different default repository files, some with invalid data?
  - Error conditions:
    - Calling DefaultViewsL() with a non-empty RArray: KErrArgument
    - (Set)DefaultViewsL() with no repository: KErrNotFound

*/

_LIT(KTitle, "t_viewstore");

RTest test(KTitle);

class CTestManager : public CBase, public MContactViewObserver
	{
public:	
	static CTestManager* NewLC();
	
	void HandleContactViewEvent(const CContactViewBase &aView, const TContactViewEvent &aEvent);
	
	void TestDefaultDefinitionL();
	void TestInvalidViewNameL();
	void TestInvalidSortPluginNameL();
	void TestMultipleValidViewDefinitionsL();
	void TestMultipleValidInvalidViewDefinitionsL();
	void TestRetrievingExistingViewsL();	
	void TestDeleteReplaceL();
	
private:
	CTestManager() {}
	~CTestManager();
	void ConstructL();
	void Close();
	
private:
	CContactDatabase* iDb;
	RPointerArray<CContactViewBase> iViews;
	};

/*
  operator == for CContactDefaultViewDefinition
*/
TBool operator == (const CContactDefaultViewDefinition& viewDef1, const CContactDefaultViewDefinition& viewDef2)
	{
	if (viewDef1.ViewPreferences() != viewDef2.ViewPreferences())
		{
		return EFalse;
		}
		
	if (!(viewDef1.SortOrder() == viewDef2.SortOrder()))
		{
		return EFalse;
		}
					
	if (viewDef1.ViewNameL().Compare(viewDef2.ViewNameL()) != 0)
		{
		return EFalse;
		}
		
	if (viewDef1.SortPluginNameL().Compare(viewDef2.SortPluginNameL()) != 0)
		{
		return EFalse;
		}
	
	if (viewDef1.ViewType() != viewDef2.ViewType())
		{
		return EFalse;
		}
	
	return ETrue;
	}

/*
*/
CContactViewDefinitionGenerator* CContactViewDefinitionGenerator::NewLC()
	{
	CContactViewDefinitionGenerator* self = new (ELeave) CContactViewDefinitionGenerator;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*
*/
void CContactViewDefinitionGenerator::ConstructL()
	{
	}
	
/*
*/
CContactDefaultViewDefinition* CContactViewDefinitionGenerator::CreateDefinitionLC(TViewDescription aDesc)
	{
	CContactDefaultViewDefinition* viewDef = 0;
	RContactViewSortOrder viewSortOrder;
	CleanupClosePushL(viewSortOrder);

	switch (aDesc)
		{
		case ETechviewContactsAppView:
			{
			// This is the view definition used
			// by the Techview Contacts application
			_LIT(KContactsAppViewName, "KMainContactsAppView");
			_LIT8(KSortPluginName, "");
			
			viewSortOrder.AppendL(KUidContactFieldFamilyName);
			viewSortOrder.AppendL(KUidContactFieldGivenName);
			viewSortOrder.AppendL(KUidContactFieldCompanyName);
			
			viewDef = CContactDefaultViewDefinition::NewLC(CContactDefaultViewDefinition::ENamedRemoteView, 
												   KContactsAppViewName, viewSortOrder, 
												   EContactsOnly, KSortPluginName);
			break;
			}
		case EValidLFCView:
			{
			// This is a generic remote view typical
			// of what might be used on a phone. It
			// sorts the contacts based on last name,
			// first name and company name, in that order.
			_LIT(KGenericLFCViewName, "KGenericLFCView");
			_LIT8(KSortPluginName, "");
			
			viewSortOrder.AppendL(KUidContactFieldFamilyName);
			viewSortOrder.AppendL(KUidContactFieldGivenName);
			viewSortOrder.AppendL(KUidContactFieldPhoneNumber);
			
			viewDef = CContactDefaultViewDefinition::NewLC(CContactDefaultViewDefinition::ENamedRemoteView, 
												   KGenericLFCViewName, viewSortOrder, 
												   EContactsOnly, KSortPluginName);
			break;
			}
		case EValidLFPView:
			{
			// This is a generic remote view typical
			// of what might be used on a phone. It
			// sorts the contacts based on last name,
			// first name and phone number, in that order.
			_LIT(KGenericLFPViewName, "KGenericLFPView");
			_LIT8(KSortPluginName, "");
			
			viewSortOrder.AppendL(KUidContactFieldPhoneNumber);
			viewSortOrder.AppendL(KUidContactFieldGivenName);
			viewSortOrder.AppendL(KUidContactFieldFamilyName);
			viewSortOrder.AppendL(KUidContactFieldCompanyName);
			
			viewDef = CContactDefaultViewDefinition::NewLC(CContactDefaultViewDefinition::ENamedRemoteView, 
												   KGenericLFPViewName, viewSortOrder, 
												   EContactsOnly, KSortPluginName);
			break;
			}
		case EInvalidViewNameView:
			{
			// This view is invalid because the view name
			// is longer than KMaxUnicodeStringLength, the 
			// maximum length descriptor that can be stored
			// in a repository key.
			_LIT(KInvalidViewName, "KViewWithANameThatIsLongerThanTheMaximumAllowedLengthInACentralRepositoryKey");
			_LIT8(KSortPluginName, "");
			
			TBuf<NCentralRepositoryConstants::KMaxUnicodeStringLength + 2> invalidDesc;
			invalidDesc.Copy(KInvalidViewName);
			invalidDesc.AppendFill('*', (invalidDesc.MaxLength() - invalidDesc.Length()));
			
			viewSortOrder.AppendL(KUidContactFieldGivenName);
			
			viewDef = CContactDefaultViewDefinition::NewLC(CContactDefaultViewDefinition::ENamedRemoteView, 
												   invalidDesc, viewSortOrder, 
												   EContactsOnly, KSortPluginName);
			break;
			}
		case EInvalidSortPluginNameView:
			{
			// This view is invalid because the view name
			// is longer than KMaxBinaryLength, the 
			// maximum length 8-bit descriptor that can be 
			// stored in a repository key
			_LIT(KInvalidSortPluginViewName, "KInvalidSortPluginNameView");
			_LIT8(KInvalidSortPluginName, "InvalidSortPluginNameThatIsTooLongToFitIntoACentralRepositoryKey");
			
			TBuf8<NCentralRepositoryConstants::KMaxBinaryLength + 2> invalidDesc;
			invalidDesc.Copy(KInvalidSortPluginName);
			invalidDesc.AppendFill('*', (invalidDesc.MaxLength() - invalidDesc.Length()));
			
			viewSortOrder.AppendL(KUidContactFieldFamilyName);
			viewSortOrder.AppendL(KUidContactFieldGivenName);
			
			viewDef = CContactDefaultViewDefinition::NewLC(CContactDefaultViewDefinition::ENamedRemoteView, 
												   KInvalidSortPluginViewName, viewSortOrder, 
												   EContactsOnly, invalidDesc);
			break;
			}
		default:
			{
			User::Leave(KErrArgument);
			}
		}

	CleanupStack::Pop(viewDef);
	CleanupStack::PopAndDestroy(&viewSortOrder);
	CleanupStack::PushL(viewDef);	
	return viewDef;
	}

//
// CTestManager
//
CTestManager::~CTestManager()
	{
	Close();
	}
	
CTestManager* CTestManager::NewLC()
	{
	CTestManager* self = new (ELeave) CTestManager;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTestManager::ConstructL()
	{
	}
	
void CTestManager::Close()
	{
	for (TInt i = 0; i < iViews.Count(); i++)
		{
		iViews[i]->Close(*this);
		}
	iViews.Close();
	if (iDb)
		{
		delete iDb;
		iDb = NULL;
		}
	}

/*
  This gets called when a view is ready to use
*/
void CTestManager::HandleContactViewEvent(const CContactViewBase& /*aView*/, const TContactViewEvent& /*aEvent*/)
	{
	CActiveScheduler::Stop();
	}

/*
  Test the storing and retrieving of the
  view definition used by the Techview
  Contacts app
*/
void CTestManager::TestDefaultDefinitionL()
	{	
	CContactViewDefinitionGenerator* gen = CContactViewDefinitionGenerator::NewLC();
	RPointerArray<CContactDefaultViewDefinition> viewDefs;
	CleanupResetAndDestroyPushL(viewDefs);
	
	// Store the 'default' view definition
	CContactDefaultViewDefinition* viewDef = gen->CreateDefinitionLC(CContactViewDefinitionGenerator::ETechviewContactsAppView);
	viewDefs.AppendL(viewDef);	
	TRAPD(err, TContactViewStore::SetDefaultViewsL(viewDefs));
	test(err == KErrNone);
	viewDefs.Reset();
	
	// Retrieve the view definition(s) and compare to what was stored
	//for (TInt i=4 ;; i++)
	//	{
		//__UHEAP_SETFAIL(RHeap::EFailNext, i);
		//__UHEAP_MARK;
		TRAP(err, TContactViewStore::GetDefaultViewsL(viewDefs));
		//viewDefs.ResetAndDestroy();
		//__UHEAP_MARKEND;
		//__UHEAP_RESET;
		
	//	if (err != KErrNoMemory)
	//		{
	//		break;
	//		}
	//	}
	test(err == KErrNone);
	test(viewDefs.Count() == 1);
	test(*viewDef == *viewDefs[0]);
	
	CleanupStack::PopAndDestroy(viewDef);
	CleanupStack::PopAndDestroy(&viewDefs);
	CleanupStack::PopAndDestroy(gen);	
	}
	
/*
  Test the storing of a view with an invalid name. 
  i.e. the name is to long to fit into the cenrep.
*/
void CTestManager::TestInvalidViewNameL()
	{
	CContactViewDefinitionGenerator* gen = CContactViewDefinitionGenerator::NewLC();
	CContactDefaultViewDefinition* viewDef = gen->CreateDefinitionLC(CContactViewDefinitionGenerator::EInvalidViewNameView);
	RPointerArray<CContactDefaultViewDefinition> viewDefs;
	
	// SetDefaultViewsL should leave with KErrArgument because
	// the view name is too long.
	viewDefs.AppendL(viewDef);
	CleanupStack::Pop(viewDef);
	TRAPD(err, TContactViewStore::SetDefaultViewsL(viewDefs));
	test(err == KErrArgument);
	
	viewDefs.ResetAndDestroy();
	CleanupStack::PopAndDestroy(gen);	
	}

/*
  Test the storing of a view with an invalid sort plugin name. 
  i.e. the name is to long to fit into the cenrep.
*/
void CTestManager::TestInvalidSortPluginNameL()
	{
	CContactViewDefinitionGenerator* gen = CContactViewDefinitionGenerator::NewLC();
	CContactDefaultViewDefinition* viewDef = gen->CreateDefinitionLC(CContactViewDefinitionGenerator::EInvalidSortPluginNameView);
	RPointerArray<CContactDefaultViewDefinition> viewDefs;
	
	// SetDefaultViewsL should leave with KErrArgument because
	// the view name is too long.
	viewDefs.AppendL(viewDef);
	CleanupStack::Pop(viewDef);
	TRAPD(err, TContactViewStore::SetDefaultViewsL(viewDefs));
	test(err == KErrArgument);
	
	viewDefs.ResetAndDestroy();
	CleanupStack::PopAndDestroy(gen);	
	}

/*
  Test the storing and retrieval of multiple
  (valid) view definitions.
*/
void CTestManager::TestMultipleValidViewDefinitionsL()
	{
	CContactViewDefinitionGenerator* gen = CContactViewDefinitionGenerator::NewLC();
	RPointerArray<CContactDefaultViewDefinition> viewDefs;
	CContactDefaultViewDefinition* viewDef;
	CContactViewDefinitionGenerator::TViewDescription desc;
	
	// create some view definitions
	for (TInt i = 0; i < 100; i++)
		{
		desc = static_cast<CContactViewDefinitionGenerator::TViewDescription>(i % CContactViewDefinitionGenerator::ENumValidViewDescriptions);
		viewDef = gen->CreateDefinitionLC(desc);
		viewDefs.AppendL(viewDef);
		CleanupStack::Pop(viewDef);
		}

	// store the view definitions
	TRAPD(err, TContactViewStore::SetDefaultViewsL(viewDefs));
	test(err == KErrNone);
	
	// retrieve the definitions
	RPointerArray<CContactDefaultViewDefinition> storedViewDefs;
	TRAP(err, TContactViewStore::GetDefaultViewsL(storedViewDefs));
	test(err == KErrNone);
	
	// check the retrieved ones match the ones which were stored
	test(storedViewDefs.Count() == viewDefs.Count());
	for (TInt j = 0; j < storedViewDefs.Count(); j++)
		{
		test(*viewDefs[j] == *storedViewDefs[j]);
		}
	
	viewDefs.ResetAndDestroy();
	storedViewDefs.ResetAndDestroy();
	CleanupStack::PopAndDestroy(gen);	
	}

/*
  Test the storing and retrieval of multiple
  valid and invalid view definitions. This
  should fail, leaving with KErrArgument
*/
void CTestManager::TestMultipleValidInvalidViewDefinitionsL()
	{
	CContactViewDefinitionGenerator* gen = CContactViewDefinitionGenerator::NewLC();
	RPointerArray<CContactDefaultViewDefinition> viewDefs;
	CContactDefaultViewDefinition* viewDef;
	CContactViewDefinitionGenerator::TViewDescription desc;
	
	// create some view definitions
	for (TInt i = 0; i < 10; i++)
		{
		desc = static_cast<CContactViewDefinitionGenerator::TViewDescription>(i % CContactViewDefinitionGenerator::ENumViewDescriptions);
		viewDef = gen->CreateDefinitionLC(desc);
		viewDefs.AppendL(viewDef);
		CleanupStack::Pop(viewDef);
		}

	// store the view definitions - this should leave
	// with KErrArgument because of the invalid view(s)
	TRAPD(err, TContactViewStore::SetDefaultViewsL(viewDefs));
	test(err == KErrArgument);
	
	viewDefs.ResetAndDestroy();
	CleanupStack::PopAndDestroy(gen);	
	}
	
/*
*/
void CTestManager::TestRetrievingExistingViewsL()
	{
	CContactViewDefinitionGenerator* gen = CContactViewDefinitionGenerator::NewLC();
	RPointerArray<CContactDefaultViewDefinition> viewDefs;
	CContactDefaultViewDefinition* viewDef;
	TInt i;
	
	// create some view definitions
	for (i = 0; i < CContactViewDefinitionGenerator::ENumValidViewDescriptions; i++)
		{
		viewDef = gen->CreateDefinitionLC(CContactViewDefinitionGenerator::TViewDescription(i));
		viewDefs.AppendL(viewDef);
		CleanupStack::Pop(viewDef);
		}

	// store the view definitions
	TRAPD(err, TContactViewStore::SetDefaultViewsL(viewDefs));
	test(err == KErrNone);
	
	// retrieve the definitions
	RPointerArray<CContactDefaultViewDefinition> storedViewDefs;
	TRAP(err, TContactViewStore::GetDefaultViewsL(storedViewDefs));
	test(err == KErrNone);
	
	// create the views that were retrieved
	TRAP(err, iDb = CContactDatabase::OpenL());
	if (err != KErrNone)
		{
		if (err == KErrNotFound)
			{
			RDebug::Printf("Error: Couldn't find default database.");
			}
		User::Leave(err);
		}
	CContactViewBase* view = NULL;
	for (i = 0; i < storedViewDefs.Count(); i++)
		{
		viewDef = storedViewDefs[i];
		if (viewDef->ViewType() == CContactDefaultViewDefinition::ERemoteView)
			{
			view = CContactRemoteView::NewL(*this, *iDb, viewDef->SortOrder(), viewDef->ViewPreferences());
			}
		else
			{
			view = CContactNamedRemoteView::NewL(*this, viewDef->ViewNameL(),
												*iDb, viewDef->SortOrder(), 
												viewDef->ViewPreferences());
			}
		iViews.Append(view);

		// Wait for the view to be created
		CActiveScheduler::Start();			
		}
		
	// get the list of existing views and checked against stored ones
	RPointerArray<CContactDefaultViewDefinition> existingViewDefs;
	TContactViewStorePrivate::GetDefinitionsOfExistingViewsL(KNullDesC, existingViewDefs);
	test(storedViewDefs.Count() == existingViewDefs.Count());
	for (i = 0; i < existingViewDefs.Count(); i++)
		{
		test(*existingViewDefs[i] == *storedViewDefs[i]);
		}
	
	existingViewDefs.ResetAndDestroy();
	storedViewDefs.ResetAndDestroy();
	viewDefs.ResetAndDestroy();
	CleanupStack::PopAndDestroy(gen);
	}

/*
*/
void CTestManager::TestDeleteReplaceL()
	{
	// Quick test: assuming server has been started in non-transient at start-up,
	// try to replace the default database.
	Close();

	CContactDatabase::DeleteDefaultFileL();
	
	// Start the contacts server in non-transient mode
	
	// Attempt to delete the default file. This should not be blocked.
	
	// Stop the contacts server
	
	// Start contacts server in non-transient mode
	
	// Attempt to replace default file.
	}
			
/*
  Implementation of test code
*/

/**

@SYMTestCaseID     PIM-T-VIEWSTORE-0001

*/

LOCAL_C void doMainL()
	{
	CTestManager* testMgr = CTestManager::NewLC();
	
	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEWSTORE-0001 Storing the Techview Contacts App view definition"));

	testMgr->TestDefaultDefinitionL();
	
	test.Next(_L("Storing a view definition with invalid view name"));

	testMgr->TestInvalidViewNameL();
	
	test.Next(_L("Storing a view definition with invalid sort plugin name"));

	testMgr->TestInvalidSortPluginNameL();
	
	test.Next(_L("Storing multiple valid view definitions"));

	testMgr->TestMultipleValidViewDefinitionsL();
	
	test.Next(_L("Storing mixed valid and invalid views"));

	testMgr->TestMultipleValidInvalidViewDefinitionsL();
	
	test.Next(_L("Retrieving the list of views that exist in the server"));

	testMgr->TestRetrievingExistingViewsL();
	
	test.Next(_L("Delete/Replace of default database when server is non-transient"));

	testMgr->TestDeleteReplaceL();

	CleanupStack::PopAndDestroy(testMgr);
	}

/*
  t_viewstore entry point
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	
	test.Title();
		
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}
		
	CActiveScheduler * scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err, doMainL());
	if (err != KErrNone)
		{
		_LIT(KOutputRes,"t_viewstore returned with %d \n");
		TBuf<64> formattable;
		formattable.Format(KOutputRes, err);
		test.Printf(formattable);
		}
	test(err == KErrNone);

	delete scheduler;
	delete cleanup;
		
	test.End();
	test.Close();	
	
	__UHEAP_MARKEND;
	return err;
	}
