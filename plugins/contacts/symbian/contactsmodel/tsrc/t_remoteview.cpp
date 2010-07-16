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
* T_RemoteView creates a database in the main thread. Another thread is created containing
* a remote view to the same database.  A number of contacts are added, then some deleted. 
* Meanwhile whenever the view is updated and available, the second thread prints all the
* contact IDs to the console using an active object, one at a time (a nicely time-consuming
* process intended to cause a backlog of messages between the thread). 
* After the contacts are added, a number of them are deleted at once.
* Before the changes for INC037352 , this would Panic. Now it executes without any errors. 
* Note: This overwrites the default contacts database:  c:\system\data\Contacts.cdb
* This is necessary if this is to be used with phonebook server which requires this file
*
*/

#include <cntdb.h>
#include <cntdef.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>

// System includes
#include <e32std.h>
#include <e32twin.h>
#include <e32test.h>
#include <cntdbobs.h>

// User includes
#include "t_utils2.h"

// Constants

_LIT(KThreadNameOne, "OneThread");

_LIT(KViewNameOne, "T_RemoteView-ViewOne");

const TInt KNumberOfContacts = 300;
const TInt KNumberOfContactsForTest2 = 300;
const TInt KNumberToDeleteAtOneTime = 45;

static RTest TheTest(_L("T_RemoteView"));

///////////////////////////////////////////////////////////////////////////////
//#pragma mark ==== Class definitions ====
///////////////////////////////////////////////////////////////////////////////

/** 
This class recieves view events, prints them, and then prints all the
contacts in the view. If a view changes while printing, it will stop, 
starting again from the top when the view becomes available again
*/
class CViewObsever : public CActive, public MContactViewObserver
	{
	public: 
		CViewObsever(RTest& aTest);
		void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);

	protected: 
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);

	private:
		void StartPrintingAllContacts(const CContactViewBase& aView);
		TBool iReady;
		RTest& iTest;
		TInt iCount;
		TInt iIndex;
		const CContactViewBase* iView;
	};

/** This class is used to create a named remote view  in its own thread, displaying the contents
	 in a console */
class CViewer : public CBase
	{
	public: 
		static TThreadId Create(const TDesC& aName, const TDesC& aViewName);
		static TInt LaunchThread(TAny* aAny);
		static CViewer* NewL(const TDesC& aViewName);
		static TInt CheckFinished(TAny* self);
		~CViewer();

	private: 
		void ConstructL(const TDesC& aViewName);
		TBool IsDone();

	private:
		RTest* iTest;
		CContactDatabase* iDatabase;
		RContactViewSortOrder iViewSortOrder;
		CViewObsever* iEventObserver;
		CContactNamedRemoteView* iView;
		RSemaphore iSemaphore;
		CIdle* iSemaphoreWatcher;
	};


//#pragma mark -

CViewObsever::CViewObsever(RTest& aTest) : CActive(EPriorityStandard+4), iTest(aTest)
	{
	}

/** Tell to print all contacts in the view (using RunL) */
void CViewObsever::StartPrintingAllContacts(const CContactViewBase& aView) 
	{
	ASSERT(iReady);
	
	if(IsActive())
		{
		Cancel();		
		}
	
	iView = &aView;
	TRAPD(err, iCount = aView.CountL() );
	if(err!=KErrNone)
		{
		RunError(err);
		return;
		}
	iIndex=iCount;
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat,KErrNone);
	SetActive();
	}


/** Print any error. If it's simply not ready, ignore it (but end the printing cycle) */
/** And, if it's not found ... do the same ... this happens when the test system is slow
either due to other processes running or the speed of the processor itself.  If the test
in question is Test2L, the attempt to gain a count may have indicated the view cannot be
found. */
TInt CViewObsever::RunError(TInt aError)
	{
	iTest.Printf(_L("Error: %i\n"),aError);
	if(aError == KErrNotReady | aError == KErrNotFound)		
	// view is not ready or count returned not found
		{	
		/* Can only get here by a Leave in AtL() (or by being called manually an equvalent Leaver in CountL() in StartPrintingAllContacts() ) 
			This means the object thinks the state is ready, but it really is not .
			This can *only* ever happen if an EUnavailable event got sent,
			but it's not been recieved yet. 
			So, to save the trouble set the readiness to false and just sit and wait
			for the EUnavailable and (eventually) the EReady events
		*/
		iReady=EFalse;
		return KErrNone;
		}
	else if(aError == KErrNotFound)
		{
		return KErrNone;
		}
	return aError;
	}

