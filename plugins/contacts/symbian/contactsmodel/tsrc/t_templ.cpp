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
LOCAL_D RTest test(_L("T_TEMPL"));

const TPtrC KDatabaseFileName=_L("C:T_TEMPL");

const TInt KTotalFindRecords=20;

#define KCardGivenName _L("Given Name #%d")
#define KCardFamilyName _L("Family Name #%d")
#define KCardFieldText _L("Card id %d, text field #%d")
#define KCardFieldEditedText _L("UPDATED Card id %d, text field #%d")

LOCAL_C void CreateDatabaseL()
//
// Create a database in a store and add some contact cards
//
	{
	CContactDatabase *db=CntTest->CreateDatabaseL();
	test.Next(_L("Add basic cards"));
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	for (TInt ii=0;ii<20;ii++)
		{
		db->AddNewContactL(*card);
		}
	CleanupStack::PopAndDestroy(); // card
	CntTest->CloseDatabase();
	}
	
LOCAL_C void AddFieldL(CContactItem *aItem,TFieldType aFieldType, const TDesC &aLabel, const TDesC &aFieldText)
	{
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,aFieldType);
	field->SetLabelL(aLabel);
	aItem->AddFieldL(*field);
	if (aFieldText.Length()>0)
		field->TextStorage()->SetTextL(aFieldText);
	CleanupStack::Pop(); // field
	}

LOCAL_C void AddDateFieldL(CContactItem *aItem,TFieldType aFieldType, const TDesC &aLabel, TDateTime *aDateTime)
	{
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeDateTime,aFieldType);
	field->SetLabelL(aLabel);
	aItem->AddFieldL(*field);
	if (aDateTime)
		field->DateTimeStorage()->SetTime(*aDateTime);
	CleanupStack::Pop(); // field
	}

LOCAL_C CContactItem* ReadContactIncHiddenFieldsLC(CContactDatabase* aDb, TContactItemId aContactId)
	{
	CContactItemViewDef *viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EIncludeHiddenFields);
	CContactItem *contact=aDb->ReadContactL(aContactId,*viewDef);
	CleanupStack::PopAndDestroy();	// viewDef
	CleanupStack::PushL(contact);
	return(contact);
	}

LOCAL_C void doTestTemplate(CContactDatabase* aDb, TContactItemId aTemplateId,CContactTemplate* aContactTemplate, TBool aTemplateLabelRemoved)
	{
	CContactItem *checkTemplate=ReadContactIncHiddenFieldsLC(aDb,aTemplateId);
	test(CompareItemFields(aContactTemplate,checkTemplate));
	CleanupStack::PopAndDestroy();	// checkTemplate
//
	CContactCard *copy=CContactCard::NewLC(aContactTemplate);
	// fails due to template lable being removed. /// test code needs fixing
	test(CompareItemFields(aContactTemplate,copy,EFalse,aTemplateLabelRemoved));
	CleanupStack::PopAndDestroy();	// copy
	}

LOCAL_C void CheckDefaultTemplate(CContactDatabase* aDb, RFs& aFs, TContactItemId aSystemTemplateId)
	{
	test.Next(_L("System template"));
	
    _LIT(KCntSystemTemplate,"z:\\resource\\cntmodel\\cntmodel.rsc");
	
	TFileName fileName(KCntSystemTemplate);
	BaflUtils::NearestLanguageFile(aFs,fileName);
//
	RResourceFile resourceFile;
	resourceFile.OpenL(CntTest->Fs(),fileName);
	CleanupClosePushL(resourceFile);
	resourceFile.ConfirmSignatureL(4);
	TResourceReader resReader;
	resReader.SetBuffer(resourceFile.AllocReadLC(R_CNTUI_NEW_FIELD_DEFNS));
//
	CContactTemplate *testTemplate=CContactTemplate::NewLC();
	TInt count=resReader.ReadInt16(); // num fields
	while(count--)
		{
		TStorageType storageType=resReader.ReadInt32();
		TUid contactFieldUid=TUid::Uid(resReader.ReadInt32());
		TUid vCardMappingUid=TUid::Uid(resReader.ReadInt32());
//
		CContactItemField* field=CContactItemField::NewLC(storageType);
//
		field->SetMapping(vCardMappingUid);
		if (contactFieldUid!=KUidContactFieldNone)
			field->AddFieldTypeL(contactFieldUid);
//
		TInt mappingCount;
		if((mappingCount=resReader.ReadInt16())!=0)
			{
			while(mappingCount--)
				field->AddFieldTypeL(TUid::Uid(resReader.ReadInt32()));
			}
		field->SetUserFlags(resReader.ReadInt32());
		field->SetLabel(resReader.ReadHBufCL());
		TUint flags=resReader.ReadInt32();
		if (flags&EContactFieldFlagHidden)
			field->SetHidden(ETrue);
		if (flags&EContactFieldFlagReadOnly)
			field->SetReadOnly(ETrue);
		if (flags&EContactFieldFlagSynchronize)
			field->SetSynchronize(ETrue);
		if (flags&EContactFieldFlagDisabled)
			field->SetDisabled(ETrue);
//
		testTemplate->AddFieldL(*field);
		CleanupStack::Pop();
		}
	doTestTemplate(aDb,aSystemTemplateId,testTemplate,ETrue);
	CleanupStack::PopAndDestroy(3);	// testTemplate,resourceFile close, resBuffer
	}

LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName, TBool aAddField)
//
// Set the contents of a text field, creating the field if required
//
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (!aAddField && pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,aType);
   		field->SetMapping(KUidContactFieldVCardMapUnusedN);
		field->TextStorage()->SetTextL(aName);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(); // item
		}
	}

LOCAL_C void AddFindRecordsL(CContactDatabase* aDb)
	{
	CContactItem* systemTemplate=ReadContactIncHiddenFieldsLC(aDb,aDb->TemplateId());
	for (TInt ii=0;ii<KTotalFindRecords;ii++)
		{
		CContactItem* item=CContactCard::NewLC(systemTemplate);
		TBuf<16> name;
		if (ii%2)
			{
			name.Format(_L("Bates %d"),ii);
			SetNameL(*item,KUidContactFieldJobTitle,name,EFalse);
   			//SetNameL(*item,KUidContactFieldFamilyName,name,EFalse);
   			SetNameL(*item,KUidContactFieldGivenName,_L("Norman"),EFalse);
			}
		else
			{
			name.Format(_L("Sod %d"),ii);
   			SetNameL(*item,KUidContactFieldFamilyName,name,EFalse);
   			SetNameL(*item,KUidContactFieldGivenName,_L("Silly"),EFalse);
			}
		if (ii%3)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeDateTime,KUidContactFieldBirthday);
			field->DateTimeStorage()->SetTime(TDateTime(1990+ii,(TMonth)(ii%12),(ii*3)%28,1,2,3,4));
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		aDb->AddNewContactL(*item);
		CleanupStack::PopAndDestroy(); // item
		}
	CleanupStack::PopAndDestroy(); // systemTemplate
	}

LOCAL_C void TestSystemTemplateL()
	{
	CContactDatabase* db=CntTest->OpenDatabaseL();
	// add fields to cards to system template
	CheckDefaultTemplate(db,CntTest->Fs(),db->TemplateId());
//
	CContactTemplate *contactTemplate=(CContactTemplate *)db->OpenContactLX(db->TemplateId());
	CleanupStack::PushL(contactTemplate);
	AddFieldL(contactTemplate,TUid::Uid(1),_L("Uid1"),_L(""));
	AddFieldL(contactTemplate,TUid::Uid(2),_L("Uid2"),_L(""));
	AddFieldL(contactTemplate,TUid::Uid(3),_L("Uid3"),_L(""));
	AddDateFieldL(contactTemplate,TUid::Uid(4),_L("Uid4"),NULL);
	db->CommitContactL(*contactTemplate);
	doTestTemplate(db,db->TemplateId(),contactTemplate,ETrue);
	CleanupStack::PopAndDestroy(2); // contactTemplate,contactTemplate.Close()
//
	AddFindRecordsL(db);

	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldJobTitle);
	CContactIdArray* idarray=db->FindLC(_L("Bates 3"),fieldDef);
	test(idarray->Count()==1);
//
// Now change the field KUidContactFieldFamilyName field type to Uid(44) and check
// the records field type also changes.
//
	contactTemplate=(CContactTemplate *)db->OpenContactLX(db->TemplateId());
	CleanupStack::PushL(contactTemplate);
	CContactItemFieldSet& fieldSet=contactTemplate->CardFields();
	TInt fieldNum=fieldSet.Find(KUidContactFieldJobTitle);
	CContentType &ct=(CContentType&)fieldSet[fieldNum].ContentType();
	ct.RemoveFieldType(KUidContactFieldJobTitle);
	TUid newUidType=TUid::Uid(44);
	ct.AddFieldTypeL(newUidType);
	db->CommitContactL(*contactTemplate);
//
	CContactItem *checkItem=(CContactTemplate *)db->ReadContactLC((*idarray)[0]);
	CContactItemFieldSet& templateFieldSet=checkItem->CardFields();
	test(templateFieldSet.Find(KUidContactFieldJobTitle)==KErrNotFound);
	test(templateFieldSet.Find(newUidType)!=KErrNotFound);
	CleanupStack::PopAndDestroy(1); // checkItem
//
// Check finding using the new field type also works
//
	fieldDef->Delete(0);
	fieldDef->AppendL(newUidType);
	CContactIdArray* idarray2=db->FindLC(_L("Bates 3"),fieldDef);
	test(idarray2->Count()==1);
	test((*idarray2)[0]==(*idarray)[0]);
	CleanupStack::PopAndDestroy(5); // fieldDef,idarray2,idarray,contactTemplate,contactTemplate.Close()
//
	CntTest->CloseDatabase();
	}

