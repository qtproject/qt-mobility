/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <barsc.h>
#include <barsread.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils.h"
#include "t_custfilter.h"

//
// Constants.
//

// Strings matching those within the cntmodel.rls file.
_LIT(KCustomFilterOne,"Custom Filterable field No.1");
_LIT(KCustomFilterTwo,"Custom Filterable field No.2");

_LIT(KTestName,"T_CustFilter");
#include "testhelpers.h"

_LIT(KLogFileName,"T_CustFilter.log");

_LIT(KDbFileName,"c:T_CustFilter.cdb");

_LIT(KRemoteViewName,"RemoteView");
_LIT(KTextDefSeparator,"");

_LIT(KNickName,		"NickName");
_LIT(KAcceptAlways, "Accept Always");
_LIT(KAskAlways, "Ask Always");
_LIT(KContactString,"Adding NEW CONTACT HERE\n");

const TInt KLocation	= 500;
const TInt KNickname	= 100;
TInt64 randomSeed = 45879;

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

CTestConductor::CTestConductor() {}

void CTestConductor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iLog=CLog::NewL(test,KLogFileName);
	iDb=CContactDatabase::ReplaceL(KDbFileName);
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(*iDb);
	RestoreFilterableFieldsFromSystemTemplateL();
	AddContactsL();
	}

void CTestConductor::AddContactL(TInt aBitwiseFilterType)
	{
	test.Printf(_L("Adding Contact"));
	iRandomGenerator->AddTypicalContactForFilterL(aBitwiseFilterType);
	iTotalContacts++;
	}

TContactItemId CTestConductor::CreateContactL(TContactFieldToFill aFld)
	{
	_LIT(KGivenName,"GivenName");
	_LIT(KFamilyName,"FamilyName");

	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,KGivenName,ETrue);
	SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,KFamilyName,ETrue);
	
	switch(aFld)
		{
		case ELocation:
			{
			const TInt rand = Math::Rand(randomSeed) % 3;
			if (rand)
				{
				if (rand == 1)
					{
					SetNameL(*item,iLocationFieldUID,KUidContactFieldVCardMapUnusedN,KAcceptAlways,ETrue);
					++iAcceptCount;
					}
				else
					{
					SetNameL(*item,iLocationFieldUID,KUidContactFieldVCardMapUnusedN,KAskAlways,ETrue);
					++iAskCount;
					}
				}
			}
		break;

		case ENickname:
			SetNameL(*item,iNicknameFieldUID,KUidContactFieldVCardMapUnusedN,KNickName,ETrue);
		break;
		default:
			User::Leave(KErrNotSupported);
		}

	item->SetTemplateRefId(KGoldenTemplateId);
	TContactItemId id=iDb->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item); 
	return id;
	}


void CTestConductor::AddContactsL()
	{
	iTotalContacts=0;
	test.Printf(KContactString);
	TInt i = 0;
	for(; i<KLocation; ++i)
		CreateContactL(ELocation);

	for(i=0; i<KNickname; ++i)
		CreateContactL(ENickname);
	}

void CTestConductor::RestoreFilterableFieldsFromSystemTemplateL()
// 
// Determines which fields in the system template are custom filterable, and sets
// iLocationFieldUID and iNicknameFieldUID to the first two field type UIDs found
//
	{
	CContactTemplate* templ = static_cast<CContactTemplate*>(iDb->ReadContactLC(KGoldenTemplateId));

	CContactItemFieldSet& fieldSet = templ->CardFields();
	const TInt count = fieldSet.Count();
	
	for (TInt i=0; i<count; ++i)
		{
		CContactItemField& field = fieldSet[i];
		if (field.IsCustomFilterable())
			{
			const TFieldType type = field.ContentType().FieldType(0);
						
			if (!field.Label().CompareC(KCustomFilterOne))
				{
				iLocationFieldUID=type;
				}
			else if (!(field.Label()).CompareC(KCustomFilterTwo))
				{
				iNicknameFieldUID=type;
				}
				
			}
		}
	CleanupStack::PopAndDestroy(templ);
	}

