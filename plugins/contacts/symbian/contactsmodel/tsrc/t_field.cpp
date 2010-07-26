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
#include <s32mem.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include "t_utils.h"
#include "cnttestsynchroniser.h"

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_FIELD"));

const TPtrC KDatabaseFileName=_L("C:T_FIELD");

#define KTestLabelName _L("Label %d")
#define KTestNewLabelName _L("New Label %d")

LOCAL_C void TestIsFullL()
	{
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldCompanyName);
	test(field->TextStorage()->IsFull()==EFalse);
	field->TextStorage()->SetTextL(_L("Should be full now"));
	test(field->TextStorage()->IsFull());
	CleanupStack::PopAndDestroy();	// field
//
	field=CContactItemField::NewLC(KStorageTypeDateTime,KUidContactFieldBirthday);
	test(field->DateTimeStorage()->IsFull()==EFalse);
	field->DateTimeStorage()->SetTime(TTime(123));
	test(field->DateTimeStorage()->IsFull());
	CleanupStack::PopAndDestroy();	// field
	}

void AddThingFieldL(CContactCard* aCard, const TDesC8 *aThingy)
	{
	if (aThingy)
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeStore);
		field->StoreStorage()->SetThingL(*aThingy);
		aCard->AddFieldL(*field);
		CleanupStack::Pop(); // field
		test((*field->StoreStorage()->Thing())==(*aThingy));
		}
	}

void TestThingyStorageL(const TDesC8 *aThingy1,const TDesC8 *aThingy2,const TDesC8 *aThingy3)
	{
	CContactCard* card=CContactCard::NewLC();
	AddThingFieldL(card,aThingy1);
	AddThingFieldL(card,aThingy2);
	AddThingFieldL(card,aThingy3);
// then add the new contact to the database
	TContactItemId id=CntTest->Db()->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
//
	card=STATIC_CAST(CContactCard*,CntTest->Db()->OpenContactL(id));
	CleanupStack::PushL(card);
	CContactItemFieldSet& newFields = card->CardFields();
	HBufC8* testThing=newFields[0].StoreStorage()->Thing();
	test(*testThing==*aThingy1);
	if (aThingy2)
		{
		testThing=newFields[1].StoreStorage()->Thing();
		test(*testThing==*aThingy2);
		if (aThingy3)
			{
			testThing=newFields[2].StoreStorage()->Thing();
			test(*testThing==*aThingy3);
			}
		}
	CleanupStack::PopAndDestroy(); // card
	CntTest->Db()->CloseContactL(id);
	}

HBufC8 *CreateThingyLC(TInt aSize)
	{
	HBufC8 *thingy=NULL;
	if (aSize>0)
		{
		thingy=HBufC8::NewL(aSize);
		TUint8 *ptr=(TUint8 *)thingy->Ptr();
		for(TInt loop=0;loop<aSize;loop++)
			*ptr++=(TUint8)(loop%255);
		thingy->Des().SetLength(aSize);
		}
	CleanupStack::PushL(thingy);
	return(thingy);
	}

void TestThingyStorageBufL(TInt aSize1,TInt aSize2,TInt aSize3)
	{
	HBufC8 *thingy1=CreateThingyLC(aSize1);
	HBufC8 *thingy2=CreateThingyLC(aSize2);
	HBufC8 *thingy3=CreateThingyLC(aSize3);
	TestThingyStorageL(thingy1,thingy2,thingy3);
	CleanupStack::PopAndDestroy(3);	// thingy1,2,3
	}

LOCAL_C void TestThingyFieldsL()
	{
// Test binary storage
	test.Next(_L("binary storage"));

	_LIT8(thingyText1,"Thingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy textThingy ");
	_LIT8(thingyText2,"x");
	_LIT8(thingyText3,"q1");
	TestThingyStorageL(&thingyText1,NULL,NULL);
	TestThingyStorageL(&thingyText2,&thingyText3,NULL);
	TestThingyStorageL(&thingyText2,&thingyText1,&thingyText3);
	for(TInt sizeLoop=5;sizeLoop<100;sizeLoop+=2)
		for(TInt sizeLoop2=0;sizeLoop2<5;sizeLoop2++)
			TestThingyStorageBufL(sizeLoop*sizeLoop+sizeLoop2,sizeLoop2+sizeLoop,sizeLoop2*sizeLoop2+sizeLoop);
	}


