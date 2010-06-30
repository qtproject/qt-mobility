/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <cntitem.h>
#include <cntfldst.h>
#include "t_utils2.h"
#include "t_database_killer.h"


// Test Macro
LOCAL_D RTest test(_L("T_DATABASE_KILLER"));

_LIT(KTestName,"t_database_killer");
_LIT(KLogFileName,"t_database_killer.log");

_LIT(KDbFileName,"c:t_database_killer.cdb");

const TInt KNumSmsContacts=5;
const TInt KNumWorkEmailContacts=4;
const TInt KNumHomeEmailContacts=4;
const TInt KNumLandlineContacts=10;
const TInt KNumFaxContacts=4;

CTestConductor* CTestConductor::NewL(TBool aKillFourTimes)
	{
	CTestConductor* self=new(ELeave) CTestConductor(aKillFourTimes);
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
	delete iTempFiles;
	iFs.Close();
	}

CTestConductor::CTestConductor(TBool aKillFourTimes)
	{
	iKillFourTimes = aKillFourTimes;
	}

void CTestConductor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iTempFiles=CTestRegister::NewLC();
	CleanupStack::Pop(iTempFiles);
	iTempFiles->RegisterL(KDbFileName, EFileTypeCnt);
	iLog=CLog::NewL(test,KLogFileName);
	iDb=CContactDatabase::ReplaceL(KDbFileName);
	}


void CTestConductor::RunTestsL()
	{
	CDatabaseTester* tester=CDatabaseTester::NewL(*iLog,*iDb,this, iKillFourTimes);
	CleanupStack::PushL(tester);
	CActiveScheduler::Start();
	CleanupStack::Pop(tester);
	// error from CDatabaseTester ?
	User::LeaveIfError(iTestError);
	}

void CTestConductor::SetTestError(TInt aTestError)
	{
	iTestError = aTestError;
	}

//
// CDatabaseTester.
//

