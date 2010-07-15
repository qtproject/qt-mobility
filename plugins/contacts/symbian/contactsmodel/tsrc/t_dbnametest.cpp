/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@SYMTestCaseID      PIM-APPENG-CNTM-CNTDB-106281-0001
@SYMTestCaseDesc    The purpose of this test is to verify the expected behaviour
                    of the CContactDatabase public APIs involving invalid database
                    file paths (eg. string length exceeds 190 characters,
                    and string containing path characters)
@SYMTestActions     Passing an empty string, a string with more than 190 characters,
                    and a string containing path into CContactDatabase's public APIs and
                    verify if the expected error code is returned
@SYMTestExpectedResults     Expected all test cases to pass.
@SYMDEF                     DEF106281
@SYMTestType                UT
@SYMTestPriority            Low
*/

#include <cntdb.h>
#include "ccntipccodes.h"
#include "t_dbnametest.h"

//===========================================================
// CDbNameTest Class
//===========================================================
CDbNameTest::~CDbNameTest()
    {
    iLongDbName.Close();
    iInvalidDbName.Close();

    if (iActiveSchedulerWait)
        {
        if (iActiveSchedulerWait->IsStarted())
            {
            iActiveSchedulerWait->AsyncStop();
            }
        delete iActiveSchedulerWait;
        }

    iTest->Close();
    delete iTest;
    }

CDbNameTest::CDbNameTest()
    : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

CDbNameTest* CDbNameTest::NewLC()
    {
    CDbNameTest* self = new (ELeave) CDbNameTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CDbNameTest::ConstructL()
    {
    iTest = new(ELeave) RTest(_L("T_DBNameTest"));

    // Create invalid DB Name (with wildcard & path characters)
    iInvalidDbName.Create(_L("c:**"));
    }

void CDbNameTest::RunTestsL()
    {
    iTest->Start(_L(" ----------- T_DBNameTest ---------- "));
	
	RArray<TInt> errArray;
	CleanupClosePushL(errArray);
	
	//MaxLength	
	errArray.AppendL(KErrNone);  //Create
	errArray.AppendL(KErrNone);     //Open
	errArray.AppendL(KErrNone); //Replace
	errArray.AppendL(KErrNone);    //Delete
	errArray.AppendL(KErrNone);    //Exists
	SetAndTestFileNameL(EMaxLength, errArray);
	
	//ELessThanMaxLength
	errArray.Reset();	
	errArray.AppendL(KErrNone);  //Create
	errArray.AppendL(KErrNone); //Open
	errArray.AppendL(KErrNone); //Replace
	errArray.AppendL(KErrNone); //Delete
	errArray.AppendL(KErrNone); //Exists
	SetAndTestFileNameL(ELessThanMaxLength, errArray);

	//ENoDriveName
	errArray.Reset();	
	errArray.AppendL(KErrBadName); //Create
	errArray.AppendL(KErrBadName); //Open
	errArray.AppendL(KErrBadName); //Replace
	errArray.AppendL(KErrBadName); //Delete
	errArray.AppendL(KErrBadName); //Exists
	SetAndTestFileNameL(ENoDriveName, errArray);
		
	//EInvalidFileName
	errArray.Reset();
	errArray.AppendL(KErrBadName); //Create
	errArray.AppendL(KErrBadName); //Open
	errArray.AppendL(KErrBadName); //Replace
	errArray.AppendL(KErrBadName); //Delete
	errArray.AppendL(KErrBadName); //Exists
	SetAndTestFileNameL(EInvalidFileName, errArray);
	
	//ENullFileName	
	errArray.Reset();
	errArray.AppendL(KErrBadName);  //Create
	errArray.AppendL(KErrBadName);  //Open
	errArray.AppendL(KErrBadName);  //Replace
	errArray.AppendL(KErrNotFound); //Delete (expects no default database in the private directory) 
	errArray.AppendL(KErrNone);     //Exists
	//deleting default database if exists.
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KNullDesC));
	SetAndTestFileNameL(ENullFileName, errArray);
	
	//EMoreThanMaxLength
	errArray.Reset();
	errArray.AppendL(KErrArgument); //Create
	errArray.AppendL(KErrArgument); //Open
	errArray.AppendL(KErrArgument); //Replace
	errArray.AppendL(KErrArgument); //Delete
	errArray.AppendL(KErrArgument); //Exists
	SetAndTestFileNameL(EMoreThanMaxLength, errArray);
   
    CleanupStack::PopAndDestroy(); //Releasing memory used by errArray.
    
    //EMoreThanMaxLength above creates a filename with length more than allowed length, Test Asyncopendatabasetest
    //requires such a file name, so it should be run after above test.
  
    AsyncOpenDatabaseTest();

    (*iTest) (iFailedTests == 0);

    iTest->End();
    }


