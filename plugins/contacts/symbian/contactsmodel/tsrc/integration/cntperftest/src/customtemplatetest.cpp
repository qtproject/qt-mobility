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
#include "clientserver.h"
#include "customtemplatetest.h"

_LIT(KRun1,"Empty");
_LIT(KRun2,"Full");
_LIT(KRun3,"Modified");
_LIT(KRun4,"MinimalModified");
_LIT(KRun5,"CustomNew");
_LIT(KRun6,"OnlyNew");
_LIT(KRun7,"Dynamic");
_LIT(KRun8,"CustomLabelUid");
_LIT(KRun9,"DeleteCustom");
_LIT(KRun10,"DeleteSystem");

_LIT(KTest1,"Testing empty template...");
_LIT(KTest2,"Testing full template...");
_LIT(KTest3,"Testing modified template...");
_LIT(KTest4,"Testing modified template loaded minimally...");
_LIT(KTest5,"Testing template plus custom new fields...");
_LIT(KTest6,"Testing template containing custom new fields only...");
_LIT(KTest7,"Testing system and custom template dynamic loading...");
_LIT(KTest8,"Testing system and custom template with custom labels and uids...");
_LIT(KTest9,"Testing deletion of custom template...");
_LIT(KTest10,"Testing deletion of system template...");



CCustomTemplateTest::CCustomTemplateTest() 
			: CCntBaseStep(1) //Number of contacts to be used by this step
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KCustomTemplateTest);
	}
	
CCustomTemplateTest::~CCustomTemplateTest()
	{
	_LIT(KDestructPrint,"CCustomTemplateTest destructor");
	INFO_PRINTF1(KDestructPrint);
	Clean();
	TRAPD( err, CommitL() );
	if( err )
		{
		_LIT(KCommitError,"Couldnt commit default contact. Error: %d");
		ERR_PRINTF2(KCommitError, err );
		}
	}

TVerdict CCustomTemplateTest::doTestStepL()
	{
	
	__UHEAP_MARK;
	InitializeL();
	iIterate->Reset();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		INFO_PRINTF1(KTest1);
		EmptyTemplateTestL(iIterate->NextL());
		}
	else if( run == KRun2 )
		{
		INFO_PRINTF1(KTest2);
		FullTemplateTestL(iIterate->NextL());
		}
	else if( run == KRun3 )
		{
		INFO_PRINTF1(KTest3);
		ModifiedTemplateTestL(iIterate->NextL(), EFalse);
		}
	else if( run == KRun4 )
		{
		INFO_PRINTF1(KTest4);
		ModifiedTemplateTestL(iIterate->NextL(), ETrue);
		}
	else if( run == KRun5 )
		{
		INFO_PRINTF1(KTest5);
		NewFieldsTemplateTestL(iIterate->NextL(), ETrue);
		}
	else if( run == KRun6 )
		{
		INFO_PRINTF1(KTest6);
		NewFieldsTemplateTestL(iIterate->NextL(), EFalse);
		}
	else if( run == KRun7 )
		{
		INFO_PRINTF1(KTest7);
		TestDynamicLoadingL(iIterate->NextL());
		}
	else if( run == KRun8 )
		{
		INFO_PRINTF1(KTest8);
		 CustomFieldsTemplateTestL(iIterate->NextL(), ETrue, ETrue );
		}
	else if( run == KRun9 )
		{
		INFO_PRINTF1(KTest9);
		DeleteCustomTemplateTestL( iIterate->NextL() );
		}
	else if( run == KRun10 )
		{
		INFO_PRINTF1(KTest10);
		iIterate->PreviousL();
		DeleteSystemTemplateTestL( iIterate->NextL() );
		}
	else
		{
		MissngTestPanic();
		}
	Cleanup();
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

