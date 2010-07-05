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


#include "t_currentdb.h"

// System includes
#include <e32std.h>
#include <e32twin.h>
#include <e32test.h>
#include <cntdbobs.h>
#include "coreappstest/testserver.h" 

// User includes
#include "t_utils.h"

// Globals
static RTest					TheTest(_L("T_CURRENTDB - Test CntModel current DB support"));
static CTrapCleanup*			TheCleanup;

// Constants (literal)
_LIT(KThreadTestFormat,			"\nThread %d running test %d\n");
_LIT(KThreadTestOkFormat,		"\nThread %d completed test %d OK\n");

// Configure database location
_LIT(KTestDatabaseFile,			"C:Contacts.cdb");
_LIT(KTestDatabaseFile2,		"C:LotsOfContacts.cdb");

// Configure .ini file location
_LIT(KContactsModelIniFile,		"c:\\private\\10003A73\\CntModel.ini");

_LIT(KTestSemaphoreOne,		"T_CURRENTDB_SEMAPHORE_ONE");
_LIT(KTestSemaphoreTwo,		"T_CURRENTDB_SEMAPHORE_TWO");
_LIT(KSpeedDialPhoneNumber, "020 11111111");
_LIT(KSpeedDialPhoneNumber2, "020 22222222");
const TInt KSpeedDialPositionOne = 1;
const TInt KSpeedDialPositionTwo = 2;

const TInt KTestSteps = 100;

////////////////////////////////////////////////////////////////////////////////////
// -------> CTestActiveScheduler (header)
////////////////////////////////////////////////////////////////////////////////////
class CTestActiveScheduler : public CActiveScheduler
	{
////////////////////////////////////////////////////////////////////////////////////
public:
////////////////////////////////////////////////////////////////////////////////////
	void Error (TInt aError) const;
	};


////////////////////////////////////////////////////////////////////////////////////
// -------> CTestActiveScheduler (source)
////////////////////////////////////////////////////////////////////////////////////
void CTestActiveScheduler::Error(TInt aError) const
	{
	User::Panic(_L("AScheduler"),aError);
	}


