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
#include <versit.h>
#include <s32file.h>
#include "t_utils.h"

//
// CCntTest
//
EXPORT_C CCntTest::~CCntTest()
	{
	delete iDb;
	delete iCleanup;
	iFs.Close();
	// RTest object is created in each test harness and does a __UHEAP_MARK
	// the Close() method does a __UHEAP_MARKEND and so must now be the last action
    iTest->Close();
	}

EXPORT_C void CCntTest::ConstructL(RTest &aTest, const TDesC &aDatabaseName)
	{
	iDatabaseName.Copy(aDatabaseName);
	CActiveScheduler::Install(new(ELeave) CActiveScheduler);
	iTest=&aTest;
	iTest->Title();
    iCleanup=CTrapCleanup::New();
	User::LeaveIfError(iFs.Connect());
	}

EXPORT_C void CCntTest::SelectDriveL()
	{
	TDriveList driveList;
	iFs.DriveList(driveList);
	driveList[25]=0;	// Get rid of Z:
	TBuf<2> drive;
	drive.Copy(_L("C:"));
	TInt driveCount=0;
	for(TInt loop0=0;loop0<driveList.Size();loop0++)
		if (driveList[loop0])
			driveCount++;
	if (driveCount>1)
		{
		iTest->Printf(_L("Select drive for multi access test:\n"));
		for(TInt loop=0;loop<driveList.Size();loop++)
			if (driveList[loop])
				iTest->Printf(_L("%c:\n"),loop+'A');
		FOREVER
			{
			TChar key=iTest->Getch();
			TInt index=key.GetLowerCase()-'a';
			if (index>=0 && index<driveList.Size() && driveList[index])
				{
				drive[0]=(TText)(index+'A');
				break;
				}
			}
		}
	TParse parse;
	parse.Set(drive,&iDatabaseName,NULL);
	iDatabaseName.Copy(parse.FullName());
	}

EXPORT_C void CCntTest::EndTestLib(TInt aErr)
	{
    (*iTest)(aErr==KErrNone);

	iTest->End();
	(*iTest)(iFs.ResourceCount()==0);
	delete CActiveScheduler::Current();
	delete this;
	}

EXPORT_C CContactDatabase *CCntTest::CreateDatabaseL()
	{
	CloseDatabase();
	iDb=CContactDatabase::ReplaceL(iDatabaseName);
	return(iDb);
	}

EXPORT_C CContactDatabase* CCntTest::OpenDatabaseL()
	{
	CloseDatabase();
	iDb=CContactDatabase::OpenL(iDatabaseName);
	return(iDb);
	}

EXPORT_C void CCntTest::CloseDatabase()
	{
	delete iDb;
	iDb=NULL;
	}

EXPORT_C void CCntTest::DeleteDatabaseL()
	{
	CContactDatabase::DeleteDatabaseL(iDatabaseName);	
	}

EXPORT_C void CCntTest::DeleteAllTemplateFieldsL()
	{
	CContactItemViewDef *viewDef=CContactItemViewDef::NewL(CContactItemViewDef::EMaskFields,CContactItemViewDef::EIncludeHiddenFields);
	CContactItem *item=iDb->OpenContactLX(iDb->TemplateId(),*viewDef);
	delete viewDef;
	CleanupStack::PushL(item);
	CContactItemFieldSet& fieldSet=item->CardFields();
	while(fieldSet.Count())
		fieldSet.Remove(0);
	iDb->CommitContactL(*item);
	CleanupStack::PopAndDestroy(2); // item,close
	}

EXPORT_C void CCntTest::TestField(CContactItemField &aField,TStorageType aType,TFieldType aFieldType,TUid aMapping)
	{
	(*iTest)(aField.StorageType()==aType);
	(*iTest)(aField.ContentType().ContainsFieldType(aFieldType));
	(*iTest)(aField.ContentType().Mapping()==aMapping);
	}

EXPORT_C void CCntTest::AdjustContactAccessCountL(TContactItemId aItemId, TInt aCount)
	{
	CContactItem *incItem=iDb->OpenContactLX(aItemId);
	CleanupStack::PushL(incItem);
	while(aCount>0)
		{
		incItem->IncAccessCount();
		aCount--;
		}
	while(aCount<0)
		{
		incItem->DecAccessCount();
		aCount++;
		}
	iDb->CommitContactL(*incItem);
	CleanupStack::PopAndDestroy(2);	// incItem, Close(incItem)
	}