void CDbNameTest::AsyncOpenDatabaseTest()
    {
    // open database with long path name
    iTest->Next(_L("TEST 1: CContactDatabase::Open() with long file name"));

    iCurrName.Set(iLongDbName);

    SetActive();
    TRequestStatus* status = &iStatus;
    iOpenOp = CContactDatabase::Open(iLongDbName, *status);

    iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait();
    iActiveSchedulerWait->Start();
    }

void CDbNameTest::AsyncOpenDatabaseNullTestL()
    {
    // open database with NULL path name
    iTest->Next(_L("TEST 2: CContactDatabase::Open() with NULL file name."));

    iCurrName.Set(KNullDesC);

    SetActive();
    TRequestStatus* status = &iStatus;
    iOpenOp = CContactDatabase::Open(KNullDesC, *status);
    }

void CDbNameTest::AsyncOpenDatabaseInvalidTest()
    {
    // open database with invalid path name
    iTest->Next(_L("TEST 3: CContactDatabase::Open() with file name containing path or wildcard characters."));

    iCurrName.Set(iInvalidDbName);

    SetActive();
    TRequestStatus* status = &iStatus;
    iOpenOp = CContactDatabase::Open(iInvalidDbName, *status);
    }

void CDbNameTest::RunL()
    {
    TInt err = iStatus.Int();

    if (iOpenOp)
        {
        if (err == KErrNone)
            {
            // if the database has been successfully opened, close it
            CContactDatabase* database = iOpenOp->TakeDatabase();
            if (database)
                {
                delete database;
                }
            }
        delete iOpenOp;
        iOpenOp = NULL;
        }

    if (iLongDbName == iCurrName)
        {
        if (err != KErrArgument)
            {
            iFailedTests++;
            iTest->Printf(_L("-> FAILED - invalid error code returned-- %d.\n"), err);
            }
        
        // Proceed to test 2
        AsyncOpenDatabaseNullTestL();
        }
    else if (iCurrName.Length() == 0)
        {
        // Asyc Open treats a NULL database path as the default database.
        // Therefore, both KErrNone, (meaning a default database exists), and 
        // KErrNotFound, (meaning the default database not yet exist), are acceptable
        // error codes.
        if (err != KErrNone && err != KErrNotFound)
            {
            iFailedTests++;
            iTest->Printf(_L("-> FAILED - invalid error code returned-- %d.\n"), err);
            }

        // Process to last test
        AsyncOpenDatabaseInvalidTest();
        }
    else
        {
        if (err != KErrBadName)
            {
            iFailedTests++;
            iTest->Printf(_L("-> FAILED - invalid error code returned-- %d.\n"), err);
            }

        iActiveSchedulerWait->AsyncStop();
        }
    }


/**
@SYMTestCaseID      		PIM-DBNAMETEST-0001
@SYMTestCaseDesc    		Attempts to test the behaviour of Contactdatabse API's
							while Opening, Deleting, Creating, Replacing a filename of different types. 
@SYMTestActions     		1)Create a filename, it can be of different types.
							2)Try to perform Create,Open,Delete,Replace on that filename.
							3)Check if it returns a correct errorcode.						
@SYMTestExpectedResults     Expected all test cases to pass.
@SYMDEF                     DEF114180
@SYMTestType                UT
@SYMTestPriority            Low
*/
    
