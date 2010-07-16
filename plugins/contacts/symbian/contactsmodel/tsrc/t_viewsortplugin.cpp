/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32file.h>
#include <e32math.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <cntview.h>
#include <coreappstest/testserver.h>

_LIT(KTestName, "T_ViewSortPlugin");
#include "testhelpers.h" //defines rtest test;
#include "t_viewsortplugin.h"

LOCAL_D	RFs theFs;

_LIT8(KReverseSortPlugin, "application.vnd.symbian.com/contacts-reverse-sort"); //SimpleSortPlugin DLL Unique name

LOCAL_C void CopyFileL(const TDesC& aFile1, const TDesC& aFile2)
	{
    RPIMTestServer serv;
    User::LeaveIfError(serv.Connect());
    serv.DeleteFileL(aFile2);
    serv.CopyFileL(aFile1, aFile2);
    serv.Close();
	}

// This test harness uses special framework for asynchronous function testing
// The test contains of steps. Every step is presented by a function with a name
// starting with "Step". Predefined step "StepDone" shut the framework down and
// must always be in the end of the Step list

// Step list is the function pointers array:
// All of the steps will be called consiquenlty from RunL() function of framework
void (CTestMachine::*StepFunc[])(void)= 
	{
	&CTestMachine::StepRestoreDatabaseL,
	&CTestMachine::StepNamedRemoteViewsL,
	&CTestMachine::StepFindViewsL,
	&CTestMachine::StepDone
	}; 

_LIT(KTestFile,"c:T_ViewSortPlugin.test");
_LIT(KTestFileBase,"c:T_ViewSortPlugin.base");

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
_LIT(KTestFileBaseFullPath,"C:\\private\\10003A73\\T_ViewSortPlugin.base");
_LIT(KTestFileFullPath,"C:\\private\\10003A73\\T_ViewSortPlugin.test");
#else
_LIT(KTestFileBaseFullPath,"C:\\private\\10003A73\\SQLite__T_ViewSortPlugin.base");
_LIT(KTestFileFullPath,"C:\\private\\10003A73\\SQLite__T_ViewSortPlugin.test");
#endif

const TInt64 KRandomSeed=648;


CTestMachine::CTestMachine() 
	: CTimer(EPriorityStandard), iSeed(KRandomSeed), iStep(-1), iSubStep(0), 
	 iNumOfEvents(0)
	{
	};

void CTestMachine::RunL()
	{
	(this->*StepFunc[iStep])();
	}

TInt CTestMachine::RunError(TInt aErrNum)
	{
	test.Printf(_L("RunError called with %d...\n"), aErrNum);
	return KErrNone;
	}

void CTestMachine::StepRestoreDatabaseL()
	{
	test.Printf(_L("Restoring Database..."));
	delete iDb; iDb = NULL; //Close

	TESTTRAPL(CopyFileL(KTestFileBaseFullPath, KTestFileFullPath));
	//Open
	test.Printf(_L("Opening cntmodel..."));
	iDb=CContactDatabase::OpenL(KTestFile);
	
	NextStep();
	//User::After(1000000);
	test.Printf(_L("done!\n"));
	}


//// Macros for asynchronous operations ////
// These macros is the main part of the framework
// Every step consists of several substeps. Between the steps active object
// is idle and waiting for request to be completed, allowing other object to do their work
// To provide this functionality framework puts hidden switch statement in the RunL 
// fuction of its active object. The switch is actually hidden by these macros:

// Don't be afraid of the macros, use them like this:
// void CTestMachine::StepMy()
//		{
//		BEGIN_ASYNC_TEST
//		WAIT_EVENT(1)
//		WAIT_EVENT(2)
//		...
//		WAIT_EVENT(n)
//		END_ASYNC_TEST
//		}
// and you gonna be alright!

