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
#include <cntfldst.h>
#include <e32def.h>
#include "t_vers.h"
#include "t_utils.h"
#include <coreappstest/testserver.h>

enum ImpExpTestFlags
	{
	EImpExpTestMultipleEntries=0x1,
	EImpExpTestModifyEntries=0x2,
	EImpExpTestExportExtraEntry=0x4,
	};

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_VERS"));
LOCAL_D CContactIdArray* TheIds;

LOCAL_D CArrayPtr<CContactItem>* TheItems;
LOCAL_D CArrayPtr<CContactItem>* TheItems2;
LOCAL_D CArrayPtr<CContactItem>* TheItems3;
LOCAL_D CArrayPtr<CContactItem>* TheItems4;

const TPtrC KDatabaseFileName=_L("C:T_VERS");

_LIT(KVCardFileToImport,"c:\\t_vers.vcf");

//
// CVersitTest
//

CVersitTest::CVersitTest()
	{}

CVersitTest::~CVersitTest()
	{
	delete iParser;
	delete iStore;
	}

void CVersitTest::StreamInL()
//Convert the iVersit into stream format, save it as "VersitIn"
//and internalize it as a CVersitParser
	{
	CFileStore* store = CDirectFileStore::ReplaceLC(CntTest->Fs(),_L("c:\\Versit2"),EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	TInt length=iVersit.Length();
	for (TInt ii=0; ii<length; ii++)
		outstream.WriteInt8L(iVersit[ii]);
	outstream.CommitL();
	CleanupStack::PopAndDestroy();
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::PopAndDestroy();
	store = CDirectFileStore::OpenLC(CntTest->Fs(),_L("c:\\Versit2"),EFileRead); //retrieve stream
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	iParser->InternalizeL(instream);
	CleanupStack::PopAndDestroy(2); //store + stream
	}

void CVersitTest::StreamLCC(RReadStream& aStream, TInt aMode, TInt aContactId)
	{
	switch(aMode)
		{
		case 1:
			GetSampleVersitL();
			break;
		case 2:
			GetSampleVersit2L(aContactId);
			break;
		case 3:
			GetSampleVersit3L();
			break;
		}
	TFileName versitIn;
	versitIn.Format(_L("c:\\VersitIn%d"),aMode);
	CFileStore* store = CDirectFileStore::ReplaceLC(CntTest->Fs(),versitIn,EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	TInt length=iVersit.Length();
	for (TInt ii=0; ii<length; ii++)
		outstream.WriteInt8L(iVersit[ii]);
	outstream.CommitL();
	CleanupStack::PopAndDestroy();
	store->SetRootL(id);
	store->CommitL();
 	CleanupStack::PopAndDestroy();	// store
	store = CDirectFileStore::OpenLC(CntTest->Fs(),versitIn,EFileRead); //retrieve stream
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	CleanupStack::Pop();	// instream
	aStream=RReadStream(instream);
	}

void CVersitTest::StreamOutL()
	{
	}

void CVersitTest::SaveFileVersitL()
// Save iParser into VersitFile	
	{
	if (iParser)
		{
		User::LeaveIfError(CntTest->Fs().Connect());
		RFile file;
		if (file.Replace(CntTest->Fs(),_L("C:\\VersitFile"),EFileWrite)==KErrNone)
			iParser->ExternalizeL(file);
		}
	}

//
// CVCardTest
//

void CVCardTest::LoadFileVersitL()
// Read VCard.vcf into iVersit	
	{
	iParser=CParserVCard::NewL();
	User::LeaveIfError(CntTest->Fs().Connect());
	RFile file;
	if (file.Open(CntTest->Fs(),_L("C:\\VCard.vcf"),EFileRead)==KErrNone)
		{		
		TInt start=0;
		iParser->InternalizeL(file,start);
		}
	}
	
LOCAL_C void DisplayField(TInt aFieldNumber, const CContentType &aContentType, const TDesC &aDes)
	{
	TBuf<128> out;
	out.Format(_L("Field %d mapped to %x {"),aFieldNumber,aContentType.Mapping());
	for(TInt loop=0;loop<aContentType.FieldTypeCount();loop++)
		{
		if (loop>0)
			out.Append(TChar(','));
		out.AppendFormat(_L("%x"),aContentType.FieldType(loop));
		}
	out.AppendFormat(_L("} is %S store type"),&aDes);
	test.Printf(out);
	}

LOCAL_C void DisplayDatabase()
	{
	TContactItemId theid;
	CContactItem* contactItem=NULL;
	TContactIter iter(*CntTest->Db());
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		test.Printf(_L("\n========================ROW ID %d"),theid);
		contactItem=CntTest->Db()->ReadContactLC(theid);
		CContactItemFieldSet& fieldSet=contactItem->CardFields();
		TInt fieldcount=fieldSet.Count();
		test.Printf(_L(" %d fields"),fieldcount);
		for (TInt ii=0;ii<fieldcount;ii++)
			{
	 		const CContentType &content=fieldSet[ii].ContentType();
			test.Printf(_L("\nLabel ["));
			test.Printf(fieldSet[ii].Label());
			test.Printf(_L("]\n"));
			switch(fieldSet[ii].StorageType())
				{
			case KStorageTypeText: 
				{
				DisplayField(ii, content, _L("Text"));
				test.Printf(_L("\n"));
				TPtrC fieldText=fieldSet[ii].TextStorage()->Text();
//				test.Printf(fieldText);
				test.Printf(_L("\tLength = %d"),fieldText.Length());
				}
				break;
			case KStorageTypeStore:
				DisplayField(ii, content, _L("Store"));
				break;
			case KStorageTypeDateTime:
				DisplayField(ii, content, _L("DateTime"));
				break;
			case KStorageTypeContactItemId:
				DisplayField(ii, content, _L("ContactItemId"));
				break;
			default:
				DisplayField(ii, content, _L("unknown"));
				break;
				}
//			test.Getch();
			}
	 	theid=iter.NextL();
		CleanupStack::PopAndDestroy(); // contactItem
		}
	test.Printf(_L("\nEnd of the database\n"));
	}
	
LOCAL_C void AddLocalLabels()
	{
	TContactItemId theid;
	TContactIter iter(*CntTest->Db());
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		CContactItem* contactItem=CntTest->Db()->OpenContactL(theid);
		CleanupStack::PushL(contactItem);
		CContactItemFieldSet& fieldSet=contactItem->CardFields();
		TInt fieldcount=fieldSet.Count();
		for (TInt ii=0;ii<fieldcount;ii=ii+2)
			{
	 		TPtrC labelName(_L("aTestLabelName"));
			fieldSet[ii].SetLabelL(labelName);
			}
	 	theid=iter.NextL();
		CntTest->Db()->CommitContactL(*contactItem);
		CleanupStack::PopAndDestroy();	// contactitem
		}				
	}

