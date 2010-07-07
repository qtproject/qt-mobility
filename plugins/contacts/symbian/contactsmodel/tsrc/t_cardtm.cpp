/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include <cntmodel.rsg>
#include "t_utils.h"

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_CARDTM"));

const TPtrC KDatabaseFileName=_L("C:T_CARDTM");
const TPtrC KTestName=_L("Test Name No%d");
const TPtrC KTestAddress=_L("Address %d");
const TPtrC KTestLabel = _L("Test label No %d");
const TPtrC KTestEmail = _L("Test@Email.com No %d");
const TPtrC KTestCountry = _L("Test Country No %d");
const TPtrC KTestCompany = _L("Test Company No %d");
const TPtrC KTestFamilyName=_L("Test Family Name No %d");
const TInt KTotalNumRecords=50;


#define KCardGivenName _L("Given Name #%d")
#define KCardFamilyName _L("Family Name #%d")
#define KCardFieldText _L("Card id %d, text field #%d")
#define KCardFieldEditedText _L("UPDATED Card id %d, text field #%d")
#define KTestData _L("test data")



/** 
 * Added to understand what support Contacts provides for user-defined templates...
 */
/*
void TestCardTemplateCreationL(CContactDatabase& aDb)
	{
	_LIT(KCardTemplateName,"CContactCardTemplate");
	CContactItem* temp = aDb.CreateContactCardTemplateLC(KCardTemplateName);
	TContactItemId templateId = temp->Id();
	TContactItemId templateRefId = temp->TemplateRefId();
	__ASSERT_ALWAYS(templateRefId==KNullContactId, User::Invariant()); //templates must not reference other cards
	__ASSERT_ALWAYS(temp->Type()==KUidContactCardTemplate, User::Invariant()); //correct type?
	CContactItemFieldSet& fieldSet = temp->CardFields();
	TInt fieldSetCount = fieldSet.Count();
	TInt labelPosition = fieldSet.Find(KUidContactFieldTemplateLabel);
	__ASSERT_ALWAYS(labelPosition!=KErrNotFound, User::Invariant());
	CContactItemField& labelField = fieldSet[labelPosition];
	__ASSERT_ALWAYS(labelField.TextStorage()->Text()==KCardTemplateName, User::Invariant());
	

	//Create a card based on the template
	CContactItem* card = CContactCard::NewLC(temp);
	TContactItemId cardTemplateRefId = card->TemplateRefId();
	__ASSERT_ALWAYS(cardTemplateRefId==templateId, User::Invariant()); 
	__ASSERT_ALWAYS(card->Type()==KUidContactCard, User::Invariant());
	CContactItemFieldSet& cardFieldSet = card->CardFields();
	TInt cardFieldSetCount = cardFieldSet.Count();
	TInt cardLabelPosition = cardFieldSet.Find(KUidContactFieldTemplateLabel);
	__ASSERT_ALWAYS(cardLabelPosition==KErrNotFound, User::Invariant());

	TContactItemId cardId = aDb.AddNewContactL(*card);
	CleanupStack::PopAndDestroy(card);
	
	CleanupStack::PopAndDestroy(temp);


	//Modify the template - does the card get the changes?
	CContactItem* modifiedTemp = aDb.OpenContactLX(templateId);
	CleanupStack::PushL(modifiedTemp);	
	const TInt fieldCount = modifiedTemp->CardFields().Count();
	for(TInt i=fieldCount-1;i>=0;i--)
		temp->RemoveField(i);
		
	CContactItemField* name = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName);
	name->SetMapping(KUidContactFieldVCardMapUnusedN);
	modifiedTemp->AddFieldL(*name);
	CleanupStack::Pop(name);

	CContactItemField* number = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
	number->SetMapping(KUidContactFieldVCardMapTEL);
	modifiedTemp->AddFieldL(*number);
	CleanupStack::Pop(number);
			
	CContactItemField* slotnum = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldICCSlot);
	modifiedTemp->AddFieldL(*slotnum);
	CleanupStack::Pop(slotnum);

	aDb.CommitContactL(*modifiedTemp);
	CleanupStack::PopAndDestroy(2);	// temp, close template

	CContactItem* fetchedCard = aDb.ReadContactLC(cardId);
	TInt cardFieldsCount = fetchedCard->CardFields().Count();
	CleanupStack::PopAndDestroy(fetchedCard);	


	//Add a new contact with the modified template
	CContactItem* fetchedTemplate = aDb.ReadContactLC(templateId);
	CContactItem* newCard = CContactCard::NewLC(fetchedTemplate);
	TInt newCardFieldsCount = newCard->CardFields().Count();
	
	TInt pos = newCard->CardFields().Find(KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN);
	if (pos!=KErrNotFound)
		{
		CContactItemField& field=fieldSet[pos];
		field.TextStorage()->SetTextL(_L("Chris"));
		}
	TContactItemId id = aDb.AddNewContactL(*newCard);
	CleanupStack::PopAndDestroy(newCard);	
	
	CContactItem* it = aDb.ReadContactLC(id);
	TInt fit = it->CardFields().Count();
	CleanupStack::PopAndDestroy(it);	
	
	

	//ICC contacts then..
	CContactItem* iccCard = CContactICCEntry::NewL(*fetchedTemplate);
	CleanupStack::PushL(iccCard);
	TInt iccFieldsCount = iccCard->CardFields().Count();
	
	TInt p = iccCard->CardFields().Find(KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN);
	if (p!=KErrNotFound)
		{
		CContactItemField& field=fieldSet[p];
		field.TextStorage()->SetTextL(_L("Chris"));
		}
	TContactItemId iccId = aDb.AddNewContactL(*iccCard);
	CleanupStack::PopAndDestroy(iccCard);
	
	CContactItem* fetchedIcc = aDb.ReadContactLC(iccId);
	TInt fetchedIccCount = fetchedIcc->CardFields().Count();
	CleanupStack::PopAndDestroy(fetchedIcc);
	
	
	
	
	CleanupStack::PopAndDestroy(fetchedTemplate);
	}
*/







