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
#include "t_utils.h"
#include "t_dbase2.h"
#include "ccontactdbeventqueue.h"

GLDEF_D CCntTest* CntTest=NULL;
GLDEF_D RTest test(_L("T_DBASE2"));

_LIT(KDatabaseFileName,"C:T_DBASE2");

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
_LIT(KPhysicalPathToDatabase, "c:\\private\\10003A73\\T_DBASE2");
#else
_LIT(KPhysicalPathToDatabase, "c:\\private\\10003A73\\SQLite__T_DBASE2");
#endif

const TPtrC KTestEmailAddress=_L("testEmail@symbian.com");

class CAsyncFinder : public CBase, public MIdleFindObserver
	{
public:
	~CAsyncFinder();
	void ConstructL(CContactDatabase *aContactDatabase, const TDesC& aText,const CContactItemFieldDef *aFieldDef);
	void ConstructL(CContactDatabase *aContactDatabase, const CDesCArray& aFindWords,const CContactTextDef *aTextDef,const TCallBack &aCallBack);
	inline CContactIdArray *TakeContactIds() {return(iFinder->TakeContactIds());};
	inline TInt Error() const {return(iFinder->Error());};
protected:
// from MIdleFindObserver
	void IdleFindCallback();
private:
	CIdleFinder *iFinder;
	};

GLDEF_C void Panic(TInt aPanic)
//
// Panic the thread with CNTMODEL as the category
//
	{
	User::Panic(_L("T_DBASE2"),aPanic);
	}

LOCAL_C void CreateExistingDatabaseTestL()
	{
	TRAPD(err,CContactDatabase::CreateL(KDatabaseFileName));
	test(err==KErrAlreadyExists);
	CContactDatabase *db=NULL;
	TRAP(err,db=CContactDatabase::OpenL(KDatabaseFileName));
	test(err==KErrNone);
	delete db;
	}

	
LOCAL_C void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName, TBool aAddField = EFalse)
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
		CleanupStack::Pop(); // field
		}
	}

LOCAL_C TContactItemId AddPhoneContact(const TDesC& aName, const TDesC& aNumber)
	{
	CContactItem* item = CContactCard::NewLC();
	SetNameL(*item, KUidContactFieldGivenName, aName, ETrue);
	SetNameL(*item, KUidContactFieldPhoneNumber, aNumber, ETrue);
	TContactItemId newId = CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy();	// item
	return(newId);
	}

LOCAL_C TInt PopulateDatabaseForAsyncFindByEmailAddrTest(const TDesC& aEmailAddress)
	{
	//This function generates a database upon which CContactDatabase::AsyncFindL(...) will fail
	//as described in DEF036083, thereby validating the provided fix.
	//The database comprises 23 entries distributed across two groups.
	//Only one entry contains the email address that will match the search criteria used by
	//the AsyncFundByEmailTest(...) function.
	//It returns the number of items that have aEmailAddress (which is 1).
	CContactIdArray* array = NULL;

	for(TInt i = 0; i < 2; i++) 
	{
		TBuf<16> groupName(_L("group"));
		groupName.AppendNum(i+1);

		CContactItem* group = CntTest->Db()->CreateContactGroupLC(groupName);
		CleanupStack::PopAndDestroy(group);

	}
	 
	array = CntTest->Db()->GetGroupIdListL();
	CleanupStack::PushL(array);
	
	// Get two groups
	CContactGroup* group = STATIC_CAST(CContactGroup*, CntTest->Db()->ReadContactLC((*array)[0]));
	const TContactItemId groupId1 = group->Id();
	CleanupStack::PopAndDestroy(group);
	group = NULL;

	group = STATIC_CAST(CContactGroup*, CntTest->Db()->ReadContactLC((*array)[1]));
	const TContactItemId groupId2 = group->Id();
	CleanupStack::PopAndDestroy(group);
	CleanupStack::PopAndDestroy(array);

	TBuf<128> buffer;
	TTime now;
	now.UniversalTime();
	TInt64 seed=now.Int64();
	
	for(TInt ii = 1; ii <= 23; ii++) 
		{

		CContactCard* newContact = CContactCard::NewLC();

		// Create random first and last names
		TBuf<16> firstName;
		TBuf<16> lastName;
		
		TInt c = (Math::Rand(seed)%26)+'A';
		firstName.Append(c);
		c = (Math::Rand(seed)%26)+'A';
		lastName.Append(c);
		TInt jj;
		for(jj=1;jj<16;jj++)
			{
			c=(Math::Rand(seed)%26)+'a';
			firstName.Append(c);
			if(jj>4 && (Math::Rand(seed)&4)==0)
				break;
			}
		for(jj=1;jj<16;jj++)
			{
			c=(Math::Rand(seed)%26)+'a';
			lastName.Append(c);
			if(jj>4 && (Math::Rand(seed)&4)==0)
				break;
			}
		
		SetNameL(*newContact,KUidContactFieldGivenName,firstName);
		SetNameL(*newContact,KUidContactFieldFamilyName,lastName);

		if(Math::Rand(seed)%4)
			{
			buffer=firstName;
			buffer.Append('.');
			buffer.Append(lastName);
			buffer.Append(_L("@email-address.com"));
			SetNameL(*newContact,KUidContactFieldEMail,buffer);
			}

		if(Math::Rand(seed)%4)
			SetNameL(*newContact,KUidContactFieldUrl,_L("http://www.some-web-address.co.uk"));

		if(Math::Rand(seed)%4)
			SetNameL(*newContact,KUidContactFieldAddress,_L("167a Hanover Road"));

		if(Math::Rand(seed)%4)
			SetNameL(*newContact,KUidContactFieldPostcode,_L("06NW10 3DN"));

		if(Math::Rand(seed)%4)
			SetNameL(*newContact,KUidContactFieldRegion,_L("LONDON"));

		if(Math::Rand(seed)%4)
			SetNameL(*newContact,KUidContactFieldLocality,_L("Willesden Green"));

		SetNameL(*newContact,KUidContactFieldPhoneNumber,_L("+46 457 11111"));

		if(Math::Rand(seed)%4)
			SetNameL(*newContact,KUidContactFieldNote,_L("The quick brown fox jumps over the smelly dog The quick brown dog jumps over the smelly cat"));

		if(Math::Rand(seed)%4)
			SetNameL(*newContact,KUidContactFieldCompanyName,_L("S.G.Software"));

		const TContactItemId id = CntTest->Db()->AddNewContactL(*newContact);

		CleanupStack::PopAndDestroy(newContact);


		if(Math::Rand(seed)%2)
			CntTest->Db()->AddContactToGroupL(id, groupId1);
		else
			CntTest->Db()->AddContactToGroupL(id, groupId2);
		}

		//Modify the 23rd entry so that it has a specific email address that will
		//be the search criteria used in AsyncFindByEmailTest(...).
		CContactItem* itemToModify = NULL;
		TContactIter iter(*CntTest->Db());
		TInt theid;
		theid=iter.FirstL();
		while(theid!=KNullContactId )
		{
			if(theid == 23)
			{
				itemToModify=CntTest->Db()->OpenContactL(theid);
				CleanupStack::PushL(itemToModify);
				buffer.Zero();
				buffer.Append(aEmailAddress);
				SetNameL(*itemToModify,KUidContactFieldEMail,buffer);
				CntTest->Db()->CommitContactL(*itemToModify);
				CleanupStack::PopAndDestroy(itemToModify);

			}
			theid=iter.NextL();
		}
		
		

		return 1;

	}

LOCAL_C void PopulateDatabaseL(TBool aPhoneNumbers)
//
// Create and populate the database
//
	{
	for (TInt ii=0;ii<KTotalNumRecords;ii++)
		{
		CContactItem* item=CContactCard::NewLC();
		TBuf<16> name;
		name.Format(KTestName,ii);
   		SetNameL(*item,KUidContactFieldFamilyName,name,ETrue);
		if (aPhoneNumbers)
			{
			TBuf<20> number;
			switch(ii%3)
				{
				case 0:
					number.Format(_L("0171-%03d %04d"),(ii*9)%1000,((ii+11)*23)%10000);
					break;
				case 1:
					number.Format(_L("%04d:%04d:%04d:%04d"),(ii*123)%10000,(ii*666)%10000,(ii*234)%10000);
					break;
				case 2:
					number.Format(_L("+00%d-%03d %04d"),(ii*123)%100,(ii*13)%1000,((ii+13)*17)%10000);
					break;
				}
   			SetNameL(*item,KUidContactFieldPhoneNumber,number,ETrue);
			if (!(ii%2))
				{
				number.Format(_L("0181-%03d %04d"),(ii*8)%1000,((ii+11)*22)%10000);
	   			SetNameL(*item,KUidContactFieldPhoneNumber,number,ETrue);
				number.Format(_L("01734-%06d"),(ii*123456)%1000000);
	   			SetNameL(*item,KUidContactFieldPhoneNumber,number,ETrue);
				}
			}
		if (ii%2)
			{
			TBuf<16> address;
			address.Format(KTestAddress,ii);
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldAddress);
			field->SetMapping(KUidContactFieldVCardMapADR);
			field->TextStorage()->SetText(address.AllocL());
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		if (ii%3)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeDateTime,KUidContactFieldBirthday);
			field->DateTimeStorage()->SetTime(TDateTime(1990+ii,(TMonth)(ii%12),(ii*3)%28,1,2,3,4));
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		if (ii%4)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeStore);
    		field->SetMapping(KUidContactFieldVCardMapAGENT);
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		if (ii%6)
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeContactItemId);
			field->SetMapping(KUidContactFieldVCardMapAGENT);
			item->AddFieldL(*field);
			CleanupStack::Pop(); // field
			}
		CntTest->Db()->AddNewContactL(*item);
		CleanupStack::PopAndDestroy(); // item
		}
	CntTest->Db()->SetDateFormatTextL(_L("%E%D%X%N%Y %1 %2 %3"));
	}

LOCAL_C void CreateBasicContactGroupsL()
	{
	test.Next(_L("Create Basic Groups"));
	__ASSERT_ALWAYS(CntTest->Db()->GroupCount()==0,Panic(KErrGeneral));
	CntTest->Db()->CreateContactGroupLC(_L("New Group 1"));
	CntTest->Db()->CreateContactGroupLC(_L("New Group 2"));
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	test(CntTest->Db()->GroupCount()==2);
	CleanupStack::PopAndDestroy(2); // newGroup newGroup2
	}

LOCAL_C void CreateBasicOwnCardL()
	{
	test.Next(_L("Create Basic Own Card"));
	CContactItem* newOwn = CntTest->Db()->CreateOwnCardL();
	delete newOwn;
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	CContactItem* newOwnCard = CntTest->Db()->OpenContactLX(CntTest->Db()->OwnCardId());
	CleanupStack::PushL(newOwnCard);
	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldFamilyName);
	field->SetMapping(KUidContactFieldVCardMapUnusedN);
	field->TextStorage()->SetTextL(_L("Dale"));
	newOwnCard->AddFieldL(*field);
	CntTest->Db()->CommitContactL(*newOwnCard);
	CleanupStack::Pop(); // field
	CleanupStack::PopAndDestroy(2); // newOwnCard close
	}

LOCAL_C void NavigateL()
	{
	test.Next(_L("Navigate from start to end"));
	TContactIter iter(*CntTest->Db());
	TContactItemId id=iter.FirstL();
	TInt count=1;
	FOREVER
		{
		TContactItemId newId=iter.NextL();
		if (newId==id || newId==KNullContactId)
			break;
		++count;
		}
	test(count==KTotalNumRecords); // +1 for own card

	test.Next(_L("Navigate from end to start"));
	id=iter.LastL();
	count=1;
	FOREVER
		{
		TContactItemId newId=iter.PreviousL();
		if (newId==id || newId==KNullContactId)
			break;
		++count;
		}
	test(count==KTotalNumRecords); // +1 for own card
	}


LOCAL_C void CrystalSortRegressionTestL()
	{
	test.Next(_L("Sort regression test"));
	CContactDatabase& db = *CntTest->CreateDatabaseL();
	_LIT(KFirstNameCarl,"Carl");
	_LIT(KFirstNameChris,"Chris");
	TContactItemId one=AddContactL(&db,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,KFirstNameCarl);
	TContactItemId two=AddContactL(&db,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,KFirstNameChris);
	
	CContactIdArray* unsortedArray = CContactIdArray::NewLC();
	unsortedArray->AddL(one);
	unsortedArray->AddL(two);
	CArrayFix<CContactDatabase::TSortPref>* sortOrder = new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(1);
	CleanupStack::PushL(sortOrder);
	CContactDatabase::TSortPref sortPref(KUidContactFieldDefinedText,CContactDatabase::TSortPref::EAsc);
	sortOrder->AppendL(sortPref);

	CContactIdArray* sorted = db.SortArrayLC(unsortedArray, sortOrder);
	test((*sorted)[0]==one);
	test((*sorted)[1]==two);
	CleanupStack::PopAndDestroy(3, unsortedArray);
	CntTest->CloseDatabase();
	}

LOCAL_C void CheckTemplateRefId()
	{
	test.Next(_L("Checking TemplateRefId after Commit"));
	CContactDatabase *db=CntTest->CreateDatabaseL();
	TContactItemId id=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("5432"));;
	__ASSERT_ALWAYS(id!=KNullContactId,Panic(KErrGeneral));
	CContactItem* item = db->OpenContactL(id);
	CleanupStack::PushL(item);	
	item->SetTemplateRefId(KNullContactId);
	db->CommitContactL(*item);
	CContactItem* retItem = db->ReadContactL(id);
	CleanupStack::PushL(retItem);	
	test(retItem->TemplateRefId()==KNullContactId);
	db->CloseContactL(id);
	CleanupStack::PopAndDestroy(2);
	CntTest->CloseDatabase();
	}

LOCAL_C void TestDeletingRecords()
	{
	test.Next(_L("Deleting records"));
//
	CContactDatabase *db=CntTest->CreateDatabaseL();
//	CntTest->DeleteAllTemplateFieldsL();
//
	TContactItemId id1=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("3333"));
	TContactItemId id2=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("2222"));
	TContactItemId id3=AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,_L("1111"));
	test(db->CountL()==3);  // +1 own card
	CntTest->AdjustContactAccessCountL(id2,1);
	db->DeleteContactL(id2);
	test(db->CountL()==2); // +1 own card
	CntTest->CheckDeletedContact(id2);
	CntTest->AdjustContactAccessCountL(id2,-1);
	CntTest->CheckContactDoesNotExist(id2);
	test(db->CountL()==2); // +1 own card
	db->DeleteContactL(id3);
	test(db->CountL()==1); // +1 own card
	CntTest->AdjustContactAccessCountL(id1,5);
	db->DeleteContactL(id1);
	test(db->CountL()==0); // +1 own card 
	CntTest->CheckDeletedContact(id1);
	CntTest->AdjustContactAccessCountL(id1,-4);
	CntTest->CheckDeletedContact(id1);
	CntTest->AdjustContactAccessCountL(id1,-1);
	CntTest->CheckContactDoesNotExist(id1);
//
	CntTest->CloseDatabase();
//
	db=CntTest->CreateDatabaseL();
	for(TInt loop=0;loop<200;loop++)
		{
		TBuf<128> text;
		text.Format(_L("Delete me %d"),loop);
		AddContactL(db,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,text);
		}
	db->DeleteContactsL(*CContactIdArray::NewLC(db->SortedItemsL()));
	CleanupStack::PopAndDestroy();	// Id Array
	test(db->SortedItemsL()->Count()==0);
	
	// Test full deletion of Phone Fields after Contact deletion INC047110
	_LIT(KShortNumber, "555");
	_LIT(KNameOne, "First");
	_LIT(KNameTwo, "Second");
	const TInt KNumMatchDigit = 3;
	
	// Add two Contacts with the same short (<7 digit) number
	TContactItemId ident1 = AddPhoneContact(KNameOne, KShortNumber);
	TContactItemId ident2 = AddPhoneContact(KNameTwo, KShortNumber);
	
	// delete one of them
	CntTest->Db()->DeleteContactL(ident1);
	
	CContactIdArray* array = CntTest->Db()->MatchPhoneNumberL(KShortNumber, KNumMatchDigit);
	CleanupStack::PushL(array);
	
	// the phone number related to ident1 should also be deleted
	test(array->Count() == 1);
	
	// tidy up
	CleanupStack::PopAndDestroy(array);
	CntTest->Db()->DeleteContactL(ident2);
	
	CntTest->CloseDatabase();
	}

LOCAL_C void TestLabelValidate(const TDesC &aLabel, TBool aValid, TInt aBadPos)
	{
	TInt badPos;
	TBool isValid=CContactItemField::IsValidLabel(aLabel,badPos);
	if (aValid)
		test(isValid);
	else
		{
		test(!isValid);
		test(aBadPos==badPos);
		}
	}

LOCAL_C void LabelValidating()
	{
	test.Next(_L("Label validating"));
	TestLabelValidate(_L("Good"),ETrue,0);
	TestLabelValidate(_L(""),ETrue,0);
	TestLabelValidate(_L("Bad.Label"),EFalse,3);
	TestLabelValidate(_L("."),EFalse,0);
	TestLabelValidate(_L("Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad Bad #Bad Bad Bad Bad Bad Bad Bad Bad "),ETrue,0);
	}

class CLetChangeNotiferRun : public CTimer
	{
public:
	inline CLetChangeNotiferRun() : CTimer(EPriorityStandard) {};
	void ConstructL();
	void RunL();
	};

void CLetChangeNotiferRun::RunL()
	{
	CActiveScheduler::Stop();
	}

void CLetChangeNotiferRun::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

LOCAL_C void CountL(TBool aCheckAll)
	{
	CContactDatabase *db=CntTest->CreateDatabaseL();
	test.Next(_L("Count()"));
	TContactItemId deleteItem=KNullContactId;
	TInt loop=0;
	for (;loop<10;loop++)
		{
		if (aCheckAll)
			test(db->CountL()==loop); // +1 own card
		CContactItem* item=CContactCard::NewLC();
		TBuf<16> name;
		name.Format(KTestName,loop);
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldFamilyName);
		field->SetMapping(KUidContactFieldVCardMapUnusedN);
		field->TextStorage()->SetTextL(name);
		item->AddFieldL(*field);
		CleanupStack::Pop(); // field
		TContactItemId itemId=db->AddNewContactL(*item);
		if (loop==5)
			deleteItem=itemId;
		CleanupStack::PopAndDestroy(); // item
		}
	if (aCheckAll)
		test(db->CountL()==loop); // +1 own card
//
	CContactDatabase* db2=CntTest->OpenDatabaseL();
	test(db->CountL()==db2->CountL());
//
	db->DeleteContactL(deleteItem);
//
	CLetChangeNotiferRun *letChangeNotiferRun=new(ELeave) CLetChangeNotiferRun;
	CleanupStack::PushL(letChangeNotiferRun);
	letChangeNotiferRun->ConstructL();
	letChangeNotiferRun->After(1000000);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();	// letChangeNotiferRun
	test(db->CountL()==loop-1); //-1 deleted +1 own card
	test(db2->CountL()==loop-1); //-1 deleted +1 own card
//
	CntTest->CloseDatabase();
	}

LOCAL_C void CurrentItemSupportTest()
	{	
	test.Next(_L("CurrentItemSupport"));
	TInt dbCount = CntTest->Db()->CountL();
	test(dbCount>1);
	CContactItem* currentItem = CntTest->Db()->ReadContactLC(dbCount/2);
	CntTest->Db()->SetCurrentItem(currentItem->Id());
	TContactItemId currentItemId = CntTest->Db()->GetCurrentItem();
	test(currentItemId == currentItem->Id());
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	currentItemId = KNullContactId;
	currentItemId = CntTest->Db()->GetCurrentItem();
	test(currentItemId == currentItem->Id());
	CntTest->Db()->DeleteContactL(currentItem->Id());
	TContactItemId nullCurrentItemId = CntTest->Db()->GetCurrentItem();
	test(nullCurrentItemId==KNullContactId);
	CleanupStack::PopAndDestroy(); //currentItem
	CntTest->CloseDatabase();
	CntTest->OpenDatabaseL();
	nullCurrentItemId = CntTest->Db()->GetCurrentItem();
	test(nullCurrentItemId==KNullContactId);
	}
	
LOCAL_C void ContactIdArrayL()
	{
	test.Next(_L("ContactIdArray"));
	TContactItemId item1=1;
	TContactItemId item2=2;
	TContactItemId item3=3;
	TContactItemId item4=4;
	CContactIdArray* ids=CContactIdArray::NewL();
	CleanupStack::PushL(ids);	// ids
	test(ids->Count()==0);
	ids->AddL(item1);
	test(ids->Count()==1);
	ids->AddL(item2);
	ids->AddL(item3);
	test(ids->Count()==3);
	test((*ids)[0]==item1);
	test((*ids)[1]==item2);
	test((*ids)[2]==item3);
	ids->Remove(1);
	test((*ids)[0]==item1);
	test((*ids)[1]==item3);
	ids->Reset();
	test(ids->Count()==0);
	ids->AddL(item3);
	test(ids->Count()==1);
	delete ids;
	CleanupStack::Pop();	// ids
//
	CContactIdArray::NewLC();
	CleanupStack::PopAndDestroy();
//
	CContactIdArray* ids3=CContactIdArray::NewLC();
	ids3->AddL(item3);
	ids3->AddL(item2);
	ids3->AddL(item1);
	test(ids3->Count()==3);
	ids3->MoveL(1,0);
	test((*ids3)[0]==item2 && (*ids3)[1]==item3 && (*ids3)[2]==item1);
	ids3->MoveL(0,2);
	test((*ids3)[0]==item3 && (*ids3)[1]==item1 && (*ids3)[2]==item2);
	ids3->MoveL(2,0);
	test((*ids3)[0]==item2 && (*ids3)[1]==item3 && (*ids3)[2]==item1);
	ids3->MoveL(0,1);
	test((*ids3)[0]==item3 && (*ids3)[1]==item2 && (*ids3)[2]==item1);
	ids3->MoveL(1,2);
	test((*ids3)[0]==item3 && (*ids3)[1]==item1 && (*ids3)[2]==item2);
	ids3->MoveL(2,1);
	test((*ids3)[0]==item3 && (*ids3)[1]==item2 && (*ids3)[2]==item1);
	ids3->InsertL(1,item4);
	test(ids3->Count()==4);
	test((*ids3)[0]==item3 && (*ids3)[1]==item4 && (*ids3)[2]==item2 && (*ids3)[3]==item1);
//
	test(ids3->Find(1)==3);
	test(ids3->Find(2)==2);
	test(ids3->Find(3)==0);
	test(ids3->Find(4)==1);
//
	CContactIdArray::NewLC(ids3);
	CleanupStack::PopAndDestroy();	// New CContactIdArray
//
	CContactIdArray* ids4=CContactIdArray::NewL(ids3);
	CleanupStack::PushL(ids4);
	test(ids3->Count()==ids4->Count());
	TInt loop=0;
	for(;loop<ids4->Count();loop++)
		test((*ids3)[loop]==(*ids4)[loop]);
//
	CleanupStack::PopAndDestroy(2);	// ids4, ids3
//
	CStreamStore* store=CBufStore::NewLC(3);
	RStoreWriteStream tempwrite;
	TStreamId streamId=tempwrite.CreateLC(*store);
//
	ids=CContactIdArray::NewLC();
	for(loop=0;loop<100;loop++)
		ids->AddL(loop*3);
	tempwrite<<*ids;
	tempwrite.Close();
	store->CommitL();
	CleanupStack::PopAndDestroy(2);	// ids, stream
//
	RStoreReadStream tempread;
	tempread.OpenLC(*store,streamId);
//
	CContactIdArray *ids2=CContactIdArray::NewLC();
	tempread>>*ids2;
	for(loop=0;loop<100;loop++)
		test((*ids2)[loop]==loop*3);
	CleanupStack::PopAndDestroy(3);	// ids2,tempread,store
	}

LOCAL_C void ScanForChangesL()
	{
	test.Next(_L("Scan for changes"));
	CContactDatabase* db=CntTest->OpenDatabaseL();
	TTime before;
	before.UniversalTime();
	User::After(1000000);
	CContactItem* item=db->ReadContactLC(2); // assumes id 3 is valid
	User::After(1000000);
	const TInt KNumImports1=3;
	TInt ii=0;
	for (;ii<KNumImports1;ii++)
		{
		CContactCard* card=CContactCard::NewLC(item);
		db->AddNewContactL(*card);
		CleanupStack::PopAndDestroy(); // card
		}
	User::After(1000000);
	TTime during;
	during.UniversalTime();
	test(during>before);
	User::After(1000000);
	const TInt KNumImports2=5;
	for (ii=0;ii<KNumImports2;ii++)
		{
		CContactCard* card=CContactCard::NewLC(item);
		db->AddNewContactL(*card);
		CleanupStack::PopAndDestroy(); // card
		}
	User::After(1000000);
	TTime after;
	after.UniversalTime();
	test(after>during);
	CContactIdArray* ids=db->ContactsChangedSinceL(before);
	test(ids->Count()==KNumImports1+KNumImports2); // +1 template access count
	delete ids;
	ids=db->ContactsChangedSinceL(during);
	test(ids->Count()==KNumImports2); // +1 template access count
	delete ids;
	ids=db->ContactsChangedSinceL(after);
	test(ids->Count()==0);
	delete ids;
	CntTest->CloseDatabase();
	
	
	test.Next(_L("INC 66340 ContactsChangedSinceL incorrectly uses the TTime argument "));
	// Note that the defect fix for INC 66340 relies on the fix for 
	// DEF067226: TTime.Parse does not parse microseconds.
	// The problem was that ContactsChangedSinceL(t) was incorrectly rounding down 
	// the time to the next lowest second. 
	// Thus both the times used in the following test 
	// were rounded down to the SAME time. The time should not be
	// rounded. Only ONE conact has been addded since TTime t2.
	// Note there was TWO reasons for this defect:
	// (1) The SQL query string used only stipulated the time to the lowest second
	// (2) Ttime::Parse did not parse microseconds (i.e. ignored microseconds).
	
	
	db=CntTest->OpenDatabaseL();
	
	{
	TTime t1;
	t1.UniversalTime();
	CContactCard* card=CContactCard::NewLC(item);
	db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); 
	ids=db->ContactsChangedSinceL(t1);
	TInt c1 = ids->Count();
	test(c1==1); 
	delete ids;

	User::After(10); //ensure next time stamp is NOT same as previous 
	TTime t2;
	t2.UniversalTime();
	card=CContactCard::NewLC(item);
	db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); 
	ids=db->ContactsChangedSinceL(t2);
	TInt c2 = ids->Count();
	test(c2==1); 
	delete ids;
	}
	
	// but, of course, the above test could pass if TTime wraps around into 
	// the next second. So, assuming this test is all going to occur within
	//  ONE second, we merely have repeat again to accommodate this.
	{
	User::After(10); //ensure next time stamp is NOT same as previous 
	TTime t3;
	t3.UniversalTime();
	CContactCard* card=CContactCard::NewLC(item);
	db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); 
	ids=db->ContactsChangedSinceL(t3);
	TInt c3 = ids->Count();
	test(c3==1); 
	delete ids;

	User::After(10); //ensure next time stamp is NOT same as previous
	TTime t4;
	t4.UniversalTime();
	card=CContactCard::NewLC(item);
	db->AddNewContactL(*card);
	CleanupStack::PopAndDestroy(); 
	ids=db->ContactsChangedSinceL(t4);
	TInt c4 = ids->Count();
	test(c4==1); 
	delete ids;
	}
	CleanupStack::PopAndDestroy(); // item
	
	CntTest->CloseDatabase();
	

	
	}

void ChangePhoneNumber(TContactItemId aId, const TDesC &aNumber)
	{
	CContactItem* item=CntTest->Db()->OpenContactLX(aId);
	CleanupStack::PushL(item);
	SetNameL(*item,KUidContactFieldPhoneNumber,aNumber,EFalse);
	CntTest->Db()->CommitContactL(*item);
	CleanupStack::PopAndDestroy();	// item
	CleanupStack::Pop();	// close record
	}


LOCAL_C TInt FileSizeL()
	{
	TEntry entry;
	User::LeaveIfError(CntTest->Fs().Entry(KPhysicalPathToDatabase, entry));
	return(entry.iSize);
	}

LOCAL_C void TestFileSizeL()
	{
	const TInt KMaxAddContacts=10;

	test.Next(_L("Test File Size"));
	CContactDatabase *db=CntTest->CreateDatabaseL();
//
	TContactItemId id0=AddPhoneContact(_L("x1"), _L("123"));
	db->CompactL();
	TContactItemId ids[KMaxAddContacts];
	for(TInt loop1=1;loop1<KMaxAddContacts;loop1++)
		{
		TInt size1=FileSizeL();
		TInt loop2=0;
		for(;loop2<loop1;loop2++)
			ids[loop2]=AddPhoneContact(_L("x2"), _L("234"));
		for(loop2=0;loop2<loop1;loop2++)
			db->DeleteContactL(ids[loop2]);
		test(db->FileSize()-db->WastedSpaceInBytes()>0);
		
		db->CompactL();
		TInt size2=FileSizeL();
		if (size1!=size2)
			db->CountL();
		test(size2==db->FileSize());
		}
//
	db->DeleteContactL(id0);
	CntTest->CloseDatabase();
	}

LOCAL_C void DisplayModelVersionNumber()
	{
	TVersion version=CntTest->Db()->Version();
	TBuf<64> buf;
	buf.Format(_L("Model version %03d.%03d (%d)\n"),version.iMajor,version.iMinor,version.iBuild);
	test.Printf(buf);
	}

LOCAL_C void SortByTypeTest()
	{
	// test sort by type
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();

//create groups
	CntTest->Db()->CreateContactGroupLC(_L("New Group 1"));
	CntTest->Db()->CreateContactGroupLC(_L("New Group 2"));
// create owncard
	CntTest->Db()->CreateOwnCardLC();
// create multiple templates
	_LIT(KTestLabel ,"Test label");
	CntTest->Db()->CreateContactCardTemplateLC(KTestLabel,EFalse);
	CntTest->Db()->CreateContactCardTemplateLC(KTestLabel,EFalse);
	CntTest->Db()->CreateContactCardTemplateLC(KTestLabel,EFalse);
	
//	do the sort and count
	CArrayFix<CContactDatabase::TSortPref>* sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName,CContactDatabase::TSortPref::EAsc));
//
	CntTest->Db()->SetDbViewContactType(KUidContactItem);
	CntTest->Db()->SortByTypeL(sortOrder); // takes ownership of sortorder
	const CContactIdArray *itemIds=CntTest->Db()->SortedItemsL();
	test(itemIds->Count()==6);
//
	sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName,CContactDatabase::TSortPref::EAsc));
	CntTest->Db()->SetDbViewContactType(KUidContactGroup);
	CntTest->Db()->SortByTypeL(sortOrder); // takes ownership of sortOrder
	const CContactIdArray *groupIds=CntTest->Db()->SortedItemsL();
	test(groupIds->Count()==2);
//
	sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName,CContactDatabase::TSortPref::EAsc));
	CntTest->Db()->SetDbViewContactType(KUidContactOwnCard);
	CntTest->Db()->SortByTypeL(sortOrder); // takes ownership of sortOrder
	const CContactIdArray *ownCardIds=CntTest->Db()->SortedItemsL();
	test(ownCardIds->Count()==1);
//
	sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName,CContactDatabase::TSortPref::EAsc));
	CntTest->Db()->SetDbViewContactType(KUidContactCard);
	CntTest->Db()->SortByTypeL(sortOrder);  // takes ownership of SortOrder
	const CContactIdArray *cardIds=CntTest->Db()->SortedItemsL();
	test(cardIds->Count()==1);
//
	sortOrder=new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName,CContactDatabase::TSortPref::EAsc));
	CntTest->Db()->SetDbViewContactType(KUidContactCardTemplate);
	CntTest->Db()->SortByTypeL(sortOrder); // takes ownership of sortOrder
	const CContactIdArray *templateIds=CntTest->Db()->SortedItemsL();
	test(templateIds->Count()==3);
//
	CleanupStack::PopAndDestroy(6); // newGroup1&2 ownCard1 multiTemplate1,2&3
	}

LOCAL_C void DeleteManyContacts()
	{
	test.Next(_L("Test DeleteContacts()"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	PopulateDatabaseL(EFalse);

	CContactIdArray* contactIds=CContactIdArray::NewLC();
	for(TInt i=1;i<KTotalNumRecords;i++)
		{
		contactIds->AddL(i);
		}
	test(contactIds->Count()==KTotalNumRecords-1);
	CntTest->Db()->DeleteContactsL(*contactIds);
	CleanupStack::PopAndDestroy(); // contactIds
	}


LOCAL_C void TestFileUidL()
    {
	CntTest->CreateDatabaseL();
	TBuf<40> fileUid1;
	fileUid1=CntTest->Db()->FileUid();
	CntTest->CloseDatabase();
	User::After(50000);	// as UID is time generated make sure time is different
	CntTest->OpenDatabaseL();
	test(fileUid1==CntTest->Db()->FileUid());
	CntTest->CloseDatabase();
	CntTest->CreateDatabaseL();
	test(fileUid1!=CntTest->Db()->FileUid());	// Should be different in a new DB
	CntTest->CloseDatabase();
	}

/** 
 * Regression test for EXT-53JGEN "CContactDatabase::GroupCount() 
 * crashes when called to a brand new default database"
 */
LOCAL_C void GroupCountRegressionTestL()
	{
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	CntTest->Db()->GroupCount();
	}

/** Check that FindLC() doesn't find aSearchString in the database */
LOCAL_C void CheckNoMatchesL(TFieldType aFieldType, const TDesC& aSearchString)
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(aFieldType);
	CContactIdArray* ids=CntTest->Db()->FindLC(aSearchString,fieldDef);
	test(ids->Count()==0);
	CleanupStack::PopAndDestroy(2, fieldDef);
	}	

/** 
 * Regression test to ensure that contact data is really deleted.
 * Phone number fields are tested by T_NOMACH
 */
LOCAL_C void DeleteContactDataRegressionTestL()
	{
	test.Next(_L("Test contact data isn't found for deleted items"));
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	_LIT(KIdentity,"Identity");
	_LIT(KContacts,"Contacts");
	_LIT(KEmail,"email.address@symbian.com");
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,KIdentity,ETrue);
	SetNameL(*item,KUidContactFieldAddress,KUidContactFieldVCardMapADR,KContacts, ETrue);
	SetNameL(*item,KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET,KEmail,ETrue);
	SetNameL(*item,KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET,KEmail,ETrue);
	item->SetTemplateRefId(KGoldenTemplateId);
	TContactItemId id=CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item); 
	CntTest->Db()->DeleteContactL(id);
	CheckNoMatchesL(KUidContactFieldGivenName,KIdentity);
	CheckNoMatchesL(KUidContactFieldAddress,KContacts);
	CheckNoMatchesL(KUidContactFieldEMail,KEmail);
	CntTest->CloseDatabase();
	}

LOCAL_C void FindContactWithApostropheL()
	{
	test.Next(_L("Test that you can find contacts with an apostrophe in their name"));
	
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	CntTest->CreateDatabaseL();
	
	_LIT(KSearchName1, "diner's");	//word with single apostrophe
	_LIT(KSearchName2, "diner's'");	//word ending with apostrophe
	_LIT(KSearchName3, "'");		//single apostrophe
	_LIT(KSearchName4, "dime''s");	//word with double apostrophe
	_LIT(KSearchName5, "test's test's test"); //exercise both realloc's  

	_LIT(KIdentity1,"diner's");
	_LIT(KIdentity2,"diner's'");
	_LIT(KIdentity3,"'");
	_LIT(KIdentity4,"dime''s'");
	_LIT(KContacts,"Contacts");
	_LIT(KEmail,"email.address@symbian.com");

	//create items
	CContactItem* item=CContactCard::NewLC();
	SetNameL(*item,KUidContactFieldGivenName,KUidContactFieldVCardMapUnusedN,KIdentity1,ETrue);
	SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,KIdentity2,ETrue);
	SetNameL(*item,KUidContactFieldAdditionalName,KUidContactFieldVCardMapUnusedN,KIdentity3,ETrue);
	SetNameL(*item,KUidContactFieldCompanyName,KUidContactFieldVCardMapUnusedN,KIdentity4,ETrue);
	SetNameL(*item,KUidContactFieldAddress,KUidContactFieldVCardMapADR,KContacts, ETrue);
	SetNameL(*item,KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET,KEmail,ETrue);
	SetNameL(*item,KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET,KEmail,ETrue);
	SetNameL(*item,KUidContactFieldEMail,KUidContactFieldVCardMapEMAILINTERNET,KSearchName5,ETrue);
	
	item->SetTemplateRefId(KGoldenTemplateId);
	CntTest->Db()->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(item); 

    //create definition
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldGivenName);
	fieldDef->AppendL(KUidContactFieldFamilyName);
	fieldDef->AppendL(KUidContactFieldAdditionalName);
	fieldDef->AppendL(KUidContactFieldCompanyName);
	
    //search for contact1
	CContactIdArray* result = CntTest->Db()->FindLC(KSearchName1, fieldDef);
	test(result->Count()==1);

	//search for contact2
	CContactIdArray* result2 = CntTest->Db()->FindLC(KSearchName2, fieldDef);
	test(result2->Count()==1);
	
	//search for contact3
	CContactIdArray* result3 = CntTest->Db()->FindLC(KSearchName3, fieldDef);
	test(result3->Count()==1);
	
	//search for contact4
	CContactIdArray* result4 = CntTest->Db()->FindLC(KSearchName4, fieldDef);
	test(result4->Count()==1);
	
	CContactIdArray* result5 = CntTest->Db()->FindLC(KSearchName5, fieldDef);
	test(result5->Count()==1);
		
	CleanupStack::PopAndDestroy(6); // fieldDef, 4 x results

    CntTest->CloseDatabase();
    }

LOCAL_C TInt AsyncFindByEmailTest(const TDesC& aEmailAddress)
	{
		//Returns the number of items that match the aEmailAddress
		//search criteria.

		CContactItemFieldDef *fieldDef=new(ELeave) CContactItemFieldDef;
		CleanupStack::PushL(fieldDef);
		fieldDef->AppendL(KUidContactFieldEMail);

		CAsyncFinder *finder=new(ELeave) CAsyncFinder;
		CleanupStack::PushL(finder);
		finder->ConstructL(CntTest->Db(),aEmailAddress,fieldDef);

		CActiveScheduler::Start();

		CContactIdArray* ids=finder->TakeContactIds();

		TInt resultCount = ids->Count();

		CleanupStack::PopAndDestroy(2); // finder, contactitemfielddef
		delete ids;
		return resultCount;
	}


void ResetContactDbL(CContactDatabase* aDb)
	{
    CContactIdArray* idList = CContactIdArray::NewLC(aDb->SortedItemsL());
    aDb->DeleteContactsL(*idList);
    CleanupStack::PopAndDestroy(idList);
	}

void GenerateContactsDbL(CContactDatabase* aDb,TUint acontactCount,TContactItemId* acontactIdArray)
	{
    ResetContactDbL(aDb);    
    for (TUint i = 0; i < acontactCount; i++)
		{
		TBuf<15> name;
		name.Format(_L("contact#%d"), i);
		CContactCard* contact = CContactCard::NewLC();        
		*(acontactIdArray + i) = aDb->AddNewContactL(*contact);        
		CleanupStack::PopAndDestroy(contact);
		}
	}

void RollBackAndRecoverL()
	{
	test.Next(_L("Test that after a rollback, due to a failure in deleting, there is a recover"));
	TInt err = 0;
	TInt i = 0;
	CContactCard* contact = NULL;
    const TInt contactCount = 10;

    TContactItemId contactIds[contactCount]; 

    CContactDatabase* contactDb = CContactDatabase::ReplaceL();
    CleanupStack::PushL(contactDb);

	// 1) Generate 10 contacts and keep the IDs in an array
    GenerateContactsDbL(contactDb, contactCount, contactIds);
  
	// 2) Delete the first 2 contacts
    CContactIdArray* contactIdList = CContactIdArray::NewLC(contactDb->SortedItemsL());
    contactDb->DeleteContactL(contactIds[0]);
    contactDb->DeleteContactL(contactIds[1]);
    CleanupStack::PopAndDestroy(contactIdList);
    // 3) Check they are not in the list anymore using CContactIdArray::Find() and CContactDatabase::ReadContactLC()
    contactIdList = CContactIdArray::NewLC(contactDb->SortedItemsL());
	test(contactIdList->Find(contactIds[0]) == KErrNotFound);
    test(contactIdList->Find(contactIds[1]) == KErrNotFound);
    TRAP(err, contact = static_cast<CContactCard*>(contactDb->ReadContactLC(contactIds[0])));
	test(err == KErrNotFound && contact == NULL);
    TRAP(err, contact = static_cast<CContactCard*>(contactDb->ReadContactLC(contactIds[1])));
	test(err == KErrNotFound && contact == NULL);
    CleanupStack::PopAndDestroy(contactIdList);

	// 4) Try deleting a contact(that has already been deleted) using CContactDatabase::DeleteContactsL() .
	//	  We should get a KErrNotfound leave  which will cause a rollback 
    contactIdList = CContactIdArray::NewLC(contactDb->SortedItemsL());
    // now add one not existing
    contactIdList->AddL(contactIds[0]); // this was deleted at step 1.
    TRAP(err, contactDb->DeleteContactsL(*contactIdList)); 
    test(err == KErrNotFound);

    // 5) Recover and check that after the rollback no contacts were deleted 
	contactDb->RecoverL();
    for (i = 0; i < contactIdList->Count() - 1; i++) // all but last
		{
        CContactCard* contact = NULL;
        TRAP(err, contact = static_cast<CContactCard*>(contactDb->ReadContactL((*contactIdList)[i])));
        if (contact)
			delete contact;
		test((err == KErrNone) && (contact != NULL));
		}
    CleanupStack::PopAndDestroy(contactIdList);

	// 1) Try deleting a contact(that is locked) using CContactDatabase::DeleteContactsL()
	//    We should get a KErrInUse leave  which will cause a rollback 
    contactIdList = CContactIdArray::NewLC(contactDb->SortedItemsL());

    // open last one
    TContactItemId lastItem = (*contactIdList)[contactIdList->Count() - 1];
    contact = static_cast<CContactCard*>(contactDb->OpenContactL(lastItem));
    CleanupStack::PushL(contact);

    TRAP(err, contactDb->DeleteContactsL(*contactIdList));
    test(err == KErrInUse);

    CContactDbEventQueue* dbEventQueue = CContactDbEventQueue::NewL(contactDb);
	CleanupStack::PushL(dbEventQueue);
	dbEventQueue->Flush();

	// 2) Recover and check that after the rollback no contacts were deleted 
	contactDb->RecoverL();

	//wait until EContactDbObserverEventRecover message arrives
    TContactDbObserverEvent event;
	while (dbEventQueue->ListenForEvent(3,event) && 
		   event.iType != EContactDbObserverEventRecover)
        {} 
	CleanupStack::PopAndDestroy(dbEventQueue);

    contactDb->CloseContactL(lastItem);
    CleanupStack::PopAndDestroy(contact);
    // check all exist
    for (i = 0; i < contactIdList->Count(); i++) 
		{
        CContactCard* contact = NULL;
        TRAP(err, contact = static_cast<CContactCard*>(contactDb->ReadContactL((*contactIdList)[i])));
        if (contact) 
			delete contact;
        test((err == KErrNone) && (contact != NULL));
		}
	
    CleanupStack::PopAndDestroy(contactIdList);

    // ok, now everything is ok here
    CleanupStack::PopAndDestroy(contactDb);
	}

// This is a test case related to defect INC084290: Contact data being overwritten.
// It check that CommitContactL can be called multiple times without problem.
void CommitMultipleContactsL()
	{
	test.Next(_L("Test multiple calls to CommitContactL"));
	
	// Populate the database with 2 contacts
	_LIT(contactName1, "Bob");
	_LIT(contactName2, "Fred");
	
	CContactItem* item1 = CContactCard::NewLC();
	SetNameL(*item1, KUidContactFieldGivenName, contactName1, ETrue);
	TContactItemId itemId1 = CntTest->Db()->AddNewContactL(*item1);
	CleanupStack::PopAndDestroy(item1);
	
	CContactItem* item2 = CContactCard::NewLC();
	SetNameL(*item2, KUidContactFieldGivenName, contactName2, ETrue);
	TContactItemId itemId2 = CntTest->Db()->AddNewContactL(*item2);
	CleanupStack::PopAndDestroy(item2);
	
	CntTest->CloseDatabase();
	
	// Open the contacts and call CommitContactL
	CntTest->OpenDatabaseL();
	CContactItem *openedItem1 = CntTest->Db()->OpenContactL(itemId1);
	CleanupStack::PushL(openedItem1);
	CContactItem *openedItem2 = CntTest->Db()->OpenContactL(itemId2);
	CleanupStack::PushL(openedItem2);
	CntTest->Db()->CommitContactL(*openedItem1);
	CntTest->Db()->CommitContactL(*openedItem2);
	CleanupStack::PopAndDestroy(openedItem2);
	openedItem2 = 0;
	CleanupStack::PopAndDestroy(openedItem1);
	openedItem1 = 0;
	
	// Get the contacts and check their contents
	openedItem1 = CntTest->Db()->ReadContactLC(itemId1);
	CContactItemFieldSet& fields1=openedItem1->CardFields();
	TPtrC fieldText1=fields1[0].TextStorage()->Text();
	test(fieldText1==contactName1);
		
	openedItem2 = CntTest->Db()->ReadContactLC(itemId2);
	CContactItemFieldSet& fields2=openedItem2->CardFields();
	TPtrC fieldText2=fields2[0].TextStorage()->Text();
	test(fieldText2==contactName2);
		
	CleanupStack::PopAndDestroy(openedItem2);
	CleanupStack::PopAndDestroy(openedItem1);	
	}

/**

@SYMTestCaseID     PIM-T-DBASE2-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-DBASE2-0001 T_DBASE2"));
	TRAPD(err,TestFileSizeL());
	test(err==KErrNone);
	test.Next(_L("Create new database"));
	
	//Begin test procedure for DEF036083
	CntTest->CreateDatabaseL();
	TInt numberOfItemsWithEmailField = PopulateDatabaseForAsyncFindByEmailAddrTest(KTestEmailAddress);
	TInt numberOfItemsFoundWithEmailField = AsyncFindByEmailTest(KTestEmailAddress);
	test(numberOfItemsWithEmailField == numberOfItemsFoundWithEmailField);  //verify that all entries are found
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
	//End test procedure for DEF036083

  	TRAP(err,TestFileUidL());
	CntTest->CreateDatabaseL();
	CntTest->CloseDatabase();
	TRAP(err,CreateExistingDatabaseTestL());
//
	CntTest->OpenDatabaseL();
	CntTest->DeleteAllTemplateFieldsL();
	DisplayModelVersionNumber();
	test(err==KErrNone);
	TRAP(err,PopulateDatabaseL(EFalse));
	test(err==KErrNone);
	LabelValidating();
	TRAP(err,NavigateL());
	test(err==KErrNone);
// group population
	TRAP(err,CreateBasicContactGroupsL());
	test(err==KErrNone);
// own card
	TRAP(err,CreateBasicOwnCardL());
	test(err==KErrNone);
// finding
	TRAP(err,FindL());
	test(err==KErrNone);
	TRAP(err,FindAsyncL());
	test(err==KErrNone);
	TRAP(err,FindAsyncInTextDefL());
	test(err==KErrNone);

//
	TRAP(err,FindDefectL());
	test(err==KErrNone);

	TRAP(err,FindScandinavianLettersL());
	test(err==KErrNone);

	//test added for defect IC049017: NTT-FindAsyncL and FindLC do not search identitiy fields when KUidContactFieldMatchAll
	TRAP(err,FindUsingKUidContactFieldMatchAllL());
	test(err==KErrNone);

// Following test create their own database
	TRAP(err,TestDeletingRecords());
	test(err==KErrNone);
// checking for templateRefId being updated in the Database tables.
	TRAP(err,CheckTemplateRefId());
	test(err==KErrNone);

	TRAP(err,CrystalSortRegressionTestL());
//	TRAP(err,SortL());
//#pragma message ("Sorting testcode isn't run at the moment because the tests are invalid")
	test(err==KErrNone);
	TRAP(err,CountL(ETrue));
	test(err==KErrNone);
	TRAP(err,CountL(EFalse));
	test(err==KErrNone);
	TRAP(err,ContactIdArrayL());
	test(err==KErrNone);
	TRAP(err,ScanForChangesL());
	test(err==KErrNone);
	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();
//
	CntTest->CreateDatabaseL();
	TRAP(err,PopulateDatabaseL(ETrue));
	test(err==KErrNone);
//
	TRAP(err,CurrentItemSupportTest());
	test(err==KErrNone);
//
	test(err==KErrNone);
//	TRAP(err,TestPhoneNumberMatchingL());
	test(err==KErrNone);
//
	TRAP(err,SortByTypeTest());
	test(err==KErrNone);

	TRAP(err,DeleteManyContacts());
	test(err==KErrNone);
	TRAP(err,GroupCountRegressionTestL());
	test(err==KErrNone);

	TRAP(err, DeleteContactDataRegressionTestL());
	test(err==KErrNone);

	TRAP(err, FindContactWithApostropheL());
	test(err==KErrNone);

	TRAP(err, RollBackAndRecoverL());
	test(err==KErrNone);

	CntTest->CloseDatabase();
	CntTest->DeleteDatabaseL();

	CntTest->CreateDatabaseL();
	TRAP(err, CommitMultipleContactsL());
	test(err==KErrNone);	
	CntTest->CloseDatabase();
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
