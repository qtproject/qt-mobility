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

// User includes
#include "t_utils2.h"

// Constant
_LIT(KDatabaseFileName, "c:t_owncard.cdb");
_LIT(KSemaphoreNameOne,		"One");
_LIT(KSemaphoreNameTwo,		"Two");

_LIT(KThreadNameOne,		"OneThread");
_LIT(KThreadNameTwo,		"TwoThread");


const TInt KDelay = 1000000;

static RTest					TheTest(_L("t_owncard"));


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



///////////////////////////////////////////////////////////////////////////////
/* CConcurrentDatabaseAccessBase - Specification                             */
// Forms the base class for both threads.
class CConcurrentDatabaseAccessBase : public CActive
	{
	public: // Data Types
		enum TStage
			{
			EOne_Startup,
			ETwo_FirstCheck,
			EThree_ChangeOwner,
			ENOP_One,
			ENOP_Two,
			EFour_SecondCheck,
			EFive_Cleanup,
			EFinished
			} ;
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
		void CloseTest();

		virtual TBool RunTestImplementationL() = 0;

	protected: // from CActive
		void DoCancel() {};
		void RunL();
		TInt RunError(TInt aError);
		void CallRunLAgain();
		TBool NextStage();


	protected:
		CContactDatabase*		iDb;
		TStage					iStage;
		TInt					iTestError;

	private:
		RSemaphore				iSemaphoreSignal;
		RSemaphore				iSemaphoreWait;
		TBool					iSemaphoreOpen;
		HBufC*					iDatabaseName;
	};

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentDatabaseAccessBase - Implementation                            */
CConcurrentDatabaseAccessBase::CConcurrentDatabaseAccessBase()
	: CActive( /*EPriorityStandard*/ EPriorityIdle )
	{
	iStage = EOne_Startup;
	}