LOCAL_C void TestFieldSetL()
//
// test CContactItemFieldSet
//
	{
	test.Next(_L("field set"));

	CContactItemFieldSet* fieldSet=CContactItemFieldSet::NewLC();
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldCompanyName,KUidContactFieldVCardMapORG);
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	test(fieldSet->Count()==2);
	
	TInt index=fieldSet->Find(KUidContactFieldCompanyName);
	test((*fieldSet)[index].ContentType().ContainsFieldType(KUidContactFieldCompanyName));

	index=fieldSet->Find(KUidContactFieldPhoneNumber);
	test((*fieldSet)[index].ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));

	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldCompanyName,KUidContactFieldVCardMapORG);
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	AddFieldL(fieldSet,KStorageTypeDateTime,KUidContactFieldBirthday,KUidContactFieldVCardMapBDAY);
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	index=fieldSet->Find(KUidContactFieldBirthday);
	test(index!=KErrNotFound);
	index=fieldSet->Find(KUidContactFieldCompanyName);
	test(index!=KErrNotFound);
//
	index=fieldSet->FindNext(KUidContactFieldCompanyName,index+1);
	test(index!=KErrNotFound);
	
	index=fieldSet->FindNext(KUidContactFieldCompanyName,index+1);
	test(index==KErrNotFound);
	index=fieldSet->Find(KUidContactFieldPhoneNumber);
	index=fieldSet->FindNext(KUidContactFieldPhoneNumber,index+1);
	test(index!=KErrNotFound);
	
	index=fieldSet->FindNext(KUidContactFieldPhoneNumber,index+1);
	test(index!=KErrNotFound);
	index=fieldSet->FindNext(KUidContactFieldPhoneNumber,index+1);
	test(index!=KErrNotFound);
	index=fieldSet->FindNext(KUidContactFieldPhoneNumber,index+1);
	test(index==KErrNotFound);

	test(fieldSet->Count()==7);
	index=fieldSet->Find(KUidContactFieldCompanyName);
	index=fieldSet->FindNext(KUidContactFieldCompanyName,index+1);
	fieldSet->Remove(index);
	test(fieldSet->Count()==6);
	
	index=fieldSet->Find(KUidContactFieldCompanyName);
	test(fieldSet->FindNext(KUidContactFieldCompanyName,index+1)==KErrNotFound);
	
	fieldSet->Reset();
	test(fieldSet->Count()==0);

	CleanupStack::PopAndDestroy(); // fieldSet

	fieldSet=CContactItemFieldSet::NewL();
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldCompanyName,KUidContactFieldVCardMapUnusedN);
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	test(fieldSet->Count()==2);
	delete fieldSet;
	}

LOCAL_C void StoreRestoreTestL()
	{
	CStreamStore* blobstore=CBufStore::NewLC(3);
	CStreamStore* textstore=CBufStore::NewLC(3);
	CStreamStore* store=CBufStore::NewLC(3);
	RStoreWriteStream stream;
	TStreamId textId=stream.CreateLC(*textstore);
//
	CContactItemFieldSet* fieldSet=CContactItemFieldSet::NewLC();
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldCompanyName,KUidContactFieldVCardMapORG);
	AddFieldL(fieldSet,KStorageTypeText,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	AddFieldL(fieldSet,KStorageTypeDateTime,KUidContactFieldBirthday,KUidContactFieldVCardMapBDAY);
//
	TStreamId streamId=fieldSet->StoreL(*store,stream,*blobstore);
	CleanupStack::PopAndDestroy(); // fieldSet
//
	CContactItemFieldSet* fieldSet2=CContactItemFieldSet::NewLC();
	CContactItemViewDef *viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EMaskHiddenFields );
	RStoreReadStream rstream;
	rstream.OpenLC(*textstore,textId);
	fieldSet2->RestoreL(*store,streamId,blobstore,*viewDef,rstream);
	test(fieldSet2->Count()==3);
	CntTest->TestField((*fieldSet2)[0],KStorageTypeText,KUidContactFieldCompanyName,KUidContactFieldVCardMapORG);
	CntTest->TestField((*fieldSet2)[1],KStorageTypeText,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL);
	CntTest->TestField((*fieldSet2)[2],KStorageTypeDateTime,KUidContactFieldBirthday,KUidContactFieldVCardMapBDAY);
	CleanupStack::PopAndDestroy(7); // viewDef,fieldSet2 stream store textstore rstream,blobstore
