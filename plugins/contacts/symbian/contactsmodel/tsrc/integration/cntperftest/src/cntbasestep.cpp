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
#include <testexecutelog.h>
#include <cntfldst.h>
#include "cntbasestep.h"
#include "nbcnttestlib.h"

_LIT(KServerUid, "*[10003a73]*" );


CCntBaseStep::~CCntBaseStep()
	{
	_LIT(KBaseDestructor,"I am in base destructor");
	INFO_PRINTF1(KBaseDestructor);
	}

TVerdict CCntBaseStep::doTestStepPreambleL()
	{
	if(!CActiveScheduler::Current())
		{
		CActiveScheduler* sched = NULL;
		sched = new(ELeave) CActiveScheduler;
		CActiveScheduler::Install(sched);
		}

		
	iViewAll = CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	iViewAll->AddL(KUidContactFieldMatchAll);
	/**
	issue 3.
	need use custom viewdef otherwise there is discrepency between number of fields between empty fields
	and edited fields
	*/
	
	iExistingUidsArray = new(ELeave) RArray<TInt32>();
	iIterate = new(ELeave) RCntList();
	
	GetExistingUIDsL(*iExistingUidsArray);
	
	//call method in derived class
	PreTestL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CCntBaseStep::doTestStepPostambleL()
	{
	_LIT(KPostamblePrint,"I am in base doTestStepPostambleL");
	INFO_PRINTF1(KPostamblePrint);
	Close();
	Cleanup();
	delete iViewAll;

	iExistingUidsArray->Close();
	CLEAR(iExistingUidsArray);
	CLEAR(iIterate);
	
	TRAPD( err, CContactDatabase::DeleteDefaultFileL() );
	if( err )
		{
		_LIT(KDBDeleteError,"Default database couldn't be deleted, error: %d");
		ERR_PRINTF2(KDBDeleteError, err);
		}
		
	WaitForServerToExitL();//waits for server to close to allow for any memory leaks to be detected
	return TestStepResult();
	}

/**initilize called within __UHEAP_MARK/__UHEAP_MARKEND of doTestStepL, 
so that all relevant memmory de/allocation is in scope*/
void CCntBaseStep::InitializeL()
	{
	iContactsDatabase = CContactDatabase::ReplaceL();//create  clean/fresh database for all tests		

	iTemplate = STATIC_CAST( CContactTemplate*, 
	iContactsDatabase->ReadContactL( iContactsDatabase->TemplateId(), *iViewAll ) );
	
	AddEmptyContactsL(iContacts);
	}
	
/**cleans up called within __UHEAP_MARK/__UHEAP_MARKEND of doTestStepL, 
so that all relevant memmory de/allocation is in scope*/	
void CCntBaseStep::Cleanup()
	{
	delete iTemplate;
	iTemplate = NULL;
	delete iContactsDatabase;
	iContactsDatabase = NULL;
	iIterate->Close();
	}
/*
adds a list of uids to all fields in aFields, uid is incremented from provided seed
*/	
void CCntBaseStep::AddUIDsL(CContactItemFieldSet &aFields, const TInt32 aFieldType) const
	{	
	TInt length = aFields.Count();
	for(TInt i = 0, uids=aFieldType; i<length; i++, uids++)
		{	
		aFields[i].AddFieldTypeL(TFieldType::Uid(uids));
		}
	}
	
/*
check that an extra uid was added to all fields, uid is incremented from provided seed
*/	
	
TBool CCntBaseStep::CheckUIDsL(const CContactItemFieldSet &aFields, const TInt32 aFieldType) const
	{
	TBool ret = EFalse;
	TInt length = aFields.Count();
	const CContentType *ctype = NULL;
	for(TInt i = 0, uids=aFieldType; i<length; i++, uids++)
		{
		ctype = &aFields[i].ContentType();
		// SIP ID will always match the system template field, therefore the added uid will be lost
		if(! ctype->ContainsFieldType(TFieldType::Uid(uids) ) && !ctype->ContainsFieldType(KUidContactFieldSIPID))
			{
			return ret;
			}
		}
	ret=ETrue;
	return ret;
	}

/*sets the label for  a field at position aPos, in aFields to aLabel*/	
void CCntBaseStep::SetLabelL(CContactItemFieldSet &aFields, const TInt aPos, const TDesC& aLabel) const 
	{
	aFields[aPos].SetLabelL(aLabel);
	}
	

/*checks that the label for  a field at position aPos, in aFields equals to aLabel*/	
TBool CCntBaseStep::CompareLabels(const CContactItemFieldSet &aFields, const TInt aPos, const TDesC& aLabel) 
	{
	
	return aFields[aPos].Label() == aLabel;
		
	}
	
/*add a new field empty or otherwise to afields, using the supplied label and uid*/
void CCntBaseStep::AddNewFieldL(CContactItemFieldSet &aFields, const TDesC& aLabel, 
const TFieldType aFieldType, const TBool aEmpty) const
	{
	CContactItemField *field = CContactItemField::NewLC(KStorageTypeText,aFieldType);
	field->SetLabelL(aLabel);
	if (!aEmpty)
		{
		field->TextStorage()->SetTextL(aLabel);//otherwise new field is not stored
		}
	aFields.AddL(*field);
	CleanupStack::Pop(field);
	}
	
/*checks if empty new field was added and uid of added field*/
TBool CCntBaseStep::CheckNewFieldL(const CContactItemFieldSet &aFields, const TInt aPos, 
const TFieldType aFieldType, const TBool aEmpty) const
	{
	TBool ret = EFalse;
	if(aEmpty)
		{
		ret = aFields.Count() <= aPos;
		}
	else
		{
		const CContentType &ctype = aFields[aPos].ContentType();
	 	ret = (ctype.FieldTypeCount() <= 2) && ( ctype.FieldType( 0 ) == aFieldType );//2 uids if other custom uid added
		}
	return ret;

	}

/*sets a specific contact field to a value as determined by its type.
field at aPos in aFields is set to aText*/
void CCntBaseStep::SetFieldL(CContactItemFieldSet &aFields, const TInt aPos, const TDesC& aText) const
	{
	CContactItemField &field = aFields[aPos];
	if( field.IsTemplateLabelField() )
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
			field.AgentStorage()->SetAgentId(TestConstants::KContactId);
		break;
		case KStorageTypeDateTime:
			field.DateTimeStorage()->SetTime(TestConstants::KDateTime);
		break;
		default:
			User::Panic(aText,EInvariantFalse);
		break;
		}
	}