void CTestConductor::RunTestsL()
	{
	CViewTester* tester=CViewTester::NewL(*iLog,*iDb,this);
	CleanupStack::PushL(tester);
	CActiveScheduler::Start();
	CleanupStack::Pop(tester);
	// error from tester ?
	User::LeaveIfError(iTestError);
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

CViewTester::~CViewTester()
	{
	iCustom1View->Close(*this);
	iCustom2View->Close(*this);
	iNamedRemoteView->Close(*this);
	iSortOrder_1.Close();
	delete iTextDef;
	}

CViewTester::CViewTester(CLog& aLog,CContactDatabase& aDb,CTestConductor* aTestConductor)
	: CActive(EPriorityStandard),iLog(aLog),iDb(aDb),iCurrentTest(-1),iTestConductor(aTestConductor)
	{
	CActiveScheduler::Add(this);
	}

void CViewTester::ConstructL()
	{
	iSortOrder_1.AppendL(KUidContactFieldGivenName);
	iSortOrder_1.AppendL(KUidContactFieldFamilyName);
	iSortOrder_1.AppendL(iTestConductor->iLocationFieldUID);

	iTextDef=CContactTextDef::NewL();
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName,KTextDefSeparator));
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName,KTextDefSeparator));
	iTextDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName,KTextDefSeparator));

	NextTest();
	}