/**
create template with no fields, create contact based on template, check that contact has no fields
*/	
void CCustomTemplateTest::EmptyTemplateTestL(const TContactItemId aCid)
	{
	LoadContactL(aCid);
	iFields->Reset();
	//create empty custom template based on empty contact(no fields)
	CreateCustomTemplateL();
	
	//create contact based on templated
	const TContactItemId &KTemplatedContactId = AddTemplatedContactL(*iCustomTemplate);
	
	Clean();
	
	//read templated contact
	LoadContactL(KTemplatedContactId);
	//check that contact is empty
	INTCOMPARE(iFields->Count(), ==, 0, 0, 0 );
	
	Clean();
	iContactsDatabase->DeleteContactL(iCustomTemplateId);
	iContactsDatabase->DeleteContactL(KTemplatedContactId);
	iCustomTemplateId = -1;
	}

/**
create template with all fields, create contact based on template, check that contact has all fields
*/		
void CCustomTemplateTest::FullTemplateTestL(const TContactItemId aCid)
	{
	LoadContactL(aCid);
	SetContactFieldsL(*iContactRead);
	const TInt KFullContactFieldCount = iFields->Count();
	//create full custom template based on full contact (fully populated list of fields)
	CreateCustomTemplateL();
	
	//create contact based on templated
	const TContactItemId &KTemplatedContactId = AddTemplatedContactL(*iCustomTemplate);
	
	Clean();
	
	//read templated contact
	LoadContactL(KTemplatedContactId);
	
	const TInt KTemplatedContactFieldCount = iFields->Count();
	
	//checks that all fields in contact are empty
	INTCOMPARE(KTemplatedContactFieldCount, ==, KFullContactFieldCount, 0 ,0); 
	//check that all fields in templated contact are empty
	INTCOMPARE(CountEmpty(*iFields), ==, KTemplatedContactFieldCount, 0, 0);
	
	Clean();
	iContactsDatabase->DeleteContactL(KTemplatedContactId);
	iContactsDatabase->DeleteContactL(iCustomTemplateId);
	iCustomTemplateId = -1;	
	}

/**
create template then set all its fields to preset values, create contact based on template
check that all field values, labels, uids match between template and contact
*/		
void CCustomTemplateTest::ModifiedTemplateTestL(const TContactItemId aCid, const TBool aReadMinimal)
	{
	LoadContactL(aCid);
	const TInt KFullContactFieldCount = iFields->Count();
	
	//create custom template based on full contact
	CreateCustomTemplateL();
	//create contact based on custom templated	
	const TContactItemId &KTemplatedContactId = AddTemplatedContactL(*iCustomTemplate);	
	Clean();
	
	//open custom template
	OpenL(iCustomTemplateId);
	//update template fields to contain preset values
	SetContactFieldsL(*iContactOpened);
	CommitL();
	
	//read contact based on custom tempalte, either fully or minimally
	LoadContactL(KTemplatedContactId, aReadMinimal);
	
	const TInt KTemplatedContactFieldCount = iFields->Count();
	if(aReadMinimal)
		{
		//minimal read doesn't merge contact with template, no contact field should be read
		INTCOMPARE( KTemplatedContactFieldCount, ==, 0, 0, 0 );
		INTCOMPARE(CountEmpty(*iFields), ==, 0, 0, 0);
		}
	else
		{
		//full read merges contact with template, all fields should be loaded
		INTCOMPARE(KTemplatedContactFieldCount, ==, KFullContactFieldCount, 0,0 );
		//all fields should be full
		INTCOMPARE(CountEmpty(*iFields), ==, 0, 0, 0);
		//templated contact should match template
		SINGLECOMPARE(CompareToTemplateL( iCustomTemplateId, *iFields), 0 ,0 );
		}
	Clean();
	iContactsDatabase->DeleteContactL(KTemplatedContactId);
	iContactsDatabase->DeleteContactL(iCustomTemplateId);
	iCustomTemplateId = -1;
	}