LOCAL_C void DecAccessCount()
	{
	TContactItemId theid;
	TContactIter iter(*CntTest->Db());
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		CContactItem* contactItem=CntTest->Db()->OpenContactL(theid);
		CleanupStack::PushL(contactItem);
		contactItem->DecAccessCount();
		CntTest->Db()->CommitContactL(*contactItem);
		theid=iter.NextL();
		CleanupStack::PopAndDestroy();	// contactitem
		}				
	}

/*
LOCAL_C void RemoveBlankFields(CContactItem &aItem)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	for(TInt loop1=0;loop1<fieldSet.Count();)
		{
		if (fieldSet[loop1].Storage()->IsFull())
			loop1++;
		else
			fieldSet.Remove(loop1);
		}
	}

LOCAL_C void TestExportCard(CContactItem &aItem, const CContactItem &aOriginal)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	CContactCard *origCopy=CContactCard::NewLC(&aOriginal);
//
	RemoveBlankFields(*origCopy);
	RemoveBlankFields(aItem);
	CContactItemFieldSet& origFieldSet=origCopy->CardFields();
//
	test(fieldSet.Count()==origFieldSet.Count());
	for(TInt loop=0;loop<fieldSet.Count();loop++)
		{
		CContactItemField& field=fieldSet[loop];
		CContactItemField& origField=origFieldSet[loop];
		switch(field.StorageType())
			{
			case KStorageTypeText:
				{
				TPtrC fieldTxt(field.TextStorage()->Text());
				TPtrC origFieldTxt(origField.TextStorage()->Text());
				test(fieldTxt==origFieldTxt);
				}
				break;
			case KStorageTypeDateTime:
				{
				const TDateTime dt1(field.DateTimeStorage()->Time().DateTime());
				const TDateTime dt2(origField.DateTimeStorage()->Time().DateTime());
				test(dt1.Year()==dt2.Year());
				test(dt1.Month()==dt2.Month());
				test(dt1.Day()==dt2.Day());
				}				
				break;
			case KStorageTypeStore:
			case KStorageTypeContactItemId:
				break;
			}
		CntTest->CompareFields(field,origField);
		}
	CleanupStack::PopAndDestroy();	// origCopy
	}

LOCAL_C void SetTestField(CContactItemFieldSet &aFieldSet,TInt aIndex, TBool aIsModifiedVersion)
	{
	CContactItemField& field=aFieldSet[aIndex];
	switch(field.StorageType())
		{
		case KStorageTypeText:
			{
			TBuf<16> buf;
			if (aIsModifiedVersion)
				buf.Format(_L("TxtMod[%d]"),aIndex);
			else
				buf.Format(_L("Txt[%d]"),aIndex);
			field.TextStorage()->SetTextL(buf);
			}
			break;
		case KStorageTypeDateTime:
			if (aIsModifiedVersion)
				field.DateTimeStorage()->SetTime(TTime(aIndex*1000000));
			else
				field.DateTimeStorage()->SetTime(TTime(aIndex*2000000));
			break;
		case KStorageTypeStore:
		case KStorageTypeContactItemId:
			break;
		}
	}

LOCAL_C void DuplicateTestField(CContactItemFieldSet &aFieldSet,TInt aIndex)
	{
	CContactItemField *field=CContactItemField::NewLC(aFieldSet[aIndex]);
	aFieldSet.InsertL(aIndex+1,*field);
	CleanupStack::Pop();	// field
	}

LOCAL_C void SetTestFields(CContactItem &aContact, TInt aTestNum, TUint aTestFlags, TBool aIsModifiedVersion)
	{
	if (aTestFlags&EImpExpTestMultipleEntries)
		{
		for(TInt loop=aTestNum;loop<aContact.CardFields().Count();loop+=(aTestNum+1))
			SetTestField(aContact.CardFields(),loop,aIsModifiedVersion);
		}
	else
		SetTestField(aContact.CardFields(),aTestNum,aIsModifiedVersion);
	}

LOCAL_C CContactItem* ReadContactIncHiddenFieldsLC(CContactDatabase* aDb, TContactItemId aContactId)
	{
	CContactItemViewDef *viewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EMaskFields,CContactItemViewDef::EIncludeHiddenFields);
	CContactItem *contact=aDb->ReadContactL(aContactId,*viewDef);
	CleanupStack::PopAndDestroy();	// viewDef
	CleanupStack::PushL(contact);
	return(contact);
	}

LOCAL_C void doTestExportImportContactL(CContactItem *aTemplateItem, TInt aTestNum, TUint aTestFlags)
	{
	CContactItem *originalContact=CContactCard::NewLC(aTemplateItem);
	SetTestFields(*originalContact,aTestNum,aTestFlags,EFalse);
	TContactItemId addId=CntTest->Db()->AddNewContactL(*originalContact);
	CContactIdArray *ids=CContactIdArray::NewLC();
	ids->AddL(addId);
	const TInt KMaxFormats=8;
	for(TInt format=0;format<KMaxFormats;format++)
		{
		TUint formatFlags=0;
		if (format&0x1)
			formatFlags|=CContactDatabase::EIncludeX;
		if (format&0x2)
			formatFlags|=CContactDatabase::ETTFormat;
		if (format&0x4)
			{
			if (aTestFlags&EImpExpTestModifyEntries)
				continue;
			formatFlags|=CContactDatabase::EExcludeUid;
			}
		else if (!(formatFlags&CContactDatabase::ETTFormat))
			continue;
		CVCardTestStore* store=CntTest->ExportContactsLC(ids,formatFlags);
//
		if (aTestFlags&EImpExpTestModifyEntries)
			{
			CContactItem* modifyItem=CntTest->Db()->OpenContactLX(originalContact->Id());
			CleanupStack::PushL(modifyItem);
			SetTestFields(*modifyItem,aTestNum,aTestFlags,ETrue);
			CntTest->Db()->CommitContactL(*modifyItem);
			CleanupStack::PopAndDestroy(2);	// modifyItem,modifyItem->Close()
			}
		if (aTestFlags&EImpExpTestExportExtraEntry)
			{
			CContactItem* modifyItem=CntTest->Db()->OpenContactLX(originalContact->Id());
			CleanupStack::PushL(modifyItem);
			SetTestFields(*modifyItem,(aTestNum+5)%10,0,EFalse);
			SetTestFields(*modifyItem,(aTestNum+1)%10,0,EFalse);
			CntTest->Db()->CommitContactL(*modifyItem);
			CleanupStack::PopAndDestroy(2);	// modifyItem,modifyItem->Close()
			}
//
		CArrayPtr<CContactItem>* items=CntTest->ImportContactsLC(store,formatFlags);
		test(items->Count()==1);
		if (!(formatFlags&CContactDatabase::EExcludeUid))
			TestExportCard(*(*items)[0],*originalContact);
		TContactItemId addedId=(*items)[0]->Id();
		CleanupStack::PopAndDestroy();	// items->ResetAndDestroy()
		CContactItem *reloadedItem=ReadContactIncHiddenFieldsLC(CntTest->Db(),addedId);
		TestExportCard(*reloadedItem,*originalContact);
		CleanupStack::PopAndDestroy();	// reloadedItem
		CleanupStack::PopAndDestroy();	// store
		}
	CleanupStack::PopAndDestroy();	// ids
	CleanupStack::PopAndDestroy();	// originalContact
//
	CContactIdArray *deleteIds=CContactIdArray::NewLC(CntTest->Db()->SortedItemsL());
	CntTest->Db()->DeleteContactsL(*deleteIds);
	CleanupStack::PopAndDestroy();	// deleteIds
//
	if (CntTest->Db()->CompressRequired())
		CntTest->Db()->CompressL();
	test.Printf(_L("."));	// Just to show some life
	}
*/


