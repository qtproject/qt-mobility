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
#include <e32math.h>
#include "ccntdbmanager.h"
#include "lowdiskspacestep.h"
#include "performancefunctionalitydefs.h"

_LIT( KTest1, "CRUD tests" );
_LIT( KTest2, "Database tests" );
_LIT( KTest3, "Other Miscellaneous apis tests" );
_LIT( KTest4, "Transaction commit failure test");
_LIT( KTest5, "Start server while in low disk mode test");
_LIT( KTest6, "Transaction rollback failure test");

_LIT( KFailCommitTransactionTest, "FailCommitTransaction" );
_LIT( KFailRollbackTransactionTest, "FailRollbackTransaction" );
_LIT( KStartServerTest, "StartServer" );

_LIT( KFileName, "c:\\anyfile.any" );
_LIT( KCntDB, "c:anyfile.cdb" );
_LIT( KCntDB2, "c:anyfile2.cdb" );
_LIT( KCntDB3, "c:anyfile3.cdb" );
_LIT( KExport1, "c:\\export1.vcard" );

_LIT(KCleanup, "cleanup");
_LIT(KDiskSize, "size: %LD, free space: %LD");
_LIT(KStars,"*******************************");


// how much empty space will be made available on disk, in bytes
const static TInt KMinusFull = KLowDiskThreshold - 8000; 
#define KNumberOfContacts 200

CLowDiskSpaceStep::CLowDiskSpaceStep(CPerformanceFunctionalityTestsSuite &aParent) : CPerformanceFunctionalityBase( KNumberOfContacts, aParent ),
																						iExpectedError( KErrDiskFull )
	{
	SetTestStepName(KLowDiskSpaceStep);
	}

	
void CLowDiskSpaceStep::CLowDiskSpaceActive::RunL()
	{
	const static TInt numberOfTests = iStep->iTests->Count();
	if( iStep->iNextTest >= numberOfTests )//have we run all the tests
		{
		CActiveScheduler::Stop();
		iStep->iNextTest = 0;
		}
	else
		{
		if( *iStep->iConfigSection == KFailCommitTransactionTest )
			{
			iStep->INFO_PRINTF1(KTest4);
			iStep->FailTransactionTestL( ECommitTransaction );
			iStep->iNextTest = numberOfTests;//last test
			}
		else if( *iStep->iConfigSection == KFailRollbackTransactionTest )
			{
			iStep->INFO_PRINTF1(KTest6);
			iStep->FailTransactionTestL( ERollbackTransaction );
			iStep->iNextTest = numberOfTests;//last test
			}
		else if( *iStep->iConfigSection == KStartServerTest )
			{
			iStep->INFO_PRINTF1(KTest5);
			iStep->StartServerTestL();
			iStep->iNextTest = numberOfTests;//last test
			}
		else
			{
			RArray< TBool (CLowDiskSpaceStep::*)() > &testArray = *( iStep->iTests );
			
			TBool (CLowDiskSpaceStep::*testFunction)() =  testArray[iStep->iNextTest++];
			
			TBool testResult = ( iStep->*testFunction )();
			
			iStep->TEST1( testResult, ETrue);//run the next test
			}
		Activate();
		}
	}
	
void CLowDiskSpaceStep::CLowDiskSpaceActive::DoCancel()
	{
	}

TInt CLowDiskSpaceStep::CLowDiskSpaceActive::RunError(TInt aError)
	{
	_LIT(KActiveError, "CLowDiskSpaceStep:: Error in doTest runL: %d");
	iStep->ERR_PRINTF2(KActiveError , aError );
	return aError;
	}

void CLowDiskSpaceStep::CLowDiskSpaceActive::Activate()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

TVerdict CLowDiskSpaceStep::doTestStepPostambleL()
	{
	CLEAR( iMyActive );
	if( iFile )
		{
		iFile->Close();
		CLEAR(iFile);
		}
	if( iTests )
		{
		iTests->Close();
		CLEAR(iTests);
		}
	return CPerformanceFunctionalityBase::doTestStepPostambleL();
	}
	