/**
create template then add 10 new custom fields set all fields to preset values, 
create contact based on template check that all field values, labels, uids 
match between template and contact
*/		
void CCustomTemplateTest::NewFieldsTemplateTestL(const TContactItemId aCid, const TBool aNewAndOldFields)
	{
	LoadContactL(aCid);
	if(!aNewAndOldFields)
		{
		//delete all contacts		
		iFields->Reset();
		}
	//add custom new fields to contact
	for(TInt i = 0, uids = TestConstants::KAltUID; i < 10; i++, uids++)
		{
		AddNewFieldL( *iFields, TestConstants::KShortString, TFieldType::Uid(uids) );
		}
		
	const TInt KBaseContactFieldCount = iFields->Count();	
	//create custom template based on base contact
	CreateCustomTemplateL();
	const TContactItemId &KTemplatedContactId = AddTemplatedContactL(*iCustomTemplate);	
	Clean();
	
	//populate custom template fields
	OpenL(iCustomTemplateId);
	SetContactFieldsL(*iContactOpened);		
	CommitL();
	
	//read templated contact
	LoadContactL(KTemplatedContactId);
	
	//templated contact should contain all fields
	const TInt KTemplatedContactFieldCount = iFields->Count();
	INTCOMPARE(KTemplatedContactFieldCount, ==, KBaseContactFieldCount, 0 ,0 );
	
	//all fields should be populated 
	INTCOMPARE(CountEmpty(*iFields), ==, 0, 0, 0);
	//fields should match template
	SINGLECOMPARE(CompareToTemplateL( iCustomTemplateId, *iFields), 0, 0);
	
	Clean();
	iContactsDatabase->DeleteContactL(KTemplatedContactId);
	iContactsDatabase->DeleteContactL(iCustomTemplateId);
	iCustomTemplateId = -1;
	}
	


/**
create custom template, then create contact based on template. perform dynmaic loading on custom template.
Repeate fro system template
*/	
void CCustomTemplateTest::TestDynamicLoadingL(const TContactItemId aCid)
	{
	_LIT(KStartSystem, "SystemTemplate Dynamic testing");
	_LIT(KStartCustom, "CustomTemplate Dynamic testing");
	
	INFO_PRINTF1(KStartSystem);
	LoadContactL(aCid);
	const TInt KBaseContactFieldCount = iFields->Count();
	//create custom template based on full contact
	CreateCustomTemplateL();	
	//create templated contact
	const TContactItemId &KTemplatedContactId = AddTemplatedContactL(*iCustomTemplate);	
	Clean();
	
	//populate custom template fields
	OpenL(iCustomTemplateId);
	SetContactFieldsL(*iContactOpened);//need to set all fields otherwise field order is not preserved		
	CommitL();
	
	//perform dyamic loading for custom template
	doDynamicLoadingL(KTemplatedContactId, iCustomTemplateId, KBaseContactFieldCount);
	iContactsDatabase->DeleteContactL(iCustomTemplateId);
	iContactsDatabase->DeleteContactL(KTemplatedContactId);
		
	iCustomTemplateId = -1;
	
	INFO_PRINTF1(KStartCustom);
	//open system template
	OpenL(iContactsDatabase->TemplateId());
	//backup template fields
	CContactItemFieldSet * SystemTemplateFieldsetCopy = CopyFieldSetLC(*iFields);
	const TInt KSystemTemplateFieldCount = iFields->Count();
	//add template contact
	const TContactItemId &KSystemTemplatedContactId = AddTemplatedContactL( iContactsDatabase->TemplateId() );
	//populate fields for system template
	SetContactFieldsL(*iContactOpened);//need to set all fields otherwise field order is not preserved	

	CContactItemFieldSet &contactFields = iContactOpened->CardFields();
	const TInt KFieldsCount = contactFields.Count();
	
	// The system template's fields must not contain any data (see CContactDatabase::CommitContactL)
	if (iContactOpened->Id() == iContactsDatabase->TemplateId())
		{	
		for( TInt j = 0; j < KFieldsCount; ++j )
			{
			contactFields[j].ResetStore();
			}
		}
		
	CommitL();
	
	//perfrom dynamic loading for system template
	doDynamicLoadingL(KSystemTemplatedContactId, iContactsDatabase->TemplateId(), KSystemTemplateFieldCount-1, ETrue);
	
	iContactsDatabase->DeleteContactL(KSystemTemplatedContactId);
	
	OpenL(iContactsDatabase->TemplateId());
	iContactOpened->UpdateFieldSet(SystemTemplateFieldsetCopy);//restore system template
	CleanupStack::Pop(SystemTemplateFieldsetCopy);
	CommitL();
	}

