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

#include "t_sampprof.h"
#include <bautils.h>
#include <profiler.h>


//************************************************************************************************************
_LIT(KTestName,    "T_SampProf");


TPtrC commandLineHelp[]=
	{
	_L("\n\nCommand Line Syntax:\n"),
	_L("T_SampProf.exe [del | gen=XX] | count=XX test=XX [batch] [prof] [lfp] [help] \n"),
	_L("  help     - Prints this help and list of databases \n"),
	_L("  del      - delete all db's \n"),
	_L("  gen=99   - gen all db's \n"),
	_L("  gen=XX   - gen db number XX \n"),
	_L("  count=XX - XX specifies number of test loops to run \n"),
	_L("  test=XX  - XX specifies the test number \n"),
	_L("  prof     - do profiling (otherwise just timings) \n"),
 	_L("  batch    - batch mode \n"),
	_L("  lfp      - Last Name, First Name, Phone Number sort order (lfc otherwise) \n\n\n")
	};
TInt KCommandLineHelpCount = sizeof(commandLineHelp)/sizeof(TPtrC);


LOCAL_D RTest test(KTestName);
TPerfTimer perfTimer;


#ifdef __SECURE_API__

TPtrC dbNames[]=
	{
   _L("c:level1-100-ATS.cdb"),		//0
   _L("c:simple-100-0.cdb"),			//1
   _L("c:simple-1000-0.cdb"),			//2
   _L("c:simple-100-8.cdb"),			//3
   _L("c:power-100-0.cdb"),			//4
   _L("c:power-1000-0.cdb"),			//5
   _L("c:corporate-100-0.cdb"),		//6
   _L("c:corporate-1000-0.cdb"),		//7
   _L("c:corporate-500-0.cdb"),		//8
   _L("c:simple-500-0.cdb"),			//9
   _L("c:level1-1000-0.cdb"),			//10
   _L("c:level2-1000-0.cdb"),			//11
   _L("c:level3-1000-0.cdb"),			//12
   _L("c:level4-1000-0.cdb")			//13
	};
	
#else

TPtrC dbNames[]=
	{
   _L("c:\\system\\data\\level1-100-ATS.cdb"),		//0
   _L("c:\\system\\data\\simple-100-0.cdb"),			//1
   _L("c:\\system\\data\\simple-1000-0.cdb"),		//2
   _L("c:\\system\\data\\simple-100-8.cdb"),			//3
   _L("c:\\system\\data\\power-100-0.cdb"),			//4
   _L("c:\\system\\data\\power-1000-0.cdb"),			//5
   _L("c:\\system\\data\\corporate-100-0.cdb"),		//6
   _L("c:\\system\\data\\corporate-1000-0.cdb"),	//7
   _L("c:\\system\\data\\corporate-500-0.cdb"),		//8
   _L("c:\\system\\data\\simple-500-0.cdb"),			//9
   _L("c:\\system\\data\\level1-1000-0.cdb"),		//10
   _L("c:\\system\\data\\level2-1000-0.cdb"),		//11
   _L("c:\\system\\data\\level3-1000-0.cdb"),		//12
   _L("c:\\system\\data\\level4-1000-0.cdb")			//13
	};

#endif

TInt KDbNamesCount = sizeof(dbNames)/sizeof(TPtrC);
	
	

//************************************************************************************************************
//   CTestLoop
//************************************************************************************************************
CTestLoop* CTestLoop::NewL(TInt aTest, TInt aCount, TSortOrder aSortOrder, TBool aProf)
   {
	CTestLoop* self = new(ELeave)CTestLoop(aTest, aCount, aSortOrder, aProf);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
   }


//------------------------------------------------------------------------------------------------------------
CTestLoop::CTestLoop(TInt aTest, TInt aCount, TSortOrder aSortOrder, TBool aProf)
:	CActive(CActive::EPriorityStandard),
   iTest(aTest),
   iCount(aCount),
	iLoopCounter(aCount),
	iSortOrder(aSortOrder),
	iProf(aProf)
   {
   CActiveScheduler::Add(this);
   }