/**
LOCAL_C void TestExportImportContactL()
	{
	test.Next(_L("Export/Import test"));

	CContactDatabase *db=CntTest->CreateDatabaseL();
	CContactItem *templateItem=ReadContactIncHiddenFieldsLC(db,db->TemplateId());
	TInt templateFieldCount=templateItem->CardFields().Count();
//	Create an contact with all template entries doubled up
	CContactItem *templateItem2=ReadContactIncHiddenFieldsLC(db,db->TemplateId());
const TInt KNonDuplicateFields=5;	// Names can not be duplicated
	for(TInt dblLoop=KNonDuplicateFields;dblLoop<templateFieldCount;dblLoop++)
		DuplicateTestField(templateItem2->CardFields(),dblLoop*2-KNonDuplicateFields);
//	Create an contact with just the fourth template entry doubled up
	CContactItem *templateItem3=ReadContactIncHiddenFieldsLC(db,db->TemplateId());
	DuplicateTestField(templateItem3->CardFields(),7);
// Test all the fields 1 at a time
	for(TInt loop1=0;loop1<templateFieldCount;loop1++)
		{
		doTestExportImportContactL(templateItem,loop1,0);
		doTestExportImportContactL(templateItem2,loop1,0);
		doTestExportImportContactL(templateItem3,loop1,0);
//		doTestExportImportContactL(templateItem,loop1,EImpExpTestExportExtraEntry);
//		doTestExportImportContactL(templateItem,loop1,EImpExpTestModifyEntries);
//		doTestExportImportContactL(templateItem2,loop1,EImpExpTestModifyEntries);
//		doTestExportImportContactL(templateItem3,loop1,EImpExpTestModifyEntries);
		}
// Test various combinations of fields being set
// (Mode 0 will test a card with all fields set)
	doTestExportImportContactL(templateItem2,0,EImpExpTestMultipleEntries|EImpExpTestModifyEntries);
	for(TInt testNum=0;testNum<templateFieldCount;testNum++)
		{
		doTestExportImportContactL(templateItem,testNum,EImpExpTestMultipleEntries);
		doTestExportImportContactL(templateItem2,testNum,EImpExpTestMultipleEntries);
		doTestExportImportContactL(templateItem3,testNum,EImpExpTestMultipleEntries);
		doTestExportImportContactL(templateItem,testNum,EImpExpTestMultipleEntries|EImpExpTestModifyEntries);
//		doTestExportImportContactL(templateItem2,testNum,EImpExpTestMultipleEntries|EImpExpTestModifyEntries);
//		doTestExportImportContactL(templateItem3,testNum,EImpExpTestMultipleEntries|EImpExpTestModifyEntries);
//		doTestExportImportContactL(templateItem,testNum,EImpExpTestMultipleEntries|EImpExpTestExportExtraEntry);
		}
//
	CleanupStack::PopAndDestroy(3);	// templateItem,templateItem2,templateItem3
	CntTest->CloseDatabase();
	}
*/