/**
delete 1 field from template (aTemplate) and set value for next field. 
reload contact (aCid) based on template, check to see that it matches the updated template.
repeat for all fields in template
*/
void CCustomTemplateTest::doDynamicLoadingL(	const TContactItemId aCid, 
												const TContactItemId aTemplate, 
												const TInt aCount,
												const TBool aSystemTemplate)
	{
	_LIT(KDynamic,"DynamicTestValues");
	CContactItemField* NextField = NULL;
	TInt ContactFieldsCount = 0;  // avoid compiler warning
	TInt PositionOfNextField = 0;  // avoid compiler warning
	const TInt KLastFieldIndex = aSystemTemplate ? 0 : 1;
	TInt initialFieldCount = 70;  // number of fields in the template

	for(TInt i = aCount ; i > KLastFieldIndex; --i)
		{
		//open template
		OpenL(aTemplate);
		
		//the template's name ("Group / Template Label") is not part of the Contact based on the template
		const CContentType& KContentType = (*iFields)[i].ContentType();	
		
		if (KContentType.ContainsFieldType(KUidContactFieldTemplateLabel) && aSystemTemplate)
			{
			CommitL();
			continue;
			}
		
		//remove 1 field
		iFields->Remove(i);
		
		// if this is not the system template
		if (iContactOpened->Id() != iContactsDatabase->TemplateId())
			{
			//set value of next field to dynamic value
			SetFieldL(*iFields, i-1, KDynamic);
			}
		
		//copy next field
		NextField = CContactItemField::NewLC( (*iFields)[i-1]);		
		
		CommitL();
		
		//read templated contact
		LoadContactL(aCid);
		ContactFieldsCount = iFields->Count();
		
		//search for next field in tempalted contact
		PositionOfNextField = FindCtype( NextField->ContentType(), *iFields );
		//check that field count equal initial fieldcount - number of fields removed	
		INTCOMPARE( ContactFieldsCount, ==, ( initialFieldCount - 1 ), i, 0 );
		initialFieldCount--;
		
		//if next field not found
		if(PositionOfNextField == KErrNotFound)
			{
			//if not system template 
			if( !aSystemTemplate) 
				{
				//fail test
				SINGLECOMPARE(EFalse, i, 0);
				}
			}
		//if next field found
		else
			{		 	
			if(!aSystemTemplate)  // the system template fields do not contain data
				{
				SINGLECOMPARE( CompareFieldsL(*iFields, PositionOfNextField, KDynamic), i, 0 );
				}
			}
		CleanupStack::PopAndDestroy(NextField);
		NextField = NULL;
		Clean();
		}
	}