////////////////////////////////////////////////////////////////////////////////////
// -------> CTestBase (source)
////////////////////////////////////////////////////////////////////////////////////
CTestBase::CTestBase(CTestManager& aTester, TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority)
:	CActive(aPriority), iTestCoordinator(aTester), iClientNumber(aClientNumber), iThreadPriority(aThreadPriority)
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestBase::Complete(TInt aReason)
	{
	// Complete the request with the specified reason.
	__ASSERT_ALWAYS(IsActive() && iStatus.Int() == KRequestPending, User::Invariant());
	TRequestStatus* status = &iStatus;
	iThread.RequestComplete(status, aReason);
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestBase::RunL()
	{
	_LIT(KThreadTestCompleteFormat,		"Thread %d finished tests");

	if	(iWaiting || iWaitingForAnyEvent)
		{
		CActiveScheduler::Stop(); // stop nested scheduler
		iWaiting = iWaitingForAnyEvent = EFalse;
		return;
		}

	// Increment test number
	++iCurrentTest;

	if	(iStatus.Int() < KErrNone) // Can this happen?
		{
		User::Invariant();
		}

	switch(iStatus.Int())
		{
	case EEndTests:
		{
		TBuf<50> buf;
		buf.Format(KThreadTestCompleteFormat, iClientNumber);
		iTest->Next(buf);
		CActiveScheduler::Stop();
		break;
		}
	default:
		{
		// Call the framework to handle the specific test number
		DoTestL(iStatus.Int());

		TBuf<200> buf;
		buf.Format(KThreadTestOkFormat, iClientNumber, iStatus.Int());
		iTest->Printf(buf);

		// Re issue another asynch request
		ReadyForNextTest();
		break;
		}
		}
	}

void CTestBase::DoCancel()
	{
	iWaitTimer.Cancel();

	// Cancel any outstanding request
	if	(!(iWaiting || iWaitingForAnyEvent))
		{
		Complete(KErrCancel);
		}
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

TInt CTestBase::ThreadFunction(TAny* aParam)
	{
	CTestBase* self = STATIC_CAST(CTestBase*, aParam);

	// Prepare the stuff required before we start the
	// active scheduler.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
	if	(!cleanup)
		{
		return KErrNoMemory;
		}

	// Call virtual handler which does anything that needs doing as
	// a result of the thread function from being created.
	TRAPD(err, self->RunTestsL());

    delete cleanup;
	return err;
	}

void CTestBase::RunTestsL()
	{
	User::LeaveIfError(iWaitTimer.CreateLocal());

	OpenSemaphoreL();
	
	HandleThreadCreationL();
	ReadyForNextTest();
	CActiveScheduler::Start();
	HandleThreadDeletionL();

	CloseSemaphore();
	
	iWaitTimer.Cancel();
	iWaitTimer.Close();
	}

TInt CTestBase::HandleThreadCreationL()
	{
	// Create and name an RTest
	TBuf<100> threadName;
	threadName.Format(_L("Test thread %d"), iClientNumber);
	iTest = new(ELeave) RTest(threadName);
	iTest->Start(_L("Starting test"));

	// Position our console window
	TSize size = iTest->Console()->ScreenSize();
	size.iWidth = size.iWidth - 4;
	size.iHeight = (size.iHeight / 2) - 3;

	iConsole = Console::NewL(threadName, size);
	delete (CONST_CAST(RTest*, iTest)->Console());
	iTest->SetConsole(iConsole);
	iConsole->ClearScreen();

	// Create contacts db session
	iTest->Next(_L("Creating contacts test utility"));
	iContactsTest = new(ELeave) CCntTest();
	OpenDatabaseL();

	// Add the CTestBase object
	// to it. Has to be done in this function, otherwise the AO can
	// end up being added to the wrong active scheduler
	CActiveScheduler::Add(this);

	return KErrNone;
	}

void CTestBase::HandleThreadDeletionL()
	{
	iContactsTest->EndTestLib(KErrNone);
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestBase::ReadyForNextTest()
//
//	Requests another test be executed
//
	{
	__ASSERT_ALWAYS(!iWaiting && !iWaitingForAnyEvent, User::Invariant());
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

void CTestBase::WaitForAnyContactsEvent(TInt aTimeInSecondsToWaitFor)
	{
	iWaitingForAnyEvent = ETrue;
	TTimeIntervalMicroSeconds32 time(aTimeInSecondsToWaitFor * 1000000);
	iWaitTimer.After(iStatus, time);
	SetActive();
	CActiveScheduler::Start();
	}

void CTestBase::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	__ASSERT_ALWAYS(iWaiting || iWaitingForAnyEvent, User::Invariant());

	iLastEvent=aEvent;
	switch(aEvent.iType)
		{
	case EContactDbObserverEventNull:
		iTest->Printf(_L("Event: EContactDbObserverEventNull\n"));
		break;
	case EContactDbObserverEventUnused:
		iTest->Printf(_L("Event: EContactDbObserverEventUnused\n"));
		break;
	case EContactDbObserverEventContactChanged:
		iTest->Printf(_L("Event: EContactDbObserverEventContactChanged\n"));
		break;
	case EContactDbObserverEventContactDeleted:
		iTest->Printf(_L("Event: EContactDbObserverEventContactDeleted\n"));
		break;
	case EContactDbObserverEventContactAdded:
		iTest->Printf(_L("Event: EContactDbObserverEventContactAdded\n"));
		break;
	case EContactDbObserverEventUnknownChanges:
		iTest->Printf(_L("Event: EContactDbObserverEventUnknownChanges\n"));
		break;
	case EContactDbObserverEventRecover:
		iTest->Printf(_L("Event: EContactDbObserverEventRecover\n"));
		break;
	case EContactDbObserverEventRollback:
		iTest->Printf(_L("Event: EContactDbObserverEventRollback\n"));
		break;
	case EContactDbObserverEventTablesClosed:
		iTest->Printf(_L("Event: EContactDbObserverEventTablesClosed\n"));
		break;
	case EContactDbObserverEventTablesOpened:
		iTest->Printf(_L("Event: EContactDbObserverEventTablesOpened\n"));
		break;
	case EContactDbObserverEventTemplateChanged:
		iTest->Printf(_L("Event: EContactDbObserverEventTemplateChanged\n"));
		break;
	case EContactDbObserverEventTemplateDeleted:
		iTest->Printf(_L("Event: EContactDbObserverEventTemplateDeleted\n"));
		break;
	case EContactDbObserverEventTemplateAdded:
		iTest->Printf(_L("Event: EContactDbObserverEventTemplateAdded\n"));
		break;
	case EContactDbObserverEventCurrentItemDeleted:
		iTest->Printf(_L("Event: EContactDbObserverEventCurrentItemDeleted\n"));
		break;
	case EContactDbObserverEventCurrentItemChanged:
		(*iTest)(aEvent.iContactId != KNullContactId);
		iTest->Printf(_L("Event: EContactDbObserverEventCurrentItemChanged\n"));
		break;
	case EContactDbObserverEventOwnCardChanged:
		iTest->Printf(_L("Event: EContactDbObserverEventOwnCardChanged\n"));
		break;
	case EContactDbObserverEventPreferredTemplateChanged:
		iTest->Printf(_L("Event: EContactDbObserverEventPreferredTemplateChanged\n"));
		break;
	case EContactDbObserverEventOwnCardDeleted:
		iTest->Printf(_L("Event: EContactDbObserverEventOwnCardDeleted\n"));
		break;
	case EContactDbObserverEventGroupAdded:
		iTest->Printf(_L("Event: EContactDbObserverEventGroupAdded\n"));
		break;
	case EContactDbObserverEventGroupChanged:
		iTest->Printf(_L("Event: EContactDbObserverEventGroupChanged\n"));
		break;
	case EContactDbObserverEventGroupDeleted:
		iTest->Printf(_L("Event: EContactDbObserverEventGroupDeleted\n"));
		break;
	case EContactDbObserverEventCurrentDatabaseChanged:
		iTest->Printf(_L("Event: EContactDbObserverEventCurrentDatabaseChanged\n"));
		break;
	case EContactDbObserverEventSpeedDialsChanged:
		iTest->Printf(_L("Event: EContactDbObserverEventSpeedDialsChanged\n"));
		break;
	default:
		iTest->Printf(_L("Unhandled Event\n"));
		User::Invariant();
		}

	if	((aEvent.iType == iEvent) || iWaitingForAnyEvent)
		{
		Cancel();
		iWaiting = iWaitingForAnyEvent = EFalse;
		CActiveScheduler::Stop();
		return;
		}
	else if (aEvent.iType != iEvent)
		{
			iTest->Printf(_L("Got an Unexpected Event.\n"));
		}
		

	}

void CTestBase::OpenSemaphoreL()
	{
	TInt error = iSemaphoreOne.CreateGlobal(KTestSemaphoreOne(), 0);
	if (error == KErrAlreadyExists || error == KErrNone)
		{
		iSemaphoreOne.OpenGlobal(KTestSemaphoreOne());
		}
	else
		{
		User::Leave(error);
		}
			
	error = iSemaphoreTwo.CreateGlobal(KTestSemaphoreTwo(), 0);
	if (error == KErrAlreadyExists || error == KErrNone)
		{
		iSemaphoreTwo.OpenGlobal(KTestSemaphoreTwo());
		}
	else
		{
		User::Leave(error);
		}		
	}

void CTestBase::CloseSemaphore()
	{
	iSemaphoreOne.Close();
	iSemaphoreTwo.Close();
	}


////////////////////////////////////////////////////////////////////////////////////
// -------> CTestThreadOne (source)
////////////////////////////////////////////////////////////////////////////////////
CTestThreadOne::CTestThreadOne(CTestManager& aTester, TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority)
:	CTestBase(aTester, aClientNumber, aThreadPriority, aPriority)
	{
	}

CTestThreadOne*	CTestThreadOne::NewL(TInt aClientNumber, const TDesC& aName, CTestManager& aTester, TThreadPriority aThreadPriority, TInt aPriority)
	{
	CTestThreadOne* self = new(ELeave) CTestThreadOne(aTester, aClientNumber, aThreadPriority, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(); // self
	return self;
	}

void CTestThreadOne::OpenDatabaseL()
	{
	TBuf<200> buf;
	buf.Format(_L("Constructing test utilities with database set as %S"), &KTestDatabaseFile);
	iTest->Next(buf);
	iContactsTest->ConstructL(*iTest, KTestDatabaseFile);
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestThreadOne::DoTestL(TInt aTestNumber)
	{
	TBuf<50> buf;
	buf.Format(KThreadTestFormat, iClientNumber, aTestNumber);
	iTest->Next(buf);

	switch(aTestNumber)
		{
	case ETest1:
		DoTest1L();
		break;
	case ETest2:
		DoTest2L();
		break;
	case ETest3:
		DoTest3L();
		break;
	case ETest4:
		DoTest4L();
		break;
	case ETest5:
		DoTest5L();
		break;
	case ETest6:
		DoTest6L();
		break;
	case ETest7:
		DoTest7L();
		break;
	case ETest8:
		DoTest8L();
		break;
	case ETest9:
 		DoTest9L();
 		break; 
	default:
		User::Invariant();
		}
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestThreadOne::DoTest1L()
	{
	// This thread has a higher priority than thread 2 so it should run first,
	// hence this test should work.
	iSemaphoreOne.Wait();
	
	iTest->Next(_L("Opening the database and check the current db is null\n"));
	iContactsTest->OpenDatabaseL();

	iTest->Printf(_L("Fetching the current database...\n"));
	TFileName currentDb;
	User::LeaveIfError(iContactsTest->Db()->GetCurrentDatabase(currentDb));
	iTest->Printf(_L("Current database reported as %S\n"), &currentDb);
	(*iTest)(currentDb.CompareF(KNullDesC) == 0);

	// Because there was no ini file found, we now set this database to be the
	// current.
	iTest->Printf(_L("\tSetting thread one's database as the current db\n"));
	User::LeaveIfError(iContactsTest->Db()->SetCurrentDatabase(KTestDatabaseFile));

	// Check that it is the current db now
	User::LeaveIfError(iContactsTest->Db()->GetCurrentDatabase(currentDb));
	iTest->Printf(_L("Current database reported as %S\n"), &currentDb);
	(*iTest)(currentDb.CompareF(KTestDatabaseFile) == 0);
	iSemaphoreTwo.Signal();
	
	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	}

void CTestThreadOne::DoTest2L()
	{
	iSemaphoreOne.Wait();
	iSemaphoreTwo.Signal();
	iTest->Next(_L("\tWaiting for thread 2 to change current db\n"));
	// Thread two is going to change the current database to 'it's' db.
	// Wait for notification...
	
	// !! Why on Earth do we need one of these? Talk about bad design.
	CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*iContactsTest->Db(), this);
	CleanupStack::PushL(changeNotifier);
	WaitForContactsEvent(EContactDbObserverEventCurrentDatabaseChanged);
	CleanupStack::PopAndDestroy(); // changeNotifier
	iTest->Printf(_L("\tCurrent database change was reported correctly\n"));
	
	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	}

void CTestThreadOne::DoTest3L()
	{
 	iSemaphoreOne.Wait();

	iTest->Next(_L("Setting the current db a lot of times\n"));
	for(TInt i = 0; i < KTestSteps; ++i)
		{
		// Open and close our database setting it as the default
		iTest->Printf(_L("\tClosing thread one's database\n"));
		iContactsTest->CloseDatabase();

		iTest->Printf(_L("\tOpening thread one's database\n"));
		iContactsTest->OpenDatabaseL();

		// Set our db
		iTest->Printf(_L("\tSetting thread one's database as the current db\n"));
		User::LeaveIfError(iContactsTest->Db()->SetCurrentDatabase(KTestDatabaseFile));
		
		// Pause this thread
		iSemaphoreTwo.Signal();
		iSemaphoreOne.Wait();
		}

	iSemaphoreOne.Wait();
	// Check that the current db is KTestDatabaseFile2 (because thread two has just closed the
	// previous current db, which was KTestDatabaseFile2).
	TFileName currentDb;
	User::LeaveIfError(iContactsTest->Db()->GetCurrentDatabase(currentDb));
	iTest->Printf(_L("Current database reported as %S\n"), &currentDb);
	(*iTest)(currentDb.CompareF(KTestDatabaseFile2) == 0);	

	iTest->Printf(_L("\tClosing thread one's database\n"));
	iContactsTest->CloseDatabase();

	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	}

void CTestThreadOne::DoTest4L()
	{
	iSemaphoreOne.Wait();
	
	iTest->Next(_L("Checking that after closing all sessions, the current database is remembered\n"));

	iTest->Printf(_L("\tOpening thread one's database\n"));
	iContactsTest->OpenDatabaseL();

	iTest->Printf(_L("\tClosing thread one's database\n"));
	iContactsTest->CloseDatabase();

	iTest->Printf(_L("\tDeleting CntModel.ini and checking server still able to cope\n"));
	// Secure Server waits 5s from last connection
	// to write the .ini file and close
	User::After(6000000);

    RPIMTestServer serv;
    User::LeaveIfError(serv.Connect());
    TRAPD(err,serv.DeleteFileL(KContactsModelIniFile));
    if (err != KErrNotFound)
		{
		User::LeaveIfError(err);
		}
    serv.Close();

	iTest->Printf(_L("\tAttempt to re-open thread one's database\n"));
	iContactsTest->OpenDatabaseL();

	iTest->Printf(_L("\tCheck that the current database is null in this instance\n"));
	TFileName currentDb;
	User::LeaveIfError(iContactsTest->Db()->GetCurrentDatabase(currentDb));
	iTest->Printf(_L("\tCurrent database reported as %S\n"), &currentDb);

	iTest->Printf(_L("\tSet the current db again\n"));
	User::LeaveIfError(iContactsTest->Db()->SetCurrentDatabase(KTestDatabaseFile));

	iTest->Printf(_L("\tWait for change notification\n"));
	iTest->Printf(_L("\tClosing thread one's database\n"));
	iContactsTest->CloseDatabase();

	iTest->Printf(_L("\tOpening thread one's database again to check correct restoration of current db\n"));
	iContactsTest->OpenDatabaseL();

	User::LeaveIfError(iContactsTest->Db()->GetCurrentDatabase(currentDb));
	iTest->Printf(_L("Current database reported as %S\n"), &currentDb);
	(*iTest)(currentDb.CompareF(KTestDatabaseFile) == 0);	

	iTest->Printf(_L("\tClosing thread one's database\n"));
	iContactsTest->CloseDatabase();
	
	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	}


/**
 * Speeddial test
 * Open contacts database
 * Wait for notification that the other client has opened the database 
 * Add a new speeddial
 * Wait for confirmation the update was received
 */
void CTestThreadOne::DoTest5L()
	{
	iSemaphoreOne.Wait();
	
	iContactsTest->OpenDatabaseL();

	iTest->Printf(_L("we've both got the database open"));

	// Add a contact and set the speeddial
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("Given"), ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, KSpeedDialPhoneNumber, ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, _L("020 22222222"), ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapCELL, _L("07747 065000"), ETrue);
	TContactItemId id = iContactsTest->Db()->AddNewContactL(*item);
	iContactsTest->Db()->SetCurrentItem(id);
	CleanupStack::PopAndDestroy(item);

	// Set speeddial
	TInt fieldId=1;
	const CContactIdArray* sortedItems = iContactsTest->Db()->SortedItemsL();

	// First item in position 1
	CContactItem* firstItem = iContactsTest->Db()->OpenContactL((*sortedItems)[0]);
	CleanupStack::PushL(firstItem);
	iContactsTest->Db()->SetFieldAsSpeedDialL(*firstItem, fieldId, KSpeedDialPositionOne);
	CleanupStack::PopAndDestroy(firstItem); 
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	
	TBuf<32> phoneNumber;
	TContactItemId id2 = iContactsTest->Db()->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KSpeedDialPhoneNumber);
	TheTest(id2 == id);
	iTest->Printf(_L("ThreadOne : set contact with speed dial at position 1, awaiting notification.\n"));

	iContactsTest->Db()->DeleteContactL(id);
	iContactsTest->Db()->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	iTest->Printf(_L("ThreadOne : deleted contact, waiting for notification\n"));
	
	iContactsTest->CloseDatabase();
	
	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	}

/**
 * Speeddial test
 * Open contacts database
 * Wait for notification that the other client has opened the database 
 * Remove a speeddial
 * Wait for confirmation the update was received
 */
void CTestThreadOne::DoTest6L()
	{
	iSemaphoreOne.Wait();
	
	iContactsTest->OpenDatabaseL();
	iTest->Printf(_L("we've both got the database open"));

	// Add a contact and set the speeddial
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("Given"), ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, KSpeedDialPhoneNumber, ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, _L("020 22222222"), ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapCELL, _L("07747 065000"), ETrue);
	TContactItemId id = iContactsTest->Db()->AddNewContactL(*item);
	iContactsTest->Db()->SetCurrentItem(id);
	CleanupStack::PopAndDestroy(item);

	// Set speeddial
	TInt fieldId=1;
	const CContactIdArray* sortedItems = iContactsTest->Db()->SortedItemsL();

	// First item in position 1
	CContactItem* firstItem = iContactsTest->Db()->OpenContactL((*sortedItems)[0]);
	CleanupStack::PushL(firstItem);
	iContactsTest->Db()->SetFieldAsSpeedDialL(*firstItem, fieldId, KSpeedDialPositionOne);
	CleanupStack::PopAndDestroy(firstItem); 

	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	
	TBuf<32> phoneNumber;
	TContactItemId id2= iContactsTest->Db()->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KSpeedDialPhoneNumber);
	TheTest(id2==id);
	iTest->Printf(_L("ThreadOne : set contact with speed dial at position 1, awaiting notification.\n"));

	iTest->Printf(_L("ThreadOne : removing speed dial that was just added.... "));
	iContactsTest->Db()->RemoveSpeedDialFieldL(id, KSpeedDialPositionOne);
	iContactsTest->Db()->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	iTest->Printf(_L("done\n"));

	iSemaphoreTwo.Signal();
	iContactsTest->CloseDatabase();
	
	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	}

/**
 * Group add test
 * Open contacts database
 * Wait for notification that the other client has opened the database
 * Add a new group
 * Wait for confirmation the update was received
 * Delete the new group
 * Wait for confirmation the update was received
 */
void CTestThreadOne::DoTest7L()
	{
	iSemaphoreOne.Wait();
	
	iContactsTest->OpenDatabaseL();
	iTest->Printf(_L("we've both got the database open"));

	CContactItem* tempGroup = iContactsTest->Db()->CreateContactGroupL(_L("NEW GROUP"));
       delete tempGroup;
 
	CContactIdArray* groupIdList = iContactsTest->Db()->GetGroupIdListL();
	CleanupStack::PushL(groupIdList);
	iContactsTest->Db()->DeleteContactL((*groupIdList)[0]);
	CleanupStack::PopAndDestroy(groupIdList);
 	iContactsTest->CloseDatabase();
	iSemaphoreTwo.Signal();
	
	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	}

void CTestThreadOne::DoTest8L()
	{
	iSemaphoreOne.Wait();
	
	iContactsTest->OpenDatabaseL();
	iTest->Printf(_L("we've both got the database open"));

	// Add a contact and set the speeddial
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("Given"), ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, KSpeedDialPhoneNumber, ETrue);
	TContactItemId id = iContactsTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item);

	// Set speeddial -  Case:1
	TInt fieldId=1;
	CContactItem* firstItem = iContactsTest->Db()->OpenContactL(id);
	CleanupStack::PushL(firstItem);
	iContactsTest->Db()->SetFieldAsSpeedDialL(*firstItem, fieldId, KSpeedDialPositionOne);
	CleanupStack::PopAndDestroy(firstItem); 

	TBuf<32> phoneNumber;
	TContactItemId id2= iContactsTest->Db()->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KSpeedDialPhoneNumber);
	TheTest(id2==id);
	iTest->Printf(_L("ThreadOne : set contact with speed dial at position 1, awaiting notification.\n"));
	
	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	
	// changing the name to see the type of notifications emitted.  - Case:2
	CContactItem* editItem = iContactsTest->Db()->OpenContactL(id);
	CleanupStack::PushL(editItem);
	SetNameL(*editItem, KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("changed"), EFalse);
	iContactsTest->Db()->CommitContactL(*editItem);
	CleanupStack::PopAndDestroy(editItem);
	iTest->Printf(_L("ThreadOne : Changing name of contact at position 1, awaiting notification.\n"));

	// changing the name & phone number, to see the type of notifications emitted.  - Case:3
	editItem = iContactsTest->Db()->OpenContactL(id);
	CleanupStack::PushL(editItem);
	SetNameL(*editItem, KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("changed"), EFalse);
	SetNameL(*editItem, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, _L("020 88888888"), EFalse);
	iContactsTest->Db()->CommitContactL(*editItem);
	CleanupStack::PopAndDestroy(editItem);
	iTest->Printf(_L("ThreadOne : Changing name & phone no. of contact at position 1, awaiting notification.\n"));
	
	iContactsTest->Db()->DeleteContactL(id);
	iTest->Printf(_L("ThreadOne : deleted contact, waiting for notification\n"));

	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	
	//add 2 contacts, set them to speed dail, change details & commit, and check notifications,
	// & delete - Case:4
	item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("Contact-1"), ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, KSpeedDialPhoneNumber, ETrue);
	id = iContactsTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item);
	
	item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("Contact-2"), ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, KSpeedDialPhoneNumber2, ETrue);
	id2 = iContactsTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item);
	
	// speeddial for the both the contacts,
	firstItem = iContactsTest->Db()->OpenContactL(id);
	CleanupStack::PushL(firstItem);
	iContactsTest->Db()->SetFieldAsSpeedDialL(*firstItem, fieldId, KSpeedDialPositionOne);
	CleanupStack::PopAndDestroy(firstItem); 
	
	CContactItem* secondItem = iContactsTest->Db()->OpenContactL(id2);
	CleanupStack::PushL(secondItem);
	iContactsTest->Db()->SetFieldAsSpeedDialL(*secondItem, fieldId, KSpeedDialPositionTwo);
	CleanupStack::PopAndDestroy(secondItem); 
	
	//change details, for second contact 1st and then the first contact.
	editItem = iContactsTest->Db()->OpenContactL(id2);
	CleanupStack::PushL(editItem);
	SetNameL(*editItem, KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("changed second"), EFalse);
	SetNameL(*editItem, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, _L("020 77777777"), EFalse);
	iContactsTest->Db()->CommitContactL(*editItem);
	CleanupStack::PopAndDestroy(editItem);
	
	editItem = iContactsTest->Db()->OpenContactL(id);
	CleanupStack::PushL(editItem);
	SetNameL(*editItem, KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("changed first"), EFalse);
	SetNameL(*editItem, KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, _L("020 66666666"), EFalse);
	iContactsTest->Db()->CommitContactL(*editItem);
	CleanupStack::PopAndDestroy(editItem);
	
	iContactsTest->Db()->DeleteContactL(id);
	iContactsTest->Db()->DeleteContactL(id2);
	iTest->Printf(_L("ThreadOne : Added 2 contacts, set speed dail for both, changed contact details for both & deleted both the contacts, waiting for notification\n"));
	
	//Case:5, Add contact, set speed dail, remove speed dail, delete contact
	// Add a contact and set the speeddial
	item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName, KUidContactFieldVCardMapUnusedN, _L("Given"), ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber, KUidContactFieldVCardMapTEL, KSpeedDialPhoneNumber, ETrue);
	id = iContactsTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item);

	// Set speeddial
	firstItem = iContactsTest->Db()->OpenContactL(id);
	CleanupStack::PushL(firstItem);
	iContactsTest->Db()->SetFieldAsSpeedDialL(*firstItem, fieldId, KSpeedDialPositionOne);
	CleanupStack::PopAndDestroy(firstItem);
	
	iSemaphoreTwo.Signal();
	
	// Remove speed dail
	id2= iContactsTest->Db()->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KSpeedDialPhoneNumber);
	TheTest(id2==id);
	
	iContactsTest->Db()->RemoveSpeedDialFieldL(id, KSpeedDialPositionOne);
	iContactsTest->Db()->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KNullDesC);
	iContactsTest->Db()->DeleteContactL(id); 
	iTest->Printf(_L("ThreadOne : Add contacts, set speed dail, remove speed dial, and delete, waiting for notification\n"));

	iContactsTest->CloseDatabase();
	
	// Pause this thread
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	}