// modified since vCards which don't exist in the database are now added rather than leaving
// this is necessary so that vcards of contacts with deleted uids are added.
LOCAL_C void SynchWithNonExistentRecordL()
	{
	CntTest->CreateDatabaseL();
	TContactItemId addId1=AddContactL(CntTest->Db(),KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add1"));
//
	CVCardTestStore* store=CntTest->ExportContactLC(addId1,CContactDatabase::ETTFormat);
	CntTest->DeleteContact(addId1);
	CArrayPtr<CContactItem>* items=NULL;
	TRAPD(err,	items=CntTest->ImportContactsL(store,CContactDatabase::ETTFormat));
	test(err==KErrNone);
	items->ResetAndDestroy();
	delete items;
//
	CleanupStack::PopAndDestroy();	// store
	CntTest->CloseDatabase();
	}

/*
LOCAL_C void MergeBlankVCardL(TContactItemId aId)
	{
	CVCardTest *vtest=new(ELeave) CVCardTest;
	CleanupStack::PushL(vtest);
	RReadStream stream;
	vtest->StreamLCC(stream,2,aId);
	stream.PushL();
	TBool success;
	CArrayPtr<CContactItem>* items=CntTest->Db()->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),stream,success,0);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,items));
	test(success);
	CleanupStack::PopAndDestroy(4);	// stream,store,vtest,items
	}
*/

// This test doesn't do anything useful anymore - since vCalendar import to 
// change deleted contacts creates new contacts rather than updating them.
//

#if defined(_DEBUG)
LOCAL_C void BlankImportL()
	{
	CntTest->CreateDatabaseL();
	CntTest->Db()->OverrideMachineUniqueId(0); //testcode assumes that HAL would return 0 
	TContactItemId addId1=AddContactL(CntTest->Db(),KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add1"));
	TContactItemId addId2=AddContactL(CntTest->Db(),KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add2"));
	CntTest->AdjustContactAccessCountL(addId1,1);
	CntTest->AdjustContactAccessCountL(addId2,1);
//
	CntTest->DeleteContact(addId1);
	CntTest->CheckDeletedContact(addId1);
//	MergeBlankVCardL(addId1);
	CntTest->CheckDeletedContact(addId1);
	CntTest->AdjustContactAccessCountL(addId1,-1);
	CntTest->CheckContactDoesNotExist(addId1);
//
//	MergeBlankVCardL(addId2);
	CntTest->DeleteContact(addId2);
	CntTest->CheckDeletedContact(addId2);
	CntTest->AdjustContactAccessCountL(addId2,-1);
	CntTest->CheckContactDoesNotExist(addId2);
//
	CntTest->CloseDatabase();
	}

LOCAL_C void ExportImportDifferentMachineIds()
	{
	CntTest->CreateDatabaseL();
	CntTest->Db()->OverrideMachineUniqueId(0x1);
	TContactItemId addId1=AddContactL(CntTest->Db(),KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add1"));
	HBufC *uid1=CntTest->ContactUidLC(addId1,0x1);
	CVCardTestStore* store=CntTest->ExportContactLC(addId1,CContactDatabase::ETTFormat);
	TInt oldCount=CntTest->Db()->CountL();
//
	CArrayPtr<CContactItem>* items=CntTest->ImportContactsLC(store,CContactDatabase::ETTFormat);
	test(items->Count()==1);
	CleanupStack::PopAndDestroy(2);	// store,items
	test(oldCount==CntTest->Db()->CountL());
//
	store=CntTest->ExportContactLC(addId1,CContactDatabase::ETTFormat);
	CntTest->Db()->OverrideMachineUniqueId(0x2);
	items=CntTest->ImportContactsLC(store,CContactDatabase::ETTFormat);
	test(items->Count()==1);
	TContactItemId importId=(*items)[0]->Id();
	test((oldCount+1)==CntTest->Db()->CountL());
	HBufC *uid2=CntTest->ContactUidLC(importId,0x2);
	test(*uid1==*uid2);
	CleanupStack::PopAndDestroy(3);	// store,items,uid2
//
	store=CntTest->ExportContactLC(addId1,CContactDatabase::ETTFormat);
	items=CntTest->ImportContactsLC(store,CContactDatabase::ETTFormat);
	test(items->Count()==1);
	CleanupStack::PopAndDestroy(2);	// store,items
	test((oldCount+1)==CntTest->Db()->CountL());
//
	store=CntTest->ExportContactLC(importId,CContactDatabase::ETTFormat);
	items=CntTest->ImportContactsLC(store,CContactDatabase::ETTFormat);
	test(items->Count()==1);
	CleanupStack::PopAndDestroy(2);	// store,items
	test((oldCount+1)==CntTest->Db()->CountL());
//
	CleanupStack::PopAndDestroy();	// uid1
	CntTest->CloseDatabase();
	}

#endif //(_DEBUG)

LOCAL_C void DoTests1L()
	{
	// create test database
	TheIds=CContactIdArray::NewLC();
	CVersitTest* vtest=NULL;
	vtest=new(ELeave)CVCardTest;
	CleanupStack::PushL(vtest);
	CVersitTest* vtest2=NULL;
	vtest2=new(ELeave)CVCardTest;
	CleanupStack::PushL(vtest2);
	//Import
	test.Next(_L("Importing vcard"));


	CContactDatabase *db=CntTest->CreateDatabaseL();
	test.Next(_L(""));

//	TInt64 machineUid = db->MachineId();
	
	
	TBool success;
	TUid vcardmode;
	vcardmode.iUid=KUidVCardConvDefaultImpl;
	RReadStream stream;
	vtest->StreamLCC(stream,1,-1);
	stream.PushL();
	TheItems=db->ImportContactsL(vcardmode,stream,success,CContactDatabase::EIncludeX|CContactDatabase::EImportSingleContact|CContactDatabase::EIncreaseAccessCount);
	test(success);
	test(db->CountL()==2);  // the first contact has an agent, that makes it 2
	TheItems2=db->ImportContactsL(vcardmode,stream,success,CContactDatabase::EIncludeX|CContactDatabase::EIncreaseAccessCount);
	test(success);
	
	AddLocalLabels();
	CleanupStack::PopAndDestroy(2);	// stream,store
	test(db->CountL()==3);
	DisplayDatabase();
	//Export
	test.Next(_L("Exporting vcard"));

	TStreamId id;
	CFileStore* store ;
	RStoreWriteStream outstream;
	TUid uid;
	/*= CDirectFileStore::ReplaceLC(CntTest->Fs(),_L("Versitout"),EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	//TUid uid;
	uid.iUid=KUidVCardConvDefaultImpl;
	TInt itemCount=TheItems->Count();
	for (TInt ii=0;ii<itemCount;ii++)
		TheIds->AddL((*TheItems)[ii]->Id());
	db->ExportSelectedContactsL(uid,*TheIds,outstream,CContactDatabase::EDefault);
	outstream.CommitL();
	store->SetRootL(id);
	store->CommitL();  	
	CleanupStack::PopAndDestroy(2); // store+ oustream*/
	//Update 2 (delete record 0)
	test.Next(_L("Updating vcard 2"));

	RReadStream stream2;
	vtest2->StreamLCC(stream2,2,1);
	stream2.PushL();
	TheItems3=db->ImportContactsL(vcardmode,stream2,success,CContactDatabase::EIncreaseAccessCount);
	test(success);
	CleanupStack::PopAndDestroy(2);	// stream,store
	test(db->CountL()==2);
//
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(1);
	CleanupStack::PushL(sortOrder);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName,CContactDatabase::TSortPref::EAsc));
	db->SortL(sortOrder);
	CleanupStack::Pop();	// sortOrder
	sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(1);
	CleanupStack::PushL(sortOrder);
	db->SortL(sortOrder);
	CleanupStack::Pop();	// sortOrder
//
	DecAccessCount();
	test(db->CountL()==2);
	//Export again
	test.Next(_L("Exporting vcard"));

	TheIds->Reset();
	TheIds->AddL(2);// zzz shouldn't be hardwired value
	store = CDirectFileStore::ReplaceLC(CntTest->Fs(),_L("c:\\Versitoutb"),EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	id = outstream.CreateLC(*store);
	uid.iUid=KUidVCardConvDefaultImpl;
	db->ExportSelectedContactsL(uid,*TheIds,outstream,CContactDatabase::EIncludeX);
	outstream.CommitL();
	store->SetRootL(id);
	store->CommitL();  	
	CleanupStack::PopAndDestroy(2); // store+ oustream

	if (TheItems)
		TheItems->ResetAndDestroy();
	delete TheItems;
	if (TheItems2)
		TheItems2->ResetAndDestroy();
	delete TheItems2;
	if (TheItems3)
		TheItems3->ResetAndDestroy();
	delete TheItems3;
	
	CleanupStack::PopAndDestroy(2); // vtest, vtest2
	CleanupStack::PopAndDestroy(); //TheIds
//
	CVCardTest *vtest3=new(ELeave) CVCardTest;
	CleanupStack::PushL(vtest3);
	test.Next(_L("Multiple card import"));

	RReadStream stream3;
	vtest3->StreamLCC(stream3,3,-1);
	stream3.PushL();
	TInt oldCount=db->CountL();
	TheItems4=db->ImportContactsL(vcardmode,stream3,success,CContactDatabase::EIncreaseAccessCount);
	test(success);
	CleanupStack::PopAndDestroy(2);	// stream3,store
	test(TheItems4->Count()==2);
	test(db->CountL()==(oldCount+TheItems4->Count()));
	CleanupStack::PopAndDestroy(); // vtest3
//
	TheItems4->ResetAndDestroy();
	delete TheItems4;
	CntTest->CloseDatabase();
	}

LOCAL_C void TestIncludeXL()
	{
	CContactDatabase *db=CntTest->CreateDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
	CContactCard *item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("FamilyName"),ETrue);
	SetNameL(*item,KUidContactFieldNote,KUidContactFieldVCardMapNOTE,_L("Note"),ETrue);
	SetNameL(*item,KUidContactFieldJobTitle,KUidContactFieldVCardMapTITLE,_L("Job Title"),ETrue);
	SetNameL(*item,KUidContactFieldPhoneNumber,KUidContactFieldVCardMapTEL,_L("Tel"),ETrue);
	CContactItemFieldSet& fieldSet=item->CardFields();
	fieldSet[3].SetHidden(ETrue);	// zzz if set to 0 fails, names don't support hidden flag
	fieldSet[1].SetReadOnly(ETrue);
	fieldSet[2].SetHidden(ETrue);
	fieldSet[2].SetReadOnly(ETrue);
	fieldSet[0].SetLabelL(_L("Field 0 Label"));
	fieldSet[1].SetLabelL(_L("Field 1 Label"));
	fieldSet[2].SetLabelL(_L("Field 2 Label"));
	fieldSet[3].SetLabelL(_L("Field 3 Label"));
	TContactItemId addId1=db->AddNewContactL(*item);
//
	CVCardTestStore* store=CntTest->ExportContactLC(addId1,CContactDatabase::EIncludeX|CContactDatabase::EExcludeUid,KVCardStoreTypeFile,9);
	
	CArrayPtr<CContactItem>* importedItems=CntTest->ImportContactsLC(store,CContactDatabase::EIncludeX|CContactDatabase::EExcludeUid);
	test(CompareItemFields((*importedItems)[0],item));
	CleanupStack::PopAndDestroy(2);	// importedItems,store
//
	CleanupStack::PopAndDestroy();	// item
//
	CntTest->CloseDatabase();
	}

void createTestVCardL(RFs& aFs)
	{
	RFile file;
	TInt err=file.Replace(aFs, KVCardFileToImport, EFileWrite+EFileShareAny+EFileStreamText);
	User::LeaveIfError(err);
	TPtrC8 content((const TText8*)	"BEGIN:VCARD\r\n"
									"VERSION:2.1\r\n"
									"REV:20020520T134824Z\r\n"
									"N:smith;john;;;\r\n"
									"TEL;HOME;VOICE:020 8450 1537\r\n"
									"END:VCARD\r\n");
	file.Write(content);
	file.Close();
	}

TContactItemId ImportVCalL(CContactDatabase* aDb,RFs& aFs,TBool aUseNullTemplateId = EFalse)
	{
	RFileReadStream vcard;
	User::LeaveIfError(vcard.Open(aFs, KVCardFileToImport, EFileRead));
	CleanupClosePushL(vcard);

	TBool success=EFalse;
	TInt options=0;
	options |= CContactDatabase::EImportSingleContact;
	if (aUseNullTemplateId)
		{
		options |= CContactDatabase::ENullTemplateId;
		}
	CArrayPtr<CContactItem>* contactItems=aDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), vcard, success, options);
	CleanupStack::PopAndDestroy(&vcard);
	TContactItemId id=(*contactItems)[0]->Id();
	contactItems->ResetAndDestroy();
	delete contactItems;
	return id;
	}