CConcurrentDatabaseAccessBase::~CConcurrentDatabaseAccessBase()
	{
	iThread.Close();
	CloseSemaphore();
	delete iDatabaseName;
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

void CConcurrentDatabaseAccessBase::CloseTest()
	{
	}

void CConcurrentDatabaseAccessBase::CallRunLAgain()
	{
	iStatus = KRequestPending;

	TRequestStatus* ptrStatus = &iStatus;
	User::RequestComplete( ptrStatus, KErrNone );
	SetActive();
	}

TBool CConcurrentDatabaseAccessBase::NextStage()
	{
	TInt stage = static_cast<TInt>( iStage );
	stage++;
	iStage = static_cast<TStage>( stage );
	return ( iStage == EFinished );
	}

void CConcurrentDatabaseAccessBase::RunL()
	{
	TBool stopProcessing = EFalse;

	stopProcessing = RunTestImplementationL();

	if (stopProcessing)
		{
		CActiveScheduler::Stop();
		}
	}

TInt CConcurrentDatabaseAccessBase::RunError(TInt aError)
	{
	// propagate the error
	iTestError = aError;

	RDebug::Print(_L("Thread failed at test stage %i, with error %i"), iStage, aError);

	CActiveScheduler::Stop();
	return KErrNone;
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

	CActiveScheduler::Add( this );

	OpenDatabaseL();

	CallRunLAgain();
	CActiveScheduler::Start();

	CloseDatabase();

	CleanupStack::PopAndDestroy( scheduler );

	// propagate error
	User::LeaveIfError(iTestError);
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
	CallRunLAgain();
	iSemaphoreSignal.Signal();
	iSemaphoreWait.Wait();
	}

void CConcurrentDatabaseAccessBase::SyncronizeWait()
	{
	CallRunLAgain();
	iSemaphoreSignal.Wait();
	iSemaphoreWait.Signal();
	}

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentOwnerCardModifier - Specification                               */
// This class should allow the two threads to compare the owner ID's of there db.
class TOwnerCardCompare
	{
	public:
	enum EThreadName
		{
			EModifier,
			EChecker
		};
	public:
	TOwnerCardCompare();
	~TOwnerCardCompare();
	void			AssignValuesAndCheckL(TContactItemId aValue, EThreadName aThreadName);

	private:
	void			CheckValuesL();
	void			ClearValues();

	private: // data
	TBool			iModifierDataPresent;
	TBool			iCheckerDataPresent;
	TContactItemId	iModifierValue;
	TContactItemId	iCheckerValue;
	};

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentOwnerCardModifier - Implementatio                              */
// This class should allow the two threads to compare the owner ID's of there db.
TOwnerCardCompare::TOwnerCardCompare()
	{
	ClearValues();
	}
TOwnerCardCompare::~TOwnerCardCompare()
	{
	ClearValues();
	}

void TOwnerCardCompare::AssignValuesAndCheckL(TContactItemId aValue, EThreadName aThreadName)
	{
	switch ( aThreadName )
		{
		case EModifier:
			iModifierDataPresent = ETrue;
			iModifierValue = aValue;
		break;

		case EChecker:
			iCheckerDataPresent = ETrue;
			iCheckerValue = aValue;
		break;
		}
	CheckValuesL();
	}

void TOwnerCardCompare::CheckValuesL()
	{
	if ( iModifierDataPresent && iCheckerDataPresent )
		{
		TheTest( (iCheckerValue == iModifierValue) );
		ClearValues();
		}
	}

void TOwnerCardCompare::ClearValues()
	{
	iCheckerDataPresent = EFalse;
	iModifierDataPresent = EFalse;
	iCheckerValue = 0;
	iModifierValue = 0xFFFFFFFF;
	}

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentOwnerCardModifier - Specification                               */
// This class should create the db, and set the owner card.
class CConcurrentOwnerCardModifier : public CConcurrentDatabaseAccessBase
	{
	public:
		CConcurrentOwnerCardModifier(TOwnerCardCompare& aOwnerCardCompare);
		~CConcurrentOwnerCardModifier();

		static CConcurrentOwnerCardModifier* NewLC(const TDesC& aFilename, TOwnerCardCompare& aOwnerCardCompare);
		virtual void ConstructL(const TDesC& aFilename);

		static TInt ThreadFunction(TAny* aSelf);


	protected:
		TBool	RunTestImplementationL();

	private:
		void					AssignAndCompareOwnerCardIdL(TContactItemId aOwner);

	private:
		TOwnerCardCompare&		iOwnerCardCompare;
	};

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentOwnerCardModifier - Implementation                              */

CConcurrentOwnerCardModifier* CConcurrentOwnerCardModifier::NewLC(const TDesC& aFilename, TOwnerCardCompare& aOwnerCardCompare)
	{
	CConcurrentOwnerCardModifier* self = NULL;
	self = new (ELeave) CConcurrentOwnerCardModifier(aOwnerCardCompare);
	CleanupStack::PushL( self );
	self->ConstructL(aFilename);
	return self;
	}

CConcurrentOwnerCardModifier::CConcurrentOwnerCardModifier(TOwnerCardCompare& aOwnerCardCompare)
	: iOwnerCardCompare( aOwnerCardCompare )
	{
	}

CConcurrentOwnerCardModifier::~CConcurrentOwnerCardModifier()
	{
	}

TInt CConcurrentOwnerCardModifier::ThreadFunction(TAny* aSelf)
	{
	CConcurrentOwnerCardModifier* self = STATIC_CAST(CConcurrentOwnerCardModifier*, aSelf);

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

void CConcurrentOwnerCardModifier::ConstructL(const TDesC& aFilename)
	{
	CConcurrentDatabaseAccessBase::ConstructL(aFilename);

	iThread.Create( KThreadNameOne, CConcurrentOwnerCardModifier::ThreadFunction, KDefaultStackSize, 0x2000, 0x20000, this, EOwnerThread );
	iThread.Resume();
	iThread.SetPriority(/*EPriorityNormal*/EPriorityAbsoluteBackground);
	}

// 1. Set up
// 2. Check current owner card.
// 3. Sync
// 4. modify owner card
// 5. Sync
// 6. Check owner card
// 7. Sync
// 8. Finished.
TBool CConcurrentOwnerCardModifier::RunTestImplementationL()
	{
	TInt bit = 0;
	TContactItemId id;
	CRandomContactGenerator* generator = NULL;
	CContactItem* item = NULL;
	TBool retval = EFalse;

	switch( iStage )
		{
		case EOne_Startup:
			OpenSemaphore();
			retval = NextStage();
			SyncronizeWait(); // pause until corresponding Searcher thread is ready.
		break;

		case ETwo_FirstCheck:
			id = iDb->OwnCardId();
			AssignAndCompareOwnerCardIdL( id );
			retval = NextStage();
			SyncronizeWait();
		break;

		case EThree_ChangeOwner:
			generator = CRandomContactGenerator::NewL();
			CleanupStack::PushL( generator );
			generator->SetDbL(*iDb);

			bit |= CContactDatabase::ESmsable;
			id = generator->AddTypicalContactForFilterL(bit);
			item = iDb->ReadContactL( id );
			CleanupStack::PushL( item );
			iDb->SetOwnCardL( *item );
			CleanupStack::PopAndDestroy( item );
			item = NULL;
			CleanupStack::PopAndDestroy( generator );
			generator = NULL;
			retval = NextStage();
			SyncronizeWait(); // wait here.
		break;

		case ENOP_One:
			CallRunLAgain();
			retval = NextStage();
		break;

		case ENOP_Two:
			CallRunLAgain();
			retval = NextStage();
		break;

		case EFour_SecondCheck:
			iDb->CountL();
			id = iDb->OwnCardId();
			AssignAndCompareOwnerCardIdL( id );
			retval = NextStage();
			SyncronizeWait();
		break;

		case EFive_Cleanup:
			CloseSemaphore();
			CloseTest();
			retval = ETrue;
		break;

		default:
		break;
		}

	return retval;
	}


void CConcurrentOwnerCardModifier::AssignAndCompareOwnerCardIdL(TContactItemId aOwner)
	{
	//SyncronizeWait();
	iOwnerCardCompare.AssignValuesAndCheckL( aOwner, TOwnerCardCompare::EModifier );
	//SyncronizeWait();
	//SyncronizeWait();
	}

///////////////////////////////////////////////////////////////////////////////
/* CConcurrentOwnerCardChecker                                               */
class CConcurrentOwnerCardChecker : public CConcurrentDatabaseAccessBase
	{
	public:
		CConcurrentOwnerCardChecker(TOwnerCardCompare& aOwnerCardCompare);
		~CConcurrentOwnerCardChecker();

		static CConcurrentOwnerCardChecker* NewLC(const TDesC& aFilename, TOwnerCardCompare& aOwnerCardCompare);
		virtual void ConstructL(const TDesC& aFilename);
		static TInt ThreadFunction(TAny* aSelf);
		static TInt FakeFunction(TAny *aParams);



	protected:
		TBool	RunTestImplementationL();

	private:
		void	AssignAndCompareOwnerCardIdL(TContactItemId aOwner);

	private:
		TOwnerCardCompare& iOwnerCardCompare;
	};

CConcurrentOwnerCardChecker* CConcurrentOwnerCardChecker::NewLC(const TDesC& aFilename, TOwnerCardCompare& aOwnerCardCompare)
	{
	CConcurrentOwnerCardChecker* self = NULL;
	self = new (ELeave) CConcurrentOwnerCardChecker(aOwnerCardCompare);
	CleanupStack::PushL( self );
	self->ConstructL(aFilename);
	return self;
	}

CConcurrentOwnerCardChecker::CConcurrentOwnerCardChecker(TOwnerCardCompare& aOwnerCardCompare)
	: iOwnerCardCompare( aOwnerCardCompare )
	{
	}

CConcurrentOwnerCardChecker::~CConcurrentOwnerCardChecker()
	{
	}

TInt CConcurrentOwnerCardChecker::ThreadFunction(TAny* aSelf)
	{
	CConcurrentOwnerCardChecker* self = static_cast<CConcurrentOwnerCardChecker*>(aSelf);

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

void CConcurrentOwnerCardChecker::ConstructL(const TDesC& aFilename)
	{
	CConcurrentDatabaseAccessBase::ConstructL(aFilename);

	iThread.Create( KThreadNameTwo, CConcurrentOwnerCardChecker::ThreadFunction, KDefaultStackSize, 0x2000, 0x20000, this, EOwnerThread );
	iThread.Resume();
	iThread.SetPriority(/*EPriorityNormal*/EPriorityAbsoluteBackground);

	}


TInt CConcurrentOwnerCardChecker::FakeFunction(TAny* /*aParams*/)
	{
	return(KErrNone);
	}

TBool CConcurrentOwnerCardChecker::RunTestImplementationL()
	{
	// Prep, and get ready to run. Then before starting the search loop
	// wait for the other thread.
	TContactItemId id;
	TBool retval = EFalse;

	switch( iStage )
		{
		case EOne_Startup:
			//TCallBack callBack(FakeFunction);
			OpenSemaphore();
			retval = NextStage();
			SyncronizeSignal(); // wait for the other thread.
		break;

		case ETwo_FirstCheck:
			id = iDb->OwnCardId();
			AssignAndCompareOwnerCardIdL( id );
			retval = NextStage();
			SyncronizeSignal();
		break;

		case EThree_ChangeOwner:
			retval = NextStage();
			SyncronizeSignal();
		break;

		case ENOP_One:
			CallRunLAgain();
			retval = NextStage();
		break;

		case ENOP_Two:
			CallRunLAgain();
			retval = NextStage();
		break;

		case EFour_SecondCheck:
			iDb->CountL();
			id = iDb->OwnCardId();
			AssignAndCompareOwnerCardIdL( id );
			retval = NextStage();
			SyncronizeSignal();
		break;

		case EFive_Cleanup:
			CloseSemaphore();
			CloseTest();
			retval = NextStage();
		break;

		default:
		break;
		}

	return retval;
	}


void CConcurrentOwnerCardChecker::AssignAndCompareOwnerCardIdL(TContactItemId aOwner)
	{
	//SyncronizeSignal();
	//SyncronizeSignal();
	iOwnerCardCompare.AssignValuesAndCheckL( aOwner, TOwnerCardCompare::EChecker );
	//SyncronizeSignal();
	}

///////////////////////////////////////////////////////////////////////////////
/* Test Function Prototypes                                                  */
///////////////////////////////////////////////////////////////////////////////
void CreateTestDatabaseL(const TDesC& aFilename);
void TestL();
void doMainL();

///////////////////////////////////////////////////////////////////////////////
/* Test Function Implementations                                             */
///////////////////////////////////////////////////////////////////////////////
void CreateTestDatabaseL(const TDesC& aFilename )
	{
	TInt counter = 0;
	TInt bit = 0;
	CContactDatabase* database = NULL;
	CRandomContactGenerator* generator = NULL;
	CContactItem* ownerCardItem = NULL;
	TContactItemId ownerCard = 0;

	database = CContactDatabase::ReplaceL(aFilename);
	CleanupStack::PushL( database );

	generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL( generator );

	generator->SetDbL(*database);


	bit |= CContactDatabase::ESmsable;
	ownerCard = generator->AddTypicalContactForFilterL(bit);
	TheTest.Printf(_L("Adding SMS contacts %d\n"), counter);


	ownerCardItem = database->ReadContactL( ownerCard );
	CleanupStack::PushL( ownerCardItem );
	database->SetOwnCardL( *ownerCardItem );
	CleanupStack::PopAndDestroy( ownerCardItem ); ownerCardItem = NULL;

	CleanupStack::PopAndDestroy( generator );
	CleanupStack::PopAndDestroy( database );
	}


void TestL()
	{
	TRequestStatus thread1;
	TRequestStatus thread2;
	TOwnerCardCompare compareTool;
	CConcurrentOwnerCardChecker* searcher = NULL;
	CConcurrentOwnerCardModifier* inserter = NULL;

	CreateTestDatabaseL( KDatabaseFileName );

	searcher = CConcurrentOwnerCardChecker::NewLC( KDatabaseFileName, compareTool );
	User::After(KDelay); // so that checker thread created first
	inserter = CConcurrentOwnerCardModifier::NewLC( KDatabaseFileName, compareTool );

	searcher->iThread.Logon(thread1);
	inserter->iThread.Logon(thread2);

	User::WaitForRequest(thread2);
	User::WaitForRequest(thread1);

	CleanupStack::PopAndDestroy( inserter );
	CleanupStack::PopAndDestroy( searcher );

	// fail if either thread failed
	User::LeaveIfError(thread1.Int());
	User::LeaveIfError(thread2.Int());
	}


////////////////////////////////////////////////////////////////////////////////////
// -------> Static global functions (source)
////////////////////////////////////////////////////////////////////////////////////
void doMainL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CTestRegister * TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDatabaseFileName, EFileTypeCnt);

	CTestActiveScheduler*  scheduler = new (ELeave) CTestActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);



	// Delete any existing ini file so that we can be sure this test is ok
	TestL();

	CleanupStack::PopAndDestroy( 3 ); // scheduler, tempfiles, fs

	}


/**

@SYMTestCaseID     PIM-T-OWNCARD-0001

*/

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanupStack = NULL;
	__UHEAP_MARK;
	TheTest.Start(_L("@SYMTESTCaseID:PIM-T-OWNCARD-0001 Multi session testcode"));

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