CDatabaseTester* CDatabaseTester::NewL(CLog& aLog,CContactDatabase& aDb,CTestConductor* aTestConductor, TBool aKillFourTimes)
	{
	CDatabaseTester* self=new(ELeave) CDatabaseTester(aLog,aDb,aTestConductor, aKillFourTimes);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CDatabaseTester::~CDatabaseTester()
	{
	delete iRandomGenerator;

	DeleteProcess();

	}

CDatabaseTester::CDatabaseTester(CLog& aLog,CContactDatabase& aDb,CTestConductor* aTestConductor, TBool aKillFourTimes)
	: CActive(EPriorityStandard),iLog(aLog),iDb(aDb),iCurrentTest(EInitialisingTests),iTestConductor(aTestConductor)
	{
	CActiveScheduler::Add(this);
	iTotalContacts = 0;
	iKillFourTimes = aKillFourTimes;
	}

void CDatabaseTester::ConstructL()
	{
	iRandomGenerator=CRandomContactGenerator::NewL();
	iRandomGenerator->SetDbL(iDb);
	NextTest();
	}

void CDatabaseTester::RunL()
	{
	CContactIdArray* arrayOfContacts = NULL;
	TInt num = 0;
	TInt counter = 0;
	TInt halfOfAllContacts = 0;
	switch (iCurrentTest)
		{
		case EAddContacts:
			iLog.LogLine(_L("=== Add Contacts to DB"));
			AddContactsL();
			NextTest();
			break;

		case EKillThreadFirstTime:
			iLog.LogLine(_L("=== Kill Server 1st Time"));
			arrayOfContacts = iDb.FindLC(_L(""), NULL);
			num = arrayOfContacts->Count();
			test(num == iTotalContacts);
			FindServerAndKillItL(2000);
			NextTest();
			break;

		case ERemoveContacts:
			iLog.LogLine(_L("==== Remove Some Contacts"));
			arrayOfContacts = iDb.FindLC(_L(""), NULL);
			num = arrayOfContacts->Count();
			test(num == iTotalContacts);

			halfOfAllContacts = num / 2;
			for ( counter = 0; counter < halfOfAllContacts; counter++)
				{
				iDb.DeleteContactL((*arrayOfContacts)[counter]);
				iTotalContacts--;
				}
			CleanupStack::PopAndDestroy(arrayOfContacts);
			arrayOfContacts = iDb.FindLC(_L(""), NULL);
			num = arrayOfContacts->Count();
			test(num == iTotalContacts);
			NextTest();
			break;

		case EKillThreadSecondTime:
			iLog.LogLine(_L("=== Kill Server 2nd Time"));
			arrayOfContacts = iDb.FindLC(_L(""), NULL);
			num = arrayOfContacts->Count();
			test(num == iTotalContacts);
			FindServerAndKillItL(2000);
			NextTest();
			break;

		case EAddMoreContacts:
			iLog.LogLine(_L("=== Add More Contacts to DB"));
			AddContactsL();
			NextTest();
			break;

		case EKillThreadThirdTime:
			iLog.LogLine(_L("=== Kill Server 3rd Time"));
			arrayOfContacts = iDb.FindLC(_L(""), NULL);
			num = arrayOfContacts->Count();
			test(num == iTotalContacts);
			FindServerAndKillItL(2000);
			NextTest();
			break;

		case ERemoveMoreContacts:
			iLog.LogLine(_L("==== Remove Some More Contacts"));
			arrayOfContacts = iDb.FindLC(_L(""), NULL);
			num = arrayOfContacts->Count();
			test(num == iTotalContacts);

			halfOfAllContacts = num / 2;
			for ( counter = 0; counter < halfOfAllContacts; counter++)
				{
				iDb.DeleteContactL((*arrayOfContacts)[counter]);
				iTotalContacts--;
				}
			CleanupStack::PopAndDestroy(arrayOfContacts);
			arrayOfContacts = iDb.FindLC(_L(""), NULL);
			num = arrayOfContacts->Count();
			test(num == iTotalContacts);
			NextTest();
			break;

		case EKillThreadForthTime:
			if (iKillFourTimes)
				{
				iLog.LogLine(_L("=== Kill Server 4th Time"));
				arrayOfContacts = iDb.FindLC(_L(""), NULL);
				num = arrayOfContacts->Count();
				test(num == iTotalContacts);
				FindServerAndKillItL(2000);
				}
			else
				{
				iLog.LogLine(_L("=== Skipping : Kill Server 4th Time"));
				}
			NextTest();
			break;

		case ENumTests:
			iLog.LogLine(_L("==== Filter View Finished"));
			arrayOfContacts = iDb.FindLC(_L(""), NULL);
			num = arrayOfContacts->Count();
			test(num == iTotalContacts);
			CActiveScheduler::Stop();
			delete this;
			break;

		default:
			ASSERT(EFalse);
			break;
		}

	if (arrayOfContacts != NULL)
		{
		CleanupStack::PopAndDestroy(arrayOfContacts);
		}
	}


TInt CDatabaseTester::RunError(TInt aError)
	{
	// propagate error back to CTestConductor
	iTestConductor->SetTestError(aError);

	// some diagnostic info on where the test failed
	switch (iCurrentTest)
		{
		case EAddContacts: test.Printf(_L("Failed at test step AddContacts (%i) error %i"), iCurrentTest, aError); break;
		case EKillThreadFirstTime: test.Printf(_L("Failed at test step KillThreadFirstTime (%i) error %i"), iCurrentTest, aError); break;
		case ERemoveContacts: test.Printf(_L("Failed at test step RemoveContacts (%i) error %i"), iCurrentTest, aError); break;
		case EKillThreadSecondTime: test.Printf(_L("Failed at test step KillThreadSecondTime (%i) error %i"), iCurrentTest, aError); break;
		case EAddMoreContacts: test.Printf(_L("Failed at test step AddMoreContacts (%i) error %i"), iCurrentTest, aError); break;
		case EKillThreadThirdTime: test.Printf(_L("Failed at test step KillThreadThirdTime (%i) error %i"), iCurrentTest, aError); break;
		case ERemoveMoreContacts: test.Printf(_L("Failed at test step RemoveMoreContacts (%i) error %i"), iCurrentTest, aError); break;
		case EKillThreadForthTime: test.Printf(_L("Failed at test step KillThreadForthTime (%i) error %i"), iCurrentTest, aError); break;

		case ENumTests: test.Printf(_L("Failed at test step NumTests (%i) error %i"), iCurrentTest, aError); break;

		default: test.Printf(_L("Failed at test step %i error %i"), iCurrentTest, aError); break;
		}

	// stop test
	CActiveScheduler::Stop();

	return KErrNone;
	}


void CDatabaseTester::NextTest()
	{
	iCurrentTest = static_cast<TTest>( static_cast<TInt>(iCurrentTest) + 1 );
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}



void CDatabaseTester::DoCancel()
	{
	}

void CDatabaseTester::FindServerAndKillItL(TInt aDelay)
	{
	if (aDelay > 0) User::After(aDelay);
	SetupProcessL();
	OpenAndKillProcess();
	WaitForServerToClose();
	}

void CDatabaseTester::AddContactsL()
	{
	test.Printf(_L("Adding "));
	TInt ii;
	for (ii=0;ii<KNumSmsContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::ESmsable;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	for (ii=0;ii<KNumWorkEmailContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::EMailable;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	for (ii=0;ii<KNumHomeEmailContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::EMailable;
		bit |= CContactDatabase::EHome;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	for (ii=0;ii<KNumLandlineContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::ELandLine;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	for (ii=0;ii<KNumFaxContacts;ii++)
		{
		TInt bit=0;
		bit |= CContactDatabase::EFaxable;
		iRandomGenerator->AddTypicalContactForFilterL(bit);
		iTotalContacts++;
		}
	}

void CDatabaseTester::OpenAndKillProcess()
	{
	iLog.LogLine(_L("==== Open and Kill Process"));
	/*TInt retVal = */iProcessOrThread.iProcess.iExecution->Open(iProcessOrThreadFullName);
	iProcessOrThread.iProcess.iExecution->Kill(KErrNone);
	}

void CDatabaseTester::OpenAndKillThread()
	{
	iLog.LogLine(_L("==== Open and Kill Thread"));
	iProcessOrThread.iThread.iExecution->Open(iProcessOrThreadFullName);
	iProcessOrThread.iThread.iExecution->Kill(KErrNone);
	}

void CDatabaseTester::WaitForServerToClose()
	{
	const TInt KRetryPeriod(100000);
	const TInt KMaxRetries(50);
	
	TInt attempts = 0;
	do
		{
		// Attempt to find the server process
		TRAPD(err, SetupProcessL());
		if (err == KErrNotFound)
			{
			// Server not found - it has closed
			return;
			}
		
		
		// Not found, wait then try again
		User::After(KRetryPeriod);
		
		_LIT(KRetryReportFormat, "Close server attempt %d failed\n");
		test.Printf(KRetryReportFormat, attempts + 1);
		++attempts;
		
		} while (attempts < KMaxRetries);
	
	// Failed to close contacts server
	_LIT(KServerCloseFailed, "Warning: Unable to verify contacts server has closed\n");
	test.Printf(KServerCloseFailed);
	}

void CDatabaseTester::SetupThreadL()
	{
	DeleteThread();
	iProcessOrThread.iThread.iFind = new (ELeave) TFindThread(_L("CntLockServer*"));
	iProcessOrThread.iThread.iExecution = new (ELeave) RThread();
	iProcessOrThread.iThread.iFind->Next(iProcessOrThreadFullName);
	}

void CDatabaseTester::SetupProcessL()
	{
	DeleteProcess();
	iProcessOrThread.iProcess.iFind = new (ELeave) TFindProcess(_L("CNTSRV*"));
	iProcessOrThread.iProcess.iExecution = new (ELeave) RProcess();
	User::LeaveIfError(iProcessOrThread.iProcess.iFind->Next(iProcessOrThreadFullName));
	}

void CDatabaseTester::DeleteThread()
	{
	delete iProcessOrThread.iThread.iFind;
	if (iProcessOrThread.iThread.iExecution != NULL) iProcessOrThread.iThread.iExecution->Close();
	delete iProcessOrThread.iThread.iExecution;

	iProcessOrThread.iThread.iFind = NULL;
	iProcessOrThread.iThread.iExecution = NULL;
	}

void CDatabaseTester::DeleteProcess()
	{
	delete iProcessOrThread.iProcess.iFind;
	if (iProcessOrThread.iProcess.iExecution != NULL) iProcessOrThread.iProcess.iExecution->Close();
	delete iProcessOrThread.iProcess.iExecution;

	iProcessOrThread.iProcess.iFind = NULL;
	iProcessOrThread.iProcess.iExecution = NULL;
	}

void CDatabaseTester::AddContactL(TInt aBitwiseFilterType)
	{
	test.Printf(_L("Adding Contact"));
	iRandomGenerator->AddTypicalContactForFilterL(aBitwiseFilterType);
	iTotalContacts++;
	}

//
// Main.
//

/**

@SYMTestCaseID     PIM-T-DATABASE-KILLER-0001

*/

GLDEF_C TInt E32Main()
	{
	TInt error = KErrNone;
	__UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CTestConductor* testConductor=NULL;
			// NEWL parameters :
			//  EFalse - kill the server 3 times
			//  ETrue  - kill the server 4 times
			//
			// - The client should keep track of the number of times the server is restarted.
			// - restarting the server more than 3 times should result in the client being paniced.
			// - this has been tested, and the ability to test for this has been included for completeness.
			test.Start(_L("@SYMTESTCaseID:PIM-T-DATABASE-KILLER-0001 t_database_killer"));

			TRAP(error,testConductor=CTestConductor::NewL(EFalse));
			RDebug::Print(_L("T_database_killer test completed \n"));
			test(error == KErrNone);
			test.End();
			delete testConductor;
			test.Close();
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
