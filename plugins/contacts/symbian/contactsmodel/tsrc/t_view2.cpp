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
#include <collate.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_view2.h"
#include "t_utils.h"
#include <badesca.h>

//#define VERBOSE_LOGGING

// Test Macro
LOCAL_D RTest test(_L("t_view2"));

//
// Constants.
//

_LIT(KTestName,"t_view2");
_LIT(KLogFileName,"t_view2.log");

_LIT(KDbFileName,"c:t_view2.cdb");

_LIT(KLocalViewName,"LocalView");
_LIT(KRemoteViewName,"RemoteView");
_LIT(KTextDefSeparator,"");
_LIT8(KUnicodeLineEnd,"\x0D\x00\x0a\x00");
_LIT(KWhiteSpaces, " ");
const TInt KNumberOfUnsortableContacts=10;

// test files
_LIT(KContactsFile1, "z:\\t_view2\\t_view2_initial_contacts.txt");
_LIT(KContactsFile2, "z:\\t_view2\\t_view2_more_contacts_1.txt");
_LIT(KContactsFile3, "z:\\t_view2\\t_view2_more_contacts_2.txt");

LOCAL_C void CleanupContactArray(TAny* aArray)	
	{
	RPointerArray<CViewContact>* array=REINTERPRET_CAST(RPointerArray<CViewContact>*,aArray);
	if (array)
		array->ResetAndDestroy();
	array->Close();
	}

//
// CTestConductor.
//

CTestConductor* CTestConductor::NewL()
	{
	CTestConductor* self=new(ELeave) CTestConductor();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTestConductor::~CTestConductor()
	{
	iLocalView->Close(*this);
	iNamedLocalView->Close(*this);
	iRemoteView_1->Close(*this);
	iRemoteView_2->Close(*this);
	iNamedRemoteView_1->Close(*this);
	iNamedRemoteView_2->Close(*this);
	iLocalViewWithViewPreferences->Close(*this);
	iSubViewLessThanA->Close(*this);
	iSubViewAbc->Close(*this);
	iSubViewDef->Close(*this);
	iSubViewGhi->Close(*this);
	iSubViewJkl->Close(*this);
	iSubViewGreaterThanOrEqualToM->Close(*this);
	iConcatenatedView->Close(*this);
	iSortOrder_1.Close();
	iSortOrder_2.Close();
	iSortOrder_3.Close();
	iSortOrder_4.Close();
	iLocalFindView->Close(*this);
	iFreshLocalView->Close(*this);

	iRemoteFindView->Close(*this);

	if(iFindDesArray)
		{
		iFindDesArray->Reset();
		delete iFindDesArray;
		}

	iFindDesArray2->Reset();
	delete iFindDesArray2;

	delete iUnsortableAdder;
	delete iRandomGenerator;
	delete iSortChecker;
	delete iViewCreator;
	delete iContactAdder;
	delete iContactRemover;
	delete iViewExerciser;
	delete iSortOrderChanger;
	delete iOwnCardRemover;
	delete iOwnCardModifier;
	delete iLog;
	delete iDb;
	iTestIds.Reset();
	iTestIds.Close();

	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDbFileName));
	iFs.Delete(KLogFileName());
	iFs.RmDir(KLogFileName());
	iFs.Close();
	}

CTestConductor::CTestConductor() : CActive(EPriorityIdle),iCurrentTestNumber(-1)
	{
	CActiveScheduler::Add(this);
	}

void CTestConductor::ConstructL()
	{
	iSortOrder_1.AppendL(KUidContactFieldGivenName);
	iSortOrder_1.AppendL(KUidContactFieldFamilyName);
	iSortOrder_1.AppendL(KUidContactFieldCompanyName);

	iSortOrder_2.AppendL(KUidContactFieldFamilyName);
	iSortOrder_2.AppendL(KUidContactFieldGivenName);
	iSortOrder_2.AppendL(KUidContactFieldCompanyName);

	iSortOrder_3.AppendL(KUidContactFieldGivenName);
	iSortOrder_3.AppendL(KUidContactFieldFamilyName);
	iSortOrder_3.AppendL(KUidContactFieldEMail);
	iSortOrder_3.AppendL(KUidContactFieldCompanyName);
	iSortOrder_3.AppendL(KUidContactFieldPhoneNumber);
	
	iSortOrder_4.AppendL(KUidContactFieldGivenName);	
	
	User::LeaveIfError(iFs.Connect());
	iFs.MkDirAll(KLogFileName());
	iLog=CLog::NewL(test,KLogFileName);
	iDb=CContactDatabase::ReplaceL(KDbFileName);

	iSortChecker=CSortChecker::NewL(*this);
	iViewCreator=CViewCreator::NewL(*this);
	iContactAdder=CContactAdder::NewL(*this);
	iContactRemover=CContactRemover::NewL(*this);
	iViewExerciser=CViewExerciser::NewL(*this);
	iSortOrderChanger=CSortOrderChanger::NewL(*this);
	iOwnCardRemover = new(ELeave) CContactOwnCardRemover(*this);
	iOwnCardModifier = new(ELeave) CContactOwnCardModifier(*this);

	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);


	NextTest();
	CActiveScheduler::Start();
	// error from active object?
	User::LeaveIfError(iTestError);
	test.Printf(_L("\n"));
	}