void CLowDiskSpaceStep::InitializeL()
	{
	CPerformanceFunctionalityBase::InitializeL();
	iGroup = iContactsDatabase->CreateContactGroupL(); 
	iCustomTemplate = iContactsDatabase->CreateContactCardTemplateL(KCntDB);
	iGroupedContact = iIterate->NextL();
	iContactsDatabase->AddContactToGroupL( iGroupedContact, iGroup->Id() );
	}
	
void CLowDiskSpaceStep::Cleanup()
	{
	CLEAR( iGroup );
	CLEAR( iCustomTemplate );
	CPerformanceFunctionalityBase::Cleanup();
	}

void CLowDiskSpaceStep::PreTestL()
	{
	iConfigSection = &ConfigSection();
	iMyActive = new (ELeave) CLowDiskSpaceStep::CLowDiskSpaceActive( this );
	iCleanup = (KCleanup() == *iConfigSection);
	
	User::LeaveIfError( iParent->Fs().Connect() );
	iFile = new(ELeave) RFile();
	
	//create the necessary database files if we are not in cleanup mode
	if(!iCleanup)
		{
		iTests = new(ELeave) RArray< TBool (CLowDiskSpaceStep::*)() >();
		
		CContactDatabase *cdb = NULL;
		cdb = CContactDatabase::ReplaceL( KCntDB2 );
		CLEAR(cdb);
		cdb = CContactDatabase::ReplaceL( KCntDB3 );
		CLEAR(cdb);
		InitializeL();
		
		iIterate->Reset();
		TInt j = 0;
		for(; j < iContacts; ++j)
			{
			OpenL(iIterate->NextL());
			SetAllFieldsL( GetFieldL(KStringFields, KField, 2) );
			CommitL(EFalse);
			}
		iIterate->Reset();
		ExportVcardL(EFalse, KExport1);
		Cleanup();
		//populates array of tests, that will be executed sequentially by the CLowDiskSpaceActive::runl()
		iTests->AppendL( &CLowDiskSpaceStep::StartLowDiskModeL );
		iTests->AppendL( &CLowDiskSpaceStep::CrudTestL );
		iTests->AppendL( &CLowDiskSpaceStep::MiscDbTestsL );
		iTests->AppendL( &CLowDiskSpaceStep::DatabaseTestsL );
		iTests->AppendL( &CLowDiskSpaceStep::StartHighdiskModeL );
		iTests->AppendL( &CLowDiskSpaceStep::CrudTestL );
		iTests->AppendL( &CLowDiskSpaceStep::MiscDbTestsL );
		iTests->AppendL( &CLowDiskSpaceStep::DatabaseTestsL );
		}
	}
	
TVerdict CLowDiskSpaceStep::doTestStepL()
	{
	if(iCleanup)
		{
		ClearDiskL();
		}
	else
		{
		__UHEAP_MARK;
		InitializeL();
		_LIT(KLowdiskStep,"CLowDiskSpaceStep::doTestStepL()");
		INFO_PRINTF1(KLowdiskStep);  //Block start 
		CActiveScheduler::Add(iMyActive);
		iMyActive->Activate();
		CActiveScheduler::Start();
		Cleanup();
		__UHEAP_MARKEND;
		}
	return TestStepResult();
	}
	