/*checks the value of a specific field as determined by it type
returns true if field at aPos in aFields equals aText*/
TBool CCntBaseStep::CompareFieldsL(const CContactItemFieldSet &aFields, const TInt aPos, const TDesC& aText) const
	{
	TBool ret = EFalse;
	const CContactItemField &field = aFields[aPos];
	if(  field.IsTemplateLabelField() )
		{
		return ret;
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
			ret = field.AgentStorage()->Value() == TestConstants::KContactId;
		break;
		case KStorageTypeDateTime:
			ret = field.DateTimeStorage()->Time() == TestConstants::KDateTime;
		break;
		default:
			User::Panic(aText,EInvariantFalse);
		break;
		}
	return ret;
	
	}
	
void CCntBaseStep::AddEmptyContactsL(const TInt aNumber)
/**
Add number of empty contacts to db, using the system template.
@return None
@param aNumber number of contacts to be added
@pre None
@post database now contains aNumber of empty contacts
*/
	{
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	for(TInt k = 0; k<aNumber; k++)
		{		
		iIterate->AddL( iContactsDatabase->AddNewContactL(*contact) );
		}	
	CleanupStack::PopAndDestroy(contact);
	}

//sets the value of all fields for a given contact, uses alternating string values
void CCntBaseStep::SetContactFieldsL(CContactItem &aContact, const TInt aCount) const
	{
	CContactItemFieldSet &fields = aContact.CardFields();
	
	for(TInt i = 0; i < aCount; i++)
		{
		SetFieldL(fields, i, TestConstants::KLongString);
		}
	}