/*
Teststep to test fix for INC116664
1)Add contact items to database
2)Add contact template to database
3)Delete contact template from database
Result:Contact template should be deleted without any panic.
*/	

void CTestThreadOne::DoTest9L()
	{
	iSemaphoreOne.Wait();
	
	iContactsTest->OpenDatabaseL();
	iTest->Printf(_L("we've both got the database open"));
	
	iTest->Printf(_L("Add new contact data\n"));
	
	CContactDatabase* db = iContactsTest->Db();
	
	TContactItemId id = AddContactL(db, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, _L("3333"));
	iTest->Printf(_L("Contact Added with Id = %d\n"), id);
	
	id =  AddContactL(db, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, _L("2222"));
	iTest->Printf(_L("Contact Added with Id = %d\n"), id);
	
	id =  AddContactL(db, KUidContactFieldFamilyName, KUidContactFieldVCardMapUnusedN, _L("1111"));
	iTest->Printf(_L("Contact Added with Id = %d\n"), id);
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	
	CContactItem* contactTemplate = iContactsTest->Db()->CreateContactCardTemplateLC(_L("TestINC116664"));
	id = contactTemplate->Id();
	iTest->Printf(_L("Contact Template Created with Id = %d\n"),id);
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();

	iContactsTest->Db()->AddNewContactL(*contactTemplate);
	iSemaphoreTwo.Signal();
	iSemaphoreOne.Wait();
	CleanupStack::PopAndDestroy(contactTemplate);	
	
	iTest->Printf(_L("Deleting Contact Template now\n"));
	iContactsTest->Db()->DeleteContactL(id);
	iSemaphoreTwo.Signal();
	
	iContactsTest->CloseDatabase();
	}	