#define BEGIN_ASYNC_TEST	\
	switch(iSubStep)		\
	{						\
	case 0:					\
		{					

#define WAIT_EVENT(num)		\
		iNumOfEvents = 1;	\
		}					\
	break;					\
	case num:				\
		{	

#define WAIT_EVENTS(num,numOfEv)\
		iNumOfEvents = numOfEv;	\
		}					\
	break;					\
	case num:				\
		{	



#define WAIT_TIMER(num,time)\
		iNumOfEvents = 0;   \
		this->After(time);	\
		++iSubStep;			\
		}					\
	break;					\
	case num:				\
		{	

#define END_ASYNC_TEST	\
		NextStep();		\
		}				\
	break;				\
	default:			\
		test.Printf(_L("Test step %d doesn't contain substep %d"), iStep, iSubStep);	\
		User::Leave(KErrAbort);	}

///////////////////////////////////////
/////////////// STEPS /////////////////

void CTestMachine::StepFindViewsL()
	{
	const TContactViewPreferences viewPrefs = TContactViewPreferences(EUnSortedAtEnd | EContactsOnly);
	_LIT(KMansell, 	"Mansell");	

	BEGIN_ASYNC_TEST
		
		/////////////////////////////////////////
		//Actions:	Create a local view with reverse sort plugin
		TESTTRAP(iLocalView = CContactLocalView::NewL(*this, *iDb, iSurnNameSortOrder, viewPrefs, KReverseSortPlugin));
	WAIT_EVENT(1)

		//Expected Result:	Check the order of entries in the view - it must be reverse.
		test.Printf(_L("Local View created:\n"));
		CheckViewOrderL(iLocalView);
		test.Printf(_L("\n\n"));

		/////////////////////////////////////////
		//Actions:	Create a remote view with reverse sort plugin
		TESTTRAP(iRemoteViewReverse = CContactRemoteView::NewL(*this, *iDb, iSurnNameSortOrder, viewPrefs, KReverseSortPlugin));
	WAIT_EVENT(2)

		//Expected Result:	Check the order of entries in the view - it must be reverse.
		test.Printf(_L("Remote View created:\n"));
		CheckViewOrderL(iRemoteViewReverse);
		test.Printf(_L("\n\n"));

		/////////////////////////////////////////
		//Actions:	Create a remote view with default sorting behaviour
		TESTTRAP(iRemoteViewForward = CContactRemoteView::NewL(*this, *iDb, iSurnNameSortOrder, viewPrefs));
	WAIT_EVENT(3)

		//Expected Result:	Check the order of entries in the view - it must be forward
		test.Printf(_L("Remote View created:\n"));
		CheckViewOrderL(iRemoteViewForward, EFalse, EFalse); //do not Print, forward order
		test.Printf(_L("\n\n"));

		/////////////////////////////////////////
		//Actions:	Create a Find view on top of the local view, using "Ma" search string 
		iFindDesArray =new(ELeave)CPtrC16Array(1);
		_LIT(KMa,"Ma");
		iFindDesArray->AppendL(TPtrC(KMa));

		iLocalFindView= CContactFindView::NewL(*iDb,*iLocalView,*this,iFindDesArray);
	WAIT_EVENT(4)

		//Expected Result:	Check the order of entries in the find view - it must be reverse
		test.Printf(_L("LocalFind View created:\n"));
		CheckViewOrderL(iLocalFindView);
		test.Printf(_L("\n\n"));

		/////////////////////////////////////////
		//Actions:	Create a Find view on top of the reverse remote view, using "el" search string 
		iFindDesArray->Reset();
		_LIT(Kel,"el");
		iFindDesArray->AppendL(TPtrC(Kel));
		iRemoteFindView = CContactFindView::NewL(*iDb,*iRemoteViewReverse,*this,iFindDesArray);
	WAIT_EVENT(5)
		
		//Expected Result:	Check the order of entries in the find view - it must be reverse
		test.Printf(_L("RemoteFind View created:\n"));
		CheckViewOrderL(iRemoteFindView);
		test.Printf(_L("\n\n"));

		/////////////////
		//Actions:	Add a new contact to the database, it must have "ma" and "el" substrings in it 
		//          to appear in either find view. Check the sorting in every view.		
		_LIT8(KIrvVCard, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Mansell;Nigel;11\r\nPensioner\r\nEND:VCARD");
		RDesReadStream stream(KIrvVCard);
		stream.PushL();
		  TBool success(EFalse);
		  CArrayPtr<CContactItem>* contactItems=iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), stream, success, NULL);
		CleanupStack::PopAndDestroy(); // stream
		
		PUSH(contactItems);
		  TESTTRUE(success);
		  test.Printf(_L("%d entry was added created\n"), contactItems->Count());
		  contactItems->ResetAndDestroy(); //Potential memory leak - must be pushed onto the stack with ResetAndDestroy
		POPD(contactItems);

	WAIT_EVENTS(6,5) //Waiting for 5 events to arrive (one from each view) 
		//Note, that if all 5 events won't arrive the code will stuck here
		//It can be solved with another "timeout" active object, but not yet implemented
		
		//Expected Result:	Every view must send an event (5 events in total). 
		//Sort order for all the views except for RemoteForward view must be reverse. 
		//New entry must be inserted in every view.
		test.Printf(_L("Local View:\n"));
		CheckViewOrderL(iLocalView);
		test.Printf(_L("\n\n"));

		test.Printf(_L("Remote View Reverse:\n"));
		CheckViewOrderL(iRemoteViewReverse);
		test.Printf(_L("\n\n"));

		test.Printf(_L("Remote View Forward:\n"));
		CheckViewOrderL(iRemoteViewForward, EFalse, EFalse); //do not Print, forward order
		test.Printf(_L("\n\n"));

		test.Printf(_L("Local Find View:\n"));
		CheckViewOrderL(iLocalFindView);
		test.Printf(_L("\n\n"));

		test.Printf(_L("Remote Find View:\n"));
		CheckViewOrderL(iRemoteFindView);
		test.Printf(_L("\n\n"));
		
		//Searching for Mansell in the DB.
		TContactItemId manselId = SearchInViewL(iLocalFindView, KMansell);
		TESTTRUEL(manselId != 0); //Have we found it?

		//Actions:	Change a newly added contact from "Nigel Mansell" to "Ayrton Senna". 
		CContactItem* mansContact = iDb->OpenContactLX(manselId);
		PUSH(mansContact);
		  CContactItemFieldSet& fieldSet      = mansContact->CardFields();
		  
		  CContactItemField&    givenNameField = fieldSet[fieldSet.Find(KUidContactFieldGivenName)];
		  givenNameField.TextStorage()->SetTextL(_L("Ayrton"));

		  CContactItemField&    familyNameField = fieldSet[fieldSet.Find(KUidContactFieldFamilyName)];
		  familyNameField.TextStorage()->SetTextL(_L("Senna"));

		  CContactItemField&    ordinalNumField = fieldSet[fieldSet.Find(KUidContactFieldAdditionalName)];
		  ordinalNumField.TextStorage()->SetTextL(_L("20")); //Number is used for sorting checking

		iDb->CommitContactL(*mansContact);
		POPD(mansContact);
		CleanupStack::PopAndDestroy(); //record lock

	WAIT_EVENTS(7,10) //Waiting for 10 events to arrive (delele and insert event from each view)
		//Actions:	Check the sorting in every view.
	
		//Expected Result:	10 events should arrive - 5 deletes and 5 inserts. 
		//Sort order for all the views except for RemoteForward view must be reverse. 
		//The entry must be updated in local and remote views and should disappear 
		//from Find views because it doesn't contain substrings "Ma" or "el" anymore.
		test.Printf(_L("Local View:\n"));
		CheckViewOrderL(iLocalView, ETrue);
		test.Printf(_L("\n\n"));

		test.Printf(_L("Remote View Reverse:\n"));
		CheckViewOrderL(iRemoteViewReverse, ETrue);
		test.Printf(_L("\n\n"));

		test.Printf(_L("Remote View Forward:\n"));
		CheckViewOrderL(iRemoteViewForward, EFalse, EFalse); //do not Print, forward order
		test.Printf(_L("\n\n"));
		
		test.Printf(_L("Local Find View:\n"));
		CheckViewOrderL(iLocalFindView, ETrue);
		test.Printf(_L("\n\n"));

		test.Printf(_L("Remote Find View:\n"));
		CheckViewOrderL(iRemoteFindView, ETrue);
		test.Printf(_L("\n\n"));

		//We changed Mansell to Senna - the entry shouldn't appear in either find views
		TContactItemId manselId = SearchInViewL(iLocalFindView, KMansell);
		TESTTRUE(manselId == 0);
		manselId = SearchInViewL(iRemoteFindView, KMansell);
		TESTTRUE(manselId == 0);

	CloseView(iLocalFindView);
	CloseView(iLocalView);
	CloseView(iRemoteFindView);
	CloseView(iRemoteViewReverse);
	CloseView(iRemoteViewForward);

	iFindDesArray->Reset();
	delete iFindDesArray; iFindDesArray = NULL;

	END_ASYNC_TEST
	}


