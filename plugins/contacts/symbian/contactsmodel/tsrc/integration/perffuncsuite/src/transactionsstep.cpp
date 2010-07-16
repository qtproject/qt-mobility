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
#include "transactionsstep.h"
#include "performancefunctionalitydefs.h"

_LIT(KVcardFile,"c:\\ExportedContacts.vcf");
const TInt KTransactionSize = 16;//number of crud operations performed within a transaction

//number of contacts used by transaction performance
const TInt KMinContacts = 10;
const TInt KMidContacts = 100;
const TInt KManyContacts = 300;
const TInt KMaxContacts = 1000;

const TInt KPerfIter = 1;//number of times performance tests are repeated to get a more accurate average
const TInt KMediumFields = 10;//number of fields to use when a contact is partially filled
const TInt KManyFields = 69;//number of (template-derived) fields in a Contact Item

_LIT(KRun1,"Nested");
_LIT(KRun2,"EmptyCommit");
_LIT(KRun3,"EmptyRevert");
_LIT(KRun4,"UpdateCommit");
_LIT(KRun5,"UpdateRevert");
_LIT(KRun6,"CreateCommit");
_LIT(KRun7,"CreatRevert");
_LIT(KRun8,"DeleteCommit");
_LIT(KRun9,"DeleteRevert");
_LIT(KRun10,"CudCommit");
_LIT(KRun11,"CudRevert");
_LIT(KRun12,"EmptyPerfomance");
_LIT(KRun13,"EmptyEmptyPerformance");
_LIT(KRun14,"MediumPerformance");
_LIT(KRun15,"MediumEmptyPerformance");
_LIT(KRun16,"FullPerformance");
_LIT(KRun17,"FullEmptyPerformance");
_LIT(KRun18,"MultipleCommit");
_LIT(KRun19,"TransactionMultiple");

_LIT(KTest1, "Nested Transaction tests");
_LIT(KTest2, "Empty; Commit Transaction tests");
_LIT(KTest3, "Empty; Revert Transaction tests");
_LIT(KTest4, "Update; Commit Transaction tests");
_LIT(KTest5, "Update; Revert Transaction tests");
_LIT(KTest6, "Create; Commit Transaction tests");
_LIT(KTest7, "Create; Revert Transaction tests");
_LIT(KTest8, "Delete; Commit Transaction tests");
_LIT(KTest9, "Delete; Revert Transaction tests");
_LIT(KTest10, "Many CUD; Commit Transaction tests");
_LIT(KTest11, "Many CUD; Revert Transaction tests");
_LIT(KTest12, "***Empty fields; Performance Transaction tests***");
_LIT(KTest13, "***Empty fields, Empty DB; Performance Transaction tests***");
_LIT(KTest14, "***Medium fields; Performance Transaction tests***");
_LIT(KTest15, "***Medium fields, Empty DB; Performance Transaction tests***");
_LIT(KTest16, "***Full fields; Performance Transaction tests***");
_LIT(KTest17, "***Full fields, Empty DB; Performance Transaction tests***");
_LIT(KTest18, "Multiple commit test");
_LIT(KTest19, "Transaction Multiple commit test");

//number of contacts used by this test step							
#define KNumberOfContacts 10

#ifdef __WINSCW__
#define KMaxTimeAllowed	500 //seconds, minimum performance
#else
#define KMaxTimeAllowed	10000 //hardware is much slower
#endif

#define KNumCUDOperations 3

_LIT(KMinContactsPrint,"$Min contacts$");
_LIT(KMidContactsPrint,"$Mid contacts$");
_LIT(KManyContactsPrint,"$Many contacts$");
_LIT(KMaxContactsPrint,"$Max contacts$");

							
CTransactionsStep::CTransactionsStep(CPerformanceFunctionalityTestsSuite &aParent) : CPerformanceFunctionalityBase(KNumberOfContacts, aParent)
	{
	SetTestStepName(KTransactionsStep);
	}

TVerdict CTransactionsStep::doTestStepPostambleL()
	{
	const TInt error = iParent->Fs().Delete(KVcardFile);
	if( KErrNotFound != error && KErrNone != KErrNone )
		{
		_LIT(KDeleteError,"Unexpected error when deleting vcard file, error: %d");
		ERR_PRINTF2(KDeleteError, error);
		}
	return CPerformanceFunctionalityBase::doTestStepPostambleL();
	}
	
