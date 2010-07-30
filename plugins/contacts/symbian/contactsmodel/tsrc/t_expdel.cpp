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
#include "t_expdel.h"
#include "t_utils.h"
#include <coreappstest/testserver.h>

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_EXPDEL"));
LOCAL_D CContactIdArray* TheIds;

LOCAL_D CArrayPtr<CContactItem>* TheItems;
LOCAL_D CArrayPtr<CContactItem>* TheItems2;
LOCAL_D CTestRegister* FileRegister;

const TPtrC KDatabaseFileName=_L("C:T_EXPDEL");
//
// CVersitTest
//

void CVCardTest::GetSampleVersitL()
	{
	iParser=CParserVCard::NewL();
	iVersit.Zero();
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("BEGIN:VCARD"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("VERSION:2.1"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("N:FName1;GName1"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TEL;HOME:111"));
	iVersit.Append(KVersitTokenCRLF);
	
	iVersit.Append(_L8("AGENT:"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("BEGIN:VCARD"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("VERSION:2.1"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("N:FName2;GName2"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TEL;HOME:222"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("END:VCARD"));
	iVersit.Append(KVersitTokenCRLF);
	
	
	iVersit.Append(_L8("END:VCARD"));
	iVersit.Append(KVersitTokenCRLF);
	}

void CVCardTest::GetSampleVersit2L()
	{
	}


CVersitTest::CVersitTest()
	{}

CVersitTest::~CVersitTest()
	{
	delete iParser;
	delete iStore;
	}

void CVersitTest::StreamInL()
//Convert the iVersit into stream format, save it as "ExpDelVersitIn"
//and internalize it as a CVersitParser
	{
	RFile file;
	FileRegister->CreateTempFileLC(file, _L("ExpDelVersit2"));
	CDirectFileStore* store	= CDirectFileStore::NewLC(file);
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
	CleanupStack::PopAndDestroy(2);	// store + file
	FileRegister->OpenTempFileLC(file, _L("ExpDelVersit2"));
	store = CDirectFileStore::NewLC(file);
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	iParser->InternalizeL(instream);
	CleanupStack::PopAndDestroy(3); //store + stream + file
	}

void CVersitTest::StreamLC(RReadStream& aStream)
	{
	GetSampleVersitL();
	RFile file;
	FileRegister->CreateTempFileLC(file, _L("ExpDelVersitIn"));
	CDirectFileStore* store	= CDirectFileStore::NewLC(file);
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
 	CleanupStack::PopAndDestroy(2);	// store + file
	FileRegister->OpenTempFileLC(file, _L("ExpDelVersitIn"));
	store = CDirectFileStore::NewLC(file);
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	CleanupStack::Pop();	// instream
 	aStream=RReadStream(instream);
	}

void CVersitTest::Stream2LC(RReadStream& aStream)
	{
	GetSampleVersit2L();
	RFile file;
	FileRegister->CreateTempFileLC(file, _L("ExpDelVersitIn2"));
	CDirectFileStore* store	= CDirectFileStore::NewLC(file);
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
	FileRegister->OpenTempFileLC(file, _L("ExpDelVersitIn2"));
	store = CDirectFileStore::NewLC(file);
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
		RFile file;
		FileRegister->CreateTempFileLC(file, _L("VersitFile"));
		iParser->ExternalizeL(file);
		CleanupStack::PopAndDestroy(&file);
		}
	}

//
// CVCardTest
//

void CVCardTest::LoadFileVersitL()
// Read VCard.vcf into iVersit	
	{
	iParser=CParserVCard::NewL();
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	RFile file;
	FileRegister->OpenTempFileLC(file, _L("VCard.vcf"));
	TInt start=0;
	iParser->InternalizeL(file,start);
	CleanupStack::PopAndDestroy(&file);
	}

LOCAL_C void SetNameL(CContactItem& aItem,TUid aVcardType,const TDesC& aName)
//
// Set the contents of a text field, creating the field if required
//
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(KUidContactFieldFamilyName);
	if (pos!=KErrNotFound)
		fieldSet[pos].TextStorage()->SetTextL(aName);
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldFamilyName);
		field->SetMapping(aVcardType);
		field->TextStorage()->SetTextL(aName);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(); // item
		}
	}


LOCAL_C void AddNewContactsL()
//
// Add 20 new contacts 
//
	{

	for (TInt ii=0;ii<20;ii++)
		{
		CContactCard* card=CContactCard::NewL();
		CleanupStack::PushL(card);
		TBuf<16> name;
		name.Format(_L("NAME #%d"),ii);
		SetNameL(*card,KUidContactFieldVCardMapUnusedN,name);
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
		field->SetMapping(KUidContactFieldVCardMapTEL);
		field->TextStorage()->SetTextL(_L("123"));
		card->AddFieldL(*field);
		CleanupStack::Pop(); // field
		CntTest->Db()->AddNewContactL(*card);
		CleanupStack::PopAndDestroy(); // card
		}
	
	}	

LOCAL_C void EmptyDatabase()
	{
	TContactItemId theid;
	TContactIter iter(*CntTest->Db());
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		CntTest->Db()->DeleteContactL(theid);
		theid=iter.NextL();
		}
	}
	

LOCAL_C void IncAccessCount()
	{
	TContactItemId theid;
	TContactIter iter(*CntTest->Db());
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		CContactItem* contactItem=CntTest->Db()->OpenContactL(theid);
		CleanupStack::PushL(contactItem);
		contactItem->IncAccessCount();
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
		if (contactItem->IsDeleted())
			test.Next(_L("Synchronizer knows record 2 is deleted"));

		contactItem->DecAccessCount();
		CntTest->Db()->CommitContactL(*contactItem);
		theid=iter.NextL();
		CleanupStack::PopAndDestroy();	// contactitem
		}				
	}

LOCAL_C void Synchronize()
	{
	DecAccessCount();
	}

/**

@SYMTestCaseID     PIM-T-EXPDEL-0001

*/

LOCAL_C void DoTestsL()
	{
	// create test database
	test.Start(_L("@SYMTESTCaseID:PIM-T-EXPDEL-0001 Preparing tests"));

	
	FileRegister = CTestRegister::NewLC();

	TheIds=CContactIdArray::NewLC();
	
	TRAPD(err,CContactDatabase::DeleteDatabaseL(KDatabaseFileName));
	if (err < KErrNotFound)
		{
		User::Leave(err);
		}
	
	CVersitTest* vtest=NULL;
	if (vtest)
		CleanupStack::PopAndDestroy(); //vtest;
	vtest=new(ELeave)CVCardTest;
	CleanupStack::PushL(vtest);
	CVersitTest* vtest2=NULL;
	vtest2=new(ELeave)CVCardTest;
	CleanupStack::PushL(vtest2);
	
	//Populating
	test.Next(_L("creating 20 contacts"));

	CContactDatabase *db=CntTest->CreateDatabaseL();
	AddNewContactsL();
	test(db->CountL()==20);
	test.Next(_L("Increase Access Count"));

	IncAccessCount();
	test.Next(_L("Removing second contact"));

	TContactIter iter(*db);
	TContactItemId theid=iter.FirstL();
	theid=iter.NextL();
	db->DeleteContactL(theid);
	test(db->CountL()==19);
	test.Next(_L("Synchronizing"));

	Synchronize();
	test(db->CountL()==19);
	test.Next(_L("Exporting"));

	TheIds=CContactIdArray::NewLC();
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		TheIds->AddL(theid);
		theid=iter.NextL();
		}
	RFile file;
	FileRegister->CreateTempFileLC(file, _L("expdela"));			
	CDirectFileStore* store = CDirectFileStore::NewLC(file);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	TUid uid;
	uid.iUid=KVersitEntityUidVCard;
	db->ExportSelectedContactsL(uid,*TheIds,outstream,CContactDatabase::EIncludeX | CContactDatabase::EIncreaseAccessCount);

	outstream.CommitL();
	store->SetRootL(id);
	store->CommitL();  	
	CleanupStack::PopAndDestroy(3); // store, oustream, file

	CleanupStack::PopAndDestroy(); //TheIds
	test.Next(_L("Emptying database"));

	EmptyDatabase();

	if (TheItems)
		TheItems->ResetAndDestroy();
	delete TheItems;
	if (TheItems2)
		TheItems2->ResetAndDestroy();
	delete TheItems2;
	
	CleanupStack::PopAndDestroy(2); // vtest, vtest2
	CntTest->CloseDatabase();
	CleanupStack::PopAndDestroy(); //TheIds
	CleanupStack::PopAndDestroy(FileRegister);
	}

GLDEF_C TInt E32Main()
	{
    CntTest=new(ELeave) CCntTest;
	CntTest->ConstructL(test,KDatabaseFileName);
    TRAPD(err,DoTestsL());
	CntTest->EndTestLib(err);
	return KErrNone;
    }