void CCntBaseStep::SetContactFieldsL(CContactItem &aContact) const
	{
	SetContactFieldsL( aContact, aContact.CardFields().Count() );
	}

//checks the value of all fields for a given contact, uses alternating string values	
TBool CCntBaseStep::CheckContactFieldsL(const CContactItem &aContact) const
	{
	TBool ret=EFalse;
	const CContactItemFieldSet& fields = aContact.CardFields();
	const TInt length = fields.Count();


	// Numerous field combinations will result in non-valid combinations, therefore the following
    // three UIDs are excluded for brevity
	for(TInt i = 0; i<length;i++)
		{	
		if (fields[i].ContentType().Mapping() == KUidContactFieldVCardMapUnusedN) 
			{
			continue;
			}
			
		if (fields[i].ContentType().ContainsFieldType(TFieldType::Uid(KIntContactFieldVCardMapTEL)))
			{
			continue;
			}
			
		if (fields[i].ContentType().ContainsFieldType(TFieldType::Uid(KIntContactFieldVCardMapSIPID)))
			{
			continue;
			}
			
		ret = CompareFieldsL(fields, i, TestConstants::KLongString);
        if (!ret) 
			{
			break;	
			}
		}	
	return ret;
	}

//set the value of all labels in a contact, uses semi-alternating string values
void CCntBaseStep::SetContactLabelsL(CContactItem &aContact) const
	{
	CContactItemFieldSet &fields = aContact.CardFields();
	TInt length = fields.Count();
	for(TInt i = 0; i<length; i++)
		{
		SetLabelL(fields, i,   TestConstants::KLongString);
		}
	}

//checks the value of all labels in a contact, uses semi-alternating fields	
TBool CCntBaseStep::CheckContactLabelsL(const CContactItem &aContact)
	{
	TBool ret=EFalse;
	CContactItemFieldSet &fields = aContact.CardFields();
	TInt length = fields.Count();
	
	// Numerous field combinations will result in non-valid combinations, therefore the following
    // three UIDs are excluded for brevity	
	for(TInt i = 0; i<length; i++)
		{	
		if (fields[i].ContentType().Mapping() == KUidContactFieldVCardMapUnusedN) 
			{
			continue;
			}
			
		if (fields[i].ContentType().ContainsFieldType(KUidContactFieldVCardMapTEL))
			{
			continue;
			}
			
		if (fields[i].ContentType().ContainsFieldType(KUidContactFieldVCardMapSIPID))
			{
			continue;
			}	
		
		ret = CompareLabels(fields, i,   TestConstants::KLongString);
	
		if(!ret) 
			{
			break;
			}
		}
	return ret;
	}
	
/*returns the number of empty fields in aFields*/
TInt CCntBaseStep::CountEmpty(const CContactItemFieldSet &aFields)
	{
	TInt count = 0;
	TInt ecount = 0;
	
	count = aFields.Count();
	for(TInt i = 0; i < count; ++i)
		{
		if( !( aFields[i].Storage()->IsFull() ) )
			{
			++ecount;
			}
		}
	return ecount;
	}
	
/*returns the number of hidden fields in aFields*/
TInt CCntBaseStep::CountHiddenFields(const CContactItemFieldSet &aFields) const
	{
	const TInt count = aFields.Count();;
	TInt hcount = 0;

	for(TInt i = 0; i < count; ++i)
		{
		if( aFields[i].IsHidden() )
			{
			++hcount;
			}
		}	
	return hcount;
	}

/*prints contents of aContact to an html file*/	
void CCntBaseStep::PrintCurrentContactL(const CContactItem &aContact) const
	{
	static TInt counter = 0;
	TFileName fileName;
	
	_LIT(KFileName,"c:\\cntperfviewdef\\%d\\%d.ViewDef.html");
	fileName.Format(KFileName,counter/4,counter%4);
	++counter;
	PrintContactL(fileName, aContact);
	}
	
