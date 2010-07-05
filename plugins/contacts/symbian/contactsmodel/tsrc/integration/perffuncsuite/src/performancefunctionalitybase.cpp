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

#include <e32panic.h>
#include <cntfldst.h>
#include "performancefunctionalitybase.h"
#include "performancefunctionalitydefs.h"

_LIT(KOtherFields,"OtherFields");
_LIT(KAgentId, "agentid");
_LIT(KFieldTime, "time");
_LIT(KServerUid, "*[10003a73]*" );

#define NM_CASE(x) case x: { return _S(#x); }


CPerformanceFunctionalityBase::~CPerformanceFunctionalityBase()
	{
	}


TVerdict CPerformanceFunctionalityBase::doTestStepPreambleL()

	{
	_LIT(KPreamblePrint,"I am in Base doTestStepPreambleL()");
    INFO_PRINTF1(KPreamblePrint);

	if(!CActiveScheduler::Current())
		{
		CActiveScheduler* sched = NULL;
		sched = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(sched);
		}

	//viewdef that loads all fields
	iViewAll = CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	iViewAll->AddL(KUidContactFieldMatchAll);

	iIterate = new(ELeave) RCntList();

	//call method in derived class
	PreTestL();

	return TestStepResult();
	}

TVerdict CPerformanceFunctionalityBase::doTestStepPostambleL()
	{
	_LIT(KPostamblePrint,"I am in Base doTestStepPostambleL()");
    INFO_PRINTF1(KPostamblePrint);
    EndCompressL();
    EndRecoverL();
	Cleanup();
	delete iViewAll;
	delete iIterate;

	WaitForServerToExitL();//waits for server to close to allow for any memory leaks to be detected

    if( !iConcurrent && !iUnclean )
    	{
		TRAPD(err, CContactDatabase::DeleteDefaultFileL() );
		if( err != KErrInUse && err!= KErrNotFound )
			{
			User::LeaveIfError( err );
			}
    	}

	return TestStepResult();
	}


/**initilize called within __UHEAP_MARK/__UHEAP_MARKEND of doTestStepL,
so that all relevant memmory de/allocation is in scope*/
void CPerformanceFunctionalityBase::InitializeL()
	{
	CLEAR(iContactsDatabase);
	iIterate->Clear();
	if( iConcurrent )
    	{
    	iContactsDatabase = CContactDatabase::OpenL(); //open database created in earlier step
    	for(TInt k = 1; k<=iContacts; k++)
    		{
    		TRAPD( err, ReadL(k));
    		CloseL( ETrue );
    		if( KErrNone == err )
    			{
    			iIterate->AddL( k );
    			}
    		}
		if( NULL  == iTemplate )
			{
			iTemplate = static_cast< CContactTemplate* >(
			iContactsDatabase->ReadContactL( iContactsDatabase->TemplateId(), *iViewAll ) );
			}
    	}
    else
    	{
		iContactsDatabase = CContactDatabase::ReplaceL();//create  clean/fresh database for all tests
		if( NULL  == iTemplate )
			{
			iTemplate = static_cast< CContactTemplate* >(
			iContactsDatabase->ReadContactL( iContactsDatabase->TemplateId(), *iViewAll ) );
			}
		AddEmptyContactsL(iContacts, EFalse);
    	}
	}

/**cleans up called within __UHEAP_MARK/__UHEAP_MARKEND of doTestStepL,
so that all relevant memmory de/allocation is in scope*/
void CPerformanceFunctionalityBase::Cleanup()
	{
	delete iTemplate;
	iTemplate = NULL;
	if(iContactItem)
		{
		iContactsDatabase->CloseContactL( iContactItem->Id() ); // does not leave, despite the L
		}
	delete iContactItem;
	iContactItem = NULL;
	delete iContactsDatabase;
	iContactsDatabase = NULL;
	iIterate->Close();
	}