LOCAL_C void TestSystemTemplateRestorationL()
	{
//	Any field values present in the template are stripped out when the template is
//	added to the database.  However, it is still possible to store default values in
//	a template if the template is first added, and then updated with default data present.

	CContactDatabase* db=CntTest->OpenDatabaseL();
	
	// create a new contact template
	CContactItem* contactTemplate=db->CreateContactCardTemplateLC(_L("fieldDataNotRestoredTemplate"));
	TContactItemId id=contactTemplate->Id();
	CleanupStack::PopAndDestroy(contactTemplate);
	// add some data to one of the template fields
	CContactTemplate *openTemplate=(CContactTemplate *)db->OpenContactLX(id);
	CleanupStack::PushL(openTemplate);
	CContactItemFieldSet& fieldSet=openTemplate->CardFields();
	TInt pos=fieldSet.Find(KUidContactFieldGivenName);
	test(fieldSet[pos].TextStorage()->Text() == KNullDesC);
	TBuf<12> givenName = _L("a given name");
	SetNameL(*openTemplate,KUidContactFieldGivenName,givenName,EFalse);
	// check the field value is set
	CContactItemFieldSet& fieldSet2=openTemplate->CardFields();
	pos=fieldSet2.Find(KUidContactFieldGivenName);
	test(fieldSet2[pos].TextStorage()->Text() == givenName);
	db->CommitContactL(*openTemplate);
	CleanupStack::PopAndDestroy(2); // openTemplate, openTemplate.Close()
	// the field value should still be set
	CContactItem* contactTemplate2=db->ReadContactLC(id);
	CContactItemFieldSet& fieldSet3=contactTemplate2->CardFields();
	pos=fieldSet3.Find(KUidContactFieldGivenName);
	test(fieldSet3[pos].TextStorage()->Text() == givenName);
	CleanupStack::PopAndDestroy(contactTemplate2);

	CntTest->CloseDatabase();
	}

LOCAL_C void TestTemplateLabelsL()
	{
	CContactDatabase* db=CntTest->OpenDatabaseL();
//
	CContactItem *contactTemplate=ReadContactIncHiddenFieldsLC(db,db->TemplateId());
	CContactItem *card=CContactCard::NewLC(contactTemplate);
	CContactItemFieldSet& fieldSet=card->CardFields();
	fieldSet[0].TextStorage()->SetTextL(_L("Sum text"));
	fieldSet[2].TextStorage()->SetTextL(_L("Some more text"));
	TContactItemId id1=db->AddNewContactL(*card);
//
	CContactItem *card2=ReadContactIncHiddenFieldsLC(db,id1);
	test(CompareItemFields(contactTemplate,card2,EFalse,ETrue));
//
	CContactItem *contactTemplate2=ReadContactIncHiddenFieldsLC(db,db->TemplateId());
	
	CContactItem *card3=CContactCard::NewLC(contactTemplate2);
	CContactItemFieldSet& fieldSet2=card3->CardFields();
	fieldSet2[0].SetLabelL(_L("Label Zero"));
	fieldSet2[0].TextStorage()->SetTextL(_L("x"));
	fieldSet2[1].TextStorage()->SetTextL(_L("x"));
	fieldSet2[2].SetLabelL(_L("Label Too"));
	TContactItemId id2=db->AddNewContactL(*card3);
	fieldSet2[1].SetLabelL(fieldSet[1].Label());	// Should be reset because SetLabelUnspecified is true
	fieldSet2[2].SetLabelL(fieldSet[2].Label());	// Should be reset because field is empty
//
	CContactItem *card4=ReadContactIncHiddenFieldsLC(db,id2);
	test(CompareItemFields(card3,card4,EFalse));
	CleanupStack::PopAndDestroy(3); // template2,card3,card4
	CleanupStack::PopAndDestroy(3); // card,card2,template,template.Close()
//
	CContactTemplate *openTemplate=(CContactTemplate *)db->OpenContactLX(db->TemplateId());
	CleanupStack::PushL(openTemplate);
	CContactItemFieldSet& templateFieldSet=openTemplate->CardFields();
	templateFieldSet[1].SetDisabled(ETrue);
	templateFieldSet[1].SetReadOnly(ETrue);
	db->CommitContactL(*openTemplate);
	CContactItem *card5=ReadContactIncHiddenFieldsLC(db,id1);
	test(CompareItemFields(openTemplate,card5,EFalse,ETrue));
	CleanupStack::PopAndDestroy(3); // card5, openTemplate, openTemplate.Close()
//
	openTemplate=(CContactTemplate *)db->OpenContactLX(db->TemplateId());
	CleanupStack::PushL(openTemplate);
	openTemplate->CardFields()[1].SetReadOnly(EFalse);
	openTemplate->CardFields()[1].SetDisabled(EFalse);
	db->CommitContactL(*openTemplate);
	CleanupStack::PopAndDestroy(2); // openTemplate, openTemplate.Close()
//
	CntTest->CloseDatabase();
	}

LOCAL_C CContactItemField *doAddFieldL(CContactItem *aItem,TFieldType aMapping,TInt aPos, const TDesC &aLabel,TFieldType *aFieldType1,TFieldType *aFieldType2,TFieldType *aFieldType3)
	{
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
	field->SetMapping(aMapping);
	field->SetLabelL(aLabel);
	if (aFieldType1)
		{
		field->AddFieldTypeL(*aFieldType1);
		if (aFieldType2)
			{
			field->AddFieldTypeL(*aFieldType2);
			if (aFieldType3)
				field->AddFieldTypeL(*aFieldType3);
			}
		}
	if (aPos<0)
		aItem->AddFieldL(*field);
	else
		aItem->InsertFieldL(*field,aPos);
	CleanupStack::Pop(); // field
	return(field);
	}

