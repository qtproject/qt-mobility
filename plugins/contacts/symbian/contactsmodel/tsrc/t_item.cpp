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
#include "t_utils.h"

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_ITEM"));

const TPtrC KDatabaseFileName=_L("C:T_ITEM");

#define KCardGivenName _L("Given Name #%d")
#define KCardFamilyName _L("Family Name #%d")
#define KCardFieldText _L("Card id %d, text field #%d")
#define KCardFieldEditedText _L("UPDATED Card id %d, text field #%d")

const TInt KNumTestContacts=20;

/** Create a database in a store and add KNumTestContacts contact cards  */
LOCAL_C void CreateDatabaseL()
	{
	CContactDatabase *db=CntTest->CreateDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
	test.Next(_L("Add basic cards"));
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	for (TInt ii=0;ii<KNumTestContacts;ii++)
		db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	CntTest->CloseDatabase();
	}
	
LOCAL_C void EditCardsL()
	{
	CContactDatabase* db=CntTest->OpenDatabaseL();
	// add fields to cards
	test.Next(_L("Add text fields to cards"));
	CContactIdArray* idList=CContactIdArray::NewLC(db->SortedItemsL());
	TInt ii=0;
	for (;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*idList)[ii];
		CContactItem* item=db->OpenContactL(index);
		CleanupStack::PushL(item);
		const TInt fieldCount=ii%9;
		for (TInt jj=0;jj<fieldCount;jj++)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldDefinedText);
			TBuf<32> text;
			text.Format(KCardFieldText,index,jj);
			field->TextStorage()->SetTextL(text);
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		db->CommitContactL(*item);
		CleanupStack::PopAndDestroy(); // item
		}
	// test field values
	test.Next(_L("Check field contents"));
	for (ii=0;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*idList)[ii];
		CContactItem* item=db->ReadContactLC(index);
		CContactItemFieldSet& fields=item->CardFields();
		const TInt fieldCount=ii%9;
		for (TInt jj=0;jj<fieldCount;jj++)
			{
			TBuf<32> text;
			text.Format(KCardFieldText,index,jj);
			TPtrC fieldText=fields[jj].TextStorage()->Text();
			test(text==fieldText);
			}
		CleanupStack::PopAndDestroy(); // item
		}
	// test field values have been persisted, then edit them
	CleanupStack::PopAndDestroy(); // idlist
	CntTest->CloseDatabase();
//
	test.Next(_L("Edit text fields"));
	db=CntTest->OpenDatabaseL();
	idList=CContactIdArray::NewLC(db->SortedItemsL());
	for (ii=0;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*idList)[ii];
		CContactItem* item=db->ReadContactLC(index);
		CContactItemFieldSet& fields=item->CardFields();
		const TInt fieldCount=fields.Count();
		test(fieldCount==ii%9);
		for (TInt jj=0;jj<fieldCount;jj++)
			{
			TBuf<32> text;
			text.Format(KCardFieldText,index,jj);
			TPtrC fieldText=fields[jj].TextStorage()->Text();
			test(text==fieldText);
			}
		CleanupStack::PopAndDestroy(); // item
		}
	for (ii=0;ii<10;ii++)
		{
		const TInt index=(*idList)[ii];
		CContactItem* item=db->OpenContactL(index);
		CleanupStack::PushL(item);
		CContactItemFieldSet& fields=item->CardFields();
		const TInt fieldCount=fields.Count();
		for (TInt jj=0;jj<fieldCount;jj++)
			{
			CContactItemField& field=fields[jj];
			TBuf<64> text;
			text.Format(KCardFieldEditedText,index,jj);
			field.TextStorage()->SetTextL(text);
			}
		db->CommitContactL(*item);
		CleanupStack::PopAndDestroy(); // item
		}
	for (ii=0;ii<10;ii++)
		{
		const TInt index=(*idList)[ii];
		CContactItem* item=db->ReadContactLC(index);
		CContactItemFieldSet& fields=item->CardFields();
		const TInt fieldCount=fields.Count();
		for (TInt jj=0;jj<fieldCount;jj++)
			{
			CContactItemField& field=fields[jj];
			TBuf<64> text;
			text.Format(KCardFieldEditedText,index,jj);
			TPtrC fieldText=field.TextStorage()->Text();
			test(text==fieldText);
			}
		CleanupStack::PopAndDestroy(); // item
		}
	CleanupStack::PopAndDestroy(); // idlist
	CntTest->CloseDatabase();
	}