EXPORT_C void CCntTest::TestAccessCount(TContactItemId aItemId, TInt aCount)
	{
	CContactItem *item=iDb->ReadContactLC(aItemId);
	(*iTest)(item->AccessCount()==aCount);
	CleanupStack::PopAndDestroy();	// item
	}

EXPORT_C TBool CCntTest::CompareFields(CContactItemField &aField1, CContactItemField &aField2)
	{
	return(aField1.Label()==aField2.Label() &&
			aField1.StorageType()==aField2.StorageType() &&
			aField1.ContentType()==aField2.ContentType());
	}

EXPORT_C void CCntTest::CheckDeletedContact(TContactItemId aDeleteId)
	{
	CContactItem *deletedContact=iDb->ReadContactLC(aDeleteId);
	(*iTest)(deletedContact->IsDeleted());
	(*iTest)(deletedContact->CardFields().Count()==0);
	TBuf<10> textDef;
	iDb->ReadContactTextDefL(aDeleteId,textDef);
	(*iTest)(textDef.Length()==0);
	CleanupStack::PopAndDestroy();	// deletedContact
	}

EXPORT_C void CCntTest::CheckContactDoesNotExist(TContactItemId aNotExistId)
	{
	TRAPD(notExist,iDb->ReadContactLC(aNotExistId));
	(*iTest)(notExist==KErrNotFound);
	}

EXPORT_C void CCntTest::DeleteContact(TContactItemId aDeleteMe)
	{
	TRAP_IGNORE(iDb->DeleteContactL(aDeleteMe));
	}

EXPORT_C HBufC *CCntTest::ContactUidLC(TContactItemId aId, TInt64 aMachineId)
	{
	CContactItem *contact=iDb->ReadContactLC(aId);
	HBufC *uid=contact->UidStringL(aMachineId).AllocL();
	CleanupStack::PopAndDestroy();	// deletedContact
	CleanupStack::PushL(uid);
	return(uid);
	}

EXPORT_C CVCardTestStore* CCntTest::ExportContactsLC(const CContactIdArray *aIds, TUint aFormatFlags, TVCardStoreType aType, TInt aFileNum)
	{
	CVCardTestStore* store=ExportContactsL(iDb,aIds,aFormatFlags, aType, &iFs, aFileNum);
	CleanupStack::PushL(store); // outstream
	return(store);
	}

EXPORT_C CVCardTestStore* CCntTest::ExportContactLC(TContactItemId aId, TUint aFormatFlags, TVCardStoreType aType, TInt aFileNum)
	{
	CContactIdArray *ids=CContactIdArray::NewLC();
	ids->AddL(aId);
	CVCardTestStore* store=ExportContactsLC(ids,aFormatFlags, aType, aFileNum);
	CleanupStack::Pop(); // store
	CleanupStack::PopAndDestroy(); // ids
	CleanupStack::PushL(store);
	return(store);
	}

EXPORT_C void CCntTest::ImportContacts1by1L(CVCardTestStore* aStore, TUint aFormatFlags)
	{
	RStoreReadStream instream;
	instream.OpenLC(*aStore->Store(),aStore->StreamId());
	TBool success=EFalse;
	do
		{
		CArrayPtr<CContactItem>* items=iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),instream,success,aFormatFlags);
		items->ResetAndDestroy();
		delete items;
		} while(success);
	CleanupStack::PopAndDestroy(); // instream
	}

EXPORT_C CArrayPtr<CContactItem>* CCntTest::ImportContactsL(CVCardTestStore* aStore, TUint aFormatFlags)
	{
	return(::ImportContactsL(iDb,aStore,aFormatFlags));
	}

EXPORT_C CArrayPtr<CContactItem>* CCntTest::ImportContactsLC(CVCardTestStore* aStore, TUint aFormatFlags)
	{
	CArrayPtr<CContactItem>* items=ImportContactsL(aStore, aFormatFlags);
	CleanupStack::PushL(TCleanupItem(CleanUpResetAndDestroy,items));
	return(items);
	}

class RTestSession : public RSessionBase
	{
public:
	TInt CreateSession();
	};

