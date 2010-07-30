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
#include "customlabeluid.h"
#include "clientserver.h"

_LIT(KRun1,"Uids");
_LIT(KRun2,"Labels");
_LIT(KRun3,"LabelsUids");
_LIT(KRun4,"LabelsUidsPlus");
_LIT(KRun5,"DuplicateUids");
_LIT(KRun6,"DuplicateExistingUids");
_LIT(KRun7,"UniqueExistingUids");

_LIT(KTest1,"Testing custom uids...");
_LIT(KTest2,"Testing custom labels...");
_LIT(KTest3,"Testing custom labels and uids...");
_LIT(KTest4,"Testing custom labels and uids plus new fields...");
_LIT(KTest5,"Testing Duplicate custom and exisiting Uids");
_LIT(KTest6,"Testing Duplicate  exisiting Uids");
_LIT(KTest7,"Testing Unique  exisiting Uids");

_LIT(KEnter,"In Test Step");


CCustomLabelUid::CCustomLabelUid() : 
			CCntBaseStep( 1 )//Number of contacts to be used by this step
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KCustomLabelUid);
	}

TVerdict CCustomLabelUid::doTestStepL()
	{
	
	__UHEAP_MARK;

	INFO_PRINTF1(KEnter);
	InitializeL();
	iIterate->Reset();
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		INFO_PRINTF1(KTest1);
		TestCustomUidsL(iIterate->NextL() );
		}
	else if( run == KRun2 )
		{
		INFO_PRINTF1(KTest2);
		TestCustomLabelsL( iIterate->NextL() );
		}
	else if( run == KRun3 )
		{
		INFO_PRINTF1(KTest3);
		TestCustomUidLabelsL(iIterate->NextL(), EFalse);
		}
	else if( run == KRun4 )
		{
		INFO_PRINTF1(KTest4);
		TestCustomUidLabelsL(iIterate->NextL(), ETrue);
		}
	else if( run == KRun5 )
		{
		INFO_PRINTF1(KTest5);
		DuplicateUidsTestL( iIterate->NextL() );
		}
	else if( run == KRun6 )
		{
		INFO_PRINTF1(KTest6);
		ExistingUidsTestL( EDuplicateUids );
		}
	else if( run == KRun7 )
		{
		INFO_PRINTF1(KTest7);
		ExistingUidsTestL( EUniqueUids );
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
Test: opens contact item, and adds a custom uid to each field, 
then checks if uid was stored correctly
*/
void CCustomLabelUid::TestCustomUidsL( const TContactItemId aCid )
	{
	//open contact	
	CContactItem *contactItem = NULL;
	contactItem = iContactsDatabase->OpenContactLX(aCid,*iViewAll);
	CleanupStack::PushL(contactItem);
	CContactItemFieldSet *ContactFields = &(contactItem->CardFields());
	
	//add an extra (custom) uid to each fieldtype of this contact
	AddUIDsL(*ContactFields,TestConstants::KInitialUID);

	//populate contact fields
	SetContactFieldsL(*contactItem);
	//update contact
	iContactsDatabase->CommitContactL(*contactItem);
	
	CleanupStack::PopAndDestroy(contactItem);
	CleanupStack::Pop();//lock
	contactItem = NULL;
	
	//read contact item
	contactItem = iContactsDatabase->ReadContactLC(aCid,*iViewAll);
	ContactFields = &(contactItem->CardFields());
	//check that contact field uids updated correctly
	SINGLECOMPARE( CheckUIDsL(*ContactFields,TestConstants::KInitialUID), 0 ,0 );
	//check that contact fields updated correctly
	SINGLECOMPARE( CheckContactFieldsL(*contactItem), 0, 0 );
	
	CleanupStack::PopAndDestroy(contactItem);
	}


/**
Test: opens contact item, add two identical uids to each field, 
alternating between custom and existing uids, 
then checks if uids were stored correctly
*/
void CCustomLabelUid::DuplicateUidsTestL( const TContactItemId aCid )
	{	
	//open contact	
	CContactItem *contactItem = NULL;
	contactItem = iContactsDatabase->OpenContactLX(aCid,*iViewAll);
	CleanupStack::PushL(contactItem);
	CContactItemFieldSet *contactFields = &(contactItem->CardFields());
	
	//add two identical uids to each field, alternating between custom and existing uids
	const TInt KFieldsCount = contactFields->Count();
	TInt error = KErrNone;


	//add duplicate custom uids
	CContactItemField *contactField1 = CContactItemField::NewL(KStorageTypeText);
	CleanupStack::PushL(contactField1);
	contactField1->SetTemplateField(EFalse);
	TRAP( error, contactField1->AddFieldTypeL( TFieldType::Uid(TestConstants::KInitialUID) ));
	INTCOMPARE( KErrNone, ==, error, 0, 0 );
	TRAP( error, contactField1->AddFieldTypeL( TFieldType::Uid(TestConstants::KInitialUID) ));
	INTCOMPARE( KErrNone, ==, error, 0, 0 );
	contactItem->AddFieldL(*contactField1);
	CleanupStack::Pop(contactField1);
	
	//add duplicate existing uids
	CContactItemField *contactField2 = CContactItemField::NewL(KStorageTypeText);
	CleanupStack::PushL(contactField2);
	contactField2->SetTemplateField(EFalse);
	TRAP( error, contactField2->AddFieldTypeL(KUidContactFieldAdditionalName));
	INTCOMPARE( KErrNone, ==, error, 1, 0 );
	TRAP( error, contactField2->AddFieldTypeL(KUidContactFieldAdditionalName));
	INTCOMPARE( KErrNone, ==, error, 1, 0 );
	contactItem->AddFieldL(*contactField2);
	CleanupStack::Pop(contactField2);


	//populate contact fields
	SetContactFieldsL(*contactItem);
	//update contact
	iContactsDatabase->CommitContactL(*contactItem);
	
	
	CleanupStack::PopAndDestroy(contactItem);
	CleanupStack::Pop();//lock
	contactItem = NULL;
	
	//read contact item
	contactItem = iContactsDatabase->ReadContactLC(aCid,*iViewAll);
	contactFields = &(contactItem->CardFields());
	//check that contact field uids updated correctly
	
	const TInt KReadFieldsCount = contactFields->Count();
	const CContentType *KContentType = NULL;
	INTCOMPARE( KReadFieldsCount, >, 0, 0, 0 );

	KContentType = &(*contactFields)[KFieldsCount].ContentType();
	SINGLECOMPARE( KContentType->ContainsFieldType( TFieldType::Uid(TestConstants::KInitialUID) ), KFieldsCount,0 );
	
	KContentType = &(*contactFields)[KFieldsCount+1].ContentType();
	SINGLECOMPARE( KContentType->ContainsFieldType(KUidContactFieldAdditionalName), KFieldsCount+1,0 );

	//check that contact fields updated correctly
	SINGLECOMPARE( CheckContactFieldsL(*contactItem), 0, 0 );

	CleanupStack::PopAndDestroy(contactItem);
	}

/**
Add new contact
Add new uid to each field that doesnt already contain that uid (EUniqueUids)
Add new uid to each field that already contains that uid (EDuplicateUids)
populate contact
set custom labels
check that all fields contain the additional uid (EUniqueUids)
check that contact was populated correctly
check that field lables were updated correctly
*/
void CCustomLabelUid::ExistingUidsTestL( const TUidTypeTest aTestType )
	{	
	/* Limitations (exceptions) to Test
	It is not possible to add Uids to all fields and expect to store and re-read them.
	This is because:
	i) Template matching (CContactItemFieldSet::MatchTemplateField) means that if a field matches a 
	template field, the additional Uids are lost
	ii) Fast access fields must be text fields
	iii) Other curious combinations of Uids seem to never have been supported (e.g. Unused Name and Additional Name)
	*/
	const TUid KFastAccessFieldUids[] = 
		{
		KUidContactFieldGivenNameValue, 
		KUidContactFieldFamilyNameValue,
		KUidContactFieldCompanyNameValue,
		KUidContactFieldGivenNamePronunciationValue,
		KUidContactFieldFamilyNamePronunciationValue,
		KUidContactFieldCompanyNamePronunciationValue
		};
	
	const TInt KUidsCount = iExistingUidsArray->Count();
	CContactCard* contact = CContactCard::NewLC(iTemplate);
	
	for(TInt i = 0; i < KUidsCount; ++i ) 
		{
		const TFieldType KFieldType = TFieldType::Uid( (*iExistingUidsArray)[i] );
				
		TBool found = EFalse;
		for (TInt nameFieldNum = 0; nameFieldNum < sizeof(KFastAccessFieldUids) / sizeof(TInt); ++nameFieldNum)
			{
			if (KFieldType == (KFastAccessFieldUids[nameFieldNum]))
				{
				found = ETrue;
				}	
			}
			
		found = found || KFieldType == KUidContactFieldVCardMapSIPID;
		found = found || KFieldType == KUidContactFieldTemplateLabel;
		found = found || KFieldType == KUidContactFieldVCardMapUnusedN;
		found = found || KFieldType == KUidContactFieldVCardMapTEL;
		
		if (found)
			{
			continue;	
			}
		
		TContactItemId contactId = KNullContactId;
		contactId = iContactsDatabase->AddNewContactL( *contact );
		
		//open contact	
		CContactItem *contactItem = NULL;
		contactItem = iContactsDatabase->OpenContactLX(contactId,*iViewAll);
		CleanupStack::PushL(contactItem);
		CContactItemFieldSet &contactFields = contactItem->CardFields();
		
		const TInt KFieldsCount = contactFields.Count();
		
		for( TInt j = 0; j < KFieldsCount; ++j )
			{		
			const CContentType &KContentType = contactFields[j].ContentType();
			
			// don't add phone/email/SIP UIDs to non-text fields
			if ((KFieldType == KUidContactFieldEMail ||
				KFieldType == KUidContactFieldPhoneNumber ||
				KFieldType == KUidContactFieldFax ||
				KFieldType == KUidContactFieldSms) && contactFields[j].StorageType() != KStorageTypeText)
				{
				continue;	
				}		
			
			//if testing duplicate uids and uid is present in contact field add uid
			// if testing unique uids and uid is not presnt in contact field add uid
			if( (aTestType == EDuplicateUids) && KContentType.ContainsFieldType( KFieldType ) )
				{
				contactFields[j].AddFieldTypeL( KFieldType );
				}
			else if( (aTestType == EUniqueUids) && !KContentType.ContainsFieldType( KFieldType ) )	
				{
				contactFields[j].AddFieldTypeL( KFieldType );
				}
				
			}
		
		//populate contact fields
		SetContactFieldsL(*contactItem);
		//setlabels of contact field to custom values
		SetContactLabelsL(*contactItem);
		
		iContactsDatabase->CommitContactL(*contactItem);
		CleanupStack::PopAndDestroy(contactItem);
		CleanupStack::Pop();//lock		
		
		//read contact item
		contactItem = iContactsDatabase->ReadContactLC(contactId,*iViewAll);
		CContactItemFieldSet &readContactFields = contactItem->CardFields();	
		
		//check that contact field uids updated correctly
		const TInt KReadFieldsCount = readContactFields.Count();
		INTCOMPARE( KReadFieldsCount, >, 0, i, 0 );
		
		if( (aTestType == EUniqueUids) )
			{
			//check that each fields contains at least one instance of the existing uid
			// (except the fast access fields)
			for( TInt k = 0; k < KReadFieldsCount; ++k )
				{
				TBool found = EFalse;
				const CContentType &KContentType = readContactFields[k].ContentType();
			
				for (TInt nameFieldNum = 0; nameFieldNum < sizeof(KFastAccessFieldUids) / sizeof(TInt); ++nameFieldNum)
				{
				if (KContentType.ContainsFieldType(KFastAccessFieldUids[nameFieldNum]))
				    {
					found = ETrue;
					}
				}
	
				if (found)
					{
					continue;	
					}
				
				// The behaviour of CContactItemFieldSet::MatchTemplateField has changed
				// add an extra UID in this case to avoid a template match (and loss of the added UID)
				if (KContentType.ContainsFieldType(KUidContactFieldVCardMapSIPID))
					{
					continue;
					}

				if (KContentType.ContainsFieldType(KUidContactFieldVCardMapTEL))
					{
					continue;
					}
		
				// some combinations of UID to Field Type are not supported		
				if (KContentType.ContainsFieldType(KUidContactFieldVCardMapUnusedN) 
				    && (KFieldType == KUidContactFieldAdditionalName || 
				        KFieldType == KUidContactFieldSuffixName ||  
				        KFieldType == KUidContactFieldPrefixName))		
					{
					continue;
					}	
								
				// don't check phone/email/SIP UIDs to non-text fields
				if ((KFieldType == KUidContactFieldEMail ||
					KFieldType == KUidContactFieldPhoneNumber ||
					KFieldType == KUidContactFieldFax ||
					KFieldType == KUidContactFieldSms) && contactFields[k].StorageType() != KStorageTypeText)
					{
					continue;	
					}						
				SINGLECOMPARE( KContentType.ContainsFieldType( KFieldType ), i, k );
				}
			}
			
		//check that contact fields updated correctly
		SINGLECOMPARE( CheckContactFieldsL(*contactItem),i,0 );
		
		//check that contact field labels were updated correctly
		SINGLECOMPARE( CheckContactLabelsL(*contactItem),i,0 );
		
		CleanupStack::PopAndDestroy(contactItem);	
		iContactsDatabase->DeleteContactL( contactId );
		}
	CleanupStack::PopAndDestroy( contact );
	}

	
/**
opens contact item, and changes the label of each field to a custom label, then checks 
if label was stored correctly
*/
void CCustomLabelUid::TestCustomLabelsL( const TContactItemId aCid )
	{
	//open contact item
	CContactItem *contactItem = NULL;
	contactItem = iContactsDatabase->OpenContactLX(aCid,*iViewAll);
	CleanupStack::PushL(contactItem);
	
	//setlabels of contact field to custom values
	SetContactLabelsL(*contactItem);
	//populate cotnact fields and update
	SetContactFieldsL(*contactItem);
	iContactsDatabase->CommitContactL(*contactItem);
	
	CleanupStack::PopAndDestroy(contactItem);
	CleanupStack::Pop();//lock record
	
	//read contact
	contactItem = NULL;
	contactItem = iContactsDatabase->ReadContactLC(aCid,*iViewAll);
	
	//check that contact field labels were updated correctly
	SINGLECOMPARE( CheckContactLabelsL(*contactItem),0,0 );
	
	//check that fields updated correctly
	SINGLECOMPARE( CheckContactFieldsL(*contactItem),0,0 );
	
	CleanupStack::PopAndDestroy(contactItem);
	}
	
/**
this function encompasses two tests
Test1(!aNewFields) opens contact item, and adds a custom uid to each field, changes lable to custom label
then checks if uid and label were stored correctly
Test2(aNewFields) opens contact item and adds ten new fields, and adds a custom uid to 
each field(new and old), changes lable to custom label then checks if uid and 
label were stored correctly. also checks both uids of new fields
*/
void CCustomLabelUid::TestCustomUidLabelsL( const TContactItemId aCid, const TBool aNewFields )
	{
	CContactItem *contactItem = NULL;
	//open contact
	contactItem = iContactsDatabase->OpenContactLX(aCid,*iViewAll);
	CleanupStack::PushL(contactItem);
	CContactItemFieldSet *ContactFields = &(contactItem->CardFields());
		
	if(aNewFields)
		{
		TInt length = 10;//add 10 new custom fields
		for(TInt i = 0, uids = TestConstants::KAltUID; i < length; i++, uids++)
			{
			AddNewFieldL( *ContactFields, TestConstants::KShortString, TFieldType::Uid(uids) );
			}
		}
	//set custom labels for contact fields	
	SetContactLabelsL(*contactItem);
	//set custom uids for contact fields
	AddUIDsL(*ContactFields,TestConstants::KInitialUID);
	//populate contact fields and update
	SetContactFieldsL(*contactItem);
	iContactsDatabase->CommitContactL(*contactItem);
	
	CleanupStack::PopAndDestroy(contactItem);
	CleanupStack::Pop();//lock
	
	//read contact
	contactItem = NULL;
	contactItem = iContactsDatabase->ReadContactLC(aCid,*iViewAll);
	ContactFields = &(contactItem->CardFields());
	
	//check uids were update correctly
	SINGLECOMPARE( CheckUIDsL(*ContactFields,TestConstants::KInitialUID),0,0 );
	//check labels were update correctly
	SINGLECOMPARE( CheckContactLabelsL(*contactItem),0,0 );
	//check fields were populated correctly
	SINGLECOMPARE( CheckContactFieldsL(*contactItem),0,0 );
	
	if(aNewFields)
		{
		//check that new fields contain the correct uids(2), initial and updated custom values
		TInt length = ContactFields->Count();
		TInt i = ContactFields->Find( TFieldType::Uid(TestConstants::KAltUID) );
		for(TInt uids = TestConstants::KAltUID; i < length; i++, uids++)
			{
			SINGLECOMPARE( CheckNewFieldL( *ContactFields, i, TFieldType::Uid(uids) ), i, 0);
			}
		}		
	CleanupStack::PopAndDestroy(contactItem);
	}