/** Test that the text of the field with index aCurrentField contains the content: "Card id 123, text field #n",
	where n is aCurrentField
@panic USER 84 if the content is not what was expected
*/
  LOCAL_C void TestField(CContactItemFieldSet& aFields, TInt aCurrentField, TInt aOriginalField)
	{
	TBuf<32> text;
	text.Format(KCardFieldText,123,aOriginalField);
	TPtrC fieldText=aFields[aCurrentField].TextStorage()->Text();
	test(text==fieldText);
	}

LOCAL_C void TestFields(CContactItemFieldSet& aFields, TInt aField1,TInt aField2,TInt aField3,TInt aField4)
	{
	TestField(aFields,0,aField1);
	TestField(aFields,1,aField2);
	TestField(aFields,2,aField3);
	TestField(aFields,3,aField4);
	}

LOCAL_C void MoveFields()
	{
	test.Next(_L("Field moving"));
//
	CContactDatabase* db=CntTest->OpenDatabaseL();
//
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	TContactItemId testId=db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
//
	CContactItem* item=db->OpenContactL(testId);
	CleanupStack::PushL(item);
	for (TInt loop=0;loop<4;loop++)
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
		TBuf<32> text;
		text.Format(KCardFieldText,123,loop);
		field->TextStorage()->SetTextL(text);
		item->AddFieldL(*field);
		CleanupStack::Pop(); // field
		}
	db->CommitContactL(*item);
//
	CContactItemFieldSet& fields=item->CardFields();
	fields.Move(0,1);
	TestFields(fields,1,0,2,3);
	fields.Move(0,3);
	TestFields(fields,0,2,3,1);
	fields.Move(0,2);
	TestFields(fields,2,3,0,1);
	fields.Move(1,3);
	TestFields(fields,2,0,1,3);
	fields.Move(2,3);
	TestFields(fields,2,0,3,1);
	fields.Move(1,2);
	TestFields(fields,2,3,0,1);
//
	CleanupStack::PopAndDestroy(); // item
	CntTest->CloseDatabase();
	}

LOCAL_C void FieldSetTests()
	{
	CntTest->OpenDatabaseL();
//
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
//
	AddFieldL(card,KStorageTypeText,KUidContactFieldNone,KUidContactFieldVCardMapUnusedN);
//
	CContactItemFieldSet *fieldSet=CContactItemFieldSet::NewLC();
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldSuffixName,KUidContactFieldVCardMapUnusedN);
	(*fieldSet)[1].SetDisabled(ETrue);
//
	card->UpdateFieldSet(fieldSet);
	CleanupStack::Pop(); // fieldSet
//
	CContactItemFieldSet &cardFieldSet=card->CardFields();
	test(cardFieldSet[0].ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
	test(cardFieldSet[1].ContentType().ContainsFieldType(KUidContactFieldSuffixName));
	test(cardFieldSet[1].IsDisabled());
//
	CContactItemField *field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldAddress);
	card->InsertFieldL(*field,1);
	CleanupStack::Pop(); // field
	test(cardFieldSet[1].ContentType().ContainsFieldType(KUidContactFieldAddress));
	test(cardFieldSet[1].IsDisabled()==EFalse);
//
	card->RemoveField(1);
	test(cardFieldSet[1].ContentType().ContainsFieldType(KUidContactFieldSuffixName));
//
	card->RemoveField(0);
	test(cardFieldSet[0].ContentType().ContainsFieldType(KUidContactFieldSuffixName));
//
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldEMail);
	card->InsertFieldL(*field,0);
	CleanupStack::Pop(); // field
	test(cardFieldSet[0].ContentType().ContainsFieldType(KUidContactFieldEMail));
//
	CleanupStack::PopAndDestroy(); // card
	CntTest->CloseDatabase();
	}