/**
populates the disk so that only a very small amount of disk space is available,
invoking low disk notification within cntsrv
*/
void CLowDiskSpaceStep::FillDiskL()
	{

	TVolumeInfo tv;
	User::LeaveIfError( iParent->Fs().Volume(tv) );
	
	INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);
	TInt frees = 0;
	iManyFiles = static_cast<TInt>(tv.iFree / KMaxTInt);
	
	_LIT(KTestNumberOfFiles, "Test will create %d full sized empty files");
	INFO_PRINTF2(KTestNumberOfFiles, iManyFiles );
	
	if ( ( tv.iFree - KMinusFull ) > KMaxTInt )
		{
		TPtrC tname( KFileName );
		TInt i = 0;
		for( ; i < iManyFiles; ++i )//create imanyfile maxsized empty files, until no more can be created
			{
			HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
			TPtr fptr = fval->Des() ;
			fptr.Append( tname );
			fptr.AppendNum( i );
			
			User::LeaveIfError( iFile->Replace( iParent->Fs(), fptr, EFileWrite ) );
			User::LeaveIfError( iFile->SetSize( KMaxTInt - KMinusFull) );
			iFile->Close();
			
			CleanupStack::PopAndDestroy( fval );
			}
		User::LeaveIfError( iParent->Fs().Volume(tv) );
		
		INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);
		//check to see if there is still significant free space and create empty file to leave no empty space
		frees = static_cast<TInt>( tv.iFree - KMinusFull );
		if( frees <= 0 )
			{
			frees = static_cast<TInt>( tv.iFree );
			}
		User::LeaveIfError( iFile->Replace( iParent->Fs(), KFileName, EFileWrite ) );
		User::LeaveIfError( iFile->SetSize( frees ) );
		iFile->Close(); 
		}
	else
		{
		frees = static_cast<TInt>( tv.iFree - KMinusFull );
		if( frees <= 0 )
			{
			frees = static_cast<TInt>( tv.iFree );
			}
		User::LeaveIfError( iFile->Replace( iParent->Fs(), KFileName, EFileWrite ) );
		_LIT(KSingleFileSize,"Size of lone file %d");
		INFO_PRINTF2(KSingleFileSize, frees );
		User::LeaveIfError( iFile->SetSize( frees ) );
		iFile->Close();
		}
		
	User::LeaveIfError( iParent->Fs().Volume(tv) );
	
	INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);
	}	


/**
deletes all empty files and databses created for this test
*/
void CLowDiskSpaceStep::ClearDiskL()
	{
	TPtrC tname( KFileName );
	TInt err = KErrNone;

	TInt i = 0;
	for(i = 0 ; KErrNotFound != err ; ++i )
		{
		HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
		TPtr fptr=fval->Des();
		fptr.Append( tname );
		fptr.AppendNum( i );
		err = iParent->Fs().Delete( fptr );
		if( KErrNone != err && KErrNotFound != err )
			{
			User::Leave( err );
			}
		
		CleanupStack::PopAndDestroy( fval );
		}
	err = iParent->Fs().Delete( KFileName );
	err |= iParent->Fs().Delete( KExport1 );

	TRAPD(err2, CContactDatabase::DeleteDatabaseL( KCntDB2 ) );
	err |= err2;
	TRAP( err2, CContactDatabase::DeleteDatabaseL( KCntDB3 ) );
	err |= err2;
		
	if( err != KErrNone && err != KErrNotFound )
		{
		User::Leave( err );
		}
	}

/**
delete an empty file so that we are no longer in lowdisk mode
*/
void CLowDiskSpaceStep::HighDiskModeL()
	{
	TInt err = iParent->Fs().Delete( KFileName );
	if( KErrNotFound == err )
		{
		TPtrC tname( KFileName );
		HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
		TPtr fptr=fval->Des();
		fptr.Append( tname );
		fptr.AppendNum( iManyFiles-1 );
		err |= iParent->Fs().Delete( fptr );
		CleanupStack::PopAndDestroy( fval );		
		}
	
	if( err != KErrNone && err != KErrNotFound )
		{
		User::Leave( err );
		}
	iExpectedError = KErrNone;
	CContactDatabase *cdb = NULL;
	cdb = CContactDatabase::ReplaceL( KCntDB2 );
	CLEAR(cdb);
	cdb = CContactDatabase::ReplaceL( KCntDB3 );
	CLEAR(cdb);
	
	TVolumeInfo tv;
	User::LeaveIfError( iParent->Fs().Volume(tv) );
	INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);	
	}

