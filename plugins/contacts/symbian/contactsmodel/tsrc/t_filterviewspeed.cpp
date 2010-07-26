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
* This test the speed of how fast it takes to filter a view, with various numbers of contacts. 
* This can be automatic when run as a part of the test suite on the emulator, but 
* on some hardware this can blow up since it can take up more memory that the device has.
* This does not mean the test fails (it's just profiling speed), it just mean the full set of tests can't be run.
* This for for testing defect INC038634 - Contact View filtered view takes too long to open.
*
*/

#include <cntdb.h>
#include <cntdef.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntview.h>

// System includes
#include <e32std.h>
#include <e32twin.h>
#include <e32test.h>
#include <cntdbobs.h>

// User includes
#include "t_utils2.h"

// optional clean up (might want to turn off if you want to keep the db for further tests)
#define DELETE_FILES_WHEN_DONE

//
// Constants.
//

_LIT(KTestName,"t_filterviewspeed");

const TInt KNewContactsPerTest[] = {50, 100, 200, 400, 800, 500, 500, 1000};
const TInt KNumberOfTests=8;
const TInt  KMaxNumberOfFilters = 6;
const TInt  KMaxFilterBits = 10;
const TInt KPrintFreq=50;

// Globals

/** This is used to keep track of the expected numbers to match each filter*/
class TFilterCounter
	{
	public: 
		TInt AddBits(TInt aBits);
		void Reset();
		void Print();
		TFilterCounter();
		TInt Count(CContactDatabase::TContactViewFilter aFilter);

	private: 
		TInt iCounter[KMaxFilterBits];
		void PrintFilterName(CContactDatabase::TContactViewFilter aFilter);
	};




/** Wait for the filter to finish and state results*/
class CWatcher : public CActive, MContactViewObserver
	{
	public:
		static CWatcher* NewL(CContactDatabase::TContactViewFilter aFilter);
		void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);
		~CWatcher();

	private:
		CWatcher(CContactDatabase::TContactViewFilter aFilter);
		void ConstructL();
		void DoCancel();
		void RunL();
		TInt RunError(TInt aTest);
		void CreateFilterL();
		void DoneFilteringL();

		TTime iInitTime;
		TTime iStartFilterTime;
		CContactDatabase* iDatabase;
		RContactViewSortOrder iViewSortOrder;
		CContactRemoteView* iView;
		CContactFilteredView* iFilterer;
		CContactDatabase::TContactViewFilter iFilter;
		TBool iStarted;
		TBool iTestNumber;

	};


CContactIdArray* PopulateTestDatabaseLC(CContactDatabase& aDatabase, TInt aCount );

LOCAL_D RTest test(KTestName);
LOCAL_D TFilterCounter gCounter;



CWatcher::~CWatcher()
	{
	if(iView)
		iView->Close(*this);
	iViewSortOrder.Close();
	if(iFilterer)
		iFilterer->Close(*this);		
	delete iDatabase;
	}

CWatcher::CWatcher(CContactDatabase::TContactViewFilter aFilter) :CActive(0), iFilter(aFilter)	{}