//------------------------------------------------------------------------------------------------------------
void CTestLoop::ConstructL()
   {
	_LIT(KParam,"");
	RProcess p;
	TInt err;   
	
	// Run the Sampling Profiler
	if (iProf)
		{
		err=p.Create(KProfilerName,KParam);
		if (err == KErrNone)
			{
			p.Resume();
			p.Close();
			User::After(1000000);
			}
		else
			{
			test.Printf(_L("ERROR %d: Unable to execute Sampling Profiler\n"), err);
			}
		}

	CompleteSelf();
	}


//------------------------------------------------------------------------------------------------------------
TInt CTestLoop::RunError(TInt /*aError*/)
   {
   CActiveScheduler::Stop();
   return KErrNone;
   }
   

//------------------------------------------------------------------------------------------------------------
void CTestLoop::CloseDb()
   {
	Cancel();
	if (iContactRemoteView) 
	   {
	   iContactRemoteView->Close(*this);
	   iContactRemoteView=NULL;
	   }

   if (iContactsDb) 
	   {
	   delete iContactsDb;
	   iContactsDb=NULL;
	   }   
   }


//------------------------------------------------------------------------------------------------------------
CTestLoop::~CTestLoop()
   {
   CloseDb();
   
	if (iProf) Profiler::Unload();
   }
  

//------------------------------------------------------------------------------------------------------------
void CTestLoop::RunL()
   {
	TBuf<100>    buf;
	TInt        ret;      	

   if (iLoopCounter == iCount)
      {
		if (iProf)
			{
			ret=Profiler::Start();
			if (ret != KErrNone)
				{
				test.Printf(_L("ERROR %d : Unable to start Sampling Profiler.\n"),ret);
				User::Leave(ret);
				}
			}
      }
   else
      {
      CloseDb();
      
      perfTimer.GetTime(buf);
      test.Printf(_L("%S Database Closed. Loops left: %d \n\n"),&buf, iLoopCounter);
      }
   
  
   if (iLoopCounter <= 0)  // End of test
      {
		if (iProf)
			{
			ret=Profiler::Stop();
			User::After(300000);
			ret=ret | Profiler::Close();
			User::After(300000);
			ret=ret | Profiler::Unload();
			User::After(300000);         
			if (ret != KErrNone)
				{
				test.Printf(_L("ERROR %d : Unable to Stop/Close/Unload Sampling Profiler\n"),ret);
				User::Leave(ret);            
				}
			}
      CActiveScheduler::Stop();
      }
   else // Next Loop
      {
		perfTimer.Reset();      
      OpenDatabaseL(dbNames[iTest]);
      CreateRemoteViewL();
      }
      
   iLoopCounter--;         
   }


//------------------------------------------------------------------------------------------------------------
void CTestLoop::HandleContactViewEvent(const CContactViewBase& /*aView*/, const TContactViewEvent& /*aEvent*/)
   {
	TBuf<100> buf;
	
   perfTimer.GetTime(buf);
   test.Printf(_L("%S Remote view Callback received.\n"),&buf);
   CompleteSelf();
   }


//------------------------------------------------------------------------------------------------------------
void CTestLoop::DoCancel()
	{
	}


//------------------------------------------------------------------------------------------------------------	
void CTestLoop::CompleteSelf(TInt aReason)
   {
   TRequestStatus* pStat = &iStatus;
	   
  	User::RequestComplete(pStat, aReason);
   SetActive();
   }
   
   
