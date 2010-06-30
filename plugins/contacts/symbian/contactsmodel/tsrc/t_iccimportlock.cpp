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
#include <e32math.h>

// User includes
#include "t_utils.h"
#include "t_utils2.h"

// Constant
_LIT(KDatabaseFileName, "c:INC016848.cdb");

_LIT(KSemaphoreNameOne,		"One");
_LIT(KSemaphoreNameTwo,		"Two");

_LIT(KThreadNameOne,		"OneThread ICC");
_LIT(KThreadNameTwo,		"TwoThread Group");


const TInt KNumberOfGroupInserts = 200;
const TInt KNumberOfInserts = 9*KNumberOfGroupInserts/4;

const TInt KStandardTimeOut = 50000;


static RTest					TheTest(_L("INC016848 - DB stressing - Group insert"));
static RFs						TheFs;

///////////////////////////////////////////////////////////////////////////////
/* CTestActiveScheduler                                                      */
//  This class has been nicked from the t_currentdb.h / .cpp files
class CTestActiveScheduler : public CActiveScheduler
	{
	public:
		void Error (TInt aError) const;
	};
void CTestActiveScheduler::Error(TInt aError) const
	{
	User::Panic(_L("AScheduler"),aError);
	}


LOCAL_C void CleanupFilesL()
	{
    // delete the database file
	if (CContactDatabase::ContactDatabaseExistsL(KDatabaseFileName) )
		{
		CContactDatabase::DeleteDatabaseL(KDatabaseFileName);
		}
	}


///////////////////////////////////////////////////////////////////////////////
/* CConcurrentDatabaseAccessBase - Specification                             */
// Forms the base class for both threads.
class CConcurrentDatabaseAccessBase : public CBase
	{
	public:
		CConcurrentDatabaseAccessBase();
		virtual ~CConcurrentDatabaseAccessBase();

		virtual void ConstructL(const TDesC& aFilename);

		void RunTestL();


	public: // data
		RThread					iThread;

	protected:
		void OpenSemaphore();
		void CloseSemaphore();
		void SyncronizeSignal();
		void SyncronizeWait();
		void OpenDatabaseL();
		void CloseDatabase();
		void SetUpTestL(const TDesC& aThreadName);
		void CloseTest();

		void OpenLogFileL(const TDesC& aFilename);
		void CloseLogFile();

		virtual void RunTestImplementationL() = 0;
		void MoveWindow(TInt aDx);

	protected:
		CContactDatabase*		iDb;
		//CConsoleBase*			iConsole;
		RTest*					iTest;
		RFile					iFile;


	private:
		RSemaphore				iSemaphoreSignal;
		RSemaphore				iSemaphoreWait;
		TBool					iSemaphoreOpen;
		HBufC*					iDatabaseName;
		CTestRegister* 			iFileRegister;
	};

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentDatabaseAccessBase - Implementation                            */
CConcurrentDatabaseAccessBase::CConcurrentDatabaseAccessBase()
	{
	}

CConcurrentDatabaseAccessBase::~CConcurrentDatabaseAccessBase()
	{
	iThread.Close();
	CloseSemaphore();
	delete iDatabaseName;
	if (iFileRegister)
		delete iFileRegister;
	}

void CConcurrentDatabaseAccessBase::ConstructL(const TDesC& aFilename)
	{
	iDatabaseName = aFilename.AllocL();
	}

void CConcurrentDatabaseAccessBase::OpenDatabaseL()
	{
	iDb = CContactDatabase::OpenL(*iDatabaseName, CContactDatabase::EMultiThread);
	}

void CConcurrentDatabaseAccessBase::CloseDatabase()
	{
	delete iDb;
	iDb = NULL;
	}

void CConcurrentDatabaseAccessBase::SetUpTestL(const TDesC& aThreadName)
	{
	// Set's up extra resources.
	CConsoleBase* console = NULL;
	TSize size;

	// Create and name an RTest
	iTest = new(ELeave) RTest(aThreadName);
	iTest->Start(_L("Starting test"));

	// Position our console window
	size = iTest->Console()->ScreenSize();
	size.iWidth = size.iWidth / 2 - 2;
	size.iHeight = size.iHeight - 3;

	console = Console::NewL(aThreadName, size);
	delete const_cast<RTest*>(iTest)->Console();

	iTest->SetConsole(console);
	console->ClearScreen();
	}

