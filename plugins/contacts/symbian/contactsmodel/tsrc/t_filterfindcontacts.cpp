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
* Tests the following:
* 1: Addition and removal of contacts from a CContactFindView
* 2: Database update when editing an unsortable contact ie making it sortable
*
*/


#include <e32test.h>	
#include <f32file.h>	
#include <cntdb.h>				
#include <cntviewbase.h>
#include <cntitem.h>

#include "t_filterfindcontacts.h"
#include "t_utils.h"	
#include "t_utils2.h"	

_LIT(KTestName,			"T_FilterFindContacts Test");

_LIT(KTestDbName,		"C:T_FilterFindContacts.cdb");

RTest test(KTestName);

CFilterFindContactsTester* CFilterFindContactsTester::NewL(CContactDatabase& aDb)
	{
	CFilterFindContactsTester* self = new (ELeave) CFilterFindContactsTester (aDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CFilterFindContactsTester::ConstructL()
	{
	iAddedFindContacts = EFalse;
	iSortOrder.AppendL(KUidContactFieldGivenName);
	iSortOrder.AppendL(KUidContactFieldFamilyName);
	iSortOrder.AppendL(KUidContactFieldCompanyName);
	NextTest();
	}

CFilterFindContactsTester::CFilterFindContactsTester(CContactDatabase& aDb)
	: CActive(EPriorityStandard), iDb(aDb)
	{
	CActiveScheduler::Add(this);
	}

CFilterFindContactsTester::~CFilterFindContactsTester()
	{
	iSortOrder.Close();
	delete iFindDesArray;
	iFindView->Close(*this);
	iFindView = NULL;
	iLocalView->Close(*this);
	iLocalView = NULL;
	}

void CFilterFindContactsTester::NextTest()
	{
	++iCurrentTest;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

//============================================================================
// CFilterFindContactsTester::HandleContactViewEvent
// From MContactViewObserver
// Allows this class to recieve notification of any view events
//============================================================================
void CFilterFindContactsTester::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& aEvent)
	{

	switch (iCurrentTest)
		{
		case ECreateLocalView:
		case ECheckAddContactEntry:
		case ECreateFindView:
			{
			NextTest();
			}
		break;
		case EFindAddContacts:
			{
			if (aEvent.iEventType == TContactViewEvent::EItemAdded && iAddedFindContacts == EFalse)
				{
				iAddedFindContacts = ETrue;
				NextTest();
				}
			}
		break;
		}
	}

//============================================================================
// CFilterFindContactsTester::RunL
// From CActive
// Performs the following actions:
// 1: Creates a CContactLocalView and CContactFindView
// 2: Creates 2 contacts, one of which will not be visible in either view
//    as it has no sortable text fields 
// 3: Modifies and updates the unsortable contact to be sortable
// 4: Deletes the contact
//============================================================================

void CFilterFindContactsTester::RunL()
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			{
			test.Next(_L("Creating local view"));
			iLocalView = CContactLocalView::NewL(*this,iDb,iSortOrder,EIgnoreUnSorted);
			}
		break;

		case ECheckAddContactEntry:
			{
			// test step for for DEF065179- bug in contact views AllFieldsLC() 
			test.Next(_L("Adding contact entry: John,Smith,Symbian"));

			AddEntryL();
			}
		break;

		case ECreateFindView:
			{
			test.Next(_L("Creating Find View, seachstring abc"));

			//Create a find view based on the local view
			iFindDesArray =new (ELeave) CPtrCArray(1);
			iFindDesArray->AppendL(_L("abc"));
			iFindView = CContactFindView::NewL(iDb,*iLocalView,*this,iFindDesArray);
			}
		break;
		case EFindAddContacts:
			{
			DisplayL();

			test.Next(_L("Adding two contacts in find view"));

			CContactTemplate*  iTemplate=STATIC_CAST(CContactTemplate*,iDb.ReadContactL(iDb.TemplateId()));
			CTestContact* contact = CTestContact::NewLC(*iTemplate);
			contact->SetFirstNameL(_L("ABC"));
			
			iFindAdd1 = iDb.AddNewContactL(contact->ContactItem());
			
			CTestContact* unsortableContact = CTestContact::NewLC(*iTemplate);
			iFindAdd2 = iDb.AddNewContactL(unsortableContact->ContactItem());

			delete iTemplate;
			iTemplate = NULL;

			CleanupStack::PopAndDestroy(2); //contact , unsortableContact
			
			}
		break;
		case EModifyUnsortableContact:
			{
			DisplayL();
			test.Next(_L("Modifying Unsortable contact"));

			CContactItem* item=iDb.OpenContactL(iFindAdd2);
			CleanupStack::PushL(item);
			TBuf<16> name;
			name.Format(_L("ABC 123456789"));
			SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,name,EFalse);
			iDb.CommitContactL(*item);
			CleanupStack::PopAndDestroy(); // item;
			Cancel();
			NextTest();
			}
		break;
		case EFindRemoveContacts:
			{
			DisplayL();
			test.Next(_L("Removing contact from find view"));

			iDb.DeleteContactL(iFindAdd2);
			NextTest();
			}
		break;
		
		case ECheckCorrectSeparators:
			// test step for for DEF065179 - bug in contact views AllFieldsLC() 
			{
			test.Next(_L("Get fields using AllFieldsLC and check commas"));

			
			_LIT(KFieldSeparator,",");
			TInt index;
			index  = iLocalView->FindL(iContactId);
			HBufC* text= iLocalView->AllFieldsLC(index,KFieldSeparator);

			_LIT(KExpectedText,"John,Smith,Symbian");
			test (text->Des() == KExpectedText);

			CleanupStack::PopAndDestroy(); // text
			NextTest();
			}
		break;
		
		case ETestsComplete:
			{
			DisplayL();
			test.Next(_L("Tests complete"));

			Cancel();
			CActiveScheduler::Stop();
			delete this;
			}
		break;
		default:
			{
			}
		break;
		}
	}