LOCAL_C CContactItemField *InsertFieldL(CContactItem *aItem,TFieldType aMapping,TInt aPos,const TDesC &aLabel,TFieldType *aFieldType1=NULL,TFieldType *aFieldType2=NULL,TFieldType *aFieldType3=NULL)
	{
	return(doAddFieldL(aItem,aMapping,aPos,aLabel,aFieldType1,aFieldType2,aFieldType3));
	}

LOCAL_C CContactItemField *AddFieldL(CContactItem *aItem,TFieldType aMapping,const TDesC &aLabel,TFieldType *aFieldType1=NULL,TFieldType *aFieldType2=NULL,TFieldType *aFieldType3=NULL)
	{
	return(doAddFieldL(aItem,aMapping,-1,aLabel,aFieldType1,aFieldType2,aFieldType3));
	}

struct TTemplateMapTest
	{
public:
	inline TTemplateMapTest(TFieldType aMapping,TFieldType *aFieldType1,TFieldType *aFieldType2,TFieldType *aFieldType3, const TDesC &aFieldName, TInt aFieldNumber, TBool aExactContentMatch, TFieldType *aExtraField=NULL, TFieldType *aMissingField=NULL) :
			iMapping(aMapping),iFieldType1(aFieldType1),iFieldType2(aFieldType2),iFieldType3(aFieldType3),iFieldNumber(aFieldNumber),iExactContentMatch(aExactContentMatch),iExtraField(aExtraField),iMissingField(aMissingField)
			{iFieldName.Set(aFieldName);};
	CContentType *ContentTypeLC() const;
public:
	TFieldType iMapping;
	TFieldType *iFieldType1;
	TFieldType *iFieldType2;
	TFieldType *iFieldType3;
	TPtrC iFieldName;
	TInt iFieldNumber;
	TBool iExactContentMatch;
	TFieldType *iExtraField;
	TFieldType *iMissingField;
	};

CContentType *TTemplateMapTest::ContentTypeLC() const
	{
	CContentType *ct=CContentType::NewL(*iFieldType1,iMapping);
	CleanupStack::PushL(ct);
	if (iFieldType2)
		ct->AddFieldTypeL(*iFieldType2);
	if (iFieldType3)
		ct->AddFieldTypeL(*iFieldType3);
	return(ct);
	}

LOCAL_C void AddTestField(CContactCard* aCard,TTemplateMapTest *aTmtest, const TDesC &aText, TInt &aExtraFields)
	{
	if (aTmtest)
		{
		CContactItemField *field=AddFieldL(aCard,aTmtest->iMapping,aTmtest->iFieldName,aTmtest->iFieldType1,aTmtest->iFieldType2,aTmtest->iFieldType3);
		field->TextStorage()->SetTextL(aText);
		if (aTmtest->iExtraField)
			aExtraFields++;
		}
	}

LOCAL_C void CheckFieldMappings(CContactDatabase* aDb, CContactItem *aTemplateItem, TTemplateMapTest *aTmtest1,TTemplateMapTest *aTmtest2=NULL,TTemplateMapTest *aTmtest3=NULL,TTemplateMapTest *aTmtest4=NULL)
	{
	CContactCard* card=CContactCard::NewLC(aTemplateItem);
//
	TInt extraFields=0;
	AddTestField(card,aTmtest1,_L("Test 1"),extraFields);
	AddTestField(card,aTmtest2,_L("Test 2"),extraFields);
	AddTestField(card,aTmtest3,_L("Test 3"),extraFields);
	AddTestField(card,aTmtest4,_L("Test 4"),extraFields);
	TContactItemId testCardId=aDb->AddNewContactL(*card);
	CleanupStack::PopAndDestroy();	// card
//
	TTemplateMapTest *testData[4]={aTmtest1,aTmtest2,aTmtest3,aTmtest4};
	//ere
	CContactItem *readCard=ReadContactIncHiddenFieldsLC(aDb,testCardId);
	CContactItemFieldSet& fieldSet=readCard->CardFields();
	CContactItemFieldSet& templateFieldSet=aTemplateItem->CardFields();
	test((templateFieldSet.Count()+extraFields)==fieldSet.Count()); 
	TInt loop=0;
	for(;loop<templateFieldSet.Count();loop++)
		{
		const CContactItemField &field=fieldSet[loop];
		const CContactItemField &templateField=templateFieldSet[loop];
		for(TInt loop2=0;loop2<4;loop2++)
			{
			TBuf<8> fieldText;
			fieldText.Format(_L("Test %d"),loop2+1);
			if (field.TextStorage()->Text()==fieldText)
				{
				TPtrC temp = testData[loop2]->iFieldName;
				test(field.Label()==temp);
				TInt fieldNumber=testData[loop2]->iFieldNumber;
				test((loop+1)==fieldNumber);
				CContentType *ct=testData[loop2]->ContentTypeLC();
				test(field.ContentType()==*ct);
				TBool templateContentMatch=(templateField.ContentType()==*ct);
				test(testData[loop2]->iExactContentMatch?templateContentMatch:!templateContentMatch);
				test(templateField.UserFlags()==field.UserFlags());
				test(templateField.ContentType()==aDb->TemplateContentType(field));
				CleanupStack::PopAndDestroy();	// ct
				}
			}
		}
	TInt nextTmTest=0;
	for(;loop<fieldSet.Count();loop++)
		{
		while(testData[nextTmTest]->iExtraField==NULL && testData[nextTmTest]->iMissingField==NULL)
			nextTmTest++;
		if (testData[nextTmTest]->iExtraField)
			test(fieldSet[loop].ContentType().ContainsFieldType(*testData[nextTmTest]->iExtraField));
		if (testData[nextTmTest]->iMissingField)
			{
			test(!fieldSet[loop].ContentType().ContainsFieldType(*testData[nextTmTest]->iMissingField));
			test(aDb->TemplateContentType(fieldSet[loop]).ContainsFieldType(*testData[nextTmTest]->iMissingField));
			}
		nextTmTest++;
		}
	CleanupStack::PopAndDestroy();	// readCard
//
	aDb->DeleteContactL(testCardId);
	}