//zzz more to do here
	}
LOCAL_C void CommonIdArrayTestsL(CContactIdArray& aIds,TInt aId)
	{
	TInt index=aIds.Find(aId);
	test(index!=KErrNotFound);
	for (TInt ii=0;ii<5;ii++)
		{
		aIds.MoveL(index,ii);
		test(aIds.Count()==5);
		const TInt newPos=aIds.Find(aId);
		test(newPos==ii);
		aIds.MoveL(newPos,index);
		test(aIds.Count()==5);
		test(aIds.Find(aId)==index);
		}
	aIds.Remove(index);
	test(aIds.Count()==4);
	aIds.InsertL(index,aId);
	test(aIds.Count()==5);
	test(aIds.Find(aId)==index);
	test(aIds[index]==aId);
	}


LOCAL_C void TestIdArrayL()
	{
	test.Next(_L("id array"));

	CContactIdArray* ids=CContactIdArray::NewLC();
	test(ids->Count()==0);
	ids->AddL(3);
	ids->AddL(78);
	ids->AddL(1);
	ids->AddL(9);
	ids->AddL(20);
	test(ids->Count()==5);
	
	CommonIdArrayTestsL(*ids,3);
	CommonIdArrayTestsL(*ids,78);
	CommonIdArrayTestsL(*ids,1);
	CommonIdArrayTestsL(*ids,9);
	CommonIdArrayTestsL(*ids,20);
	
	ids->Reset();
	test(ids->Count()==0);
	ids->AddL(1);
	test(ids->Count()==1);
	CleanupStack::PopAndDestroy(); // ids

	ids=CContactIdArray::NewL();
	test(ids->Count()==0);
	ids->AddL(4);
	test(ids->Count()==1);
	delete ids;
	}


LOCAL_C void MainTestsL()
//
// Create a database in a store and add some contact cards
//
	{
	CContactDatabase *db=CntTest->CreateDatabaseL();
	TestFieldSetL();
	TestThingyFieldsL();
	TestIdArrayL();
	CntTest->DeleteAllTemplateFieldsL();
	
	test.Next(_L("Labels"));

	CContactCard* card=CContactCard::NewLC();
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName);
	field->SetLabelL(_L("Test label"));
	field->TextStorage()->SetTextL(_L("x"));
	card->AddFieldL(*field);
	CleanupStack::Pop(); // field
	TContactItemId id=db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	card=STATIC_CAST(CContactCard*,db->OpenContactL(id));
	CleanupStack::PushL(card);
		{
	CContactItemFieldSet& fieldSet3=card->CardFields();
	test(fieldSet3.Count()==1);
	test(fieldSet3[0].Label()==_L("Test label"));
	fieldSet3[0].SetLabelL(_L("New test label"));
		}
	db->CommitContactL(*card);

	CleanupStack::PopAndDestroy(); // card
	card=STATIC_CAST(CContactCard*,db->ReadContactL(id));
		{
	CContactItemFieldSet& fieldSet4=card->CardFields();
	test(fieldSet4.Count()==1);
	test(fieldSet4[0].Label()==_L("New test label"));
		}
	delete card;
	
	test.Next(_L("Attributes"));

	card=CContactCard::NewLC();
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName);
	field->SetHidden(ETrue);
	field->SetReadOnly(ETrue);
	field->SetSynchronize(EFalse);
	field->TextStorage()->SetTextL(_L("x"));
	card->AddFieldL(*field);
	CleanupStack::Pop(); // field
	id=db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
//
	card=STATIC_CAST(CContactCard*,db->ReadContactLC(id));
	CContactItemFieldSet& fieldSet5=card->CardFields();
	test(fieldSet5.Count()==0); // ignore hidden fields by default
	CleanupStack::PopAndDestroy(); // card
//
	card=STATIC_CAST(CContactCard*,db->OpenContactL(id));
	CleanupStack::PushL(card);
	CContactItemFieldSet& fieldSet5b=card->CardFields();
	test(fieldSet5b.Count()==1); // ignore hidden fields by default
	db->CloseContactL(id);
