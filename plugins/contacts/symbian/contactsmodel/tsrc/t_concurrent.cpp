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

#include "t_concurrent.h"
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>            
#include <cntdb.h>
#include <cntdbobs.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>


//  Constants
_LIT(KThreadTitle, "Thread2");
_LIT(KTextFailed, " failed, leave code = %d");

// Global Variables
RTest MainTest(_L("Concurrent test"));	// Main RTest
CContactDatabase * database = NULL;		// Shared database.
RSemaphore sem;							// Synchronising Semaphore
TInt t1MachineId = 0;					// Thread1 machineId from db.
TInt currentStep = 0;					// Current test step.

///////////////////////////////////////////////////////////
//                  CHILD THREAD FUNCTIONS.
///////////////////////////////////////////////////////////

/**
* The main function called when the child thread is invoked.
*/
TInt RunChildThread(TAny * /* ptr */)
	{
    // Create cleanup stack
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CActiveScheduler::Install(scheduler);
    CChildThread* childThread = NULL;
    
    TInt err;
    TRAP(err, childThread = CChildThread::NewL());
	if (err == KErrNone)
		{
		TRAP(err, childThread->AccessDBTestL() );
		}
    
	if (err != KErrNone)
    	MainTest.Printf( _L(" Child thread test returned with error = %d"),  err);

	delete childThread;
    delete cleanup;
    delete scheduler;
	__UHEAP_MARKEND;
    return KErrNone;	
	}