/**
increase size of last empty file so that we reach diskfull mode again
*/	
void CLowDiskSpaceStep::RefillL()
	{
	TVolumeInfo tv;
	User::LeaveIfError( iParent->Fs().Volume(tv) );
	
	//if not in lowdisk mode or disk is full, return
	if( ( KErrDiskFull != iExpectedError ) || tv.iFree <= KMinusFull)
		{
		return;
		}
	TInt err = iFile->Open( iParent->Fs(), KFileName, EFileWrite );
	if( KErrNotFound == err )
		{
		TPtrC tname( KFileName );
		HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
		TPtr fptr=fval->Des();
		fptr.Append( tname );
		fptr.AppendNum( iManyFiles-1 );
		err |= iFile->Open( iParent->Fs(), fptr, EFileWrite );
		CleanupStack::PopAndDestroy( fval );		
		}
	if( err != KErrNone && err != KErrNotFound )
		{
		User::Leave( err );
		}
	TInt oldsize = 0;
	User::LeaveIfError( iFile->Size( oldsize ) );
	TInt newsize =  oldsize + static_cast<TInt>(tv.iFree - KMinusFull);
	User::LeaveIfError( iFile->SetSize( newsize ) );
	iFile->Close();
	
	User::LeaveIfError( iParent->Fs().Volume(tv) );
	INFO_PRINTF3(KDiskSize, tv.iSize, tv.iFree);	
	}
	
void CLowDiskSpaceStep::PrintFail( const TDesC &aFunc, const TBool aRet, const TInt aErr )
	{
	_LIT(KFail,"Lowdisk test for %S Api has failed with following error: %d");
	if(!aRet && !iErrorPrinted)
		{
		INFO_PRINTF3(KFail, &aFunc, aErr);
		iErrorPrinted = ETrue;
		}
	}
	
TBool CLowDiskSpaceStep::CrudTestL()
	{
	INFO_PRINTF1(KTest1);
	iErrorPrinted = EFalse;
	TBool ret = EFalse;
	TInt err = KErrNone;
	CContactItem *item = NULL;
	
	
	RefillL();
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	TRAP( err, iContactsDatabase->AddNewContactL(*contact) );
	CleanupStack::PopAndDestroy(contact);
	ret = ( iExpectedError == err );
	_LIT(KAddNewContact,"AddNewContactL");
	PrintFail(KAddNewContact, ret, err );
	
	RefillL();
	TRAP( err, 	item = iContactsDatabase->OpenContactL( iIterate->NextL() ) );
	if( item )
		{
		CleanupStack::PushL( item );
		}
	ret = ret && ( KErrNone == err );
	_LIT(KOpenContactL,"OpenContactL");
	PrintFail(KOpenContactL, ret, err );
	
	if( NULL != item )
		{
		RefillL();
		TRAP( err, iContactsDatabase->CommitContactL( *item ) );
		ret = ret && ( iExpectedError == err );
		CleanupStack::PopAndDestroy( item );
		item = NULL;
		_LIT(KCommitContact,"CommitContactL");
		PrintFail(KCommitContact, ret, err );
		}
	
	RefillL();
	TRAP( err, 	item = iContactsDatabase->ReadContactL( iIterate->NextL() ) );
	if( item )
		{
		CleanupStack::PushL( item );
		}
	ret = ret && ( KErrNone == err );
	_LIT(KReadContact,"ReadContactL");
	PrintFail(KReadContact, ret, err );
	

	if( NULL != item )
		{
		RefillL();
		TRAP( err, 
					iContactsDatabase->UpdateContactLC( iIterate->CurrentL(),  item );
					CleanupStack::PopAndDestroy() );
		ret = ret && ( iExpectedError == err );
		CleanupStack::PopAndDestroy( item );
		item = NULL;
		_LIT(KUpdateContact,"UpdateContactLC");
		PrintFail(KUpdateContact, ret, err );
		}
	
	RefillL();	
	TRAP( err, iContactsDatabase->DeleteContactL( iIterate->NextL() ) );
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__  // Should always be able to delete (even in low disk) on 9.5 through SQLite	
	ret = ret && ( KErrNone == err );
#else
    ret = ret && ( iExpectedError  == err );
#endif	
	_LIT(KDeleteContact,"DeleteContactL");
	PrintFail(KDeleteContact, ret, err );
	if( KErrNone == err )
		{
		iIterate->RemoveL( iIterate->CurrentL() );
		--iContacts;
		}
		
	return ret;
	}

