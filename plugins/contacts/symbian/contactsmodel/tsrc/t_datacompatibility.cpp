/*
* Copyright (c) 2003-2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <f32file.h>
#include <coreappstest/testserver.h>

#define __SYMBIAN_CNTMODEL_USE_SQLITE__

_LIT(KTestName, "T_DataCompatibility");

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__ 

//That file is a contact model database from a Calypso device
_LIT(KDatabaseV4, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\FormatV4.cdb"); 
_LIT(KDatabaseV5, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\FormatV5.cdb"); 

_LIT(KDatabaseV6With10Entries, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\FormatV6-10.cdb"); 
_LIT(KDatabaseV7With25Entries, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\FormatV7-25.cdb"); 
_LIT(KDatabaseV4With4Entries, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\FormatV4-4.cdb");
_LIT(KDatabaseV8, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\DBS_100065FF_performance_100_4.cdb");

_LIT(KDatabaseCDriveFullPath,"C:\\private\\10003A73\\contacts.cdb");
_LIT(KOldDatabaseCDriveFullPath,"C:\\private\\10003A73\\DBS_100065FF_performance_100_4.cdb");

_LIT(KDatabaseCDrive,"C:contacts.cdb");
_LIT(KOldDatabaseCDrive,"C:DBS_100065FF_performance_100_4.cdb");
    
_LIT(KOldFileFormatPrefix,"DBS_100065FF");

RFs fs;

// Logging macros & utilities //
GLDEF_C RTest test(KTestName);


LOCAL_C void CleanupFilesL()
    {
    // delete the database files
    if (CContactDatabase::ContactDatabaseExistsL(KDatabaseCDrive) )
        {
        CContactDatabase::DeleteDatabaseL(KDatabaseCDrive);
        }
    if (CContactDatabase::ContactDatabaseExistsL(KOldDatabaseCDrive) )
        {
        CContactDatabase::DeleteDatabaseL(KOldDatabaseCDrive);
        }
    }





void CopyCdbFileToCDriveL(const TDesC& aFileName)
    {
    //Will probably need more additions to sos to do this
    RPIMTestServer serv;
    User::LeaveIfError(serv.Connect());
    serv.CopyFileL(aFileName, KDatabaseCDriveFullPath);
    serv.Close();
    }
    
void CopyOldCdbFileToCDriveL(const TDesC& aFileName)
    {
    //Will probably need more additions to sos to do this
    RPIMTestServer serv;
    User::LeaveIfError(serv.Connect());
    serv.CopyFileL(aFileName, KOldDatabaseCDriveFullPath);
    serv.Close();
    }

typedef void (*TTestFunction)(TBool);

void DoTestV8DatabaseL()
    {
    CopyOldCdbFileToCDriveL(KDatabaseV8);
    CContactDatabase* db=CContactDatabase::OpenL(KDatabaseCDrive); 
    TDriveUnit driveUnit = _L("c:");
    TBuf<190> driveName;
    CDesCArray* dataBaseList = db->ListDatabasesL(driveUnit);
    CleanupStack::PushL(dataBaseList);
    TInt theCount = dataBaseList->Count();
    TBool oldFileFormat = EFalse;
    for(TInt i = 0; i< theCount; i++)
        {
        driveName = (*dataBaseList)[i];
        test.Printf(_L("ListDatabaseTestL, ListDatabasesL: %s \n"),driveName.PtrZ());
        TPtrC filenamePtr(driveName);
        TInt found = filenamePtr.FindF(KOldFileFormatPrefix);
        if (found != 0 && found != KErrNotFound)
            {
            oldFileFormat = ETrue;  
            }
        }
    test(oldFileFormat);

    CleanupStack::PopAndDestroy(dataBaseList);
    delete db;
    }

void DoTestV7DatabaseL()
    {
    CopyCdbFileToCDriveL(KDatabaseV7With25Entries);
    CContactDatabase* db=CContactDatabase::OpenL(KDatabaseCDrive); 
    delete db;
    }

void DoTestV6DatabaseL()
    {
    CopyCdbFileToCDriveL(KDatabaseV6With10Entries);
    CContactDatabase* db=CContactDatabase::OpenL(KDatabaseCDrive); 
    delete db;
    }

void DoTestV5DatabaseL()
    {
    CopyCdbFileToCDriveL(KDatabaseV5);
    CContactDatabase* db=CContactDatabase::OpenL(KDatabaseCDrive); 
    delete db;
    }

void DoTestV4DatabaseL(TBool aOOMTest = ETrue)
    {
    if (aOOMTest)
        {
        CopyCdbFileToCDriveL(KDatabaseV4With4Entries);
        }
    else
        {
        CopyCdbFileToCDriveL(KDatabaseV4);
        }
    
    TTime startTime;
    TTime endTime;
        
        
    //Test end profiling
    startTime.UniversalTime(); //start profiling
    CContactDatabase* db=CContactDatabase::OpenL(KDatabaseCDrive); 
    endTime.UniversalTime(); //end profiling
    CleanupStack::PushL(db);
    
    //need to sort item otherwise CountL return 0 - only in old model
/*  CContactDatabase::TSortPref sortPref(KUidContactFieldGivenName);
    CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(1);
    CleanupStack::PushL(sortOrder);
    sortOrder->AppendL(sortPref);
    db->SortL(sortOrder); //Takes ownership
    CleanupStack::Pop(sortOrder);*/

    TInt count=db->CountL();
    
    //Now compute delay and print on the screen
    TTimeIntervalMicroSeconds delay=endTime.MicroSecondsFrom(startTime);
    test.Printf(_L("A v4 database containing %d contacts was converted in %d µs\n"),count,delay.Int64());

    CleanupStack::PopAndDestroy(db); //db
    }