/** Print the next item in the view*/
void CViewObsever::RunL()
	{
	if(iReady && --iIndex > (iCount-10))
		{
		iView->AtL(iIndex);
		
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat,KErrNone);
		SetActive();
		}
	iTest.Printf(_L("\n"));
	}

/** Stop printing */
void CViewObsever::DoCancel()
	{
	iIndex = iCount;
	iTest.Printf(_L("Cancelling view observer\n"));
	}

/** Print the current readiness state and the incoming event.  If the view is ready, start printing the list of contacts*/
void CViewObsever::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	TPtrC name(RThread().Name());
	if(iReady) 
		{
		iTest.Printf(_L("%S Ready: "), &name);
		}
	else 
		{
		iTest.Printf(_L("%S Not ready: "), &name);
		}
	switch(aEvent.iEventType)
		{
		case TContactViewEvent::EUnavailable: 
			iReady = EFalse;
			iTest.Printf(_L("EUnavailable\n"));
			break;
		case TContactViewEvent::EReady:
			iReady = ETrue;
			iTest.Printf(_L("EReady\n"));
			break;
		case TContactViewEvent::ESortOrderChanged:
			iTest.Printf(_L("ESortOrderChanged\n"));
			break;
		case TContactViewEvent::ESortError:
		case TContactViewEvent::EServerError:
		case TContactViewEvent::EIndexingError:
			iTest.Printf(_L("Error: %x %i\n"),aEvent.iEventType, aEvent.iInt);
			iReady = EFalse;
			break;
		case TContactViewEvent::EItemAdded:
			iTest.Printf(_L("EItemAdded %i (%i)\n"), aEvent.iContactId, aEvent.iInt);
			break;
		case TContactViewEvent::EItemRemoved:
			iTest.Printf(_L("EItemRemoved\n"));
			break;
		case TContactViewEvent::EGroupChanged:
			iTest.Printf(_L("EGroupChanged\n"));
			break;
		default:
			return;	
		}
	
	if(iReady)
		{
		StartPrintingAllContacts(aView);
		}
	}
	
//#pragma mark -

CViewer* CViewer::NewL(const TDesC& aViewName)
	{
	CViewer* self = new(ELeave) CViewer;
	CleanupStack::PushL(self);
	self->ConstructL(aViewName);
	CleanupStack::Pop(self);
	return(self);
	}

CViewer::~CViewer()
	{
	TPtrC name(RThread().Name());
	iTest->Printf(_L("Deleting viewer: %S\n"), &name);
	iView->Close(*iEventObserver);
	delete iEventObserver;
	iViewSortOrder.Close();
	
	delete iDatabase;
	iSemaphore.Close();
	iTest->End();
	iTest->Close();
	delete iTest;
	delete iSemaphoreWatcher;
	CActiveScheduler::Stop();
	}

void CViewer::ConstructL( const TDesC& aViewName)
	{

	TPtrC name(RThread().Name());

	// Create and name an RTest
	iTest = new(ELeave) RTest(name);
	CConsoleBase* console = iTest->Console();
	// Position our console window
	if(console) 
		{
		TSize size = console->ScreenSize();
		size.iWidth = size.iWidth /2 -3;
		console = Console::NewL(name, size);
		}
	else
		{
		TSize size(40,10);
		console = Console::NewL(name, size);
		}
	delete const_cast<RTest*>(iTest)->Console();
	iTest->SetConsole(console);
	console->SetTitle(name);
	iTest->Start(_L("Thread: Starting Viewer"));
	
	iTest->Next(_L("Thread: Creating view sort order"));
	iViewSortOrder.AppendL(KUidContactFieldFamilyName);
   	iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

	iTest->Next(_L("Thread: Creating view observer"));
	iEventObserver = new(ELeave) CViewObsever(*iTest);
	
	User::LeaveIfError(iSemaphore.OpenGlobal(aViewName));

	User::After(2000000);		// give the other thread a chance to do stuff
	TInt err=KErrNotFound;
	TInt count=0;
	while(err!=KErrNone)
		{
		if(count) 
			{
			iTest->Printf(_L("  retry %i\n"),count);
			}
		else
			{
			iTest->Next(_L("Thread: Starting Database"));
			}
		TRAP(err, iDatabase = CContactDatabase::OpenL());
		count++;
		if(count>30)
			{
			User::Leave(err);
			}
		}

	CActiveScheduler::Add(iEventObserver);
	
	iTest->Next(_L("Thread: Starting View"));
	iView = CContactNamedRemoteView::NewL(*iEventObserver, aViewName, *iDatabase, iViewSortOrder, EContactsOnly);

	iSemaphore.Signal(); // main thread can start doing things
	iSemaphoreWatcher = CIdle::NewL(CActive::EPriorityIdle-100);			// really very idle
	TCallBack callback(CViewer::CheckFinished,this); 
	iSemaphoreWatcher->Start(callback);
	}