TInt CFilterFindContactsTester::RunError(TInt aError)
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView: test.Printf(_L("test failed at step CreateLocalView (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateFindView: test.Printf(_L("test failed at step  CreateFindView (%i) with error %i"), iCurrentTest, aError); break;
		case EFindAddContacts: test.Printf(_L("test failed at step FindAddContacts (%i) with error %i"), iCurrentTest, aError); break;
		case EModifyUnsortableContact: test.Printf(_L("test failed at step ModifyUnsortableContacts (%i) with error %i"), iCurrentTest, aError); break;
		case EFindRemoveContacts: test.Printf(_L("test failed at step FindRemoveContacts (%i) with error %i"), iCurrentTest, aError); break;
		case ETestsComplete: test.Printf(_L("test failed at step TestsComplete (%i) with error %i"), iCurrentTest, aError); break;

		default: test.Printf(_L("test failed at step %i with error %i"), iCurrentTest, aError); break;
		}

	// stop the test
	test(aError != KErrNone);
	return KErrNone;
	}

//============================================================================
// CFilterFindContactsTester::DisplayL()
// Prints side by side comparison of the CContactFindView and the 
// CContactLocalView. 
//============================================================================
void CFilterFindContactsTester::DisplayL()
	{
	test.Printf(_L("Database Count: \t%i\n"),	iDb.CountL());
	test.Printf(_L("LocalView Count:\t%i\n"),	iLocalView->CountL());
	test.Printf(_L("Findview Count: \t%i\n"),	iFindView->CountL());

	for (TInt x = 0; x < iLocalView->CountL(); x++)
		{
		HBufC* text1 = iLocalView->AllFieldsLC(x,_L(","));
		test.Printf(_L("LocalView: %d:\t"),x+1);

		TPtrC16 firstName = text1->Des();
		TInt length = firstName.Locate(',');
		test.Printf(firstName.Left(length));
		test.Printf(_L("\t"));
		CleanupStack::PopAndDestroy();

		if (x < iFindView->CountL())
			{
			HBufC* text2 = iFindView->AllFieldsLC(x,_L(","));
			test.Printf(text2->Des() );
			test.Printf(_L("FindView: %d:\t"),x+1);
			TPtrC16 firstName = text2->Des();
			TInt length = firstName.Locate(',');
			test.Printf(firstName.Left(length));
			CleanupStack::PopAndDestroy();
			}
		test.Printf(_L("\n"));
		}
	test.Printf(_L("\n"));
	}

void CFilterFindContactsTester::DoCancel()
	{
	}