void DoOOMTestL(TTestFunction aTestFunction)
    {
    TInt ret = KErrNoMemory;
    TInt failAt = 0;
    while(ret != KErrNone)
        {
        failAt += 1 + failAt/30;
        test.Printf(_L("OOM step: %d\n"), failAt);
        __UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
        __UHEAP_MARK;
        TRAP(ret, aTestFunction(ETrue));
        __UHEAP_MARKEND;
        __UHEAP_RESET;
        
        if  (ret != KErrNoMemory && ret != KErrNone)
            {
            test.Printf(_L("OOM test failed: %d\n"),ret);
            }

        if (!(ret == KErrNoMemory || ret == KErrNone) )
            {
            TRAP_IGNORE(CleanupFilesL() );
            }
        test(ret == KErrNoMemory || ret == KErrNone);
        }
    }


void DoTestL()
    {
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    CTestRegister* TempFiles = CTestRegister::NewLC();
    TempFiles->RegisterL(KDatabaseCDrive, EFileTypeCnt);

    _LIT(KTestV8Database,"Opening DBMS secured (old) database...");
    _LIT(KTestV7Database,"Opening v7 format database...");
    _LIT(KTestV6Database,"Opening v6 format database...");
    _LIT(KTestV5Database,"Opening v5 format database...");
    _LIT(KTestV4Database,"Opening v4 format database...");
    
    
    test.Next(_L("Opening v7 format database..."));

    DoTestV7DatabaseL();

    test.Next(_L("Opening DBMS secured (old) database..."));

    DoTestV8DatabaseL();

        
    test.Next(_L("Opening v6 format database..."));

    DoTestV6DatabaseL();
    
    test.Next(_L("Opening v5 format database..."));

    DoTestV5DatabaseL();

    test.Next(_L("Opening v4 format database..."));

    DoTestV4DatabaseL(EFalse);

    //v4 conversion to v7 executes code involved in any other conversion.
    DoOOMTestL(DoTestV4DatabaseL); 
    //DoOOMTestL(DoTestV5DatabaseL);
    //DoOOMTestL(DoTestV6DatabaseL);
    //DoOOMTestL(DoTestV7DatabaseL);

    CleanupStack::PopAndDestroy(2); // TempFiles, fs
    }

