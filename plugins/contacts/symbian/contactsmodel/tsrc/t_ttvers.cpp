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
#include "t_ttvers.h"
#include "t_utils.h"
#include <coreappstest/testserver.h>

CCntTest* CntTest=NULL;
LOCAL_D RTest test(_L("T_TTVERS"));
LOCAL_D CContactIdArray* TheIds;

LOCAL_D CArrayPtr<CContactItem>* TheItems;
LOCAL_D CArrayPtr<CContactItem>* TheItems2;
LOCAL_D CArrayPtr<CContactItem>* TheItems3;
LOCAL_D RFs TheFs;
const TPtrC KDatabaseFileName=_L("C:TT_VERS");

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
//Convert the iVersit into stream format, save it as "TTVersitIn"
//and internalize it as a CVersitParser
	{
	RFs fsSession; 
	User::LeaveIfError(fsSession.Connect());
	CFileStore* store = CDirectFileStore::ReplaceLC(fsSession,_L("c:\\TTVersit2"),EFileWrite);
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
	store = CDirectFileStore::OpenLC(fsSession,_L("c:\\TTVersit2"),EFileRead); //retrieve stream
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	iParser->InternalizeL(instream);
	CleanupStack::PopAndDestroy(2); //store + stream
	}

void CVersitTest::StreamLC(RReadStream& aStream)
	{
	GetSampleVersitL();
	RFs fsSession; 
	User::LeaveIfError(fsSession.Connect());
	CFileStore* store = CDirectFileStore::ReplaceLC(fsSession,_L("c:\\TTVersitIn"),EFileWrite);
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
	store = CDirectFileStore::OpenLC(fsSession,_L("c:\\TTVersitIn"),EFileRead); //retrieve stream
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	CleanupStack::Pop();	// instream
	aStream=RReadStream(instream);
	}

void CVersitTest::Stream2LC(RReadStream& aStream)
	{
	GetSampleVersit2L();
	RFs fsSession; 
	User::LeaveIfError(fsSession.Connect());
	CFileStore* store = CDirectFileStore::ReplaceLC(fsSession,_L("c:\\TTVersitIn2"),EFileWrite);
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
	store = CDirectFileStore::OpenLC(fsSession,_L("c:\\TTVersitIn2"),EFileRead); //retrieve stream
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	CleanupStack::Pop();	// instream
	aStream=RReadStream(instream);
	}

void CVersitTest::Stream3LC(RReadStream& aStream)
	{
	GetSampleVersit3L();
	RFs fsSession; 
	User::LeaveIfError(fsSession.Connect());
	CFileStore* store = CDirectFileStore::ReplaceLC(fsSession,_L("c:\\TTVersitIn3"),EFileWrite);
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
	store = CDirectFileStore::OpenLC(fsSession,_L("c:\\TTVersitIn3"),EFileRead); //retrieve stream
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
		RFs fsSession;
		User::LeaveIfError(fsSession.Connect());
		RFile file;
		if (file.Replace(fsSession,_L("C:\\VersitFile"),EFileWrite)==KErrNone)
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
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	RFile file;
	if (file.Open(fsSession,_L("C:\\VCard.vcf"),EFileRead)==KErrNone)
		{		
		TInt start=0;
		iParser->InternalizeL(file,start);
		}
	}
/*
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
	test.Getch();
	}

LOCAL_C void DisplayDatabase(CContactDatabase *aDb)
	{
	TContactItemId theid;
	CContactItem* contactItem=NULL;
	TContactIter iter(*aDb);
	theid=iter.FirstL();
	while(theid!=KNullContactId )
		{
		test.Printf(_L("\n========================ROW ID %d"),theid);
		contactItem=aDb->ReadContactLC(theid);
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
				test.Printf(fieldText);
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
*/	