void CTransactionsStep::PreTestL()
	{
	User::LeaveIfError( iParent->Fs().Connect() );
	}


TVerdict CTransactionsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{	
	__UHEAP_MARK;
	InitializeL();
	_LIT(KStartTest,"CTransactionsStep::doTestStepL()");
	ALLPRINT(KStartTest);  //Block start 
	iIterate->Reset();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		ALLPRINT(KTest1);
		NestedTransactionTestL();
		}
	else if( run == KRun2 )
		{
		ALLPRINT(KTest2);
		EmptyCommitTransactionTestL(EFalse);
		}
	else if( run == KRun3 )
		{
		ALLPRINT(KTest3);
		EmptyCommitTransactionTestL(ETrue);
		}
	else if( run == KRun4 )
		{
		ALLPRINT(KTest4);
		UpdateCommitTransactionTestL(EFalse);
		}
	else if( run == KRun5 )
		{
		ALLPRINT(KTest5);
		UpdateCommitTransactionTestL(ETrue);
		}
	else if( run == KRun6 )
		{
		ALLPRINT(KTest6);
		CreateCommitTransactionTestL(EFalse);
		}
	else if( run == KRun7 )
		{
		ALLPRINT(KTest7);
		CreateCommitTransactionTestL(ETrue);
		}
	else if( run == KRun8 )
		{
		ALLPRINT(KTest8);
		DeleteCommitTransactionTestL(EFalse);
		}
	else if( run == KRun9 )
		{
		ALLPRINT(KTest9);
		DeleteCommitTransactionTestL(ETrue);
		}
	else if( run == KRun10 )
		{
		ALLPRINT(KTest10);
		ManyUpdateCommitTransactionTestL(EFalse);
		}
	else if( run == KRun11 )
		{
		ALLPRINT(KTest11);
		ManyUpdateCommitTransactionTestL(ETrue);
		}
	else if( run == KRun12 )
		{
		ALLPRINT(KTest12);
		EmptyTransactionPerformanceTestL( EFalse);
		}
	else if( run == KRun13 )
		{
		ALLPRINT(KTest13);
		EmptyTransactionPerformanceTestL( ETrue );
		}
	else if( run == KRun14 )
		{
		ALLPRINT(KTest14);
		MediumTransactionPerformanceTestL( EFalse);
		}
	else if( run == KRun15 )
		{
		ALLPRINT(KTest15);
		MediumTransactionPerformanceTestL( ETrue);
		}
	else if( run == KRun16 )
		{
		ALLPRINT(KTest16);
		FullTransactionPerformanceTestL( EFalse);
		}
	else if( run == KRun17 )
		{
		ALLPRINT(KTest17);
		FullTransactionPerformanceTestL( ETrue);
		}
	else if( run == KRun18 )
		{
		ALLPRINT(KTest18);
		MultipleCommitTestL( EFalse);
		}
	else if( run == KRun19 )
		{
		ALLPRINT(KTest19);
		MultipleCommitTestL( ETrue);
		}
	else
		{
		MissingTestPanic();
		}

	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
if in revert mode rollback transaction, otherwise commit
*/
void CTransactionsStep::EndTransactionL(const TBool aRevert)
	{
	if(aRevert)
		{
		iContactsDatabase->DatabaseRollback();
		CleanupStack::Pop();
		if( iContactsDatabase->IsDamaged() )
			{
			iContactsDatabase->RecoverL();
			}
		}
	else
		{
		iContactsDatabase->DatabaseCommitLP(EFalse);
		}
	}

void CTransactionsStep::NestedTransactionTestL()
	{
	TContactItemId cid = iIterate->NextL();
	
	iContactsDatabase->DatabaseBeginLC(EFalse);
	iContactsDatabase->DatabaseBeginLC(ETrue);
	OpenL(cid);
	SetFieldL(1, GetFieldL(KStringFields, KField, 1) );
	CommitL(ETrue);
	iContactsDatabase->DatabaseCommitLP(ETrue);
	OpenL(cid);
	SetFieldL(2, GetFieldL(KStringFields, KField, 2) );
	CommitL(ETrue);	
	EndTransactionL(ETrue);
	
	iIterate->Reset();
	TInt err = KErrNone;
	TInt k = 0;
	TESTPRINT( iContactsDatabase->CountL() == iContacts );
	
	for( k = 0; k < iContacts; ++k)
		{
		TRAP(err, ReadL( iIterate->NextL() ) );
		TESTPRINTI( KErrNone == err, k );
		TESTPRINTI( CheckAllFieldsL(KNullDesC), k );
		CloseL( ETrue );
		}	
	}