void CTestMachine::StepNamedRemoteViewsL()
	{
	const TContactViewPreferences viewPrefs = TContactViewPreferences(EUnSortedAtEnd | EContactsOnly);
	_LIT(KNamedRemoteView, "NamedRemoteView");

	RContactViewSortOrder theNameSurnSortOrder;
	theNameSurnSortOrder.AppendL(KUidContactFieldGivenName);
	theNameSurnSortOrder.AppendL(KUidContactFieldFamilyName);
	theNameSurnSortOrder.AppendL(KUidContactFieldPrefixName); //Contains order numbers for given name

	BEGIN_ASYNC_TEST
		//Actions:	Create named remote view a reverse sorting plugin. Check sorting order.
		iNamedRemoteView = CContactNamedRemoteView::NewL(*this, KNamedRemoteView, *iDb,iSurnNameSortOrder,viewPrefs, KReverseSortPlugin);

	WAIT_EVENT(1)
		//Expected Result:	Named remote view must be created - sorting order must be reverse.
		test.Printf(_L("Named Remote View:\n"));
		CheckViewOrderL(iNamedRemoteView, ETrue);
		test.Printf(_L("\n\n"));

		//Actions:	Create another instance of the named remote view with the same name, 
		//			but without a sorting plugin this time. Check sorting order.
		TESTTRAP(iNamedRemoteView2 = CContactNamedRemoteView::NewL(*this, KNamedRemoteView, *iDb, iSurnNameSortOrder,viewPrefs));
	WAIT_EVENT(2)

		//Expected Result:	When we create another named remote view with the name of existing view 
		// - another copy of the view will not be created, instead will receive the handle 
		//to existing view. That is why sort order should be as first time - reverse.
		test.Printf(_L("Another Copy of Named Remote View:\n"));
		CheckViewOrderL(iNamedRemoteView2, ETrue);
		test.Printf(_L("\n\n"));

		//Actions:	Change the sort order of the named remote view to sort on 
		//	Name and then Surname of the contacts. Check sorting for both handles of the view.
		iNamedRemoteView2->ChangeSortOrderL(theNameSurnSortOrder);
	WAIT_EVENTS(3,4) //2 EUnavailable and 2 EOrderChanged

		//Expected Result:	Both instances of the view must be sorted in reverse order on 
		//Names and Surnames of the contacts.
		test.Printf(_L("First Copy of Named Remote View:\n"));
		CheckViewOrderL(iNamedRemoteView, ETrue);
		test.Printf(_L("\n\n"));

		test.Printf(_L("Another Copy of Named Remote View:\n"));
		CheckViewOrderL(iNamedRemoteView2, ETrue);
		test.Printf(_L("\n\n"));

		/////////////////////////////////////////
		//Actions:	Create a Find view on top of the first instance of named remote view, 
		//using "Ma" search string 
		iFindDesArray =new(ELeave)CPtrC16Array(1);
		_LIT(KMa,"Ma");
		iFindDesArray->AppendL(TPtrC(KMa));

		iNamedRemoteFindView= CContactFindView::NewL(*iDb,*iNamedRemoteView,*this,iFindDesArray);
	WAIT_EVENT(4)

		//Expected Result:	Check the order of entries in the find view - it must be reverse,
		// sorting on Names and Surnames
		test.Printf(_L("NamedRemoteFind View created:\n"));
		CheckViewOrderL(iNamedRemoteFindView, ETrue);
		test.Printf(_L("\n\n"));

		/////////////////////////////////////////
		//Actions:	Create a Find view on top of the second instance of named remote view, 
		// using "el" search string 
		iFindDesArray->Reset();
		_LIT(Kel,"el");
		iFindDesArray->AppendL(TPtrC(Kel));
		iNamedRemoteFindView2 = CContactFindView::NewL(*iDb,*iNamedRemoteView2,*this,iFindDesArray);
	WAIT_EVENT(5)

		//Expected Result:	Check the order of entries in the find view - it must be reverse, 
		//sorting on Names and Surnames
		test.Printf(_L("NamedRemoteFind View 2 created:\n"));
		CheckViewOrderL(iNamedRemoteFindView2, ETrue);
		test.Printf(_L("\n\n"));
		
		CloseView(iNamedRemoteFindView);
		CloseView(iNamedRemoteFindView2);
		CloseView(iNamedRemoteView);
		CloseView(iNamedRemoteView2);

		iFindDesArray->Reset();
		delete iFindDesArray; iFindDesArray = NULL;

	END_ASYNC_TEST

	theNameSurnSortOrder.Close();
	}