TBool CViewer::IsDone()
	{
		return ETrue;
	}

TInt CViewer::CheckFinished(TAny* self)
	{
	if( ((CViewer*) self)->IsDone() )
		{
		delete  ((CViewer*) self);
		return false;
		}
	return true;
	}

static CViewer* InitL(TDesC& aViewName)
	{
	CActiveScheduler *scheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	return CViewer::NewL(aViewName);
	}

TInt CViewer::LaunchThread(TAny* aAny) 
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanUpStack=CTrapCleanup::New();
	if (!cleanUpStack)
		{
		return KErrNoMemory;
		}
	TRAPD(r,InitL(*((TDesC*) aAny)));
	if (r==KErrNone)
		{
		User::After(2000000);		// give the other thread a chance to do stuff
		CActiveScheduler::Start();
		User::After(2000000);		// give the other thread a chance to do stuff
		}
	delete CActiveScheduler::Current();
	delete cleanUpStack;
	
	__UHEAP_MARKEND;
	return r;
	}

/* This is the only thing one needs to call in order to set the view in motion. */
/** Creates a remote view in its own thread. 
@param aName			the name of the thread
@param aViewName	the name of the view*/
TThreadId CViewer::Create(const TDesC& aName, const TDesC& aViewName)
	{
	RThread thread;
	TInt err = thread.Create( aName, CViewer::LaunchThread, KDefaultStackSize, 0x2000, 0x20000, (void*) &aViewName, EOwnerThread );
	if(err!=KErrNone)
		{
		return KNullUidValue;
		}
	thread.Resume();
	thread.SetPriority(EPriorityNormal);
	TThreadId id = thread.Id();
	thread.Close();
	return id;
	}



///////////////////////////////////////////////////////////////////////////////
//#pragma mark -
///////////////////////////////////////////////////////////////////////////////

/** This object creates and closes a thread containing a view to the contacts database. 
	The view will output all changes and the current state of the database using an RDebug object. 
	This thread acts completely independently until it is told to close. 
	@see CViewObsever

	Call Open() to create initailise the other thread. WaitUntilDbIsReady() or Close() must be called eventually. 
	if CheckIfDbIsReady() returns true, WaitUntilDbIsReady() will return immediately. 
	Close() is synchronous and will not return until the other thread dies. 
*/
class RRemoteViewThread
	{
	public:
		TInt Open(const TDesC& aViewName, const TDesC& aThreadName);
		void WaitUntilDbIsReady(); 
		TBool CheckIfDbIsReady();
		TInt Close();

	private:
		RSemaphore iSemaphore;
		TBool iDbReady;
		TRequestStatus iStatus;
	};

/** Create and initalise the view thread. 
	There is no reason why the view name and thread name cannot be the same.
	This call is more or less asynchronous, and the thread will not be immediately ready.
	Call CheckIfDbIsReady() or WaitUntilDbIsReady() to find out if the thread is ready.
	This can be closed at any time, even if the thread is not ready.
@param	aViewName	The name of the view
@param aThreadName	The name of the thread. 
@return the error state
*/

TInt RRemoteViewThread::Open(const TDesC& aViewName, const TDesC& aThreadName)
	{
	// Thread1: create semaphone for communicating w/Thread 2
	iDbReady = EFalse;
	
	//create semphore for communicating between threads:
	TInt err = iSemaphore.CreateGlobal(aViewName, 0);	
	if(err != KErrNone)
		{
		return err;
		}
		
	// this thread: Start Other thread
	TheTest.Printf(_L("Starting view thread %S"), &aThreadName);
	TThreadId id = CViewer::Create(aThreadName, aViewName);
		
	RThread thread;
	err = thread.Open(id);
	if(err==KErrNone)
		{	// tell kernel that this thread wants to know when the other thread dies
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat,KRequestPending);
		thread.Logon(iStatus);
		thread.Close();
		}
	
	return err;
	}