void CTestConductor::NextTest()
	{
	++iCurrentTestNumber;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CTestConductor::RunL()
	{
	switch (iCurrentTestNumber)
		{
		case ECreateLocalView:
			iLog->LogLine(_L("=== Create local view"));
			iCurrentTestObject=iViewCreator;
			iLocalView=iViewCreator->CreateLocalViewL(iSortOrder_1);
			break;
		case EAddInitialContacts:
			iLog->LogLine(_L("=== Add initial contacts"));
			iCurrentTestObject=iContactAdder;
			iContactAdder->AddContactsL(KContactsFile1,1);
			break;
		case EExerciseLocalView_1:
			iLog->LogLine(_L("=== Exercise local view (1)"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iLocalView);
			iLog->LogLine(_L("=== Profiling local view Contact At()(1)"));
			iViewExerciser->ProfileViewL(*iLocalView);
			break;
		case ECreateLocalNamedView:
			iLog->LogLine(_L("=== Create named local view"));
			iCurrentTestObject=iViewCreator;
			iNamedLocalView=iViewCreator->CreateNamedLocalViewL(KLocalViewName,iSortOrder_1);
			break;
		case EExerciseLocalNamedView_1:
			iLog->LogLine(_L("=== Exercise named local view (1)"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iNamedLocalView);
			break;
		case EAddMoreContacts_1:
			iLog->LogLine(_L("=== Add more contacts (1)"));
			iCurrentTestObject=iContactAdder;
			iContactAdder->AddContactsL(KContactsFile2,2);
			break;
		case EExerciseLocalView_2:
			iLog->LogLine(_L("=== Exercise local view (2)"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iLocalView);
			break;
		case EExerciseLocalNamedView_2:
			iLog->LogLine(_L("=== Exercise named local view (2)"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iNamedLocalView);
			break;
		case ECreateRemoteView1:
			iLog->LogLine(_L("=== Create remote view"));
			iCurrentTestObject=iViewCreator;
			iRemoteView_1=iViewCreator->CreateRemoteViewL(iSortOrder_1);
			break;
		case EExerciseRemoteView1:
			{
			iLog->LogLine(_L("=== Exercise remote view"));
			iCurrentTestObject=iViewExerciser;

			iFindDesArray =new(ELeave)CPtrC16Array(1);
			iFindDesArray->AppendL(_L("Administrator"));
			iLocalFindView= CContactFindView::NewL(*iDb,*iLocalView,*this,iFindDesArray);
			iRemoteFindView = CContactFindView::NewL(*iDb,*iRemoteView_1,*this,iFindDesArray);
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iRemoteView_1);
			iLog->LogLine(_L("=== Profiling Remote view Contact At()"));
			iViewExerciser->ProfileViewL(*iRemoteView_1);
			}
			break;
		case ECreateRemoteView2:
			iLog->LogLine(_L("=== Create remote view - view of same sort already exists"));
			iCurrentTestObject=iViewCreator;
			iRemoteView_2=iViewCreator->CreateRemoteViewL(iSortOrder_1);
			break;
		case EExerciseRemoteView2:
			{
			iLog->LogLine(_L("=== Exercise remote view - view of same sort already exists"));
			iFindDesArray2 =new(ELeave)CPtrC16Array(2);
			iFindDesArray2->AppendL(_L("Administrator"));
			iFindDesArray2->AppendL(_L("Notes"));
			iFindDesArray->Reset();
			delete iFindDesArray;
			iLocalFindView->RefineFindViewL(iFindDesArray2);
			iRemoteFindView->RefineFindViewL(iFindDesArray2);
			iFindDesArray =new(ELeave)CPtrC16Array(1);
			iFindDesArray->AppendL(_L("Administrator"));
			iLocalFindView->RefineFindViewL(iFindDesArray);
			iRemoteFindView->RefineFindViewL(iFindDesArray);
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iRemoteView_2);
			}
			break;
		case ECreateNamedRemoteView1:
			iLog->LogLine(_L("=== Create named remote view"));
			iCurrentTestObject=iViewCreator;
			iNamedRemoteView_1=iViewCreator->CreateNamedRemoteViewL(KRemoteViewName,iSortOrder_1);
			break;
		case EExerciseNamedRemoteView1:
			iLog->LogLine(_L("=== Exercise named remote view"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iNamedRemoteView_1);
			break;
		case ECreateNamedRemoteView2:
			iLog->LogLine(_L("=== Create named remote view - view of same name already exists"));
			iCurrentTestObject=iViewCreator;
			iNamedRemoteView_2=iViewCreator->CreateNamedRemoteViewL(KRemoteViewName,iSortOrder_1);
			break;
		case EExerciseNamedRemoteView2:
			iLog->LogLine(_L("=== Exercise named remote view - view of same name already exists"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iNamedRemoteView_2);
			break;
		case ECreateSubViewLessThanA:
			iLog->LogLine(_L("=== Create \"less than A\" subview"));
			iCurrentTestObject=iViewCreator;
			iSubViewLessThanA=iViewCreator->CreateSubViewL(*iNamedRemoteView_1,_L("<a"));
			break;
		case EExerciseSubViewLessThanA:
			iLog->LogLine(_L("=== Exercise \"less than A\" subview"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseLowSubViewL(CSortChecker::EFirstNameLastName,*iSubViewLessThanA,_L("a"));
			break;
		case ECreateSubViewAbc:
			iLog->LogLine(_L("=== Create \"ABC\" subview"));
			iCurrentTestObject=iViewCreator;
			iSubViewAbc=iViewCreator->CreateSubViewL(*iNamedRemoteView_1,_L(">=a"),_L("<d"));
			break;
		case EExerciseSubViewAbc:
			iLog->LogLine(_L("=== Exercise \"ABC\" subview"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseSubViewL(CSortChecker::EFirstNameLastName,*iSubViewAbc,_L("a"),_L("d"));
			break;
		case ECreateSubViewDef:
			iLog->LogLine(_L("=== Create \"DEF\" subview"));
			iCurrentTestObject=iViewCreator;
			iSubViewDef=iViewCreator->CreateSubViewL(*iNamedRemoteView_1,_L(">=d"),_L("<g"));
			break;
		case EExerciseSubViewDef:
			iLog->LogLine(_L("=== Exercise \"DEF\" subview"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseSubViewL(CSortChecker::EFirstNameLastName,*iSubViewDef,_L("d"),_L("g"));
			break;
		case ECreateSubViewGhi:
			iLog->LogLine(_L("=== Create \"GHI\" subview"));
			iCurrentTestObject=iViewCreator;
			iSubViewGhi=iViewCreator->CreateSubViewL(*iNamedRemoteView_1,_L(">=g"),_L("<j"));
			break;
		case EExerciseSubViewGhi:
			iLog->LogLine(_L("=== Exercise \"GHI\" subview"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseSubViewL(CSortChecker::EFirstNameLastName,*iSubViewGhi,_L("g"),_L("j"));
			break;
		case ECreateSubViewJkl:
			iLog->LogLine(_L("=== Create \"JKL\" subview"));
			iCurrentTestObject=iViewCreator;
			iSubViewJkl=iViewCreator->CreateSubViewL(*iNamedRemoteView_1,_L(">=j"),_L("<m"));
			break;
		case EExerciseSubViewJkl:
			iLog->LogLine(_L("=== Exercise \"JKL\" subview"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseSubViewL(CSortChecker::EFirstNameLastName,*iSubViewJkl,_L("j"),_L("m"));
			break;
		case ECreateSubViewGreaterThanOrEqualToM:
			iLog->LogLine(_L("=== Create \"greater than M\" subview"));
			iCurrentTestObject=iViewCreator;
			iSubViewGreaterThanOrEqualToM=iViewCreator->CreateSubViewL(*iNamedRemoteView_1,_L(">=m"));
			break;
		case EExerciseSubViewGreaterThanOrEqualToM:
			iLog->LogLine(_L("=== Exercise \"greater than M\" subview"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseHighSubViewL(CSortChecker::EFirstNameLastName,*iSubViewGreaterThanOrEqualToM,_L("m"));
			break;
		case ECreateConcatenatedView:
			iLog->LogLine(_L("=== Create concatenated view of all the subviews"));
			{
			RPointerArray<CContactViewBase> views;
			User::LeaveIfError(views.Append(iSubViewLessThanA));
			User::LeaveIfError(views.Append(iSubViewAbc));
			User::LeaveIfError(views.Append(iSubViewDef));
			User::LeaveIfError(views.Append(iSubViewGhi));
			User::LeaveIfError(views.Append(iSubViewJkl));
			User::LeaveIfError(views.Append(iSubViewGreaterThanOrEqualToM));
			iCurrentTestObject=iViewCreator;
			iConcatenatedView=iViewCreator->CreateConcatenatedViewL(views);
			views.Close();
			}
			break;
		case EExerciseConcatenatedView:
			iLog->LogLine(_L("=== Exercise concatenated view of all the subviews"));
			iCurrentTestObject=iViewExerciser;
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iConcatenatedView);
			break;
		case EChangeSortOrder1:
			iLog->LogLine(_L("=== Change sort order (1) of underlying named remoted view"));
			iCurrentTestObject=iSortOrderChanger;
			iSortOrderChanger->ChangeSortOrderL(iSortOrder_3,*iNamedRemoteView_1,14);
			break;
		case EChangeSortOrder2:
			iLog->LogLine(_L("=== Change sort order (2) of underlying named remoted view"));
			iCurrentTestObject=iSortOrderChanger;
			iSortOrderChanger->ChangeSortOrderL(iSortOrder_4,*iNamedRemoteView_1,14);
			break;
		case EChangeSortOrder3:
			iLog->LogLine(_L("=== Change sort order (3) of underlying named remoted view"));
			iCurrentTestObject=iSortOrderChanger;
			iSortOrderChanger->ChangeSortOrderL(iSortOrder_2,*iNamedRemoteView_1,14);
			break;
		case EExerciseAllAffectedViews:
			iLog->LogLine(_L("=== Exercise all views affected by sort order change"));
			iViewExerciser->QueueNextTestWhenComplete(EFalse);
			iLog->LogLine(_L("==== Exercise named remote view"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iNamedRemoteView_1);
			iLog->LogLine(_L("==== Exercise named remote view - view of same name already exists"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iNamedRemoteView_2);
			iLog->LogLine(_L("==== Exercise \"less than A\" subview"));
			iViewExerciser->ExerciseLowSubViewL(CSortChecker::ELastNameFirstName,*iSubViewLessThanA,_L("a"));
			iLog->LogLine(_L("==== Exercise \"ABC\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewAbc,_L("a"),_L("d"));
			iLog->LogLine(_L("==== Exercise \"DEF\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewDef,_L("d"),_L("g"));
			iLog->LogLine(_L("==== Exercise \"GHI\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewGhi,_L("g"),_L("j"));
			iLog->LogLine(_L("==== Exercise \"JKL\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewJkl,_L("j"),_L("m"));
			iLog->LogLine(_L("==== Exercise \"greater than M\" subview"));
			iViewExerciser->ExerciseHighSubViewL(CSortChecker::ELastNameFirstName,*iSubViewGreaterThanOrEqualToM,_L("m"));
			iLog->LogLine(_L("==== Exercise concatenated view of all the subviews"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iConcatenatedView);
			NextTest();
			break;
		case EAddMoreContacts_2:
			iLog->LogLine(_L("=== Add more contacts (2)"));
			iCurrentTestObject=iContactAdder;
			iContactAdder->AddContactsL(KContactsFile3,18);
			break;
		case EExerciseAllViews_1:
			iLog->LogLine(_L("=== Exercise all views (1)"));
			iLog->LogLine(_L("==== Exercise local view (2)"));
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iLocalView);
			iLog->LogLine(_L("==== Exercise named local view (2)"));
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iNamedLocalView);
			iLog->LogLine(_L("==== Exercise remote view"));
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iRemoteView_1);
			iLog->LogLine(_L("==== Exercise remote view - view of same sort already exists"));
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iRemoteView_2);
			iLog->LogLine(_L("==== Exercise named remote view"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iNamedRemoteView_1);
			iLog->LogLine(_L("==== Exercise named remote view - view of same name already exists"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iNamedRemoteView_2);
			iLog->LogLine(_L("==== Exercise \"less than A\" subview"));
			iViewExerciser->ExerciseLowSubViewL(CSortChecker::ELastNameFirstName,*iSubViewLessThanA,_L("a"));
			iLog->LogLine(_L("==== Exercise \"ABC\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewAbc,_L("a"),_L("d"));
			iLog->LogLine(_L("==== Exercise \"DEF\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewDef,_L("d"),_L("g"));
			iLog->LogLine(_L("==== Exercise \"GHI\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewGhi,_L("g"),_L("j"));
			iLog->LogLine(_L("==== Exercise \"JKL\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewJkl,_L("j"),_L("m"));
			iLog->LogLine(_L("==== Exercise \"greater than M\" subview"));
			iViewExerciser->ExerciseHighSubViewL(CSortChecker::ELastNameFirstName,*iSubViewGreaterThanOrEqualToM,_L("m"));
			iLog->LogLine(_L("==== Exercise concatenated view of all the subviews"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iConcatenatedView);
			NextTest();
			break;
		case ERemoveSomeContacts:
			iLog->LogLine(_L("=== Remove some contacts"));
			iCurrentTestObject=iContactRemover;
			//iContactRemover->RemoveContactsL(KContactsFile2,18);
			NextTest();
			break;
		case EExerciseAllViews_2:
			iLog->LogLine(_L("=== Exercise all views (2)"));
			iLog->LogLine(_L("==== Exercise local view (3)"));
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iLocalView);
			iLog->LogLine(_L("==== Exercise named local view (3)"));
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iNamedLocalView);
			iLog->LogLine(_L("==== Exercise remote view"));
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iRemoteView_1);
			iLog->LogLine(_L("==== Exercise remote view - view of same sort already exists"));
			iViewExerciser->ExerciseViewL(CSortChecker::EFirstNameLastName,*iRemoteView_2);
			iLog->LogLine(_L("==== Exercise named remote view"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iNamedRemoteView_1);
			iLog->LogLine(_L("==== Exercise named remote view - view of same name already exists"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iNamedRemoteView_2);
			iLog->LogLine(_L("==== Exercise \"less than A\" subview"));
			iViewExerciser->ExerciseLowSubViewL(CSortChecker::ELastNameFirstName,*iSubViewLessThanA,_L("a"));
			iLog->LogLine(_L("==== Exercise \"ABC\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewAbc,_L("a"),_L("d"));
			iLog->LogLine(_L("==== Exercise \"DEF\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewDef,_L("d"),_L("g"));
			iLog->LogLine(_L("==== Exercise \"GHI\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewGhi,_L("g"),_L("j"));
			iLog->LogLine(_L("==== Exercise \"JKL\" subview"));
			iViewExerciser->ExerciseSubViewL(CSortChecker::ELastNameFirstName,*iSubViewJkl,_L("j"),_L("m"));
			iLog->LogLine(_L("==== Exercise \"greater than M\" subview"));
			iViewExerciser->ExerciseHighSubViewL(CSortChecker::ELastNameFirstName,*iSubViewGreaterThanOrEqualToM,_L("m"));
			iLog->LogLine(_L("==== Exercise concatenated view of all the subviews"));
			iViewExerciser->ExerciseViewL(CSortChecker::ELastNameFirstName,*iConcatenatedView);
			NextTest();
			break;
		case EAllViewsOutOfBoundsAccess:
			{
			//Views depend on their underlying views being in a good state, however
			//as some base views are potentially in other processes they must be resistant
			//to out of date views accessesing out of bound members, views, should not
			//panic but should leave with KErrNotFound;
			//local view
			TInt err=0;
			iLog->LogLine(_L("=== Test views for out of bounds access"));
			TInt outCount = iLocalView->CountL();
			TRAP(err,iLocalView->AtL(outCount));
			test(err==KErrNotFound);
			TRAP(err,iLocalView->ContactAtL(outCount));
			test(err==KErrNotFound);
			//Remote View
			outCount = iRemoteView_1->CountL();
			TRAP(err,iRemoteView_1->AtL(outCount));
			test(err==KErrNotFound);
			TRAP(err,iRemoteView_1->ContactAtL(outCount));
			test(err==KErrNotFound);
			//Sub View
			outCount = iSubViewAbc->CountL();
			TRAP(err,iSubViewAbc->AtL(outCount));
			test(err==KErrNotFound);
			TRAP(err,iSubViewAbc->ContactAtL(outCount));
			test(err==KErrNotFound);
			//Sub View
			outCount = iConcatenatedView->CountL();
			TRAP(err,iConcatenatedView->AtL(outCount));
			test(err==KErrNotFound);
			TRAP(err,iConcatenatedView->ContactAtL(outCount));
			test(err==KErrNotFound);	
			//Local Find View View
			outCount = iLocalFindView->CountL();  
			TRAP(err,iLocalFindView->AtL(outCount));
			test(err==KErrNotFound);
			TRAP(err,iLocalFindView->ContactAtL(outCount));
			test(err==KErrNotFound);	
			//Local Find View View
			outCount = iRemoteFindView->CountL();  
			TRAP(err,iRemoteFindView->AtL(outCount));
			test(err==KErrNotFound);
			TRAP(err,iRemoteFindView->ContactAtL(outCount));
			test(err==KErrNotFound);
			NextTest();
			}
			break;
		case ECreateLocalViewWithUnsortableAtEnd:
			{
			iLog->LogLine(_L("=== Create local view with Unsortable Contacts at End"));
			iCurrentTestObject=iViewCreator;
			TInt32 viewPrefs=0;
			viewPrefs|=EUnSortedAtEnd;
			iLocalViewWithViewPreferences=iViewCreator->CreateLocalViewL(iSortOrder_1,TContactViewPreferences(viewPrefs));
			}
			break;
		case EAddUnsortableContacts:
			{
			iLog->LogLine(_L("=== Add Contacts with no sortable fields only phone numbers"));
			iUnsortableAdder = CUnsortableContactAdder::NewL(*this,*iLocalViewWithViewPreferences);
			iCurrentTestObject=iUnsortableAdder;//will recieve notificaitons
			TInt32 viewPrefs=0;
			viewPrefs|=EUnSortedAtEnd;
			iUnsortableAdder->AddContactsL(KNumberOfUnsortableContacts,TContactViewPreferences(viewPrefs));
			}
			break;
		case EExerciseLocalViewWithUnsortableAtEnd:
			{
			iLog->LogLine(_L("=== Exercies Views for view preferences"));
			ExerciseViewPreferencesL(iLocalViewWithViewPreferences,TContactViewPreferences(EUnSortedAtEnd));
			ExerciseViewPreferencesL(iLocalView,TContactViewPreferences(EUnSortedAtBeginning));
			ExerciseViewPreferencesL(iRemoteView_1,TContactViewPreferences(EUnSortedAtBeginning));
			NextTest();
			}
			break;
		case EAddFreshLocalView:
			{
			iLog->LogLine(_L("=== Create new view  with Unsortable Contacts at End"));
			iCurrentTestObject=iViewCreator;
			TInt32 viewPrefs=0;
			viewPrefs|=EUnSortedAtEnd;
			viewPrefs|=ESingleWhiteSpaceIsEmptyField;
			iFreshLocalView=iViewCreator->CreateLocalViewL(iSortOrder_1,TContactViewPreferences(viewPrefs));
			}
			break;
		case EExerciseFreshLocalView:
			{
			iLog->LogLine(_L("=== Exercies new view for view preferences"));
			ExerciseViewPreferencesL(iFreshLocalView,TContactViewPreferences(EUnSortedAtEnd));
			NextTest();
			}
			break;	
		case EAddSomeMoreUnsortableContacts:
			{
			iLog->LogLine(_L("=== Add more Contacts with no sortable fields only phone numbers"));
			iCurrentTestObject=iUnsortableAdder;//will recieve notificaitons
			TInt32 viewPrefs=0;
			viewPrefs|=EUnSortedAtEnd;//doesn't matter if Atbegining either same for adder.
			iUnsortableAdder->AddContactsL(KNumberOfUnsortableContacts,TContactViewPreferences(viewPrefs));
			}
			break;
		case EExerciseViewForPrefsAgain:
			{
			iLog->LogLine(_L("=== Exercies Views for view preferences again after updates"));
			ExerciseViewPreferencesL(iLocalViewWithViewPreferences,TContactViewPreferences(EUnSortedAtEnd));
			ExerciseViewPreferencesL(iLocalView,TContactViewPreferences(EUnSortedAtBeginning));
			ExerciseViewPreferencesL(iRemoteView_1,TContactViewPreferences(EUnSortedAtBeginning));
			ExerciseViewPreferencesL(iFreshLocalView,TContactViewPreferences(EUnSortedAtEnd));
			NextTest();
			}
			break;
		case EAddWhiteSpaceContacts:
			{
			iLog->LogLine(_L("=== Add more Contacts with white space in some sort fields"));
			iCurrentTestObject=iUnsortableAdder;//will recieve notificaitons
			TInt32 viewPrefs=0;
			viewPrefs|=ESingleWhiteSpaceIsEmptyField;//doesn't matter if Atbegining either same for adder.
			iTestIds.Reset();
			iUnsortableAdder->AddContactsL(KNumberOfUnsortableContacts,TContactViewPreferences(viewPrefs));
			}
			break;
		case EExerciseWhiteSpaceContacts:
			{
			//iFreshLocalView
			ExerciseViewPreferencesL(iFreshLocalView,TContactViewPreferences(ESingleWhiteSpaceIsEmptyField));
			NextTest();
			}
			break;
		case ERemoveOwnCardContact:
			{
			iLog->LogLine(_L("=== Remove Own Card contact"));
			iCurrentTestObject=iOwnCardRemover;
			iOwnCardRemover->RemoveOwnCardL(*iFreshLocalView);
			}
			break;
		case (EModifyOwnCardContact):
			{
			NextTest();
			break;
			/*
			iLog->LogLine(_L("=== Modify Own Card contact"));
			iCurrentTestObject=iOwnCardModifier;
			iOwnCardModifier->ModifyOwnCardL(*iFreshLocalView);
			break;
			*/
			}
		case ESearchForEuroChar:
			{
			iLog->LogLine(_L("=== Search for a Euro character"));
			_LIT(KFindString1,"\x20a0");
			CPtrCArray* array =new(ELeave)CPtrC16Array(1);
			CleanupStack::PushL(array);
			array->AppendL(KFindString1());
			RPointerArray<CViewContact> contactArray;
			CleanupStack::PushL(TCleanupItem(CleanupContactArray,&contactArray));
			iRemoteView_1->ContactsMatchingCriteriaL(*array, contactArray);
			CleanupStack::PopAndDestroy(2, array);
			NextTest();
			}
			break;
		case ENumTests:
			{
			RThread thread;
			test(thread.RequestCount()==0);
			CActiveScheduler::Stop();
			}
			break;
		default:
			ASSERT(EFalse);
			break;
		}
	}

void CTestConductor::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	if(&aView == iRemoteFindView) 
		{
		return;
		}

	if(&aView == iLocalFindView)
		{
		return;
		}
	TRAPD(err, iCurrentTestObject->HandleNotificationL(aView,aEvent) );
	test(err == KErrNone);
	}

void CTestConductor::DoCancel()
	{
	}


TInt CTestConductor::RunError(TInt aError)
	{
	iTestError = aError;

	// quick code to report the test step and leave error
	// allows test to exit nicely, rather than Panic
	switch (iCurrentTestNumber)
		{
		case ECreateLocalView: test.Printf(_L("RunL left in step CreateLocalView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddInitialContacts: test.Printf(_L("RunL left in step AddInitialContacts (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseLocalView_1: test.Printf(_L("RunL left in step ExerciseLocalView_1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateLocalNamedView: test.Printf(_L("RunL left in step CreateLocalNamedView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseLocalNamedView_1: test.Printf(_L("RunL left in step ExerciseLocalNamedView_1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddMoreContacts_1: test.Printf(_L("RunL left in step AddMoreContacts_1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseLocalView_2: test.Printf(_L("RunL left in step ExerciseLocalView_2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseLocalNamedView_2: test.Printf(_L("RunL left in step ExerciseLocalNamedView_2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateRemoteView1: test.Printf(_L("RunL left in step CreateRemoteView1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseRemoteView1: test.Printf(_L("RunL left in step ExerciseRemoteView1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateRemoteView2: test.Printf(_L("RunL left in step CreateRemoteView2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseRemoteView2: test.Printf(_L("RunL left in step ExerciseRemoteView2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateNamedRemoteView1: test.Printf(_L("RunL left in step CreateNamedRemoteView1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseNamedRemoteView1: test.Printf(_L("RunL left in step ExerciseNamedRemoteView1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateNamedRemoteView2: test.Printf(_L("RunL left in step CreateNamedRemoteView2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseNamedRemoteView2: test.Printf(_L("RunL left in step ExerciseNamedRemoteView2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateSubViewLessThanA: test.Printf(_L("RunL left in step CreateSubViewLessThanA (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseSubViewLessThanA: test.Printf(_L("RunL left in step ExerciseSubViewLessThanA (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateSubViewAbc: test.Printf(_L("RunL left in step CreateSubViewAbc (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseSubViewAbc: test.Printf(_L("RunL left in step ExerciseSubViewAbc (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateSubViewDef: test.Printf(_L("RunL left in step CreateSubViewDef (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseSubViewDef: test.Printf(_L("RunL left in step ExerciseSubViewDef (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateSubViewGhi: test.Printf(_L("RunL left in step CreateSubViewGhi (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseSubViewGhi: test.Printf(_L("RunL left in step ExerciseSubViewGhi (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateSubViewJkl: test.Printf(_L("RunL left in step CreateSubViewJkl (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseSubViewJkl: test.Printf(_L("RunL left in step ExerciseSubViewJkl (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateSubViewGreaterThanOrEqualToM: test.Printf(_L("RunL left in step CreateSubViewGreaterThanOrEqualToM (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseSubViewGreaterThanOrEqualToM: test.Printf(_L("RunL left in step ExerciseSubViewGreaterThanOrEqualToM (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateConcatenatedView: test.Printf(_L("RunL left in step CreateConcatenatedView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseConcatenatedView: test.Printf(_L("RunL left in step ExerciseConcatenatedView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EChangeSortOrder1: test.Printf(_L("RunL left in step ChangeSortOrder1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EChangeSortOrder2: test.Printf(_L("RunL left in step ChangeSortOrder2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EChangeSortOrder3: test.Printf(_L("RunL left in step ChangeSortOrder3 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseAllAffectedViews: test.Printf(_L("RunL left in step ExerciseAllAffectedViews (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddMoreContacts_2: test.Printf(_L("RunL left in step AddMoreContacts_2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseAllViews_1: test.Printf(_L("RunL left in step ExerciseAllViews_1 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ERemoveSomeContacts: test.Printf(_L("RunL left in step RemoveSomeContacts (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseAllViews_2: test.Printf(_L("RunL left in step ExerciseAllViews_2 (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAllViewsOutOfBoundsAccess: test.Printf(_L("RunL left in step AllViewsOutOfBoundsAccess (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ECreateLocalViewWithUnsortableAtEnd: test.Printf(_L("RunL left in step CreateLocalViewWithUnsortableAtEnd (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddUnsortableContacts: test.Printf(_L("RunL left in step AddUnsortableContacts (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseLocalViewWithUnsortableAtEnd: test.Printf(_L("RunL left in step ExerciseLocalViewWithUnsortableAtEnd (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddFreshLocalView: test.Printf(_L("RunL left in step AddFreshLocalView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseFreshLocalView: test.Printf(_L("RunL left in step ExerciseFreshLocalView (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddSomeMoreUnsortableContacts: test.Printf(_L("RunL left in step AddSomeMoreUnsortableContacts (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseViewForPrefsAgain: test.Printf(_L("RunL left in step ExerciseViewForPrefsAgain (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EAddWhiteSpaceContacts: test.Printf(_L("RunL left in step AddWhiteSpaceContacts (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EExerciseWhiteSpaceContacts: test.Printf(_L("RunL left in step ExerciseWhiteSpaceContacts (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ERemoveOwnCardContact: test.Printf(_L("RunL left in step RemoveOwnCardContact (%i) with error %i"), iCurrentTestNumber, aError); break;
		case EModifyOwnCardContact: test.Printf(_L("RunL left in step ModifyOwnCardContact (%i) with error %i"), iCurrentTestNumber, aError); break;
		case ESearchForEuroChar: test.Printf(_L("RunL left in step SearchForEuroChar (%i) with error %i"), iCurrentTestNumber, aError); break;

		case ENumTests: test.Printf(_L("RunL left in step NumTests (%i) with error %i"), iCurrentTestNumber, aError); break;

		default: test.Printf(_L("RunL left in step %i with error %i"), iCurrentTestNumber, aError); break;
		}

	// stop test
	CActiveScheduler::Stop();
	return KErrNone;
	}


void CTestConductor::ExerciseViewPreferencesL(const CContactViewBase* aViewToTest,TContactViewPreferences aPrefsToTest)
	{
	//test the viewpreferences of a view. All views with EunsortedatBegining for example should place, any contact
	//without the field specified in a sort to the begining of the main sorted list of contact
	TInt normalContacts = aViewToTest->CountL()-iTestIds.Count();
	test(normalContacts>0);
	TContactItemId id=KErrNotFound;
	TInt index =KErrNotFound;
	for(TInt ii=0;ii<iTestIds.Count();ii++)
		{
		index = aViewToTest->FindL(iTestIds[ii]);
		test(index>=0);//no error code
		if(aPrefsToTest&EUnSortedAtBeginning)
			{
			//should be before 'normal' contacts
			test(index<iTestIds.Count());//in the right place
			}
		else if(aPrefsToTest&EUnSortedAtEnd)
			{
			//should be after the last index of the 'normal' contacts
			test(index>=normalContacts);//in the right place
			}
		//Integrity check
		id=aViewToTest->AtL(index);
		const CViewContact& contact=aViewToTest->ContactAtL(index);
		test(contact.Id()==id);
		if(aPrefsToTest&ESingleWhiteSpaceIsEmptyField)
			{
			//Contact Fields Should not have any white space as a field, _L(" ")
			for(TInt zz=0;zz<contact.FieldCount();zz++)
				{
				TCollationMethod collateMethod = *Mem::CollationMethodByIndex(0);
				collateMethod.iFlags|=TCollationMethod::EIgnoreNone;
				test((contact.Field(zz).CompareC(KWhiteSpaces,3,&collateMethod))!=0);
				}
			}
		}
	}

//
// CSortChecker.
//

CSortChecker* CSortChecker::NewL(CTestConductor& aConductor)
	{
	CSortChecker* self=new(ELeave) CSortChecker(aConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CSortChecker::CSortChecker(CTestConductor& aConductor) : iConductor(aConductor)
	{
	}

void CSortChecker::ConstructL()
	{
	iCollateMethod=new(ELeave) TCollationMethod;
	*iCollateMethod=*Mem::CollationMethodByIndex(0);
	iCollateMethod->iFlags|=TCollationMethod::EIgnoreNone;
	}

CSortChecker::~CSortChecker()
	{
	iSortedFirstLast.ResetAndDestroy();
	iSortedLastFirst.ResetAndDestroy();
	delete iCollateMethod;
	}

void CSortChecker::AddContactL(const TDesC& aFirstName,const TDesC& aLastName)
	{
	HBufC* buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length()+1);
	TPtr bufPtr(buf->Des());
	bufPtr.Append(aFirstName);
 	if (aFirstName.Length())
 		bufPtr.Append(_L(" "));
	bufPtr.Append(aLastName);
	User::LeaveIfError(iSortedFirstLast.Append(buf));
	CleanupStack::Pop(); // buf.

	buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length()+1);
	TPtr bufPtr2(buf->Des());
	bufPtr2.Append(aLastName);
 	if (aLastName.Length())
 		bufPtr2.Append(_L(" "));
	bufPtr2.Append(aFirstName);
	User::LeaveIfError(iSortedLastFirst.Append(buf));
	CleanupStack::Pop(); // buf.
	}

void CSortChecker::RemoveContactL(const TDesC& aFirstName,const TDesC& aLastName)
	{
	HBufC* buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length()+1);
	TPtr bufPtr(buf->Des());
	bufPtr.Append(aFirstName);
 	if (aFirstName.Length())
 		bufPtr.Append(_L(" "));
	bufPtr.Append(aLastName);
	TInt index=iSortedFirstLast.Find(buf,TIdentityRelation<HBufC>(Match));
	ASSERT(index!=KErrNotFound);
	delete iSortedFirstLast[index];
	iSortedFirstLast.Remove(index);
	CleanupStack::PopAndDestroy(); // buf.

	buf=HBufC::NewLC(aFirstName.Length()+aLastName.Length()+1);
	TPtr bufPtr2(buf->Des());
	bufPtr2.Append(aLastName);
 	if (aLastName.Length())
 		bufPtr2.Append(_L(" "));
	bufPtr2.Append(aFirstName);
	index=iSortedLastFirst.Find(buf,TIdentityRelation<HBufC>(Match));
	ASSERT(index!=KErrNotFound);
	delete iSortedLastFirst[index];
	iSortedLastFirst.Remove(index);
	CleanupStack::PopAndDestroy(); // buf.
	}

TBool CSortChecker::Match(const HBufC& aFirst,const HBufC& aSecond)
	{
	if (aFirst.Compare(aSecond)==0)
		{
		return ETrue;
		}
	return EFalse;
	}

TPtrC CSortChecker::ContactAt(TSortOrder aOrder,TInt aIndex) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	return TPtrC(*sortedList[aIndex]);
	}

TInt CSortChecker::Count() const
	{
	return iSortedFirstLast.Count();
	}

TInt CSortChecker::Count(TSortOrder aOrder,const TDesC& aLowBoundary,const TDesC& aHighBoundary) const
	{
	const TInt low=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	const TInt high=FindIndex(aOrder,aHighBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (low==KErrNotFound)
		{
		test(high==KErrNotFound);
		return 0;
		}
	return high-low;
	}

TInt CSortChecker::CountLow(TSortOrder aOrder,const TDesC& aHighBoundary) const
	{
	const TInt high=FindIndex(aOrder,aHighBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (high==KErrNotFound)
		{
		return 0;
		}
	return high;
	}

TInt CSortChecker::CountHigh(TSortOrder aOrder,const TDesC& aLowBoundary) const
	{
	const TInt low=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (low==KErrNotFound)
		{
		return 0;
		}
	return Count()-low;
	}

void CSortChecker::Sort()
	{
 	iSortedFirstLast.Sort(TLinearOrder<HBufC>(CompareWithoutIgnoringSpacesL));
 	iSortedLastFirst.Sort(TLinearOrder<HBufC>(CompareWithoutIgnoringSpacesL));

	const TInt numContacts=iSortedFirstLast.Count();
	__ASSERT_ALWAYS(numContacts==iSortedLastFirst.Count(),User::Invariant());
	}

void CSortChecker::CheckSort(TSortOrder aOrder,const TDesC& aName,TInt aIndex) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
#ifdef VERBOSE_LOGGING	
	iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,sortedList[aIndex]);
#endif
	test(sortedList[aIndex]->Compare(aName)==0);
	}

void CSortChecker::CheckSortLowSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aHighBoundary) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const HBufC* name=sortedList[aIndex];
#ifdef VERBOSE_LOGGING	
	iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,name);
#endif
	test(name->CompareC(aHighBoundary,3,iCollateMethod)<0);
	test(name->Compare(aName)==0);
	}

void CSortChecker::CheckSortHighSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aLowBoundary) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const TInt lowIndex=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (lowIndex!=KErrNotFound)
		{
#ifdef VERBOSE_LOGGING	
		iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,sortedList[lowIndex+aIndex]);
#endif
		test(sortedList[lowIndex+aIndex]->Compare(aName)==0);
		}
	}

void CSortChecker::CheckSortSubView(TSortOrder aOrder,const TDesC& aName,TInt aIndex,const TDesC& aLowBoundary,const TDesC& aHighBoundary) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const TInt lowIndex=FindIndex(aOrder,aLowBoundary,CSortChecker::EGreaterThanOrEqualTo);
	if (lowIndex!=KErrNotFound)
		{
		const HBufC* name=sortedList[lowIndex+aIndex];
#ifdef VERBOSE_LOGGING	
		iConductor.iLog->LogLineNoEcho(_L("Checking - %S against %S"),&aName,name);
#endif		
		test(name->CompareC(aHighBoundary,3,iCollateMethod)<0);
		test(name->Compare(aName)==0);
		}
	}

TInt CSortChecker::FindIndex(TSortOrder aOrder,const TDesC& aBoundary,TCriteria aCriteria) const
	{
	const RPointerArray<HBufC>& sortedList=(aOrder==ELastNameFirstName) ? iSortedLastFirst : iSortedFirstLast;
	const TInt numContacts=sortedList.Count();
	for (TInt ii=0;ii<numContacts;++ii)
		{
		switch (aCriteria)
			{
			case ELessThan:
				if (sortedList[ii]->CompareC(aBoundary,3,iCollateMethod)<0)
					{
					return ii;
					}
				break;
			case ELessThanOrEqualTo:
				if (sortedList[ii]->CompareC(aBoundary,3,iCollateMethod)<=0)
					{
					return ii;
					}
				break;
			case EGreaterThan:
				if (sortedList[ii]->CompareC(aBoundary,3,iCollateMethod)>0)
					{
					return ii;
					}
				break;
			case EGreaterThanOrEqualTo:
				if (sortedList[ii]->CompareC(aBoundary,3,iCollateMethod)>=0)
					{
					return ii;
					}
				break;
			default:
				ASSERT(EFalse);
			}
		}
	return KErrNotFound;
	}

TInt CSortChecker::Compare(const HBufC& aFirst, const HBufC& aSecond)
	{
	return aFirst.CompareC(aSecond);
	}

TInt CSortChecker::CompareWithoutIgnoringSpacesL(const HBufC& aFirst, const HBufC& aSecond)
 	{
 	// In order to sort correctly, we need to recognise spaces, but ignore other punctuation
 	HBufC* temp1=HBufC::NewLC(aFirst.Length());
 	TPtr first(temp1->Des());
 	first.Copy(aFirst);
 	HBufC* temp2=HBufC::NewLC(aSecond.Length());
 	TPtr second(temp2->Des());
	 second.Copy(aSecond);

   	TCollationMethod collateMethod;
   	collateMethod = *Mem::CollationMethodByIndex(0);
   	collateMethod.iFlags|=TCollationMethod::EIgnoreNone;
 	TInt compare=0;
 	TInt collationLevel=0;
 
 	while (!compare && collationLevel<4)
 		{
 		compare=first.CompareC(second,collationLevel,&collateMethod);
 		++collationLevel;
 		}
 
 	CleanupStack::PopAndDestroy(2);
 	return compare;
 	}

//
// CTestBase.
//

CTestBase::CTestBase(CTestConductor& aConductor) : iConductor(aConductor)
	{
	}


//
// CViewCreator.
//

CViewCreator* CViewCreator::NewL(CTestConductor& aConductor)
	{
	CViewCreator* self=new(ELeave) CViewCreator(aConductor);
	return self;
	}

CContactLocalView* CViewCreator::CreateLocalViewL(const RContactViewSortOrder& aSortOrder,TContactViewPreferences aViewPrefs)
	{
	iView=CContactLocalView::NewL(iConductor,*iConductor.iDb,aSortOrder,aViewPrefs);
	return STATIC_CAST(CContactLocalView*,iView);
	}

CContactLocalView* CViewCreator::CreateLocalViewL(const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EUnSortedAtBeginning;
	iView=CContactLocalView::NewL(iConductor,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactLocalView*,iView);
	}

CContactNamedLocalView* CViewCreator::CreateNamedLocalViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EUnSortedAtBeginning;
	iView=CContactNamedLocalView::NewL(iConductor,aName,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactNamedLocalView*,iView);
	}

CContactRemoteView* CViewCreator::CreateRemoteViewL(const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EUnSortedAtBeginning;
	iView=CContactRemoteView::NewL(iConductor,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactRemoteView*,iView);
	}

CContactNamedRemoteView* CViewCreator::CreateNamedRemoteViewL(const TDesC& aName,const RContactViewSortOrder& aSortOrder)
	{
	TInt32 viewPrefs=0;
	viewPrefs|=EUnSortedAtBeginning;
	iView=CContactNamedRemoteView::NewL(iConductor,aName,*iConductor.iDb,aSortOrder,TContactViewPreferences(viewPrefs));
	return STATIC_CAST(CContactNamedRemoteView*,iView);
	}

CContactSubView* CViewCreator::CreateSubViewL(CContactViewBase& aUnderlyingView,const TDesC& aBoundary)
	{
	iView=CContactSubView::NewL(iConductor,*iConductor.iDb,aUnderlyingView,aBoundary);
	return STATIC_CAST(CContactSubView*,iView);
	}

CContactSubView* CViewCreator::CreateSubViewL(CContactViewBase& aUnderlyingView,const TDesC& aLowBoundary,const TDesC& aHighBoundary)
	{
	iView=CContactSubView::NewL(iConductor,*iConductor.iDb,aUnderlyingView,aLowBoundary,aHighBoundary);
	return STATIC_CAST(CContactSubView*,iView);
	}

CContactConcatenatedView* CViewCreator::CreateConcatenatedViewL(RPointerArray<CContactViewBase>& aComponentViews)
	{
	iView=CContactConcatenatedView::NewL(iConductor,*iConductor.iDb,aComponentViews);
	return STATIC_CAST(CContactConcatenatedView*,iView);
	}

void CViewCreator::HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	if(&aView==iView)
		{
		test(aEvent.iEventType==TContactViewEvent::EReady);
		iConductor.NextTest();
		}
	}

CViewCreator::CViewCreator(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}


//
// CContactAdder.
//

CContactAdder* CContactAdder::NewL(CTestConductor& aConductor)
	{
	CContactAdder* self=new(ELeave) CContactAdder(aConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CContactAdder::~CContactAdder()
	{
	delete iTemplate;
	iFile.Close();
	iFs.Close();
	}

void CContactAdder::AddContactsL(const TDesC& aFileName,TInt aNumViews)
	{
	TInt err = iFile.Open(iFs,aFileName,EFileShareAny|EFileStreamText);
	if (err == KErrNotFound)
		{
		test.Printf(_L("Contact test data file (\"%S\") is missing from build, ensure exports are built."), &aFileName);
		}
	User::LeaveIfError(err);
	iNumViews=aNumViews;
	TInt pos=2;
	iFile.Seek(ESeekStart,pos); // Ignore Unicode header.
	test.Printf(_L("Adding contact "));
	AddOneContactL();
	}

void CContactAdder::HandleNotificationL(const CContactViewBase&,const TContactViewEvent& aEvent)
	{
	if (++iNumNotifications==iNumViews)
		{
		if (iFinished)
			{
			iConductor.NextTest();
			}
		else
			{
			AddOneContactL();
			}
		}
	else
		{
		test(aEvent.iEventType==TContactViewEvent::EItemAdded);
		}
	}

CContactAdder::CContactAdder(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}

void CContactAdder::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iTemplate=STATIC_CAST(CContactTemplate*,iConductor.iDb->ReadContactL(iConductor.iDb->TemplateId()));
	}

void CContactAdder::AddOneContactL()
	{
	iNumNotifications=0;
	TBuf<64> firstName;
	TBuf<64> lastName;
	iConductor.iLog->IncVisualCounter();
	iFinished=!ReadNamePairL(iFile,firstName,lastName);
#ifdef VERBOSE_LOGGING	
	iConductor.iLog->LogLineNoEcho(_L("Adding %S %S"),&firstName,&lastName);
#endif	
	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	contact->SetFirstNameL(firstName);
	contact->SetLastNameL(lastName);
	iConductor.iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(); // contact.
	iConductor.iDb->CompactL();
	iConductor.iSortChecker->AddContactL(firstName,lastName);
	if (iFinished)
		{
		iConductor.iLog->EndVisualCounter();
		iConductor.iSortChecker->Sort();
		}
	}

TBool CContactAdder::ReadNamePairL(RFile aFile,TDes& aFirstName,TDes& aLastName)
	{
	TBuf8<2> character=_L8("  ");
	TBuf8<4> lineEndTestBuf=_L8("    ");

	aFirstName.SetLength(0);
	aLastName.SetLength(0);
	TBool readingLastName=ETrue;
	while (lineEndTestBuf!=KUnicodeLineEnd)
		{
		User::LeaveIfError(aFile.Read(character,2));
		if (character.Length()>0)
			{
			lineEndTestBuf[0]=lineEndTestBuf[2];
			lineEndTestBuf[1]=lineEndTestBuf[3];
			lineEndTestBuf[2]=character[0];
			lineEndTestBuf[3]=character[1];

			const TPtrC16 widePtr((TUint16*)character.Ptr(),1);
			if (widePtr[0]==';')
				{
				readingLastName=EFalse;
				}
			else if (readingLastName)
				{
				aLastName.Append(widePtr);
				}
			else
				{
				aFirstName.Append(widePtr);
				}
			}
		else
			{
			return EFalse;
			}
		}

	aFirstName.Delete(aFirstName.Length()-2,2); // Strip off \n\r.
	return ETrue;
	}


//
// CContactRemover.
//

CContactRemover* CContactRemover::NewL(CTestConductor& aConductor)
	{
	CContactRemover* self=new(ELeave) CContactRemover(aConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CContactRemover::~CContactRemover()
	{
	iFile.Close();
	iFs.Close();
	}

void CContactRemover::RemoveContactsL(const TDesC& aFileName,TInt aNumViews)
	{
	User::LeaveIfError(iFile.Open(iFs,aFileName,EFileShareAny|EFileStreamText));
	iNumViews=aNumViews;
	TInt pos=2;
	iFile.Seek(ESeekStart,pos); // Ignore Unicode header.
	test.Printf(_L("Removing contact "));
	RemoveOneContactL();
	}

void CContactRemover::HandleNotificationL(const CContactViewBase&,const TContactViewEvent& aEvent)
	{
	if (++iNumNotifications==iNumViews)
		{
		if (iFinished)
			{
			iConductor.NextTest();
			}
		else
			{
			RemoveOneContactL();
			}
		}
	else
		{
		test(aEvent.iEventType==TContactViewEvent::EItemRemoved);
		}
	}

CContactRemover::CContactRemover(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}

void CContactRemover::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}

void CContactRemover::RemoveOneContactL()
	{
	iNumNotifications=0;
	TBuf<64> firstName;
	TBuf<64> lastName;
	iConductor.iLog->IncVisualCounter();
	iFinished=!CContactAdder::ReadNamePairL(iFile,firstName,lastName);
#ifdef VERBOSE_LOGGING	
	iConductor.iLog->LogLineNoEcho(_L("Removing %S %S"),&firstName,&lastName);
#endif
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	def->AppendL(KUidContactFieldFamilyName);
	CContactIdArray* matchList1=iConductor.iDb->FindLC(lastName,def);
	CContactIdArray* matchList2=iConductor.iDb->FindLC(firstName,def);

	for (TInt ii=matchList1->Count()-1;ii>=0;--ii)
		{
		TContactItemId thisId=(*matchList1)[ii];
		for (TInt jj=matchList2->Count()-1;jj>=0;--jj)
			{
			if (thisId==(*matchList2)[jj])
				{
				iConductor.iDb->DeleteContactL(thisId);
				goto carry_on;
				}
			}
		}
	ASSERT(EFalse);

carry_on:
	CleanupStack::PopAndDestroy(3); // matchList2, matchList1, def.
	iConductor.iDb->CompactL();
	iConductor.iSortChecker->RemoveContactL(firstName,lastName);
	if (iFinished)
		{
		iConductor.iLog->EndVisualCounter();
		iConductor.iSortChecker->Sort();
		}
	}


//
// CViewExerciser.
//

CViewExerciser* CViewExerciser::NewL(CTestConductor& aConductor)
	{
	CViewExerciser* self=new(ELeave) CViewExerciser(aConductor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

void CViewExerciser::ProfileViewL(CContactViewBase& aView)
	{
	TInt viewCount = aView.CountL();
	RDebug::Print(_L("Profile view Count: %d"),viewCount);
	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	for (TInt ii=0;ii<viewCount;++ii)
		{
		aView.ContactAtL(ii);
		}
	CCntTest::ProfileEnd(0);
	TCntProfile profile[1];
	CCntTest::ProfileResult(profile,0,1);
	TReal result = profile[0].iTime;///1000000.0;
	RDebug::Print(_L("Profile ContactAt Time Taken: %g"),result);
	}

void CViewExerciser::ExerciseViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView)
	{
	TBuf<128> scratchBuf;
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	test(iConductor.iSortChecker->Count()==numItems);

	for (TInt ii=0;ii<numItems;++ii)
		{
		TContactItemId thisId=aView.AtL(ii);
		if (ii==numItems-1)
			{
			lastId=thisId;
			}

		GetComparisonStringL(thisId,aSortOrder,scratchBuf);
		iConductor.iSortChecker->CheckSort(aSortOrder,scratchBuf,ii);
		scratchBuf.SetLength(0);
		}

	test(aView.FindL(lastId)==numItems-1);

	if (iQueue)
		{
		iConductor.NextTest();
		}
	}

void CViewExerciser::ExerciseSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aLowBoundary,const TDesC& aHighBoundary)
	{
	TBuf<128> scratchBuf;
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	test(iConductor.iSortChecker->Count(aSortOrder,aLowBoundary,aHighBoundary)==numItems);

	for (TInt ii=0;ii<numItems;++ii)
		{
		TContactItemId thisId=aView.AtL(ii);
		if (ii==numItems-1)
			{
			lastId=thisId;
			}
		GetComparisonStringL(thisId,aSortOrder,scratchBuf);
		iConductor.iSortChecker->CheckSortSubView(aSortOrder,scratchBuf,ii,aLowBoundary,aHighBoundary);
		scratchBuf.SetLength(0);
		}

	test(aView.FindL(lastId)==numItems-1);

	if (iQueue)
		{
		iConductor.NextTest();
		}
	}

void CViewExerciser::ExerciseLowSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aLowBoundary)
	{
	TBuf<128> scratchBuf;
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	test(iConductor.iSortChecker->CountLow(aSortOrder,aLowBoundary)==numItems);

	for (TInt ii=0;ii<numItems;++ii)
		{
		TContactItemId thisId=aView.AtL(ii);
		if (ii==numItems-1)
			{
			lastId=thisId;
			}
		GetComparisonStringL(thisId,aSortOrder,scratchBuf);
		iConductor.iSortChecker->CheckSortLowSubView(aSortOrder,scratchBuf,ii,aLowBoundary);
		scratchBuf.SetLength(0);
		}

	test(aView.FindL(lastId)==numItems-1);

	if (iQueue)
		{
		iConductor.NextTest();
		}
	}

void CViewExerciser::ExerciseHighSubViewL(CSortChecker::TSortOrder aSortOrder,CContactViewBase& aView,const TDesC& aHighBoundary)
	{
	TBuf<128> scratchBuf;
	TContactItemId lastId=0;
	const TInt numItems=aView.CountL();
	test(iConductor.iSortChecker->CountHigh(aSortOrder,aHighBoundary)==numItems);

	for (TInt ii=0;ii<numItems;++ii)
		{
		TContactItemId thisId=aView.AtL(ii);
		if (ii==numItems-1)
			{
			lastId=thisId;
			}
		GetComparisonStringL(thisId,aSortOrder,scratchBuf);
		iConductor.iSortChecker->CheckSortHighSubView(aSortOrder,scratchBuf,ii,aHighBoundary);
		scratchBuf.SetLength(0);
		}

	test(aView.FindL(lastId)==numItems-1);

	if (iQueue)
		{
		iConductor.NextTest();
		}
	}

CViewExerciser::~CViewExerciser()
	{
	delete iTextDef_FirstLast;
	delete iTextDef_LastFirst;
	}

void CViewExerciser::HandleNotificationL(const CContactViewBase&,const TContactViewEvent&)
	{
	test(EFalse);
	}

void CViewExerciser::GetComparisonStringL(const TContactItemId aContactId, const CSortChecker::TSortOrder aSortOrder, TDes& aContactInfo)
 	{
 	TBuf<64> first;
 	TBuf<64> last;
 
 	CContactItem* contact=iConductor.iDb->ReadMinimalContactLC(aContactId);
 
 	CContactItemFieldSet& fieldSet=contact->CardFields();
 	TInt pos=fieldSet.Find(KUidContactFieldGivenName);
 	if (pos!=KErrNotFound)
 		first.Append(fieldSet[pos].TextStorage()->Text());
 	pos=fieldSet.Find(KUidContactFieldFamilyName);
 	if (pos!=KErrNotFound)
 		last.Append(fieldSet[pos].TextStorage()->Text());
 	CleanupStack::PopAndDestroy(contact);
 
 	if (aSortOrder==CSortChecker::EFirstNameLastName)
 		{
 		aContactInfo.Append(first);
 		if (first.Length())
 			aContactInfo.Append(_L(" "));
 		aContactInfo.Append(last);
 		}
 	else
 		{
 		aContactInfo.Append(last);
 		if (last.Length())
 			aContactInfo.Append(_L(" "));
 		aContactInfo.Append(first);
 		}
 	}

CViewExerciser::CViewExerciser(CTestConductor& aConductor) : CTestBase(aConductor),iQueue(ETrue)
	{
	}

void CViewExerciser::ConstructL()
	{
	iTextDef_FirstLast=CContactTextDef::NewL();
	iTextDef_FirstLast->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
	iTextDef_FirstLast->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
	iTextDef_FirstLast->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));

	iTextDef_LastFirst=CContactTextDef::NewL();
	iTextDef_LastFirst->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
	iTextDef_LastFirst->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
	iTextDef_LastFirst->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));
	}

void CViewExerciser::QueueNextTestWhenComplete(TBool aQueue)
	{
	iQueue=aQueue;
	}


//
// CSortOrderChanger.
//

CSortOrderChanger* CSortOrderChanger::NewL(CTestConductor& aConductor)
	{
	CSortOrderChanger* self=new(ELeave) CSortOrderChanger(aConductor);
	return self;
	}

void CSortOrderChanger::ChangeSortOrderL(RContactViewSortOrder& aSortOrder,CContactNamedLocalView& aView,TInt aNumViews)
	{
	aView.ChangeSortOrderL(aSortOrder);
	iNumViews=aNumViews;
	iNumNotifications=0;
	}

void CSortOrderChanger::ChangeSortOrderL(RContactViewSortOrder& aSortOrder,CContactNamedRemoteView& aView,TInt aNumViews)
	{
	aView.ChangeSortOrderL(aSortOrder);
	iNumViews=aNumViews;
	iNumNotifications=0;
	}

CSortOrderChanger::~CSortOrderChanger()
	{
	}

void CSortOrderChanger::HandleNotificationL(const CContactViewBase&,const TContactViewEvent& aEvent)
	{
	test(aEvent.iEventType==TContactViewEvent::EUnavailable || aEvent.iEventType==TContactViewEvent::ESortOrderChanged);
	if (++iNumNotifications==iNumViews*2) // 2 because there will be EUnavailable, ESortOrderChanged notifications for each view.		
		{
		iConductor.NextTest();
		}
	}

CSortOrderChanger::CSortOrderChanger(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}


//
// CUnsortableContactAdder.
//

CUnsortableContactAdder::CUnsortableContactAdder(CTestConductor& aConductor,const CContactViewBase& aView)
	:CTestBase(aConductor),iView(&aView)
	{}

CUnsortableContactAdder* CUnsortableContactAdder::NewL(CTestConductor& aConductor,const CContactViewBase& aView)
	{
	CUnsortableContactAdder* self=new(ELeave) CUnsortableContactAdder(aConductor,aView);
	return self;
	}

CUnsortableContactAdder::~CUnsortableContactAdder()
		{
		}

void CUnsortableContactAdder::AddContactsL(TInt aNumberOfContact,TContactViewPreferences aContactTypes)
	{
	iNumContacts = aNumberOfContact;
	iNumNotifications=0;
	for(TInt ii=0;ii<KNumberOfUnsortableContacts;ii++)
		{
		TContactItemId id = iConductor.iRandomGenerator->AddTypicalContactForLocalViewPreferencesL(TContactViewPreferences(aContactTypes));
		iConductor.iTestIds.Append(id);
		}


	}
void CUnsortableContactAdder::HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	if(aEvent.iEventType==TContactViewEvent::EItemAdded && &aView == iView )
		{
		//only  start the next test when all the contact have been added to our view.
		if (++iNumNotifications==iNumContacts) 
			{
			iConductor.NextTest();
			}
		}
	}

/** Constructor */
CContactOwnCardRemover::CContactOwnCardRemover(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}

/** Default destructor */
CContactOwnCardRemover::~CContactOwnCardRemover()
	{
	}

/** Sets aId as an own card, then deletes it */
void CContactOwnCardRemover::RemoveOwnCardL(const CContactViewBase& aView)
	{
	iView=&aView;
	iOriginalCount = aView.CountL();
	TInt id = aView.AtL(0);
	CContactItem* item = iConductor.iDb->ReadContactLC(id);
	iConductor.iDb->SetOwnCardL(*item);
	CleanupStack::PopAndDestroy(item);
	iConductor.iDb->DeleteContactL(id);
	}

/** Verifies that a remove event was sent as a result of deleting the item */
void CContactOwnCardRemover::HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	if (&aView == iView && aEvent.iEventType == TContactViewEvent::EItemRemoved)
		{
		test(iView->CountL() == iOriginalCount - 1);
		iConductor.NextTest();
		}
	}

//	Added test code here for modification of own card

_LIT(KNewOwnerCardFirstName, "T_VIEW2_OWNCARD");

/** Constructor */
CContactOwnCardModifier::CContactOwnCardModifier(CTestConductor& aConductor) : CTestBase(aConductor)
	{
	}

/** Default destructor */
CContactOwnCardModifier::~CContactOwnCardModifier()
	{
	}

/** Sets aId as an own card, then modifies it */
void CContactOwnCardModifier::ModifyOwnCardL(const CContactViewBase& aView)
{
	iView=&aView;
	iOriginalCount = aView.CountL();
	TInt id = aView.AtL(0);
	CContactItem* item = iConductor.iDb->OpenContactLX(id);
	if (item)
	{
		CleanupStack::PushL(item);
		iConductor.iDb->SetOwnCardL(*item);
	//	Modify first name	
		CContactItemFieldSet& fieldSet = item->CardFields();
		TInt index=fieldSet.Find(KUidContactFieldGivenName);
		User::LeaveIfError(index);
		CContactItemField& field = fieldSet[index];
		ASSERT(field.StorageType()==KStorageTypeText);
		STATIC_CAST(CContactTextField*,field.Storage())->SetTextL(KNewOwnerCardFirstName);
		iConductor.iDb->CommitContactL(*item);
		CleanupStack::PopAndDestroy(2);	//	item, lock record
	}
}

/** Verifies that notification of own card item modification has been received*/
void CContactOwnCardModifier::HandleNotificationL(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	if (&aView == iView && aEvent.iEventType == TContactViewEvent::EItemAdded)
		{//	THIS SHOULD NOT LEAVE...
		test(iView->CountL()==iOriginalCount);	// Follows corresponding remove
	//	Test that the own card data has been updated correctly
		TContactItemId ownCardId = iConductor.iDb->OwnCardId();
		TInt ownCardIndex = aView.FindL(ownCardId);
		const CViewContact& contact = aView.ContactAtL(ownCardIndex);
		test(ownCardId==contact.Id());			//	No reason this should fail
		
		const RContactViewSortOrder& sortOrder = aView.SortOrderL();
	//	Get the first name field
		TInt index = 0;
		TInt soCount = sortOrder.Count();
		for (; index < soCount; ++index)
		{
			if (KUidContactFieldGivenName == sortOrder[index])
				{
				break;
				}
		}
		
		TPtrC fieldData(contact.Field(index) );
		test( (fieldData.CompareC(KNewOwnerCardFirstName) ) == 0);
		iNotificationsReceived++;
		if (iNotificationsReceived == 2)
			{
			iConductor.NextTest();
			}
		}
	}


//
// Main.
//

/**

@SYMTestCaseID     PIM-T-VIEW2-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
//	UserHal::SetAutoSwitchOffBehavior(ESwitchOffDisabled); 
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* tester=NULL;
			test.Start(_L("@SYMTESTCaseID:PIM-T-VIEW2-0001 t_view2"));

			TRAPD(err,tester=CTestConductor::NewL());
			__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
			test.End();
			delete tester;
			test.Close();
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