void CTransactionsStep::EmptyCommitTransactionTestL(const TBool aRevert)
	{
	CPerformanceFunctionalityBase::InitializeL();
	
	iContactsDatabase->DatabaseBeginLC(EFalse);
	EndTransactionL(aRevert);
	iContactsDatabase->DatabaseBeginLC(EFalse);
	ReadL( iIterate->NextL() );
	EndTransactionL(aRevert);
	CloseL( ETrue );
	
	iIterate->Reset();
	TInt i = 0;
	for(; i < iContacts; ++i)
		{
		ReadL( iIterate->NextL() );
		TESTPRINTI( CheckAllFieldsL(KNullDesC), i );
		CloseL( ETrue );
		}
			
	}
	
void CTransactionsStep::UpdateCommitTransactionTestL(const TBool aRevert)
	{
	CPerformanceFunctionalityBase::InitializeL();
	
	const TContactItemId cid = iIterate->NextL();
	const TContactItemId cid2 = iIterate->NextL();
	
	iContactsDatabase->DatabaseBeginLC(EFalse);
	OpenL( cid );
	SetAllFieldsL( GetNextFieldL(KStringFields, KField, ETrue) );
	CommitL(ETrue);
	EndTransactionL(aRevert);

	iContactsDatabase->DatabaseBeginLC(EFalse);
	ReadL( cid2 );
	SetAllFieldsL( GetNextFieldL(KStringFields, KField, EFalse) );
	iContactsDatabase->UpdateContactLC(cid2, iContactItem);
	CleanupStack::PopAndDestroy();
	CloseL( ETrue );
	EndTransactionL(aRevert);
	
	if(aRevert)
		{
		iIterate->Reset();
		TInt i = 0;
		for(; i < iContacts; ++i)
			{
			ReadL( iIterate->NextL() );
			TESTPRINTI( CheckAllFieldsL(KNullDesC), i );
			CloseL( ETrue );
			}
		}
	else
		{
		ReadL( cid );
		TESTPRINT( CheckAllFieldsL( GetNextFieldL(KStringFields, KField, ETrue) ) );
		CloseL( ETrue );
		
		ReadL( cid2 );
		TESTPRINT( CheckAllFieldsL( GetNextFieldL(KStringFields, KField, EFalse) ) );
		CloseL( ETrue );
		}
	}
	
void CTransactionsStep::CreateCommitTransactionTestL(const TBool aRevert)
	{
	iContacts = 0;
	CPerformanceFunctionalityBase::InitializeL();
	
	CContactItemFieldSet& templateFields = iTemplate->CardFields();
	TInt fieldsCount = templateFields.Count() - 1;
	
	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	iContacts = 2;
	TInt k = 0;
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	for(; k < iContacts; k++)
		{		
		iIterate->AddL( iContactsDatabase->doAddNewContactL(*contact,EFalse,ETrue) );
		}	
	CleanupStack::PopAndDestroy(contact);
	
	EndTransactionL(aRevert);
	
	if(aRevert)
		{
		iIterate->Reset();
		TInt err = KErrNone;
		TESTPRINT( iContactsDatabase->CountL() == 0 );
		
		for( k = 0; k < iContacts; ++k)
			{
			TRAP(err, ReadL( iIterate->NextL() ) );
			TESTPRINTI( KErrNotFound == err, k );
			CloseL( ETrue );
			}
		}
	else
		{
		iIterate->Reset();
		TESTPRINT( ( iContactsDatabase->CountL() == iContacts ) );
		
		for(k = 0; k < iContacts; ++k)
			{
			ReadL( iIterate->NextL() );
			TESTPRINTI( iFields->Count() == fieldsCount, k );
			TESTPRINTI( CheckAllFieldsL(KNullDesC), k );
			CloseL( ETrue );
			}
		}
	}	
	
void CTransactionsStep::DeleteCommitTransactionTestL(const TBool aRevert)
	{
	iContacts = 10;
	CPerformanceFunctionalityBase::InitializeL();
	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	TInt k = 0;
	for(; k < iContacts; k++)
		{		
		iContactsDatabase->doDeleteContactL(iIterate->NextL(), ETrue, ETrue);
		}
	
	EndTransactionL(aRevert);
	
	if(aRevert)
		{
		TESTPRINT( iContactsDatabase->CountL() == iContacts );
		iIterate->Reset();
		TInt err = KErrNone;
		for(k = 0; k < iContacts; ++k)
			{
			TRAP(err, ReadL( iIterate->NextL() ) );
			TESTPRINTI( KErrNone == err, k );
			TESTPRINTI( CheckAllFieldsL( KNullDesC ), k );
			CloseL( ETrue );
			}
		}
	else
		{
		TESTPRINT( iContactsDatabase->CountL() == 0 );
		
		iIterate->Reset();
		TInt err = KErrNone;
		for(k = 0; k < iContacts; ++k)
			{
			TRAP(err, ReadL( iIterate->NextL() ) );
			TESTPRINTI( KErrNotFound == err, k );
			CloseL( ETrue );
			}
		iIterate->Clear();
		}
	}
	
void CTransactionsStep::ManyUpdateCommitTransactionTestL( const TBool aRevert )
	{
	iContacts = 1000;
	CPerformanceFunctionalityBase::InitializeL();
	RCntList backuplist;
	backuplist.CopyL( *iIterate );
	iContactsDatabase->DatabaseBeginLC(EFalse);
	
	TInt added = 0;
	TInt deleted = 0;
	TInt updated = 0;
	TInt k = 0;
	for(; k < iContacts; k++)
		{
		switch( k % KNumCUDOperations )
			{
			case ( EOpen ):
				{
				OpenL( iIterate->NextL() );
				//if setting first contact read first field from stringfields section
				//otherwise read next field in list of fields stored in ini file secion
				SetAllFieldsL( GetNextFieldL(KStringFields, KField, 0 == k ) );
				CommitL(ETrue);
				++updated;
				}
			break;
			case ( EUpdate ):
				{
				TContactItemId cid = iIterate->NextL();
				ReadL( cid );
				//if setting first contact read first field from stringfields section
				//otherwise read next field in list of fields stored in ini file secion
				SetAllFieldsL( GetNextFieldL(KStringFields, KField, 0 == k ) );
				iContactsDatabase->UpdateContactLC(cid, iContactItem);
				CleanupStack::PopAndDestroy();
				CloseL( ETrue );
				++updated;
				}
			break;
			case ( EAddDelete ):
				{
				DeleteContactL( iIterate->NextL(), ETrue );
				//iterator position is shifted back, so that next nextL call is correct
				iIterate->PreviousL();
				++deleted;
				//Empty contact is appended to the database, 
				//Contact id is stored at the end of iterator list not at current iterator position
				AddEmptyContactsL( 1, ETrue );
				++added;
				}
			break;
			default:
				{
				_LIT(KInvalidTest, "Invalid Case");
				User::Panic(KInvalidTest, 555);
				}
			break;
			}
		}
	
	EndTransactionL(aRevert);
	TESTPRINT( iContactsDatabase->CountL() == iContacts );
	
	CContactItemFieldSet& templateFields = iTemplate->CardFields();
	TInt fieldsCount = templateFields.Count() - 1;
	
	if(aRevert)
		{
		//if transaction is reverted / rolledback, all contacts should be
		//unchanged, i.e. they should all contain empty fields only
		backuplist.Reset();
		TInt err = KErrNone;
		for(k = 0; k < iContacts; ++k)
			{
			TRAP(err, ReadL( backuplist.NextL() ) );
			TESTPRINTI( KErrNone == err, k );
			TESTPRINTI( CheckAllFieldsL( KNullDesC ), k );
			CloseL( ETrue );
			}
		}
	else
		{
		iIterate->Reset();
		//all updated contact should contain the appropriated updated field value
		for(k = 0; k < updated; ++k)
			{
			ReadL( iIterate->NextL() );
			TESTPRINTI(iFields->Count() == fieldsCount, k);
			//if reading first contact read first field from stringfields section
			//otherwise read next field in list of fields stored in ini file secion
			TESTPRINTI( CheckAllFieldsL( GetNextFieldL(KStringFields, KField, 0 == k ) ), k );
			CloseL( ETrue );
			}
			
		//all newly	added contacts, should contain empty fields only.
		for(k = 0; k < added; ++k)
			{
			ReadL( iIterate->NextL() );
			TESTPRINTI( iFields->Count() == fieldsCount, k);
			TESTPRINTI( CheckAllFieldsL( KNullDesC ), k);
			CloseL( ETrue );
			}
		}
	}

void CTransactionsStep::TransactionPerformanceL(	const TBool aDelete, 
													const FieldFullness aField,
													const TInt aContacts,
													TReal &aDiffseconds,
													TReal &aCumseconds)
	{
	iContacts = ( ( aContacts / KTransactionSize ) + 1 ) * KTransactionSize ;
	TInt64 cumlative = 0;
	TInt64 diff = 0;
	TTime startT;
	TTime finishT;
	TTime start;
	TTime finish;

	TInt j = 0;
	for(; j < KPerfIter; ++j)
		{
		CPerformanceFunctionalityBase::InitializeL();
		ModifyContactsL(aField);
		ExportVcardL( aDelete, KVcardFile );
		RFileReadStream rfrs;
		CleanupClosePushL( rfrs );
		User::LeaveIfError( rfrs.Open( iParent->Fs(), KVcardFile, EFileRead | EFileShareAny ) );
		
		TInt i = 0;
		start.UniversalTime();
		for(; i < iContacts; i+=KTransactionSize)
			{
			iContactsDatabase->DatabaseBeginLC(EFalse);
			ImportVcardL(KTransactionSize, rfrs);
			startT.UniversalTime();
			iContactsDatabase->DatabaseCommitLP(EFalse);
			finishT.UniversalTime();
			cumlative += ( finishT.MicroSecondsFrom( startT ).Int64() );
			}
		finish.UniversalTime();
		diff += ( finish.MicroSecondsFrom(start).Int64() );
		CleanupStack::PopAndDestroy(&rfrs);
		}
	User::LeaveIfError( iParent->Fs().Delete(KVcardFile) );
	
	aDiffseconds = (TReal) diff / (1000000 * KPerfIter);
	aCumseconds = (TReal) cumlative / (1000000 * KPerfIter);
	
	_LIT(KMicroPerformance, "Micro performance: %d");
	_LIT(KSecondsPerformance, "performance in seconds: %f");
	_LIT(KCommitPerformance, "commit performance in seconds: %f");
	ALLPRINT2(KMicroPerformance, diff); 
	ALLPRINT2(KSecondsPerformance, aDiffseconds ); 
	ALLPRINT2(KCommitPerformance, aCumseconds ); 
	}

void CTransactionsStep::EmptyTransactionPerformanceTestL(const TBool aDelete)
	{
	TReal diffsecondsMax = 0;
	TReal commitsecondsMax = 0;
	
	_LIT(KMaxContactsPrint,"$Max contacts$");
	ALLPRINT(KMaxContactsPrint); 
	TransactionPerformanceL(aDelete, EEmpty, KMaxContacts, diffsecondsMax, commitsecondsMax);
	
	TESTPRINT( diffsecondsMax < KMaxTimeAllowed );
	}

void CTransactionsStep::MediumTransactionPerformanceTestL(const TBool aDelete)
	{
	TReal diffsecondsMin = 0;
	TReal commitsecondsMin = 0;
	TReal diffsecondsMid = 0;
	TReal commitsecondsMid = 0;
	TReal diffsecondsMan = 0;
	TReal commitsecondsMan = 0;
	TReal diffsecondsMax = 0;
	TReal commitsecondsMax = 0;
		
	ALLPRINT(KMinContactsPrint); 
	TransactionPerformanceL(aDelete, EMedium, KMinContacts, diffsecondsMin, commitsecondsMin);
	ALLPRINT(KMidContactsPrint); 
	TransactionPerformanceL(aDelete, EMedium, KMidContacts, diffsecondsMid, commitsecondsMid);
	ALLPRINT(KManyContactsPrint); 
	TransactionPerformanceL(aDelete, EMedium, KManyContacts, diffsecondsMan, commitsecondsMan);
	ALLPRINT(KMaxContactsPrint); 
	TransactionPerformanceL(aDelete, EMedium, KMaxContacts, diffsecondsMax, commitsecondsMax);


	TESTPRINT( diffsecondsMax < KMaxTimeAllowed );
	}

void CTransactionsStep::FullTransactionPerformanceTestL(const TBool aDelete)
	{
	TReal diffsecondsMin = 0;
	TReal commitsecondsMin = 0;
	TReal diffsecondsMid = 0;
	TReal commitsecondsMid = 0;
	TReal diffsecondsMan = 0;
	TReal commitsecondsMan = 0;
	TReal diffsecondsMax = 0;
#ifdef __WINSCW__
	TReal commitsecondsMax = 0;
#endif
	
	ALLPRINT(KMinContactsPrint); 
	TransactionPerformanceL(aDelete, EFull, KMinContacts, diffsecondsMin, commitsecondsMin);
	ALLPRINT(KMidContactsPrint); 
	TransactionPerformanceL(aDelete, EFull, KMidContacts, diffsecondsMid, commitsecondsMid);
	ALLPRINT(KManyContactsPrint); 
	TransactionPerformanceL(aDelete, EFull, KManyContacts, diffsecondsMan, commitsecondsMan);
	//is too slow on hardware, also takes up too much disk space
	#ifdef __WINSCW__
	ALLPRINT(KMaxContactsPrint); 
	TransactionPerformanceL(aDelete, EFull, KMaxContacts, diffsecondsMax, commitsecondsMax);
	#endif
	
	TESTPRINT( diffsecondsMax < KMaxTimeAllowed );
	}

void CTransactionsStep::MultipleCommitTestL(const TBool aTrans)
	{
	iContacts = 40;
	CPerformanceFunctionalityBase::InitializeL();
	if(aTrans)
		{
		iContactsDatabase->DatabaseBeginLC(EFalse);
		}
	MultipleCommitL(aTrans);
	if(aTrans)
		{
		EndTransactionL(EFalse);
		}
	TInt k = 0;
	iIterate->Reset();
	for(; k < iContacts; ++k)
		{
		ReadL( iIterate->NextL() );
		TESTPRINTI( CheckAllFieldsL( GetNextFieldL(KStringFields, KField, 0 == k ) ), k );
		CloseL( ETrue );
		}
	}

/**
destroy array from within cleanupstack
*/
static void ArrayCleanup(TAny *aArray)
	{
	RPointerArray<CContactItem> *parray = static_cast< RPointerArray<CContactItem> * >(aArray);
	parray->ResetAndDestroy();
	}

void CTransactionsStep::MultipleCommitL(const TBool aTrans)
	{
	RPointerArray<CContactItem> itemarray( 500, 512 );
	TCleanupItem tclean(ArrayCleanup,&itemarray);
	CleanupStack::PushL( tclean );
	TInt k = 0;
	iIterate->Reset();
	for(; k < iContacts; ++k)
		{
		itemarray.Append( iContactsDatabase->OpenContactLX( iIterate->NextL(), *iViewAll ) );
		iFields = &( itemarray[k]->CardFields() );
		SetAllFieldsL( GetNextFieldL(KStringFields, KField, 0 == k ) );
		}

	iIterate->Reset();
	if(aTrans)
		{
		for( k = iContacts - 1; k >=0 ; --k )
			{
			iContactsDatabase->doCommitContactL(*itemarray[k], ETrue, ETrue );
			CleanupStack::Pop();
			}
		}
	else
		{
		for( k = iContacts - 1; k >=0 ; --k )
			{
			iContactsDatabase->CommitContactL(*itemarray[k]);
			CleanupStack::Pop();
			}
		}
	CleanupStack::PopAndDestroy();//tclean
	}

/**
populates all contacts either moderately, fully, or not at all
*/
void CTransactionsStep::ModifyContactsL(const FieldFullness aField)
	{
	if(aField == EEmpty)
		{
		return;
		}
	iIterate->Reset();
	TInt j = 0;
	for(; j < iContacts; ++j)
		{
		OpenL(iIterate->NextL());
		if(aField == EFull)
			{
			SetAllFieldsL( GetFieldL(KStringFields, KField, 2) );
			}
		else
			{
			SetManyFieldsL(GetFieldL(KStringFields, KField, 4) , KMediumFields);
			}
		CommitL(EFalse);
		}
	}

