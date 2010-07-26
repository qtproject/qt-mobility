/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @publishedAll
 @released
*/

#include <testexecutelog.h>
#include <cntfldst.h>
#include <e32panic.h>
#include <e32cmn.h>
#include "clientserver.h"
#include "contactviewtest.h"
#include "viewobserver.h"

_LIT(KRun1,"EmptyAll");
_LIT(KRun2,"FullContacts");
_LIT(KRun3,"FullView");
_LIT(KRun4,"FullAll");
_LIT(KRun5,"EmptyFieldset");
_LIT(KRun6,"EmptyFieldsetView");
_LIT(KRun7,"MissingFields");
_LIT(KRun8,"Performance");
_LIT(KRun9,"CompressRecover");
_LIT(KRun10,"ConcurrentCNR");

_LIT(KTest1,"Testing Empty contacts and empty view");
_LIT(KTest2,"Testing Full contacts and empty view");
_LIT(KTest3,"Testing Empty contacts and Full view");
_LIT(KTest4,"Testing Full contacts and Full view");
_LIT(KTest5,"Testing Contacts with empty fieldsets and Full view");
_LIT(KTest6,"Testing Contacts with empty fieldsets and empty view");
_LIT(KTest7,"Testing Contacts with 1 or more empty fields and empty view");
_LIT(KTest8,"***Testing Empty Contacts Views performance***");
_LIT(KTest81,"***Testing Medium Contacts Views performance***");
_LIT(KTest82,"***Testing Full Contacts Views performance***");
_LIT(KTest9,"Compress and recovery test");
_LIT(KTest10,"Concurrent Compress and recovery test");

static const TInt KManyPerfIters = 1;
_LIT(KUfield,"field");
_LIT(KMissingFields, "MissingFields");
_LIT(KPLView, "PLView");
_LIT(KRecover, "Recover_" );

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
_LIT(KContactsDB,"c:\\private\\10003A73\\Contacts.cdb");
#else
_LIT(KContactsDB,"C:\\private\\10003A73\\SQLite__Contacts.cdb");
#endif

_LIT(KPersistenceDB,"c:\\private\\10273EFB\\Contacts.cdb");

#define ALLPRINT( x )	INFO_PRINTF1( x );\
						RDebug::Print( x );

#define ALLPRINT2( x, y )	INFO_PRINTF2( x, y );\
							RDebug::Print( x, y );


#ifdef _WINS
#define KMaxTimeAllowed	500 //seconds, minimum performance
#else
#define KMaxTimeAllowed	10000 //hardware is slower
#endif

_LIT(KStars,"***************");
_LIT(KStartRecover,"StartRecover");
_LIT(KEndRecover,"EndRecover");
_LIT(KStartCompress,"StartCompress");
_LIT(KEndCompress,"EndCompress");

//Number of contacts
CContactViewTest::CContactViewTest(CCntPerfServer &aParent):CCntBaseStep(10,aParent),
															iFullness(EBlank),
															iPerformanceContacts( EFew )
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KContactViewTest);
	}

void CContactViewTest::PreTestL()
	{
	iSemaphore.CreateLocal( 0 );
	iSemaphore2.CreateLocal( 0 );
	const TDesC &KSection = ConfigSection();
	iPLView = ( KSection.Find( KPLView ) == 0 ); // section has to start with plview
	iRecoverFlag = ( KSection.Find( KRecover ) == 0 );
	}

TVerdict CContactViewTest::doTestStepPostambleL()
	{
	iSemaphore.Close();
	EndRecoverL();
	EndCompressL();
	return CCntBaseStep::doTestStepPostambleL();
	}

void CContactViewTest::CleanupL()
	{
	if( iObserver )
		{
		iObserver->CleanView();
		CLEAR( iObserver );
		}

    CLEAR( iContactsDatabase );
	CLEAR( iContactsDatabase2 );
	if( iPLView )
		{
		CLEAR(iPersistence);
		if( iPimServ )
			{
			iPimServ->DeleteFileL(KPersistenceDB);
			iPimServ->Close();
			CLEAR(iPimServ);
			}
		}
	if( iSortOrder )
		{
		iSortOrder->Close();
		CLEAR(iSortOrder);
		}
	CCntBaseStep::Cleanup();
	}

void CContactViewTest::InitializeL()
	{
	CCntBaseStep::InitializeL();
	if( iPLView )
		{
		iPimServ = new(ELeave) RPIMTestServer();
		User::LeaveIfError(iPimServ->Connect());
		User::LeaveIfError( iParent->Fs().Connect() );
		}
	OpenPersistanceL();//opens iContactsDatabase2
	iSortOrder = new(ELeave) RContactViewSortOrder();
	iObserver = CViewObserver::NewLC( *this, &CContactViewTest::AllEmptyTestL );// first test to run
	CleanupStack::Pop( iObserver );
	this->Logger().HtmlLogger().ShareAuto();
	iObserver->Activate();
	}