TBool CLowDiskSpaceStep::DatabaseTestsL()
	{
	INFO_PRINTF1(KTest2);
	iErrorPrinted = EFalse;
	TBool ret = EFalse;
	TInt err = KErrNone;
	CContactDatabase *cdb = NULL;
	
	RefillL();
	CLEAR( iContactsDatabase );
	TRAP( err, iContactsDatabase = CContactDatabase::OpenL() );
	ret = ( KErrNone == err );
	_LIT(KOpenDB,"OpenL");
	PrintFail(KOpenDB, ret, err );
	
	RefillL();
	TRAP( err, cdb = CContactDatabase::ReplaceL( KCntDB ) );
	ret = ret && ( KErrNone == err );
	_LIT(KReplaceDB,"ReplaceL");
	PrintFail(KReplaceDB, ret, err );
	CLEAR( cdb );
	
	RefillL();
	TRAP( err, CContactDatabase::DeleteDatabaseL( KCntDB ) );
	ret = ret && ( KErrNone == err );
	_LIT(KDeleteDB,"DeleteDatabaseL");
	PrintFail(KDeleteDB, ret, err );
	
	RefillL();
	TRAP( err, iContactsDatabase->RecoverL() );
	ret = ret && ( KErrNone == err );
	_LIT(KRecoverDB,"RecoverL");
	PrintFail(KRecoverDB, ret, err );
	
	RefillL();
	TRAP( err, iContactsDatabase->CompactL() );
	ret = ret && ( KErrNone == err );
	_LIT(KCompactDB,"CompactL");
	PrintFail(KCompactDB, ret, err );
	RefillL();
	
	return ret;
	}

	