void CPerformanceFunctionalityBase::AddEmptyContactsL(const TInt aNumber, const TBool aTrans)
/**
Add number of empty contacts to db, using the system template.
@return None
@param aNumber number of contacts to be added
@param aTrans perform add within transaction, for compatibility with old model
@pre None
@post database now contains aNumber of empty contacts
*/
	{
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	if( aTrans )
		{
		for(TInt k = 0; k<aNumber; k++)
			{
			//if within transaction use this api, for test on old model (pre bravo)
			iIterate->AddL( iContactsDatabase->doAddNewContactL(*contact,EFalse,ETrue) );
			}
		}
	else
		{
		for(TInt k = 0; k<aNumber; k++)
			{
			iIterate->AddL( iContactsDatabase->AddNewContactL(*contact) );
			}
		}
	CleanupStack::PopAndDestroy(contact);
	}

void CPerformanceFunctionalityBase::DeleteContactL(const TContactItemId aCid, const TBool aTrans)
	{
	if(aTrans)
		{
		//if within transaction use this api, for test on old model (pre bravo)
		iContactsDatabase->doDeleteContactL( aCid, ETrue, ETrue );
		}
	else
		{
		iContactsDatabase->DeleteContactL( aCid );
		}
	iIterate->RemoveL(aCid);
	}

/**
Iterate throught a list of values in the ini file, reset start iteration from beginning
*/
TPtrC CPerformanceFunctionalityBase::GetNextFieldL( const TDesC &aConfigSection,
													const TDesC &aName,
													const TBool aNewSection)
	{
	if( aNewSection )
		{
		iNextFieldPosition = 0;
		}
	++iNextFieldPosition;
	TPtrC ret = GetFieldL( aConfigSection, aName, iNextFieldPosition );
	if( ret == KNullDesC )
		{
		return GetNextFieldL(aConfigSection, aName, ETrue);
		}
	return ret;
	}
/**
get the nth item from a list of items in the ini file
*/
TPtrC CPerformanceFunctionalityBase::GetFieldL(const TDesC &aConfigSection, const TDesC &aName, const TInt aIndex)
	{
	TPtrC ret;
	TPtrC tfield( aName );
	HBufC *fval=HBufC::NewLC(tfield.Length()+2);//assume #fields < 100

	TPtr fptr=fval->Des();
	fptr.Append( tfield );
	fptr.AppendNum( aIndex );
	GetStringFromConfig( aConfigSection, fptr, ret );
	CleanupStack::PopAndDestroy( fval );
	return ret;
	}


/*opens contact item at aCid using viewdef aView for editing, initilises iContactItem*/
void CPerformanceFunctionalityBase::OpenL( const TContactItemId aCid )
	{
	iContactItem = iContactsDatabase->OpenContactL( aCid, *iViewAll );
	iFields = &(iContactItem->CardFields());
	}

/*commits open contact, must be called after openl*/
void CPerformanceFunctionalityBase::CommitL(const TBool aTrans)
	{
	if(iContactItem != NULL)
		{
		iFields = NULL;
		if(aTrans)
			{
			iContactsDatabase->doCommitContactL(*iContactItem, ETrue, ETrue );
			}
		else
			{
			iContactsDatabase->CommitContactL(*iContactItem);
			}
		delete iContactItem;
		iContactItem = NULL;
		}
	}
/*reads (readonly) contact item at aCid using viewdef aView for viewing, initilises iContactItem*/
void CPerformanceFunctionalityBase::ReadL( const TContactItemId aCid )
	{
	iContactItem = iContactsDatabase->ReadContactL( aCid, *iViewAll );
	iFields = &(iContactItem->CardFields());
	}

/*cleans and closes open contact, must be called after readl/openl,
if aClose true, only unloads contact from memory*/
void CPerformanceFunctionalityBase::CloseL(const TBool aClose)
	{
	if( iContactItem != NULL )
		{
		if ( aClose )
			{
			iContactsDatabase->CloseContactL( iContactItem->Id() );//this function cant leave
			}
		iFields = NULL;
		CLEAR( iContactItem );
		}
	}