//
	CContactItemViewDef* viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
    viewDef->AddL(KUidContactFieldMatchAll);
	CContactViewDef* def=CContactViewDef::NewLC(viewDef);
    CleanupStack::Pop(); // viewDef
    db->SetViewDefinitionL(def);
    CleanupStack::Pop(); // def
	CleanupStack::PopAndDestroy(); // card
	card=STATIC_CAST(CContactCard*,db->OpenContactL(id));
	CleanupStack::PushL(card);
		{
	CContactItemFieldSet& fieldSet6=card->CardFields();
	test(fieldSet6.Count()==1); // now includes hidden fields
	field=&fieldSet6[0];
	test(field->IsHidden());
	test(field->IsReadOnly());
	test(!field->DoSynchronize());
	field->SetHidden(EFalse);
	field->SetReadOnly(EFalse);
	field->SetSynchronize(ETrue);
	field->SetDisabled(ETrue);
		}
		{
	CContactItemFieldSet& fieldSet7=card->CardFields();
	test(fieldSet7.Count()==1); 
	db->CommitContactL(*card);

	CleanupStack::PopAndDestroy(); // card
	card=STATIC_CAST(CContactCard*,db->ReadContactL(id));
		}
		{
	CContactItemFieldSet& fieldSet8=card->CardFields();
	test(fieldSet8.Count()==1);
	field=&fieldSet8[0];
	test(!field->IsHidden());
	test(!field->IsReadOnly());
	test(field->DoSynchronize());
	test(field->IsDisabled());
	delete card;
		}

	test.Next(_L("Multiple cards"));

	card=CContactCard::NewLC();
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName);
	field->TextStorage()->SetTextL(_L("x"));
	card->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field=&(card->CardFields())[0];
	TContactItemId ids[5];
	TInt ii=0;
	for (ii=0;ii<5;ii++)
		{
		TBuf<16> label;
		label.Format(KTestLabelName,ii);
		field->SetLabelL(label);
		field->SetHidden(ETrue);
		field->SetReadOnly(EFalse);
		field->SetSynchronize(EFalse);
		field->SetDisabled(ii&1);
		ids[ii]=db->AddNewContactL(*card);
		}
	CleanupStack::PopAndDestroy(); // card
	for (ii=0;ii<5;ii++)
		{
		card=STATIC_CAST(CContactCard*,db->OpenContactL(ids[ii]));
		CleanupStack::PushL(card);
		test(card->CardFields().Count()==1);
		field=&(card->CardFields())[0];
		TBuf<16> label;
		label.Format(KTestLabelName,ii);
		test(field->Label()==label);
		test(field->IsHidden());
		test(!field->IsReadOnly());
		test(!field->DoSynchronize());
		TBool disabled=field->IsDisabled();
		test(ii&1?disabled:!disabled);
		// edit card
		label.Format(KTestNewLabelName,ii);
		field->SetLabelL(label);
		field->SetHidden(EFalse);
		field->SetReadOnly(ETrue);
		field->SetSynchronize(ETrue);
		TBool uaf=EFalse;
		if (ii%1)
			uaf=ETrue;
		field->SetUserAddedField(uaf);
		db->CommitContactL(*card);

		CleanupStack::PopAndDestroy(); // card
		}
	for (ii=0;ii<5;ii++)
		{
		card=STATIC_CAST(CContactCard*,db->ReadContactL(ids[ii]));
		test(card->CardFields().Count()==1);
		field=&(card->CardFields())[0];
		TBuf<16> label;
		label.Format(KTestNewLabelName,ii);
		test(field->Label()==label);
		test(!field->IsHidden());
		test(field->IsReadOnly());
		test(field->DoSynchronize());
		TBool uaf=field->UserAddedField();
		test(ii%1?uaf:!uaf);
		delete card;
		}

	test.Next(_L("Multiple fields"));

	card=CContactCard::NewLC();
	for (ii=0;ii<10;ii++)
		{
		field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName);
		TBuf<16> label;
		label.Format(KTestLabelName,ii);
		field->SetLabelL(label);
		field->SetHidden(ETrue);
		field->SetReadOnly(EFalse);
		field->SetSynchronize(ETrue);
		field->TextStorage()->SetTextL(_L("x"));
		card->AddFieldL(*field);
		CleanupStack::Pop(); // field
		}
	id=db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	card=STATIC_CAST(CContactCard*,db->OpenContactL(id));
	CleanupStack::PushL(card);
	for (ii=0;ii<10;ii++)
		{
		field=&(card->CardFields())[ii];
		TBuf<16> label;
		label.Format(KTestLabelName,ii);
		test(field->Label()==label);
		test(field->IsHidden());
		test(!field->IsReadOnly());
		test(field->DoSynchronize());
		// edit fields
		label.Format(KTestNewLabelName,ii);
		field->SetLabelL(label);
		field->SetHidden(EFalse);
		field->SetReadOnly(EFalse);
		field->SetSynchronize(EFalse);
		}
	db->CommitContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	card=STATIC_CAST(CContactCard*,db->ReadContactL(id));
	for (ii=0;ii<10;ii++)
		{
		field=&(card->CardFields())[ii];
		TBuf<16> label;
		label.Format(KTestNewLabelName,ii);
		test(field->Label()==label);
		test(!field->IsHidden());
		test(!field->IsReadOnly());
		test(!field->DoSynchronize());
		}
	delete card;
	CntTest->CloseDatabase();
	}