//------------------------------------------------------------------------------------------------------------
void CTestLoop::CreateRemoteViewL()
   {
   RContactViewSortOrder   viewSortOrder;
	TBuf<100> buf;
	
   perfTimer.GetTime(buf);
   test.Printf(_L("%S Creating remote view (sync)."),&buf);
   
   viewSortOrder.AppendL(KUidContactFieldFamilyName);
   viewSortOrder.AppendL(KUidContactFieldGivenName);
   if (iSortOrder==ELFC)
   	{
   	viewSortOrder.AppendL(KUidContactFieldCompanyName);
		test.Printf(_L(" Sort order: LFC.\n"));   	
   	}
   else
   	{
   	viewSortOrder.AppendL(KUidContactFieldPhoneNumber);   
		test.Printf(_L(" Sort order: LFP.\n"));
   	}
   
   iContactRemoteView = CContactRemoteView::NewL(*this, *iContactsDb, viewSortOrder, EContactsOnly);
   viewSortOrder.Close();
   
   perfTimer.GetTime(buf);
   test.Printf(_L("%S Remote view created (sync).\n"),&buf);
   }


//------------------------------------------------------------------------------------------------------------
void CTestLoop::OpenDatabaseL(const TDesC& aDbName)
   {
	TBuf<100> buf;
	
   perfTimer.GetTime(buf);
   test.Printf(_L("%S Opening Database: %S\n"),&buf, &aDbName);
   
   iContactsDb = CContactDatabase::OpenL(aDbName);
   
   perfTimer.GetTime(buf);
   test.Printf(_L("%S Database Opened.\n"),&buf);   
   }
   
   


//************************************************************************************************************
//  GENERATE DATABASES
//************************************************************************************************************
   
//@SYMTestCaseID PIM-TSAMPPROF-0001
   
void genDatabasesL(TInt aDbNumber)
	{
	TBuf<100> buf;
	bool genAllDbs = false;
	
	// 99 means generate all databases
	if (aDbNumber == 99) genAllDbs = true;
	
   test.Start(_L("@SYMTestCaseID PIM-TSAMPPROF-0001 Generate Database  - with timings"));
   perfTimer.Reset();
	   
   perfTimer.GetTime(buf);
   test.Printf(_L("%S Creating the CPerfDbCreator\n"),&buf);
   
   CPerfDbCreator* dbCreator = CPerfDbCreator::NewL();
	CleanupStack::PushL(dbCreator);


	if ((aDbNumber == 0) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
   	test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[0]);
   	dbCreator->GenAllTheSameDbL(ELevel1, 100, true, dbNames[0]);
		}

	if ((aDbNumber == 1) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[1]); 
	   dbCreator->GenDbL(ESimple, 100,  0, true, dbNames[1]);
		}
		
	if ((aDbNumber == 2) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[2]);
	   dbCreator->GenDbL(ESimple, 1000, 0, true, dbNames[2]);
		}
		
	if ((aDbNumber == 3) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[3]);
	   dbCreator->GenDbL(ESimple, 100,  8, true, dbNames[3]);
		}
		
	if ((aDbNumber == 4) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[4]);
	   dbCreator->GenDbL(EPower, 100,  0, true, dbNames[4]);
		}
		
	if ((aDbNumber == 5) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[5]);
	   dbCreator->GenDbL(EPower, 1000, 0, true, dbNames[5]);
		}
		
	if ((aDbNumber == 6) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[6]);
	   dbCreator->GenDbL(ECorporate, 100,  0, true, dbNames[6]);
		}

	if ((aDbNumber == 7) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[7]);   
	   dbCreator->GenDbL(ECorporate, 1000, 0, true, dbNames[7]);
		}
				
	if ((aDbNumber == 8) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[8]);   
	   dbCreator->GenDbL(ECorporate, 500, 0, true, dbNames[8]);
		}
		
	if ((aDbNumber == 9) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[9]);   
	   dbCreator->GenDbL(ESimple, 500, 0, true, dbNames[9]);
		}		
		
	if ((aDbNumber == 10) || (genAllDbs == true))
		{
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[10]);
	   dbCreator->GenDbL(EL1Only, 1000,  0, true, dbNames[10]);
		}
		
	if ((aDbNumber == 11) || (genAllDbs == true))
		{	   
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[11]);
	   dbCreator->GenDbL(EL2Only, 1000,  0, true, dbNames[11]);
		}
		
	if ((aDbNumber == 12) || (genAllDbs == true))
		{	   
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[12]);
	   dbCreator->GenDbL(EL3Only, 1000,  0, true, dbNames[12]);
		}
		
	if ((aDbNumber == 13) || (genAllDbs == true))
		{	   
	   perfTimer.GetTime(buf);
	   test.Printf(_L("%S Generating:%S\n"), &buf, &dbNames[13]);
	   dbCreator->GenDbL(EL4Only, 1000,  0, true, dbNames[13]);         
		}
		
   
   perfTimer.GetTime(buf);
   test.Printf(_L("%S Destroying the CPerfDbCreator\n"),&buf);   

	CleanupStack::PopAndDestroy(dbCreator);
	
	test.End();
	}



