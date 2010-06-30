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
*
*/


#include "t_casesensitivedb.h"

// System includes
#include <e32std.h>
#include <e32test.h>
#include <cntdbobs.h>

#include <cntitem.h>
#include <cntfldst.h>

#include "coreappstest/testserver.h" 


// Globals
static RTest					TheTest(_L("T_CASESENSITIVEDB - Test CntModel support"));
static CTrapCleanup*			TheCleanup;

// Constants (literal)

// Configure database location
_LIT(KTestDatabaseFileU,		"C:Contacts.cdb");
_LIT(KTestDatabaseFileL,		"C:contacts.cdb");

// Configure .ini file location
_LIT(KContactsModelIniFile,		"C:\\private\\10003A73\\CntModel.ini");


_LIT(KTestSemaphoreName,		"T_CASESENSITIVEDB_SEMAPHORE");

_LIT(KNewContactName,"Test Name");

class CTestActiveScheduler : public CActiveScheduler
	{
public:
	void Error (TInt aError) const;
	};

void CTestActiveScheduler::Error(TInt aError) const
	{
	User::Panic(_L("AScheduler"),aError);
	}

CTestBase::CTestBase(TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority)
:	CActive(aPriority), 
	iClientNumber(aClientNumber), 
	iThreadPriority(aThreadPriority)
	{
	}

CTestBase::~CTestBase()
	{
	Cancel();
	iThread.Close();
	}

void CTestBase::ConstructL(const TDesC& aName)
	{
	User::LeaveIfError(iThread.Create(aName, ThreadFunction, KDefaultStackSize, 0x2000, 0x20000, this, EOwnerThread));
	iThread.Resume();
	iThread.SetPriority(iThreadPriority);
	}

void CTestBase::Complete(TInt aReason)
	{
	// Complete the request with the specified reason.
	__ASSERT_ALWAYS(IsActive() && iStatus.Int() == KRequestPending, User::Invariant());
	TRequestStatus* status = &iStatus;
	iThread.RequestComplete(status, aReason);
	}

void CTestBase::RunL()
	{
	if(!iTestFinished)
		{
		if(!iWaiting)
			{
			DoTestL();
			CActiveScheduler::Stop();	
			}
		else
			{
			//if test hasnt finished and is waiting for notification then it must have 
			//failed as timer on waiting for notification has expired.
			iTest->Printf(_L("ERROR:No notification received"));
			(*iTest)(EFalse);
			}
		}
	}

void CTestBase::DoCancel()
	{
	iWaitTimer.Cancel();

	// Cancel any outstanding request
	if	(!(iWaiting))
		Complete(KErrCancel);
	}

TInt CTestBase::ThreadFunction(TAny* aParam)
	{
	CTestBase* self = STATIC_CAST(CTestBase*, aParam);

	// Prepare the stuff required before we start the
	// active scheduler.
    CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;

	// Call virtual handler which does anything that needs doing as
	// a result of the thread function from being created.
	TRAPD(err, self->RunTestsL());

    delete cleanup;
	return err;
	}

void CTestBase::RunTestsL()
	{
	User::LeaveIfError(iWaitTimer.CreateLocal());

	TBuf<100> threadName;
	threadName.Format(_L("Test thread %d"), iClientNumber);
	iTest = new(ELeave) RTest(threadName);
	iTest->Start(threadName);
	iTest->Printf(_L("Starting Test thread %d \n"), iClientNumber);
	//create Active Schedular for this thread
	CActiveScheduler::Install(new(ELeave) CActiveScheduler);
	
	// Create contacts db session
	OpenDatabaseL();

	// Add the CTestBase object
	// to it. Has to be done in this function, otherwise the AO can
	// end up being added to the wrong active scheduler
	CActiveScheduler::Add(this);

	//Set request status 
	SetRequestStatus();

	CActiveScheduler::Start();

	//close thread
	iTest->Printf(_L("\tClosing thread %d \n"), iClientNumber);
	iTest->End();
	iTest->Close();

	delete iDb;
	delete CActiveScheduler::Current();

	iWaitTimer.Cancel();
	iWaitTimer.Close();
	
	//Setting this flag should be the last thing that happens
	iTestFinished = ETrue;
	}