TInt RTestSession::CreateSession()
	{
	TVersion version;
	return(RSessionBase::CreateSession(_L("CntLockServer"),version,1));
	}

EXPORT_C TBool CCntTest::LockServerSessionExists(TBool aPause) const
	{
	if (aPause)
		User::After(200000);
	RTestSession session;
	TInt sessionErr=session.CreateSession();
	session.Close();
	return(sessionErr==KErrNone);
	}

EXPORT_C void CCntTest::PrintfNewline() const
	{
	(*iTest).Printf(_L("\n"));
	}

EXPORT_C TInt CCntTest::LockServerProcessCount(TBool aPause, TBool aDisplayCount, TBool aDisplayList) const
	{
	if (aPause)
		User::After(200000);
	TInt count=0;
	TFullName fullName;
	TFindProcess find(_L("cntsrv*"));
	if (aDisplayList)
		iTest->Printf(_L("Lock server process list\n"));
	while(find.Next(fullName)==KErrNone)
		{
		count++;
		if (aDisplayList)
			{
			iTest->Printf(fullName);
			PrintfNewline();
			}
		}
	if (aDisplayCount)
		{
		iTest->Printf(_L("Process count=%d"),count);
		PrintfNewline();
		}
	return(count);
	}

EXPORT_C void CCntTest::TestLockServerExists() const
	{
	(*iTest)(LockServerProcessCount(EFalse,EFalse,ETrue)==1);
	}

EXPORT_C void CCntTest::TestNoLockServer() const
	{
	TInt retries=0;
	FOREVER
		{
		TInt count=LockServerProcessCount(EFalse,EFalse,ETrue);
		if (count==0)
			break;
		if (retries==5)
			{
			iTest->Printf(_L("Error lock server count=%d\nPress 't' to try again\n"),count);
			TChar key=iTest->Getch();
			if (key=='t')
				retries--;
			else
				(*iTest)(EFalse);
			}
		User::After(100000);
		retries++;
		}
	}


EXPORT_C void CCntTest::TestLockServerCloses() const
	{
	// Contacts Lock Server waits 5sec from last client until shutdown
	TInt time = 0;
	TInt count = LockServerProcessCount();
	
	while (count)
		{
		if (time > 55)
			{ // 5.5 sec - too slow
			(*iTest)(EFalse);
			}
		// wait a 1/10th second, then count again
		User::After(100000);
		time++;
		count = LockServerProcessCount();
		}
	}
	

/*
The RDebug::Profile APIs are removed from secure builds using
the __SECURE_API__ macro.

Although the APIs are present in EKA2, the EKA2 macro has been
used because the implementation of the RDebug::Profile APIs 
does not do anything in EKA2.

In EKA2
TCntProfileTls is stored in 'Thread Local Storage' and contains
a TTime, used to store the time when ProfileStart() is called
and a TCntProfile, used to store the time elapsed and to count
the pairs of calls to ProfileStart() and ProfileEnd().
Only 1 'Profile Bin' can be used in EKA2 with this approach.

The 'Thread Local Storage' is stored on the heap so therfore
if a call to ProfileStart() is made then it must be followed
by a call to either ProfileReset() or ProfileResult() to ensure
the memory is freed
*/

EXPORT_C TInt CCntTest::ProfileResult(TCntProfile* aProfile,TInt,TInt)
	{
	TCntProfileTls* tls = (TCntProfileTls*)Dll::Tls();
	if(!tls)
  		{
  		// The tls has not been created
  		// do nothing
  		}
  	else
  		{
  		*aProfile = tls->iProfile;
  		delete tls;
  		Dll::FreeTls();
  		}
	return KErrNone;
	}

EXPORT_C TInt CCntTest::ProfileReset(TInt,TInt)
	{
	// Always exit with no tls allocated
	TCntProfileTls* tls = (TCntProfileTls*)Dll::Tls();
	if(!tls)
		{
  		// The tls has not been created
  		// do nothing
		}
	else
		{
  		delete tls;
		Dll::FreeTls();
  		}
	return KErrNone;
	}

EXPORT_C TInt CCntTest::ProfileStart(TInt)
	{
	TCntProfileTls* tls = (TCntProfileTls*)Dll::Tls();
	if(!tls)
  		{
  		tls = new (ELeave) TCntProfileTls();
  		Dll::SetTls(tls);
  		tls->iProfile.iTime = 0;
  		tls->iProfile.iCount = 0;
  		}
	++tls->iProfile.iCount;
	tls->iTime.UniversalTime();
	return KErrNone;
	}