/////////////////////////////////////////////////////////////////
//
//	SUPPORT MODULES
//
/////////////////////////////////////////////////////////////////

LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName)
//
// Set the contents of a text field, creating the field if required
//
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	}


LOCAL_C void CompareFieldSetsL(CContactItem* aContact, CContactItem* aTemplate)
//
// check new card fields are the same as the templates
//
	{
	CContactItemFieldSet& contactFields = aContact->CardFields();
	CContactItemFieldSet& templateFields = aTemplate->CardFields();
	test(contactFields.Count() == (templateFields.Count() - 1)); 
	//-1 label field
	for (TInt jj=1;jj < (contactFields.Count()-1);jj++) 
		// need to start at 1 since label field wont exist in contact card
		{	
		CContactItemField& tempField = (templateFields)[jj];
		TBool exists = EFalse;
		for (TInt counter=0;counter<contactFields.Count();counter++)
			{
			CContactItemField& contField = (contactFields)[counter];
			if (contField.ContentType()==tempField.ContentType())
				{	
				exists=ETrue;
				break;
				}		
			}
		test(exists);
		}
	}

LOCAL_C void CompareFieldSetsWithTextL(CContactItem* aContact, CContactItem* aTemplate)
//
// check new card fields are the same as the templates
//
	{
	CContactItemFieldSet& contactFields = aContact->CardFields();
	CContactItemFieldSet& templateFields = aTemplate->CardFields();
	test(contactFields.Count() == (templateFields.Count()-1));
	//-1 label field
	for (TInt jj=1;jj < (contactFields.Count()-1);jj++)
		{	
		CContactItemField& tempField = (templateFields)[jj];
		TBool exists = EFalse;
		for (TInt counter=0;counter<contactFields.Count();counter++)
			{
			CContactItemField& contField = (contactFields)[counter];
			if (contField.ContentType()==tempField.ContentType())
				{	
				exists=ETrue;
				if (contField.StorageType()==KStorageTypeText)
					{
					exists=ETrue;
					break;
					}
				}		
			}
		test(exists);
		}
	}

LOCAL_C void PopulateDatabaseL(CContactItem& aTemplate, TInt aNumberToPopulate, TBool aPhoneNumbers, TBool aWithTime)
//
// Create and populate the database
//
	{
	TTime before=TTime();
	if (aWithTime)
		before.UniversalTime();
	for (TInt counter=0;counter<aNumberToPopulate;counter++)
		{
		CContactItem* item=CContactCard::NewLC(&aTemplate);
		TBuf<16> name;
		name.Format(KTestName,counter);
   		SetNameL(*item,KUidContactFieldGivenName,name);
		if (aPhoneNumbers)
			{
			TBuf<20> number;
			switch(counter%3)
				{
				case 0:
					number.Format(_L("0171-%03d %04d"),(counter*9)%1000,((counter+11)*23)%10000);
					break;
				case 1:
					number.Format(_L("%04d:%04d:%04d:%04d"),(counter*123)%10000,(counter*666)%10000,(counter*234)%10000);
					break;
				case 2:
					number.Format(_L("+00%d-%03d %04d"),(counter*123)%100,(counter*13)%1000,((counter+13)*17)%10000);
					break;
				}
   			SetNameL(*item,KUidContactFieldPhoneNumber,number);
			if (!(counter%2))
				{
				number.Format(_L("0181-%03d %04d"),(counter*8)%1000,((counter+11)*22)%10000);
	   			SetNameL(*item,KUidContactFieldPhoneNumber,number);
				number.Format(_L("01734-%06d"),(counter*123456)%1000000);
	   			SetNameL(*item,KUidContactFieldPhoneNumber,number);
				}
			}
		TBuf<32> address;
		address.Format(KTestAddress,counter);
		SetNameL(*item,KUidContactFieldAddress,address);
        //
		TBuf<32> familyName;
		familyName.Format(KTestFamilyName,counter);
		SetNameL(*item,KUidContactFieldFamilyName,familyName);
        //
		TBuf<32> email;
		email.Format(KTestEmail,counter);
		SetNameL(*item,KUidContactFieldEMail,email);
        //
		TBuf<32> country;
		country.Format(KTestCountry,counter);
		SetNameL(*item,KUidContactFieldCountry,country);
        //	
		TBuf<32> company;
		company.Format(KTestCompany,counter);
		SetNameL(*item,KUidContactFieldCompanyName,company);
        //	
		CContactItemFieldSet& itemFields = item->CardFields();
		for (TInt counter2=0;counter2<itemFields.Count();counter2++)
			{
			if (!itemFields[counter2].Storage()->IsFull() && itemFields[counter2].StorageType()==KStorageTypeText)
				itemFields[counter2].TextStorage()->SetTextL(KTestData);
			}
		CntTest->Db()->AddNewContactL(*item); //templ
		CleanupStack::PopAndDestroy(); // item
        //	
		if (counter%100==0 && aWithTime)
			test.Printf(_L("."));	// Just to show some life

		}
	CntTest->Db()->SetDateFormatTextL(_L("%E%D%X%N%Y %1 %2 %3"));
	CntTest->Db()->CompactL();
	if (aWithTime)
		{
		TTime after;
		after.UniversalTime();
		TTimeIntervalSeconds secondsTaken;
		after.SecondsFrom(before,secondsTaken);
		test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
		test.Printf(_L("\n"));	
		}
	}

/////////////////////////////////////////////////////////////////
//
//	TEST MODULES
//
/////////////////////////////////////////////////////////////////