/*sets a specific contact field to a value as determined by its type.
field at aPos in aFields is set to aText*/
void CPerformanceFunctionalityBase::SetFieldL(const TInt aPos, const TDesC& aText)
	{
	CContactItemField &field = (*iFields)[aPos];
	if( field.IsTemplateLabelField() )
		{
		return;
		}

	if( 0 == aText.Size() )
		{
		return;
		}

	switch(field.StorageType())
		{
		case KStorageTypeText:
			field.TextStorage()->SetTextL(aText);
		break;
		case KStorageTypeStore:
			{
			HBufC8 *buf = HBufC8::NewLC(aText.Size());
			TPtr8 tptr8 = buf->Des();
			tptr8.Copy(aText);
			field.StoreStorage()->SetThingL(*buf);
			CleanupStack::PopAndDestroy(buf);
			}
		break;
		case KStorageTypeContactItemId:
			{
			TInt id = 0;
			if( aText.Size() > 0 )
				{
				GetIntFromConfig( KOtherFields, KAgentId, id );
				}
			field.AgentStorage()->SetAgentId(id);
			}
		break;
		case KStorageTypeDateTime:
			{
			if( aText.Size() > 0 )
				{
				TPtrC time;
				GetStringFromConfig( KOtherFields, KFieldTime, time );
				field.DateTimeStorage()->SetTime( TTime(time) );
				}
			else
				{
				field.DateTimeStorage()->SetTime( TTime(0) );
				}
			}
		break;
		default:
			User::Panic(aText,EInvariantFalse);
		break;
		}
	}


/*checks the value of a specific field as determined by it type
returns true if field at aPos in aFields equals aText*/
TBool CPerformanceFunctionalityBase::CompareFieldL(const TInt aPos, const TDesC& aText)
	{
	TBool ret = EFalse;
	const CContactItemField &field = (*iFields)[aPos];

	if(  field.IsTemplateLabelField() )
		{
		return ret;
		}

	if( 0 == aText.Size() && !field.Storage()->IsFull() )
		{
		return ETrue;
		}
	else if( !field.Storage()->IsFull() )
		{
		return EFalse;
		}

	switch( field.StorageType() )
		{
		case KStorageTypeText:
			ret = field.TextStorage()->Text() == aText;
		break;
		case KStorageTypeStore:
			{
			HBufC8 *buf = HBufC8::NewLC(aText.Size());
			TPtr8 tptr8 = buf->Des();
			tptr8.Copy(aText);
			ret = field.StoreStorage()->Thing()->Compare(*buf) == 0;
			CleanupStack::PopAndDestroy(buf);
			}
		break;
		case KStorageTypeContactItemId:
			{
			TInt id = 0;
			GetIntFromConfig( KOtherFields, KAgentId, id );
			ret = field.AgentStorage()->Value() == id;
			}
		break;
		case KStorageTypeDateTime:
			{
			TPtrC time;
			GetStringFromConfig( KOtherFields, KFieldTime, time );
			ret = field.DateTimeStorage()->Time() == TTime(time);
			}
		break;
		default:
			User::Panic(aText,EInvariantFalse);
		break;
		}
	return ret;

	}
/**
sets all the fields of a currently read / open item to aText
*/
void CPerformanceFunctionalityBase::SetAllFieldsL(const TDesC& aText)
	{
	const TInt count = iFields->Count();
	SetManyFieldsL(aText, count);
	}
/**
sets aMany fields of a currently read / open item to aText
*/
void CPerformanceFunctionalityBase::SetManyFieldsL(const TDesC& aText, const TInt aMany)
	{
	TInt count = iFields->Count();
	if( count > aMany )
		{
		count = aMany;
		}
	TInt i = 0;
	for(; i < count; ++i)
		{
		SetFieldL(i,aText);
		}
	}