//	Requests another test be executed
void CTestBase::SetRequestStatus()
	{
	__ASSERT_ALWAYS(!iWaiting, User::Invariant());
	SetActive();
	iStatus = KRequestPending;
	}

void CTestBase::WaitForContactsEvent(TContactDbObserverEventType aEvent, TInt aTimeInSecondsToWaitFor)
	{
	iWaiting = ETrue;
	iEvent = aEvent;
	TTimeIntervalMicroSeconds32 time(aTimeInSecondsToWaitFor * 1000000);

	iWaitTimer.After(iStatus, time);
	SetActive();
	CActiveScheduler::Start();
	}

void CTestBase::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	__ASSERT_ALWAYS(iWaiting, User::Invariant());

	switch(aEvent.iType)
		{
	case EContactDbObserverEventContactAdded:
		iTest->Printf(_L("Event: EContactDbObserverEventContactAdded\n"));
		break;
	default:
		iTest->Printf(_L("Unhandled Event\n"));
		}

	if	(aEvent.iType == iEvent)
		{
		Cancel();
		iWaiting = EFalse;
		CActiveScheduler::Stop();
		return;
		}
	}

CTestThreadOne::CTestThreadOne(TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority)
:	CTestBase(aClientNumber, aThreadPriority, aPriority)
	{
	}

CTestThreadOne*	CTestThreadOne::NewL(TInt aClientNumber, const TDesC& aName, TThreadPriority aThreadPriority, TInt aPriority)
	{
	CTestThreadOne* self = new(ELeave) CTestThreadOne(aClientNumber, aThreadPriority, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(self);
	return self;
	}

void CTestThreadOne::OpenDatabaseL()
	{
	TBuf<200> buf;
	buf.Format(_L("Opening database %S for thread 1 \n"), &KTestDatabaseFileU);
	iTest->Printf(buf);
	iDb = CContactDatabase::OpenL(KTestDatabaseFileU);
	}

LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName)
//
// Set the contents of a text field, creating the field if required
//
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	}

void CTestThreadOne::DoTestL()
	{
	RSemaphore semaphore;
	TInt semCreateErr = semaphore.CreateGlobal(KTestSemaphoreName, 0);
	if (semCreateErr == KErrAlreadyExists)
		{
		User::LeaveIfError(semaphore.OpenGlobal(KTestSemaphoreName));
		}
	else
		{
		User::LeaveIfError(semCreateErr);		
		}
	CleanupClosePushL(semaphore);
	
	// Pause this thread while thread 2 sets up change notifier
	semaphore.Wait();
	
	iTest->Printf(_L("Add new contact data\n"));

	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,KNewContactName);
	iDb->AddNewContactL(*item);	
	
	// Pause this thread while thread 2 checks for update
	semaphore.Wait();
	
	CleanupStack::PopAndDestroy(2,&semaphore); // item, semaphore
	}

CTestThreadTwo::CTestThreadTwo(TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority)
:	CTestBase(aClientNumber, aThreadPriority, aPriority)
	{
	}