LOCAL_C void CreateCardTemplateL()
	{
	test.Next(_L("Create Card Template"));

	//copy 'golden' template
	CContactItem* tempTemplate = CntTest->Db()->CreateContactCardTemplateL(_L("label1"));
	CleanupStack::PushL(tempTemplate);

	// create template with more fields than the golden template
	CContactItemField* field1=CContactItemField::NewLC(KStorageTypeText,TUid::Uid(1));
   	field1->SetMapping(KUidContactFieldVCardMapUnusedN);
	tempTemplate->AddFieldL(*field1);
	CContactItemField* field2=CContactItemField::NewLC(KStorageTypeText,TUid::Uid(2));
   	field2->SetMapping(KUidContactFieldVCardMapUnusedN);
	tempTemplate->AddFieldL(*field2);
	CleanupStack::Pop(2); // field1,2
	CContactItem* secondTempTemplate = CntTest->Db()->CreateContactCardTemplateL(tempTemplate,_L("TEMPLATE2"));
	CleanupStack::PushL(secondTempTemplate);

	// create template with less fields than the golden template
	CContactItemFieldSet& tempFields = tempTemplate->CardFields();
	TInt fieldCount=tempFields.Count();
	TInt index = fieldCount-1;
	TInt counter = 0;
	
	while(counter < 12)
	    {
	    if(!tempFields[index].IsHidden())
	        {
	        //delete visible fields only as the tests only test againest the visible fields. 
    		tempTemplate->RemoveField(index);
    		++counter;
	        }
	    --index;
	    }
		
	CContactItem* thirdTempTemplate = CntTest->Db()->CreateContactCardTemplateL(tempTemplate,_L("TEMPLATE3"));
	CleanupStack::PushL(thirdTempTemplate);

	// create template with just the label field
	tempFields.Reset();
	CContactItem* fourthTempTemplate = CntTest->Db()->CreateContactCardTemplateL(tempTemplate,_L("TEMPLATE4"));
	
	CleanupStack::PopAndDestroy(3); // thirdTempTemplate secondTempTemplate tempTemplate
	delete fourthTempTemplate;
	}


LOCAL_C void CheckCardTemplateListPersistL()
	{
	test.Next(_L("Check Card Template LIST Persistance"));


	// get golden template
	TContactItemId goldenTemplateId = CntTest->Db()->TemplateId();
	CContactItem* goldenTemplate = CntTest->Db()->ReadContactLC(goldenTemplateId);
	CContactItemFieldSet& goldenTemplateFields = goldenTemplate->CardFields();
	TInt originalCount = goldenTemplateFields.Count();
	CleanupStack::PopAndDestroy(goldenTemplate);

	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	test(templateCount == 4);

	// Check Template definitions created by CreateCardTemplateL()
		{
		CContactItem* goldCopyTemp = CntTest->Db()->ReadContactLC((*tempIdList)[0]);
		CContactItemFieldSet& goldenCopyFields = goldCopyTemp->CardFields();
		// original + 1 (+ template label field)
		test(goldenCopyFields.Count() == originalCount + 1);
		CleanupStack::PopAndDestroy(goldCopyTemp);
		}

		{
		CContactItem* largeCopyTemp = CntTest->Db()->ReadContactLC((*tempIdList)[1]);
		CContactItemFieldSet& largeCopyFields = largeCopyTemp->CardFields();
		// original + 3 (+ template label field +2 UIDs)
		test(largeCopyFields.Count() == originalCount + 3);
		CleanupStack::PopAndDestroy(largeCopyTemp);
		}

		{
		CContactItem* smallCopyTemp = CntTest->Db()->ReadContactLC((*tempIdList)[2]);
		CContactItemFieldSet& smallCopyFields = smallCopyTemp->CardFields();
		// (original + template label field + 2 UIDs - 12 fields from end of template) == originalCount - 9
		test(smallCopyFields.Count() == originalCount - 9);
		CleanupStack::PopAndDestroy(smallCopyTemp);
		}

		{
		CContactItem* singleFieldCopyTemp = CntTest->Db()->ReadContactLC((*tempIdList)[3]);
		CContactItemFieldSet& singleCopyFields = singleFieldCopyTemp->CardFields();
		// template just has template label
		test(singleCopyFields.Count() == 1);
		CleanupStack::PopAndDestroy(singleFieldCopyTemp);
		}

	CleanupStack::PopAndDestroy(tempIdList);
	}	