CWatcher* CWatcher::NewL(CContactDatabase::TContactViewFilter aFilter)
	{
	CWatcher* self = new(ELeave) CWatcher(aFilter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
	}


void CWatcher::ConstructL()
	{
	iInitTime.UniversalTime();
	// open new db
	iDatabase = CContactDatabase::ReplaceL();

	// create contacts
	CContactIdArray& contacts = *PopulateTestDatabaseLC(*iDatabase, KNewContactsPerTest[iTestNumber] );
	gCounter.Print();

	// create view
	iViewSortOrder.AppendL(KUidContactFieldFamilyName);
   	iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
	iView = CContactRemoteView::NewL(*this, *iDatabase, iViewSortOrder, EContactsOnly);
	CleanupStack::PopAndDestroy( &contacts );
	CActiveScheduler::Add(this);
	}


void CWatcher::DoCancel() {}

/** Try to start the view anyway, even if we can't get the desired number of contacts */
TInt CWatcher::RunError(TInt /*aTest*/)
	{
	if(!iView)
		{
		TRAPD(err,
			iView = CContactRemoteView::NewL(*this, *iDatabase, iViewSortOrder, EContactsOnly)
		);
		return err;
		}
	return KErrNone;
	}

/**  add more contacts and make a new remote view*/
void CWatcher::RunL()
	{
	iView->Close(*this);		// delete it
	iView = NULL;
	CContactIdArray& contacts = *PopulateTestDatabaseLC(*iDatabase, KNewContactsPerTest[iTestNumber] );
	gCounter.Print();
	CleanupStack::PopAndDestroy( &contacts );
	iView = CContactRemoteView::NewL(*this, *iDatabase, iViewSortOrder, EContactsOnly);
	}

/** when the remote view is ready start the filter. When the filter is ready end the test (or go to next test)*/


void CWatcher::HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent)
	{
	TInt err1(KErrNone);
	TInt err2(KErrNone);
	if(!iStarted && &aView == iView && TContactViewEvent::EReady==aEvent.iEventType) 
		{
		iStarted = ETrue;
		test.Next(_L("Filter time test"));

		TRAP(err1, CreateFilterL() );
		}

	if(iStarted && &aView == iFilterer && TContactViewEvent::EReady==aEvent.iEventType) 
		{
		TRAP(err2, DoneFilteringL() );
		iStarted = EFalse;
		}

	if (err1 || err2)
		{
		_LIT(KErrMsg, "Contact view error in CWatcher::HandleContactViewEvent()\n");
		test.Printf(KErrMsg);
		User::Invariant();
		}
	}

/** start timing and make a new filter */
void CWatcher::CreateFilterL()
	{
	iStartFilterTime.UniversalTime();
	iFilterer = CContactFilteredView::NewL(*this, *iDatabase, *iView, iFilter);
	}

/** report the time and either end the tests or go to the next one */
void CWatcher::DoneFilteringL()
	{
	TTime stopTime;
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds elapsedTime = stopTime.MicroSecondsFrom(iStartFilterTime);
	test.Printf(_L("Number of Contacts: %i\t\t Test time: %ims\n"), iView->CountL(), elapsedTime.Int64()/1000);
	test(gCounter.Count(iFilter) ==	iFilterer->CountL());
	iFilterer->Close(*this);		// this deletes iFilterer
	iFilterer=NULL;
	iTestNumber++;
	if(iTestNumber==KNumberOfTests)
		{
		CActiveScheduler::Stop();
		}
	else 
		{
		TRequestStatus* stat = &iStatus;
		User::RequestComplete(stat,KErrNone);
		SetActive();
		}
	}



TFilterCounter::TFilterCounter()
	{
	Reset();
	}

void TFilterCounter::Reset()
	{
	for(TInt i=0;i<KMaxFilterBits;i++)
		iCounter[i]=0;
	}


TInt TFilterCounter::AddBits(TInt aBits)
	{
	for(TInt i=0;i<KMaxFilterBits;i++)
		{
		if ((1<<i) & aBits) 
			iCounter[i]++;
		}
	return aBits;
	}

void TFilterCounter::PrintFilterName(CContactDatabase::TContactViewFilter aFilter)
	{
	switch(aFilter)
		{
		case CContactDatabase::EUnfiltered: test.Printf(_L("Unfiltered"));break;
		case CContactDatabase::EMailable: test.Printf(_L("Mailable"));break;
		case CContactDatabase::ESmsable: test.Printf(_L("Smsable"));break;
		case CContactDatabase::ELandLine: test.Printf(_L("LandLine"));break;
		case CContactDatabase::EFaxable: test.Printf(_L("Faxable"));break;
		case CContactDatabase::EPhonable: test.Printf(_L("Phonable"));break;
		case CContactDatabase::EWork: test.Printf(_L("Work"));break;
		case CContactDatabase::EHome: test.Printf(_L("Home"));break;
		case CContactDatabase::ERingTone: test.Printf(_L("RingTone"));break;
		case CContactDatabase::EVoiceDial: test.Printf(_L("VoiceDial"));break;
		case CContactDatabase::EIMAddress: test.Printf(_L("IMAddress"));break;
		case CContactDatabase::EWirelessVillage: test.Printf(_L("WirelessVillage"));break;
		case CContactDatabase::ECustomFilter1: test.Printf(_L("CustomFilter1"));break;
		case CContactDatabase::ECustomFilter2: test.Printf(_L("CustomFilter2"));break;
		case CContactDatabase::ECustomFilter3: test.Printf(_L("CustomFilter3"));break;
		case CContactDatabase::ECustomFilter4: test.Printf(_L("CustomFilter4"));break;
		}
	}