#if defined(_DEBUG)
LOCAL_C void TestImportWithNullTemplateIdL()
	{
	CntTest->CreateDatabaseL();
//
	// Get the number of fields in the golden template
	CContactItem* item = CntTest->Db()->ReadContactLC(KGoldenTemplateId);
	TInt goldenTemplateCount = item->CardFields().Count();
	CleanupStack::PopAndDestroy(item);
//
	// Test fields imported using golden template
	createTestVCardL(CntTest->Fs());
	TContactItemId id = ImportVCalL(CntTest->Db(),CntTest->Fs());
	item = CntTest->Db()->ReadContactLC(id);
	test(item->CardFields().Count() == goldenTemplateCount);
	CleanupStack::PopAndDestroy(item);
//
	// Test fields imported using null template
	createTestVCardL(CntTest->Fs());
	id = ImportVCalL(CntTest->Db(),CntTest->Fs(),ETrue);
	item = CntTest->Db()->ReadContactLC(id);
	test(item->CardFields().Count() == 3); // number of fields in vCard
	CleanupStack::PopAndDestroy(item);
//
	CntTest->Fs().Delete(KVCardFileToImport);
	CntTest->CloseDatabase();
	}

LOCAL_C void TestsAccessCountL()
	{
	CntTest->OpenDatabaseL();
	TContactItemId addId1=AddContactL(CntTest->Db(),KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add1"));
	CContactIdArray *ids=CContactIdArray::NewLC();
	ids->AddL(addId1);
//
	CntTest->TestAccessCount(addId1,0);
	CVCardTestStore* store1=CntTest->ExportContactsLC(ids,CContactDatabase::EIncreaseAccessCount);
	CntTest->TestAccessCount(addId1,1);
	CntTest->ImportContactsLC(store1,CContactDatabase::EIncreaseAccessCount);
	CleanupStack::PopAndDestroy(2);	// items->ResetAndDestroy(),store1
	CntTest->TestAccessCount(addId1,2);
//
	CVCardTestStore* store2=CntTest->ExportContactsLC(ids,CContactDatabase::EDecreaseAccessCount);
	CntTest->TestAccessCount(addId1,1);
	CntTest->ImportContactsLC(store2,CContactDatabase::EDecreaseAccessCount);
	CleanupStack::PopAndDestroy(2);	// items->ResetAndDestroy(),store2
	CntTest->TestAccessCount(addId1,0);
	CleanupStack::PopAndDestroy();	// ids
//
	TContactItemId addId2=AddContactL(CntTest->Db(),KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("Add1"));
	ids=CContactIdArray::NewLC();
	ids->AddL(addId2);
//
	CntTest->TestAccessCount(addId2,0);
	CntTest->ExportContactsLC(ids,CContactDatabase::EIncreaseAccessCount);
	CleanupStack::PopAndDestroy(2);	// ids,store from ExportContactsLC
	CntTest->TestAccessCount(addId2,1);
//
	CVCardTest *vtest=new(ELeave) CVCardTest;
	CleanupStack::PushL(vtest);
	RReadStream stream;
	vtest->StreamLCC(stream,2,addId2);
	stream.PushL();
	TBool success;
	CntTest->Db()->OverrideMachineUniqueId(0); //testcode assumes that HAL would return 0 
	CArrayPtr<CContactItem>* items=CntTest->Db()->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),stream,success,CContactDatabase::EDecreaseAccessCount);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,items));
	CntTest->CheckContactDoesNotExist(addId2);
	CleanupStack::PopAndDestroy(4);	// stream,store,vtest,items