void FieldIdTestsL()
	{
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CContactCard* card=CContactCard::NewLC();
	card->AddFieldL(*CContactItemField::NewLC(KStorageTypeText,KUidContactFieldCompanyName));
	CleanupStack::Pop(); // New field
	card->AddFieldL(*CContactItemField::NewLC(KStorageTypeText,KUidContactFieldGivenName));
	CleanupStack::Pop(); // New field
	card->AddFieldL(*CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber));
	CleanupStack::Pop(); // New field
//
	TContactItemId id=db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	card=NULL;
//
	card=STATIC_CAST(CContactCard*,db->ReadContactL(id));
	CleanupStack::PushL(card);
	CContactItemFieldSet& fieldSet2=card->CardFields();
	for(TInt loop1=0;loop1<fieldSet2.Count();loop1++)
		for(TInt loop2=0;loop2<fieldSet2.Count();loop2++)
			if (loop1!=loop2)
				test(fieldSet2[loop1].Id()!=fieldSet2[loop2].Id());
	CleanupStack::PopAndDestroy(); // card
	CntTest->CloseDatabase();
	}

const TUid KExtraUid1={0x1};
const TUid KExtraUid2={0x2};
const TUid KExtraUid3={0x12345678};

void AdditionalUidTests1L()
	{
	CContactItemFieldSet* fieldSet=CContactItemFieldSet::NewLC();
//
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldCompanyName);
	field->AddFieldTypeL(KExtraUid1);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KExtraUid2);
	field->AddFieldTypeL(KUidContactFieldAdditionalName);
	field->AddFieldTypeL(KUidContactFieldAddress);
	field->AddFieldTypeL(KExtraUid3);
	fieldSet->AddL(*field);
	CleanupStack::Pop(); // field
//
	test((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactFieldCompanyName));
	test((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
	test((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactFieldAdditionalName));
	test((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactFieldAddress));
	test((*fieldSet)[0].ContentType().ContainsFieldType(KExtraUid1));
	test((*fieldSet)[0].ContentType().ContainsFieldType(KExtraUid2));
	test((*fieldSet)[0].ContentType().ContainsFieldType(KExtraUid3));
	CleanupStack::PopAndDestroy(); // fieldSet
	}

void AdditionalUidTests2L()
	{
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CContactCard* card=CContactCard::NewLC();
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KExtraUid1);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KExtraUid2);
	field->AddFieldTypeL(KUidContactFieldAdditionalName);
	field->AddFieldTypeL(KExtraUid3);
	field->AddFieldTypeL(KUidContactFieldAddress);
	field->TextStorage()->SetTextL(_L("x"));
	card->AddFieldL(*field);
	CleanupStack::Pop(); // field
//
	TContactItemId id=db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); // card
	card=NULL;
//
	card=STATIC_CAST(CContactCard*,db->ReadContactL(id));
	CleanupStack::PushL(card);
	CContactItemFieldSet& fieldSet2=card->CardFields();
	test(fieldSet2[0].ContentType().ContainsFieldType(KExtraUid1));
	test(fieldSet2[0].ContentType().ContainsFieldType(KExtraUid2));
	test(fieldSet2[0].ContentType().ContainsFieldType(KExtraUid3));
	test(fieldSet2[0].ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
	test(fieldSet2[0].ContentType().ContainsFieldType(KUidContactFieldAdditionalName));
	test(fieldSet2[0].ContentType().ContainsFieldType(KUidContactFieldAddress));

	CleanupStack::PopAndDestroy(); // card
//
	CntTest->CloseDatabase();
	}