void TFilterCounter::Print()
	{
	for(TInt i=0;i<KMaxNumberOfFilters;i++)
		{
		PrintFilterName((CContactDatabase::TContactViewFilter) (1<<i));
		test.Printf(_L(": %i\t"), iCounter[i]);
		}
	test.Printf(_L("\n"));;
	}

TInt TFilterCounter::Count(CContactDatabase::TContactViewFilter aFilter)
	{
	for(TInt i=0;i<KMaxFilterBits;i++)
		{
		if((1<<i) == aFilter)
			return iCounter[i];
		}
	return 0;
	}


/** Ranomly generate up to KMaxNumberOfFilters (6)  filters for a contact*/
TInt RandomFilterBits(TInt64& aSeed)
	{
	TInt bits = 0;
	TInt i;
	for(i=Math::Rand(aSeed)%KMaxNumberOfFilters;i>=0;i--)
		{
		bits |= 1 << (Math::Rand(aSeed)%(KMaxFilterBits+1));		// random TContactViewFilter
		}
	return bits;
	}

/** add aCount random contacts to the open database. The 1st item will always be the own card */
CContactIdArray* PopulateTestDatabaseLC(CContactDatabase& aDatabase, TInt aCount )
	{
	__ASSERT_ALWAYS(aCount>0, User::Invariant());
	CContactIdArray* contacts = CContactIdArray::NewLC();
	CRandomContactGenerator* generator = CRandomContactGenerator::NewL();
	CleanupStack::PushL( generator );
	generator->SetDbL(aDatabase);

	TInt64 seed=47;
	TContactItemId itemId;
	for(TInt counter = 0;counter<aCount;counter++)
		{
 		itemId = generator->AddTypicalContactForFilterL(gCounter.AddBits(RandomFilterBits(seed)));
		if(counter%KPrintFreq==0)
			{
			aDatabase.CompactL();
			test.Printf(_L("Added contact %d: %i\n"), counter, itemId);
			}
		contacts->AddL(itemId);
		}
	CleanupStack::PopAndDestroy( generator );
	return contacts;
	}

/** Delete default contacts DB ignoring any errors
	@return KErrNone if the DB is deleted */
TInt DeleteDbFile()
	{		
	TRAPD(err,CContactDatabase::DeleteDefaultFileL())
	return err;
	}

void TestL(CContactDatabase::TContactViewFilter aFilter)
	{

	CActiveScheduler* scheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL( scheduler );


	CWatcher* watch = CWatcher::NewL(aFilter);
	
	CActiveScheduler::Start();
	delete watch;

#ifdef DELETE_FILES_WHEN_DONE
	DeleteDbFile(); //cntmodel need CActiveScheduler to run this function
#endif

	CleanupStack::PopAndDestroy(scheduler );
	}



void doMainL()
	{
	__UHEAP_MARK;
	TestL(CContactDatabase::EMailable);
	__UHEAP_MARKEND;
	}



/**

@SYMTestCaseID     PIM-T-FILTERVIEWSPEED-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-FILTERVIEWSPEED-0001 "));

	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup)
		{
		//Get rid of the warnings.
		//T_utils2 uses efsrv.dll, but we don't use this functions in this test code.
		//So we pretend to use efsrv to keep T_utils2.obj and linker happy
		RFs fs;
		fs.Connect();
		fs.Close();

		TRAP_IGNORE(doMainL());	
		/* The error does not matter. It could be out of memory or disk full or others, 
		but the only errors that matter are the ones tested for already.
		This tests what it can and if there's not enough room to test more, it stops*/
		delete cleanup;
		}
	test.End();
	test.Close();
	
	__UHEAP_MARKEND;
	return KErrNone;
    }