TBool CLowDiskSpaceStep::MiscDbTestsL()
	{
	INFO_PRINTF1(KTest3);
	iErrorPrinted = EFalse;
	TBool ret = EFalse;
	TInt err = KErrNone;
	
	RefillL();
	TRAP( err, iContactsDatabase->DatabaseBeginL(EFalse) );
	ret = ( iExpectedError == err );
	_LIT(KTransactionStart,"DatabaseBeginL");
	PrintFail(KTransactionStart, ret, err );
	
	if( KErrNone == err)
		{
		iContactsDatabase->DatabaseCommitL( EFalse );
		}
	RefillL();
	TRAP( err, 
				CDesCArray *array = CContactDatabase::ListDatabasesL(); 
				delete array;
		);
	ret = ret && ( KErrNone == err  );
	_LIT(KListDBs,"ListDatabasesL");
	PrintFail(KListDBs, ret, err );
	
	RefillL();
	TRAP( err, CContactDatabase::ContactDatabaseExistsL(KCntDB2) );
	ret = ret && ( KErrNone == err  );
	_LIT(KDatabaseExists,"ContactDatabaseExistsL");
	PrintFail(KDatabaseExists, ret, err );
	
	RefillL();
	TRAP( err, 
				CContactItem  *array = iContactsDatabase->CreateContactGroupL(); 
				delete array;
		);
	ret = ret && ( iExpectedError == err  );
	_LIT(KCreateContactGroup,"CreateContactGroupL");
	PrintFail(KCreateContactGroup, ret, err );
	TRAP( err, 
				CContactItem  *array = iContactsDatabase->CreateContactCardTemplateL(KCntDB2);
				delete array;
		);
	ret = ret && ( iExpectedError == err  );
	_LIT(KCreateContactCardTemplate2,"CreateContactCardTemplateL DB2");
	PrintFail(KCreateContactCardTemplate2, ret, err );

	TContactItemId contactId = iIterate->NextL();
	OpenL( contactId );
	
	RefillL();
	TRAP( err, 
				CContactItem  *array = iContactsDatabase->CreateContactCardTemplateL(iContactItem, KCntDB3);
				delete array;
		);
	ret = ret && ( iExpectedError == err  );
	_LIT(KCreateContactCardTemplate3,"CreateContactCardTemplateL DB3");
	PrintFail(KCreateContactCardTemplate3, ret, err );
	RefillL();
	
	TInt fieldId=iFields->Find(KUidContactFieldPhoneNumber);
	if( KErrNone == iExpectedError )
		{
		// if this is not set, speeddial cant be set
		TRAP(err,
			SetFieldL(fieldId,GetFieldL(KStringFields, KField, 1));
			CommitL(EFalse);
			OpenL(contactId);
			);
		if( err )
			{
			_LIT(KSpeedDialError,"setting speed dial filed in contact fails with %d error");
			INFO_PRINTF2(KSpeedDialError, err);
			}
		}
	TRAP( err, iContactsDatabase->SetFieldAsSpeedDialL(*iContactItem, fieldId, 1) );
	ret = ret && ( iExpectedError == err  );
	_LIT(KSetFieldAsSpeedDial,"SetFieldAsSpeedDialL");
	PrintFail(KSetFieldAsSpeedDial, ret, err );
	
	RefillL();
	TRAP( err, iContactsDatabase->SetOwnCardL( *iContactItem ) );
	ret = ret && ( iExpectedError == err  );
	_LIT(KSetOwnCard,"SetOwnCardL");
	PrintFail(KSetOwnCard, ret, err );
	CloseL( ETrue );
	//this doesnt leave... it should though... why does it leave with -1(trapped internally)?
	RefillL();
	TRAP( err, iContactsDatabase->SetCurrentItem( iIterate->NextL() ) );
	ret = ret && ( KErrNone == err  );
	_LIT(KSetCurrentItem,"SetCurrentItem");
	PrintFail(KSetCurrentItem, ret, err );
	RefillL();
	TRAP( err, iContactsDatabase->SetPrefTemplateL( *iCustomTemplate ) );
	ret = ret && ( iExpectedError == err  );
	_LIT(KSetPrefTemplate,"SetPrefTemplateL");
	PrintFail(KSetPrefTemplate, ret, err );
	RefillL();
	TRAP( err, 
				CContactItem  *array = iContactsDatabase->CreateOwnCardL();
				delete array;
		);
	ret = ret && ( iExpectedError == err  );
	_LIT(KCreateOwnCard,"CreateOwnCardL");
	PrintFail(KCreateOwnCard, ret, err );
	RefillL();
	TRAP( err, CContactDatabase::RecreateSystemTemplateL( KCntDB2 ) );
	ret = ret && ( iExpectedError == err  );
	_LIT(KRecreateSystemTemplate,"RecreateSystemTemplateL");
	PrintFail(KRecreateSystemTemplate, ret, err );
	RefillL();
	TRAP( err, iContactsDatabase->AddContactToGroupL( iIterate->CurrentL(), iGroup->Id() ) );
	ret = ret && ( iExpectedError == err  );
	_LIT(KAddContactToGroup,"AddContactToGroupL");
	PrintFail(KAddContactToGroup, ret, err );
	RefillL();
	TRAP( err, iContactsDatabase->RemoveContactFromGroupL( iGroupedContact, iGroup->Id() ) );
	ret = ret && ( iExpectedError == err  );
	_LIT(KRemoveContactFromGroup,"RemoveContactFromGroupL");
	PrintFail(KRemoveContactFromGroup, ret, err );
	
	RefillL();
	RFileReadStream rfrs;
	CleanupClosePushL( rfrs );
	User::LeaveIfError( rfrs.Open( iParent->Fs(), KExport1, EFileRead | EFileShareAny ) );
	TRAP( err, ImportVcardL(iContacts+10, rfrs); );
	CleanupStack::PopAndDestroy(&rfrs);
	if(err != KErrNoMemory)
		{
	    ret = ret && ( iExpectedError == err );
		_LIT(KImportVcard,"ImportVcardL");
		PrintFail(KImportVcard, ret, err );
		}
	
	return ret;
	}