void CConcurrentDatabaseAccessBase::CloseTest()
	{
	iTest->Close();
	delete iTest;
	iTest = NULL;
	}

void CConcurrentDatabaseAccessBase::OpenLogFileL(const TDesC& aFilename)
	{
	if (iFileRegister)
		{
		delete iFileRegister;
		iFileRegister = NULL;
		}
	iFileRegister = CTestRegister::NewLC();
	CleanupStack::Pop(iFileRegister);

	// Open Debug File
	iFileRegister->CreateLogFileLC(iFile, aFilename);
	CleanupStack::Pop(&iFile);
	}

void CConcurrentDatabaseAccessBase::CloseLogFile()
	{
	iFile.Close();
	delete iFileRegister;
	iFileRegister = NULL;
	}


void CConcurrentDatabaseAccessBase::RunTestL()
	{
	// set's up an active scheduler for the thread
	// and calls the RunTesImplementation function to actually
	// preform the tests. This function should be implemented in the
	// child class.
	CTestActiveScheduler*  scheduler = NULL;

	scheduler = new (ELeave) CTestActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	OpenDatabaseL();

	RunTestImplementationL();

	CloseDatabase();

	CleanupStack::PopAndDestroy( scheduler );
	}

void CConcurrentDatabaseAccessBase::OpenSemaphore()
	{
	TInt success = KErrNone;

	success = iSemaphoreSignal.OpenGlobal( KSemaphoreNameOne );
	if ( success == KErrNotFound )
		{
		iSemaphoreSignal.CreateGlobal( KSemaphoreNameOne, 0 );
		success = KErrNone;
		}


	success = iSemaphoreWait.OpenGlobal( KSemaphoreNameTwo );
	if ( success == KErrNotFound )
		{
		iSemaphoreWait.CreateGlobal( KSemaphoreNameTwo, 0 );
		}

	iSemaphoreOpen = ETrue;
	}

void CConcurrentDatabaseAccessBase::CloseSemaphore()
	{
	if (iSemaphoreOpen)
		{
		iSemaphoreSignal.Close();
		iSemaphoreWait.Close();
		}

	iSemaphoreOpen = EFalse;
	}

void CConcurrentDatabaseAccessBase::SyncronizeSignal()
	{
	iSemaphoreSignal.Signal();
	iSemaphoreWait.Wait();
	}

void CConcurrentDatabaseAccessBase::SyncronizeWait()
	{
	iSemaphoreSignal.Wait();
	iSemaphoreWait.Signal();
	}


void CConcurrentDatabaseAccessBase::MoveWindow(TInt aDx)
	{
	const TInt scrHgtChars=iTest->Console()->ScreenSize().iHeight;
	TMachineInfoV1Buf machineBuf;
	UserHal::MachineInfo(machineBuf);
	const TMachineInfoV1& machineInfo=machineBuf();
	const TSize scrSizePixels=machineInfo.iDisplaySizeInPixels;
	const TInt xCentre=scrSizePixels.iWidth/2;
	const TInt xStart=scrSizePixels.iWidth/8;
	const TInt xEnd=3*scrSizePixels.iWidth/8;
	const TInt xMoveBy=(xEnd-xStart)/3;
	const TInt yCentre=scrSizePixels.iHeight/2;
	const TInt yEnd=yCentre+scrSizePixels.iHeight/scrHgtChars;
	TRawEvent event;
	event.Set(TRawEvent::EButton1Down,xCentre+aDx*xStart,yCentre);
	UserSvr::AddEvent(event);
	event.Set(TRawEvent::EPointerMove,xCentre+aDx*(xStart+xMoveBy),yEnd);
	UserSvr::AddEvent(event);
	event.Set(TRawEvent::EPointerMove,xCentre+aDx*(xEnd-xMoveBy),yEnd);
	UserSvr::AddEvent(event);
	/*event.Set(TRawEvent::EPointerMove,xCentre+aDx*xEnd,yEnd);
	UserSvr::AddEvent(event);*/
	event.Set(TRawEvent::EButton1Up,xCentre+aDx*xEnd,yEnd);
	UserSvr::AddEvent(event);
	}


///////////////////////////////////////////////////////////////////////////////
/* CConcurrentDatabaseInserter - Specification                               */
// This class is the inserter thread. It's thread function should insert
// contacts into the database using the t_utils CRandomContactGenerator class
class CConcurrentDatabaseInserter : public CConcurrentDatabaseAccessBase
	{
	public:
		CConcurrentDatabaseInserter();
		~CConcurrentDatabaseInserter();

		static CConcurrentDatabaseInserter* NewLC(const TDesC& aFilename);
		virtual void ConstructL(const TDesC& aFilename);

		static TInt ThreadFunction(TAny* aSelf);


	protected:
		void	RunTestImplementationL();

	private:
	};

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentDatabaseInserter - Implementation                              */

CConcurrentDatabaseInserter* CConcurrentDatabaseInserter::NewLC(const TDesC& aFilename)
	{
	CConcurrentDatabaseInserter* self = NULL;
	self = new (ELeave) CConcurrentDatabaseInserter();
	CleanupStack::PushL( self );
	self->ConstructL(aFilename);
	return self;
	}

CConcurrentDatabaseInserter::CConcurrentDatabaseInserter()
	{
	}

CConcurrentDatabaseInserter::~CConcurrentDatabaseInserter()
	{
	}

TInt CConcurrentDatabaseInserter::ThreadFunction(TAny* aSelf)
	{
	CConcurrentDatabaseInserter* self = STATIC_CAST(CConcurrentDatabaseInserter*, aSelf);

	// Prepare the stuff required before we start the
	// active scheduler.
	TInt error = KErrNone;
    CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;

	// Call virtual handler which does anything that needs doing as
	// a result of the thread function from being created.
	TRAP(error, self->RunTestL());


    delete cleanup;
	return error;
	}

void CConcurrentDatabaseInserter::ConstructL(const TDesC& aFilename)
	{
	CConcurrentDatabaseAccessBase::ConstructL(aFilename);

	iThread.Create( KThreadNameOne, CConcurrentDatabaseInserter::ThreadFunction, KDefaultStackSize, 0x2000, 0x20000, this, EOwnerThread );
	iThread.Resume();
	iThread.SetPriority(EPriorityLess);
	}


void CConcurrentDatabaseInserter::RunTestImplementationL()
	{
	TInt retries = 0;
	TInt counter = 0;
	TInt errorCount = 0;
	TInt bit = 0;
	TInt leaveCode = KErrNone;
	HBufC8* fileBuffer = HBufC8::NewL( 256 );
	CleanupStack::PushL( fileBuffer );
	TPtr8 ptr = fileBuffer->Des();

	CRandomContactGenerator* generator = NULL;

	SetUpTestL(KThreadNameOne());

	generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL( generator );
	generator->SetDbL(*iDb);
	OpenLogFileL(_L("CConcurrentDatabaseInserter.txt"));

	bit|= CContactDatabase::ESmsable;

	OpenSemaphore();
	SyncronizeWait(); // pause until corresponding Searcher thread is ready.

	SyncronizeWait(); // Wait while other thread moves there window
	MoveWindow(1);
	SyncronizeWait(); // Tell other thread our window has moved

	for (counter = 0; counter < KNumberOfInserts ; counter++)
		{

		retries = 0;

		do
			{
			leaveCode = KErrNone;
			retries++;
			generator->AddTypicalContactForFilterL(bit, leaveCode);
			iTest->Printf(_L("Insert contact %d Error=%d\n"), counter, leaveCode );
			if ( leaveCode != KErrNone )
				{
				errorCount ++;
				User::After( KStandardTimeOut * retries );
				}
			}
		while ( leaveCode != KErrNone );

		User::After( KStandardTimeOut ); // delay to allow second thread access.
		}

	iTest->Printf(_L("Insert contacts errorCount (retries) = %d\n"), errorCount );

	CleanupStack::PopAndDestroy( generator );
	CleanupStack::PopAndDestroy( fileBuffer );
	ptr.Set(NULL, 0, 0);

	iTest->Printf(_L("Finished, waiting for the other thread to stop\n"));
	SyncronizeWait();
	CloseSemaphore();
	CloseLogFile();
	CloseTest();
	}

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentDBGroupInserter                                               */
class CConcurrentDBGroupInserter : public CConcurrentDatabaseAccessBase
	{
	public:
		CConcurrentDBGroupInserter();
		~CConcurrentDBGroupInserter();

		static CConcurrentDBGroupInserter* NewLC(const TDesC& aFilename);
		virtual void ConstructL(const TDesC& aFilename);
		static TInt ThreadFunction(TAny* aSelf);
		static TInt FakeFunction(TAny *aParams);



	protected:
		void	RunTestImplementationL();

	private:
	};

CConcurrentDBGroupInserter* CConcurrentDBGroupInserter::NewLC(const TDesC& aFilename)
	{
	CConcurrentDBGroupInserter* self = NULL;
	self = new (ELeave) CConcurrentDBGroupInserter();
	CleanupStack::PushL( self );
	self->ConstructL(aFilename);
	return self;
	}

CConcurrentDBGroupInserter::CConcurrentDBGroupInserter()
	{
	}

CConcurrentDBGroupInserter::~CConcurrentDBGroupInserter()
	{
	}

TInt CConcurrentDBGroupInserter::ThreadFunction(TAny* aSelf)
	{
	CConcurrentDBGroupInserter* self = static_cast<CConcurrentDBGroupInserter*>(aSelf);

	// Prepare the stuff required before we start the
	// active scheduler.
	TInt error = KErrNone;

    CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;

	// Call virtual handler which does anything that needs doing as
	// a result of the thread function from being created.
	TRAP(error, self->RunTestL());


    delete cleanup;
	return error;
	}

void CConcurrentDBGroupInserter::ConstructL(const TDesC& aFilename)
	{
	CConcurrentDatabaseAccessBase::ConstructL(aFilename);

	iThread.Create( KThreadNameTwo, CConcurrentDBGroupInserter::ThreadFunction, KDefaultStackSize, 0x2000, 0x20000, this, EOwnerThread );
	iThread.Resume();
	iThread.SetPriority(EPriorityNormal);

	}


TInt CConcurrentDBGroupInserter::FakeFunction(TAny* /*aParams*/)
	{
	return(KErrNone);
	}

void CConcurrentDBGroupInserter::RunTestImplementationL()
	{
	// Prep, and get ready to run. Then before starting the search loop
	// wait for the other thread.
	_LIT(KGroupName, "GroupName[%d]");
	TInt counter = 0;
	TInt errorCount = 0;
	TInt error = 0;
	CContactItem* item = NULL;
	TBuf<80> groupNameBuffer;

	SetUpTestL(KThreadNameTwo());
	OpenSemaphore();
	SyncronizeSignal(); // wait for the other thread.

	MoveWindow(-1);
	SyncronizeSignal();	// Tell other thread our window has moved
	SyncronizeSignal(); // Wait until other thread has moved it's window

	User::After( KStandardTimeOut ); // simulate user delay,

	const TInt KDelayIncrease=10000;
	const TInt KDelayMinimum=10000;
	const TInt KIncreaseEvery=4;
	TInt delay=KDelayMinimum;
	for (counter = 0; counter < KNumberOfGroupInserts; counter++)
		{
		User::After(delay);
		groupNameBuffer.Format( KGroupName, counter );

		FOREVER
			{
			// Trap the search, and display the error code on the screen
			TRAP(error,
				{
				item = iDb->CreateContactGroupLC( KGroupName, /* t/f ? */ EFalse );
				CleanupStack::PopAndDestroy( item );
				item = NULL;
				});

			iTest->Printf(_L("Insert Group %d Error=%d\n"), counter, error );
			if( error == KErrNone )
				break;

			errorCount ++;
			// bail for any error other than Locked
			if (error != KErrLocked)
				{
				TRAP_IGNORE(CleanupFilesL());
				}
			(*iTest)( error == KErrLocked );
			}

		if (counter%KIncreaseEvery==KIncreaseEvery-1)
			delay+=KDelayIncrease;
		}

	iTest->Printf(_L("Create contact groups errorCount (retries) = %d\n"), errorCount );

	iTest->Printf(_L("Finished, waiting for the other thread to stop\n"));

//The errorCount counts the number of times the call to CreateContactGroupLC above returns
//KErrLocked. On hardware it is expected that KErrLocked will occur when a write is attempted and the
//other thread has a lock on the database.  Due to the Wins threading model this should be 0 on Wins.
//This harness can therefore be used as a benchmark on harware and the number of KErrLocked
//contentions observed manually.
#ifdef __WINS__
	if (errorCount != 0)
		{
		TRAP_IGNORE(CleanupFilesL());
		}
	(*iTest)( errorCount == 0 );
#endif
	SyncronizeSignal();
	CloseSemaphore();
	CloseTest();
	}

///////////////////////////////////////////////////////////////////////////////
/* Test Function Prototypes                                                  */
///////////////////////////////////////////////////////////////////////////////
void CreateTestDatabaseL(const TDesC& aFilename );
void TestL();
void doMainL();

///////////////////////////////////////////////////////////////////////////////
/* Test Function Implementations                                             */
///////////////////////////////////////////////////////////////////////////////
void CreateTestDatabaseL(const TDesC& aFilename )
	{
	CContactDatabase* database = NULL;

	database = CContactDatabase::ReplaceL(aFilename);
	CleanupStack::PushL( database );

	CleanupStack::PopAndDestroy( database );
	}


void TestL()
	{
	TRequestStatus groupThread;
	TRequestStatus inserterThread;
	CConcurrentDBGroupInserter* groupInserter = NULL;
	CConcurrentDatabaseInserter* inserter = NULL;
	TCntProfile profile[1];

	CreateTestDatabaseL( KDatabaseFileName );

	CCntTest::ProfileStart(0);

	groupInserter = CConcurrentDBGroupInserter::NewLC( KDatabaseFileName );
	inserter = CConcurrentDatabaseInserter::NewLC( KDatabaseFileName );

	groupInserter->iThread.Logon(groupThread);
	inserter->iThread.Logon(inserterThread);

	User::WaitForRequest(groupThread);
	User::WaitForRequest(inserterThread);

	CCntTest::ProfileEnd(0);
	CCntTest::ProfileResult(profile, 0, 1);

	RDebug::Print(_L("Time taken (secs) %f\n"), profile[0].iTime/1000000.0);

	CleanupStack::PopAndDestroy( inserter );
	CleanupStack::PopAndDestroy( groupInserter );

	}


////////////////////////////////////////////////////////////////////////////////////
// -------> Static global functions (source)
////////////////////////////////////////////////////////////////////////////////////
void doMainL()
	{
	User::LeaveIfError( TheFs.Connect() );
	CleanupClosePushL(TheFs);

	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDatabaseFileName, EFileTypeCnt);

	// Delete any existing ini file so that we can be sure this test is ok
	TestL();

	CleanupStack::PopAndDestroy(2, &TheFs );

	}

/**

@SYMTestCaseID     PIM-T-ICCIMPORTLOCK-0001

*/

GLDEF_C TInt E32Main()
	{
	CTestActiveScheduler*  scheduler = new CTestActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanupStack = NULL;
	__UHEAP_MARK;
	TheTest.Start(_L("@SYMTESTCaseID:PIM-T-ICCIMPORTLOCK-0001 Multi session testcode"));

	TheTest.Title();
	cleanupStack = CTrapCleanup::New();
	TRAPD(ret, doMainL());
    // ensure files are cleaned up even if doMainL() leaves
    TRAP_IGNORE(CleanupFilesL() );
	TheTest(ret == KErrNone);
	delete cleanupStack;
	delete scheduler;

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
