/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Tests Contacts Database can be opened and read with only ReadUserData capability
*
*/


#include <e32std.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_utils.h"
#include <testserver.h>
#include "t_platsec.h"

//
// Configuration.
//


//
// Constants.
//

_LIT(KTestName,"t_platsec");

#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
//_LIT(KDbFileName58920,"c:SQLite__Contacts58920.cdb");
_LIT(KDbFileName58920,"c:Contacts58920.cdb");
_LIT(KSourceDbFileName58920,"z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\SQLite__Contacts58920.cdb");
_LIT(KDestDbFileName58920,"SQLite__Contacts58920.cdb");
#else //__SYMBIAN_CNTMODEL_USE_SQLITE__
_LIT(KDbFileName58920,"c:contacts58920.cdb");
_LIT(KSourceDbFileName58920,"z:\\Core-Apps\\App-Engines\\CntModel\\Databases\\contacts58920.cdb");
_LIT(KDestDbFileName58920,"contacts58920.cdb");
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__

_LIT(KLogFileName,"t_platsec.log");


//
// CPlatSecTest.
//

CPlatSecTest* CPlatSecTest::NewLC(RTest& aTest)
	{
	CPlatSecTest* self = new CPlatSecTest(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPlatSecTest::CPlatSecTest(RTest& aTest) : iTest(aTest)
	{
	}

CPlatSecTest::~CPlatSecTest()
	{
	if (iTestServer.Handle() != 0)
		{ // cleanup temp database
		HBufC* fullDbName = GetFullDbName(KDestDbFileName58920);
		if(fullDbName)
		    {
    		TRAP_IGNORE(iTestServer.DeleteFileL(*fullDbName));
    		delete fullDbName;
		    }
		iTestServer.Close();
		}
	delete iDb;
	delete iLog;
	iFs.Close();
	iTest.Close();
	}

void CPlatSecTest::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iTestServer.Connect());

	// Copy test contacts database
	HBufC* fullDbName = GetFullDbName(KDestDbFileName58920);
	User::LeaveIfNull(fullDbName);

    CleanupStack::PushL(fullDbName);
	iTestServer.CopyFileL(KSourceDbFileName58920, *fullDbName);
    CleanupStack::PopAndDestroy(fullDbName);

	iLogFileName=KLogFileName;
	iDbFileName=KDbFileName58920;
	iLog=CLog::NewL(iTest,iLogFileName);
	}

void CPlatSecTest::IdleFindCallback()
	{
	}

void CPlatSecTest::DoTestL()
	{

	TPtrC match;

	// Test for DEF58920 (Contacts not usable if only ReadUserData capabilitiy)
	// Note absence of WriteUserData in .mmp file


	iLog->LogLine(_L("Open database with ReadUserData capability only"));
	// Prior to bug fix used to get the following when doing an open
	// *PlatSec* ERROR - Capability check failed
    // - A Message (function number=0x464000f1) from Thread
    // T_PLATSEC.EXE[00000000]0001::Main, sent to Server
    // !DBMS server, was checked by Thread EDBSRV.EXE[100012a5]0001::!DBMS
    // server and was found to be missing the capabilities: WriteUserData .
    // t_platsec finishes with -46 error
	iDb=CContactDatabase::OpenL(iDbFileName);


	iLog->LogLine(_L("DatabaseDrive()"));
	TDriveUnit driveUnit;
	TBool bHasBeenSet = iDb->DatabaseDrive(driveUnit);

	iLog->LogLine(_L("GetCurrentDatabase()"));
	TFileName currentDb;
	iDb->GetCurrentDatabase(currentDb);

	iLog->LogLine(_L("CountL()"));
    TInt numberContacts = iDb->CountL();

	iLog->LogLine(_L("SortedItemsL()"));
    CContactIdArray* idList=CContactIdArray::NewLC(iDb->SortedItemsL());
	const TInt index=(*idList)[0];

	iLog->LogLine(_L("ReadContactLC()"));
	CContactItem* item= iDb->ReadContactLC(index);

	CContactItemFieldSet& fields=item->CardFields();
	const TInt fieldCount=fields.Count();

	CContactItemField &field1 = fields[1];
	TPtrC fieldText1=field1.TextStorage()->Text();

	// check that can read out of database
	iTest.Next(_L("Reading Contact field"));

#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
  	iTest(fieldText1 == _L("john"));
#else //__SYMBIAN_CNTMODEL_USE_SQLITE__
  	iTest(fieldText1 == _L("9AB24C76BmQAte[POqjm_PoyFam\\\\cKq"));
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__

    CleanupStack::PopAndDestroy(item);
    CleanupStack::PopAndDestroy(idList);

	//
	}

void RunTestsL(RTest& aTest)
	{
	CPlatSecTest* platSecTest=CPlatSecTest::NewLC(aTest);

	platSecTest->DoTestL();

	CleanupStack::PopAndDestroy(platSecTest);
	}


//
// Main.
//

/**

@SYMTestCaseID     PIM-T-PLATSEC-0001

*/

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("t_platsec started"));
	__UHEAP_MARK;
#ifndef __SECURE_API__
	RProcess process;
	TBuf<256> commandLine;
	process.CommandLine(commandLine);
#else
	TBuf<256> commandLine;
	User::CommandLine(commandLine);
#endif
	TInt result(KErrNone);
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			RTest test(KTestName);

			test.Start(_L("@SYMTESTCaseID:PIM-T-PLATSEC-0001 Test for DEF58920"));


			TRAP(result,RunTestsL(test));
			test(result == KErrNone);

			// test SUCCESS
			test.End();
			test.Close();
			delete cleanup;
			}

		delete scheduler;
		}
	__UHEAP_MARKEND;
	return result;
    }