CFilterFindContactsTestConductor* CFilterFindContactsTestConductor::NewL()
	{
	CFilterFindContactsTestConductor* self=new(ELeave) CFilterFindContactsTestConductor();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CFilterFindContactsTestConductor::~CFilterFindContactsTestConductor()
	{
	delete iDb;
	delete iRandomGenerator;
	}

CFilterFindContactsTestConductor::CFilterFindContactsTestConductor() 
	{
	}

void CFilterFindContactsTestConductor::ConstructL()
	{
	iDb=CContactDatabase::ReplaceL(KTestDbName);
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);
	AddSomeContactsL(10);
	}

void CFilterFindContactsTestConductor::RunTestsL()
	{
	CFilterFindContactsTester* tester = CFilterFindContactsTester::NewL(*iDb);
	CleanupStack::PushL(tester);
	CActiveScheduler::Start();
	CleanupStack::Pop(tester);
	}
	
//============================================================================
// CFilterFindContactsTester::AddEntryL
// Adds a contact item to the contact database.
//============================================================================ 
void CFilterFindContactsTester::AddEntryL()
	{
	_LIT(KForename,"John"); 
	_LIT(KSurname,"Smith"); 
	_LIT(KCompanyName,"Symbian"); 

	// Create a  contact card to contain the data
	CContactCard* newCard = CContactCard::NewLC();
    
	// Create the firstName field and add the data to it
	CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	firstName->TextStorage()->SetTextL(KForename);
	newCard->AddFieldL(*firstName);
  	CleanupStack::Pop(firstName);
  	
	// Create the lastName field and add the data to it
   	CContactItemField* lastName= CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
  	lastName ->TextStorage()->SetTextL(KSurname);
  	newCard->AddFieldL(*lastName);
   	CleanupStack::Pop(lastName);
   		    
	// Create the company name and add the data to it
  	CContactItemField* companyName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCompanyName);
	companyName->SetMapping(KUidContactFieldCompanyName);
	companyName ->TextStorage()->SetTextL(KCompanyName);
	newCard->AddFieldL(*companyName);
	CleanupStack::Pop(companyName);
	
	// Add newCard to the database
	iContactId = iDb.AddNewContactL(*newCard);
	CleanupStack::PopAndDestroy(newCard);
	}

//============================================================================
// CFilterFindContactsTestConductor::AddSomeContactsL
// Creates a number of contacts and adds them to the database
//============================================================================
void CFilterFindContactsTestConductor::AddSomeContactsL(TInt aCount)
	{
	test.Printf(_L("Adding %d contacts directly to database\n"),aCount);

	CContactIdArray* contacts = CContactIdArray::NewLC();
	CRandomContactGenerator* generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL( generator );
	generator->SetDbL(*iDb);

	TInt bit = CContactDatabase::ESmsable;
	
	TContactItemId itemId = 0;
	
		for(TInt i = 1;i <= aCount;i++)
		{
		itemId = generator->AddTypicalContactForFilterL(bit);
		contacts->AddL(itemId);
		}

	CleanupStack::PopAndDestroy( generator );
	CleanupStack::PopAndDestroy( contacts );

	}

//============================================================================
// doMainL
// Creates a CFilterFindContactsTestConducter, which creates a database
// instance and populates it with a few random contacts
//============================================================================
LOCAL_C void doMainL()
	{
	__UHEAP_MARK;
	CFilterFindContactsTestConductor* testConductor = CFilterFindContactsTestConductor::NewL();
	CleanupStack::PushL( testConductor );
	testConductor->RunTestsL();
	CleanupStack::PopAndDestroy( testConductor );
	__UHEAP_MARKEND;
	}

//============================================================================
// E32Main
// Main function called by E32
//============================================================================
/**

@SYMTestCaseID     PIM-T-FILTERFINDCONTACTS-0001

*/

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;											// Mark heap state
	test.Start(_L("@SYMTESTCaseID:PIM-T-FILTERFINDCONTACTS-0001 T_FilterFindContacts Test"));

	CTrapCleanup* cleanup	= CTrapCleanup::New();			// Get cleanup stack
	CActiveScheduler* as	= new (ELeave) CActiveScheduler;// Get Active Scheduler
	CActiveScheduler::Install(as);							// Install Active Scheduler
	TRAPD(ret,doMainL());									// Run the test
	test(ret == KErrNone);
	delete as;												// Destroy the active scheduler
	delete cleanup;											// destroy clean-up stack
	test.End();
 	test.Close();
	__UHEAP_MARKEND;										// check no memory leak
	return (KErrNone);										// and return
    }