/**
set custom labels and/or uids for template. then create contact based on template. 
check contact matches template. repeate for contact based on system template
*/	
void CCustomTemplateTest::CustomFieldsTemplateTestL(	const TContactItemId aCid, 
														const TBool aSetCustomLabels,
														const TBool aUids)
	{
	LoadContactL(aCid);
	//create custom template based on full contact
	CreateCustomTemplateL();	
	//create templated contact
	const TContactItemId &KTemplatedContactId = AddTemplatedContactL(*iCustomTemplate);	
	Clean();
	
	//open custom template
	OpenL(iCustomTemplateId);
	SetContactFieldsL(*iContactOpened);//need to set all fields otherwise field order is not preserved
	if( aSetCustomLabels )
		{
		//update template field labels
		SetContactLabelsL(*iContactOpened);
		}
	if( aUids )
		{
		//update template field uids
		AddUIDsL(*iFields,TestConstants::KInitialUID);
		}
	CommitL();
	
	//load templated contact
	LoadContactL(KTemplatedContactId);
	//compare fields from template contact to template
	SINGLECOMPARE(CompareToTemplateL( iCustomTemplateId, *iFields),0,0);
	Clean();
	
	iContactsDatabase->DeleteContactL(KTemplatedContactId);
	iContactsDatabase->DeleteContactL(iCustomTemplateId);
	iCustomTemplateId = -1;
	
	//create templated contact based on sytem template
	const TContactItemId &KSystemTemplatedContactId = AddTemplatedContactL( iContactsDatabase->TemplateId() );
	OpenL(iContactsDatabase->TemplateId());
	//backup system template fields
	CContactItemFieldSet *SystemTemplateFieldsetCopy = CopyFieldSetLC(*iFields);
	iFields->Remove( iFields->Find(KUidContactFieldTemplateLabel) );//remove group field
	
	const TInt KFieldsCount = iFields->Count();
	for( TInt j = 0; j < KFieldsCount; ++j )
		{
		(*iFields)[j].ResetStore();
		}
	
	
	if( aSetCustomLabels )
		{
		//update template field labels
		SetContactLabelsL(*iContactOpened);
		}
	if( aUids )
		{
		//update template field uids
		AddUIDsL(*iFields,TestConstants::KInitialUID);
		}
	CommitL();
	
	//load templated contact
	LoadContactL(KSystemTemplatedContactId);
	//compare fields from template contact to system template
	SINGLECOMPARE(CompareToTemplateL( iContactsDatabase->TemplateId(), *iFields, 0, ETrue ),0,0);
	Clean();
	
	iContactsDatabase->DeleteContactL(KSystemTemplatedContactId);
	
	OpenL(iContactsDatabase->TemplateId());
	iContactOpened->UpdateFieldSet(SystemTemplateFieldsetCopy);//restore system template
	CleanupStack::Pop(SystemTemplateFieldsetCopy);
	CommitL();
	}

/**
create custom template, create contact based on template, delete template. reload template,
check to see it is marked as deleted. attempt to read contact again. delete contact and check
whether template is still in the dbase, if so delete template
*/	
void CCustomTemplateTest::DeleteCustomTemplateTestL(const TContactItemId aCid)
	{
	TInt err = KErrNone;
	CContactItem *ContactItem = NULL;
	
	//read contact item		
	LoadContactL(aCid);
	const TInt KContactFieldCount = iFields->Count();
	const TInt KContactHiddenCount = CountHiddenFields(*iFields);
	
	//create custom template based on full contact
	CreateCustomTemplateL();
	//create templated contact	
	const TContactItemId &KTemplatedContactId = AddTemplatedContactL(*iCustomTemplate);
	Clean();
	
	//open custom template
	OpenL(iCustomTemplateId);
	//populate template
	SetContactFieldsL(*iContactOpened);//need to set all fields otherwise field order is not preserved
	
	const TInt KTemplateAccessCount = iContactOpened->AccessCount();
	//check that template is not deleteable as a conatct is based on it
	SINGLECOMPARE(!iContactOpened->IsDeletable(),0,0);
	//access count should equal number of contacts based on template, 1
	INTCOMPARE( KTemplateAccessCount, ==, 1, 0, 0  );		
	CommitL();
	
	//attempt to delete custom template
	iContactsDatabase->DeleteContactL(iCustomTemplateId);
	
	//read deleted template		
	TRAP( err, ContactItem = iContactsDatabase->ReadContactL(iCustomTemplateId,*iViewAll) );
	//template should not have been fully deleted
	INTCOMPARE( err, ==, KErrNone, 0, 0  );
	//template should be marked as deleted
	SINGLECOMPARE(ContactItem->IsDeleted(),0,0);
	CLEAR(ContactItem);
	
	//deleted template with access count > 0 should contain the same number of fields pre as post delete.
	SINGLECOMPARE(CheckDeleteItemL(KTemplatedContactId, KContactFieldCount, KContactHiddenCount),0,0);
	SINGLECOMPARE(CheckDeleteItemL(iCustomTemplateId, KContactFieldCount+1, KContactHiddenCount),0,0);
	
	//it should be possible to read templated contact
	TRAP( err, ContactItem = iContactsDatabase->ReadContactL(KTemplatedContactId,*iViewAll) );
	INTCOMPARE( err, ==, KErrNone,0,0);
	CLEAR(ContactItem);
	
	//it should be possible to delete templated contact
	iContactsDatabase->DeleteContactL(KTemplatedContactId);
	//templated contact should not exist / be deleted
	TRAP( err, ContactItem = iContactsDatabase->ReadContactL(iCustomTemplateId,*iViewAll) );
	INTCOMPARE( err, ==, KErrNotFound,0,0 );
	CLEAR(ContactItem);
	
	iCustomTemplateId = -1;
	}