/** Searches the view for a contact with particular surname */
TContactItemId CTestMachine::SearchInViewL(CContactViewBase* aView, const TDesC& aSurname)
	{		
	//Searching for aSurname in the DB.
	for (TInt i=aView->CountL() - 1; i >= 0; --i)
		{
		const CViewContact& theContact = aView->ContactAtL(i);
		const TPtrC& surn = theContact.Field(0);
		if (surn == aSurname)
			return theContact.Id();
		}
	return NULL;
	}


void CTestMachine::CheckViewOrderL(const CContactViewBase* aView, TBool aPrint, TBool aReverseOrder)
	{
	ASSERT(aView);
	const TInt numOfCont = aView->CountL();
	TInt prevOrder = aReverseOrder ? 99 : -1; //used for first comparison
	//99 - More than items in database, -1 less than an index of first item

	for (TInt i=0; i < numOfCont; ++i)
		{
		const CViewContact& theContact = aView->ContactAtL(i);
		const TPtrC& theNum = theContact.Field(2);
		
		const TPtrC& theFamilyName = theContact.Field(0);
		if (aPrint)
			{ //Print only if we asked to
			const TPtrC& theGivenName = theContact.Field(1);
			test.Printf(_L("%S %S, %S\n"), &theNum, &theFamilyName, &theGivenName);
			}

		//Order checking 
		//The number of contact in alphabetically sorted list is given in MiddleName field
		TLex16 strOrder(theNum);
		TInt   currOrder; 
		TESTNOERRL(strOrder.Val(currOrder)) //Convert string to integer

		if (aReverseOrder)
			TESTTRUE(prevOrder > currOrder) //Descending order for reverse sorting
		else
			TESTTRUE(prevOrder < currOrder) //Ascending order for forward sorting

		prevOrder = currOrder;
		}
	}