LOCAL_C void ExportImportTestL()
	{
	CDirectFileStore *store=CDirectFileStore::ReplaceLC(CntTest->Fs(),_L("c:\\ttVersitout"),EFileWrite);
  	store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outStream;
	TStreamId streamId=outStream.CreateLC(*store);
//
	CContactDatabase* db=CntTest->Db();
	CContactIdArray *idList=CContactIdArray::NewLC();
	CContactCard *card=CContactCard::NewLC();
	SetNameL(*card,KUidContactFieldNote,KUidContactFieldVCardMapNOTE,_L("ABCDE"),EFalse);
	SetNameL(*card,KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET,_L("One"),EFalse);
	idList->AddL(db->AddNewContactL(*card));
//zzz hits a Versit bug
//	SetNameL(*card,KUidContactFieldFax,KUidContactFieldVCardMapFAX,_L("ZYX"),EFalse);
	SetNameL(*card,KUidContactFieldFax,KUidContactFieldVCardMapTEL,_L("ZYX"),EFalse);
	SetNameL(*card,KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET,_L("Two"),EFalse);
	idList->AddL(db->AddNewContactL(*card));
	SetNameL(*card,KUidContactFieldSms,KUidContactFieldVCardMapTEL,_L("QQQQ"),EFalse);
	SetNameL(*card,KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET,_L("Three"),EFalse);
	idList->AddL(db->AddNewContactL(*card));
	CleanupStack::PopAndDestroy();	// card
//
   	db->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl),*idList,outStream,CContactDatabase::ETTFormat);
	TInt targetCount=db->CountL();
	for(TInt loop3=0;loop3<idList->Count();loop3++)
		{
		CContactItem *delFieldContact=db->OpenContactLX((*idList)[loop3]);
		CleanupStack::PushL(delFieldContact);
		delFieldContact->CardFields().Remove(0);
		db->CommitContactL(*delFieldContact);
		CleanupStack::PopAndDestroy(2);	// OpenContact, delFieldContact
		}
	CleanupStack::PopAndDestroy();	// idList
	outStream.CommitL();
	store->SetRootL(streamId);
	store->CommitL();  	
	CleanupStack::PopAndDestroy();	// outStream
//
	RStoreReadStream inStream;
	inStream.OpenLC(*store,streamId);
	TBool success=EFalse;
	CArrayPtr<CContactItem>* importedContacts=db->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),inStream,success,CContactDatabase::ETTFormat);
	CleanupStack::PushL(importedContacts);
	test(success);
	test(importedContacts->Count()==3);
	test(targetCount==db->CountL());
//
	const CContactIdArray *sortList=db->SortedItemsL();
	for(TInt loop2=0;loop2<sortList->Count();loop2++)
		{
		CContactItem *contact=db->ReadContactLC((*sortList)[loop2]);
		TPtrC fieldTxt=FieldText(*contact,KUidContactFieldGivenName);
		if (fieldTxt==_L("One"))
			test(FieldText(*contact,KUidContactFieldNote)==_L("ABCDE"));
		else if (fieldTxt==_L("Two"))
			test(FieldText(*contact,KUidContactFieldFax)==_L("ZYX"));
		else if (fieldTxt==_L("Three"))
			test(FieldText(*contact,KUidContactFieldPhoneNumber)==_L("QQQQ"));
		CleanupStack::PopAndDestroy();	// contact
		}
//
	for(TInt loop=0;loop<importedContacts->Count();loop++)
		db->DeleteContactL((*importedContacts)[loop]->Id());
	importedContacts->ResetAndDestroy();
	CleanupStack::PopAndDestroy(2);	// importedContacts, inStream
//
	CleanupStack::PopAndDestroy();	// store
	}

/**

@SYMTestCaseID     PIM-T-TTVERS-0001

*/