LOCAL_C void CreateEmptyContactsFromTemplateL()
//
// create empty contact cards using all templates
//
	{
	test.Next(_L("Create Empty Contacts using Templates"));

//
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
 		CContactItem* tempTemplate = CntTest->Db()->OpenContactL(templateId);
        //
		CleanupStack::PushL(tempTemplate);
		for(TInt jj=0; jj<5; jj++)
			{
			CContactItem* newContact = CContactCard::NewL(tempTemplate);
			CleanupStack::PushL(newContact);
			CntTest->Db()->AddNewContactL(*newContact);
			CleanupStack::PopAndDestroy(); // newContact
			}
		CleanupStack::PopAndDestroy(); // tempTemplate
		CntTest->Db()->CloseContactL(templateId);
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void CheckEmptyContactsFromTemplatePersistL()
    //
    // check empty card template matching persist
    //
	{
	test.Next(_L("Check Empty Contacts Match Templates & Persist"));

    
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	TInt jj=5;
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
		CContactItem* tempTemplate = CntTest->Db()->OpenContactL(templateId);
		CleanupStack::PushL(tempTemplate);
    
		CContactItem* newContact = CntTest->Db()->OpenContactL(jj);
		CleanupStack::PushL(newContact);
		CompareFieldSetsL(newContact, tempTemplate);
		CntTest->Db()->CloseContactL(tempTemplate->Id());
		CntTest->Db()->CloseContactL(newContact->Id());
		CleanupStack::PopAndDestroy(2); // newContact tempTemplate
		jj+=5;
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}

LOCAL_C void CreateFullContactsFromTemplateL()
    //
    // create FULL contact cards using all templates
    //
	{
	test.Next(_L("Create FULL Contacts using Templates"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
 		CContactItem* tempTemplate = CntTest->Db()->ReadContactL(templateId);
    //
		CleanupStack::PushL(tempTemplate);
		PopulateDatabaseL(*tempTemplate, 5, ETrue, EFalse);
		CleanupStack::PopAndDestroy(); // tempTemplate
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void CheckFullContactsFromTemplatePersistL()
    //
    // check FULL card template matching persist
    //
	{
	test.Next(_L("Check FULL Contacts Match Templates & Persist"));

    
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	TInt jj=25;
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
		CContactItem* tempTemplate = CntTest->Db()->ReadContactL(templateId);
		CleanupStack::PushL(tempTemplate);
		CContactItem* newContact = CntTest->Db()->ReadContactL(jj);
		CleanupStack::PushL(newContact);
    
		CompareFieldSetsWithTextL(newContact, tempTemplate);
		CleanupStack::PopAndDestroy(2); // newContact tempTemplate
		jj+=5;
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void ModifyAddTemplateL()
    //
    // modify / add fields to templates
    //
	{
	test.Next(_L("Modify / add fields to templates"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
 		CContactItem* tempTemplate = CntTest->Db()->OpenContactL(templateId);
		CleanupStack::PushL(tempTemplate);
        //
		for(TInt jj=0; jj<3; jj++)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPrefixName);
	   		field->SetMapping(KUidContactFieldVCardMapUnusedN);
			tempTemplate->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		CntTest->Db()->CommitContactL(*tempTemplate);
		CntTest->Db()->CloseContactL(templateId);
        //
		CleanupStack::PopAndDestroy(); // tempTemplate
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void CheckModifyAddContactsFromTemplatePersistL()
    //
    // check modified card template match cards persist
    //
	{
	test.Next(_L("Check contacts Match modified Templates & Persist"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	TInt jj=25;
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
		CContactItem* tempTemplate = CntTest->Db()->ReadContactL(templateId);
		CleanupStack::PushL(tempTemplate);

		CContactItem* newContact = CntTest->Db()->ReadContactL(jj);
		CleanupStack::PushL(newContact);
		CompareFieldSetsL(newContact, tempTemplate);
		CleanupStack::PopAndDestroy(2); // newContact tempTemplate
		jj+=5;
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void ModifyRemoveTemplateL()
    //
    // modify / remove fields to templates
    //
	{
	test.Next(_L("Modify / REMOVE fields to templates"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	for (TInt counter=0; counter<templateCount-1; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
 		CContactItem* tempTemplate = CntTest->Db()->OpenContactL(templateId);
		CContactItemFieldSet& tempFieldSet = tempTemplate->CardFields();
		TInt fieldCount = tempFieldSet.Count();
		CleanupStack::PushL(tempTemplate);
        //
		for(TInt jj=1; jj<15; jj++)
			{
			tempTemplate->RemoveField(fieldCount-jj);
			}
		CntTest->Db()->CommitContactL(*tempTemplate);
		CntTest->Db()->CloseContactL(templateId);
        //
		CleanupStack::PopAndDestroy(); // tempTemplate
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}

LOCAL_C void CheckModifyRemoveContactsFromTemplatePersistL()
    //
    // check modified REMOVE card template match cards persist
    //
	{
	test.Next(_L("Check contacts Match modified Templates & Persist"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	TInt jj=5;
	// CHECK EMPTY CONTACTS FIRST 
	for (TInt counter=0; counter<templateCount-1; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
		CContactItem* tempTemplate = CntTest->Db()->ReadContactL(templateId);
		CleanupStack::PushL(tempTemplate);

		CContactItem* newContact = CntTest->Db()->ReadContactL(jj);
		CleanupStack::PushL(newContact);
		CompareFieldSetsL(newContact, tempTemplate);
		CleanupStack::PopAndDestroy(2); // newContact tempTemplate
		jj+=5;
		}
	jj=25;
	// check full contacts fields persists
	for (TInt counter2=0; counter2<templateCount-1; counter2++)
		{
		TContactItemId templateId = (*tempIdList)[counter2];
		CContactItem* tempTemplate = CntTest->Db()->ReadContactL(templateId);
		CleanupStack::PushL(tempTemplate);

		CContactItem* newContact = CntTest->Db()->ReadContactL(jj);
		CleanupStack::PushL(newContact);
	    //
		CContactItemFieldSet& templ = tempTemplate->CardFields();
		CContactItemFieldSet& item = newContact->CardFields();
		test(item.Count()>templ.Count());
		CleanupStack::PopAndDestroy(2); // newContact tempTemplate
		jj+=5;
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void SetTemplateLabelL()
    //
    // Set template fields label
    //
	{
	test.Next(_L("Modify field labels in templates"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
 		CContactItem* tempTemplate = CntTest->Db()->OpenContactL(templateId);
		CContactItemFieldSet& tempFieldSet = tempTemplate->CardFields();
		TInt fieldCount = tempFieldSet.Count();
		CleanupStack::PushL(tempTemplate);
        //
		for(TInt jj=0; jj<fieldCount; jj++)
			{
			(tempFieldSet)[jj].SetLabelL(KTestLabel);
			}
		CntTest->Db()->CommitContactL(*tempTemplate);
		CntTest->Db()->CloseContactL(templateId);
        //
		CleanupStack::PopAndDestroy(); // tempTemplate
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void CheckFieldLabelsPersistL()
    //
    // check modified field labels persist
    //
	{
	test.Next(_L("Check modified field labels Persist"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
		CContactItem* tempTemplate = CntTest->Db()->OpenContactL(templateId);
		CleanupStack::PushL(tempTemplate);
		CContactItemFieldSet& tempFieldSet = tempTemplate->CardFields();
		TInt fieldCount = tempFieldSet.Count();
        //
		for(TInt jj=0; jj<fieldCount; jj++)
			{
			test((tempFieldSet)[jj].Label() == KTestLabel);
			}
		CleanupStack::PopAndDestroy(); // tempTemplate
		CntTest->Db()->CloseContactL(templateId);
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	// !!!!!!! ADD CHECKING FOR CONTACT AS WELL.
	}


LOCAL_C void DeleteCardTemplateL()
    //
    // Delete Card Template
    //
	{
	test.Next(_L("Delete Card Template"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	test(tempIdList->Count() > 0);
	TContactItemId templateId = (*tempIdList)[1];

	CntTest->Db()->DeleteContactL(templateId);
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void CheckDeleteCardTemplateL()
    //
    // Check Delete Card Template
    //
	{
	test.Next(_L("Check Delete Card Template"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	test(templateCount==3);	
    //
	CContactItem* tempTemplate = CntTest->Db()->ReadContactL(11);
	CleanupStack::PushL(tempTemplate);
    //
	CleanupStack::PopAndDestroy(2); // tempTemplate tempIdList
	}


LOCAL_C void CheckLargePopulatedTemplate()
    //
    // create MANY FULL contact cards using all templates
    //
	{
	test.Next(_L("Create MANY FULL Contacts using Templates"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	for (TInt counter=0; counter<templateCount; counter++)
		{
		TContactItemId templateId = (*tempIdList)[counter];
 		CContactItem* tempTemplate = CntTest->Db()->ReadContactL(templateId);
		CleanupStack::PushL(tempTemplate);
        //
		PopulateDatabaseL(*tempTemplate, KTotalNumRecords, ETrue, ETrue);
		//PopulateDatabaseL(*tempTemplate, 1000, ETrue, ETrue);
		CleanupStack::PopAndDestroy(); // tempTemplate
		}
	CleanupStack::PopAndDestroy(); // tempIdList
	}

LOCAL_C void ModifyCardBasedOnTemplate()
    //
    // modify FULL contact cards based on template and check persist
    //
	{
	test.Next(_L("modify Contact Card & check persist"));


	CContactItemViewDef* itemDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields); 
	itemDef->AddL(KUidContactFieldMatchAll);

	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TContactItemId templateId = (*tempIdList)[0]; // first in the list
	CContactItem* tempTemplate = CntTest->Db()->ReadContactLC(templateId,*itemDef);

	CContactItem* newContactItem = CContactCard::NewLC(tempTemplate);
	CContactItemFieldSet& fieldSet = newContactItem->CardFields();
    //
	// fill up the contact
	TInt counter=0;
	for (;counter<fieldSet.Count();counter++)
		{
		if (!fieldSet[counter].Storage()->IsFull() && fieldSet[counter].StorageType()==KStorageTypeText)
			fieldSet[counter].TextStorage()->SetTextL(KTestData);
		}
		
	// add the new contact to the db
	TContactItemId newItemID = CntTest->Db()->AddNewContactL(*newContactItem);  
    //
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
    //
	CContactItem* newContactItem2 = CntTest->Db()->OpenContactLX(newItemID,*itemDef);  // gets the new contact from the db
	CleanupStack::PushL(newContactItem2);
	CContactItemFieldSet& fieldSet2 = newContactItem2->CardFields();
    //
	// check the same
	for (counter=0;counter<fieldSet2.Count();counter++)
		{
		if (fieldSet2[counter].StorageType()==KStorageTypeText)
			{
			TPtrC fieldText = fieldSet[counter].TextStorage()->Text();
			TPtrC fieldText2 = fieldSet2[counter].TextStorage()->Text();
			test.Printf(_L("\n Field %d = %S  ... %S "),counter,&fieldText,&fieldText2);	// Show what text is in field 3
			User::After(20000);
			//test.Getch();
			}
		}
			
	test.Printf(_L("\n"));
	test.Next(_L("Remove Field Data and Check Persist"));

	
	fieldSet2[4].TextStorage()->SetStandardTextL(_L(""));
	CntTest->Db()->CommitContactL(*newContactItem2);  // commit the changes

	CContactItem* newContactItem3 = CntTest->Db()->OpenContactLX(newItemID,*itemDef);  // gets the new contact from the db
	CleanupStack::PushL(newContactItem3);
	CContactItemFieldSet& fieldSet3 = newContactItem3->CardFields();
    //
	// check the same
	for (counter=0;counter<fieldSet3.Count();counter++)
		{
		if (fieldSet3[counter].StorageType()==KStorageTypeText)
			{
			TPtrC fieldText = fieldSet[counter].TextStorage()->Text();
			TPtrC fieldText2 = fieldSet2[counter].TextStorage()->Text();
			TPtrC fieldText3 = fieldSet3[counter].TextStorage()->Text();
			test.Printf(_L("\n Field %d = %S  ... %S  ... %S "),counter,&fieldText,&fieldText2,&fieldText3);	// Show what text is in field 3
			User::After(20000);
			//test.Getch();
			}
		}
	fieldSet3[5].TextStorage()->SetStandardTextL(_L(""));
	CntTest->Db()->CommitContactL(*newContactItem3);  // commit the changes

	CContactItem* newContactItem4 = CntTest->Db()->OpenContactLX(newItemID,*itemDef);  // gets the new contact from the db
	CleanupStack::PushL(newContactItem4);
	CContactItemFieldSet& fieldSet4 = newContactItem4->CardFields();
    //
	// check the same
	for (counter=0;counter<fieldSet4.Count();counter++)
		{
		if (fieldSet4[counter].StorageType()==KStorageTypeText)
			{
			TPtrC fieldText = fieldSet[counter].TextStorage()->Text();
			TPtrC fieldText2 = fieldSet2[counter].TextStorage()->Text();
			TPtrC fieldText3 = fieldSet3[counter].TextStorage()->Text();
			TPtrC fieldText4 = fieldSet4[counter].TextStorage()->Text();
			test.Printf(_L("\n Field %d = %S  ... %S  ... %S ... %S "),counter,&fieldText,&fieldText2,&fieldText3,&fieldText4);	// Show what text is in field 3
			User::After(20000);
			//st.Getch();
			}
		}
    //
	CntTest->Db()->CloseContactL(newContactItem4->Id());  // commit the changes
	CleanupStack::PopAndDestroy(10); // tempIdList tempTemplate openlx newContactItem2, itemDef
	}


LOCAL_C void DeleteTemplateCheckPersist()
	{
	test.Next(_L("Delete Template Check Persist"));
    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	for (TInt counter=0; counter<templateCount; counter++)
		{
		CntTest->Db()->DeleteContactL((*tempIdList)[counter]);
		}
    //
	for (TInt counter2=KTotalNumRecords-10; counter2<(KTotalNumRecords*templateCount); counter2+=KTotalNumRecords)
		{
		CContactItem* contact = CntTest->Db()->ReadContactL(counter2);
		delete contact;
		}

	CContactIdArray* tempIdList2 = CntTest->Db()->GetCardTemplateIdListL();
	TInt templateCount2 = tempIdList2->Count();
	test(templateCount2==0);
	delete tempIdList2;
	CleanupStack::PopAndDestroy(); // tempIdList
	}


LOCAL_C void CheckManyTemplates()
    //
    // create MANY templates
    //
	{
	test.Next(_L("Create MANY Templates"));

    //
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();

	TTime before;
	before.UniversalTime();
	for (TInt counter=0; counter<KTotalNumRecords; counter++)
		{
 		CContactItem* tempTemplate = CntTest->Db()->CreateContactCardTemplateL(_L("TEMPLATE"));
		delete tempTemplate;
		if (counter%100==0)
			test.Printf(_L("."));	// Just to show some life
		}
	test(CntTest->Db()->TemplateCount()==KTotalNumRecords);
	TTime after;
	after.UniversalTime();
	TTimeIntervalSeconds secondsTaken;
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	}


LOCAL_C void TestModifyCardAttribs()
    //
    // set attribs and check persist
    //
	{
	test.Next(_L("set attribs check persist"));
    ////////////////////////////////////////////////////////////////////
    // multiple template created card
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	TContactItemId templateId = (*tempIdList)[0]; // first in the list
	delete tempIdList;
	CContactItem* tempTemplate = CntTest->Db()->OpenContactLX(templateId);	
    /////////////////////////////////////////////////////////////////
    // golden template created card 

    //	CContactItem* tempTemplate = CntTest->Db()->OpenContactLX(0);
    ////////////////////////////////////////////////////////////////
	CleanupStack::PushL(tempTemplate);
	CContactItem* newContactItem = CContactCard::NewLC(tempTemplate);
	CntTest->Db()->CloseContactL(tempTemplate->Id());
	CContactItemFieldSet& fieldSet = newContactItem->CardFields();
    //
	// fill up the contact
	TInt counter=0;
	for (;counter<fieldSet.Count();counter++)
		{
		if (!fieldSet[counter].Storage()->IsFull() && fieldSet[counter].StorageType()==KStorageTypeText)
			fieldSet[counter].TextStorage()->SetTextL(KTestData);
		}		
	// add the new contact to the db
	TContactItemId newItemID = CntTest->Db()->AddNewContactL(*newContactItem);  
    //
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();

	CContactItem* newContactItem2 = CntTest->Db()->OpenContactLX(newItemID);
	CleanupStack::PushL(newContactItem2);
	CContactItemFieldSet& fieldSet2 = newContactItem2->CardFields();
    //
	for (counter=0;counter<fieldSet2.Count();counter++)
		{
		fieldSet2[counter].SetPrivate(ETrue);
		if (counter%2)
			{
			fieldSet2[counter].SetHidden(ETrue);
			test.Printf(_L("\n Field %d is SetHidden"),counter);
			User::After(20000);
			}
		else
			{
			fieldSet2[counter].SetReadOnly(ETrue);
			test.Printf(_L("\n Field %d is ReadOnly"),counter);
			User::After(20000);
			}
		}
	CntTest->Db()->CommitContactL(*newContactItem2);  // commit the changes		
	CleanupStack::PopAndDestroy(2); // newContactItem2 openlx

	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();

	test.Printf(_L("\n"));
    //	
	test.Next(_L("Check hidden Persists"));

	CContactItem* newContactItem3 = CntTest->Db()->OpenContactL(newItemID);  
	CleanupStack::PushL(newContactItem3);
	CContactItemFieldSet& fieldSet3 = newContactItem3->CardFields();
	// check the same
	for (counter=0;counter<fieldSet3.Count();counter++)
		{
		if (fieldSet3[counter].IsHidden())
			{
			if (fieldSet3[counter].IsPrivate())
				{
				test.Printf(_L("\n Field %d is Hidden & PRIVATE"),counter);
				User::After(20000);
				}
			else
				{
				test.Printf(_L("\n Field %d is Hidden"),counter);
				User::After(20000);
				}
			}
		if (fieldSet3[counter].IsReadOnly())
			{
			if (fieldSet3[counter].IsPrivate())
				{
				test.Printf(_L("\n Field %d is READ ONLY & PRIVATE"),counter);
				User::After(20000);
				}
			else
				{
				test.Printf(_L("\n Field %d is READ ONLY "),counter);
				User::After(20000);
				}
			}
		}
	CntTest->Db()->CloseContactL(newContactItem3->Id());  
	CleanupStack::PopAndDestroy(4); // newContactItem2 openlx
	}


LOCAL_C void PopulateUsingManyTemplates()
    //
    // create MANY templates
    //
	{
	test.Next(_L("Create Contacts from MANY Templates"));

    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();
	test(templateCount == KTotalNumRecords);
    //
	TTime before;
	before.UniversalTime();
	for (TInt counter=0; counter<KTotalNumRecords; counter++)
		{
 		CContactItem* tempTemplate = CntTest->Db()->ReadContactL((*tempIdList)[counter]);
		CleanupStack::PushL(tempTemplate);
		PopulateDatabaseL(*tempTemplate, 1, ETrue, EFalse);
		if (counter%100==0)
			test.Printf(_L("."));	// Just to show some life
		CleanupStack::PopAndDestroy(); // tempTemplate
		}
	TTime after;
	after.UniversalTime();
	TTimeIntervalSeconds secondsTaken;
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	CleanupStack::PopAndDestroy();  // tempIdList
	}


LOCAL_C void DeleteManyTemplates()
    //
    // Delete MANY templates
    //
	{
	test.Next(_L("Delete MANY Templates"));

    //
	CContactIdArray* tempIdList = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(tempIdList);
	TInt templateCount = tempIdList->Count();

	// -1 because of DeletePreferredTemplate()
	test(templateCount == KTotalNumRecords - 1);
    //
	TTime before;
	before.UniversalTime();

	// -1 because of DeletePreferredTemplate()
	for (TInt counter=0; counter<KTotalNumRecords-1; counter++)
		{
 		CntTest->Db()->DeleteContactL((*tempIdList)[counter]);
		if (counter%100==0)
			test.Printf(_L("."));	// Just to show some life
		}
	TTime after;
	after.UniversalTime();
	TTimeIntervalSeconds secondsTaken;
	after.SecondsFrom(before,secondsTaken);
	test.Printf(_L(" TIME: %d  Secs"),secondsTaken.Int());
	test.Printf(_L("\n"));	
	CContactIdArray* tempIdList2 = CntTest->Db()->GetCardTemplateIdListL();
	test(tempIdList2->Count()==0);
	delete tempIdList2;
	CleanupStack::PopAndDestroy();  // tempIdList
	}

LOCAL_C void SetTemplateName()
	{
//
	test.Next(_L("Set Template Label"));

	CContactIdArray* templateIds = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(templateIds);
	CContactItem* tempTemplate = CntTest->Db()->OpenContactL((*templateIds)[1]);	
	TContactItemId id = tempTemplate->Id();
	CleanupStack::PushL(tempTemplate);
	TBuf<32> label;
	label.Format(KTestLabel,99);
	STATIC_CAST(CContactCardTemplate*,tempTemplate)->SetTemplateLabelL(label);

	CntTest->Db()->CommitContactL(*tempTemplate);
	CntTest->Db()->CloseContactL(id);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();

	CContactItem* tempTemplate2 = CntTest->Db()->OpenContactL(id);
	TPtrC tempLabel = STATIC_CAST(CContactCardTemplate*,tempTemplate2)->GetTemplateLabelL();
	CntTest->Db()->CloseContactL(id);
	test(tempLabel==label);
	delete tempTemplate2;
	CleanupStack::PopAndDestroy(2);  // tempTemplate templateIds
	}


LOCAL_C void SetPreferredTemplate()
	{
	test.Next(_L("Set & Test Preferred Template"));

	CContactIdArray* templateIds = CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(templateIds);
    //
	TInt templateCount = templateIds->Count();
	test(templateCount == KTotalNumRecords);
    //
	TContactItemId prefTemplateId = CntTest->Db()->PrefTemplateId();
	test(prefTemplateId == KNullContactId);

    // set the preferred template 
	CContactItem* firstTemplate = CntTest->Db()->ReadContactLC((*templateIds)[0]);
	CntTest->Db()->SetPrefTemplateL(*firstTemplate);

	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	
	prefTemplateId = CntTest->Db()->PrefTemplateId();
	test(prefTemplateId == firstTemplate->Id());
    //
	CleanupStack::PopAndDestroy(2, templateIds); // firstTemplate;
	}


LOCAL_C void SetBadPreferredTemplate()
	{
	// this function is TRAPD
	test.Next(_L("Set BAD pref Template"));
	CContactItem* item=CContactCard::NewLC();
	TBuf<16> name;
	name.Format(KTestName,1);
   	SetNameL(*item,KUidContactFieldGivenName,name);
	CntTest->Db()->AddNewContactL(*item);
	CntTest->Db()->SetPrefTemplateL(*item);
	// should leave with KErrNotSupported
	}

LOCAL_C void DeletePreferredTemplate()
	{
	// this function is TRAPD
	test.Next(_L("Delete pref Template"));


	// get the template id
	TContactItemId prefTemplateId = CntTest->Db()->PrefTemplateId();
	test(prefTemplateId != KNullContactId);

	// delete the preferred template
	CntTest->Db()->DeleteContactL(prefTemplateId);
	

	// retest the preftempId
	prefTemplateId = CntTest->Db()->PrefTemplateId();
	// and ensure it has been reset to null.
	test(prefTemplateId == KNullContactId);

	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();

	prefTemplateId = CntTest->Db()->PrefTemplateId();
	test(prefTemplateId == KNullContactId);
	}


LOCAL_C void DeleteAll()
	{
	// creates a new database - adds loads of contacts based on the golden template
	// then deletes everything, then creates loads of contacts based on a cardtemplate
	// then deletes everything.

	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	test.Next(_L("Delete All based on Golden"));

    // get the golden template
	CContactItem* goldenTemplate = CntTest->Db()->OpenContactLX(0);
	CleanupStack::PushL(goldenTemplate); // 2 things on the stack

	TInt i=0;
	for (; i<KTotalNumRecords; i++)
		{
		CContactItem* newItem = CContactCard::NewLC(goldenTemplate);
		CntTest->Db()->AddNewContactL(*newItem);
		CleanupStack::PopAndDestroy(); //newItem
		}
	CntTest->Db()->CloseContactL(0);
	const CContactIdArray* sortedItems = CntTest->Db()->SortedItemsL();

	// delete the all entries in the db.
	TInt dbCount = sortedItems->Count();

	for (i=dbCount-1; i > -1; i--)
		{
		CntTest->Db()->DeleteContactL((*sortedItems)[i]);
		}
	
	dbCount = CntTest->Db()->CountL();

	// now wit multi template
	CContactItem* tempTemplate = CntTest->Db()->CreateContactCardTemplateL(_L("label1"));
	CleanupStack::PushL(tempTemplate);

	for (i=0; i<KTotalNumRecords; i++)
		{
		CContactItem* newItem = CContactCard::NewLC(tempTemplate);
		CntTest->Db()->AddNewContactL(*newItem);
		CleanupStack::PopAndDestroy(); //newItem
		}
	CntTest->Db()->CloseContactL(0);
	const CContactIdArray* sortedItems2 = CntTest->Db()->SortedItemsL();

	// delete the all entries in the db.
	dbCount = sortedItems2->Count();

	for (i=dbCount-1; i > -1; i--)
		{
		CntTest->Db()->DeleteContactL((*sortedItems2)[i]);
		}
	
	dbCount = CntTest->Db()->CountL();

	CleanupStack::PopAndDestroy(3); // goldenTemplate close() tempTemplate
	}


LOCAL_C void TestTemplateCachingL()
//
// create MANY templates
//
	{
    const TInt KNumTestTemplates=32;
	test.Next(_L("Template caching"));
    //
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();

	CContactIdArray* cntIdArray=CContactIdArray::NewLC();
	for (TInt loop=0; loop<KNumTestTemplates; loop++)
		{
		TBuf<32> buf;
		buf.Format(_L("Template Cache %2d"),loop);
 		CContactItem* tempTemplate = CntTest->Db()->CreateContactCardTemplateLC(buf);
		TContactItemId templateId=tempTemplate->Id();
		CleanupStack::PopAndDestroy();	// tempTemplate
        //
		tempTemplate=(CContactCardTemplate*)CntTest->Db()->OpenContactLX(templateId);
		CleanupStack::PushL(tempTemplate);	// 
		for(TInt delLoop=tempTemplate->CardFields().Count()-1;delLoop>=0;delLoop--)
			tempTemplate->RemoveField(delLoop);
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName);
		buf.Format(_L("Label %2d"),loop);
		field->SetLabelL(buf);
		tempTemplate->AddFieldL(*field);
		CleanupStack::Pop();	// field
		CntTest->Db()->CommitContactL(*tempTemplate);
		CleanupStack::PopAndDestroy(2);	// tempTemplate, close template
        //
		tempTemplate=(CContactCardTemplate*)CntTest->Db()->ReadContactLC(templateId);
 		CContactCard *card=CContactCard::NewLC(tempTemplate);
		cntIdArray->AddL(CntTest->Db()->AddNewContactL(*card));
		CleanupStack::PopAndDestroy(2);	// tempTemplate,card
		}
	test(CntTest->Db()->TemplateCount()==KNumTestTemplates);
	CContactIdArray *ids=CntTest->Db()->GetCardTemplateIdListL();
	CleanupStack::PushL(ids);
	test(ids->Count()==KNumTestTemplates);
	for (TInt loop2=0; loop2<KNumTestTemplates; loop2++)
		{
		TBuf<32> name;
		name.Format(_L("Template Cache %2d"),loop2);
		TBuf<32> label;
		label.Format(_L("Label %2d"),loop2);
		for (TInt mode=0; mode<3; mode++)
			{
			if (mode==1)
				{
				CContactCardTemplate* editTemplate=(CContactCardTemplate*)CntTest->Db()->OpenContactLX((*ids)[loop2]);
				CleanupStack::PushL(editTemplate);
				name.Format(_L("Template Cache B%2d"),loop2);
				label.Format(_L("Label B%2d"),loop2);
				editTemplate->SetTemplateLabelL(name);
				editTemplate->CardFields()[1].SetLabelL(label);
				CntTest->Db()->CommitContactL(*editTemplate);
				CleanupStack::PopAndDestroy(2);	// tempTemplate2,close template
				}
			else
				{
				CContactItem* card=CntTest->Db()->ReadContactLC((*cntIdArray)[loop2]);
				CContactCardTemplate* tempTemplate2=(CContactCardTemplate*)CntTest->Db()->ReadContactLC((*ids)[loop2]);
				test(tempTemplate2->GetTemplateLabelL()==name);
				TBuf<32> tl1(card->CardFields()[0].Label());
				TBuf<32> tl2(tempTemplate2->CardFields()[1].Label());
				test(tl1==label);
				test(tl2==label);
				CleanupStack::PopAndDestroy(2);	// card,tempTemplate2
				}
			}
		}
	CleanupStack::PopAndDestroy(2);	// ids,cntIdArray
	}

/////////////////////////////////////////////////////////////////
//
//	Main dispatching module
//
/////////////////////////////////////////////////////////////////
/**

@SYMTestCaseID     PIM-T-CARDTM-0001

*/

void DoTestsL()
    {
    //	UserHal::SetAutoSwitchOffBehavior(ESwitchOffDisabled);
	test.Start(_L("@SYMTESTCaseID:PIM-T-CARDTM-0001 Create new database"));
	
	TRAPD(err,CntTest->CreateDatabaseL());
	test(err==KErrNone);

	TRAP(err,CreateCardTemplateL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,CheckCardTemplateListPersistL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,CreateEmptyContactsFromTemplateL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,CheckEmptyContactsFromTemplatePersistL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,CreateFullContactsFromTemplateL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,CheckFullContactsFromTemplatePersistL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,ModifyAddTemplateL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,CheckModifyAddContactsFromTemplatePersistL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,ModifyRemoveTemplateL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,CheckModifyRemoveContactsFromTemplatePersistL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,SetTemplateLabelL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	TRAP(err,CheckFieldLabelsPersistL());
	test(err==KErrNone);
	TRAP(err,DeleteCardTemplateL());
	test(err==KErrNone);
	TRAP(err,CheckDeleteCardTemplateL());
	test(err==KErrNone);
	TRAP(err,CheckLargePopulatedTemplate());
	test(err==KErrNone);
	TRAP(err,ModifyCardBasedOnTemplate());
	test(err==KErrNone);
	TRAP(err,DeleteTemplateCheckPersist());
	test(err==KErrNone);
	TRAP(err,CheckManyTemplates());
	test(err==KErrNone);
	TRAP(err,TestModifyCardAttribs());
	test(err==KErrNone);
	TRAP(err,PopulateUsingManyTemplates());
	test(err==KErrNone);
	TRAP(err,SetTemplateName());
	test(err==KErrNone);
	TRAP(err,SetPreferredTemplate());
	test(err==KErrNone);
	TRAP(err,SetBadPreferredTemplate());
	test(err==KErrNotSupported);
	TRAP(err,DeletePreferredTemplate());
	test(err==KErrNone);
	TRAP(err,DeleteManyTemplates());
	test(err==KErrNone);
	TRAP(err,TestTemplateCachingL());
	test(err==KErrNone);
	TRAP(err,DeleteAll());
	test(err==KErrNone);
    //	TestCardTemplateCreationL(*(CntTest->Db()));
	CntTest->CloseDatabase();
	
	test.Next(_L("Delete database"));

	TRAP(err,CntTest->DeleteDatabaseL());
	test(err==KErrNone);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	__UHEAP_MARKEND;
	return KErrNone;
    }
