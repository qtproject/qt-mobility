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
LOCAL_D RTest test(_L("T_DBASE"));

const TPtrC KDatabaseFileName=_L("C:T_DBASE.cdb");

const TInt KNumTestContacts=20;

LOCAL_C void CreateDatabaseL()
//
// Create a database in a store
//
	{
	CntTest->TestLockServerCloses();
	CntTest->CreateDatabaseL();
	CntTest->TestLockServerExists();
	CntTest->DeleteAllTemplateFieldsL();
	CntTest->CloseDatabase();
	CntTest->TestLockServerCloses();
	}
	
LOCAL_C void OpenDatabaseL()
//
// Open an existing database in a store
//
	{
	CContactDatabase* db=CContactDatabase::OpenL(KDatabaseFileName);
	CntTest->TestLockServerExists();
	delete db;
	CntTest->TestLockServerCloses();
	}

LOCAL_C TPtrC Name(CContactItem& aItem)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(KUidContactFieldFamilyName);
	if (pos==KErrNotFound)
		return _L("");
	return fieldSet[pos].TextStorage()->Text();
	}

LOCAL_C void SetDriveErrMsg()
	{
	test.Printf(_L("Error: For this test to run properly "));
#if defined(__WINS__)
	test.Printf(_L("_EPOC_DRIVE_D must be set up to create a valid drive D:\n"));
#else
	test.Printf(_L("a writable D: must exist\n"));
#endif
	test.Getch();
	}

LOCAL_C void TestDriveSettingL()
	{
	RFs fs;
	test(fs.Connect()==KErrNone);

	TDriveInfo driveInfo;
	if (fs.Drive(driveInfo,EDriveD)!=KErrNone || driveInfo.iType==EMediaNotPresent || driveInfo.iMediaAtt&KMediaAttWriteProtected)
		SetDriveErrMsg();
	else
		{
		TInt err=fs.MkDirAll(_L("D:\\___t___\\"));
		if (err!=KErrNone)
			SetDriveErrMsg();
		else
			{
			fs.RmDir(_L("D:\\___t___\\"));
			TUint att;
			TFileName name;
			CContactDatabase::GetDefaultNameL(name);
			if (fs.Att(name,att)==KErrNotFound)
				{
				CContactDatabase *createDb=CContactDatabase::CreateL();
				delete createDb;
				}
			TDriveUnit driveUnit;
			CContactDatabase::DatabaseDrive(driveUnit);
			if (driveUnit!=TDriveUnit(EDriveC))
				CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC));
		//
		// NEEDS _EPOC_DRIVE_D SET TO POINT TO A VALID DIRECTORY NAME
		//
			CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveD));
			test(CContactDatabase::DatabaseDrive(driveUnit));
			test(driveUnit==TDriveUnit(EDriveD));
		// _EPOC_DRIVE_P Must not exist for the following test
			TRAPD(err,CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveP)));
			test(err!=KErrNone);
			test(CContactDatabase::DatabaseDrive(driveUnit));
			test(driveUnit==TDriveUnit(EDriveD));
		//
			CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC));
			test(CContactDatabase::DatabaseDrive(driveUnit));
			test(driveUnit==TDriveUnit(EDriveC));

			//
			TDriveUnit cColon(EDriveC);
			CContactDatabase::SetDatabaseDriveL(cColon);	// Leave it set to C: for further tests
			test(CContactDatabase::DatabaseDrive(driveUnit));
			test(driveUnit==cColon);
			}
		}
	fs.Close();
	CntTest->TestLockServerCloses();
	}

LOCAL_C void TestDeleteFileL()
	{
	TFileName defaultName;
	CContactDatabase::GetDefaultNameL(defaultName);
	test(CntTest->Fs().SetAtt(defaultName,0,KEntryAttReadOnly)==KErrNone);
	CntTest->TestLockServerCloses();
	CContactDatabase *db=CContactDatabase::ReplaceL();
	delete db;
	CntTest->TestLockServerCloses();
	TUint att;
	test(CntTest->Fs().Att(defaultName,att)==KErrNone);
	CContactDatabase::DeleteDefaultFileL();
	test(CntTest->Fs().Att(defaultName,att)==KErrNotFound);
//
	db=CContactDatabase::CreateL();
	delete db;
	test(CntTest->Fs().Att(defaultName,att)==KErrNone);
	test(CntTest->Fs().SetAtt(defaultName,KEntryAttReadOnly,0)==KErrNone);
	TRAPD(readOnlyErr,CContactDatabase::DeleteDefaultFileL());
	test(readOnlyErr==KErrAccessDenied);
	test(CntTest->Fs().SetAtt(defaultName,0,KEntryAttReadOnly)==KErrNone);
	CContactDatabase::DeleteDefaultFileL();
	}