/*prints contents of contactitem at aCid to an html file*/	
void CCntBaseStep::PrintCurrentContactL(const TContactItemId aCid) const
	{
	CContactItem *citem = iContactsDatabase->ReadContactLC(aCid,*iViewAll);
	PrintCurrentContactL(*citem);
	CleanupStack::PopAndDestroy(citem);
	}

/*opens contact item at aCid using viewdef aView for editing, initilises iContactItem*/
void CCntBaseStep::OpenL(	const TContactItemId aCid, 
							const CContactItemViewDef &aView, 
							const TBool aDefault)
	{
	if(aDefault)
		{
		iContactItem = iContactsDatabase->OpenContactL(aCid);
		}
	else
		{
		iContactItem = iContactsDatabase->OpenContactL(aCid,aView);
		}
	iFields = &(iContactItem->CardFields());
	}
/*commits open contact, must be called after openl*/
void CCntBaseStep::CommitL()
	{
	if(iContactItem != NULL)
		{		
		iFields = NULL;
		iContactsDatabase->CommitContactL(*iContactItem);
		delete iContactItem;
		iContactItem = NULL;
		}
	}
/*reads (readonly) contact item at aCid using viewdef aView for viewing, initilises iContactItem*/	
void CCntBaseStep::ReadL(	const TContactItemId aCid, 
							const CContactItemViewDef &aView, 
							const TBool aDefault)
	{
	if(aDefault)
		{
		iContactItem = iContactsDatabase->ReadContactL(aCid);
		}
	else
		{
		iContactItem = iContactsDatabase->ReadContactL(aCid,aView);
		}
	iFields = &(iContactItem->CardFields());
	}
/*reads non empty fields (readonly) for ocontact item at aCid using viewdef 
aView for viewing, initilises iContactItem*/		
void CCntBaseStep::ReadMinimalL(const TContactItemId aCid, 
								const CContactItemViewDef &aView, 
								const CContactItemViewDef &aDefView,
								const TBool aDefault)
	{
	if(aDefault)
		{
		iContactItem = iContactsDatabase->ReadMinimalContactL(aCid);
		}
	else
		{
		iContactsDatabase->SetViewDefinitionL( CContactViewDef::NewL( CopyViewL(aView) ) );
		iContactItem = iContactsDatabase->ReadMinimalContactL(aCid);
		iContactsDatabase->SetViewDefinitionL( CContactViewDef::NewL( CopyViewL(aDefView) ) );
		}
	iFields = &(iContactItem->CardFields());
	}
	
/*closes open contact, must be called after readl/openl*/	
void CCntBaseStep::Close()
	{
	if(iContactItem != NULL)
		{
		iFields = NULL;
		TRAPD( err, iContactsDatabase->CloseContactL( iContactItem->Id() ) );
		if( err )
			{
			_LIT(KClosingError,"Default contact couldn't be closed, error: %d");
			ERR_PRINTF2(KClosingError, err);
			}
		delete iContactItem;
		iContactItem = NULL;
		}
	}

/*returns a copy of aView*/	
CContactItemViewDef *CCntBaseStep::CopyViewL(const CContactItemViewDef &aView)
	{
	CContactItemViewDef *ret=CContactItemViewDef::NewLC( aView.Use(), aView.Mode() );
	TInt count = aView.Count();
	for(TInt i=0; i < count; i++)
		{
		ret->AddL(aView[i]);
		}
	CleanupStack::Pop(ret);
	return ret;
	}