/** Delete the surname field of the contact indicated by the index. Will Leave if the contact has no surname 
@leave	KErrNotFound	contact has no surname
@param		aDb		The contact database
@param		aIndex	The index of the contact whose surname will be removed
*/
LOCAL_C void TestDeleteNameL(CContactDatabase& aDb, TInt aIndex) 
	{
	CContactItem* item=aDb.OpenContactL(aIndex);
	CleanupStack::PushL(item);
	CContactItemFieldSet& fields=item->CardFields();
	TInt i = fields.Find(KUidContactFieldFamilyName);
	User::LeaveIfError(i);
	fields[i].TextStorage()->SetTextL(KNullDesC);
	aDb.CommitContactL(*item);
	CleanupStack::PopAndDestroy(item);
	}


/** Add first and last name fields to the contact in the format "Given Name #n" and "Family Name #n",
	where n is the supplied index.
@param		aItem	The contact database
@param		aIndex	The number attached to the first and last names
*/
LOCAL_C void AddNamesL(CContactItem& aItem, TInt aIndex)
	{
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldFamilyName);
	TBuf<32> text;
	text.Format(KCardFamilyName,aIndex);
	field->TextStorage()->SetTextL(text);
	aItem.AddFieldL(*field);
	CleanupStack::Pop(field); 
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName);
	text.Format(KCardGivenName,aIndex);
	field->TextStorage()->SetTextL(text);
	aItem.AddFieldL(*field);
	CleanupStack::Pop(field); 
	}



LOCAL_C void DeleteNamesL()
	{
	CContactDatabase* db=CntTest->OpenDatabaseL();
	// add fields to cards
	test.Next(_L("Add names to cards"));
	CContactIdArray* idList=CContactIdArray::NewLC(db->SortedItemsL());
	TInt ii=0;
	for (;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*idList)[ii];
		CContactItem* item=db->OpenContactL(index);
		CleanupStack::PushL(item);
		const TInt fieldCount=ii%9;
		for (TInt jj=0;jj<fieldCount;jj++)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldDefinedText);
			TBuf<32> text;
			text.Format(KCardFieldText,index,jj);
			field->TextStorage()->SetTextL(text);
			item->AddFieldL(*field);
			CleanupStack::Pop(field); // 
			}
		AddNamesL(*item, ii);
		db->CommitContactL(*item);
		CleanupStack::PopAndDestroy(); // item
		}
	
	// test field values
	// test field values have been persisted, then edit them
	CleanupStack::PopAndDestroy(); // idlist
	CntTest->CloseDatabase();

	db=CntTest->OpenDatabaseL();
	idList=CContactIdArray::NewLC(db->SortedItemsL());
	test.Next(_L("Delete last names"));
	for (ii=0;ii<KNumTestContacts;ii++)
		{
		TestDeleteNameL(*db, (*idList)[ii] );
		}

	test.Next(_L("Check field contents"));
	for (ii=0;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*idList)[ii];
		CContactItem* item=db->ReadContactLC(index);
		CContactItemFieldSet& fields=item->CardFields();
		TInt i = fields.Find(KUidContactFieldFamilyName);
		test(i==KErrNotFound);
		CleanupStack::PopAndDestroy(item); 
		}
	CleanupStack::PopAndDestroy(idList); 
	CntTest->CloseDatabase();
	}


/** Test CContactItem::Guid method. */
LOCAL_C void TestGuidL()
	{
	CContactCard* card=CContactCard::NewLC();
	TPtrC ptr = card->Guid();
	test(ptr==KNullDesC);
	CntTest->OpenDatabaseL();
	CContactDatabase& db = *(CntTest->Db());
	TContactItemId id = db.AddNewContactL(*card);
	CleanupStack::PopAndDestroy(card);
	CContactItem* card2 = db.OpenContactL(id);
	CleanupStack::PushL(card2);
	TPtrC guid = card2->Guid();
	test(guid.Length()!=0);
	db.CloseContactL(id);
	CleanupStack::PopAndDestroy(card2);
	CntTest->CloseDatabase();
	}

/**

@SYMTestCaseID     PIM-T-ITEM-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTestCaseID:PIM-T-ITEM-0001 Create new database"));
	CreateDatabaseL();
	TRAPD(ret,EditCardsL());
	test(ret==KErrNone);
	TRAP(ret,MoveFields());
	test(ret==KErrNone);
	TRAP(ret,FieldSetTests());
	test(ret==KErrNone);
	test.Next(_L("Test CContactItem::Guid method"));
	TestGuidL();
	TRAP(ret,DeleteNamesL());
	test(ret==KErrNone);
	test.Next(_L("Delete database"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
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