CTestThreadTwo*	CTestThreadTwo::NewL(TInt aClientNumber, const TDesC& aName, TThreadPriority aThreadPriority, TInt aPriority)
	{
	CTestThreadTwo* self = new(ELeave) CTestThreadTwo(aClientNumber, aThreadPriority, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(self);
	return self;
	}

void CTestThreadTwo::OpenDatabaseL()
	{
	TBuf<200> buf;
	buf.Format(_L("Opening database %S for thread 2 \n"), &KTestDatabaseFileL);
	iTest->Printf(buf);
	iDb = CContactDatabase::OpenL(KTestDatabaseFileL);
	}

void CTestThreadTwo::DoTestL()
	{
	RSemaphore semaphore;
	TInt semCreateErr = semaphore.CreateGlobal(KTestSemaphoreName, 0);
	if (semCreateErr == KErrAlreadyExists)
		{
		User::LeaveIfError(semaphore.OpenGlobal(KTestSemaphoreName));
		}
	else
		{
		User::LeaveIfError(semCreateErr);		
		}
	CleanupClosePushL(semaphore);
	
	iTest->Printf(_L("Setting up thread 2 change notifier\n"));

	CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*iDb, this);
	CleanupStack::PushL(changeNotifier);

	// Wait for change notifier to start
	User::After(2000000);
	
	// Signal thread 1
	semaphore.Signal();

		// This will wait for it
	iTest->Printf(_L("\tWaiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventContactAdded);
	iTest->Printf(_L("\tNotification complete...\n"));
	CleanupStack::PopAndDestroy(changeNotifier); // changeNotifier
	
	// Signal thread 1
	semaphore.Signal();
	
	CleanupStack::PopAndDestroy(&semaphore); // semaphore
	}

CTestManager::CTestManager(TInt aPriority)
:	CTimer(aPriority)
	{
	CActiveScheduler::Add(this);
	}

CTestManager::~CTestManager()
	{
	Cancel();
	delete iClient1;
	delete iClient2;
	}

void CTestManager::ConstructL()
	{
	CTimer::ConstructL();

	iClient2 = CTestThreadTwo::NewL(2, _L("Client-2"), EPriorityLess); // lower
	iClient1 = CTestThreadOne::NewL(1, _L("Client-1"), EPriorityMore);
	IssueTimerRequest();
	}

CTestManager* CTestManager::NewL()
	{
	CTestManager* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CTestManager* CTestManager::NewLC()
	{
	CTestManager* self = new (ELeave) CTestManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTestManager::RunL()
	{

	if	( iClient1->IsActive() && iClient2->IsActive() 
		&& !(iClient1->Waiting() || iClient2->Waiting()))
		{
		iClient1->Complete(1);
		iClient2->Complete(1);
		}	
	if ( iClient1->TestFinished() && iClient2->TestFinished()) 
		{				
		//tests are finished so stop active schedular
		CActiveScheduler::Stop();
		return; // prevent from starting timer again
		}
		
	// Re-issue the asynchronous request to the CTimer
	IssueTimerRequest();
	}

void CTestManager::IssueTimerRequest()
	{
	const TInt KOneSecond = 1000000;
	After(KOneSecond);
	}

static void doMainL()
	{
	CTestActiveScheduler*  scheduler = new (ELeave) CTestActiveScheduler;
	CleanupStack::PushL(scheduler);                                 
	CActiveScheduler::Install(scheduler);

	// Replace contacts, using two styles of capitalisation
	CContactDatabase* db;

	db = CContactDatabase::ReplaceL(KTestDatabaseFileU);
	delete db;
	db = NULL;

	db = CContactDatabase::ReplaceL(KTestDatabaseFileL);
	delete db;
	db = NULL;
	
	// Secure Server waits 5s from last connection
	// to write the .ini file and close
	User::After(6000000);
	// Delete any existing ini file so that we can be sure this test is ok
    RPIMTestServer serv;
    User::LeaveIfError(serv.Connect());
	serv.DeleteFileL(KContactsModelIniFile);
    serv.Close();

	// create test manager
	CTestManager::NewLC();
	// run tests
	CActiveScheduler::Start();
	// clean up
	CleanupStack::PopAndDestroy(2,scheduler); // scheduler, tester
	}


/**

@SYMTestCaseID     PIM-T-CASESENSITIVEDB-0001

*/

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	TheTest.Start(_L("@SYMTESTCaseID:PIM-T-CASESENSITIVEDB-0001 Multi session case sensitive testcode"));

	TheTest.Title();
	TheCleanup = CTrapCleanup::New();
	TRAPD(ret, doMainL());	
	TheTest(ret == KErrNone);
	delete TheCleanup;	

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