/**
attempt to delete system template, check that it fails and doesnt affect any contacts based on it.
*/
void CCustomTemplateTest::DeleteSystemTemplateTestL(const TContactItemId aCid)
	{
	TInt err = KErrNone;
	CContactItem *ContactItem = NULL;
	const TContactItemId &KSystemTemplateId = iContactsDatabase->TemplateId();
	LoadContactL(aCid);
	const TInt KContactFieldCount = iFields->Count();
	Clean();
	//attempt to delete system template
	TRAP( err, iContactsDatabase->DeleteContactL( KSystemTemplateId ) );
	//it should not be possible
	INTCOMPARE(KErrNotSupported, ==, err,0,0);
	
	//attempt to read system template
	TRAP( err, ContactItem = iContactsDatabase->ReadContactL( KSystemTemplateId ,*iViewAll) );
	CLEAR(ContactItem);
	//template should not have been deleted
	INTCOMPARE( KErrNone, ==, err,0,0 );
	
	//read system template
	LoadContactL(aCid);
	const TInt KDeletedContactFieldCount = iFields->Count();
	Clean();
	//number of fields should be unchanged
	INTCOMPARE( KDeletedContactFieldCount, ==, KContactFieldCount,0,0 );
	}

//deleted template with access count > 0 should contain the same number of fields pre as post delete.
TBool CCustomTemplateTest::CheckDeleteItemL(const TContactItemId aCid,
											const TInt aFieldCountPreDelete, 
											const TInt aHiddenCountPreDelete )
	{
	TBool ret = EFalse;
	
	LoadContactL(aCid);
	const TInt KFieldCountPostDelete = iFields->Count();
	const TInt KHiddenFieldCountPostDelete = CountHiddenFields(*iFields);
	const TInt KEmptyFieldCountPostDelete = CountEmpty(*iFields);
	ret = (KFieldCountPostDelete == aFieldCountPreDelete);
	ret = ret && ( KEmptyFieldCountPostDelete == 0 ) && ( KHiddenFieldCountPostDelete == aHiddenCountPreDelete);
	
	Clean();
	
	return ret;
	}
	
void CCustomTemplateTest::CreateCustomTemplateL()
	{
	_LIT(KTLabel,"UserDefined template");
	iCustomTemplate = iContactsDatabase->CreateContactCardTemplateL(iContactRead,KTLabel);
	iCustomTemplateId = iCustomTemplate->Id();
	}
	
void CCustomTemplateTest::LoadContactL(const TContactItemId aCid, const TBool aReadMinimal)
	{
	if(aReadMinimal)
		{
		iContactRead = iContactsDatabase->ReadMinimalContactL(aCid);
		}
	else
		{
		iContactRead = iContactsDatabase->ReadContactL(aCid,*iViewAll);
		}
	iFields = &(iContactRead->CardFields());
	}
	
void CCustomTemplateTest::OpenL(const TContactItemId aCid)
	{
	iContactOpened = iContactsDatabase->OpenContactL(aCid,*iViewAll);
	iFields = &(iContactOpened->CardFields());
	}
	
void CCustomTemplateTest::CommitL()
	{
	if(NULL == iContactOpened)
		{
		return;
		}
	iContactsDatabase->CommitContactL(*iContactOpened);
	CLEAR(iContactOpened);
	iFields = NULL;
	}
	
TContactItemId CCustomTemplateTest::AddTemplatedContactL(const CContactItem &aTemplate)
	{
	CContactCard* contact = CContactCard::NewLC(&aTemplate);
	const TContactItemId &cid = iContactsDatabase->AddNewContactL(*contact);
	CleanupStack::PopAndDestroy(contact);
	return cid;
	}

