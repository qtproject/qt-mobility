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
#include <e32math.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include <cntview.h>
#include <cntviewbase.h>

#include "t_groupviewmodifiedcontact.h"

// Test Macro
LOCAL_D RTest test(_L("T_GROUPVIEWMODIFIEDCONTACT"));

//
// Constants.
//
_LIT(KTestName,"t_groupviewmodifiedcontact");
_LIT(KLogFileName,"t_groupviewmodifiedcontact.log");

_LIT(KDbFileName,"c:t_groupview.cdb");

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
	}


void CTestConductor::RunTestsL()
	{
	CGroupViewTester* tester=CGroupViewTester::NewL(*iLog, this, *iDb);
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

CGroupViewTester* CGroupViewTester::NewL(CLog& aLog, CTestConductor* aTestConductor, CContactDatabase& aDb)
	{
	CGroupViewTester* self=new(ELeave) CGroupViewTester(aLog, aTestConductor, aDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CGroupViewTester::~CGroupViewTester()
	{
	iGroupViewOne->Close(*this);

	iLocalView->Close(*this);
	iSortOrder_1.Close();

	delete iGroupOne;
	}

CGroupViewTester::CGroupViewTester(CLog& aLog, CTestConductor* aTestConductor, CContactDatabase& aDb)
	: CActive(EPriorityStandard),iLog(aLog),iTestConductor(aTestConductor),iDb(aDb),iCurrentTest(-1)
	{
	CActiveScheduler::Add(this);
	}

void CGroupViewTester::ConstructL()
	{
	iSortOrder_1.AppendL(KUidContactFieldGivenName);
	iSortOrder_1.AppendL(KUidContactFieldFamilyName);
	iSortOrder_1.AppendL(KUidContactFieldCompanyName);
	NextTest();
	}


void CGroupViewTester::RunL()
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iLog.LogLine(_L("=== Create local view"));
			iLocalView=CContactLocalView::NewL(*this,iDb,iSortOrder_1,EContactAndGroups/*EContactsOnly*/);
			break;

		case ECreateGroupOne:
			iLog.LogLine(_L("==== Create Group \n"));
			iGroupOne = STATIC_CAST(CContactGroup*,iDb.CreateContactGroupL(KGroupOneName));
			iGroupOneId = iGroupOne->Id();
			delete iGroupOne;
			iGroupOne = NULL;
			iGroupOne = STATIC_CAST(CContactGroup*,iDb.ReadContactL(iGroupOneId));

			break;

		case ECreateGroupOneView:
			{
			iLog.LogLine(_L("=== GroupOneView"));
			iGroupViewOne=CContactGroupView::NewL(iDb,*iLocalView,*this,iGroupOne->Id(),CContactGroupView::EShowContactsInGroup);
			}
			break;


		case EAddContact:
			{
			iLog.LogLine(_L("==== Add Contact \n"));
			CContactTemplate* templateContact=STATIC_CAST(CContactTemplate*,iDb.ReadContactL(iDb.TemplateId()));
			CleanupStack::PushL(templateContact);
			CTestContact* contact=CTestContact::NewLC(*templateContact);

			contact->SetFirstNameL(_L("FirstName"));

			contact->SetLastNameL(_L("LastName"));

			contact->SetCompanyNameL(_L("CompanyName"));

			contact->SetWorkPhoneL(_L("0123456789"));

			contact->SetWorkMobileL(_L("0223456789"));

			contact->SetHomePhoneL(_L("0323456789"));

			contact->SetWorkEmailAddressL(_L("work@email.com"));

			iSingleContactId = iDb.AddNewContactL(contact->ContactItem());

			CleanupStack::PopAndDestroy(contact);
			CleanupStack::PopAndDestroy(templateContact);
			iNumNotificationExpected = 2; // Once for the total view, one for the group view.
			}

		break;

		case EAddContactToGroupOne: // since we've only one contact anyway - we can be sure it's id will be 1.
			iLog.LogLine(_L("==== Assign contact to Group \n"));
			iDb.AddContactToGroupL(iSingleContactId,iGroupOne->Id());
			iNumNotificationExpected = 6;
		break;

		case EModifyContactInGroupOne:
			{
			iLog.LogLine(_L("==== Modify Contact \n"));
			CContactItem* contactToModify = iDb.OpenContactL(iSingleContactId);
			CleanupStack::PushL(contactToModify);
			CTestContact* testContactToModify = CTestContact::NewLC(*contactToModify);

			CContactItemFieldSet& fields = contactToModify->CardFields();
			CContactItemField& lastNameField = fields[fields.Find(KUidContactFieldFamilyName)];
			CContactTextField* lastNameTextField = static_cast<CContactTextField*>(lastNameField.Storage());
			lastNameTextField->SetText(_L("modified_lastname").AllocL());

			iDb.CommitContactL(*contactToModify);
			iDb.CloseContactL(1);
			CleanupStack::PopAndDestroy(testContactToModify);
			CleanupStack::PopAndDestroy(contactToModify);
			iNumNotificationExpected = 4;
			}

		break;

		case ENumTests:
			iLog.LogLine(_L("==== Group View Modified Contact Tests Finished, All Passed...\n"));
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
		case ECreateGroupOne: test.Printf(_L("Test failed at step CreateGroupOne (%i) with error %i"), iCurrentTest, aError); break;
		case ECreateGroupOneView: test.Printf(_L("Test failed at step CreateGroupOneView (%i) with error %i"), iCurrentTest, aError); break;
		case EAddContact: test.Printf(_L("Test failed at step AddContact (%i) with error %i"), iCurrentTest, aError); break;
		case EAddContactToGroupOne: test.Printf(_L("Test failed at step AddContactToGroupOne (%i) with error %i"), iCurrentTest, aError); break;
		case EModifyContactInGroupOne: test.Printf(_L("Test failed at step ModifyContactInGroupOne (%i) with error %i"), iCurrentTest, aError); break;

		case ENumTests: test.Printf(_L("Test failed at step NumTests (%i) with error %i"), iCurrentTest, aError); break;

		default: test.Printf(_L("Test failed at step %i with error %i"), iCurrentTest, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}

void CGroupViewTester::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	switch (iCurrentTest)
		{
		case ECreateLocalView:
			iLog.LogLine(_L("==== Handling the local view creation \n"));
			test(iLocalView==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
			break;

		case ECreateGroupOne:
			iLog.LogLine(_L("==== Handling the group creation \n"));
		break;

		case ECreateGroupOneView:
			iLog.LogLine(_L("==== Handling the group view creation \n"));
			test(iGroupViewOne==&aView);
			test(aEvent.iEventType==TContactViewEvent::EReady);
		break;

		case EAddContactToGroupOne:
			iLog.LogLine(_L("==== Handling the addition of a contact to the group \n"));
		break;

		case EAddContact:
			iLog.LogLine(_L("==== Handling the addition of a contact \n"));
		break;

		case EModifyContactInGroupOne:
		iLog.LogLine(_L("==== Handling Contact being modified. \n"));

			switch (aEvent.iEventType)
			{
				case TContactViewEvent::EItemRemoved:
					iLog.LogLine(_L("==== Contact Is removed for modify \n"));
				break;

				case TContactViewEvent::EItemAdded:
					iLog.LogLine(_L("==== Contact Is added for modify \n"));
					{
					TInt numberOfEntries = 0;
					TRAPD(err, numberOfEntries = aView.CountL() );
					test(err == KErrNone && numberOfEntries != 0);
					}
				break;
				default:
					break;
			}
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

void CGroupViewTester::NextTest()
	{
	++iCurrentTest;
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}






void CGroupViewTester::DoCancel()
	{
	}

//
// Main.
//

/**

@SYMTestCaseID     PIM-T-GROUPVIEWMODIFIEDCONTACT-0001

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
			test.Start(_L("@SYMTESTCaseID:PIM-T-GROUPVIEWMODIFIEDCONTACT-0001 t_groupviewmodifiedcontact"));

			TRAPD(err,testConductor=CTestConductor::NewL());
			test(err == KErrNone);
			test.End();
			test.Close();
			delete testConductor;
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