/**
checks all the fields of a currently read / open item equal to aText
*/
TBool CPerformanceFunctionalityBase::CheckAllFieldsL(const TDesC& aText)
	{
	const TInt count = iFields->Count();
	TInt i = 0;
	TInt ret = ETrue;
	for(; i < count; ++i)
		{
		ret = ret && CompareFieldL(i, aText);
		}
	if ( 0 == count )
		{
		ret = EFalse;
		}
	return ret;
	}

/**
exports all contacts items in db to vcard file, if aDelete, cleans database afterwards
*/
void CPerformanceFunctionalityBase::ExportVcardL(const TBool aDelete, const TDesC &aVCard)
	{
	iIterate->Reset();
	CContactIdArray *ids = CContactIdArray::NewLC();
	TInt i = 0;
	for(; i < iContacts; ++i)
		{
		ids->AddL( iIterate->NextL() );
		}

	RFileWriteStream rfws;
	CleanupClosePushL(rfws);
	User::LeaveIfError( rfws.Replace( iParent->Fs(),aVCard , EFileWrite | EFileShareAny ) );

	iContactsDatabase->ExportSelectedContactsL(	TUid::Uid(KUidVCardConvDefaultImpl), *ids, rfws,
										CContactDatabase::EIncludeX | CContactDatabase::ETTFormat );

	CleanupStack::PopAndDestroy(&rfws);
	if( aDelete )
		{
		CLEAR( iContactsDatabase );
		iContactsDatabase = CContactDatabase::ReplaceL();
		}
	CleanupStack::PopAndDestroy(ids);
	}


/**
imports aMany contacts from a vacrds file, as specified by the readstream
*/
void CPerformanceFunctionalityBase::ImportVcardL(const TInt aMany, RFileReadStream &aRfrs)
	{
	TBool sucess = EFalse;

	TInt i = 0;
	for(; i < aMany; ++i)
		{
		CArrayPtr<CContactItem>* imported= 	iContactsDatabase->ImportContactsL(
											TUid::Uid( KUidVCardConvDefaultImpl ), aRfrs, sucess,
											CContactDatabase::EIncludeX | CContactDatabase::ETTFormat |
											CContactDatabase::EImportSingleContact
											);
		imported->ResetAndDestroy();
		CLEAR(imported);
		if( !sucess )
			{
			break;
			}
		sucess = EFalse;
		}
	}

/**
used by concurrent tests to read shared Ints from aShare
*/
TInt CPerformanceFunctionalityBase::GetSharedIntL(const TDesC &aShare)
	{
	TInt ret = -1;
	TBuf<6> txtReadData(NULL); // assume #event/contacts < 1000,000
	ReadSharedDataL(aShare, txtReadData); // Data is read into the buffer
	if ( txtReadData.Length() > 0 )
		{
		TLex lex( txtReadData );
		User::LeaveIfError( lex.Val( ret ) );
		}
	return ret;

	}

/**
used by concurrent tests to write shared Ints to aShare either appedning or overwriteing
*/
void CPerformanceFunctionalityBase::ShareIntL(	const TDesC & aShare, const TInt aInt,
												const TBool aAppend, const TDesC &aSeperator)
	{
	HBufC *fval=HBufC::NewLC(4);//assume #events < 10000

	TPtr fptr=fval->Des();
	fptr.Append(aSeperator);
	fptr.AppendNum( aInt );
	WriteSharedDataL(aShare, fptr,  (aAppend ? EAppendText : ESetText) ); // Data copied to shared location
	CleanupStack::PopAndDestroy( fval );
	}

/**
used by concurrent tests to write shared text to aShare either appedning or overwriteing
*/
void CPerformanceFunctionalityBase::SetSharedTextL(const TDesC &aShare, const TDesC &aText, const TBool aAppend)
	{

	TPtrC txtWriteData;
	txtWriteData.Set(aText); // Copy the literal to a text buffer
	WriteSharedDataL(aShare, txtWriteData, (aAppend ? EAppendText : ESetText) ); // Data copied to shared location

	}