//populates an array with all - vast majority of - contact uids
void CCntBaseStep::GetExistingUIDsL(RArray<TInt32> &aArray) const
	{
	
	aArray.AppendL(KUidContactFieldAddressValue);
	aArray.AppendL(KUidContactFieldPostOfficeValue);
	aArray.AppendL(KUidContactFieldExtendedAddressValue);
	aArray.AppendL(KUidContactFieldLocalityValue);
	aArray.AppendL(KUidContactFieldRegionValue);
	aArray.AppendL(KUidContactFieldPostCodeValue);
	aArray.AppendL(KUidContactFieldCountryValue);
	aArray.AppendL(KUidContactFieldCompanyNameValue);
	aArray.AppendL(KUidContactFieldCompanyNamePronunciationValue);
	aArray.AppendL(KUidContactFieldPhoneNumberValue);
	aArray.AppendL(KUidContactFieldGivenNameValue);
	aArray.AppendL(KUidContactFieldFamilyNameValue);
	aArray.AppendL(KUidContactFieldGivenNamePronunciationValue);
	aArray.AppendL(KUidContactFieldFamilyNamePronunciationValue);
	aArray.AppendL(KUidContactFieldAdditionalNameValue);
	aArray.AppendL(KUidContactFieldSuffixNameValue);
	aArray.AppendL(KUidContactFieldPrefixNameValue);
	aArray.AppendL(KUidContactFieldHiddenValue);
	aArray.AppendL(KUidContactFieldEMailValue);
	aArray.AppendL(KUidContactFieldMsgValue);
	aArray.AppendL(KUidContactFieldSmsValue);
	aArray.AppendL(KUidContactFieldFaxValue);
	aArray.AppendL(KUidContactFieldDefinedTextValue);
	aArray.AppendL(KUidContactFieldNoteValue);
	aArray.AppendL(KUidContactFieldBirthdayValue);
	aArray.AppendL(KUidContactFieldUrlValue);
	aArray.AppendL(KUidContactFieldStorageInlineValue);
	aArray.AppendL(KUidContactFieldTemplateLabelValue);
	aArray.AppendL(KUidContactFieldPictureValue);
	aArray.AppendL(KUidContactFieldRingToneValue);
	aArray.AppendL(KUidContactFieldDTMFValue);
	aArray.AppendL(KUidContactsVoiceDialFieldValue);
	aArray.AppendL(KUidContactFieldNoneValue);
	aArray.AppendL(KUidContactFieldJobTitleValue);
	aArray.AppendL(KUidContactFieldICCSlotValue);
	aArray.AppendL(KUidContactFieldICCPhonebookValue);
	aArray.AppendL(KUidContactFieldICCGroupValue);
	aArray.AppendL(KUidContactFieldIMAddressValue);
	aArray.AppendL(KUidContactFieldSecondNameValue);
	aArray.AppendL(KUidContactFieldSIPIDValue);
	aArray.AppendL(KIntContactFieldVCardMapWORK);
	aArray.AppendL(KIntContactFieldVCardMapHOME);
	aArray.AppendL(KIntContactFieldVCardMapMSG);
	aArray.AppendL(KIntContactFieldVCardMapVOICE);
	aArray.AppendL(KIntContactFieldVCardMapFAX);
	aArray.AppendL(KIntContactFieldVCardMapPREF);
	aArray.AppendL(KIntContactFieldVCardMapCELL);
	aArray.AppendL(KIntContactFieldVCardMapPAGER);
	aArray.AppendL(KIntContactFieldVCardMapBBS);
	aArray.AppendL(KIntContactFieldVCardMapMODEM);
	aArray.AppendL(KIntContactFieldVCardMapCAR);  
	aArray.AppendL(KIntContactFieldVCardMapISDN);
	aArray.AppendL(KIntContactFieldVCardMapVIDEO);
	aArray.AppendL(KIntContactFieldVCardMapDOM);
	aArray.AppendL(KIntContactFieldVCardMapADR);
	aArray.AppendL(KIntContactFieldVCardMapPOSTOFFICE);
	aArray.AppendL(KIntContactFieldVCardMapEXTENDEDADR);
	aArray.AppendL(KIntContactFieldVCardMapLOCALITY);
	aArray.AppendL(KIntContactFieldVCardMapREGION);
	aArray.AppendL(KIntContactFieldVCardMapPOSTCODE);
	aArray.AppendL(KIntContactFieldVCardMapCOUNTRY);
	aArray.AppendL(KIntContactFieldVCardMapAGENT);
	aArray.AppendL(KIntContactFieldVCardMapBDAY);
	aArray.AppendL(KIntContactFieldVCardMapEMAILINTERNET);
	aArray.AppendL(KIntContactFieldVCardMapGEO);
	aArray.AppendL(KIntContactFieldVCardMapLABEL);
	aArray.AppendL(KIntContactFieldVCardMapLOGO);
	aArray.AppendL(KIntContactFieldVCardMapMAILER);
	aArray.AppendL(KIntContactFieldVCardMapNOTE);
	aArray.AppendL(KIntContactFieldVCardMapORG);
	aArray.AppendL(KIntContactFieldVCardMapORGPronunciation);
	aArray.AppendL(KIntContactFieldVCardMapPHOTO);
	aArray.AppendL(KIntContactFieldVCardMapROLE);
	aArray.AppendL(KIntContactFieldVCardMapSOUND);
	aArray.AppendL(KIntContactFieldVCardMapTEL);
	aArray.AppendL(KIntContactFieldVCardMapTELFAX);
	aArray.AppendL(KIntContactFieldVCardMapTITLE);
	aArray.AppendL(KIntContactFieldVCardMapURL);
	aArray.AppendL(KIntContactFieldVCardMapUnusedN);
	aArray.AppendL(KIntContactFieldVCardMapUnusedFN);
	aArray.AppendL(KIntContactFieldVCardMapNotRequired);
	aArray.AppendL(KIntContactFieldVCardMapUnknownXDash);
	aArray.AppendL(KIntContactFieldVCardMapUnknown);
	aArray.AppendL(KIntContactFieldVCardMapUID);
	aArray.AppendL(KIntContactFieldVCardMapINTL);
	aArray.AppendL(KIntContactFieldVCardMapPOSTAL);
	aArray.AppendL(KIntContactFieldVCardMapPARCEL);
	aArray.AppendL(KIntContactFieldVCardMapGIF);
	aArray.AppendL(KIntContactFieldVCardMapCGM);
	aArray.AppendL(KIntContactFieldVCardMapWMF);
	aArray.AppendL(KIntContactFieldVCardMapBMP);
	aArray.AppendL(KIntContactFieldVCardMapMET);
	aArray.AppendL(KIntContactFieldVCardMapPMB);
	aArray.AppendL(KIntContactFieldVCardMapDIB);
	aArray.AppendL(KIntContactFieldVCardMapPICT);
	aArray.AppendL(KIntContactFieldVCardMapTIFF);
	aArray.AppendL(KIntContactFieldVCardMapPDF);
	aArray.AppendL(KIntContactFieldVCardMapPS);
	aArray.AppendL(KIntContactFieldVCardMapJPEG);
	aArray.AppendL(KIntContactFieldVCardMapMPEG);
	aArray.AppendL(KIntContactFieldVCardMapMPEG2);
	aArray.AppendL(KIntContactFieldVCardMapAVI);
	aArray.AppendL(KIntContactFieldVCardMapQTIME);
	aArray.AppendL(KIntContactFieldVCardMapTZ);
	aArray.AppendL(KIntContactFieldVCardMapKEY);
	aArray.AppendL(KIntContactFieldVCardMapX509);
	aArray.AppendL(KIntContactFieldVCardMapPGP);
	aArray.AppendL(KIntContactFieldVCardMapSMIME);
	aArray.AppendL(KIntContactFieldVCardMapWV);
	aArray.AppendL(KIntContactFieldVCardMapSECONDNAME);
	aArray.AppendL(KIntContactFieldVCardMapSIPID);
	aArray.AppendL(KIntContactFieldVCardMapPOC);
	aArray.AppendL(KIntContactFieldVCardMapSWIS);
	aArray.AppendL(KIntContactFieldVCardMapVOIP);

	
	}
	
		
void CCntBaseStep::MissngTestPanic() const
	{
	_LIT(KInvalidTest,"Invalid Test");
	User::Panic(KInvalidTest, 444);
	}

void CCntBaseStep::WaitForServerToExitL()
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

void CCntBaseStep::CloseProcessL(const TDesC& aProcessName, const TInt aIteration)
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

TBool CCntBaseStep::StringComparisonTestPrint(	const TDesC &aCompareL,
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


TBool CCntBaseStep::IntegerComparisonTestPrint(	const TInt	aCompareL,
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