LOCAL_C void TestTemplateFieldMatchingL()
	{
	test.Next(_L("Create template"));
	CreateDatabaseL();
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
//
	_LIT(KField1,"Field 1");
	_LIT(KField2,"Field 2");
	_LIT(KField3,"Field 3");
	_LIT(KField4,"Field 4");
	_LIT(KField5,"Field 5");
	_LIT(KField6,"Field 6");
	_LIT(KField7,"Field 7");
	_LIT(KField8,"Field 8");
	_LIT(KField9,"Field 9");
	_LIT(KFieldX1,"Field X1");
	_LIT(KFieldX2,"Field X2");
	_LIT(KFieldHome,"Field Home");
	_LIT(KFieldWork,"Field Work");
//
	CContactTemplate *contactTemplate=(CContactTemplate *)db->OpenContactLX(db->TemplateId());
	CleanupStack::PushL(contactTemplate);
	TFieldType mapping1(TUid::Uid(1));
	TFieldType mapping2(TUid::Uid(2));
	TFieldType mapping3(TUid::Uid(3));
	TFieldType mapping4(TUid::Uid(4));
	TFieldType mappingX(KUidContactFieldVCardMapUnusedN);
	TFieldType fieldType1(TUid::Uid(101));
	TFieldType fieldType2(TUid::Uid(102));
	TFieldType fieldType3(TUid::Uid(103));
	TFieldType fieldType4(TUid::Uid(104));
	TFieldType fieldType5(TUid::Uid(105));
	TFieldType fieldTypeHome(TUid::Uid(KIntContactFieldVCardMapHOME));
	TFieldType fieldTypeWork(TUid::Uid(KIntContactFieldVCardMapWORK));
	AddFieldL(contactTemplate,mapping1,KField1());
	SetUserFlags(contactTemplate,1);
	AddFieldL(contactTemplate,mapping2,KField2(),&fieldType2);
	SetUserFlags(contactTemplate,2);
	AddFieldL(contactTemplate,mapping2,KField3(),&fieldType3);
	SetUserFlags(contactTemplate,3);
	AddFieldL(contactTemplate,mapping3,KField4(),&fieldType2);
	SetUserFlags(contactTemplate,0);
	AddFieldL(contactTemplate,mapping3,KField5(),&fieldType2,&fieldType4);
	SetUserFlags(contactTemplate,1);
	AddFieldL(contactTemplate,mapping3,KField6(),&fieldType2,&fieldType1);
	SetUserFlags(contactTemplate,2);
	AddFieldL(contactTemplate,mapping4,KField8(),&fieldType4);
	SetUserFlags(contactTemplate,3);
	InsertFieldL(contactTemplate,mapping4,6,KField7());
	SetUserFlags(contactTemplate,0);
	AddFieldL(contactTemplate,mapping4,KField9(),&fieldType4,&fieldType2,&fieldType1);
	SetUserFlags(contactTemplate,1);
	AddFieldL(contactTemplate,mappingX,KFieldX1());
	SetUserFlags(contactTemplate,2);
	AddFieldL(contactTemplate,mappingX,KFieldX2(),&fieldType2);
	SetUserFlags(contactTemplate,3);
	AddFieldL(contactTemplate,mapping1,KFieldHome(),&fieldType1,&fieldType5,&fieldTypeHome);
	SetUserFlags(contactTemplate,0);
	AddFieldL(contactTemplate,mapping1,KFieldWork(),&fieldType1,&fieldType5,&fieldTypeWork);
	
	SetUserFlags(contactTemplate,0);
	db->CommitContactL(*contactTemplate);
	CleanupStack::PopAndDestroy(); // contactTemplate
	CleanupStack::Pop(); // contactTemplate.Close()
//
	TTemplateMapTest tmTest1(mapping1,&fieldType4,NULL,NULL,KField1(),1,EFalse);
	TTemplateMapTest tmTest2(mapping2,&fieldType1,NULL,NULL,KField2(),2,EFalse);
	TTemplateMapTest tmTest3(mapping2,&fieldType2,NULL,NULL,KField2(),2,ETrue);
	TTemplateMapTest tmTest4(mapping2,&fieldType3,NULL,NULL,KField3(),3,ETrue);
	TTemplateMapTest tmTest5(mapping3,&fieldType4,NULL,NULL,KField5(),5,EFalse);
	TTemplateMapTest tmTest6(mapping3,&fieldType1,NULL,NULL,KField6(),6,EFalse);
	TTemplateMapTest tmTest7(mapping3,&fieldType2,&fieldType4,NULL,KField5(),5,ETrue);
	TTemplateMapTest tmTest8(mapping4,&fieldType3,NULL,NULL,KField7(),7,EFalse);
	TTemplateMapTest tmTest9(mapping4,&fieldType4,NULL,NULL,KField8(),8,ETrue);
	TTemplateMapTest tmTest10(mapping4,&fieldType4,&fieldType2,NULL,KField9(),9,EFalse);
	TTemplateMapTest tmTestX1(mappingX,&fieldType4,NULL,NULL,KFieldX1(),10,EFalse,&mappingX);
	TTemplateMapTest tmTestX2(mappingX,&fieldType2,NULL,NULL,KFieldX2(),11,ETrue);
	TTemplateMapTest tmTestWork(mapping1,&fieldType1,&fieldType5,NULL,KFieldWork(),13,EFalse,NULL,&fieldTypeWork);//zzz change to work
	contactTemplate=(CContactTemplate *)ReadContactIncHiddenFieldsLC(db,db->TemplateId());
	CheckFieldMappings(db,contactTemplate,&tmTest1);
	CheckFieldMappings(db,contactTemplate,&tmTest1,&tmTest3,&tmTest4);
	CheckFieldMappings(db,contactTemplate,&tmTest5,&tmTest2,&tmTest1,&tmTest6);
	CheckFieldMappings(db,contactTemplate,&tmTest7,&tmTest10,&tmTest8);
	CheckFieldMappings(db,contactTemplate,&tmTest9);
	CheckFieldMappings(db,contactTemplate,&tmTestX1);
	CheckFieldMappings(db,contactTemplate,&tmTestX2);
	CheckFieldMappings(db,contactTemplate,&tmTestWork);
//
	CleanupStack::PopAndDestroy(); // contactTemplate
	CntTest->CloseDatabase();
	}