////////////////////////////////////////////////////////////////////////////////////
// -------> CTestThreadTwo (source)
////////////////////////////////////////////////////////////////////////////////////
CTestThreadTwo::CTestThreadTwo(CTestManager& aTester, TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority)
:	CTestBase(aTester, aClientNumber, aThreadPriority, aPriority)
	{
	}

CTestThreadTwo*	CTestThreadTwo::NewL(TInt aClientNumber, const TDesC& aName, CTestManager& aTester, TThreadPriority aThreadPriority, TInt aPriority)
	{
	CTestThreadTwo* self = new(ELeave) CTestThreadTwo(aTester, aClientNumber, aThreadPriority, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	CleanupStack::Pop(); // self
	return self;
	}

void CTestThreadTwo::OpenDatabaseL()
	{
	TBuf<200> buf;
	buf.Format(_L("Constructing test utilities with database set as %S"), &KTestDatabaseFile);
	iTest->Next(buf);
	iContactsTest->ConstructL(*iTest, KTestDatabaseFile2);
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestThreadTwo::DoTestL(TInt aTestNumber)
	{
	TBuf<50> buf;
	buf.Format(KThreadTestFormat, iClientNumber, aTestNumber);
	iTest->Next(buf);

	switch(aTestNumber)
		{
	case ETest1:
		DoTest1L();
		break;
	case ETest2:
		DoTest2L();
		break;
	case ETest3:
		DoTest3L();
		break;
	case ETest4:
		DoTest4L();
		break;
	case ETest5:
		DoTest5L();
		break;
	case ETest6:
		DoTest6L();
		break;
	case ETest7:
		DoTest7L();
		break;
	case ETest8:
		DoTest8L();
		break;
	case ETest9:
 		DoTest9L();
 		break;
	default:
		User::Invariant();
		}
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestThreadTwo::DoTest1L()
	{
	iSemaphoreOne.Signal();
	// This thread has a lower priority than thread 1 so it should run second,
	// hence this test should work.
	iSemaphoreTwo.Wait();
	iTest->Next(_L("Opening the database and check it is not made the current db\n"));
	iContactsTest->OpenDatabaseL();

	iTest->Printf(_L("\tFetching the current database...\n"));
	TFileName currentDb;
	User::LeaveIfError(iContactsTest->Db()->GetCurrentDatabase(currentDb));
	iTest->Printf(_L("\tCurrent database reported as %S\n"), &currentDb);

	// The current database should still be the normal 'contacts.cdb'
	(*iTest)(currentDb.CompareF(KTestDatabaseFile) == 0);
	
	// Pause this thread
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	}

void CTestThreadTwo::DoTest2L()
	{
	iSemaphoreOne.Signal();
	
	iTest->Next(_L("Setting the current database and check..."));

	// Set our database as the current (it was thread 1's)...
	User::LeaveIfError(iContactsTest->Db()->SetCurrentDatabase(KTestDatabaseFile2));

	// Check it's been changed...
	iTest->Printf(_L("\tFetching the current database...\n"));
	TFileName currentDb;
	User::LeaveIfError(iContactsTest->Db()->GetCurrentDatabase(currentDb));
	iTest->Printf(_L("\tCurrent database reported as %S\n"), &currentDb);
	(*iTest)(currentDb.CompareF(KTestDatabaseFile2) == 0);
	
	// Pause this thread
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	}

void CTestThreadTwo::DoTest3L()
	{
	iSemaphoreOne.Signal();
	
	iTest->Next(_L("Waiting for thread 1 to set the current db a lot of times\n"));
	for(TInt i = 0; i < KTestSteps; ++i)
		{
		CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*iContactsTest->Db(), this);
		CleanupStack::PushL(changeNotifier);
		
		// This will wait for it
		iTest->Printf(_L("\tWaiting for change notification...\n"));
		WaitForContactsEvent(EContactDbObserverEventCurrentDatabaseChanged);
		iTest->Printf(_L("\tNotification complete...\n"));
		CleanupStack::PopAndDestroy(); // changeNotifier
		
		iSemaphoreOne.Signal();
		iSemaphoreTwo.Wait();
		}
	
	// Set our database as the current (it was thread 1's)...
	User::LeaveIfError(iContactsTest->Db()->SetCurrentDatabase(KTestDatabaseFile2));

	// Now close our db
	iTest->Printf(_L("\tClosing thread two's database\n"));
	iContactsTest->CloseDatabase();
	iSemaphoreOne.Signal();
	
	// Pause this thread
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	}

void CTestThreadTwo::DoTest4L()
	{
	iSemaphoreOne.Signal();
	iTest->Next(_L("Thread two, simple wait for notification to aid thread one\n"));
	
	iTest->Printf(_L("\tOpening thread two's database\n"));
	iContactsTest->OpenDatabaseL();

	// This will wait notification
	CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*iContactsTest->Db(), this);
	CleanupStack::PushL(changeNotifier);
	iTest->Printf(_L("\tWaiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventCurrentDatabaseChanged);
	iTest->Printf(_L("\tNotification complete...\n"));
	CleanupStack::PopAndDestroy(); // changeNotifier

	// Close our db
	iTest->Printf(_L("\tClosing thread two's database\n"));
	iContactsTest->CloseDatabase();
	
	// Pause this thread
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	}


/**
 * Speeddial test
 * Open thread one's contacts database
 * Wait for notification that the other client added a speeddial
 * Check that it is possible to read the speeddial
 */
void CTestThreadTwo::DoTest5L()
	{	
	iTest->Printf(_L("\tOpening thread one's database\n"));
	CContactDatabase* threadTwoDb = iContactsTest->Db()->OpenL(KTestDatabaseFile);
	CleanupStack::PushL(threadTwoDb);

	// This will wait notification
	CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*threadTwoDb, this);
	CleanupStack::PushL(changeNotifier);
	iSemaphoreOne.Signal(); // indicate we've got the same database open
	iSemaphoreTwo.Wait();
	iSemaphoreTwo.Wait();
	
	iTest->Printf(_L("\tThreadTwo : Waiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventContactAdded);
	WaitForContactsEvent(EContactDbObserverEventCurrentItemChanged);
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);	
	iTest->Printf(_L("\tThreadTwo : We expect a speed dial to have been added... "));
	TheTest(iLastEvent.iContactId==1);
	iTest->Printf(_L("correct.\n"));
		
	TBuf<32> phoneNumber;
	TContactItemId id = threadTwoDb->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber == KSpeedDialPhoneNumber);
	TheTest(id == 1);

	iSemaphoreOne.Signal();
	
	iTest->Printf(_L("\tThreadTwo : Waiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	iTest->Printf(_L("\tThreadTwo : We expect the contact owning the speed dial to have been deleted... "));
	TheTest(iLastEvent.iContactId == id); // Speed dial has been deleted by CTestThreadOne::DoTest5L(), 
									    //event returns ID on deletion
	iTest->Printf(_L("\n\tSpeed dial contact has been deleted by CTestThreadOne::DoTest5L(),.\n"));
	iTest->Printf(_L("correct.\n"));
	CleanupStack::PopAndDestroy(); // changeNotifier

	id = threadTwoDb->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber == KNullDesC);
	TheTest(id==-1);

	CleanupStack::PopAndDestroy();
	
	// Signal thread 1
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	}


/**
 * Speeddial test
 * Open thread one's contacts database
 * Wait for notification that the other client removed a speeddial
 * Check that it is possible to read the speeddial
 */
void CTestThreadTwo::DoTest6L()
	{
	iTest->Printf(_L("\tOpening thread one's database\n"));
	CContactDatabase* threadOneDb = iContactsTest->Db()->OpenL(KTestDatabaseFile);
	CleanupStack::PushL(threadOneDb);

	// This will wait notification
	CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*threadOneDb, this);
	CleanupStack::PushL(changeNotifier);
	
	iSemaphoreOne.Signal(); // indicate we've got the same database open
	iSemaphoreTwo.Wait();
	
	iTest->Printf(_L("\tWaiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventContactAdded);
	WaitForContactsEvent(EContactDbObserverEventCurrentItemChanged);
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);	
	iTest->Printf(_L("\tSpeedDial Notification complete\n"));
	TheTest(iLastEvent.iContactId==2); // we deleted contact 1
		
	TBuf<32> phoneNumber;
	TContactItemId id = threadOneDb->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KSpeedDialPhoneNumber);
	TheTest(id==2);

	iSemaphoreOne.Signal(); // indicate we've got the same database open
	iSemaphoreTwo.Wait();
	
	WaitForContactsEvent(EContactDbObserverEventContactChanged);
	TheTest(iLastEvent.iContactId==id);	//speed dial has been removed by CTestThreadOne::DoTest6L(), 
										// event returns ID on removal
	iTest->Printf(_L("\n\tspeed dial has been removed by CTestThreadOne::DoTest6L(), \n"));
	CleanupStack::PopAndDestroy(); // changeNotifier

	id = threadOneDb->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KNullDesC);
	TheTest(id==-1);

	CleanupStack::PopAndDestroy(); // threadOneDb
	
	// Pause this thread
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	}

/**
 * Group test
 * Open thread one's contacts database
 * Wait for notification that the other client added a contact group
 * and acknowledge the same
 * Wait for notification that the other client deleted a contact group
 * and acknowledge the same
 */
void CTestThreadTwo::DoTest7L()
	{
	iTest->Printf(_L("\tOpening thread one's database\n"));
	CContactDatabase* threadOneDb = iContactsTest->Db()->OpenL(KTestDatabaseFile);
	CleanupStack::PushL(threadOneDb);

	// This will wait notification
	CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*threadOneDb, this);
	CleanupStack::PushL(changeNotifier);
	
	iSemaphoreOne.Signal(); // indicate we've got the same database open
	iSemaphoreTwo.Wait();
	

	WaitForContactsEvent(EContactDbObserverEventGroupAdded);
       TheTest(threadOneDb->GroupCount()==1);

	WaitForContactsEvent(EContactDbObserverEventGroupDeleted);
       TheTest(threadOneDb->GroupCount()==0);

	CleanupStack::PopAndDestroy(2); // threadOneDb, changeNotifier
	
	// Signal thread 1
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	}