LOCAL_C void DoTestsL()
	{
	// create test database
	test.Start(_L("@SYMTESTCaseID:PIM-T-TTVERS-0001 Preparing tests"));

	User::LeaveIfError(TheFs.Connect());
	CleanupClosePushL(TheFs);
	CTestRegister * TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KDatabaseFileName, EFileTypeCnt);
	TempFiles->RegisterL(_L("C:\\ttversitin"));
	TempFiles->RegisterL(_L("C:\\ttversitin2"));
	TempFiles->RegisterL(_L("C:\\ttversitin3"));
	TempFiles->RegisterL(_L("C:\\ttversitout"));
	TempFiles->RegisterL(_L("C:\\ttversitout.vcf"));
	TempFiles->RegisterL(_L("C:\\ttversitoutb.vcf"));

	TheIds=CContactIdArray::NewLC();
	TRAPD(err, CContactDatabase::DeleteDatabaseL(KDatabaseFileName));
	if ((err != KErrNone) && (err != KErrNotFound))
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
	CVersitTest* vtest3=NULL;
	vtest3=new(ELeave)CVCardTest;
	CleanupStack::PushL(vtest3);
	//Import
	test.Next(_L("Importing vcard"));

	CContactDatabase* db=CntTest->CreateDatabaseL();
	CntTest->Db()->OverrideMachineUniqueId(0); //testcode assumes machineUID is 0
	TBool success;
	TUid vcardmode;
	vcardmode.iUid=KUidVCardConvDefaultImpl;
	RReadStream stream;
	vtest->StreamLC(stream);
	stream.PushL();
	TheItems=db->ImportContactsL(vcardmode,stream,success,CContactDatabase::EIncludeX);
	test(success);
	CleanupStack::PopAndDestroy(2);	// stream,store
	test(db->CountL()==1);
	test(TheItems->Count()==1);
	test((*TheItems)[0]->UidStringL(0x0)==_L("AAA"));
	//Export
	test.Next(_L("Exporting vcard"));

	CFileStore* store = CDirectFileStore::ReplaceLC(TheFs,_L("c:\\ttVersitout.vcf"),EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	TUid uid;
	uid.iUid=KUidVCardConvDefaultImpl;
	TInt itemCount=TheItems->Count();
	for (TInt ii=0;ii<itemCount;ii++)
		TheIds->AddL((*TheItems)[ii]->Id());
	db->ExportSelectedContactsL(uid,*TheIds,outstream,CContactDatabase::ETTFormat);
	outstream.CommitL();
	store->SetRootL(id);
	store->CommitL();  	
	CleanupStack::PopAndDestroy(2); // store+ oustream
	// check there are no X-EPOC
	CFileStore* store2 = CDirectFileStore::OpenLC(TheFs,_L("c:\\ttVersitout.vcf"),EFileRead); //retrieve stream
	RStoreReadStream tinstream;
	tinstream.OpenLC(*store2,id);
	RReadStream xstream=RReadStream(tinstream);
	HBufC* xdes=HBufC::NewLC(99);
	TPtr ptr(xdes->Des());
	xstream.ReadL(ptr);
    test(xdes->Des().Match(_L("*X-*"))==KErrNotFound);
	CleanupStack::PopAndDestroy(3); // store2+ instream
	//DisplayDatabase(db);
	test.Next(_L("Updating vcard 2"));

	RReadStream stream2;
	vtest2->Stream2LC(stream2);
	stream2.PushL();
	TheItems2=db->ImportContactsL(vcardmode,stream2,success,CContactDatabase::ETTFormat);
	test(success);
	CleanupStack::PopAndDestroy(2);	// stream,store
	test(db->CountL()==1);
	//Export again
	//DisplayDatabase(db);
	test.Next(_L("Exporting vcard"));

	TheIds->Reset();
	TheIds->AddL(1);
	store = CDirectFileStore::ReplaceLC(TheFs,_L("c:\\ttVersitoutb.vcf"),EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);
	id = outstream.CreateLC(*store);
	uid.iUid=KUidVCardConvDefaultImpl;
	itemCount=TheItems->Count();
	db->ExportSelectedContactsL(uid,*TheIds,outstream,CContactDatabase::ETTFormat);
	outstream.CommitL();
	store->SetRootL(id);
	store->CommitL();  	
	CleanupStack::PopAndDestroy(2); // store+ oustream
/**
@SYMTestCaseID     PIM-T-TTVERS-0002
@SYMTestType UT
@SYMTestPriority High
@SYMDEF INC056117
@SYMTestCaseDesc To verify that the incorrect behaviour described in INC056117
no longer exists.
@SYMTestActions A vCard is imported where the N, ADR;WORK, EMAIL;INTERNET,
TITLE, ORG, NOTE, URL, ROLE and FN property values all have > 255 characters.
@SYMTestExpectedResults The vCard is correctly imported i.e. properties which
map to text fields with maximum of 255 characters are truncated and no overflow
occurs in the DBMS component (leading to a panic in the test case).  Further
sanity checks are made to ensure that the contacts database contains the
expected number of contacts (2), that the number of CContactItem objects in
TheItems3 is as expected (1) and that the UID string for the CContactItem
object in TheItems3 is as expected ("BBB").
*/
	test.Next(_L("@SYMTESTCaseID:PIM-T-TTVERS-0002 Import vCard with property values > 255 characters"));

	RReadStream stream3;
	vtest3->Stream3LC(stream3);
	stream3.PushL();
	TheItems3=db->ImportContactsL(vcardmode,stream3,success,CContactDatabase::EIncludeX);
	test(success);
	CleanupStack::PopAndDestroy(2);	// stream3,store
	test(db->CountL()==2);
	test(TheItems3->Count()==1);
	test((*TheItems3)[0]->UidStringL(0x0)==_L("BBB"));

	if (TheItems)
		TheItems->ResetAndDestroy();
	delete TheItems;
	if (TheItems2)
		TheItems2->ResetAndDestroy();
	delete TheItems2;
	if (TheItems3)
		TheItems3->ResetAndDestroy();
	delete TheItems3;
	
	CleanupStack::PopAndDestroy(4); // TheIds, vtest, vtest2, vtest3
//
	ExportImportTestL();
//
	CntTest->CloseDatabase();
	User::After(1000000);
	CntTest->DeleteDatabaseL();
	CleanupStack::PopAndDestroy(2); // FS, TempFiles
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