LOCAL_C void TestTemplateContentTypeL()
	{
	CreateDatabaseL();
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CContactItem* systemTemplate=ReadContactIncHiddenFieldsLC(db,db->TemplateId());
	CContactCard *card=CContactCard::NewLC(systemTemplate);
	CContactItemFieldSet& fieldSet=card->CardFields();
	for(TInt loop=0;loop<fieldSet.Count();loop++)
		test(db->TemplateContentType(fieldSet[loop])==fieldSet[loop].ContentType());
	CleanupStack::PopAndDestroy(2); // systemTemplate,card
	CntTest->CloseDatabase();
	}

LOCAL_C void TestMultiTemplateCacheL()
	{
	test.Next(_L("Create template"));
	CreateDatabaseL();
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
//
	CContactTemplate* contactTemplate=CContactTemplate::NewLC();
	TContactItemId id1=db->AddNewContactL(*contactTemplate);
	CleanupStack::PopAndDestroy(); // contactTemplate
	contactTemplate=(CContactTemplate *)db->ReadContactLC(id1);
// add fields to cards
	AddFieldL(contactTemplate,KUidContactFieldGivenName,_L("First name"),_L("Bilbo"));
	AddFieldL(contactTemplate,KUidContactFieldFamilyName,_L("Surname"),_L("Baggins"));
	AddFieldL(contactTemplate,KUidContactFieldCompanyName,_L("Company"),_L("Great Ape Software"));
	TDateTime dt(1965,EJuly,15,0,0,0,0);
	AddDateFieldL(contactTemplate,KUidContactFieldNone,_L("Birthday"),&dt);
//
	TContactItemId templateId=db->AddNewContactL(*contactTemplate);
//
	doTestTemplate(db,templateId,contactTemplate,EFalse);
	CleanupStack::PopAndDestroy(); // contactTemplate
	CntTest->CloseDatabase();
	}

LOCAL_C void TestTemplateL()
	{
	test.Next(_L("Create template"));
	CreateDatabaseL();
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
//
	CContactTemplate* contactTemplate=CContactTemplate::NewLC();
	TContactItemId id1=db->AddNewContactL(*contactTemplate);
	CleanupStack::PopAndDestroy(); // contactTemplate
	contactTemplate=(CContactTemplate *)db->ReadContactLC(id1);
// add fields to cards
	AddFieldL(contactTemplate,KUidContactFieldGivenName,_L("First name"),_L("Bilbo"));
	AddFieldL(contactTemplate,KUidContactFieldFamilyName,_L("Surname"),_L("Baggins"));
	AddFieldL(contactTemplate,KUidContactFieldCompanyName,_L("Company"),_L("Great Ape Software"));
	TDateTime dt(1965,EJuly,15,0,0,0,0);
	AddDateFieldL(contactTemplate,KUidContactFieldNone,_L("Birthday"),&dt);
//
	TContactItemId templateId=db->AddNewContactL(*contactTemplate);
//
	doTestTemplate(db,templateId,contactTemplate,EFalse);
	CleanupStack::PopAndDestroy(); // contactTemplate
	CntTest->CloseDatabase();
	}