/**
start transaction
add new contacts
transition to lowdisk mode
add new contact
commit/rollback transactions
check that commit was successful
check that only initial batch of/no contact were added to the database
*/
void CLowDiskSpaceStep::FailTransactionTestL( const TTransactionType aTransactionType )
	{
	const TInt KInitialContacts = 2;
	const TInt KFullContacts = 5;
	
	TInt error = KErrNone;
	iContacts = 0;
	CPerformanceFunctionalityBase::InitializeL();
	
	//start transaction
	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	iContacts = KInitialContacts;
	TInt k = 0;
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	//add empty contacts
	for(; k < iContacts; k++)
		{		
		iIterate->AddL( iContactsDatabase->doAddNewContactL(*contact,EFalse,ETrue) );
		}	
		
	//transition to low disk mode
	StartLowDiskModeL();
	iContacts = KFullContacts;
	
	//confirm that you can't add new contact in low disk mode
	for(; k < iContacts; k++)
		{		
		TRAP( error, 
			iIterate->AddL( iContactsDatabase->doAddNewContactL(*contact,EFalse,ETrue) );
			);
		TESTPRINTI( KErrDiskFull == error, k);
		}
		
	CleanupStack::PopAndDestroy(contact);
	
	if( aTransactionType == ERollbackTransaction )
		{
		//rollback transaction
		TRAP( error, iContactsDatabase->DatabaseRollback() );
		//rollback transaction was succusseful in lowdisk mode
		TESTPRINT( KErrNone == error );
		}
	else
		{	
		//commit transaction
		TRAP( error, iContactsDatabase->DatabaseCommitL(EFalse) );
		//commit transaction was succusseful in lowdisk mode
		TESTPRINT( KErrNone == error );
		}
	
	CleanupStack::Pop();//DatabaseBeginLC
		
	
	//transition out of low disk mode
	StartHighdiskModeL();
	
	iIterate->Reset();
	
	
	if( aTransactionType == ERollbackTransaction )
		{
		//confirm that no contacts were added to the database
		TESTPRINT( ( iContactsDatabase->CountL() == 0 ) );
		//inital contact ids were added to local iterator but were not commited to the database
		TESTPRINT( ( iIterate->Count() == KInitialContacts ) );
		}
	else
		{
		//confirm that only initial contacts were added to the database
		TESTPRINT( ( iContactsDatabase->CountL() == KInitialContacts ) );
		TESTPRINT( ( iIterate->Count() == KInitialContacts ) );
		}
	}
	
/**
close session to server
wait for server to close
transition to low disk mode
open new session
check that new session was opened successfully, or that the call failed KErrDiskFull
(Although the Contacts model does not itself leave with KErrDiskFull in this circumstance
the call to OpenL and ReplaceL on DBMS and SQLite can leave with these error codes
in some full disk states).
*/
void CLowDiskSpaceStep::StartServerTestL()
	{
	//close database session
	CLEAR( iContactsDatabase );
	iIterate->Clear();
	//wait for server to close
	WaitForServerToExitL();
	//transition to low disk mode
	StartLowDiskModeL();
	//open new session to server and open defualt database
	TRAPD( error, iContactsDatabase = CContactDatabase::OpenL() );		
	TESTPRINT( error == KErrNone || error == KErrDiskFull );
	//close database session
	CLEAR( iContactsDatabase );
	//wait for server to close
	WaitForServerToExitL();
	//open new session to server and replace defualt database
	TRAP( error, iContactsDatabase = CContactDatabase::ReplaceL() );		
	TESTPRINT( error == KErrNone || error == KErrDiskFull);
	//transition out of low disk state
	StartHighdiskModeL();
	}
	
/**
Transition out of lowdisk mode
*/
TBool CLowDiskSpaceStep::StartHighdiskModeL()
	{
	_LIT(KHighDiskMode,"Switch to testing high disk mode");
	INFO_PRINTF1(KStars);
	INFO_PRINTF1(KHighDiskMode);
	INFO_PRINTF1(KStars);
	HighDiskModeL();
	return ETrue;
	}
	
/**
Transition into lowdisk mode
*/
TBool CLowDiskSpaceStep::StartLowDiskModeL()
	{
	_LIT(KLowDiskMode,"Testing low disk mode");
	INFO_PRINTF1(KStars);
	INFO_PRINTF1(KLowDiskMode);
	INFO_PRINTF1(KStars);
	FillDiskL();
	return ETrue;
	}