/**
used by concurrent tests to read the length of string stored in shared memmory aShare
*/
TInt CPerformanceFunctionalityBase::SharedCountL(const TDesC &aShare)
	{

	TBuf<256> txtReadData(NULL); // Initialise a buffer for reading shared data
	ReadSharedDataL(aShare, txtReadData); // Data is read into the buffer
	return txtReadData.Length();

	}

void CPerformanceFunctionalityBase::MissingTestPanic() const
	{
	_LIT(KInvalid,"Invalid Test Case");
	User::Panic(KInvalid, 444);
	}

const TText* CPerformanceFunctionalityBase::PrintNotification(const TInt aNotification) const
	{
	switch ( aNotification )
		{
		NM_CASE( EContactDbObserverEventNull );
		NM_CASE( EContactDbObserverEventUnused );
		NM_CASE( EContactDbObserverEventContactChanged );
		NM_CASE( EContactDbObserverEventContactDeleted );
		NM_CASE( EContactDbObserverEventContactAdded );
		NM_CASE( EContactDbObserverEventUnknownChanges );
		NM_CASE( EContactDbObserverEventRecover );
		NM_CASE( EContactDbObserverEventRollback );
		NM_CASE( EContactDbObserverEventTablesClosed );
		NM_CASE( EContactDbObserverEventTablesOpened );
		NM_CASE( EContactDbObserverEventTemplateChanged );
		NM_CASE( EContactDbObserverEventTemplateDeleted );
		NM_CASE( EContactDbObserverEventTemplateAdded );
		NM_CASE( EContactDbObserverEventCurrentItemDeleted );
		NM_CASE( EContactDbObserverEventCurrentItemChanged );
		NM_CASE( EContactDbObserverEventOwnCardChanged );
		NM_CASE( EContactDbObserverEventPreferredTemplateChanged );
		NM_CASE( EContactDbObserverEventOwnCardDeleted );
		NM_CASE( EContactDbObserverEventGroupAdded );
		NM_CASE( EContactDbObserverEventGroupChanged );
		NM_CASE( EContactDbObserverEventGroupDeleted );
		NM_CASE( EContactDbObserverEventCurrentDatabaseChanged );
		NM_CASE( EContactDbObserverEventSpeedDialsChanged );
		NM_CASE( EContactDbObserverEventSortOrderChanged );
		NM_CASE( EContactDbObserverEventBackupBeginning );
		NM_CASE( EContactDbObserverEventRestoreBeginning );
		NM_CASE( EContactDbObserverEventBackupRestoreCompleted );
		NM_CASE( EContactDbObserverEventRestoreBadDatabase );
		default: return _S( "UnsupportedEvent" );
		}
	}

void CPerformanceFunctionalityBase::PrintNotifications(	const TDesC &aType, const RArray<TInt> &aEvents,
														const RArray<TInt> &aContacts)
	{
	_LIT(KEvent, "%S %d event(s)");
	const TInt count = ( aEvents.Count() < aContacts.Count() ) ? aEvents.Count() : aContacts.Count();
	INFO_PRINTF3(KEvent , &aType, count );
	for(TInt i = 0; i < count ; ++i)
		{
		_LIT(KEventAndContact,"%S event %s for contact %d");
		INFO_PRINTF4(KEventAndContact , &aType, PrintNotification(aEvents[i]), aContacts[i] );
		}
	}