LOCAL_C void TestSystemTemplateFieldsL()
	{
	// create a new db and checks the template fields for instances of LOGO, PHOTO, JOB TITLE
	CContactDatabase* db=CntTest->CreateDatabaseL();
	
	CContactTemplate *contactTemplate=(CContactTemplate *)db->OpenContactLX(db->TemplateId());
	CleanupStack::PushL(contactTemplate);
	CContactItemFieldSet& templateFieldSet = contactTemplate->CardFields();
	TInt templateFieldCount = templateFieldSet.Count();
	TBool titleFound=EFalse;
	TBool logoFound=EFalse;
	TBool photoFound=EFalse;
	for (TInt ii=0; ii < templateFieldCount; ii++)
		{

		// JOB TITLE SEARCH
		if (templateFieldSet[ii].ContentType().ContainsFieldType(KUidContactFieldVCardMapTITLE))
			{
			// SATISFIED
			titleFound=ETrue;
			}

		// LOGO SEARCH
		if (templateFieldSet[ii].ContentType().ContainsFieldType(KUidContactFieldVCardMapLOGO))
			{
			// SATISFIED
			logoFound=ETrue;
			}

		// LOGO SEARCH
		if (templateFieldSet[ii].ContentType().ContainsFieldType(KUidContactFieldVCardMapPHOTO))
			{
			// SATISFIED
			photoFound=ETrue;
			}
		};
	test(photoFound);
	test(logoFound);
	test(titleFound);

	CleanupStack::PopAndDestroy(2); // contactTemplate,contactTemplate.Close()
//
	CntTest->CloseDatabase();
	}


/** 
 * Regression testcode for EDNSKSA-53ZKXM "Problem with TemplateRefId"
 */
void TestTemplateReferenceCountingL()
	{
	CreateDatabaseL();
	CContactDatabase* db=CntTest->OpenDatabaseL();
	//create a template and a card based on it
	_LIT(KUserDefinedTemplate,"MyTemplate");
	CContactItem* temp = db->CreateContactCardTemplateLC(KUserDefinedTemplate);
	CContactItem* card = CContactCard::NewLC(temp);
	TContactItemId one= db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(card);
	CleanupStack::PopAndDestroy(temp);
	//increment reference count (simulate synchronisation)
	CContactItem* card2 = db->OpenContactLX(one);
	CleanupStack::PushL(card2);
	card2->IncAccessCount();
	db->CommitContactL(*card2);
	CleanupStack::PopAndDestroy(card2); 
	CleanupStack::PopAndDestroy();//lock record
	//delete both synchronised copies
	db->DeleteContactL(one);
	db->DeleteContactL(one);
	CntTest->CloseDatabase();
	}


/** 
 * Test Case required for INC078981 "Contacts model field type mathching urgent fix"   
 */
LOCAL_C void TestTemplateFieldTypeMatchingL()
	{
	test.Next(_L("Create template"));
	CreateDatabaseL();
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();

	_LIT(KField1,"Field 1");

	CContactTemplate *contactTemplate=(CContactTemplate *)db->OpenContactLX(db->TemplateId());
	CleanupStack::PushL(contactTemplate);
	TFieldType mapping1(TUid::Uid(KIntContactFieldVCardMapSIPID));
	TFieldType mappingX(TUid::Uid(KIntContactFieldVCardMapSIPID));
	TFieldType mapping2(TUid::Uid(KIntContactFieldVCardMapVOIP));
	TFieldType mapping3(TUid::Uid(3));
	TFieldType mapping4(TUid::Uid(4));
	TFieldType fieldType1(TUid::Uid(KUidContactFieldSIPIDValue));
	TFieldType fieldType2(TUid::Uid(KIntContactFieldVCardMapVOIP));

	AddFieldL(contactTemplate,mapping1,KField1(),&fieldType1,&mappingX);

	AddFieldL(contactTemplate,mapping1,KField1(),&fieldType1,&mapping2);
	AddFieldL(contactTemplate,mapping1,KField1(),&fieldType1,&mapping3);
	AddFieldL(contactTemplate,mapping1,KField1(),&fieldType1,&mapping4);
	
	db->CommitContactL(*contactTemplate);

	CleanupStack::PopAndDestroy(); // contactTemplate
	CleanupStack::Pop(); // contactTemplate.Close()

	TTemplateMapTest tmTest1(mapping1,&fieldType1,&mappingX,NULL,KField1(),1,ETrue);
	TTemplateMapTest tmTest2(mapping1,&fieldType1,&mapping2,NULL,KField1(),2,ETrue);
	TTemplateMapTest tmTest3(mapping1,&fieldType1,&mapping3,NULL,KField1(),3,ETrue);
	TTemplateMapTest tmTest4(mapping1,&fieldType1,&mapping4,NULL,KField1(),4,ETrue);
	TTemplateMapTest tmTest5(mapping1,&fieldType2,&mappingX,NULL,KField1(),1,EFalse);

	contactTemplate=(CContactTemplate *)ReadContactIncHiddenFieldsLC(db,db->TemplateId());
	CheckFieldMappings(db,contactTemplate,&tmTest1);
	CheckFieldMappings(db,contactTemplate,&tmTest3);
	CheckFieldMappings(db,contactTemplate,&tmTest2);
	CheckFieldMappings(db,contactTemplate,&tmTest4);
	CheckFieldMappings(db,contactTemplate,&tmTest5);

	CleanupStack::PopAndDestroy(); // contactTemplate
	CntTest->CloseDatabase();
	}
	