//************************************************************************************************************
//  DEL DATABASES
//************************************************************************************************************
	
//@SYMTestCaseID PIM-TSAMPPROF-0002
	
void delDatabasesL()
	{
	TBuf<100> buf;
	
   test.Start(_L("@SYMTestCaseID PIM-TSAMPPROF-0002 Delete Database  - with timings"));
   perfTimer.Reset();   
   
   for (TInt i = 0; i < KDbNamesCount; i++)
	   if (CContactDatabase::ContactDatabaseExistsL(dbNames[i]))
	      {
	      perfTimer.GetTime(buf);      
	      test.Printf(_L("%S Deleting:%S\n"), &buf, &dbNames[i]);
	      CContactDatabase::DeleteDatabaseL(dbNames[i]);
	      }
	      
	test.End();
	}	


//************************************************************************************************************
//  DO TEST
//************************************************************************************************************
	
//@SYMTestCaseID PIM-TSAMPPROF-0003
	
void doTestL(TInt aTest, TInt aCount, TSortOrder aSortOrder, TBool aProf)
	{
   test.Start(_L("@SYMTestCaseID PIM-TSAMPPROF-0003 API Open with View - timings"));

   CTestLoop* testLoop=CTestLoop::NewL(aTest, aCount, aSortOrder, aProf);
   CleanupStack::PushL(testLoop);
   
   CActiveScheduler::Start();
   
	CleanupStack::PopAndDestroy(testLoop);
	
	test.End();
	}


//************************************************************************************************************
// PARSE PARAMETER
//************************************************************************************************************
TInt parseNumberL(TDes &aArgs, const TDesC &aParam)
   {
   TInt     pos, num, ret;
	TBuf<100> argsTmp;
   TLex     lexArgs;	   
   
   pos = aArgs.Find(aParam);
   if (pos == KErrNotFound)
      User::Leave(KErrArgument);

   argsTmp.Insert(0,aArgs);
   argsTmp.Delete(0,pos + aParam.Length());
   lexArgs=argsTmp;
   ret=lexArgs.Val(num);
   
   if (ret != KErrNone)
      User::Leave(KErrArgument);
   
   return num;
   }


//************************************************************************************************************
// PRINT HELP
//************************************************************************************************************
void printHelp()
	{
	TInt i;
	for (i = 0; i < KCommandLineHelpCount; i++) 	
		{
		test.Printf(_L("%S"), &commandLineHelp[i]);
		}
	
	test.Printf(_L("DATABASES:\n"));	
	for (i = 0; i < KDbNamesCount; i++) 	
		{
		test.Printf(_L(" %2d - %S \n"), i, &dbNames[i]);
		}
	}