/**

@SYMTestCaseID     PIM-T-DATACOMPATIBILITY-0001

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
            test.Title();
            test.Start(_L("@SYMTESTCaseID:PIM-T-DATACOMPATIBILITY-0001 T_DataCompatibility"));

            TRAPD(err, DoTestL());
            // delete database files if they are not cleaned up elsewhere 
            TRAP_IGNORE(CleanupFilesL() );
            test(err == KErrNone);
            test.End();
            User::After(1000000); // wait 1 second and exit
            test.Close();
            delete cleanup;
            }
        delete scheduler;
        }
    __UHEAP_MARKEND;
    return KErrNone;
    }
#else

_LIT(KDatabase9_2, "Z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\SQLite__Contacts.cdb");
_LIT(KDatabaseCDriveFullPath,"C:\\private\\10003A73\\SQLite__OLDVERSION_9_2.CDB");
_LIT(KDatabaseCDrive,"C:OLDVERSION_9_2.cdb");

RFs fs;

//////////// Logging macros & utilities ///////////////
LOCAL_D RTest test(KTestName);


LOCAL_C void CleanupFilesL()
	{
    // delete the database files
	if (CContactDatabase::ContactDatabaseExistsL(KDatabaseCDrive) )
		{
		CContactDatabase::DeleteDatabaseL(KDatabaseCDrive);
		}
	}

	
void Copy9_2CdbFileToCDriveL(const TDesC& aFileName)
	{
	//Will probably need more additions to sos to do this
	RPIMTestServer serv;
	User::LeaveIfError(serv.Connect());
	serv.CopyFileL(aFileName, KDatabaseCDriveFullPath);
	serv.Close();
	}

/*
Reads the database and gets the existing column count for this table


TInt GetExistingColumnCountFromDatabaseL()
    {    
    _LIT(KTableName, "contact");
    
    RSqlDatabase iDatabase;
    iDatabase.OpenL( KDatabaseCDriveFullPath );
    
    //Get the table information using the PRAGMA Statement
    RSqlStatement ColumnInfoStatement;
    CleanupClosePushL( ColumnInfoStatement );
    
    HBufC* sqlstmt = HBufC::NewLC( KPragmaStmnt().Length() + 
            KTableName().Length() );
    sqlstmt->Des().Format( KPragmaStmnt, &KTableName() );    
        
    //Get the name and type
    User::LeaveIfError( ColumnInfoStatement.Prepare( iDatabase, *sqlstmt ) );
    TInt count = ColumnInfoStatement.ColumnCount();
        
    CleanupStack::PopAndDestroy() ; //sqlstmt
    CleanupStack::PopAndDestroy( &ColumnInfoStatement );    
    
    return count;
    }*/

typedef void (*TTestFunction)(TBool);

void DoTest9_2_DatabaseL()
	{
	Copy9_2CdbFileToCDriveL(KDatabase9_2);	
	CContactDatabase* db=CContactDatabase::OpenL(KDatabaseCDrive);	
	delete db;	
	}


void DoTestL()
	{
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDatabaseCDrive, EFileTypeCnt);
	
	test.Next(_L("Opening DBMS secured (9.2 old) database..."));
	
	DoTest9_2_DatabaseL();		

	CleanupStack::PopAndDestroy(2); // TempFiles, fs
	}

/**

@SYMTestCaseID     PIM-T-DATACOMPATIBILITY-0001

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
			test.Title();
			test.Start(_L("@SYMTESTCaseID:PIM-T-DATACOMPATIBILITY-0001 T_DataCompatibility"));

			TRAPD(err, DoTestL());
			// delete database files if they are not cleaned up elsewhere 
			TRAP_IGNORE(CleanupFilesL() );
			test(err == KErrNone);
			test.End();
			User::After(1000000); // wait 1 second and exit
			test.Close();
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }


#endif