/**
* The child thread accesses the shared db session.
* A basic operation like getting the count of contacts is performed.
*/
CChildThread * CChildThread::NewL()
	{
	CChildThread * self = new(ELeave) CChildThread;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CChildThread::ConstructL()
	{
	TBuf<100> threadName;
	threadName.Format(_L("Child Thread"));
	iTest = new(ELeave) RTest(threadName);
	}

CChildThread::~CChildThread()
	{
	iTest->Close();
	delete iTest;
	iTest = NULL;
	}
	

/**
* This function tries to access the db and check the count, or perform some other action
* on the db. The session object is shared with the main thread .
*/
void CChildThread::AccessDBTestL()
	{
    
    while(currentStep != ETestsEnd)
    	{
	    TInt t2MachineId = database->MachineId();
	    TInt count = database->CountL();
	    iTest->Printf( _L("Thread2: Contact Count = %d"), count  );
	    
	    // Check that the child thread is accessing the shared db session correctly.
	    MainTest(count==1);
	    MainTest( t2MachineId == t1MachineId );
	    
	    sem.Signal();
	    RThread().Suspend();
	    }
	   
	// Child thread is now over.  
	sem.Signal();  
	    
	}


///////////////////////////////////////////////////////////
//                  MAIN THREAD FUNCTIONS.
///////////////////////////////////////////////////////////
/**
* The main thread object which owns the session with the database in thread shared mode.
*/
CMainThread * CMainThread::NewL()
	{
	CMainThread * self = new (ELeave) CMainThread;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CMainThread::ConstructL()
	{
	TBuf<100> threadName;
	threadName.Format(_L("Main Thread"));
	iTest = new(ELeave) RTest(threadName);
	}
	
CMainThread::~CMainThread()
	{
	TRAP_IGNORE(CloseDatabaseSessionL() );
	iTest->Close();
	delete iTest;
	iTest = NULL;
	iChildThread.Close();
	}
	
/** 
* The function to empty the database and close the database session.
*/
void CMainThread::CloseDatabaseSessionL()
	{
		TestDeleteContactL();
		delete database;
		database = NULL;
	}

/**
* Create the database in shared mode.
*/
void CMainThread::TestCreateDatabaseL()
	{
	// Creat the database in the first thread.
    iTest->Next( _L("CreateDatabaseL :Test creating the db in multi - thread mode.") );
    TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());
    database = CContactDatabase::CreateL( CContactDatabase::EMultiThread );
    t1MachineId = database->MachineId();
	}

/**
* Open the database in shared mode.
*/
void CMainThread::TestOpenDatabaseL()
	{
	// Open the database in the first thread.
    iTest->Next( _L("OpenDatabaseL :Test Opening the db in multi - thread mode.") );
    database = CContactDatabase::OpenL( CContactDatabase::EMultiThread );
    t1MachineId = database->MachineId();
	}

/**
* Replace the database in shared mode.
*/
void CMainThread::TestReplaceDatabaseL()
	{
    // Replace the database in the first thread, in mutithread mode.
    iTest->Next( _L("ReplaceDatabaseL :Test Replacing the db in multi - thread mode.") );
    database = CContactDatabase::ReplaceL( CContactDatabase::EMultiThread );
    t1MachineId = database->MachineId();
	}

/**
* Add one contact to the database.
*/
void CMainThread::TestAddContactL()
	{
    
    // Add a contact 
	_LIT(KForename,"Jo"); 
	_LIT(KSurname,"Stichbury"); 

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
   		    
	// Add newCard to the database
	iContactId = database->AddNewContactL(*newCard);
	CleanupStack::PopAndDestroy(newCard);
	
	iTest->Printf( _L("Thread1: Added 1 contact to the database."));
	}

/**
* Delete the contact from the database.
*/
void CMainThread::TestDeleteContactL()
	{
	// Check if a contact has been stored in the db and delete it.
	if (iContactId)
		{
		database->DeleteContactL(iContactId);
		iContactId = 0;
		}
	}

/**
* The main thread controls the child thread
*/
void CMainThread::LaunchChildThreadL()
	{
	
    TRAPD(err,iChildThread.Create(KThreadTitle, RunChildThread, KDefaultStackSize, NULL, NULL, EOwnerProcess));
    
    if(err != KErrNone)
		{
		iTest->Printf(_L("Failed to create the child thread, error code=%d"), err);
		User::Leave(err);
		}
	}

/**
* Resume the child thread.
*/	

void CMainThread::ResumeChildThreadL()
	{
    iChildThread.Resume();
	}


///////////////////////////////////////////////////////////
//                  MAIN FUNCTIONS.
///////////////////////////////////////////////////////////

/**
* The main function which creates the main thread controller object 
* and invokes all the tests.
*/

/**

@SYMTestCaseID     PIM-T-CONCURRENT-0001

*/

LOCAL_C void DoTestsL()
    {
    
    // Initialisation
    MainTest.Start(_L("@SYMTESTCaseID:PIM-T-CONCURRENT-0001 T_Concurrent"));

    
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    // Create main thread object, child thread and synchnising semaphore.
    sem.CreateLocal(0);
    CMainThread * ptr = CMainThread::NewL();
    CleanupStack::PushL(ptr);
    ptr->LaunchChildThreadL();
	
	// Run all the tests
	while (currentStep <= ETestsEnd)
		{
		switch(currentStep)
			{
			case ETestCreate:
				// create the db in shared mode and add a contact. 
			    ptr->TestCreateDatabaseL();
			    ptr->TestAddContactL();
				break;
			case ETestOpen:
			 	// create the db in shared mode 
			    ptr->TestOpenDatabaseL();
			    ptr->TestAddContactL();
				break;
			case ETestReplace:
				// create the db in shared mode 
			    ptr->TestReplaceDatabaseL();
				ptr->TestAddContactL();
				break;
			case ETestsEnd:
				break;
			default:
				break;
			} // end of switch
			
			// Run the child thread and let it access the shared session.
		    ptr->ResumeChildThreadL();
		    sem.Wait();
		    ptr->CloseDatabaseSessionL();
		    currentStep++ ;
		} // end of while

    // Cleanup and close.
    sem.Close();
    CleanupStack::PopAndDestroy(ptr);
    CleanupStack::PopAndDestroy(scheduler);
    }


/**
*  Entry point function.
*/
GLDEF_C TInt E32Main()
    {
    // Create cleanup stack
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
	
    TRAPD(mainError, DoTestsL());
    if (mainError)
        MainTest.Printf(KTextFailed, mainError);
    
    MainTest.End();
    MainTest.Close();
    delete cleanup;
    
    __UHEAP_MARKEND;
    return KErrNone;
    }

	





