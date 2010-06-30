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


#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>
#include "t_sec_asyncopen.h"
#include <coreappstest/testserver.h>

//
// Constants.
//
_LIT(KTestName,"T_Sec_AsyncOpen");
_LIT(KDbFileName,"c:T_Sec_AsyncOpen.cdb");

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__			
//That file is a contact model database from a Calypso device
_LIT(KDatabaseV4, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\FormatV4.cdb");
_LIT(KDbOldVersionFilePath,"C:\\private\\10003A73\\OLDVERSION.CDB");
#else
//Just use a SQlite file - this still tests async and sync openings of the file
_LIT(KDatabaseV4, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\SQLite__Contacts.cdb");
_LIT(KDbOldVersionFilePath,"C:\\private\\10003A73\\SQLite__OLDVERSION.CDB");
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__

_LIT(KDbOldVersionFileName,"C:OldVersion.cdb");

_LIT(KDbDamaged,"c:Damaged.cdb");
_LIT(KDefaultDb,"c:Contacts.cdb");
_LIT(KDbFileNameNotExist,"c:NotHere.cdb");

_LIT(KThreadOpenAsync,"OpenAsync");
_LIT(KThreadOpenSync,"OpenSync");
const TInt KNumWaitLoops = 3;


LOCAL_D RTest test(KTestName);

//
// CTestConductor.
//

CTestConductor* CTestConductor::NewL()
	{
	CTestConductor* self = new(ELeave) CTestConductor();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTestConductor::~CTestConductor()
	{	
    //state deleted during tests
	}

CTestConductor::CTestConductor() : CActive(EPriorityStandard),iCurrentTestNumber(-1),iWaitCounter(0)
	{
	CActiveScheduler::Add(this);
	}

void CTestConductor::ConstructL()
	{	
	// Create the db files used in the tests
	iDb = CContactDatabase::ReplaceL(KDbDamaged);
    CleanupStack::PushL(iDb);
    iDb->DamageDatabaseL(0x666);
    CleanupStack::PopAndDestroy(iDb);
    
	iDb = CContactDatabase::ReplaceL(KDbFileName);
	delete iDb;
	
	iDb = CContactDatabase::ReplaceL(KDefaultDb);
	delete iDb;
	
    CopyCdbFileL(KDatabaseV4,KDbOldVersionFilePath);	
    
	iDb = NULL;

	iTestError = KErrNone;

	NextTest();
	CActiveScheduler::Start();

	User::LeaveIfError(iTestError);
	}

void CTestConductor::NextTest()
	{
	++iCurrentTestNumber;
	TRequestStatus *pS = &iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}


void CTestConductor::RunL()
	{
	User::After(1000);
	switch (iCurrentTestNumber)
		{
		// Test One - Async Open of a Named Database
		case EAsyncOpenNamed:
	    	test.Next(_L("=== Async Open of Named Db"));

			SetActive();
			iContactOpenOperation = CContactDatabase::Open(KDbFileName,iStatus);
			++iCurrentTestNumber;
			
			break;
			
		// Check Test One Results
		case EAsyncOpenedNamed:
			test(iStatus.Int() == KErrNone);
			test.Printf(_L("=== Opened Async\n"));
			iDb = iContactOpenOperation->TakeDatabase();
			
			test(iDb != NULL);
			delete iContactOpenOperation;
			delete iDb;
			
			NextTest();
			break;

        // Test Two - Async Open of the Default Database
		case EAsyncOpenDefault:
		    test.Next(_L("=== Async Open Default"));

			SetActive();
			iContactOpenOperation = CContactDatabase::Open(iStatus);
			++iCurrentTestNumber;

			break;
		
		// Check Test Two Results	
		case EAsyncOpenedDefault:
			test(iStatus.Int() == KErrNone);
	     	test.Printf(_L("=== Opened Async Default \n"));
			iDb = iContactOpenOperation->TakeDatabase();
            test(iDb != NULL);
            delete iContactOpenOperation;
            delete iDb;            
			
			NextTest();
			break;			
		
		// Test Three - Attempted Async Open of a Non-Existent File
		case EAsyncOpenNonExistent:		
			test.Next(_L("=== Attempted Open of Non-Existent File"));

			SetActive();
			iContactOpenOperation = CContactDatabase::Open(KDbFileNameNotExist,iStatus);
			++iCurrentTestNumber;
			
			break;
			
		// Check Test Three Results	
		case EAsyncOpenedNonExistent:
		    test.Printf(_L("=== Opened Non Existent Response\n"));		
			test(iStatus.Int() == KErrNotFound);
			
			delete iContactOpenOperation;
			NextTest();
			break;	
			
		// Test Four - Cancel the Async Open
		case EAsyncCancelOpenDefault:
			test.Next(_L("=== Cancel Async Open"));


			SetActive();
			iContactOpenOperation = CContactDatabase::Open(iStatus);
			++iCurrentTestNumber;
			
			// Cancel the request
			delete iContactOpenOperation;
						
			break;	
			
		// Check Test Four Results	
		case EAsyncCanceledOpenedDefault:
			test.Printf(_L("=== Cancelled Async Open\n"));
			test(iStatus.Int() == KErrCancel);
			NextTest();
			break;	
			
		// Test Five - Async Open of a damaged Database
		case EAsyncOpenDamaged:					
			test.Next(_L("Open a damaged database"));

            
            SetActive();
			iContactOpenOperation = CContactDatabase::Open(KDbDamaged,iStatus);
			++iCurrentTestNumber;
			
			break;	
		
		// Check Test Five Results
		case EAsyncOpenedDamaged:	
			test(iStatus.Int() == KErrNone);
	     	test.Printf(_L("=== Opened Damaged Db \n"));
			iDb = iContactOpenOperation->TakeDatabase();
            test(iDb != NULL);
            delete iContactOpenOperation;
            delete iDb;
            
			NextTest();
				
			break;
							
		// Test Six - Async Open of a Previous version of the Database	
		case EAsyncOpenPreviousVersion:					
			test.Next(_L("Open an old version database"));

    
			SetActive();
			iContactOpenOperation = CContactDatabase::Open(KDbOldVersionFileName,iStatus);
			++iCurrentTestNumber;
			
			break;	
		
		// Check Test Six Results
		case EAsyncOpenedPreviousVersion:	
   			test(iStatus.Int() == KErrNone);
	     	test.Printf(_L("=== Opened Previoius Version Db \n"));
	     	
			iDb = iContactOpenOperation->TakeDatabase();
            test(iDb != NULL);
            delete iContactOpenOperation;
            delete iDb;			
            
			NextTest();
			break;
			
		// Test Seven - Async Open of a Previous version of the Database, and an Async
		// call open of the same database in a different thread.	
		case EOpenAsyncAndAsync:
		    test.Next(_L("Open Async Then Sync"));

		    
		    // Copy Version 4 Db
		    CopyCdbFileL(KDatabaseV4,KDbOldVersionFilePath);
		
		    // Open Async
		    SetActive();
			iContactOpenOperation = CContactDatabase::Open(KDbOldVersionFileName,iStatus);
			++iCurrentTestNumber;
		    
		    // Lauch Thread to Open Db Asynchronously
		    CDbOpenLauncher::Create(KThreadOpenAsync, KThreadOpenAsync);		
		    break;
		
		// Check Test Seven Results 
		case EOpenedAsyncAndAsync:
			test(iStatus.Int() == KErrNone);
	     	test.Printf(_L("=== Opened After Async Open Db \n"));
	     	
			iDb = iContactOpenOperation->TakeDatabase();
            test(iDb != NULL);
			delete iContactOpenOperation;
            delete iDb;
             
			NextTest();
			break;	
			
			
		// Iterate through these states (allowing other thread to open asynchronously)
		case EWaitState:
			test.Printf(_L("=== Waiting in first thread \n"));
			iCurrentTestNumber = EWaitedState;
			SetActive();
			iContactOpenOperation = CContactDatabase::Open(KDbOldVersionFileName,iStatus);

		    break;
		    
		case EWaitedState:
		
		    test(iStatus.Int() == KErrNone);
	     	test.Printf(_L("=== Opened After Wait Db \n"));
	     	
			iDb = iContactOpenOperation->TakeDatabase();
            test(iDb != NULL);
			delete iContactOpenOperation;
            delete iDb;
            
            if (iWaitCounter < KNumWaitLoops) 
            	{
            	iCurrentTestNumber = EWaitState - 1;  
            	iWaitCounter++; 	
            	}
            else
            	{
            	// move onto next test
                iCurrentTestNumber = EWaitedState;
            	}
            
			NextTest();
			
			break;	
		
		// Test Eight - Async Open of a Previous version of the Database, and a Sync
		// open of the same database in a different thread.	
		case EOpenAsyncAndSync:		
			test.Next(_L("Open Async Then Sync"));

		    
		    // Copy Version 4 Db
		    CopyCdbFileL(KDatabaseV4,KDbOldVersionFilePath);
		
		    // Open Async
		    SetActive();
			iContactOpenOperation = CContactDatabase::Open(KDbOldVersionFileName,iStatus);
		    ++iCurrentTestNumber;
		    
		    // Lauch Thread to Open Db Synchronously
		    CDbOpenLauncher::Create(KThreadOpenSync, KThreadOpenSync);
	        break;
	
		// Check Test Eight Results
		case EOpenedAsyncAndSync:
		  	test(iStatus.Int() == KErrNone);
	     	test.Printf(_L("=== Opened After Sync Open Db \n"));
	     	
			iDb = iContactOpenOperation->TakeDatabase();
            test(iDb != NULL);
			delete iContactOpenOperation;
            delete iDb;
             
			NextTest();
			break;				

		// End of Tests	
		case ENumTests:
			{
			test.Printf(_L("=== Main Tests Complete\n"));
			RThread thread;
			test(thread.RequestCount()==0);

			// tidy up files
			CContactDatabase::DeleteDatabaseL(KDbDamaged);
			CContactDatabase::DeleteDatabaseL(KDefaultDb);			
			CContactDatabase::DeleteDatabaseL(KDbOldVersionFileName);
			 			

			CContactDatabase::DeleteDatabaseL(KDbFileName);
			
			CActiveScheduler::Stop();
			}
			break;
		
		
		default:
			ASSERT(EFalse);
			break;
		}
	}


void CTestConductor::DoCancel()
	{
	}


TInt CTestConductor::RunError(TInt aError)
	{
	// (only a few steps call leaving methods but we have case statements for each)
	// propagate aError back to caller
	iTestError = aError;


	switch (iCurrentTestNumber)
		{
		case EAsyncOpenNamed: test.Printf(_L("Failed at test step AsyncOpenNamed (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpenedNamed: test.Printf(_L("Failed at test step AsyncOpenedNamed (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpenDefault: test.Printf(_L("Failed at test step AsyncOpenDefault (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpenedDefault: test.Printf(_L("Failed at test step AsyncOpenedDefault (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpenNonExistent: test.Printf(_L("Failed at test step AsyncOpenNonExistent (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpenedNonExistent: test.Printf(_L("Failed at test step AsyncOpenedNonExistent (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncCancelOpenDefault: test.Printf(_L("Failed at test step AsyncCancelOpenDefault (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncCanceledOpenedDefault: test.Printf(_L("Failed at test step AsyncCanceledOpenedDefault (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpenDamaged: test.Printf(_L("Failed at test step AsyncOpenDamaged (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpenedDamaged: test.Printf(_L("Failed at test step AsyncOpenedDamaged (%i), with error %i"), iCurrentTestNumber, aError); break;		
		case EAsyncOpenPreviousVersion: test.Printf(_L("Failed at test step AsyncOpenPreviousVersion (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EAsyncOpenedPreviousVersion: test.Printf(_L("Failed at test step AsyncOpenedPreviousVersion (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EOpenAsyncAndAsync: test.Printf(_L("Failed at test step OpenAsyncAndAsync (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EOpenedAsyncAndAsync: test.Printf(_L("Failed at test step OpenedAsyncAndAsync (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EWaitState: test.Printf(_L("Failed at test step WaitState (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EWaitedState: test.Printf(_L("Failed at test step WaitedState (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EOpenAsyncAndSync: test.Printf(_L("Failed at test step OpenAsyncAndSync (%i), with error %i"), iCurrentTestNumber, aError); break;
		case EOpenedAsyncAndSync: test.Printf(_L("Failed at test step OpenedAsyncAndSync (%i), with error %i"), iCurrentTestNumber, aError); break;

		case ENumTests: test.Printf(_L("Failed at test step NumTests (%i), with error %i"), iCurrentTestNumber, aError); break;

		default: test.Printf(_L("Failed at test step %i, with error %i"), iCurrentTestNumber, aError); break;
		}

	CActiveScheduler::Stop();
	return KErrNone;
	}


void CTestConductor::CopyCdbFileL(const TDesC& aFileName, const TDesC& aToFileName)
	{
	RPIMTestServer serv;
	User::LeaveIfError(serv.Connect());
	serv.CopyFileL(aFileName, aToFileName);
	serv.Close();
	}	


//
// Separate Thread to Open Database
//
CDbOpenLauncher::CViewer* CDbOpenLauncher::CViewer::NewL(const TDesC& aViewName)
	{
	CViewer* self = new(ELeave) CViewer;
	CleanupStack::PushL(self);
	self->ConstructL(aViewName);
	CleanupStack::Pop(self);
	return(self);
	}

CDbOpenLauncher::CViewer::~CViewer()
	{
	delete iDatabase;
	}

void CDbOpenLauncher::CViewer::ConstructL(const TDesC& aViewName)
	{
	if(aViewName.Compare(KThreadOpenAsync) == 0)
		{	
		// Create Async Open 
		tester=CTester::NewL();
		}		
	else if (aViewName.Compare(KThreadOpenSync) == 0)
		{
		//Open the database Synchronously
		iDatabase = CContactDatabase::OpenL(KDbOldVersionFileName);
		test(iDatabase != NULL);
		delete iDatabase;
		}		
	}

void CDbOpenLauncher::DoMainL(const TDesC& aName)
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);	
	CViewer* view = CViewer::NewL(aName);  
	CleanupStack::PushL(view);

	CActiveScheduler::Start();
		
	CleanupStack::PopAndDestroy(view);
	}
		
// This is the only thing one needs to call in order to set the view in motion. 
void CDbOpenLauncher::Create(const TDesC& aName, const TDesC& aViewName)
	{
	RThread thread;
	thread.Create( aName, CDbOpenLauncher::LaunchThread, KDefaultStackSize, 0x2000, 0x20000, (void*) &aViewName, EOwnerThread );
	thread.Resume();
	thread.SetPriority(EPriorityNormal);
	thread.Close();
	}


TInt CDbOpenLauncher::LaunchThread(TAny* aAny) 
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanUpStack=CTrapCleanup::New();
	if (!cleanUpStack)
		{
		return KErrNoMemory;
		}			
					
	TRAPD(r,DoMainL(*((TDesC*) aAny)));

	delete CActiveScheduler::Current();
	delete cleanUpStack;

	__UHEAP_MARKEND;
	return r;
	}
	
	
//
// CTester Active Object to test Async Open in thread whilst async open is going on in other thread
//	
CTester* CTester::NewL()
	{
	CTester* self = new(ELeave) CTester();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CTester::DoCancel()	
	{
	}

CTester::~CTester()
	{	
    //state deleted during tests
	}

CTester::CTester() : CActive(EPriorityIdle),iCurrentTestNumber(0)
	{
	CActiveScheduler::Add(this);
	}

void CTester::ConstructL()
	{
	NextTest();
	CActiveScheduler::Start();
	}

void CTester::NextTest()
	{
	TRequestStatus *pS = &iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CTester::RunL()
	{
	if (iCurrentTestNumber == EAsyncOpenNamed) 
		{
    	SetActive();
		iContactOpenOperation = CContactDatabase::Open(KDbFileName,iStatus);
		++iCurrentTestNumber;    		
		}
	else if(iCurrentTestNumber == EAsyncOpenedNamed)
		{
		test(iStatus.Int() == KErrNone);
		iDb = iContactOpenOperation->TakeDatabase();
			
		test(iDb != NULL);
		delete iContactOpenOperation;
		delete iDb;
		CActiveScheduler::Stop();
		}
	
	
	}
	
//
// Main.
//

/**

@SYMTestCaseID     PIM-T-SEC-ASYNCOPEN-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-SEC-ASYNCOPEN-0001 Async Open Testing"));

	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* tester=NULL;
			TRAPD(err,tester=CTestConductor::NewL());
			test(err == KErrNone);
			delete tester;
			delete cleanup;
			}
		delete scheduler;
		}
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
    }	
	
	
		
		