EXPORT_C TInt CCntTest::ProfileEnd(TInt)
	{
	TCntProfileTls* tls = (TCntProfileTls*)Dll::Tls();
	if(!tls)
  		{
  		// The tls has not been created
  		// do nothing
  		}
  	else
  		{
		TTime myTime;
		myTime.UniversalTime();
		tls->iProfile.iTime += ((myTime).MicroSecondsFrom(tls->iTime)).Int64();
  		}
  	return KErrNone;
	}

/*
The RDebug::Profile APIs are present and working on EKA1 (keep these for when 
this functionality is reinstated for EKA2.



EXPORT_C TInt CCntTest::ProfileResult(TCntProfile* aProfile,TInt aStart,TInt aCount)
	{
	TProfile* p = reinterpret_cast<TProfile*>(&aProfile[0]);
	const TInt result = RDebug::ProfileResult(p,aStart,aCount);
	return result;
	}

EXPORT_C TInt CCntTest::ProfileReset(TInt aStart,TInt aCount)
	{
	return RDebug::ProfileReset(aStart, aCount);
	}

EXPORT_C TInt CCntTest::ProfileStart(TInt aProfile)
	{
	return RDebug::ProfileStart(aProfile);
	}

EXPORT_C TInt CCntTest::ProfileEnd(TInt aProfile)
	{
	return RDebug::ProfileEnd(aProfile);
	}
*/


EXPORT_C TBool TestGroupStateL(CContactDatabase* aDb, TInt aGroupCount, TInt aGroupEntryCount)
	{
	CContactIdArray* groupIdList=aDb->GetGroupIdListL();
	CleanupStack::PushL(groupIdList);
	TInt groupCount=groupIdList->Count();
	if (aGroupCount>=0 && groupCount!=aGroupCount)
		return(EFalse);
	TInt groupEntryCount=0;
// Cross check that all contacts think they are in all the groups that think they contain the contact
	for(TInt groupIndex=0;groupIndex<groupCount;groupIndex++)
		{
		TContactItemId groupId=(*groupIdList)[groupIndex];
		CContactGroup *group=(CContactGroup *)aDb->ReadContactLC(groupId);
		const CContactIdArray* itemsContained=group->ItemsContained();
		TInt itemCount=itemsContained->Count();
		groupEntryCount+=itemCount;
		for (TInt itemLoop=0;itemLoop<itemCount;itemLoop++)
			{
			CContactItemPlusGroup *contact=(CContactItemPlusGroup*)aDb->ReadContactLC((*itemsContained)[itemLoop]);
			CContactIdArray* groupsJoined=contact->GroupsJoinedLC();
			TInt groupsJoinedLoop=0;
			for (;groupsJoinedLoop<groupsJoined->Count();groupsJoinedLoop++)
				{
				if ((*groupsJoined)[groupsJoinedLoop]==groupId)
					break;
				}
			if (groupsJoinedLoop==groupsJoined->Count())
				return(EFalse);
			CleanupStack::PopAndDestroy(2,contact);
			}
		CleanupStack::PopAndDestroy(group);
		}
	if (aGroupEntryCount>=0 && groupEntryCount!=aGroupEntryCount)
		return(EFalse);
	CleanupStack::PopAndDestroy(groupIdList);
// Now cross check the other way, 
	const CContactIdArray* contacts=aDb->SortedItemsL();
	for(TInt contactIndex=0;contactIndex<contacts->Count();contactIndex++)
		{
		TContactItemId contactId=(*contacts)[contactIndex];
		CContactItemPlusGroup *item=(CContactItemPlusGroup*)aDb->ReadContactLC(contactId);
		CContactIdArray* groupsJoined=item->GroupsJoinedLC();
		for (TInt groupsJoinedLoop=0;groupsJoinedLoop<groupsJoined->Count();groupsJoinedLoop++)
			{
			CContactGroup *group=(CContactGroup *)aDb->ReadContactLC((*groupsJoined)[groupsJoinedLoop]);
			const CContactIdArray* itemsContained=group->ItemsContained();
			TInt itemsCount=itemsContained->Count();
			TInt itemLoop=0;
			for (;itemLoop<itemsCount;itemLoop++)
				{
				if ((*itemsContained)[itemLoop]==contactId)
					break;
				}
			if (itemLoop==itemsCount)
				return(EFalse);
			CleanupStack::PopAndDestroy(group);
			}
		CleanupStack::PopAndDestroy(2,item);
		}
	return(ETrue);
	}