/** write fields to the card and read them back again */

void AdditionalUidTests3L()
	{
	const TInt KNumberOfFields=6;
	const TInt KFirstPronunciationField=3;

	_LIT(KCoName,"Symbian Ltd.");
	_LIT(K1stName,"John");
	_LIT(KLastName,"Doe");
	_LIT(KFormatCo,"%S\n");
	_LIT(KFormat1st,"%S ");
	_LIT(KFormatLast,"%S from ");
	_LIT(KFormatPronunciation,"<%S>");
	_LIT(KCoRead,"Sim'bee'an");
	_LIT(K1stRead,"Jon");
	_LIT(KLastRead,"Dough");

	// do this so we can just loop rather than repeat code many times
	const void* names[KNumberOfFields] = { &K1stName, &KLastName, &KCoName, &K1stRead, &KLastRead,&KCoRead};
	const void* formats[] = {&KFormat1st, &KFormatLast, &KFormatCo};
	TInt fieldIds[KNumberOfFields] = {KUidContactFieldGivenNameValue, KUidContactFieldFamilyNameValue, KUidContactFieldCompanyNameValue, 
		KUidContactFieldGivenNamePronunciationValue, KUidContactFieldFamilyNamePronunciationValue, KUidContactFieldCompanyNamePronunciationValue};
	test.Next(_L("Pronunciation fields"));


	
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CContactCard* card=CContactCard::NewLC();
	CContactItemField* field;

	TInt i=0;	
	for(i=0;i<KNumberOfFields;i++)
		{	// write each field to the card as a text type
		field=CContactItemField::NewLC(KStorageTypeText,TUid::Uid(fieldIds[i]));
		field->TextStorage()->SetTextL(* ((const TDesC*) names[i]));
		card->AddFieldL(*field);
		CleanupStack::Pop(field); 
		}
	
	TContactItemId id=db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(card);
	card=NULL;
 
	card=STATIC_CAST(CContactCard*,db->ReadContactL(id));
	CleanupStack::PushL(card);
	CContactItemFieldSet& fieldSet=card->CardFields();

	// alternate printing pronunciation then name
	for(i=0;i<KNumberOfFields;i++)
		{
		const TInt delta = (i%2) ? 0: KFirstPronunciationField;
		const TInt index = fieldSet.Find( TUid::Uid(fieldIds[i/2+delta]));
		test(index!=KErrNotFound);		// field should be there
		TPtrC text(fieldSet[index].TextStorage()->Text());
		test(text.Compare(* ((const TDesC*) names[i/2+delta]))==0); // content should be what we expect
		TPtrC format(KFormatPronunciation);
		if(i%2) format.Set(* ((const TDesC*) formats[i/2]));
		test.Printf(format ,&text);	// print this just to make sure
		}
	CleanupStack::PopAndDestroy(card); 
	CntTest->CloseDatabase();
	}


void AdditionalUidTestsL()
	{
	AdditionalUidTests1L();
	AdditionalUidTests2L();
	AdditionalUidTests3L();
	}

/**

@SYMTestCaseID     PIM-T-FIELD-0001

*/

void DoTestsL()
    {
    CContactSyncChecker::ResetL();

	test.Start(_L("@SYMTESTCaseID:PIM-T-FIELD-0001 Test fields"));

    CContactSyncChecker::ResetL();

	TRAPD(ret,MainTestsL());
	test.Printf(_L("MainTestsL() returned with %d\n"), ret);
	test(ret==KErrNone);
//
	test.Next(_L("Is Full"));

	TRAP(ret,TestIsFullL());
	test(ret==KErrNone);
//
	test.Next(_L("Field Ids"));

	TRAP(ret,FieldIdTestsL());
	test(ret==KErrNone);
//
	test.Next(_L("Store restore test"));

	TRAP(ret,StoreRestoreTestL());
	test(ret==KErrNone);
//
	test.Next(_L("Additional Uids"));

	TRAP(ret,AdditionalUidTestsL());
	test(ret==KErrNone);
//
	test.Next(_L("Delete database"));

	TRAP(ret,CntTest->DeleteDatabaseL());
	test(ret==KErrNone);
    }

GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	return KErrNone;
    }