void CPerformanceFunctionalityBase::WaitForServerToExitL()
	{
/**
hardware testing is performed using techview, which always has a session open to cntsrv,
hence cntsrv never shutsdown when performing HW testing.
*/
#ifdef __WINSCW__

	// The name of the CntSrv process includes its uid like this CNTSRV.EXE[10003a73]001
	TInt findProcessResult = KErrNone;
	TFindProcess findProcess( KServerUid );
	TFullName  fullNameLastProcess;
	TFullName  fullNameNextProcess;
	//TFullName
	findProcessResult = findProcess.Next(fullNameNextProcess);


	for ( TInt iteration = 1; findProcessResult == KErrNone; ++iteration )
		{
		fullNameLastProcess = fullNameNextProcess;
		RProcess process;
		const TInt OpenError = process.Open(fullNameLastProcess);
		if ( OpenError == KErrNone)
			{
			TExitCategoryName category = process.ExitCategory();
			//if this handle to server is still open, then wait for this handle to close
			if( 0 == category.Length() )
			    {
			    CloseProcessL( fullNameLastProcess, iteration );
			    }
			}
		findProcessResult = findProcess.Next(fullNameNextProcess);
		}

#else
KServerUid();//removes unused variable warning
#endif
	}

void CPerformanceFunctionalityBase::CloseProcessL(const TDesC& aProcessName, const TInt aIteration)
    {
	_LIT(KillString, "Kill");
	if ( aProcessName != KNullDesC )
		{
		_LIT(KServerRunning,"*** The CntSrv process is running ***");
		_LIT(KServerIteration, "Found server process on iteration: %d");
		INFO_PRINTF1(KServerRunning);
		INFO_PRINTF2(KServerIteration, aIteration);

		// the CntSrv process is running so wait
		RProcess process;
		const TInt OpenError = process.Open(aProcessName);
		if ( OpenError == KErrNone)
			{
			TRequestStatus status;
			process.Logon(status); // ask for a callback when the process ends
			User::WaitForRequest(status);
			const TInt serverError = process.ExitReason();
			TExitCategoryName category = process.ExitCategory();
			_LIT(KServerClosed,"*** The CntSrv process has ended with error type: %S, and error value: %d ***");
			INFO_PRINTF3(KServerClosed, &category, serverError);
			process.Close();
			if( ( category != KillString ) || ( serverError != KErrNone ) )
				{
				_LIT(KServerError,"*** The server caused an error while closing ***");
				ERR_PRINTF1(KServerError);
				TEST1( EFalse, ETrue);
				}
			}
		else
			{
			_LIT(KProcessFailure,"*** Failed to open process, error: %d ***");
			ERR_PRINTF2(KProcessFailure, OpenError);
			TEST1(EFalse, ETrue);
			}
		}
	else
		{
		_LIT(KNoProcess,"*** The CntSrv process is NOT running ***");
		ERR_PRINTF1(KNoProcess);
		TEST1(EFalse, ETrue);
		}
    }

void CPerformanceFunctionalityBase::PrintTestComparison(const TBool aResult,
														const TText *aCompare,
														const TInt aLine,
														const TText *aFile)
	{
	_LIT(KComparePrint, "The comparison: %s failed at line: %d in file: %s");
	if( !aResult )
		{
		ERR_PRINTF4( KComparePrint, aCompare, aLine, aFile );
		TEST1( EFalse , ETrue );
		}
	}

void CPerformanceFunctionalityBase::PrintTestComparisonIteration(	const TBool aResult,
																	const TInt aIteration,
																	const TText *aCompare,
																	const TInt aLine,
																	const TText *aFile)
	{
	_LIT(KComparePrint, "The comparison: %s failed on iteration: %d, at line: %d in file: %s");
	if( !aResult )
		{
		ERR_PRINTF5( KComparePrint, aCompare, aIteration, aLine, aFile );
		TEST1( EFalse , ETrue );
		}
	}