void CViewTester::PrintTimeTaken(TInt aTime)
	{
	iScratchBuf.Format(_L("**  Test Took [%ds%dus]  **\n"), aTime/1000000, aTime%1000000);
	iLog.LogLine(iScratchBuf);
	/*TBuf<100> buffer;
	buffer.Format(_L("  Test Took [%ds%dus]\n"), aProfile.iTime/1000000, aProfile.iTime%1000000);
	iLog.LogLine(buffer);*/
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

CContactFilteredView* CViewTester::CreateFilterViewL()
	{
	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	return CContactFilteredView::NewL(*this,iDb,*iNamedRemoteView, iFilter);
	}

void CViewTester::RunL()
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iLog.LogLine(_L("=== Create Name Remote local view"));
			CCntTest::ProfileReset(0,1);
			CCntTest::ProfileStart(0);
			iNamedRemoteView=CContactNamedRemoteView::NewL(*this,KRemoteViewName,iDb,iSortOrder_1,EContactsOnly);
			break;
		case EExerciseLocalView:
			iLog.LogLine(_L("=== Exercise local view"));
			ExceriseViewL(*iNamedRemoteView);
			NextTest();
			break;

		case ECreateCustom1View:
			iLog.LogLine(_L("==== Create Location Filter View 1"));
			iFilter = CContactDatabase::ECustomFilter1;
			iCustom1View = CreateFilterViewL();
			break;

		case ECreateCustom2View:
			iLog.LogLine(_L("==== Create Nickname Filter View 2"));
			iFilter = CContactDatabase::ECustomFilter2;
			iCustom2View = CreateFilterViewL();
			break;

		case ETestCustom1:
			{
			iLog.LogLine(_L("==== Exercise Location View"));
			TESTTRUE( iCustom1View->CountL() == iTestConductor->iAcceptCount + iTestConductor->iAskCount);
			TESTTRAP(TestViewIndiciesL(*iCustom1View));

			TESTTRUE(iTestConductor->iAcceptCount > 0);
			TESTTRUE(iTestConductor->iAskCount	  > 0);


			for (TInt i=0; i < iCustom1View->CountL(); ++i)
				{
				const CViewContact& cnt = iCustom1View->ContactAtL(i);
				TPtrC fld = cnt.Field(cnt.FieldCount()-1);

				if (fld.Compare(KAcceptAlways) == 0)
					--iTestConductor->iAcceptCount;
				if (fld.Compare(KAskAlways) == 0)
					--iTestConductor->iAskCount;
				}

			TESTTRUE(iTestConductor->iAcceptCount == 0); //all the contacts with "Accept" setting 
													 //are found in the filterview
			TESTTRUE(iTestConductor->iAskCount	  == 0); //all the contacts with "Ask" setting 
			NextTest();								 //are found in the filterview
			}
			break;

		case ETestCustom2:
			iLog.LogLine(_L("==== Exercise Nickname View"));
			TESTTRUE( iCustom2View->CountL() == KNickname );
			TestViewIndiciesL(*iCustom2View);
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
		case ECreateLocalView: test.Printf(_L("Failed at test step CreateLocalView (%i), with error %i"), iCurrentTest, aError); break;
		case EExerciseLocalView: test.Printf(_L("Failed at test step ExerciseLocalView (%i), with error %i"), iCurrentTest, aError); break;
		case ECreateCustom1View: test.Printf(_L("Failed at test step CreateCustom1View (%i), with error %i"), iCurrentTest, aError); break;
		case ECreateCustom2View: test.Printf(_L("Failed at test step CreateCustom2View (%i), with error %i"), iCurrentTest, aError); break;
		case ETestCustom1: test.Printf(_L("Failed at test step TestCustom1 (%i), with error %i"), iCurrentTest, aError); break;
		case ETestCustom2: test.Printf(_L("Failed at test step TestCustom2 (%i), with error %i"), iCurrentTest, aError); break;
		case ENumTests: test.Printf(_L("Failed at test step NumTests (%i), with error %i"), iCurrentTest, aError); break;

		default: test.Printf(_L("Failed at test step %i, with error %i"), iCurrentTest, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}


void CViewTester::TestViewIndiciesL(const CContactViewBase& aView)
	{
	TContactItemId viewId = NULL, contactId = NULL;
	for (TInt i=0; i < aView.CountL(); ++i)
		{
		TESTTRAP(viewId = aView.AtL(i));
		TESTTRAP(contactId = aView.ContactAtL(i).Id());
		TESTTRUE(contactId == viewId);
		}
	}

void CViewTester::HandleViewCreation(const CContactViewBase& aView, const TContactViewEvent& aEvent, CContactViewBase* aPtrView)
	{
	TESTTRUE(aPtrView==&aView);
	TESTTRUE(aEvent.iEventType==TContactViewEvent::EReady);
	CCntTest::ProfileEnd(0);
	CCntTest::ProfileResult(&iProfile,0,1);
	iTimeForTestsToComplete[iCurrentTest] = iProfile.iTime;
	PrintTimeTaken(iProfile.iTime);
	}


void CViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			HandleViewCreation( aView, aEvent, iNamedRemoteView );
			break;
		case EExerciseLocalView:
			test(ETrue);
			break;

		case ECreateCustom1View:
			HandleViewCreation( aView, aEvent, iCustom1View );
			break;

		case ECreateCustom2View:
			HandleViewCreation( aView, aEvent, iCustom2View );
			break;

		case ENumTests:
		default:
			test(EFalse);
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
	}

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

	TESTTRUE(aView.FindL(lastId)==numItems-1);
	}

void CViewTester::DoCancel()
	{
	}

void doMainL()
	{
	__UHEAP_MARK;
	CTestConductor* testConductor = NULL;
	testConductor = CTestConductor::NewL();
	CleanupStack::PushL( testConductor );
	CleanupStack::PopAndDestroy( testConductor );
	__UHEAP_MARKEND;
	}
//
// Main.
//


/**

@SYMTestCaseID     PIM-T-CUSTFILTER-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-CUSTFILTER-0001 "));

	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,doMainL());
			test(err == KErrNone);
			delete cleanup;
			}
		delete scheduler;
		}
	test(__NB_failures == 0);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }
