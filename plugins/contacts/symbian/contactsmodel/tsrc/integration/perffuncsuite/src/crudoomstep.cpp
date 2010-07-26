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
* These are all out of memory tests.
* Call api first in order that all lazy initializations take place
* check heap balance is maintained for client side tests
* make sure all failures result only in KErrNoMemory only.
*
*/


/**
 @file 
 @publishedAll
 @released
*/

#include "crudoomstep.h"
#include "performancefunctionalitydefs.h"

_LIT(KRun1,"Read");
_LIT(KRun2,"Open");
_LIT(KRun3,"Delete");
_LIT(KRun4,"Add");
_LIT(KRun5,"Close");
_LIT(KRun6,"Commit");
_LIT(KRun7,"Update");
_LIT(KRun8,"ViewsRemote");
_LIT(KRun9,"ViewsLocal");
_LIT(KRun10,"ViewsLocalFS");

_LIT(KTest1, "Read test");
_LIT(KTest2, "Open test");
_LIT(KTest3, "Delete test");
_LIT(KTest4, "Add contact test");
_LIT(KTest5, "Close contact test");
_LIT(KTest6, "Commit contact test");
_LIT(KTest7, "Update contact test");
_LIT(KTest8, "Remote View test");
_LIT(KTest9, "Local View test");

_LIT(KView, "Views");

_LIT(KStars,"***************************");

const static TInt KNumberOfContacts = 10;

const static TInt32 KExistingUidsArray[] = 
	{
	KUidContactFieldAddressValue,
	KUidContactFieldPostOfficeValue,
	KUidContactFieldExtendedAddressValue,
	KUidContactFieldLocalityValue,
	KUidContactFieldRegionValue,
	KUidContactFieldPostCodeValue,
	KUidContactFieldCountryValue,
	KUidContactFieldCompanyNameValue,
	KUidContactFieldCompanyNamePronunciationValue,
	KUidContactFieldPhoneNumberValue,
	KUidContactFieldGivenNameValue,
	KUidContactFieldFamilyNameValue,
	KUidContactFieldGivenNamePronunciationValue,
	KUidContactFieldFamilyNamePronunciationValue,
	KUidContactFieldAdditionalNameValue,
	KUidContactFieldSuffixNameValue,
	KUidContactFieldPrefixNameValue,
	KUidContactFieldHiddenValue,
	KUidContactFieldEMailValue,
	KUidContactFieldMsgValue,
	KUidContactFieldSmsValue,
	KUidContactFieldFaxValue,
	KUidContactFieldDefinedTextValue,
	KUidContactFieldNoteValue,
	KUidContactFieldBirthdayValue,
	KUidContactFieldUrlValue,
	KUidContactFieldStorageInlineValue,
	KUidContactFieldTemplateLabelValue,
	KUidContactFieldPictureValue,
	KUidContactFieldRingToneValue,
	KUidContactFieldDTMFValue,
	KUidContactsVoiceDialFieldValue,
	KUidContactFieldNoneValue,
	KUidContactFieldJobTitleValue,
	KUidContactFieldICCSlotValue,
	KUidContactFieldICCPhonebookValue,
	KUidContactFieldICCGroupValue,
	KUidContactFieldIMAddressValue,
	KUidContactFieldSecondNameValue,
	KUidContactFieldSIPIDValue,
	KIntContactFieldVCardMapWORK,
	KIntContactFieldVCardMapHOME,
	KIntContactFieldVCardMapMSG,
	KIntContactFieldVCardMapVOICE,
	KIntContactFieldVCardMapFAX,
	KIntContactFieldVCardMapPREF,
	KIntContactFieldVCardMapCELL,
	KIntContactFieldVCardMapPAGER,
	KIntContactFieldVCardMapBBS,
	KIntContactFieldVCardMapMODEM,
	KIntContactFieldVCardMapCAR,
	KIntContactFieldVCardMapISDN,
	KIntContactFieldVCardMapVIDEO,
	KIntContactFieldVCardMapDOM,
	KIntContactFieldVCardMapADR,
	KIntContactFieldVCardMapPOSTOFFICE,
	KIntContactFieldVCardMapEXTENDEDADR,
	KIntContactFieldVCardMapLOCALITY,
	KIntContactFieldVCardMapREGION,
	KIntContactFieldVCardMapPOSTCODE,
	KIntContactFieldVCardMapCOUNTRY,
	KIntContactFieldVCardMapAGENT,
	KIntContactFieldVCardMapBDAY,
	KIntContactFieldVCardMapEMAILINTERNET,
	KIntContactFieldVCardMapGEO,
	KIntContactFieldVCardMapLABEL,
	KIntContactFieldVCardMapLOGO,
	KIntContactFieldVCardMapMAILER,
	KIntContactFieldVCardMapNOTE,
	KIntContactFieldVCardMapORG,
	KIntContactFieldVCardMapORGPronunciation,
	KIntContactFieldVCardMapPHOTO,
	KIntContactFieldVCardMapROLE,
	KIntContactFieldVCardMapSOUND,
	KIntContactFieldVCardMapTEL,
	KIntContactFieldVCardMapTELFAX,
	KIntContactFieldVCardMapTITLE,
	KIntContactFieldVCardMapURL,
	KIntContactFieldVCardMapUnusedN,
	KIntContactFieldVCardMapUnusedFN,
	KIntContactFieldVCardMapNotRequired,
	KIntContactFieldVCardMapUnknownXDash,
	KIntContactFieldVCardMapUnknown,
	KIntContactFieldVCardMapUID,
	KIntContactFieldVCardMapINTL,
	KIntContactFieldVCardMapPOSTAL,
	KIntContactFieldVCardMapPARCEL,
	KIntContactFieldVCardMapGIF,
	KIntContactFieldVCardMapCGM,
	KIntContactFieldVCardMapWMF,
	KIntContactFieldVCardMapBMP,
	KIntContactFieldVCardMapMET,
	KIntContactFieldVCardMapPMB,
	KIntContactFieldVCardMapDIB,
	KIntContactFieldVCardMapPICT,
	KIntContactFieldVCardMapTIFF,
	KIntContactFieldVCardMapPDF,
	KIntContactFieldVCardMapPS,
	KIntContactFieldVCardMapJPEG,
	KIntContactFieldVCardMapMPEG,
	KIntContactFieldVCardMapMPEG2,
	KIntContactFieldVCardMapAVI,
	KIntContactFieldVCardMapQTIME,
	KIntContactFieldVCardMapTZ,
	KIntContactFieldVCardMapKEY,
	KIntContactFieldVCardMapX509,
	KIntContactFieldVCardMapPGP,
	KIntContactFieldVCardMapSMIME,
	KIntContactFieldVCardMapWV,
	KIntContactFieldVCardMapSECONDNAME,
	KIntContactFieldVCardMapSIPID,
	KIntContactFieldVCardMapPOC,
	KIntContactFieldVCardMapSWIS,
	KIntContactFieldVCardMapVOIP
	};

	
CCRUDOOMStep::CCRUDOOMStep() 
		: CPerformanceFunctionalityBase(KNumberOfContacts)
	{
	SetTestStepName(KCRUDOOMStep);
	}

void CCRUDOOMStep::PreTestL()
	{
	const TDesC &run = ConfigSection();//for views tests only use one contact, otherwise tests too slow
	if( run.Find(KView) == 0 )
		{
		iContacts = 1;
		}
	}

void CCRUDOOMStep::InitializeL()
	{
	CPerformanceFunctionalityBase::InitializeL();
	iDestroyView = new (ELeave) CDestroyView( this );
	CActiveScheduler::Add(iDestroyView);
	}

void CCRUDOOMStep::Cleanup()
	{
	CLEAR( iDestroyView );
	CPerformanceFunctionalityBase::Cleanup();
	}

TVerdict CCRUDOOMStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KDostepPrint,"CCRUDOOMStep::doTestStepL()");
	INFO_PRINTF1(KDostepPrint);
	iIterate->Reset();
	
	const TDesC &run = ConfigSection();
	_LIT(KFullContactOOMTest,"Full contacts OOM testing");
	_LIT(KEmptyOOMTest,"Empty contacts OOM testing");
	
	//perform clientside and serverside testing when contacts are empty
	if( run != KRun10 )
		{
		INFO_PRINTF1(KNullDesC);
		INFO_PRINTF1(KStars);
		INFO_PRINTF1(KEmptyOOMTest);
		INFO_PRINTF1(KStars);
		INFO_PRINTF1(KNullDesC);
		RunAllTestsL(ETrue);
		RunAllTestsL(EFalse);
		}
	
	//perform clientside and serverside testing when contacts are full
	INFO_PRINTF1(KNullDesC);
	INFO_PRINTF1(KStars);
	INFO_PRINTF1(KFullContactOOMTest);
	INFO_PRINTF1(KStars);
	INFO_PRINTF1(KNullDesC);
	iIterate->Reset();
	
	TInt j = 0;
	for(; j < iContacts; ++j)
		{
		OpenL(iIterate->NextL());
		SetAllFieldsL( GetFieldL(KStringFields, KField, 2) );
		CommitL(EFalse);
		}
	iIterate->Reset();	
	if( run != KRun10 )
		{
		RunAllTestsL(ETrue);
		}
	if( run != KRun9 ) //with full contacts, localview serverside oom test is too slow, run as sperate step
		{
		RunAllTestsL(EFalse);
		}
	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}
/**
If aClient is true perform client side OOM testing 
Otherwise perform Serverside Heap testing
*/
void CCRUDOOMStep::RunAllTestsL(const TBool aClient)
	{
	if(aClient)
		{
		_LIT(KClientOOM,"Clientside OOM testing");
		INFO_PRINTF1(KClientOOM);
		INFO_PRINTF1(KNullDesC);
		}
	else
		{
		INFO_PRINTF1(KNullDesC);
		_LIT(KServerOOM,"Serverside OOM testing");
		INFO_PRINTF1(KServerOOM);
		INFO_PRINTF1(KNullDesC);
		}
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		ReadOOML(aClient);
		}
	else if( run == KRun2 )
		{
		OpenOOML(aClient);
		}
	else if( run == KRun3 )
		{
		DeleteOOML(aClient);
		}
	else if( run == KRun4 )
		{
		AddContactOOML(aClient);
		}
	else if( run == KRun5 )
		{	
		CloseContactOOML(aClient);
		}
	else if( run == KRun6 )
		{
		CommitContactOOML(aClient);
		}
	else if( run == KRun7 )
		{
		UpdateContactOOML(aClient);
		}
	else if( run == KRun8 )
		{
		INFO_PRINTF1(KTest8);
		ViewOOMTestL(ETrue, aClient);
		}
	else if( run == KRun9 || run == KRun10 )
		{
		INFO_PRINTF1(KTest9);
		ViewOOMTestL(EFalse, aClient);
		}
	else
		{
		MissingTestPanic();
		}
	}
/**
set heapmark and set heap failure to aFailAt
performs testing on client or serverside
*/
void CCRUDOOMStep::StartHeapCheck(const TInt aFailAt, const TBool aClient)
	{
	if( aClient)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, aFailAt); // leavescan thinks this macro is a leaver but it isn't
		__UHEAP_MARK;
		}
	else
		{
		iContactsDatabase->SetCntServerHeapFailure(RHeap::EDeterministic, aFailAt); //__UHEAP_SETFAIL
		}
	}


/**
reset heapmark and reset heap failure
check that error receved from api failiure equals KErrNoMemory or there was no failiure
performs checking on client or serverside
*/
TBool CCRUDOOMStep::EndHeapCheckL(const TInt aErr, const TBool aClient)
	{	
	TBool ret = EFalse;
	if( aClient)
		{
		__UHEAP_RESET;
		RecoverL(aErr);
		__UHEAP_MARKEND;
		}
	else
		{
		iContactsDatabase->SetCntServerHeapFailure(RHeap::ENone, 1);//__UHEAP_RESET
		RecoverL(aErr);
		}
	TESTPRINT( ret = ( (KErrNoMemory == aErr) || (KErrNone == aErr) ) );
	return ret;
	}

/**
check value of received error and print relevant error message as necessary
*/
void CCRUDOOMStep::ErrorProcess(const TInt aFailAt, const TInt aErr, 
								const TBool aClient, const TBool aCleanup)
	{
	_LIT(KNonMemoryError,"iteration %d failed - non-memory error in OOM test: %d");
	_LIT(KCleanupError,"iteration %d failed - cleanup error in OOM test");
	if( aClient )
		{
		if( (KErrNoMemory != aErr) && (KErrNone != aErr)  )
			{
			ERR_PRINTF3(KNonMemoryError, aFailAt, aErr);
			}
		else
			{
			ERR_PRINTF2(KCleanupError, aFailAt);
			}
		}
	else
		{
		if( (KErrNoMemory == aErr) || (KErrNone == aErr) )
			{
			if( aCleanup )
				{
				ERR_PRINTF2(KCleanupError, aFailAt);
				}
			else
				{
				_LIT(KHeapError,"iteration %d failed - server-heap imbalance error in OOM test");
				ERR_PRINTF2(KHeapError, aFailAt);
				}
			}
		else
			{
			ERR_PRINTF3(KNonMemoryError, aFailAt, aErr);
			}
		}
	}

/**
recover database if it is damaged
*/
void CCRUDOOMStep::RecoverL(const TInt aErr)
	{
	if( aErr != KErrNone )
		{
		if( iContactsDatabase->IsDamaged() )
			{
			iContactsDatabase->RecoverL();
			}
		}
	}

/**
perform oom testing for read contact
*/
void CCRUDOOMStep::ReadOOML(const TBool aClient)
	{
	INFO_PRINTF1(KTest1);
	TBool ErrorCheckTest = EFalse;
	TContactItemId cid = iIterate->NextL();
	CContactItem *contactItem = NULL;
	
	//any lazy initialization should be performed here, so that it doesnt interfere with oom testing
	ReadL( cid );
	CloseL( EFalse );
	iContactsDatabase->CompactL();
	
	TInt err = KErrNone;
	TInt failAt = 0;
	do
		{
		++failAt;
		//set heap failure on client/server
		StartHeapCheck(failAt, aClient);
		
		//perform read	
		TRAP(err, contactItem = iContactsDatabase->ReadContactL( cid, *iViewAll ); );
		
		if (err == KErrNone)
			{
			CLEAR(contactItem);	
			}
		
		//reset heap failure on client/server, check that there is no memory leak on client
		TESTPRINTI( ErrorCheckTest = EndHeapCheckL(err, aClient), failAt);
		
		if(!ErrorCheckTest)
			{
			//if test failed print appropriate message
			ErrorProcess(failAt, err, aClient, EFalse);
			break;
			}
		}
	while(err != KErrNone);
	}


/**
perform oom testing for open contact
*/	
void CCRUDOOMStep::OpenOOML(const TBool aClient)
	{
	INFO_PRINTF1(KTest2);
	TBool ErrorCheckTest = EFalse;
	TContactItemId cid = iIterate->NextL();
	CContactItem *contactItem = NULL;
	
	//any lazy initialization should be performed here, so that it doesnt interfere with oom testing
	OpenL( cid );
	CloseL( ETrue );
	iContactsDatabase->CompactL();
	
	TInt err = KErrNone;
	TInt failAt = 0;
	do
		{
		++failAt;
		//set heap failure on client/server
		StartHeapCheck(failAt, aClient);
		
		//perform open contact				
		TRAP(err, contactItem = iContactsDatabase->OpenContactL( cid, *iViewAll ); );
		
		if (err == KErrNone)
			{
			CLEAR(contactItem);	
			}
		
		
		//reset heap failure on client/server, check that there is no memory leak on client	
		TESTPRINTI( ErrorCheckTest = EndHeapCheckL(err, aClient), failAt );
		
		if(!ErrorCheckTest)
			{
			//if test failed print appropriate message
			ErrorProcess(failAt, err, aClient, EFalse);
			iIterate->RemoveL(cid);	
			break;
			}
		}
	while(err != KErrNone);
	
	iContactsDatabase->CloseContactL( cid );
	}


/**
perform oom testing for delete contact
*/	
void CCRUDOOMStep::DeleteOOML(const TBool aClient)
	{
	INFO_PRINTF1(KTest3);
	TBool ErrorCheckAllTests 	= EFalse;
	TBool ErrorCheckSingleTest 	= EFalse;
	TBool cleanupError = EFalse;
	TContactItemId cid = iIterate->NextL();
	
	//any lazy initialization should be performed here, so that it doesnt interfere with oom testing
	DeleteContactL( cid, EFalse );
	iContactsDatabase->CompactL();
	
	cid = iIterate->NextL();
		
	TInt err = KErrNone;
	TInt failAt = 0;
	
	const TInt KInitialCount = iContactsDatabase->CountL();
	
	do
		{
		++failAt;
		//set heap failure on client/server
		StartHeapCheck(failAt, aClient);
			
		//perform delete contact	
		TRAP(err, iContactsDatabase->DeleteContactL( cid ); );
			
		//reset heap failure on client/server, check that there is no memory leak on client	
		TESTPRINTI( ErrorCheckSingleTest = EndHeapCheckL(err, aClient), failAt);
		ErrorCheckAllTests = ErrorCheckSingleTest;
		
		if (err == KErrNone)
			{
			//if no error, check that contact was deleted
			iIterate->RemoveL(cid);
			//if initial count -1 doesn't equal number of contact, there is a cleanup error
			cleanupError = ( (KInitialCount - 1) != iContactsDatabase->CountL() );
			TESTPRINTI( ErrorCheckSingleTest = !cleanupError, failAt );	
			ErrorCheckAllTests = ErrorCheckAllTests && ErrorCheckSingleTest;
			}
		else
			{
			//if error, check that contact wasn't deleted
			cleanupError = ( KInitialCount != iContactsDatabase->CountL() );
			TESTPRINTI( ErrorCheckSingleTest = !cleanupError , failAt );
			ErrorCheckAllTests = ErrorCheckAllTests && ErrorCheckSingleTest;
			}
		if(!ErrorCheckAllTests)
			{
			//if checking error, print message, end test
			ErrorProcess(failAt, err, aClient, cleanupError);
			break;
			}
		}
	while(err != KErrNone);
	}


/**
perform oom testing for add contact
*/	
void CCRUDOOMStep::AddContactOOML(const TBool aClient)
	{
	INFO_PRINTF1(KTest4);
	TBool ErrorCheckAllTests 	= EFalse;
	TBool ErrorCheckSingleTest 	= EFalse;
	TBool cleanupError = EFalse;
	TContactItemId cid = 0;
		
	TInt err = KErrNone;
	TInt failAt = 0;
	//any lazy initialization should be performed here, so that it doesnt interfere with oom testing
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	iContactsDatabase->AddNewContactL(*contact);
	iContactsDatabase->CompactL();
	
	const TInt KInitialCount = iContactsDatabase->CountL();
	
	do
		{
		++failAt;
		//set heap failure on client/server
		StartHeapCheck(failAt, aClient);
				
		//perform add contact
		TRAP(err, cid = iContactsDatabase->AddNewContactL(*contact); );
			
		//reset heap failure on client/server, check that there is no memory leak on client	
		TESTPRINTI( ErrorCheckSingleTest = EndHeapCheckL(err, aClient), failAt );
		ErrorCheckAllTests = ErrorCheckSingleTest;

		if (err == KErrNone)
			{
			//if no error, check that contact was added
			iIterate->AddL(cid);
			//if initial count + 1 doesn't equal number of contact, there is a cleanup error
			cleanupError = ( ( KInitialCount + 1 ) != iContactsDatabase->CountL() );
			TESTPRINTI( ErrorCheckSingleTest = !cleanupError, failAt );		
			ErrorCheckAllTests = ErrorCheckAllTests && ErrorCheckSingleTest;
			}
		else
			{
			//if error, check that contact wasn't added
			cleanupError = ( KInitialCount != iContactsDatabase->CountL() );
			TESTPRINTI( ErrorCheckSingleTest = !cleanupError , failAt );	
			ErrorCheckAllTests = ErrorCheckAllTests && ErrorCheckSingleTest;
			}
		if(!ErrorCheckAllTests)
			{
			//if checking error, print message, end test
			ErrorProcess(failAt, err, aClient, cleanupError);
			break;
			}
		}
	while(err != KErrNone);
	
	CleanupStack::PopAndDestroy(contact);	
	}


/**
perform oom testing for close contact
*/	
void CCRUDOOMStep::CloseContactOOML(const TBool aClient)
	{
	INFO_PRINTF1(KTest5);
	TBool ErrorCheckAllTests 	= EFalse;
	TBool ErrorCheckSingleTest 	= EFalse;
	TContactItemId cid = iIterate->NextL();
	CContactItem *contactItem = NULL;
	
	//any lazy initialization should be performed here, so that it doesnt interfere with oom testing
	OpenL( cid );
	CloseL( ETrue );
	iContactsDatabase->CompactL();
	
	contactItem = iContactsDatabase->OpenContactL( cid, *iViewAll );
	CleanupStack::PushL(contactItem);
	TInt err = KErrNone;
	TInt failAt = 0;
	do
		{
		++failAt;
		//set heap failure on client/server
		StartHeapCheck(failAt, aClient);
			
		//perform close contact
		TRAP(err, iContactsDatabase->CloseContactL( cid )  );
			
		//reset heap failure on client/server, check that there is no memory leak on client	
		TESTPRINTI( ErrorCheckSingleTest = EndHeapCheckL(err, aClient), failAt );
		ErrorCheckAllTests = ErrorCheckSingleTest;
		
		if ( KErrNone != err )
			{
			//if close successful and contact still locked there is a cleanup error
			TRAP(err, 	CContactItem *contact = iContactsDatabase->OpenContactL( cid, *iViewAll );
						CLEAR(contact); 
				);
			TESTPRINTI( ErrorCheckSingleTest = (KErrInUse == err), failAt );
			ErrorCheckAllTests = ErrorCheckAllTests && ErrorCheckSingleTest;
			}
		
		if( !ErrorCheckAllTests )
			{
			//if checking error, print message, end test
			ErrorProcess(failAt, err, aClient, EFalse);
			iIterate->RemoveL(cid);
			break;
			}
		}
	while(err != KErrNone);
	
	iContactsDatabase->CloseContactL( cid );
	CleanupStack::PopAndDestroy(contactItem);
	contactItem = NULL;
	}


/**
perform oom testing for commit contact
*/	
void CCRUDOOMStep::CommitContactOOML(const TBool aClient)
	{
	INFO_PRINTF1(KTest6);
	TBool ErrorCheckAllTests 	= EFalse;
	TBool ErrorCheckSingleTest 	= EFalse;
	TContactItemId cid = iIterate->NextL();
	CContactItem *contactItem = NULL;
	
	//any lazy initialization should be performed here, so that it doesnt interfere with oom testing
	OpenL( cid );
	CommitL(EFalse);
	iContactsDatabase->CompactL();//lazy initialization
	
	TInt err = KErrNone;
	TInt failAt = 0;
	do
		{
		++failAt;
		
		// test set-up (pre-condition)
		// need to open the Contact in case Commit succeeds up to a check that the Contact is locked
		contactItem = iContactsDatabase->OpenContactL( cid, *iViewAll );
	
		iContactItem = contactItem;
		iFields = &(iContactItem->CardFields());
		SetFieldL(1, GetFieldL(KStringFields, KField, 1) );
		iContactItem = NULL;
		iFields = NULL;
		CleanupStack::PushL(contactItem);			
		
		//set heap failure on client/server
		StartHeapCheck(failAt, aClient);
			
		//perform commit contact
		TRAP(err, iContactsDatabase->CommitContactL( *contactItem )  );
			
		//reset heap failure on client/server, check that there is no memory leak on client	
		TESTPRINTI( ErrorCheckSingleTest = EndHeapCheckL(err, aClient), failAt );
		ErrorCheckAllTests = ErrorCheckSingleTest;
		
		if( KErrNone != err )
			{
			//if commit successful and contact not fully updated there is a cleanup error
			ReadL( cid );
			TESTPRINTI( ErrorCheckSingleTest = !CompareFieldL(1, GetFieldL(KStringFields, KField, 1) ), failAt );
			ErrorCheckAllTests = ErrorCheckAllTests && ErrorCheckSingleTest;
			CloseL( EFalse );
			}
		if(!ErrorCheckAllTests)
			{
			//if checking error, print message, end test
			ErrorProcess(failAt, err, aClient, EFalse);
			iIterate->RemoveL(cid);
			break;
			}
			
			
		// test set-up (post-condition)
		// Close the Contact so it can be re-Opened in the loop above
		TRAP_IGNORE(iContactsDatabase->CloseContactL(cid));	
		CleanupStack::PopAndDestroy(contactItem);	
		}
	while(err != KErrNone);
	}


/**
perform oom testing for update contact
*/	
void CCRUDOOMStep::UpdateContactOOML(const TBool aClient)
	{
	INFO_PRINTF1(KTest7);
	TBool ErrorCheckAllTests 	= EFalse;
	TBool ErrorCheckSingleTest 	= EFalse;
	TContactItemId cid = iIterate->NextL();
	TContactItemId cid2 = iIterate->NextL();
	CContactItem *contactItem = NULL;
	
	//do initial update so that any lazy initialization is performed, so that it doesnt interfere with oom testing
	contactItem = iContactsDatabase->ReadContactL( cid, *iViewAll );
	CleanupStack::PushL(contactItem);
	iContactsDatabase->UpdateContactLC( cid2, contactItem );
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(contactItem);
	contactItem = NULL;
	//end initial update
	iContactsDatabase->CompactL();//lazy initialization
	
	ReadL( cid );
	
	SetFieldL(1, GetFieldL(KStringFields, KField, 1) );
		
	TInt err = KErrNone;
	TInt failAt = 0;
	do
		{
		++failAt;
		//set heap failure on client/server
		StartHeapCheck(failAt, aClient);
			
		//perform update contact
		TRAP(err, 
				iContactsDatabase->UpdateContactLC( cid2, iContactItem );
				CleanupStack::PopAndDestroy();  
			);
			
		//reset heap failure on client/server, check that there is no memory leak on client	
		TESTPRINTI( ErrorCheckSingleTest = EndHeapCheckL(err, aClient), failAt );
		ErrorCheckAllTests = ErrorCheckSingleTest;
		
		if( KErrNone != err )
			{
			CloseL( EFalse );//close read contact
			//if update successful and contact not fully updated there is a cleanup error
			ReadL( cid2 );
			TESTPRINTI( ErrorCheckSingleTest = !CompareFieldL(1, GetFieldL(KStringFields, KField, 1) ), failAt );
			ErrorCheckAllTests = ErrorCheckAllTests && ErrorCheckSingleTest;
			CloseL( EFalse );
			ReadL( cid );
			}
		if( !ErrorCheckAllTests )
			{
			//if checking error, print message, end test
			ErrorProcess(failAt, err, aClient, EFalse);
			break;
			}
		}
	while(err != KErrNone);
	
	CloseL( EFalse );
	}

void CCRUDOOMStep::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& aEvent)
	{
	iEvent = TContactViewEvent( aEvent.iEventType, aEvent.iInt, aEvent.iContactId );
	
	switch( aEvent.iEventType )
		{
		case TContactViewEvent::EUnavailable:
		case TContactViewEvent::ESortError:
		case TContactViewEvent::EServerError:
		case TContactViewEvent::EIndexingError:
			{
			iError = aEvent.iInt;
			if( iError != KErrNoMemory )
				{
				_LIT(KEventPrint,"EventType: %d, Error: %d, ContactId: %d");
				INFO_PRINTF4(KEventPrint, aEvent.iEventType, aEvent.iInt, aEvent.iContactId );
				}
			break;
			}
		default:
			{
			iError = KErrNone;
			break;
			}
		}
	iDestroyView->Activate();
	}

void CCRUDOOMStep::PopulateFullViewL(const TBool aRemote, RContactViewSortOrder& aSortOrder)
	{
	CContactDatabase *cntdb = CContactDatabase::OpenL();
	CleanupStack::PushL(cntdb);
	if(aRemote)
		{
		iView = CContactRemoteView::NewL(	*this, *cntdb, aSortOrder, EContactsOnly);
		}
	else
		{
		iView = CContactLocalView::NewL(	*this, *cntdb, aSortOrder, EContactsOnly);
		}
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy( cntdb );
	}

/**
perform oom testing for remote/local views
*/
void CCRUDOOMStep::ViewOOMTestL(const TBool aRemote, const TBool aClient)
	{
	TBool ErrorCheckAllTests 	= EFalse;
	const TInt length = ( sizeof KExistingUidsArray ) / ( sizeof KExistingUidsArray[0] );
	TInt i = 0;
	RContactViewSortOrder sortOrder = RContactViewSortOrder();
	CleanupClosePushL( sortOrder );
	for(i = 0 ; i < length ; ++i)
		{
		sortOrder.AppendL( TUid::Uid( KExistingUidsArray[i] ) );
		}
	
	//any lazy initialization should be performed here, so that it doesnt interfere with oom testing
	PopulateFullViewL(aRemote, sortOrder);
	
	TInt err = KErrNone;
	TInt failAt = 0;
	do
		{
		++failAt;
		//set heap failure on client/server
		StartHeapCheck(failAt, aClient);
		//reset view population error
		iError = KErrNone;	
		
		//create view and wait to populate
		TRAP(err, 
				PopulateFullViewL(aRemote, sortOrder); 
			);
			
		//reset heap failure on client/server, check that there is no memory leak on client	
		TESTPRINTI( ErrorCheckAllTests = EndHeapCheckL( ( err != KErrNone ) ? err : (err = iError), aClient), i);
		
		if(!ErrorCheckAllTests)
			{
			//if checking error, print message, end test
			ErrorProcess(failAt, err, aClient, EFalse);
			break;
			}
		}
	while(err != KErrNone);	
			
	CleanupStack::PopAndDestroy();//sortorder
	
	CLEAR( iContactsDatabase );
	WaitForServerToExitL();
	iContactsDatabase = CContactDatabase::OpenL();
	}

/**
Active object that destroys view on first run
then stop active scheduler on second run.
this allows any view cleanup operation to take place in the 
active scheduler after the view is closed
*/
void CCRUDOOMStep::CDestroyView::RunL()
	{
	iInitial = !iInitial;
	if( iInitial )
		{
		iTest->iView->Close( *iTest );
		iTest->iView = NULL;
		Activate();
		}
	else
		{
		CActiveScheduler::Stop();
		}
	}

void CCRUDOOMStep::CDestroyView::Activate()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}
	
void CCRUDOOMStep::CDestroyView::DoCancel()
	{
	//nothing to clean up
	}