LOCAL_C void AddNewContactsL(CArrayFix<TContactItemId>* aIdList)
//
// Add KNumTestContacts new contacts with varying numbres of text fields
//
	{
	CntTest->TestLockServerCloses();
	CContactDatabase* db=CntTest->OpenDatabaseL();
	CntTest->TestLockServerExists();
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	TContactItemId id=KNullContactId;
	TContactItemId newId=KNullContactId;
	for (TInt ii=0;ii<KNumTestContacts;ii++)
		{
		TBuf<16> name;
		name.Format(_L("NAME #%d"),ii);
		SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,name,EFalse);
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
		card->AddFieldL(*field);
		CleanupStack::Pop(); // field
		newId=db->AddNewContactL(*card);
		id=newId;
		aIdList->AppendL(id);
		}
	CleanupStack::PopAndDestroy(); // card
	CntTest->CloseDatabase();
	CntTest->TestLockServerCloses();
	}

LOCAL_C void EditContactsL(CArrayFix<TContactItemId>* aIdList)
//
// Check then edit contact names
//
	{
	CContactDatabase* db=CntTest->OpenDatabaseL();
	TInt ii=0;
	for (;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*aIdList)[ii];
		CContactItem* item=db->OpenContactL(index);
		CleanupStack::PushL(item);
		TBuf<16> name;
		name.Format(_L("NAME #%d"),ii);
		test(name==Name(*item));
		name.Format(_L("NEW NAME #%d"),index);
		SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,name,EFalse);
		db->CommitContactL(*item);
		CleanupStack::PopAndDestroy(); // item;
		}
	for (ii=0;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*aIdList)[ii];
		CContactItem* item=db->ReadContactL(index);
		TBuf<16> name;
		name.Format(_L("NEW NAME #%d"),index);
		test(name==Name(*item));
		delete item;
		}
	for (ii=0;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*aIdList)[ii];
		CContactItem* item=db->OpenContactLX(index);
		delete item;
		CleanupStack::PopAndDestroy(); // close
		}

	// try to read a non-existant contact
	TRAPD(err,db->ReadContactL(KNullContactId));
	test(err!=KErrNone);
	// try to edit a contact that hasn't been opened
	CContactItem* item=db->ReadContactLC((*aIdList)[0]);
	TRAP(err,db->CommitContactL(*item));
	CleanupStack::PopAndDestroy(); // item
	test(err==KErrAccessDenied);
	// try to close a non-existant contact
	TRAP(err,db->CloseContactL(KNullContactId));
	test(err==KErrNone); // test changed from 001 (using DBMS 043) - this test makes more sense anyway
	// try to close a contact that hasn't been opened
	TRAP(err,db->CloseContactL((*aIdList)[0]));
	test(err==KErrNone);
	
	CntTest->CloseDatabase();
	}

LOCAL_C void DeleteContactsL(CArrayFix<TContactItemId>* aIdList)
//
// Delete all contacts
//
	{
	CContactDatabase* db=CntTest->OpenDatabaseL();

	// try to delete a contact that is open
	const TContactItemId id=(*aIdList)[0];
	CContactItem* item=db->OpenContactL(id);
	CleanupStack::PushL(item);
	TRAPD(err,db->DeleteContactL(id));

	CleanupStack::PopAndDestroy(); // item
	test(err==KErrInUse);
	db->CloseContactL(id);
	// delete all contacts
	for (TInt ii=0;ii<KNumTestContacts;ii++)
		{
		const TInt index=(*aIdList)[ii];
		db->DeleteContactL(index);
		}
	// try to delete a non-existant contact
	TRAP(err,db->DeleteContactL(KNullContactId));
	test(err!=KErrNone);
	CntTest->CloseDatabase();
	}

void TestErr(TInt aErr)
    {
	if (aErr!=KErrNone)
		{
		test.Printf(_L("Error %d\n"),aErr);
		test(EFalse);
		}
	}
	
/**

@SYMTestCaseID     PIM-T-DBASE-0001

*/


void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-DBASE-0001 T_DBASE"));

	test.Next(_L("Test drive setting"));

	test.Printf(_L("The following tests will delete your default contacts file"));
	test.Printf(_L("Press to 'd' to run the test or any other key to skip them\n"));
	if (test.Getch()=='d')
		{
		test.Next(_L("Set Drive"));

		TRAPD(ret,TestDriveSettingL());
		TestErr(ret);
		test.Next(_L("Delete database file"));

		TRAP(ret,TestDeleteFileL());
		TestErr(ret);
		}
	test.Next(_L("Create new database"));

	TRAPD(ret,CreateDatabaseL());
	TestErr(ret);
	test.Next(_L("Re-open existing database"));

	TRAP(ret,OpenDatabaseL());
	TestErr(ret);
	CArrayFix<TContactItemId>* idList=new(ELeave) CArrayFixFlat<TContactItemId>(5);
	CleanupStack::PushL(idList);
	test.Next(_L("Add new contacts"));

	TRAP(ret,AddNewContactsL(idList));
	TestErr(ret);
	test.Next(_L("Edit contacts"));

	TRAP(ret,EditContactsL(idList));
	TestErr(ret);
	test.Next(_L("Delete contacts"));

	TRAP(ret,DeleteContactsL(idList));
	CleanupStack::PopAndDestroy(); // idList
	TestErr(ret);
	test.Next(_L("Delete empty database"));

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