void CTestMachine::StepOOML()
	{
	//TInt allocCells(0);

	BEGIN_ASYNC_TEST
		if (iOOMCounter % 100 == 0)
			test.Printf(_L("Step: %d...\n"), iOOMCounter);
		++iOOMCounter;

		//__SRVHEAP_SETFAIL(iOOMCounter);
		//allocCells = iAgnServ->_DebugRequestAllocatedCellsL();				//__UHEAP_MARK;
		
		int ret=0;
		//TRAP(ret, DoSomething); 
		
		if (ret == KErrNoMemory)
			{
			//iAgnServ->_DebugSetHeapFail(RHeap::ENone,1);					  //__UHEAP_RESET; 
			//TESTVALUEL(iAgnServ->_DebugRequestAllocatedCellsL(), allocCells); //__UHEAP_MARKEND; 

			--iSubStep; //Stay in the same step
			//Completed(KErrNone);
			}
		else if (ret != KErrNone)
			{
			//__SRVHEAP_RESET;
			TESTNOERRL(ret); //Bad leaving code
			}
		
		iError = NULL;

	WAIT_EVENT(1);
		if (iError == KErrNoMemory)
			{//__UHEAP_MARKEND; //Cannot test, because agenda stores Category in long-living object
			iSubStep -=2; //One steps back
			//__SRVHEAP_RESET;
			}
		else if (iError != KErrNone)
			{
			TESTNOERRL(iError); //Bad leaving code
			}
		
		__UHEAP_RESET;
		//Completed(KErrNone);

	WAIT_EVENT(2);

		test.Printf(_L("OOM test finished successfully on step: %d...\n"), iOOMCounter);

	END_ASYNC_TEST
	}