TContactItemId CCustomTemplateTest::AddTemplatedContactL(const TContactItemId aTemplate)
	{
	CContactItem *ctemplate = iContactsDatabase->ReadContactLC(aTemplate,*iViewAll);
	const TContactItemId &cid = AddTemplatedContactL(*ctemplate);
	CleanupStack::PopAndDestroy(ctemplate);
	return cid;
	}
	
void CCustomTemplateTest::Clean()
	{
	delete iCustomTemplate;
	iCustomTemplate = NULL;
	CLEAR(iContactRead);
	iFields = NULL;
	}

TBool CCustomTemplateTest::CompareToTemplateL( 	const TContactItemId aCid, 
												const CContactItemFieldSet &aFields,
												const TInt aStart,
												const TBool aNogroup )
	{
	_LIT(KGroupField, "Group / Template Label");
	TBool ret = EFalse;
	const CContactItem *citem = iContactsDatabase->ReadContactLC(aCid,*iViewAll);
	const CContactItemFieldSet &cfields = citem->CardFields();
	const TInt count = cfields.Count();

	TInt fPos = -1;
	TInt i = aStart;//ignore first field in template as that is template 
					//related custom field (access count?)
		
	for(; i < count; ++i)
		{
		if( aNogroup && ( cfields[i].Label() ).Compare(KGroupField) == 0)//ignore group field
			{
			continue;
			}
		else if( ( fPos = FindCtype( cfields[i].ContentType(), aFields ) ) == KErrNotFound )
			{
			break;
			}
		else if( ( aFields[fPos].Label() ).Compare( cfields[i].Label() ) != 0 )
			{
			break;
			}
		// the system template fields should contain no data (i.e. don't test for this)	
		}
		
	if( count == i )
		{
		ret = ETrue;
		}
	CleanupStack::PopAndDestroy( CONST_CAST(CContactItem *, citem) );
	return ret;
	}

TInt CCustomTemplateTest::FindCtype( const CContentType &aCtype, const CContactItemFieldSet &aFields )
	{
	TInt ret = KErrNotFound;
	const TInt count = aFields.Count();
	for(TInt i = 0; i < count; ++i)
		{
		if( aCtype == aFields[i].ContentType() )
			{
			return i;
			}
		}
	return ret;
	}

CContactItemFieldSet *CCustomTemplateTest::CopyFieldSetLC(const CContactItemFieldSet &aFields)
	{
	const TInt count = aFields.Count();
	CContactItemFieldSet *FieldSetCopy = CContactItemFieldSet::NewLC();
	FieldSetCopy->Reset();//make sure its empty
	CContactItemField *field = NULL;
	
	for( TInt i = 0; i < count; ++i )
		{
		field = CContactItemField::NewLC(aFields[i]);
		FieldSetCopy->AddL(*field);
		CleanupStack::Pop(field);
		field = NULL;
		}
	return FieldSetCopy;
	}

TBool CCustomTemplateTest::CompareFields(const CContactItemField &aField1, const CContactItemField &aField2) const
	{
	TBool ret = EFalse;
	if(  aField1.IsTemplateLabelField() || aField2.IsTemplateLabelField() )
		{
		return ret;
		}
	switch( aField1.StorageType() )
		{
		case KStorageTypeText:
			ret = (  (aField1.TextStorage()->Text()).Compare(aField2.TextStorage()->Text()) ) == 0;
		break;
		case KStorageTypeStore:
			ret = ( ( aField1.StoreStorage()->Thing() )->Compare( *(aField2.StoreStorage()->Thing()) ) == 0 );
		break;
		case KStorageTypeContactItemId:
			ret = aField1.AgentStorage()->Value() == aField2.AgentStorage()->Value();
		break;
		case KStorageTypeDateTime:
			ret = aField1.DateTimeStorage()->Time() == aField2.DateTimeStorage()->Time();
		break;
		default:
			{
			_LIT(KCompareFields,"comparefields panic");
			User::Panic(KCompareFields,EInvariantFalse);
			}
		break;
		}
	return ret;
	
	}