TBool CPerformanceFunctionalityBase::StringComparisonTestPrint(	const TDesC &aCompareL,
                												const TText *aCompareOperator,
                												const TDesC &aCompareR,
                												const TBool aResult,
                												const TInt 	aIteration,
                												const TInt 	aSubIteration,
                												const TInt 	aLine,
                												const TText *aFile)
	{
	_LIT(KComparePrint, "%s %s %s: failed on subiteration %d of iteration %d");
	_LIT(KFilePrint, "In file: %s at line: %d");
	if( !aResult )
		{
		ERR_PRINTF6( KComparePrint, (TText*)aCompareL.Ptr(), aCompareOperator, (TText*)aCompareR.Ptr(), aSubIteration, aIteration);
		ERR_PRINTF3( KFilePrint, aFile, aLine );
		TEST1( EFalse , ETrue );

		}
	return aResult;
	}

TBool CPerformanceFunctionalityBase::IntegerComparisonTestPrint(	const TInt	aCompareL,
                    												const TText *aCompareOperator,
                    												const TInt 	aCompareR,
                    												const TBool aResult,
                    												const TInt 	aIteration,
                    												const TInt 	aSubIteration,
                    												const TInt 	aLine,
                    												const TText *aFile)
	{
	_LIT(KComparePrint, "%d %s %d: failed on subiteration %d of iteration %d");
	_LIT(KFilePrint, "In file: %s at line: %d");
	if( !aResult )
		{
		ERR_PRINTF6( KComparePrint, aCompareL, aCompareOperator, aCompareR, aSubIteration, aIteration);
		ERR_PRINTF3( KFilePrint, aFile, aLine );
		TEST1( EFalse , ETrue );
		}
	return aResult;
	}

TActiveRecoverObserver::TActiveRecoverObserver(CPerformanceFunctionalityBase *aTestStep)
                            : iTestStep(aTestStep)
    {
    }

void TActiveRecoverObserver::Step(TInt )
	{
	//does nothing
	}

void TActiveRecoverObserver::HandleError(TInt aError)
	{
	_LIT(KPrintError, "Error in active compress/recover: %d");
	iCompressOrRecoverError = aError;
	iTestStep->ALLPRINT2(KPrintError, iCompressOrRecoverError);
	}


/**
Damage database and start active recovery
*/
void CPerformanceFunctionalityBase::StartRecoverL()
	{
	iActiveRecoverObserver.iCompressOrRecoverError = KErrNone;
	_LIT(KStartRecover, "StartRecover");
	ALLPRINT(KStartRecover);

	#ifdef _DEBUG
		#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
			if( !iContactsDatabase->IsDamaged() )
				{
				iContactsDatabase->DamageDatabaseL(0x666);//will give us consistent behaviour on debug builds
				}
		#endif
	#endif

	iRecover = iContactsDatabase->CreateRecoverLC();
	iRecover->SetObserver( &iActiveRecoverObserver );
	CleanupStack::Pop( iRecover );
	}

/**
if iCompleteSteps, complete active recovery and cancel
otherwise just cancel
*/
void CPerformanceFunctionalityBase::EndRecoverL()
	{
	_LIT(KEndRecover, "EndRecover");

	if( iRecover )
		{
        	ALLPRINT(KEndRecover);
		if(iCompleteSteps)
			{
			while(iRecover->Step()){}
			}
		iRecover->Cancel();
		iRecover = NULL;
		}
	}

/**
Start active compress
*/
void CPerformanceFunctionalityBase::StartCompressL()
	{
	iActiveRecoverObserver.iCompressOrRecoverError = KErrNone;
	_LIT(KStartCompress, "StartCompress");
	ALLPRINT(KStartCompress);
	iCompress = iContactsDatabase->CreateCompressorLC();
	iCompress->SetObserver( &iActiveRecoverObserver );
	CleanupStack::Pop( iCompress );
	}

/**
if iCompleteSteps, complete active compress and cancel
otherwise just cancel
*/
void CPerformanceFunctionalityBase::EndCompressL()
	{
	_LIT(KEndCompress, "EndCompress");

	if( iCompress )
		{
        	ALLPRINT(KEndCompress);
		if(iCompleteSteps)
			{
			while(iCompress->Step()){}
			}
		iCompress->Cancel();
		iCompress = NULL;
		}
	}