void CTestMachine::StepDone()
	{
	test.Printf(_L("Shutting down\n"));
	User::After(1000000); // Wait for any server side processes to release the file
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestFileBase));
    TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KTestFile));
	CActiveScheduler::Stop();
	}


void CTestMachine::HandleContactViewEvent(const CContactViewBase& aView,
										  const TContactViewEvent& aEvent)
	{
	//Indentifying the view
	TPtrC viewName(KNullDesC);
	_LIT(KLocalView, "LocalView");
	_LIT(KRemoteViewReverse, "RemoteViewReverse");
	_LIT(KRemoteViewForward, "RemoteViewForward");
	_LIT(KNamedRemoteView, "NamedRemoteView");
	_LIT(KNamedRemoteView2, "NamedRemoteView2");
	_LIT(KLocalFindView, "LocalFindView");
	_LIT(KRemoteFindView, "RemoteFindView");
	_LIT(KNamedRemoteFindView, "NamedRemoteFindView");
	_LIT(KNamedRemoteFindView2, "NamedRemoteFindView2");

	if (&aView == iLocalView)
		viewName.Set(KLocalView);
	else if (&aView == iRemoteViewReverse)
		viewName.Set(KRemoteViewReverse);
	else if (&aView == iRemoteViewForward)
		viewName.Set(KRemoteViewForward);
	else if (&aView == iLocalFindView)
		viewName.Set(KLocalFindView);
	else if (&aView == iRemoteFindView)
		viewName.Set(KRemoteFindView);
	else if (&aView == iNamedRemoteView)
		viewName.Set(KNamedRemoteView);
	else if (&aView == iNamedRemoteView2)
		viewName.Set(KNamedRemoteView2);
	else if (&aView == iNamedRemoteFindView)
		viewName.Set(KNamedRemoteFindView);
	else if (&aView == iNamedRemoteFindView2)
		viewName.Set(KNamedRemoteFindView2);


	TPtrC eventName(KNullDesC);
	_LIT(KEventUnavailable, "Unavailable");
	_LIT(KEventReady, "Ready");
	_LIT(KEventSortOrderChanged, "Sort Order Changed");
	_LIT(KEventSortError, "Sort Error");
	_LIT(KEventServerError, "Server Error");
	_LIT(KEventIndexingError, "Indexing Error");
	_LIT(KEventItemAdded, "Item Added");
	_LIT(KEventItemRemoved, "Item Removed");
	_LIT(KEventGroupChanged, "Group Changed");
	_LIT(KEventUnknown, "unknown ??");

	
	switch (aEvent.iEventType)
		{
	case TContactViewEvent::EUnavailable:
		eventName.Set(KEventUnavailable);
		break;
	case TContactViewEvent::EReady:
		eventName.Set(KEventReady);
		break;
	case TContactViewEvent::ESortOrderChanged:
		eventName.Set(KEventSortOrderChanged);
		break;
	case TContactViewEvent::ESortError:
		eventName.Set(KEventSortError);
		break;
	case TContactViewEvent::EServerError:
		eventName.Set(KEventServerError);
		break;
	case TContactViewEvent::EIndexingError:
		eventName.Set(KEventIndexingError);
		break;
	case TContactViewEvent::EItemAdded:
		eventName.Set(KEventItemAdded);
		break;
	case TContactViewEvent::EItemRemoved:
		eventName.Set(KEventItemRemoved);
		break;
	case TContactViewEvent::EGroupChanged:
		eventName.Set(KEventGroupChanged);
		break;
	default:
		eventName.Set(KEventUnknown);
		break;
		}

	test.Printf(_L("=> %S has sent an event: %S\n"), &viewName, &eventName);

//	iError = aError;

	
	if (--iNumOfEvents == 0) 
		{ //We have received all planned events
		++iSubStep;

		TRequestStatus *pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		SetActive();
		}
	else if (iNumOfEvents < 0)
		{
		TESTTRUE(EFalse);  //Too many events
		}
	}