void CTestThreadTwo::DoTest8L()
	{
	iTest->Printf(_L("\tOpening thread one's database\n"));
	CContactDatabase* threadOneDb = iContactsTest->Db()->OpenL(KTestDatabaseFile);
	CleanupStack::PushL(threadOneDb);

	// This will wait notification
	CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*threadOneDb, this);
	CleanupStack::PushL(changeNotifier);
	
	iSemaphoreOne.Signal(); // indicate we've got the same database open
	iSemaphoreTwo.Wait();
		
	iTest->Printf(_L("\tThreadTwo : Waiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventContactAdded);
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	iTest->Printf(_L("\tThreadTwo : We expect a speed dial to have been added... "));
	TheTest(iLastEvent.iContactId == 4);
	iTest->Printf(_L("correct.\n"));
		
	TBuf<32> phoneNumber;
	TContactItemId id = threadOneDb->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KSpeedDialPhoneNumber);
	TheTest(id == 4);

	// Pause this thread
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	
	iTest->Printf(_L("\tThreadTwo : Waiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	WaitForContactsEvent(EContactDbObserverEventContactChanged);  
	iTest->Printf(_L("\tThreadTwo : We expect a contact having speed dial to have been modified... "));
	TheTest(iLastEvent.iContactId == 4);
	iTest->Printf(_L("correct.\n"));
	
	//changed both name & phone no. of contact with speed dial
	iTest->Printf(_L("\tThreadTwo : Waiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	WaitForContactsEvent(EContactDbObserverEventContactChanged);  
	iTest->Printf(_L("\tThreadTwo : Both contact having speed dial have been modified ... "));
	TheTest(iLastEvent.iContactId == 4);
	iTest->Printf(_L("correct.\n"));
	
	iTest->Printf(_L("\tThreadTwo : Waiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	WaitForContactsEvent(EContactDbObserverEventContactDeleted);
	iTest->Printf(_L("\tThreadTwo : We expect the contact owning the speed dial to have been deleted... "));
	TheTest(iLastEvent.iContactId==id); // Speed dial has been deleted by CTestThreadOne::DoTest5L(), 
									    //event returns ID on deletion
	iTest->Printf(_L("\n\tSpeed dial contact has been deleted by CTestThreadOne::DoTest8L(),.\n"));
	id = threadOneDb->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KNullDesC);
	TheTest(id==-1);
	iTest->Printf(_L("correct.\n"));
	
	// Pause this thread
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	
	//case-4
	iTest->Printf(_L("\tThreadTwo : Waiting for change notification for case-4...\n"));
	WaitForContactsEvent(EContactDbObserverEventContactAdded);
	TheTest(iLastEvent.iContactId==5);
	WaitForContactsEvent(EContactDbObserverEventContactAdded);
	iTest->Printf(_L("\tThreadTwo : We expect a 2 contacts to have been added... "));
	TheTest(iLastEvent.iContactId==6);
	iTest->Printf(_L("correct.\n"));
	
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	TheTest(iLastEvent.iContactId==5);
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	iTest->Printf(_L("\tThreadTwo : We expect a speed dial to have been set for both the contacts... "));
	TheTest(iLastEvent.iContactId==6);
	iTest->Printf(_L("correct.\n"));
	
	//changed name & phone no. of both the contact with speed dial
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	WaitForContactsEvent(EContactDbObserverEventContactChanged);  
	TheTest(iLastEvent.iContactId == 6);
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	WaitForContactsEvent(EContactDbObserverEventContactChanged);  
	iTest->Printf(_L("\tThreadTwo : Both contacts having speed dial have been modified ... "));
	TheTest(iLastEvent.iContactId == 5);
	iTest->Printf(_L("correct.\n"));
	
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	WaitForContactsEvent(EContactDbObserverEventContactDeleted);
	TheTest(iLastEvent.iContactId==5); // Speed dial has been deleted 
	iTest->Printf(_L("\tSpeed dial for contact-1 has been deleted by CTestThreadOne::DoTest8L(),.\n"));
	id = threadOneDb->GetSpeedDialFieldL(KSpeedDialPositionOne, phoneNumber);
	TheTest(phoneNumber==KNullDesC);
	TheTest(id==-1);
	iTest->Printf(_L("correct.\n"));
	
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	WaitForContactsEvent(EContactDbObserverEventContactDeleted);
	id = threadOneDb->GetSpeedDialFieldL(KSpeedDialPositionTwo, phoneNumber);
	TheTest(iLastEvent.iContactId==6); // Speed dial has been deleted 
	iTest->Printf(_L("\tSpeed dial for contact-2 has been deleted by CTestThreadOne::DoTest8L(),.\n"));
	id = threadOneDb->GetSpeedDialFieldL(KSpeedDialPositionTwo, phoneNumber);
	TheTest(phoneNumber==KNullDesC);
	TheTest(id==-1);
	iTest->Printf(_L("correct.\n"));
	iTest->Printf(_L("\tThreadTwo : We expect the contact owning the speed dial to have been deleted... "));
	
	iTest->Printf(_L("\tThreadTwo : Waiting for change notification...\n"));
	WaitForContactsEvent(EContactDbObserverEventContactAdded);
	WaitForContactsEvent(EContactDbObserverEventSpeedDialsChanged);
	WaitForContactsEvent(EContactDbObserverEventContactChanged);
	WaitForContactsEvent(EContactDbObserverEventContactDeleted);
	iTest->Printf(_L("\tThreadTwo : We expect a speed dial to have been added and removed... "));
	TheTest(iLastEvent.iContactId == 7);
	iTest->Printf(_L("correct.\n"));
	
	CleanupStack::PopAndDestroy(); // changeNotifier,
	CleanupStack::PopAndDestroy(); // threadOneDb
	
	// Signal thread 1
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	}
	

/*
Teststep to test fix for INC116664
1)Open database and wait for notifications
Result: Notifications for contact and template addition along with template deletion should be recieved.
*/
void CTestThreadTwo::DoTest9L()
	{
	iTest->Printf(_L("\tOpening thread one's database\n"));
	CContactDatabase* threadOneDb = iContactsTest->Db()->OpenL(KTestDatabaseFile);
	CleanupStack::PushL(threadOneDb);

	// This will wait notification
	CContactChangeNotifier* changeNotifier = CContactChangeNotifier::NewL(*threadOneDb, this);
	CleanupStack::PushL(changeNotifier);
	iSemaphoreOne.Signal(); // indicate we've got the same database open
	iSemaphoreTwo.Wait();
	
	WaitForContactsEvent(EContactDbObserverEventContactAdded);

	WaitForContactsEvent(EContactDbObserverEventContactAdded);

	WaitForContactsEvent(EContactDbObserverEventContactAdded);
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
	
	WaitForContactsEvent(EContactDbObserverEventTemplateAdded);
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();
    
	WaitForContactsEvent(EContactDbObserverEventTemplateAdded);
	iSemaphoreOne.Signal();
	iSemaphoreTwo.Wait();

	WaitForContactsEvent(EContactDbObserverEventTemplateDeleted);
	iSemaphoreOne.Signal();
	
	CleanupStack::PopAndDestroy(2); // threadOneDb, changeNotifier
	}

////////////////////////////////////////////////////////////////////////////////////
// -------> CTestManager (source)
////////////////////////////////////////////////////////////////////////////////////
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

	iClient2 = CTestThreadTwo::NewL(2, _L("Client-2"), *this, EPriorityLess); // lower
	iClient1 = CTestThreadOne::NewL(1, _L("Client-1"), *this, EPriorityMore);

	IssueTimerRequest();
	CActiveScheduler::Start();
	}

CTestManager* CTestManager::NewL()
	{
	CTestManager* self = NewLC();
	CleanupStack::Pop(); // self
	return self;
	}

CTestManager* CTestManager::NewLC()
	{
	CTestManager* self = new (ELeave) CTestManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestManager::RunL()
	{
	if	(  iClient1->IsActive() && iClient2->IsActive() 
		&& iClient1->CurrentTest() == iClient2->CurrentTest()
		&& !(iClient1->Waiting() || iClient2->Waiting())
		)
		// We can only complete a request if the active objects are ready. This is
		// determined using IsActive()
		{
		if	(iTestNumber >= CTestBase::EEndTests)
			// Have we run all the tests?
			{
			TRequestStatus thread1, thread2;

			// Log on to the threads so that we'll be notified when they die
			// Must do this before telling threads to end!
			iClient1->Thread().Logon(thread1);
			iClient2->Thread().Logon(thread2);

			iClient1->Complete(CTestBase::EEndTests);
			iClient2->Complete(CTestBase::EEndTests);
			
			// Wait for them both to die...
			User::WaitForRequest(thread2);
			User::WaitForRequest(thread1);
			
			CActiveScheduler::Stop();
			return; // prevent from starting timer again
			}
		
		_LIT(KNextTestFormatTitle, "Test %d");
		TBuf<20> testBuf;
		testBuf.Format(KNextTestFormatTitle, iTestNumber);
		
		TheTest.Next(_L(" "));

		iClient1->Complete(iTestNumber);
		iClient2->Complete(iTestNumber);

		// We've just requested a test be completed, so we have to increment this
		// so that it's ready for the next test...
		iTestNumber++;
		}

	// Re-issue the asynchronous request to the CTimer
	IssueTimerRequest();
	}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void CTestManager::IssueTimerRequest()
	{
	const TInt KOneSecond = 1000000;
	After(KOneSecond);
	}


////////////////////////////////////////////////////////////////////////////////////
// -------> Static global functions (source)
////////////////////////////////////////////////////////////////////////////////////
static void doMainL()
	{
	CTestActiveScheduler*  scheduler = new (ELeave) CTestActiveScheduler;
	CleanupStack::PushL(scheduler);                                 
	CActiveScheduler::Install(scheduler);

    RPIMTestServer serv;
    User::LeaveIfError(serv.Connect());
    CleanupClosePushL(serv);
    
  	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);

	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KTestDatabaseFile2, EFileTypeCnt);

	// Create two contacts databases - ignore return value
	CContactDatabase* db1 = CContactDatabase::ReplaceL(KTestDatabaseFile);
	delete db1;
	CContactDatabase* db2 = CContactDatabase::ReplaceL(KTestDatabaseFile2);
	delete db2;
	
	// Secure Server waits 5s from last connection
	// to write the .ini file and close
	User::After(6000000);

	// Delete any existing ini file so that we can be sure this test is ok
	serv.DeleteFileL(KContactsModelIniFile);
    serv.Close();

	CTestManager::NewLC();
	CleanupStack::PopAndDestroy(5, scheduler); // tester, TempFiles, fsSession
	}


/**

@SYMTestCaseID     PIM-T-CURRENTDB-0001

*/

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	TheTest.Start(_L("@SYMTESTCaseID:PIM-T-CURRENTDB-0001 Multi session testcode"));

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