/** Sit and wait until the thread is ready*/
void RRemoteViewThread::WaitUntilDbIsReady()
	{
	if(iDbReady)
		{
		return;
		}
	iSemaphore.Wait();
	iDbReady = ETrue;
	}

/** Always call WaitUntilDbIsReady after this returns a true result
@return true if the thread is ready, false otherwise*/
TBool RRemoteViewThread::CheckIfDbIsReady()
	{
	if(iDbReady)
		{
		return ETrue;
		}
	iDbReady = ETrue;
	return iDbReady;
	}

/** Signal to the other thread that it should shut down and then wait for it to signal that is has closed 
*/
TInt RRemoteViewThread::Close()
	{
	if(!iDbReady)
		{
		WaitUntilDbIsReady();
		}
	iSemaphore.Signal();		// tell thread one it can die now
	TheTest.Printf(_L("Waiting for other thread to die"));
	// iStatus is set when the other thread dies
	User::WaitForRequest(iStatus);
	iSemaphore.Close();
	return iStatus.Int();
	}

///////////////////////////////////////////////////////////////////////////////
//#pragma mark ==== Test Function Prototypes ====
///////////////////////////////////////////////////////////////////////////////
void PopulateTestDatabaseL(CContactDatabase& aDatabase);
void TestL();
void doMainL();

///////////////////////////////////////////////////////////////////////////////
//#pragma mark ==== Test Function Implementations ====
///////////////////////////////////////////////////////////////////////////////
const TInt KPrintFreq=10;

/** add aCount random contacts to the open database. The 1st item will always be the own card */
CContactIdArray* PopulateTestDatabaseLC(CContactDatabase& aDatabase, TInt aCount )
	{
	__ASSERT_ALWAYS(aCount>0, User::Invariant());
	CContactIdArray* contacts = CContactIdArray::NewLC();
	CRandomContactGenerator* generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL( generator );
	generator->SetDbL(aDatabase);

	TInt bit = CContactDatabase::ESmsable;
	
	TContactItemId itemId = generator->AddTypicalContactForFilterL(bit);
	TInt counter = 0;
	TheTest.Printf(_L("Adding own card %d\n"), counter++);
	CContactItem* ownerCardItem = aDatabase.ReadContactL( itemId );
	CleanupStack::PushL( ownerCardItem );
	aDatabase.SetOwnCardL( *ownerCardItem );
	contacts->AddL(itemId);
	for(;counter<aCount;counter++)
		{
		itemId = generator->AddTypicalContactForFilterL(bit);
		if(counter%KPrintFreq==0)
			{
			TheTest.Printf(_L("Added contact %d: %i\n"), counter, itemId);
			}
		contacts->AddL(itemId);
		}
	CleanupStack::PopAndDestroy( ownerCardItem ); 
	CleanupStack::PopAndDestroy( generator );
	return contacts;
	}


/** Test that the view thread always correctly knows weather or not the view is ready for use*/
void Test1L()
	{
	// Thread 1: Create db
	CContactDatabase* database = CContactDatabase::ReplaceL();
	CleanupStack::PushL( database );
 
	// Thread1: create and start a thread with a remote view
	RRemoteViewThread viewer;
	User::LeaveIfError(viewer.Open(KViewNameOne, KThreadNameOne));
	CleanupClosePushL(viewer);

	// Thread 2: Open database
	// Thread 2: Create named view

	viewer.WaitUntilDbIsReady(); // wait until we've at least got the Db open (can cause file sharing trouble otherwise)

	// Thread 1: add a whole lot of contacts
	CContactIdArray& contacts = *PopulateTestDatabaseLC(*database, KNumberOfContacts );
	// Thread 2: print all the contacts in the view

	TInt64 seed=85204;
	CContactIdArray* toDelete = CContactIdArray::NewLC();
	TInt i;
	TInt end = contacts.Count();
	TInt start = Math::Rand(seed)%end;
	if(start+KNumberToDeleteAtOneTime<end)
		{
		end = start+KNumberToDeleteAtOneTime;
		}
	TheTest.Printf(_L("Deleting contatcs %i -- %i\n"), contacts[start],contacts[end-1]);
	for(i=start;i<end;i++)
		{
		toDelete->AddL(contacts[i]);
		}
	contacts.Remove(start,end-start);
	database->DeleteContactsL(*toDelete);
	toDelete->Reset();
	
	CleanupStack::PopAndDestroy(toDelete );	
	CleanupStack::PopAndDestroy(&contacts );
	CleanupStack::Pop( );	
	TheTest( viewer.Close()  == KErrNone );
	
	CleanupStack::PopAndDestroy(database );

	}