//
	CntTest->CloseDatabase();
	}

#endif //(_DEBUG)

/**

@SYMTestCaseID     PIM-T-VERS-0001

*/

LOCAL_C void DoTestsL()
	{
	test.Start(_L("@SYMTESTCaseID:PIM-T-VERS-0001 Preparing tests"));


	CTestRegister * TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDatabaseFileName, EFileTypeCnt);
	TempFiles->RegisterL(KVCardFileToImport);
	TempFiles->RegisterL(_L("c:\\versit2"));
	TempFiles->RegisterL(_L("c:\\versitin1"));
	TempFiles->RegisterL(_L("c:\\versitin2"));
	TempFiles->RegisterL(_L("c:\\versitin3"));
	TempFiles->RegisterL(_L("c:\\versitfile"));
	TempFiles->RegisterL(_L("c:\\vcard.vcf"));
	TempFiles->RegisterL(_L("c:\\versitoutb"));

	DoTests1L();
#if defined(_DEBUG)
	TestsAccessCountL();
	ExportImportDifferentMachineIds();
#endif
	SynchWithNonExistentRecordL();
#if defined(_DEBUG)
	BlankImportL();
#endif
	TestIncludeXL();
#if defined(_DEBUG)
	TestImportWithNullTemplateIdL();
#endif
//	TestExportImportContactL();

	CleanupStack::PopAndDestroy(TempFiles);
	}

GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	return KErrNone;
    }