TVerdict CContactViewTest::doTestStepL()
	{

	void (CContactViewTest::*mainFunc)() = NULL;

	const TDesC &run = ConfigSection();

	if( run.Find( KRun1 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::AllEmptyTestL;
		}
	else if( run.Find( KRun2 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::ViewEmptyTestL;
		}
	else if( run.Find( KRun3 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::ContactEmptyTestL;
		}
	else if( run.Find( KRun4 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::AllFullTestL;
		}
	else if( run.Find( KRun5 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::EmptyFieldsetTestL;
		}
	else if( run.Find( KRun6 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::EmptyFieldsetSortTestL;
		}
	else if( run.Find( KRun7 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::MissingFieldTestL;
		}
	else if( run.Find( KRun8 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::PerformanceTestL;
		}
	else if( run.Find( KRun9 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::RecoverCompressTestL; // this test does not apply to the SQLite version of contacts
		}
	else if( run.Find( KRun10 ) > KErrNotFound )
		{
		mainFunc = &CContactViewTest::ConcurrentRecoverCompressTestL; // this test does not apply to the SQLite version of contacts
		}
	else
		{
		MissngTestPanic();
		}

	__UHEAP_MARK;

	iRemote = EFalse;
	ALLPRINT(KStars);
	if( iPLView )
		{
		_LIT(KPersistance,"Initialising LocalView on Persistence Layers tests");
		ALLPRINT(KPersistance);
		}
	else
		{
		_LIT(KLocalViewCS,"Initialising LocalView on Client/Server tests");
		ALLPRINT(KLocalViewCS);
		}
	ALLPRINT(KStars);
	InitializeL();
	iObserver->iMainFunc = mainFunc;
	CActiveScheduler::Start();

	if( !iPLView )
		{
		ALLPRINT(KStars);
		_LIT(KRemoteViewCS,"Initialising RemoteView on Client/Server tests");
		ALLPRINT(KRemoteViewCS);
		ALLPRINT(KStars);
		iRemote = ETrue;
        // Ensure that the recover is deleted before another one is created.
        EndRecoverL();
		ResetDatabaseL(10);
		iObserver->iMainFunc = mainFunc;// test to run
		iObserver->iDoFunc = NULL;
		iObserver->CleanView();
		iObserver->Activate();
		CActiveScheduler::Start();
		}

	if( !iPLView )
		{
		ALLPRINT(KStars);
		_LIT(KRemoteViewCS,"Initialising RemoteView on Client/Server tests");
		ALLPRINT(KRemoteViewCS);
		ALLPRINT(KStars);
		iRemote = ETrue;
		ResetDatabaseL(10);
		iObserver->iMainFunc = mainFunc;// test to run
		iObserver->iDoFunc = NULL;
		iObserver->CleanView();
		iObserver->Activate();
		CActiveScheduler::Start();
		}

		CleanupL();
		__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
create local view with empty sortorder,
database is contains ten empty contacts
*/
void CContactViewTest::AllEmptyTestL()
	{
	ALLPRINT(KTest1);
	ResetSortL();
	iObserver->SetView( CreateViewL() );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doAllEmptyTestL;
	}

/**
check that local view contains 10 contacts, each containing 1 empty field
*/
void CContactViewTest::doAllEmptyTestL(const CContactViewBase &aLView)
	{
	ClosePersistanceL(ETrue);
	TInt length = aLView.CountL();
	TInt manyfields = 0;
	TInt i;
	INTCOMPARE( length , ==,  iContacts, 0, 0 );

	for(i = 0; i < length; ++i)
		{
		manyfields = aLView.ContactAtL(i).FieldCount();
		INTCOMPARE( manyfields , ==,  1,  i, 0 );
		SINGLECOMPARE(  CheckViewContact( aLView.ContactAtL(i), KNullDesC ),  i ,  0 );
		}
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = NULL;
	iObserver->CleanView();
	}

/**
create local view with empty sortorder,
database contains 10 contacts with all fields set to preset values
*/
void CContactViewTest::ViewEmptyTestL()
	{
	ALLPRINT(KTest2);
	ClosePersistanceL(EFalse);
	ResetDatabaseL();
	for(TInt i = 0; i < iContacts; ++i)
		{
		const TContactItemId cid = iIterate->NextL();
		OpenL(cid, *iViewAll);
		SetContactL();
		CommitL();
		}
	ResetSortL();
	OpenPersistanceL();
	iObserver->SetView( CreateViewL() );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doViewEmptyTestL;
	}

/**
check that local view contains 10 contacts, each containing 1 empty field
*/
void CContactViewTest::doViewEmptyTestL(const CContactViewBase &aLView)
	{
	TInt length = aLView.CountL();
	TInt manyfields = 0;
	TInt i;
	INTCOMPARE( length , ==,  iContacts, 0, 0 );
	iIterate->Reset();
	for(i = 0; i < length; ++i)
		{
		const CViewContact &vcontact = aLView.ContactAtL(i);
		manyfields = vcontact.FieldCount();
		INTCOMPARE( manyfields , ==,  1,  i, 0 );
		SINGLECOMPARE(  CheckViewContact( vcontact, GetFieldL(1) ),  i,  0 );
		}
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = NULL;
	iObserver->CleanView();
	}

/**
create local view with sortorder containing all (existing) uids,
database contains 10 empty contacts
*/
void CContactViewTest::ContactEmptyTestL()
	{
	ALLPRINT(KTest3);
	ClosePersistanceL(EFalse);
	ResetDatabaseL();
	ResetSortL();

	TInt length = iExistingUidsArray->Count();
	TInt i = 0;
	for( ; i < length ; ++i)
		{
		iSortOrder->AppendL( TUid::Uid( (*iExistingUidsArray)[i] ) );
		}
	OpenPersistanceL();
	iObserver->SetView( CreateViewL() );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doContactEmptyTestL;
	}

void CContactViewTest::doContactEmptyTestL(const CContactViewBase &aLView)
	{
	TInt length = aLView.CountL();
	TInt arrayLength = iExistingUidsArray->Count();
	TInt manyfields = 0;
	TInt i;
	INTCOMPARE( length , ==,  iContacts, 0, 0 );

	for(i = 0; i < length; ++i)
		{
		const CViewContact &vcontact = aLView.ContactAtL(i);
		manyfields = vcontact.FieldCount();
		INTCOMPARE( manyfields , ==,  arrayLength,  i, 0 );
		SINGLECOMPARE(  CheckViewContact( vcontact, KNullDesC ),  i ,  0 );
		}
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = NULL;
	iObserver->CleanView();
	}

void CContactViewTest::ResetDatabaseL(const TInt aMany)
	{
	iIterate->Reset();
	for(TInt i = 0; i < iContacts; ++i)
		{
		const TContactItemId cid = iIterate->NextL();
		iIterate->RemoveL(cid);
		}
	if( aMany > 0 )
		{
		iContacts = aMany;
		}
	CLEAR( iContactsDatabase );
	CLEAR( iContactsDatabase2 );
	iContactsDatabase = CContactDatabase::ReplaceL();

	iContactsDatabase2 = CContactDatabase::OpenL( CContactDatabase::EMultiThread );
	AddEmptyContactsL(iContacts);
	iIterate->Reset();
	}

TBool CContactViewTest::CheckViewContact(const CViewContact &aVContact, const TDesC &aVal)
	{
	TBool SingleCheckResult = EFalse;
	TBool OverallCheckResult = ETrue;
	TInt length = aVContact.FieldCount();
	TInt i = 0;
	for(; i < length; ++i)
		{
		SingleCheckResult = STRINGCOMPARE( aVContact.Field(i), ==, aVal, i, 0);
		OverallCheckResult = OverallCheckResult && SingleCheckResult;
		}
	return ( OverallCheckResult && length > 0 );
	}

/**
create local view with sortorder containing all (existing) uids,
database contains 10 contacts with all fields set to preset values
*/
void CContactViewTest::AllFullTestL()
	{
	ALLPRINT(KTest4);
	ClosePersistanceL(EFalse);
	ResetDatabaseL();
	TInt i;
	for(i = 0; i < iContacts; ++i)
		{
		const TContactItemId cid = iIterate->NextL();
		OpenL(cid, *iViewAll);
		SetContactL();
		CommitL();
		}


	TInt length = iExistingUidsArray->Count();
	ResetSortL();
	for(i = 0 ; i < length ; ++i)
		{
		iSortOrder->AppendL( TUid::Uid( (*iExistingUidsArray)[i] ) );
		}

	OpenPersistanceL();
	iObserver->SetView( CreateViewL() );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doAllFullTestL;
	}

/**
check that local view contains 10 contacts, each containing as many fields
as there are uids (114), all field that have a corresponding uid should be
non-empty
*/
void CContactViewTest::doAllFullTestL(const CContactViewBase &aLView)
	{
	ClosePersistanceL(ETrue);
	TInt length = aLView.CountL();
	TInt arrayLength = iExistingUidsArray->Count();
	TInt manyfields = 0;
	TInt i;
	INTCOMPARE( length , ==,  iContacts, 0, 0 );

	for(i = 0; i < length; ++i)
		{
		const CViewContact &vcontact = aLView.ContactAtL(i);
		manyfields = vcontact.FieldCount();
		INTCOMPARE( manyfields , ==,  arrayLength,  i, 0 );
		SINGLECOMPARE(  CheckViewContactL( vcontact ),  i ,  0 );
		}
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = NULL;
	iObserver->CleanView();
	}


void CContactViewTest::SetContactL()
	{
	TInt length = iFields->Count();
	TInt i;
	for( i = 0; i < length; ++i)
		{
		SetFieldL(*iFields, i, (*iFields)[i].Label());
		}
	}

/**
Check that contents of all fields in view contact equal corresponding fields in contact item
*/
TBool CContactViewTest::CheckViewContactL(const CViewContact &aVContact)
	{
	TBool OverallCheckResult = EFalse;

	//retrieve contact item id of view contact
	TContactItemId cid = aVContact.Id();

	//open contact item
	ReadL(cid, *iViewAll);
	TInt length = aVContact.FieldCount();
	TInt i = 0;
	for(; i < length; ++i)
		{
		OverallCheckResult = EFalse;
		//find first field in contact item that equals uid (of view contact field) stored in sort order
		TInt pos = iFields->Find((*iSortOrder)[i]);
		TDesC *ContactField = NULL;

		if(pos < 0)
			{
			ContactField = const_cast<TDesC *>(&KNullDesC);
			}
		else
			{
			//converts contents of field - matching the view contact uid - to a string
			ContactField = FieldStringLC( (*iFields)[pos] );
			}
		//check that conents of contact item field equal conents of corresponing view contact field
		OverallCheckResult = STRINGCOMPARE( aVContact.Field(i), ==, *ContactField, i, 0 );

		if(pos > -1)
			{
			CleanupStack::PopAndDestroy( ContactField );
			}

		//if view contact field doesnt equal contact item field, fail test
		if( !OverallCheckResult )
			{
			break;//test failed
			}
		}
	Close();

	return OverallCheckResult;
	}
/**
cleanup sort order, create new empty sort order
*/
void CContactViewTest::ResetSortL()
	{
	iSortOrder->Close();
	CLEAR(iSortOrder);
	iSortOrder = new(ELeave) RContactViewSortOrder();
	}

/**
converts contents of a contact item field to a string
*/
HBufC *CContactViewTest::FieldStringLC(const CContactItemField &aField)
	{
	HBufC *buf = NULL;
	if( aField.IsTemplateLabelField() )
		{
		buf = HBufC::NewLC(0);
		return buf;
		}
	switch(aField.StorageType())
		{
		case KStorageTypeText:
			{
			buf = HBufC::NewLC(aField.Label().Length()+2);
			TPtr ptr = buf->Des();
			ptr.Append( aField.Label() );
			}
		break;
		case KStorageTypeContactItemId:
			{
			buf = HBufC::NewLC(20);//assume cid < 20 digits
			TContactItemId cid = aField.AgentStorage()->Value();
			TPtr ptr = buf->Des();
			ptr.AppendNum(cid);
			}
		break;
		case KStorageTypeStore:
		case KStorageTypeDateTime:
			{
			buf = HBufC::NewLC(0);//views dont care about date/time or binary fields
			}
		break;
		default:
			{
			_LIT(KCCVTest,"CCVTest");
			User::Panic(KCCVTest , EInvariantFalse );
			}
		break;
		}
	return buf;

	}

/**
create local view with sortorder containing all (existing) uids,
database contains 10 contacts with a fieldset containing no fields.
*/
void CContactViewTest::EmptyFieldsetTestL()
	{
	ALLPRINT(KTest5);
	ClosePersistanceL(EFalse);
	ResetDatabaseL();
	TInt i;
	for(i = 0; i < iContacts; ++i)
		{
		const TContactItemId cid = iIterate->NextL();
		OpenL(cid, *iViewAll);
		iFields->Reset();
		CommitL();
		}


	TInt length = iExistingUidsArray->Count();
	ResetSortL();
	for(i = 0 ; i < length ; ++i)
		{
		iSortOrder->AppendL( TUid::Uid( (*iExistingUidsArray)[i] ) );
		}

	OpenPersistanceL();
	iObserver->SetView( CreateViewL() );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doEmptyFieldsetTestL;
	}

/**
check that local view contains 10 contacts, each containing as many fields
as there are uids (114), all field should be empty. create new view with
empty sortorder
*/
void CContactViewTest::doEmptyFieldsetTestL(const CContactViewBase &aLView)
	{
	TInt length = aLView.CountL();
	TInt arrayLength = iExistingUidsArray->Count();
	TInt manyfields = 0;
	TInt i;
	INTCOMPARE( length , ==,  iContacts, 0, 0 );

	for(i = 0; i < length; ++i)
		{
		const CViewContact &vcontact = aLView.ContactAtL(i);
		manyfields = vcontact.FieldCount();
		INTCOMPARE( manyfields , ==,  arrayLength,  i, 0 );
		SINGLECOMPARE(  CheckViewContact( aLView.ContactAtL(i), KNullDesC ),  i,  0 );
		}
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = NULL;
	iObserver->CleanView();
	}

/**
Delete all fields from all contacts
*/
void CContactViewTest::EmptyFieldsetSortTestL()
	{
	ALLPRINT(KTest6);
	ClosePersistanceL(EFalse);
	ResetDatabaseL();
	TInt i;
	for(i = 0; i < iContacts; ++i)
		{
		const TContactItemId cid = iIterate->NextL();
		OpenL(cid, *iViewAll);
		//deletes all fields
		iFields->Reset();
		CommitL();
		}

	ResetSortL();
	OpenPersistanceL();
	iObserver->SetView( CreateViewL() );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doEmptyFieldsetSortTestL;
	}

/**
Check that view contains 1 field for each contact and that field is null
*/
void CContactViewTest::doEmptyFieldsetSortTestL(const CContactViewBase &aLView)
	{
	TInt length = aLView.CountL();
	TInt manyfields = 0;
	TInt i;
	INTCOMPARE( length , ==,  iContacts, 0, 0 );

	for(i = 0; i < length; ++i)
		{
		const CViewContact &vcontact = aLView.ContactAtL(i);
		manyfields = vcontact.FieldCount();
		INTCOMPARE( manyfields , ==,  1,  i, 0 );
		SINGLECOMPARE(  CheckViewContact( aLView.ContactAtL(i), KNullDesC ),  i ,  0 );
		}
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = NULL;
	iObserver->CleanView();
	}

/**
Set all fields of all contacts to contain their own label string
use empty sort order
*/
void CContactViewTest::MissingFieldTestL()
	{
	ALLPRINT(KTest7);
	ClosePersistanceL(EFalse);
	ResetDatabaseL();
	for(TInt i = 0; i < iContacts; ++i)
		{
		const TContactItemId cid = iIterate->NextL();
		OpenL(cid, *iViewAll);
		SetContactL();
		CommitL();
		}
	ResetSortL();
	OpenPersistanceL();
	iObserver->SetView( CreateViewL() );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doMissingFieldTestL;
	}

/**
check that a field is generated for each view contact and that field matches list of missing of fields
remove generated field from contact item and repopulate contact view repeat until view contact field is null
*/
void CContactViewTest::doMissingFieldTestL(const CContactViewBase &aLView)
	{
	ClosePersistanceL(ETrue);
	TBool end = ETrue;
	TInt length = aLView.CountL();
	TInt manyfields = 0;
	TInt i;
	//iterate through ini file, that stores list of expected field in order
	const TPtrC mfield = GetNextFieldL();
	INTCOMPARE( length , ==,  iContacts, 0, 0 );
	iIterate->Reset();

	for(i = 0; i < length; ++i)
		{
		const TInt loc = aLView.FindL(iIterate->NextL());
		const CViewContact &vcontact = aLView.ContactAtL( loc );
		manyfields = vcontact.FieldCount();
		const TPtrC vfield = vcontact.Field(0);
		INTCOMPARE(  manyfields , ==,  1,  i, 0 );
		
		//compare view contact field to field stored in ini file
		STRINGCOMPARE( vfield, ==, mfield, i, 0 );
		//if view contact field is null, end test
		end = end && (vfield == KNullDesC);
		//find view contact field in contact item and set to null, if field not found, end test
		end = end || !FindAndClearFieldL( vcontact.Id(), vfield );
		}


	if( end )
		{
		iObserver->iMainFunc = NULL;
		iObserver->iDoFunc = NULL;
		iObserver->CleanView();
		}
	else
		{
		//if test not complete, reset view and repopulate
		OpenPersistanceL();
		iObserver->CleanView();
		iObserver->SetView( CreateViewL() );
		}
	}

/**
populate database with full, medium or empty contacts
*/
void CContactViewTest::PerformanceTestL()
	{
	ClosePersistanceL( EFalse );
	ResetDatabaseL(iPerformanceContacts);

	//populate contacts
	if( EMedium == iFullness )
		{
		if( EFew == iPerformanceContacts )
			{
			ALLPRINT(KTest81);
			}
		TInt i = 0;
		for(; i < iContacts; ++i)
			{
			const TContactItemId cid = iIterate->NextL();
			OpenL(cid, *iViewAll);
			SetMediumL();
			CommitL();
			}
		}
	else if( EFull == iFullness )
		{
		if( EFew == iPerformanceContacts )
			{
			ALLPRINT(KTest82);
			}
		TInt i = 0;
		for(; i < iContacts; ++i)
			{
			const TContactItemId cid = iIterate->NextL();
			OpenL(cid, *iViewAll);
			SetFullL();
			CommitL();
			}
		}
	else
		{
		if( EFew == iPerformanceContacts )
			{
			ALLPRINT(KTest8);
			}
		}

	/**
	Print the number of contacts used for this test
	*/
	switch( iPerformanceContacts )
		{
		case EFew:
			{
			_LIT(KFewContacts,"Few contacts");
			ALLPRINT( KFewContacts );
			iPerformanceContacts = EMid;
			break;
			}
		case EMid:
			{
			_LIT(KMidContacts,"Mid contacts");
			ALLPRINT( KMidContacts );
			iPerformanceContacts = EMany;
			break;
			}
		case EMany:
			{
			_LIT(KManyContacts,"Many contacts");
			ALLPRINT( KManyContacts );
			iPerformanceContacts = EMax;
			break;
			}
		case EMax:
			{
			_LIT(KMaxContacts,"Max contacts");
			ALLPRINT( KMaxContacts );
			iPerformanceContacts = EDone;
			break;
			}
		default:
			{
			_LIT(KInvalidContacts,"bad performance contacts");
			User::Panic(KInvalidContacts, 333 );
			break;
			}
		}

	_LIT(KStartCompact,"Start Compact");
	ALLPRINT( KStartCompact );
	iContactsDatabase->CompactL();//to make sure that the server is not busy doing automatic compaction
	_LIT(KEndCompact,"End Compact");
	ALLPRINT( KEndCompact );


	//Create sortorder with typical uids
	ResetSortL();
	iSortOrder->AppendL(KUidContactFieldGivenName);
	iSortOrder->AppendL(KUidContactFieldAdditionalName);
	iSortOrder->AppendL(KUidContactFieldFamilyName);
	iSortOrder->AppendL(KUidContactFieldPhoneNumber);

	OpenPersistanceL();
	_LIT(KStartView,"Start view creation");
	ALLPRINT( KStartView );
	//note time view is created
	iStartC.UniversalTime();
	iObserver->SetView( CreateViewL() );
	//note time vie creat is completed
	iStartP.UniversalTime();
	_LIT(KEndView,"End view creation, wait for view to be populated");
	ALLPRINT( KEndView );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doPerformanceTestL;
	}

/**
Calculate/Print time taken to create view
Calculate/Print time take to populate view
*/
void CContactViewTest::doPerformanceTestL(const CContactViewBase &/*aLView*/)
	{
	static TInt iters = KManyPerfIters;
	static TInt64 creation = 0;
	static TInt64 population = 0;

	_LIT(KIteration,"Iteration: %d");
	ALLPRINT2(KIteration, iters);

	//note time when view has been populated
	iFinishP.UniversalTime();
	//time taken to pupulate view
	population += ( iFinishP.MicroSecondsFrom( iStartP ).Int64() );
	//time taken to create view
	creation += ( iStartP.MicroSecondsFrom( iStartC ).Int64() );

	//if average of many iterations of view population is being measured, wait for last iteration before
	//calculating average tiem taken to creat/populate a view
	if(--iters <= 0)
		{
		iters = KManyPerfIters;

		TReal creatSec = 0;
		TReal popSec = 0;

		creatSec =static_cast<TReal>(creation)/(1000000*iters);
		popSec = static_cast<TReal>(population)/(1000000*iters);

		_LIT(KCreationTime,"Creation Time: %f seconds");
		ALLPRINT2(KCreationTime, creatSec);

		_LIT(KPopulationTime,"Population time: %f seconds");
		ALLPRINT2(KPopulationTime, popSec);
		creation = 0;
		population = 0;

		if( EFull == iFullness )
			{
			iters = KManyPerfIters;
			}

		iObserver->iDoFunc = NULL;
		iObserver->CleanView();

		//time taken to create and populate view should not exceed defined limit
		INTCOMPARE( (creatSec + popSec), <, KMaxTimeAllowed, 0, 0 );

		//if empty view, change to medium view performance test
		if( EBlank == iFullness )
			{
			if( EDone == iPerformanceContacts )
				{
				iPerformanceContacts = EFew;
				iFullness = EMedium;
				}
			iObserver->iMainFunc = &CContactViewTest::PerformanceTestL;
			iObserver->Activate();
			}

		//if medium view, change to full view performance test
		else if( EMedium == iFullness )
			{
			if( EDone == iPerformanceContacts )
				{
				iPerformanceContacts = EFew;
				iFullness = EFull;
				}
			iObserver->iMainFunc = &CContactViewTest::PerformanceTestL;
			iObserver->Activate();

			}
		//if full view
		else
			{
			//if all contact permuations done, end test
			if( EDone == iPerformanceContacts )
				{
				iPerformanceContacts = EFew;
				iFullness = EBlank;
				iObserver->iMainFunc = NULL;
				iObserver->CleanView();
				}
			else
				{
				//if other contact permuations availabe for full view, continue testing
				iObserver->iMainFunc = &CContactViewTest::PerformanceTestL;
				iObserver->Activate();
				}
			}

		}
	//if not final iteration
	else
		{
		iObserver->iMainFunc = NULL;
		iObserver->iDoFunc = &CContactViewTest::doPerformanceTestL;
		//reset view
		iObserver->CleanView();
		iStartC.UniversalTime();
		//recreate view
		iObserver->SetView( CreateViewL() );
		iStartP.UniversalTime();
		}
	}

/**
populate contact on first iteration
reset view and start compress/recover
*/
void CContactViewTest::RecoverCompressTestL()
	{
	ALLPRINT(KTest9);
	TInt i;
	if( !iIteration )
		{
		ResetDatabaseL();

		for(i = 0; i < iContacts; ++i)
			{
			const TContactItemId cid = iIterate->NextL();
			OpenL(cid, *iViewAll);
			SetContactL();
			CommitL();
			}
		}

	//sortorder contains all uids
	TInt length = iExistingUidsArray->Count();
	ResetSortL();
	for(i = 0 ; i < length ; ++i)
		{
		iSortOrder->AppendL( TUid::Uid( (*iExistingUidsArray)[i] ) );
		}

	iObserver->SetView( CreateViewL() );
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = &CContactViewTest::doRecoverCompressTestL;

	//if first iteration start compress/recover
	if( !iIteration++)
		{
		if(iRecoverFlag)
			{
			StartRecoverL();
			}
		else
			{
			StartCompressL();
			}
		}
	}

/**
if expected error generated, end compress or recover and regenerate view
otherwise check view is correctly populated
*/
void CContactViewTest::doRecoverCompressTestL(const CContactViewBase &aLView)
	{
	//check expected generated error 
	if( ( ( KErrAccessDenied == iObserver->iEvent.iInt && !iRecoverFlag ) ||
		  ( KErrDisconnected == iObserver->iEvent.iInt && iRecoverFlag ) || iRemote)
		    && iIteration == 1)
		{
		//end recover / compress
		if(iRecoverFlag)
			{
			EndRecoverL();
			}
		else
			{
			EndCompressL();
			}
		//reset view
		iObserver->iMainFunc = &CContactViewTest::RecoverCompressTestL;
		iObserver->iDoFunc = NULL;
		iObserver->CleanView();
		iObserver->Activate();
		iObserver->iEvent = TContactViewEvent();
		return;
		}

	//reset iteration
	iIteration = 0;
	TInt length = aLView.CountL();
	TInt arrayLength = iExistingUidsArray->Count();
	TInt manyfields = 0;
	TInt i;
	INTCOMPARE( length , ==,  iContacts, 0, 0 );

	//check view correctly populated
	for(i = 0; i < length; ++i)
		{
		const CViewContact &vcontact = aLView.ContactAtL(i);
		manyfields = vcontact.FieldCount();
		INTCOMPARE( manyfields , ==,  arrayLength,  i, 0 );
		SINGLECOMPARE(  CheckViewContactL( vcontact ),  i,  0 );
		}
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = NULL;
	iObserver->CleanView();
	iObserver->iEvent = TContactViewEvent();
	}

/**
start compress / recover before view is created in seperate thread
create view
end compress / recover in seperate thread
*/
void CContactViewTest::ConcurrentRecoverCompressTestL()
	{
	ALLPRINT(KTest10);
	TInt i;
	if( !iIteration )
		{
		ResetDatabaseL();

		for(i = 0; i < iContacts; ++i)
			{
			const TContactItemId cid = iIterate->NextL();
			OpenL(cid, *iViewAll);
			SetContactL();
			CommitL();
			}
		}

	TInt length = iExistingUidsArray->Count();
	ResetSortL();
	for(i = 0 ; i < length ; ++i)
		{
		iSortOrder->AppendL( TUid::Uid( (*iExistingUidsArray)[i] ) );
		}

	if (iThread.Handle() != RThread().Handle() )
		{
		iThread.Close();
		}

	if( !iIteration )
		{
		iThread.Create( KTest10, CContactViewTest::ThreadRunL, KDefaultStackSize, NULL, this, EOwnerThread );
		iThread.Resume();
		iThread.SetPriority(EPriorityNormal);
		iSemaphore.Wait();//wait for recovery compress to be started
		}

	TRAPD(err, iObserver->SetView(CreateViewL()));
	if( !iIteration )
		{
		iSemaphore2.Signal();// signal that view has been created
		}
	if( !iIteration )
		{
		iSemaphore.Wait();//wait for recovery/compress to be completed
		}

	if (err == KErrNotReady)
 		{
		_LIT(KCreateViewResult,"Create view leave with KErrNotReady(-18) in iIteration=%d");
		ALLPRINT2(KCreateViewResult, iIteration);
		TEST(iIteration == 0); // expect err=KErrNotReady for iIteration=0 in remote view test

		iObserver->iMainFunc = &CContactViewTest::ConcurrentRecoverCompressTestL;
		iObserver->iDoFunc = NULL;
		iObserver->CleanView();
		iObserver->Activate();
		iObserver->iEvent = TContactViewEvent();
		}
	else
		{
		iObserver->iMainFunc = NULL;
		iObserver->iDoFunc = &CContactViewTest::doConcurrentRecoverCompressTestL;
		}

	++iIteration;	
	}

/**
on first iteration check that expected error is generated
on next iteration check that view is correctly populated
*/
void CContactViewTest::doConcurrentRecoverCompressTestL(const CContactViewBase &aLView)
	{
	//check expected error generated
	if( ( ( KErrAccessDenied == iObserver->iEvent.iInt && !iRecoverFlag ) ||
		  ( KErrDisconnected == iObserver->iEvent.iInt && iRecoverFlag ) || iRemote)
		    && iIteration == 1)
		{
		//end recover / compress
		if(iRecoverFlag)
			{
			EndRecoverL();
			}
		else
			{
			EndCompressL();
			}
		//reset view
		iObserver->iMainFunc = &CContactViewTest::RecoverCompressTestL;
		iObserver->iDoFunc = NULL;
		iObserver->CleanView();
		iObserver->Activate();
		iObserver->iEvent = TContactViewEvent();
		return;
		}
		
	iIteration = 0;
	TInt length = aLView.CountL();
	TInt arrayLength = iExistingUidsArray->Count();
	TInt manyfields = 0;
	TInt i;
	INTCOMPARE( length , ==,  iContacts, 0, 0 );

	for(i = 0; i < length; ++i)
		{
		const CViewContact &vcontact = aLView.ContactAtL(i);
		manyfields = vcontact.FieldCount();
		INTCOMPARE( manyfields , ==,  arrayLength,  i, 0 );
		SINGLECOMPARE(  CheckViewContactL( vcontact ),  i,  0 );
		}
	iObserver->iMainFunc = NULL;
	iObserver->iDoFunc = NULL;
	iObserver->CleanView();
	iObserver->iEvent = TContactViewEvent();
	}

/**
New thread function, start recovery/compress before view creation and
completes recovery/compress after view creation
*/
void CContactViewTest::RecoverAndCompressThreadTestL()
	{
	CContactDatabase *cntdb = CContactDatabase::OpenL();
	CleanupStack::PushL( cntdb );

	if( iRecoverFlag )
		{
		// The views access the database directly and are oblivious of the state of the contacts server.
		// Therefore they will try and access a corrupt (damaged) database.  This leads to a DBMS panic
		// It is beleived that 9.1-9.4 the Contacts model has always exhibited this behavior (fixed in 9.5)
		#ifdef _DEBUG
			if( !cntdb->IsDamaged() && iDamageCounter == 0 )
				{
				cntdb->DamageDatabaseL(0x666);//will give us consistent behaviour on debug builds
				}
		#endif
		++iDamageCounter;
		CContactActiveRecover *recover = cntdb->CreateRecoverLC();
		recover->SetObserver( this );
		iSemaphore.Signal();//signal that recovery has started
		iSemaphore2.Wait();//wait for view to be created

		ALLPRINT(KStartRecover);
		while(recover->Step())
			{
			User::After( 1000 );
			}
		CleanupStack::PopAndDestroy(recover);
		ALLPRINT(KEndRecover);
		}
	else
		{
		CContactActiveCompress 	*compress = cntdb->CreateCompressorLC();
		compress->SetObserver( this );
		iSemaphore.Signal();//signal that compress has started
		iSemaphore2.Wait();//wait for view to be created
		ALLPRINT(KStartCompress);
		while(compress->Step())
			{
			User::After( 1000 );
			}
		CleanupStack::PopAndDestroy(compress);
		ALLPRINT(KEndCompress);
		}
	iSemaphore.Signal();//signal that recovery compress is complete
	CleanupStack::PopAndDestroy( cntdb );
	}

/**
Runs thread function, called from thread constructor
*/
TInt CContactViewTest::ThreadRunL(TAny *aAny)
	{
	TInt ThreadError = KErrNone;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CContactViewTest &test = *static_cast<CContactViewTest *>(aAny);
	TRAP(ThreadError, test.RecoverAndCompressThreadTestL()	);
	delete cleanup;
	delete sched;
	return ThreadError;
	}

/**
Search for a field in contact item acid, that has the same label as aField
Sets the value of found field to null/empty
returns true if field found, false otherwise
*/
TBool CContactViewTest::FindAndClearFieldL(const TContactItemId aCid, const TDesC &aField)
	{
	TBool ret = EFalse;
	OpenL(aCid,*iViewAll);
	TInt length = iFields->Count();
	TInt i;
	for(i = 0; i < length; ++i)
		{
		if( (*iFields)[i].Label() == aField && (*iFields)[i].Storage()->IsFull() )
			{
			SetFieldL(*iFields, i, KNullDesC);
			ret = ETrue;
			break;
			}
		}
	CommitL();
	return ret;
	}

/**
Iterates through the missing fields list in the ini file,
retrieves the next field in the list
*/
TPtrC CContactViewTest::GetNextFieldL()
	{
	static TInt iter = 0;
	++iter;
	TPtrC ret = GetFieldL(iter);
	if( ret == KNullDesC )
		{
		iter = 0;
		}
	return ret;
	}

/**
Retrieves the field at aIndex of under missing fields section in ini file
*/
TPtrC CContactViewTest::GetFieldL(const TInt aIndex)
	{
	TPtrC ret;
	TPtrC tfield = KUfield();
	HBufC *fval=HBufC::NewLC(tfield.Length()+2);//assume #fields < 100

	TPtr fptr=fval->Des();
	fptr.Append( tfield );
	fptr.AppendNum( aIndex );
	GetStringFromConfig( KMissingFields, fptr, ret );
	CleanupStack::PopAndDestroy( fval );
	return ret;
	}

/**
Creates a persistance layer local view, Client/server local view or remote view
depending on the type of testing being performed.
*/
CContactViewBase* CContactViewTest::CreateViewL()
	{
	if( iPLView )
		{
		return CContactLocalView::NewL(	*iObserver, *iContactsDatabase,
										*iSortOrder, EContactsOnly, &iPersistence->FactoryL(), KNullDesC8 );
		}
	else
		{
		if(iRemote)
			{
			return CContactRemoteView::NewL(	*iObserver, *iContactsDatabase,
											*iSortOrder, EContactsOnly);
			}
		else
			{
			return CContactLocalView::NewL(	*iObserver, *iContactsDatabase,
										*iSortOrder, EContactsOnly);
			}
		}
	}

/**
If performing persistance layer testing, cleanup the persistance layer instance
if aReOpen, also open sessions to contact server
*/
void CContactViewTest::ClosePersistanceL(const TBool aReOpen)
	{
	if( iPLView )
		{
		if( iPersistence != NULL )
			{
			CLEAR( iPersistence );
			//wait for persistance layer to close, before opening server
			User::After( 200000 );
			if( aReOpen )
				{
				iContactsDatabase = CContactDatabase::OpenL();

				iContactsDatabase2 = CContactDatabase::OpenL( CContactDatabase::EMultiThread );
				}
			}
		}
	}

/**
If performing persistance layer testing, copy database from cntsrv private directory to test server
private directory using the pim test server. Then open copied db file using persistance layer.
*/
void CContactViewTest::OpenPersistanceL()
	{
	if( iPLView )
		{
		TBuf<190> buf;
		iContactsDatabase->GetDefaultNameL(buf);
		CLEAR(iContactsDatabase);
		CLEAR(iContactsDatabase2);
		WaitForServerToExitL();
		iPimServ->CopyFileL(KContactsDB, KPersistenceDB);
		iPersistence = CPersistenceLayer::NewLC(iParent->Fs());
		CleanupStack::Pop(iPersistence);

		#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
		TRAPD( err, iPersistence->ContactsFileL().OpenL( buf ));
		#else
		TRAPD( err,
					while( iPersistence->ContactsFileL().OpenStepL( buf ) ) {}
		);
		#endif

		User::LeaveIfError(err);
		}
	}

/**
Set all fields in currently opened contact to a medium length string
*/
void CContactViewTest::SetMediumL()
	{

	for(TInt i = 0; i< 10; ++i)
		{
		SetFieldL(*iFields, i, TestConstants::KMediumString);
		}
	}

/**
Set all fields in currently opened contact to a long length string
*/
void CContactViewTest::SetFullL()
	{
	const TInt count = iFields->Count();

	for(TInt i = 0; i< count; ++i)
		{
		SetFieldL(*iFields, i, TestConstants::KLongString);
		}
	}
/**
damage database and start active recovery
*/
void CContactViewTest::StartRecoverL()
	{
	ALLPRINT(KStartRecover);
	#ifdef _DEBUG
	    #ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
		iContactsDatabase2->DamageDatabaseL(0x666);//will give us consistent behaviour on debug builds
		#endif
	#endif

    User::After( 200000 );

	iRecover = iContactsDatabase2->CreateRecoverLC();
	CleanupStack::Pop( iRecover );

	iRecover->SetObserver( this );

	while (iRecover->Step()) {}

	}

/**
cancel active recovery
*/
void CContactViewTest::EndRecoverL()
	{

	if( iRecover )
		{
		ALLPRINT(KEndRecover);

		iRecover->Cancel();//deletes irecover
		iRecover = NULL;
		}
	}

/**
start active compress
*/
void CContactViewTest::StartCompressL()
	{
	ALLPRINT(KStartCompress);
	iCompress = iContactsDatabase2->CreateCompressorLC();
	iCompress->SetObserver( this );
	CleanupStack::Pop( iCompress );

	}

/**
cancels active compress
*/
void CContactViewTest::EndCompressL()
	{

	if( iCompress )
		{
        ALLPRINT(KEndCompress);

		iCompress->Cancel();//deletes iCompress
		iCompress = NULL;
		}
	}

/**
Prints any errors encountered while performing active compress/recover
*/
void CContactViewTest::HandleError(TInt aError)
	{
	iError = aError;
	_LIT(KCompressRecoverError,"Error in active compress/recover: %d");
	ALLPRINT2(KCompressRecoverError, iError);
	}