CTestMachine* CTestMachine::NewLC()
	{
	CTestMachine* self = new (ELeave) CTestMachine;
	PUSH(self);
	self->ConstructL();
	CActiveScheduler::Add(self);
	self->NextStep();
	return self;
	}

void CTestMachine::ConstructL()
	{
	CTimer::ConstructL();

	TESTTRAP(iDb=CContactDatabase::ReplaceL(KTestFileBase));

	RFileReadStream vcard;
	CleanupClosePushL(vcard);
	TESTNOERR(vcard.Open(theFs, _L("z:\\cntvcard\\T_ViewSortPlugin.vcf"), EFileRead));

	TBool success=EFalse;
	CArrayPtr<CContactItem>* contactItems=iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, NULL);
	CleanupStack::PopAndDestroy(); //read stream vcard

	TESTTRUEL(success);

	test.Printf(_L("Sample Database with %d entries created\n"), contactItems->Count());

	contactItems->ResetAndDestroy();
	delete contactItems;

	//Prepare sort order for the views
	iSurnNameSortOrder.AppendL(KUidContactFieldFamilyName);
	iSurnNameSortOrder.AppendL(KUidContactFieldGivenName);
	iSurnNameSortOrder.AppendL(KUidContactFieldAdditionalName); //Contains order number for family name
	}

CTestMachine::~CTestMachine()
	{
	iSurnNameSortOrder.Close();
	//iRemoteSortOrder.Close();

	delete iDb;
	}

void CTestMachine::CloseView(CContactViewBase* aView)
	{
	if (aView)
		aView->Close(*this);
	aView = NULL;
	}


void CTestMachine::NextStep()
	{
	iError = KErrNone; //reset callback error variable
	++iStep;
	iSubStep = 0;

	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

LOCAL_C void doMainL()
	{
	TESTNOERR(theFs.Connect());
	CleanupClosePushL(theFs);
	CTestMachine* tm = CTestMachine::NewLC();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(tm);
	CleanupStack::PopAndDestroy(); // Close FS
	}


/**

@SYMTestCaseID     PIM-T-VIEWSORTPLUGIN-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("@SYMTESTCaseID:PIM-T-VIEWSORTPLUGIN-0001 T_ViewSortPlugin"));

	CActiveScheduler* scheduler=new CActiveScheduler;
	test(scheduler != NULL);
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());	
	test(ret==KErrNone);
	test(__NB_failures == 0); //There were failures before.
	delete theCleanup;	
	delete scheduler;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}




///// Helpers //////
void RandomText(TDes& aText,TInt64& aSeed)
{
	TInt numChar=Math::Rand(aSeed)%aText.MaxLength();
	if(Math::Rand(aSeed)%2)
		{
		for (TInt i=0;i<numChar;++i)
			{
			TInt c=Math::Rand(aSeed)%6;
			c= (c==0)? ' ' : ((Math::Rand(aSeed)%26)+    ((c==2)? 'A' : 'a'));
			aText.Append(c);
			}
		}
	else
		aText=KNullDesC;
}


//Export routine
//static void ExportContactAsVCardL(CContactDatabase& aDatabase, TContactItemId aId, RFs& aFsSession, const TDesC& aVCardFileName)
/*	CContactIdArray* idArray = CContactIdArray::NewLC();
	idArray->AddL(id);
	//
	RFile outfile;
	User::LeaveIfError(outfile.Replace(theFs, theFamilyName, EFileWrite));
	CleanupClosePushL(outfile);
	//
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	//	
	const TUid KVersitVCardUid = { KUidVCardConvDefaultImpl };
	iDb->ExportSelectedContactsL(KVersitVCardUid, *idArray, writeStream, CContactDatabase::EDefault);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(3, idArray); // writeStream, outfile, idArray*/