void CDbNameTest::SetAndTestFileNameL(TFileNameType aType, RArray<TInt>& aErrArray)
	{
	_LIT16(KDrive,"c:");
	
	TBool driveName = ETrue;
	TBool invalidFileName = EFalse;
	TBool createFilename = ETrue;
	TBool nullFilename = EFalse;
	TInt nameLength = 0;
	
	
	switch (aType)
		{
		case ENoDriveName:
			{
			iTest->Next(_L("\n*****Operations without Drive name.*****"));
			nameLength = 50;
			driveName = EFalse;	
			}
		break;
		
		case EMoreThanMaxLength:
			{
			iTest->Next(_L("\n*****Operations with Filename more than maximum allowed length.*****"));
			nameLength = 200;	
			}
		break;
		
		case EMaxLength:
			{
			iTest->Next(_L("\n*****Operations with Filename having maximum allowed length.*****"));
			nameLength = 190;	
			}
		break;
		
		case ELessThanMaxLength:
			{
			iTest->Next(_L("\n*****Operations with Filename length less than maximum allowed length.*****"));
			nameLength = 100;	
			}
		break;
		
		case  EInvalidFileName:
			{
			iTest->Next(_L("\n*****Operations with Invalid Filename.*****"));
			invalidFileName = ETrue;
			createFilename = EFalse;
			driveName = EFalse;
			nameLength = 5;	
			}
		break;
		
		case ENullFileName:
			{
			iTest->Next(_L("\n*****Operations with NULL Filename.*****"));			
			nullFilename = ETrue;
			createFilename = EFalse;
			driveName = EFalse;	
			}
		break;	
		
		default:
		User::Leave(KErrArgument);
		
		}
	
	iLongDbName.Close();
	iLongDbName.CreateL(nameLength);
	
	if(driveName)
		{
		iLongDbName.Append(KDrive);	
		}
	
	if(invalidFileName)
		{
		iLongDbName = iInvalidDbName;	
		}
		
	if(nullFilename)
		{
		iLongDbName.Append(KNullDesC);	
		}
		
	if(createFilename)
		{
	    for (TInt i = 1; i < iLongDbName.MaxLength()-2; i++)
	        {
	        TChar ch('a');
	        ch += i % 26;
	        iLongDbName.Append(ch);
	        }		
		}

	TInt typeOfTests = 5;
 	for(TInt loop = 0;loop < typeOfTests ; ++loop)
	 	{
	 	
	 	CContactDatabase* db = NULL;
	 	
	 	TInt err(0);
	 	
	 	TInt typeOfError(0);
	 	
	 	switch(loop)
	 	{
	 		case 0:   // CREATE DATABASE TEST
	 	 		{
		 		iTest->Printf(_L("TEST : CContactDatabase::CreateL() ."));
		    	
		    	TRAP(err, db = CContactDatabase::CreateL(iLongDbName));
		    	
		    	if (err == aErrArray[0])
			    	{
			    	typeOfError = 1;					
			    	}
		    	else if(err == KErrAlreadyExists || err == KErrBadName || err == KErrDiskFull || err == KErrArgument)
			    	{
			     	typeOfError = 2;	
			    	}
				} 
	     	break;
	 
	 		case 1:	  // OPEN DATABASE TEST
		 		{
		 		iTest->Printf(_L("TEST : CContactDatabase::OpenL()."));
		 		
		    	TRAP(err, db = CContactDatabase::OpenL(iLongDbName));
		    	
		    	if (err == aErrArray[1])
			    	{
			    	typeOfError = 1;					
			    	}
		    	else if(err == KErrNotFound || err == KErrLocked || err == KErrBadName || err == KErrDiskFull || err == KErrArgument)
			    	{
			     	typeOfError = 2;	
			    	}		    	
				}    	
	 		break;
	 		
	 		case 2:  // REPLACE DATABASE TEST
		 		{
		 		iTest->Printf(_L("TEST : CContactDatabase::ReplaceL()()."));
		 		
		    	TRAP(err, db = CContactDatabase::ReplaceL(iLongDbName));
		    	
		    	if (err == aErrArray[2])
			    	{
			    	typeOfError = 1;					
			    	}
		    	else if(err == KErrInUse || err == KErrBadName || err == KErrDiskFull || err == KErrArgument)
			    	{
			     	typeOfError = 2;	
			    	}		    	
				}  		
	 		break;
	 		
	 		case 3:  // DELETE DATABASE TEST
	 	 		{
		 		iTest->Printf(_L("TEST : CContactDatabase::DeleteDatabaseL()."));
		 		
		    	TRAP(err, CContactDatabase::DeleteDatabaseL(iLongDbName));
		    	
		    	if (err == aErrArray[3])
			    	{
			    	typeOfError = 1;					
			    	}
		    	else if(err == KErrNotFound || err == KErrBadName || err == KErrInUse || err == KErrArgument)
			    	{
			     	typeOfError = 2;	
			    	}		    	
				} 		
	 		break;
	 		
	 		case 4:  // DATABASE EXISTS TEST
	 	 		{
		 		iTest->Printf(_L("TEST : CContactDatabase::ContactDatabaseExistsL()."));
		 		
		    	TRAP(err, CContactDatabase::ContactDatabaseExistsL(iLongDbName));
		    	
		    	if (err == aErrArray[4])
			    	{
			    	typeOfError = 1;					
			    	}
		    	else if(err == KErrNotReady || err == KErrBadName || err == KErrNotFound || err == KErrArgument || err == KErrCorrupt)
			    	{
			     	typeOfError = 2;	
			    	}		    	
				} 		
	 		break;
	 		
	 		
	 		default:
	 			User::Leave(KErrArgument);
	 		break;	 		
	 		
	 	}
	 	
	 	if(typeOfError == 1)
		 	{
			iTest->Printf(_L("Correct Error Code Returned: %d"), err);
		 	}
	 	else if(typeOfError == 2)
		 	{
			iTest->Printf(_L("Incorrect, but one of the possible error codes returned: %d"), err);
		 	}
	 	else
		 	{
			iTest->Printf(_L("ERROR:- Test Failed, Unexpected Error Code returned: %d"), err);
		 	}
 	 
	    if (db)
	        {
	        delete db;
	        }
	  	}  	
				
	}


LOCAL_C void DoTestsL()
    {
    CDbNameTest* test = CDbNameTest::NewLC();
    test->RunTestsL();
    CleanupStack::PopAndDestroy(test);
    }

GLDEF_C TInt E32Main()
    {
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }

    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        {
        return KErrNoMemory;
        }
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    __UHEAP_MARK;
    TRAPD(err, DoTestsL());
    __UHEAP_MARKEND;

    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
    return err;
    }