//************************************************************************************************************
// PRINT BUILD DATE
//************************************************************************************************************
void printBuildDate()
	{
	TBuf<100>         buf;
	TPtrC8           ptrDate8 = (TText8*)__DATE__;
	TPtrC8           ptrTime8 = (TText8*)__TIME__;	
	TBuf16<50>       bufDate16;
	TBuf16<50>       bufTime16;	

	test.Printf(_L("\n\n\n"));
	bufDate16.Copy(ptrDate8);	
	bufTime16.Copy(ptrTime8);
	buf.Append(bufDate16);
	buf.Append(_L("  "));   
	buf.Append(bufTime16);

	test.Printf(_L("*** Build: %S ***\n"), &buf);   
	test.Title();
	}

   
//************************************************************************************************************
//************************************************************************************************************
//  MAIN
//************************************************************************************************************
//************************************************************************************************************
void doMainL()
	{
	TBuf<100>       buf, args;
	TInt            count,testNo;
	TBool			prof=EFalse;
	RFs             fs;
	TSortOrder		sortOrder;

   _LIT(KParamGen,	"gen=");
   _LIT(KParamDel,	"del");
   _LIT(KParamCount,	"count=");
   _LIT(KParamTest,	"test=");
   _LIT(KParamSort,	"lfp");      
	_LIT(KParamBatch,	"batch");   
	_LIT(KParamProf,	"prof");
	_LIT(KParamHelp,	"help");

	
	printBuildDate();
		
   // Get and prepare Command Line
#ifndef __SECURE_API__
   RProcess().CommandLine(args);
#else
   User::CommandLine(args);   
#endif
   args.LowerCase();
   

   // Parse param and decide what to do
   if (args.Find(KParamHelp) != KErrNotFound)
		printHelp();   
   else if (args.Find(KParamDel) != KErrNotFound)
      delDatabasesL();
   else if (args.Find(KParamGen) != KErrNotFound)
      genDatabasesL(parseNumberL(args, KParamGen));
   else
      {
		// ECOM should be already running on a real phone. Does not in testshell mode
		REComSession ecom;
		ecom.OpenL();
		User::After(2000000);	      
      
      // Parse test number and number of test loops
      count  = parseNumberL(args, KParamCount);
      testNo = parseNumberL(args, KParamTest);
      
      // Do profiling?
      if (args.Find(KParamProf) != KErrNotFound)	prof = true;
      
		// Check the sort order
		if (args.Find(KParamSort) != KErrNotFound)
			{
			sortOrder=ELFP;
   		test.Printf(_L("\n\nSORT ORDER: LFP\n\n"));
			}
		else
			{
			sortOrder=ELFC;
   		test.Printf(_L("\n\nSORT ORDER: LFC\n\n"));
			}
				
		// Perform the test
      doTestL(testNo, count, sortOrder, prof);
      
		// Copy the profiler.dat file from drive C: to drive E:
		if (prof)
			{
			User::After(10000000);
			buf.Zero();
			buf.Format(_L("e:\\sp_test%d_count%d.dat"),testNo, count);
			fs.Connect();
			TInt ret=BaflUtils::CopyFile(fs, _L("c:\\PROFILER.DAT"), buf);
			test.Printf(_L("Copy PROFILER.DAT. Code: %d\n"),ret);
			fs.Close();
			}
			
	   ecom.Close();
	   ecom.FinalClose();   
      }
   
   
   if (args.Find(KParamBatch) == KErrNotFound)
      {
      test.Printf(_L("\n\nPress any key.\n"));
      test.Getch();   
      }
      
	test.Close();      
	}



//************************************************************************************************************
//************************************************************************************************************
// Program Entry
//************************************************************************************************************
//************************************************************************************************************
GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	CActiveScheduler* rootScheduler = new CActiveScheduler;
	CActiveScheduler::Install(rootScheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());	
	
	test(ret==KErrNone);
	delete theCleanup;	
	delete rootScheduler;
	__UHEAP_MARKEND;
	return(KErrNone);
	}