/**
@SYMTestCaseID PIM-T-TEMPL-INC100029-0001
@SYMTestType UT
@SYMTestPriority Critical
@SYMDEF INC100029
@SYMTestCaseDesc Tests that if an attempt is made to commit the System template
with a field which contains data that the commit leaves with KErrNotSupported.

@SYMTestActions
1. Open the System template.
2. Add data to the field KUidContactFieldGivenName.
3. Commit the System template.
4. Check that step 3 leaves with error KErrNotSupported.
   
@SYMTestExpectedResults As per Test Action 4.
*/
LOCAL_C void TestTemplateFieldModificationL()
	{
	test.Next( _L("@SYMTestCaseID:PIM-T-TEMPL-INC100029-0001 Validate template field modification") );

	CContactDatabase* db = CntTest->OpenDatabaseL();

	//
	// 1. Open the System template.
	//
	CContactTemplate* contactTemplate = static_cast<CContactTemplate*>( db->OpenContactLX( db->TemplateId() ) );
	CleanupStack::PushL( contactTemplate );
	
	//
	// 2. Add data to the field KUidContactFieldGivenName.
	//
	SetNameL(*contactTemplate, KUidContactFieldGivenName, _L("Invalid"), EFalse );

	//
	// 3. Commit the System template.
	//
	TRAPD( err, db->CommitContactL( *contactTemplate ) );

	//
	// 4. Check that step 3 leaves with error KErrNotSupported.
	//
	test( err == KErrNotSupported );
	
	CleanupStack::PopAndDestroy(2); // contactTemplate and lock.
	CntTest->CloseDatabase();
	}

LOCAL_C void TestAsyncOpenDbAndAddTemplateL() 
	{
	//test if necesary sanity check was made
	test.Next( _L("Open async database and simulate a template add events"));
	
	TRequestStatus requestStatus(0);
	CContactOpenOperation* op = CContactDatabase::Open(KDatabaseFileName, requestStatus); 
	// note: op doesn't have to be on CS
	User::WaitForRequest(requestStatus);
	CContactDatabase* database = op->TakeDatabase();
	CleanupStack::PushL(database);
	
	//simulate a template add event
	TContactDbObserverEvent event;
	event.iType = EContactDbObserverEventTemplateAdded;
	database->HandleDatabaseEventL(event);

	CleanupStack::PopAndDestroy(database);
	}
	
LOCAL_C void TestAsyncOpenDbAndDeleteTemplateL() 
	{
	//test if necesary sanity check was made
	test.Next( _L("Open async database and simulate a template delete event"));
	
	TRequestStatus requestStatus(0);
	CContactOpenOperation* op = CContactDatabase::Open(KDatabaseFileName, requestStatus); 
	// note: op doesn't have to be on CS
	User::WaitForRequest(requestStatus);
	CContactDatabase* database = op->TakeDatabase();
	CleanupStack::PushL(database);
	
	//simulate a template remove event
	TContactDbObserverEvent event;
	event.iType = EContactDbObserverEventTemplateDeleted;
	database->HandleDatabaseEventL(event);

	CleanupStack::PopAndDestroy(database);
	}

LOCAL_C void TestAsyncOpenDbAndDeleteContactL() 
	{
	//test if necesary sanity check was made
	test.Next( _L("Open async database and simulate a template delete event"));
	
	TRequestStatus requestStatus(0);
	CContactOpenOperation* op = CContactDatabase::Open(KDatabaseFileName, requestStatus); 
	// note: op doesn't have to be on CS
	User::WaitForRequest(requestStatus);
	CContactDatabase* database = op->TakeDatabase();
	CleanupStack::PushL(database);
	
	TContactItemId contactId = 1; //anything but not system template
	database->DeleteContactL(contactId); //aim is to check if it's safe to delete from template
	
	CleanupStack::PopAndDestroy(database);
	}

/**

@SYMTestCaseID PIM-T-TEMPL-0001

*/

void DoTestsL()
    {
	test.Start(_L("@SYMTestCaseID:PIM-T-TEMPL-0001 Create new database"));
	CreateDatabaseL();
	TestTemplateLabelsL();
	TestSystemTemplateL();
	TestSystemTemplateRestorationL();
	TestTemplateContentTypeL();
	TestTemplateL();
	TestMultiTemplateCacheL();
	TestTemplateFieldMatchingL();
	TestSystemTemplateFieldsL();
	TestTemplateReferenceCountingL();
	TestTemplateFieldTypeMatchingL();
	TestTemplateFieldModificationL();
	TestAsyncOpenDbAndAddTemplateL();
	TestAsyncOpenDbAndDeleteTemplateL();
    TestAsyncOpenDbAndDeleteContactL();
	test.Next(_L("Delete database"));
	CntTest->DeleteDatabaseL();
    }

GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	return KErrNone;
    }


