/** Test to make sure another thread can open the db when this one is performing many operations on it */
void Test2L()
	{
	// Thread 1: Create db
	CContactDatabase* database = CContactDatabase::ReplaceL();
	CleanupStack::PushL( database );

 	// Thread 1: add a whole lot of contacts
	CContactIdArray& contacts = *PopulateTestDatabaseLC(*database, KNumberOfContactsForTest2 );


 	// Thread 1: create a group
	CContactItem* group = database->CreateContactGroupL(_L("My Group"));
	CleanupStack::PushL(group);
	TContactItemId groupId = group->Id();

	// Thread1: create and start a thread with a remote view
	RRemoteViewThread viewer;
	User::LeaveIfError(viewer.Open(KViewNameOne, KThreadNameOne));
	CleanupClosePushL(viewer);

	// Thread 1: start hammering the database
	TInt i;
	TInt err;
	for(i=-1/*contacts.Count()-1*/;i>=0;i--) 
		{
		TRAP(err,database->AddContactToGroupL(contacts[i],groupId));
		if(err==KErrNotReady || err==KErrLocked)
			{
			i++;		// try again
			}
		else 
			{
			User::LeaveIfError(err);
			if(i%KPrintFreq==0)
				{
				TheTest.Printf(_L("Added contact %d to group\n"),contacts[i]);
				viewer.CheckIfDbIsReady(); // acknowledge db-is-ready signal
				}
			}
		}

	// Thread 2: Open database
	// Thread 2: Create named view

	viewer.WaitUntilDbIsReady(); // just in case is not ready yet -- acknowledge db-is-ready signal
	
	// Thread 2: print all the contacts in the view

	CContactIdArray* toDelete = CContactIdArray::NewLC();
	TInt64 seed=85204;
	TInt end   = contacts.Count();
	TInt start = Math::Rand(seed)%end;
	if(start+KNumberToDeleteAtOneTime<end)
		{
		end = start+KNumberToDeleteAtOneTime;
		}
	TheTest.Printf(_L("\nDeleting contacts %i -- %i\n"), contacts[start],contacts[end-1]);
	for(i=start;i<end;i++)
		{
		toDelete->AddL(contacts[i]);
		}
	contacts.Remove(start,end-start);
	database->DeleteContactsL(*toDelete);
	toDelete->Reset();
	
	CleanupStack::PopAndDestroy(toDelete );	
	CleanupStack::Pop( );	
	TheTest( viewer.Close()  == KErrNone );
	
	CleanupStack::PopAndDestroy(group );
	CleanupStack::PopAndDestroy(&contacts );
	CleanupStack::PopAndDestroy(database );
	}


////////////////////////////////////////////////////////////////////////////////////
// -------> Static global functions (source)
////////////////////////////////////////////////////////////////////////////////////
void doMainL()
	{
	CActiveScheduler*  scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// This call is just included to get rid of a linker warning LNK4089
	// Probably the reason for the warning is that the MVC++ environment 
	// sometimes does not realise when a library is actually used.
	// We do not care about if the server is hung or not, we just want to 
	// make the call.

	// Delete any existing ini file so that we can be sure this test is ok
	Test1L();			// Test read-when-not-ready Panic defect

	Test2L();
	CleanupStack::PopAndDestroy( scheduler );
	// delete db file here
	}


/**

@SYMTestCaseID     PIM-T-REMOTEVIEW-0001

*/

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("t_bench started"));

	CTrapCleanup* cleanupStack = NULL;
	__UHEAP_MARK;
	TheTest.Start(_L("@SYMTESTCaseID:PIM-T-REMOTEVIEW-0001 Remote View testing"));

	TheTest.Title();
	cleanupStack = CTrapCleanup::New();
	TRAPD(ret, doMainL());
	TheTest(ret == KErrNone);
	delete cleanupStack;

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