//
// Misc Util functions
//

EXPORT_C void CleanUpResetAndDestroy(TAny *aArray)
	{
	if (aArray)
		{
		CArrayPtr<CContactItem>* array=(CArrayPtr<CContactItem>*)aArray;
		array->ResetAndDestroy();
		delete array;
		}
	}

EXPORT_C TPtrC FieldText(CContactItem& aItem,TUid aType)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (pos==KErrNotFound || fieldSet[pos].StorageType()!=KStorageTypeText)
		return _L("");
	return fieldSet[pos].TextStorage()->Text();
	}

EXPORT_C void SetNameL(CContactItem& aItem,TUid aFieldType,TUid aMapping, const TDesC& aName, TBool aAddEntry)
//
// Set the contents of a text field, creating the field if required
//
	{
	TInt pos=KErrNotFound;
	if (!aAddEntry)
		{
		CContactItemFieldSet& fieldSet=aItem.CardFields();
		pos=fieldSet.Find(aFieldType);
		if (pos!=KErrNotFound)
			fieldSet[pos].TextStorage()->SetTextL(aName);
		}
	if (pos==KErrNotFound)
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,aFieldType);
		field->SetMapping(aMapping);
		field->TextStorage()->SetTextL(aName);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(); // item
		}
	}

EXPORT_C void SetUserFlags(CContactItemFieldSet& aFieldSet, TUint aFlags, TInt aIndex)
	{
	aFieldSet[aIndex].SetUserFlags(aFlags);
	}

EXPORT_C void SetUserFlags(CContactItemFieldSet& aFieldSet, TUint aFlags)
	{
	SetUserFlags(aFieldSet,aFlags,aFieldSet.Count()-1);
	}

EXPORT_C void SetUserFlags(CContactItem* aItem, TUint aFlags, TInt aIndex)
	{
	SetUserFlags(aItem->CardFields(),aFlags,aIndex);
	}

EXPORT_C void SetUserFlags(CContactItem* aItem, TUint aFlags)
	{
	SetUserFlags(aItem->CardFields(),aFlags);
	}

EXPORT_C void AddFieldL(CContactItemFieldSet* aFieldSet, TStorageType aType,TFieldType aFieldType,TUid aMapping)
	{
	CContactItemField* field=CContactItemField::NewLC(aType,aFieldType);
	field->SetMapping(aMapping);
	aFieldSet->AddL(*field);
	CleanupStack::Pop(); // field
	}

EXPORT_C void AddFieldL(CContactItem* aItem, TStorageType aType,TFieldType aFieldType,TUid aMapping)
	{
	CContactItemField* field=CContactItemField::NewLC(aType,aFieldType);
	field->SetMapping(aMapping);
	aItem->AddFieldL(*field);
	CleanupStack::Pop(); // field
	}

EXPORT_C void InsertFieldL(CContactItem& aItem, TInt aPos, TStorageType aType,TFieldType aFieldType,TUid aMapping)
	{
	CContactItemField* field=CContactItemField::NewLC(aType,aFieldType);
	field->SetMapping(aMapping);
	aItem.InsertFieldL(*field,aPos);
	CleanupStack::Pop(); // field
	}

EXPORT_C void InsertFieldL(CContactItem& aItem, TInt aPos, TStorageType aType,TFieldType aFieldType,TUid aMapping, const TDesC& aText)
	{
	CContactItemField* field=CContactItemField::NewLC(aType,aFieldType);
	field->SetMapping(aMapping);
	aItem.InsertFieldL(*field,aPos);
	field->TextStorage()->SetTextL(aText);
	CleanupStack::Pop(); // field
	}

EXPORT_C TContactItemId AddContactL(CContactDatabase* aDb, TFieldType aFieldType,TUid aMapping, const TDesC &aText)
	{
	CContactCard *item=CContactCard::NewLC();
	SetNameL(*item,aFieldType,aMapping,aText,ETrue);
	TContactItemId id=aDb->AddNewContactL(*item);
	CleanupStack::PopAndDestroy();	// item
	return(id);
	}

EXPORT_C CVCardTestStore* ExportContactsL(CContactDatabase* aDb, const CContactIdArray *aIds, TUint aFormatFlags, TVCardStoreType aType, RFs *aFs, TInt aFileNum)
	{
	CVCardTestStore* store=CVCardTestStore::NewLC(aType,aFs,aFileNum);
	RStoreWriteStream outstream;
	TStreamId streamId=outstream.CreateLC(*store->Store());
	TUid uid;
	uid.iUid=KUidVCardConvDefaultImpl;
	aDb->ExportSelectedContactsL(uid,*aIds,outstream,aFormatFlags);
	outstream.CommitL();
	store->SetStreamId(streamId);
	store->Store()->CommitL();  	
	CleanupStack::PopAndDestroy(); // outstream
	CleanupStack::Pop(); // store
	return(store);
	}

EXPORT_C CArrayPtr<CContactItem>* ImportContactsL(CContactDatabase* aDb, CVCardTestStore* aStore, TUint aFormatFlags)
	{
	RStoreReadStream instream;
	instream.OpenLC(*aStore->Store(),aStore->StreamId());
	TBool success=EFalse;
	CArrayPtr<CContactItem>* items=aDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),instream,success,aFormatFlags);
	CleanupStack::PopAndDestroy(); // instream
	return(items);
	}

EXPORT_C TBool CompareItemField(const CContactItemField &aItemField1, const CContactItemField &aItemField2, TBool aCheckStorage)
	{
	if (aItemField1.StorageType()!=aItemField2.StorageType())
		return(EFalse);
	TInt count=aItemField1.ContentType().FieldTypeCount();
	if (count!=aItemField2.ContentType().FieldTypeCount())
		return(EFalse);
	for(TInt loop=0;loop<count;loop++)
		if (!aItemField2.ContentType().ContainsFieldType(aItemField1.ContentType().FieldType(loop)))
			return(EFalse);
	if (aItemField1.StorageType()!=aItemField2.StorageType())
		return(EFalse);
	if (aItemField1.Label()!=aItemField2.Label())
		return(EFalse);
	if (aItemField1.IsHidden()!=aItemField2.IsHidden())
		return(EFalse);
	if (aItemField1.IsReadOnly()!=aItemField2.IsReadOnly())
		return(EFalse);
	if (aItemField1.IsDisabled()!=aItemField2.IsDisabled())
		return(EFalse);
	if (aItemField1.UserFlags()!=aItemField2.UserFlags())
		return(EFalse);
	if (aCheckStorage)
		{
		switch(aItemField1.StorageType())
			{
		case KStorageTypeText:
			{
			TPtrC f1=aItemField1.TextStorage()->Text();
			TPtrC f2=aItemField2.TextStorage()->Text();
			TInt len=f1.Length();
			if (len==f2.Length()+1 && f1[0]==' ')
				f1.Set(&f1[1],len-1);
			//if (aItemField1.TextStorage()->Text()!=aItemField2.TextStorage()->Text())
			if (f1!=f2)
				return(EFalse);
			break;
			}
		case KStorageTypeStore:
			break;
		case KStorageTypeContactItemId:
			break;
		case KStorageTypeDateTime:
			if (aItemField1.DateTimeStorage()->Time()!=aItemField2.DateTimeStorage()->Time())
				return(EFalse);
			break;
			}
		}
	return(ETrue);
	}

EXPORT_C TBool CompareItemFields(CContactItem *aItem1, CContactItem *aItem2, TBool aCheckStorage, TBool aTemplateLabelRemoved)
	{
	CContactItemFieldSet& fieldSet1 = aItem1->CardFields();
	CContactItemFieldSet& fieldSet2 = aItem2->CardFields();
	TInt count = fieldSet1.Count();
	TInt check = aTemplateLabelRemoved?count-1:count;
	if (check != fieldSet2.Count())
	    {
		return(EFalse);
	    }
	    
	for(TInt loop=0;loop<count;loop++)
		{
		const CContactItemField &field1 = fieldSet1[loop];
		if (aTemplateLabelRemoved && field1.ContentType().ContainsFieldType(KUidContactFieldTemplateLabel))
		    {
		    continue;
		    }
		
		TBool found(EFalse);
		for(TInt loop2 = 0; loop2 < count; loop2++)
		    {
		    found = CompareItemField(field1, fieldSet2[loop2], aCheckStorage);
		    if(found) 
		        {
		        break;
		        }
		    }
		    
	    if(!found)    
	        {
	        return EFalse;
	        }
		}
	return(ETrue);
	}

// This was originally created as the database files were going to reside in
// the SQLite private directory. However, the file is now to be stored in the
// contacts model private directory like the old DBMS file. So this is now a
// bit redundant and perhaps could be removed at some point.
_LIT(KSqlServerPrivateDir, "\\private\\10003A73\\");

EXPORT_C HBufC* GetFullDbName(const TDesC& aDbName)
	{
	TParse parse;
	parse.Set(aDbName, &KSqlServerPrivateDir(), 0);
    return parse.FullName().Alloc();    
	}

//
// CVCardTestStore
//

EXPORT_C CVCardTestStore* CVCardTestStore::NewLC(TVCardStoreType aType, RFs *aFs, TInt aFileNum)
	{
	CVCardTestStore* store=CVCardTestStore::NewL(aType,aFs,aFileNum);
	CleanupStack::PushL(store);
	return(store);
	}

EXPORT_C CVCardTestStore* CVCardTestStore::NewL(TVCardStoreType aType, RFs *aFs, TInt aFileNum)
	{
	CVCardTestStore* store=new(ELeave) CVCardTestStore();
	store->ConstructL(aType,aFs, aFileNum);
	return(store);
	}

EXPORT_C CVCardTestStore::~CVCardTestStore()
	{
	delete iBufStore;
	delete iFileStore;
	}

EXPORT_C void CVCardTestStore::ConstructL(TVCardStoreType aType, RFs *aFs, TInt aFileNum)
	{
	iType=aType;
	switch(iType)
		{
		case KVCardStoreTypeBuf:
			iBufStore=CBufStore::NewL(256);
			break;
		case KVCardStoreTypeFile:
			{
			aFs->MkDirAll(_L("c:\\vcards\\"));
			TFileName fileName;
			fileName.Format(_L("c:\\vcards\\VCard%04d"),aFileNum);
			iFileStore=CDirectFileStore::ReplaceL(*aFs,fileName,EFileWrite);
			iFileStore->SetTypeL(KDirectFileStoreLayoutUid);
			}
			break;
		}
	}

EXPORT_C CStreamStore *CVCardTestStore::Store() const
	{
	CStreamStore *store=NULL;
	switch(iType)
		{
		case KVCardStoreTypeBuf:
			store=iBufStore;
			break;
		case KVCardStoreTypeFile:
			store=iFileStore;
			break;
		}
	return(store);
	}

//////

//
// TCntPerfTimer
//

EXPORT_C TCntPerfTimer::TCntPerfTimer()
	: iTickCount1(0), iTickCount2(0) 
	{
	}

EXPORT_C void TCntPerfTimer::StartTimer()
	{
	iTickCount1 = User::NTickCount();
	}

EXPORT_C void TCntPerfTimer::StopTimer()
	{
	iTickCount2 = User::NTickCount();
	}

EXPORT_C void TCntPerfTimer::ResetTimer()
	{
	iTickCount1 = 0;
	iTickCount2 = 0; 
	}

EXPORT_C TUint TCntPerfTimer::Result()
	{
	TInt tickPeriodMicroSeconds;
	HAL::Get(HAL::ENanoTickPeriod, tickPeriodMicroSeconds);
	TUint result(0);
	if (iTickCount1 > 0 && iTickCount2 > iTickCount1)
		{
 		result = (iTickCount2 - iTickCount1) * tickPeriodMicroSeconds;
		}
	return result;
	}


EXPORT_C TBool DbShouldBeInDamagedState(CContactDatabase* aDb)
	{
	TBool isDamaged(aDb->IsDamaged());
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	return isDamaged;
#else
	return ETrue;
#endif
	}

EXPORT_C CContactItem* ReadContactFromDamagedDbLC(CContactDatabase* aDb, TContactItemId aContactId)
	{
	CContactItem* item(aDb->ReadContactLC(aContactId) );
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	return item;
#else
	User::Leave(KErrNotReady);
	return NULL;
#endif
	}
