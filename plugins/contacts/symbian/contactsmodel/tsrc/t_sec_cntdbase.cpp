/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
 
#include <e32std.h>
#include <f32file.h>
#include <s32file.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <testserver.h>
#include "t_utils.h"

_LIT(KTestName,"T_SEC_CNTDBASE");

#include "testhelpers.h"

LOCAL_C RFs fileServer;
LOCAL_C TDriveList drives;
LOCAL_C const TInt NoDrives = 2;
LOCAL_C TInt driveIdx=0;
LOCAL_C TInt NoDatabases=0;
LOCAL_C TInt NoFileOnPreviousDrive=0;
const TInt KNumTestContacts=20;
LOCAL_C TFileName KNewCntTestFile(_L("Contacts.cbrchar"));


LOCAL_C TDriveUnit driveArray[NoDrives];
LOCAL_C CContactDatabase* db;


_LIT(KNoFiles,"NoFiles");
_LIT(KContactsFilename,"Contacts.cdb");

// These defines and the Local method DrivesOfMediaTypeL should be moved to the 
// Agenda Test Library (agntestlibrary)
_LIT(KMediaNotPresent,	"MediaNotPresent");
_LIT(KMediaUnknown,		"MediaUnknown");
_LIT(KMediaFloppy,		"MediaFloppy");
_LIT(KMediaHardDisk,	"MediaHardDisk");
_LIT(KMediaCdRom,		"MediaCdRom");
_LIT(KMediaRam,			"MediaRam");
_LIT(KMediaFlash,		"MediaFlash");
_LIT(KMediaRom,			"MediaRom");
_LIT(KMediaRemote,		"MediaRemote");
_LIT(KMediaNANDFlash,	"MediaNANDFlash");
TPtrC MediaTypes[] = 
			{ KMediaNotPresent(),KMediaUnknown(),KMediaFloppy(),KMediaHardDisk(),KMediaCdRom(),
			KMediaRam(),KMediaFlash(),KMediaRom(),KMediaRemote(),KMediaNANDFlash()};
_LIT(KCdbSecure,"c:T_Sec_Cntdbase.cdb");

#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
_LIT(KCdbPrivate,"c:\\private\\10003A73\\SQLite__T_Sec_Cntdbase.cdb");
#else
_LIT(KCdbPrivate,"c:\\private\\10003A73\\T_Sec_Cntdbase.cdb");
#endif

_LIT(KCntNameFormat, "NAME #%d");
_LIT(KCntNewNameFormat, "NEW NAME #%d");

LOCAL_C CArrayFix<TInt>* DrivesOfMediaTypeL( TMediaType aMedia, RFs& aFs, TBool aStopAtFirst )
	{
	TDriveInfo info;
	TInt error = KErrNone;
	TDriveList driveList;
	CArrayFix<TInt>* array = new (ELeave) CArrayFixFlat<TInt>(10);
	CleanupStack::PushL( array );
	error = aFs.DriveList(driveList);
	User::LeaveIfError(error);
	for(TInt drive=EDriveA;drive<=EDriveZ;drive++)
		{
		if( driveList[drive] )
			{
			error = aFs.Drive(info, drive);
			if	(error != KErrNone)
				{
				continue;
				}
			if	( info.iType == aMedia )
				{
				array->AppendL( drive );
				if( aStopAtFirst )
					{ // return if only the first drive of the required type.
					break;
					}
				}
			}
		}
	CleanupStack::Pop( array );
	return array;
	}

/**
	MMCDriveL
	
	Determines a list of drives of type EMediaHardDisk.
	The first drive in this list is returned and used
	by the rest of the test.

        NOTE: if this test is running on ARMv5, an MMC card
        must be present otherwise the test fails.

*/
LOCAL_C TDriveUnit MMCDriveL()
	{
	CArrayFix<TInt>* drives = DrivesOfMediaTypeL(EMediaHardDisk,fileServer,EFalse);
	CleanupStack::PushL(drives);
	TInt drivesTInt = drives->Count();
	if( drivesTInt )
		{
		test.Printf(_L("Found the following drives of type %S\n"),&MediaTypes[EMediaHardDisk]);
		for(TInt i=0;i<drivesTInt;i++)
			{
			TDriveUnit drv(drives->At(i));
			TPtrC drvPtr(drv.Name());
			test.Printf(_L("%S\n"),&drvPtr);
			}
		}
	else
		{
		test.Printf( _L("No drives found of type %S\n"),&MediaTypes[EMediaHardDisk]);
		User::LeaveIfError(KErrHardwareNotAvailable);
		}
	TDriveUnit mmcDrive(drives->At(0));
	CleanupStack::PopAndDestroy(drives);
	// Use the first drive in the list for the rest of the test.
	return mmcDrive;
	}

/**
	AddNewContactsL
	Add KNumTestContacts new contacts with varying numbres of text fields

	@param aIdList
*/
LOCAL_C void AddNewContactsL(CArrayFix<TContactItemId>* aIdList)
	{
	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	TContactItemId id=KNullContactId;
	TContactItemId newId=KNullContactId;
	for (TInt ii=0;ii<KNumTestContacts;ii++)
		{
		TBuf<16> name;
		name.Format(KCntNameFormat, ii);
		SetNameL(*card,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,name,EFalse);
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
		card->AddFieldL(*field);
		CleanupStack::Pop(); // field
		newId=db->AddNewContactL(*card);
		id=newId;
		aIdList->AppendL(id);
		}
	CleanupStack::PopAndDestroy(); // card
	}

LOCAL_C TPtrC Name(CContactItem& aItem)
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(KUidContactFieldFamilyName);
	if (pos==KErrNotFound)
		return _L("");
	return fieldSet[pos].TextStorage()->Text();
	}

/**
	EditContactsL
	Check then edit contact names

	@param aIdList
*/
LOCAL_C void EditContactsL(CArrayFix<TContactItemId>* aIdList)
	{
	TInt ii=0;
	test.Next(_L("Change all contact names"));
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
	for (;ii<aIdList->Count();++ii)
#else		
	for (;ii<KNumTestContacts;ii++)
#endif
		{
		const TInt index=(*aIdList)[ii];
		// exercise OpenContactLX api, Commit the change and Pop the lock record
		CContactItem* item=db->OpenContactLX(index);
		CleanupStack::PushL(item);
		TBuf<16> name;
		name.Format(KCntNameFormat, ii); // "NAME %d"
		test(name==Name(*item));
		name.Format(KCntNewNameFormat, index);	// "NEW NAME %d"
		SetNameL(*item,KUidContactFieldFamilyName,KUidContactFieldVCardMapUnusedN,name,EFalse);
		db->CommitContactL(*item);
		CleanupStack::PopAndDestroy(); // item;
		CleanupStack::Pop(); // lock record
		}
	test.Next(_L("Check new contact names"));
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
	for (;ii<aIdList->Count();++ii)
#else
	for (ii=0;ii<KNumTestContacts;ii++)
#endif
		{
		const TInt index=(*aIdList)[ii];
		CContactItem* item=db->ReadContactL(index);
		TBuf<16> name;
		name.Format(KCntNewNameFormat, index);
		test(name==Name(*item));
		delete item;
		}
	test.Next(_L("Regression test for Contact locking/unlocking (DEF067162)"));
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
	for (;ii<aIdList->Count();++ii)
#else	
	for (ii = 0; ii < KNumTestContacts; ii++)
#endif
		{
		// open 2 different items
		const TInt index1 = (*aIdList)[ii];
		const TInt index2 = (*aIdList)[(ii + 1) % KNumTestContacts];
		CContactItem* item1 = db->OpenContactLX(index1);
		CleanupStack::PushL(item1);
		CContactItem* item2 = db->OpenContactLX(index2);
		delete item2;
		CleanupStack::PopAndDestroy(); // destroy lock = close item2
		CleanupStack::PopAndDestroy(item1);
		CleanupStack::PopAndDestroy(); // destroy lock = close item1

		// re-open 1st item - should not be locked
		// OpenContactLX() and PopAndDestroy()
		item1 = db->OpenContactLX(index1);
		delete item1;
		CleanupStack::PopAndDestroy(); // destroy lock = close item1

		// re-open 2nd item - should not be locked
		// OpenContactL() and CloseContactL()
		item2 = db->OpenContactL(index2);
		CleanupStack::PushL(item2);
		db->CloseContactL(index2);				// explicitly close item 2
		CleanupStack::PopAndDestroy(item2);
		}


	// try to read a non-existant contact
	TRAPD(err,db->ReadContactL(KNullContactId));
	TESTTRUEL(err!=KErrNone);
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
	if(aIdList->Count() > 0)
		{
#endif		
	// try to edit a contact that hasn't been opened
	CContactItem* item=db->ReadContactLC((*aIdList)[0]);
	TRAP(err,db->CommitContactL(*item));
	CleanupStack::PopAndDestroy(); // item
	TESTTRUEL(err==KErrAccessDenied);
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
		}
#endif			
	// try to close a non-existant contact
	TRAP(err,db->CloseContactL(KNullContactId));
	TESTTRUEL(err==KErrNone); // test changed from 001 (using DBMS 043) - this test makes more sense anyway
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
	if(aIdList->Count() > 0)
		{
#endif			
	// try to close a contact that hasn't been opened
	TRAP(err,db->CloseContactL((*aIdList)[0]));
	TESTTRUEL(err==KErrNone);
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
		}
#endif			
	}

/**
	Gets the list of installed drives and then stores the first 2
	in the array drives for use within the test. The second entry
	in this array should use method MMCDriveL as used in T_Sec_Tidy
	and T_Sec_ListAgendaFile.
*/
LOCAL_C void DrivesL()
	{
	driveArray[0] = EDriveC;
	driveArray[1] = MMCDriveL();
	
	// Tell the user which drive has been chosen. 
	TPtrC drv( driveArray[1].Name() );
	test.Printf(_L("%S chosen for the test.\n"),&drv);
	}

/**
	Ensure that the filename conforms to the correct form.
	The analysis functionality is dependent on the flags:
	SYMBIAN_SECURE_DBMS. Unlike Agenda, in contacts as GetDefaultNameL
	existed before platsec there is legacy functionality that must
	maintained. In not SYMBIAN_SECURE_DBMS the full path must be checked
	for. The default filename exits on the current drive.
	@param reference to the default file name.
	@return ETrue if the default filename is correctly formed.
*/
LOCAL_C TBool CheckDefaultContactsFilename( TFileName& aFilename )
	{
	_LIT(KDriveFailure, "Incorrect Drive Specified" );
	_LIT(KPathFailure, "Incorrect No Path expected" );
	_LIT(KFilenameFailure, "Incorrect Filename" );

	TDriveUnit lDriveUnit;

	// Get the current drive.
	CContactDatabase::DatabaseDrive(lDriveUnit);

	TParse filename;
	filename.Set( aFilename, NULL, NULL);
	
	// Check that the drive letter exists. No drive is an error 
	// in both modes.
	if( filename.Drive() != lDriveUnit.Name())
		{
		test.Printf(KDriveFailure);
		return EFalse;
		}
	if( filename.Path().Length())
		{
		test.Printf(KPathFailure);
		return EFalse;
		}
	if( filename.NameAndExt().CompareF(KContactsFilename()))
		{
		test.Printf(KFilenameFailure);
		return EFalse;
		}
	return ETrue;
	}

/** 
	CheckNotDefaultContactsFiles
	
	Uses all the default database related API's to ensure that
	they all return negative, now that the default db has been
	removed.
	@return ETrue if a default database cannot be found on the
	default drive.
*/
LOCAL_C TBool CheckNotDefaultContactsFiles()
	{
	// The default database on the current default drive.(3)
	// The default database on the specific drive. (4)
	TBool response = EFalse;
	TRAPD(err, response = CContactDatabase::DefaultContactDatabaseExistsL());
	if( !(err == KErrNone && !response ))
		{
		return EFalse;
		}
	
	// Use the default database, as we have confirmed previously
	// this shouldn't exits. This is dependent upon the Security mode.
	TFileName filename;
	filename.Copy(driveArray[driveIdx].Name());
	filename.Append(KContactsFilename);
	// (5)
	TRAP(err, response = CContactDatabase::ContactDatabaseExistsL(filename));
	if( !(err == KErrNone && !response ))
		{
		return EFalse;
		}
	
	// None of the API return true, no default Contact DB files were detected. 
	return ETrue;
	}


/**
	CheckCntFilesL
	
	Ensure that cntFiles contains the list of correct files.
	The analysis functionality is dependent on the flags:
	_SECURE_DATA_ and _UNSECURE_DATA_
	Determines the list of agn files, checks for the existance of aFilename
	within the list of aNum names.

	@param aNum number of expected files. 
	@param aFilename a reference to an expected filename.
	@return aFileExpectedToExist ETrue if the list contains the expected files.
*/
LOCAL_C TBool CheckCntFilesL( TInt aNum,  const TDesC& aFilename , TBool aFileExpectedToExist = ETrue)
	{
	// Data bases on all drives.
	CDesCArray* cntFiles = CContactDatabase::ListDatabasesL();
	CleanupStack::PushL(cntFiles);
	// Data bases on the specified drive.
	CDesCArray* cntFilesFromDrive = CContactDatabase::ListDatabasesL(driveArray[driveIdx]);
	CleanupStack::PushL(cntFilesFromDrive);
	
	TInt DriveZCount=0;
	// Determine files on drive Z:
	
	if( !cntFiles->Count() && !cntFilesFromDrive->Count() && !aNum )
		{
		CleanupStack::PopAndDestroy( cntFilesFromDrive );
		CleanupStack::PopAndDestroy( cntFiles );								
		return !aFileExpectedToExist;
		}
	// Ensure that all drive contents add up.
	if( cntFiles->Count() != NoFileOnPreviousDrive + DriveZCount + cntFilesFromDrive->Count() )
		{
		CleanupStack::PopAndDestroy(cntFilesFromDrive);
		CleanupStack::PopAndDestroy(cntFiles);
		return !aFileExpectedToExist;
		}
	TFileName filename;
	// In un-secure mode if aFilename contains no path, then
	// a backslash must be included between the drive and the
	// filename. This because when asked to create filename 
	// "c:filename" ListDatabasesL returns the name "c:\filename"
	// hence a backslash is inserted for comparison purposes.
	TParse file;
	file.Set(aFilename,NULL,NULL);

	filename.Copy(aFilename);	

	// Check that aFilename is in the list.
	TBool retValue = ETrue;
	if (!aFileExpectedToExist)
		{
		// File is not expected to exist. So return EFalse if we find it.
		for(TInt i=0;i<cntFilesFromDrive->Count();i++)
			{
			TFileName driveList_1 = (*cntFilesFromDrive)[i];
			if(!driveList_1.CompareF(filename))
				{
				CleanupStack::PopAndDestroy(cntFilesFromDrive);
				CleanupStack::PopAndDestroy(cntFiles);
				return !retValue;
				}
			}
		retValue = EFalse;
		}
	else
		{
		// File is expected to exist. We should find it in both lists.
		// return EFalse if not found in this list.
		TBool fileFound = EFalse;
		// does file exist in list from drive
		for(TInt j=0;j<cntFilesFromDrive->Count();j++)
			{
			TFileName driveList_2 = (*cntFilesFromDrive)[j];
			if(!driveList_2.CompareF(filename))
				{
				fileFound = ETrue;
				break;
				}
			}

		if(!fileFound)
			{
			CleanupStack::PopAndDestroy(cntFilesFromDrive);
			CleanupStack::PopAndDestroy(cntFiles);
			return !retValue;
			}
		}

	// We found the file in cntFilesFromDrive 
	// Does file exist in list from all drives
	for(TInt k=0;k<cntFiles->Count();k++)
		{
		TFileName driveList_3 = (*cntFiles)[k];
		if(!driveList_3.CompareF(filename))
			{
			CleanupStack::PopAndDestroy(cntFilesFromDrive);
			CleanupStack::PopAndDestroy(cntFiles);
			return retValue;
			}
		}	
	CleanupStack::PopAndDestroy(cntFilesFromDrive);
	CleanupStack::PopAndDestroy(cntFiles);
	// File was not found so return false.
	return !retValue;
	}

/**
	This API uses all the Contacts API's relating to identification
	of the default file.
*/
LOCAL_C TBool DefaultDatabaseL()
	{
		// The default database on the current default drive.
	if(!CContactDatabase::DefaultContactDatabaseExistsL())
		return EFalse;

	// Use the default database, as we have confirmed previously
	// this shouldn't exits. This is dependent upon the Security mode.
	TFileName filename;
	filename.Copy(driveArray[driveIdx].Name());
	filename.Append(KContactsFilename);
	if(!CContactDatabase::ContactDatabaseExistsL(filename))
		return EFalse;
	
	// None of the API return true, no files were detected.
	return ETrue;

	}
	
/**
	This function uses Five API's available to determine the existance
	of database files. These are (1)ListDatabasesL(),(2)ListDatabasesL(TDriveUnit&)
	(3)DefaultContactDatabaseExistsL(),(4)DefaultContactDatabaseExistsL(TDriveUnit&)
	and (5)ContactDatabaseExistsL(const TDesC&). All five should confirm that there
	are no contacts database files.
*/
LOCAL_C TBool CheckNoDatabaseFilesL()
	{

	TFileName filename;
	TBool retVal = EFalse;
	filename.Copy(driveArray[driveIdx].Name());

	filename.Append(KContactsFilename);

	retVal = CheckCntFilesL(NoDatabases,  filename , EFalse);

	if (retVal == EFalse)
		return retVal;
	else
		return CheckNotDefaultContactsFiles();
	}

/**
	Tidy
	
	This function removes any files that may exist as a result of a
	previous run of the T_Sec_cntdbase test.
	The configuration of this operation is dependent upon __SECURE_DATA_
*/

LOCAL_C void Tidy()
	{
	TFileName addedFile;
	CDesCArray* driveFiles = NULL;
	for(TInt i =0;i<NoDrives; i++ )
		{
		addedFile.Copy(driveArray[i].Name());
		addedFile.Append(KNewCntTestFile);
		TRAPD( err, CContactDatabase::SetDatabaseDriveL(driveArray[i], EFalse));
		TRAP( err, CContactDatabase::DeleteDefaultFileL());
		TRAP(err, CContactDatabase::DeleteDatabaseL(addedFile));
		}
	
	TRAPD(ret, driveFiles = CContactDatabase::ListDatabasesL());
	for(TInt j =0;j<driveFiles->Count();j++)
		{
		CContactDatabase::DeleteDatabaseL((*driveFiles)[j]);
		}	
	if( ret == KErrNone )
		{
		delete driveFiles;
		}
	}

// support CreateL and ReplaceL OOM tests
LOCAL_C TInt CheckDatabaseIsAbsentOrOpenable(TBool aDeleteDatabase=EFalse)
	{
	// CreateL or ReplaceL failed, so database can either be absent or it must Open
	TBool exists(EFalse);
	
	TRAPD(err, exists = CContactDatabase::DefaultContactDatabaseExistsL());

	if (err)
		{
		test.Printf(_L("CheckDatabaseIsAbsentOrOpenable - DefaultContactDatabaseExistsL error %d"), err);
		}
	
	if(exists)
		{
		CContactDatabase* dbaseOpen = NULL;
				
		TRAP(err, dbaseOpen = CContactDatabase::OpenL());
		
		if (err)
			{
			test.Printf(_L("CheckDatabaseIsAbsentOrOpenable - OpenL error %d"), err);
			}
	
		if (dbaseOpen != NULL)
			{
			// database exists, so it should have opened
			delete dbaseOpen;
			}
			
		if (aDeleteDatabase && (err == KErrNone))
			{
			// for client side CreateL OOM test
			CContactDatabase::DeleteDefaultFileL();
			}
		}

	return err;
	}


//////////////////////////// Client Side OOM Tests ////////////////////////////	
LOCAL_C TBool ReplaceLOOM_ClientSideL()
	{
	test.Next(_L("ReplaceL client side OOM test.\n"));

	CContactDatabase* dBase = NULL;
	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(ret, dBase = CContactDatabase::ReplaceL());
		if( ret == KErrNone )
			{
			delete dBase;
			}
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if (ret != KErrNone)
			{
			if(ret != KErrNoMemory)
				{
				test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
				TESTTRUEL(ret == KErrNoMemory);
				}

			TESTNOERRL(CheckDatabaseIsAbsentOrOpenable());
			}
		}

	// Remove the default database, created by Replace.
	CContactDatabase::DeleteDefaultFileL();
	test.Printf(_L("\n"));
	return ETrue;
	}

	
LOCAL_C TBool ContactDatabaseExistsLOOM_ClientSideL()
	{
	test.Next(_L("ContactDatabaseExistsL client side OOM test.\n"));
	_LIT(KContactDBase,"c:ContactDBase");

	TBool dExists = EFalse;
	// First create a database to be detected.
	CContactDatabase* dBase = NULL;
	TRAPD(err, dBase = CContactDatabase::CreateL(KContactDBase));

	// need the file to be present to test if it exists
	TESTTRUEL((err == KErrNone) || (err == KErrAlreadyExists));
	if (err == KErrNone)
		{
		delete dBase;
		}

	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(ret, dExists = CContactDatabase::ContactDatabaseExistsL(KContactDBase));
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if	(ret != KErrNoMemory && ret != KErrNone)
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}	
		}
	if( ret == KErrNone && dExists )
		{
		// Remove the database.
		CContactDatabase::DeleteDatabaseL(KContactDBase);
		}
	test.Printf(_L("\n"));
	return ETrue;
	}

LOCAL_C TBool DefaultContactDatabaseExistsLOOM_ClientSideL()
	{
	test.Next(_L("DefaultContactDatabaseExistsL client side OOM test.\n"));

	TBool dExists = EFalse;
	// First create a default database.
	CContactDatabase* dBase = NULL;
	TRAPD( err, dBase = CContactDatabase::CreateL());
	// need the file to be present to test if it exists
	TESTTRUEL((err == KErrNone) || (err == KErrAlreadyExists));
	if (err == KErrNone)
		{
		delete dBase;
		}

	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(ret, dExists = CContactDatabase::DefaultContactDatabaseExistsL());
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if	(ret != KErrNoMemory && ret != KErrNone)
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}
		}
	if( ret == KErrNone && dExists )
		{
		// Remove the default database
		CContactDatabase::DeleteDefaultFileL();
		}
	test.Printf(_L("\n"));
	return ETrue;
	}

LOCAL_C TBool GetDefaultNameLOOM_ClientSideL()
	{
	test.Next(_L("GetDefaultNameL client side OOM test.\n"));
	TFileName filename;

	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(ret, CContactDatabase::GetDefaultNameL(filename));
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if	(ret != KErrNoMemory && ret != KErrNone)
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}		
		}
	test.Printf(_L("\n"));
	return ETrue;
	}
	
LOCAL_C TBool DeleteDatabaseLOOM_ClientSideL()
	{
	test.Next(_L("DeleteDatabaseL client side OOM test.\n"));
	_LIT(KTestFile,"C:TestFile");
	CContactDatabase* dBase = NULL;
	// Create a file to delete.
	TRAPD(err, dBase = CContactDatabase::CreateL(KTestFile));
	// need the file to be present to delete it
	TESTTRUEL((err == KErrNone) || (err == KErrAlreadyExists));
	if (err == KErrNone)
		{
		delete dBase;
		}

	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(ret, CContactDatabase::DeleteDatabaseL(KTestFile));
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if	(ret != KErrNoMemory && ret != KErrNone)
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}		
		}
	test.Printf(_L("\n"));
	// Check to see that database has been removed.
	TBool dExists = EFalse;
	TRAP(ret, dExists = CContactDatabase::ContactDatabaseExistsL(KTestFile));
	TESTNOERRL(ret);

	return !dExists;
	}

LOCAL_C TBool DeleteDefaultFileLOOM_ClientSideL()
	{
	test.Next(_L("DeleteDefaultFileL client side OOM test.\n"));

	CContactDatabase* dBase = NULL;
	TRAPD( err, dBase = CContactDatabase::CreateL());
	// need the file to be present to delete it
	TESTTRUEL((err == KErrNone) || (err == KErrAlreadyExists));
	if (err == KErrNone)
		{
		delete dBase;
		}

	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(ret, CContactDatabase::DeleteDefaultFileL());
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if	(ret != KErrNoMemory && ret != KErrNone)
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}
		}
	test.Printf(_L("\n"));
	// Ensure that the default File has been removed.
	TBool dExists = EFalse;
	TRAP(ret, dExists = CContactDatabase::DefaultContactDatabaseExistsL());
	TESTNOERRL(ret);

	return !dExists;
	}
	
LOCAL_C TBool CreateLOOM_ClientSideL()
	{
	test.Next(_L("CreateL client side OOM test.\n"));

	TRAPD(err, CContactDatabase::DeleteDefaultFileL());
	TESTTRUEL((err == KErrNone) || (err == KErrNotFound));
	
	CContactDatabase* dBase = NULL;
	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
			
		TRAP(ret, dBase = CContactDatabase::CreateL());
		if( ret == KErrNone ) 
			{
			delete dBase;
			dBase=NULL;
			}
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		if (ret != KErrNone)
			{
			if(ret != KErrNoMemory)
				{
				test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
				TESTTRUEL(ret == KErrNoMemory);
				}

			TESTNOERRL(CheckDatabaseIsAbsentOrOpenable(ETrue));
			}
		}

	//cleanup
	if(CContactDatabase::DefaultContactDatabaseExistsL())
		{
		CContactDatabase::DeleteDefaultFileL();
		}

	test.Printf(_L("\n"));
	return ETrue;
	}

LOCAL_C TBool OpenLOOM_ClientSideL()
	{
	test.Next(_L("OpenL client side OOM test.\n"));

	// Create a default database.
	CContactDatabase* dBase = NULL;
	TRAPD( err, dBase = CContactDatabase::CreateL());
	// need the file to be present to open it
	TESTTRUEL((err == KErrNone) || (err == KErrAlreadyExists));
	if (err == KErrNone)
		{
		delete dBase;
		}		
	
	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(ret, dBase = CContactDatabase::OpenL());
		if( ret == KErrNone ) 
			{
			delete dBase;
			}
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if	(ret != KErrNoMemory && ret != KErrNone)
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}
			
		}
	test.Printf(_L("\n"));
	// Remove the default file we just opened.
	TRAP( ret, CContactDatabase::DeleteDefaultFileL());
	
	return ETrue;
	}

LOCAL_C TBool ListDatabasesLOOM_ClientSideL()
	{
	test.Next(_L("ListDatabasesL client side OOM test.\n"));

	CDesCArray* driveFiles = NULL;
	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone )
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;
		TRAP(ret, driveFiles = CContactDatabase::ListDatabasesL());
		if( ret == KErrNone )
			{
			delete driveFiles;
			}
		__UHEAP_MARKEND;
		__UHEAP_RESET;

		if	(ret != KErrNoMemory && ret != KErrNone)
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}
		}
	test.Printf(_L("\n"));
	return ETrue;
	}

#ifdef _DEBUG
LOCAL_C TBool CreateLOOM_ServerSideL()
	{
	test.Next(_L("CreateL Server side OOM test.\n"));

	_LIT(KTemporaryFile,"C:TempFile");

	CContactDatabase* dBase = NULL;
	CContactDatabase* newDataBase = NULL;

	// First create an instance of CContactDatabase to be used to call
	// Server side heap methods.
	TRAPD( err, dBase = CContactDatabase::ReplaceL(KTemporaryFile));
	// need the file to be present for server connection
	TESTTRUEL((err == KErrNone) && (dBase != NULL));
	CleanupStack::PushL(dBase);

	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());

	TInt allocCells(0);	
 	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	
	while(ret != KErrNone)
		{
		failAt++;

		if( !(failAt % 5 ))
			{
			test.Printf(_L("%d,"), failAt);
			}

		dBase->SetCntServerHeapFailure(RHeap::EDeterministic, failAt);
		allocCells = dBase->CntServerResourceCount();    //__UHEAP_MARK;
		
		TRAP(ret, newDataBase = CContactDatabase::CreateL());
		if( ret == KErrNone )
			{
			delete newDataBase;
			}
			
		dBase->SetCntServerHeapFailure(RHeap::ENone, 1);

		if (ret != KErrNone)
			{
			if	(ret == KErrNoMemory)
				{
				TInt allocLatest = dBase->CntServerResourceCount();
				if (allocLatest != allocCells)
					{
				#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__	
					if(allocLatest != allocCells -1)
				#endif	
						{		
						test.Printf(_L("iteraton %d failed - initial alloc cells = %d, alloc Latest = %d\n"), 
							failAt, allocCells, allocLatest);
						TESTVALUEL(allocLatest, allocCells); //__UHEAP_MARKEND;											
						}
					}
				}
			else
				{
				test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
				TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
				}
			
			TESTNOERRL(CheckDatabaseIsAbsentOrOpenable(ETrue));
			}
		}

	CleanupStack::PopAndDestroy(dBase);
	// This test has created two files; a default and KTemporaryFile
	// both must be deleted.
	TRAP( ret, CContactDatabase::DeleteDefaultFileL());
	TRAP( ret, CContactDatabase::DeleteDatabaseL( KTemporaryFile ));

	return ETrue;
	}

LOCAL_C TBool DeleteDatabaseLOOM_ServerSideL()
	{
	test.Next(_L("DeleteDatabaseL Server side OOM test.\n"));

	_LIT(KFileToDelete,"c:FileToDelete");
	CContactDatabase* dBase = NULL;
	CContactDatabase* dBaseForFile = NULL;

	//
	// create db file for delete test
	TRAPD( err, dBaseForFile = CContactDatabase::CreateL(KFileToDelete));
	TESTTRUEL((err == KErrNone) || (err == KErrAlreadyExists));
	if (err == KErrNone)
		{
		// Close the file
		delete dBaseForFile;
		}

	// Create Default dBase to be used by ServerSide OOM Calls.
	TRAP( err, dBase = CContactDatabase::ReplaceL());
	// need the file to be present for server connection
	TESTTRUEL(err == KErrNone);
	CleanupStack::PushL(dBase);


	TInt allocCells(0);
 	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone)
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}
		dBase->SetCntServerHeapFailure(RHeap::EDeterministic, failAt);
		allocCells = dBase->CntServerResourceCount();    //__UHEAP_MARK;

		TRAP(ret, CContactDatabase::DeleteDatabaseL(KFileToDelete));

		//__SRVHEAP_RESET;
		dBase->SetCntServerHeapFailure(RHeap::ENone, 1);

		if	(ret == KErrNoMemory)
			{
			TInt allocLatest = dBase->CntServerResourceCount();

			if (allocLatest != allocCells)
				{
				test.Printf(_L("iteraton %d failed - initial alloc cells = %d, alloc Latest = %d\n"), 
					failAt, allocCells, allocLatest);
				TESTVALUEL(allocLatest, allocCells); //__UHEAP_MARKEND;
				}
			}
		else if ( ret != KErrNone )
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}
		}
	CleanupStack::PopAndDestroy(dBase);
	// Delete default file.
	TRAP(ret, CContactDatabase::DeleteDefaultFileL());
	
	return ETrue;
	}

LOCAL_C TBool DefaultContactDatabaseExistsLOOM_ServerSideL()
	{
	test.Next(_L("DefaultContactDatabaseExistsL Server side OOM test.\n"));

	CContactDatabase* dBase = NULL;

	TRAPD( err, dBase = CContactDatabase::ReplaceL());
	// need the file to be present for server connection
	TESTTRUEL((err == KErrNone) && (dBase != NULL));
	CleanupStack::PushL(dBase);

	TInt allocCells(0);
 	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone)
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}
		dBase->SetCntServerHeapFailure(RHeap::EDeterministic, failAt);
		allocCells = dBase->CntServerResourceCount();    //__UHEAP_MARK;

		TRAP(ret, CContactDatabase::DefaultContactDatabaseExistsL());

		//__SRVHEAP_RESET;
		dBase->SetCntServerHeapFailure(RHeap::ENone, 1);

		if	(ret == KErrNoMemory)
			{
			TInt allocLatest = dBase->CntServerResourceCount();

			if (allocLatest != allocCells)
				{
				test.Printf(_L("iteraton %d failed - initial alloc cells = %d, alloc Latest = %d\n"), 
					failAt, allocCells, allocLatest);
				TESTVALUEL(allocLatest, allocCells); //__UHEAP_MARKEND; 
				}

			}
		else if ( ret != KErrNone )
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone)); //Bad leaving code
			}
		}
	CleanupStack::PopAndDestroy(dBase);
	// Check to see that database has been removed.
	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());

	return ETrue;
	}
	

LOCAL_C TBool OpenLOOM_ServerSideL()
	{
	test.Next(_L("OpenL Server side OOM test.\n"));

	CContactDatabase* dBase = NULL;
	CContactDatabase* dBaseToOpen = NULL;

	// Create the default contacts file.
	TRAPD(err, dBase = CContactDatabase::ReplaceL());
	// need the file to be present for server connection
	TESTTRUEL((err == KErrNone) && (dBase != NULL));
	CleanupStack::PushL(dBase);

	TInt allocCells(0);
 	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone)
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}
		dBase->SetCntServerHeapFailure(RHeap::EDeterministic, failAt);
		allocCells = dBase->CntServerResourceCount();    //__UHEAP_MARK;

		TRAP(ret, dBaseToOpen = CContactDatabase::OpenL());
		if( ret == KErrNone )
			{
			delete dBaseToOpen;
			}

		//__SRVHEAP_RESET;
		dBase->SetCntServerHeapFailure(RHeap::ENone, 1);

		if	(ret == KErrNoMemory)
			{
			TInt allocLatest = dBase->CntServerResourceCount();

			if (allocLatest != allocCells)
				{
			#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__	
				if(allocLatest != allocCells + 1)
			#endif	
					{
					test.Printf(_L("iteraton %d failed - initial alloc cells = %d, alloc Latest = %d\n"), 
							failAt, allocCells, allocLatest);
					TESTVALUEL(allocLatest, allocCells); //__UHEAP_MARKEND;	
					}
				}
			}
		else if ( ret != KErrNone )
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}
		
		}
	CleanupStack::PopAndDestroy(dBase);
	// Remove the default.
	TRAP(ret, CContactDatabase::DeleteDefaultFileL());
	
	return ETrue;
	}

LOCAL_C TBool ContactDatabaseExistsLOOM_ServerSideL()
	{
	test.Next(_L("ContactDatabaseExistsL Server side OOM test.\n"));

	_LIT(KCntFileToExist,"C:FileToDetect");

	CContactDatabase* dBase = NULL;
	// Create the named contacts file.
	TRAPD(err, dBase = CContactDatabase::ReplaceL(KCntFileToExist));
	// need the file to be present for server connection
	TESTTRUEL((err == KErrNone) && (dBase != NULL));
	CleanupStack::PushL(dBase);

	TInt allocCells(0);
 	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone)
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}
		dBase->SetCntServerHeapFailure(RHeap::EDeterministic, failAt);
		allocCells = dBase->CntServerResourceCount();    //__UHEAP_MARK;
		TRAP(ret, CContactDatabase::ContactDatabaseExistsL(KCntFileToExist));

		//__SRVHEAP_RESET;
		dBase->SetCntServerHeapFailure(RHeap::ENone, 1);

		if	(ret == KErrNoMemory)
			{
			TInt allocLatest = dBase->CntServerResourceCount();

			if (allocLatest != allocCells)
				{
				test.Printf(_L("iteraton %d failed - initial alloc cells = %d, alloc Latest = %d\n"), 
					failAt, allocCells, allocLatest);
				TESTVALUEL(allocLatest, allocCells); //__UHEAP_MARKEND; 
				}
			}
		else if ( ret != KErrNone )
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));//Bad leaving code
			}
		}
	CleanupStack::PopAndDestroy(dBase);
	// Remove the default.
	TRAP(ret, CContactDatabase::DeleteDatabaseL(KCntFileToExist));

	return ETrue;
	}

LOCAL_C TBool ReplaceLOOM_ServerSideL()
	{
	test.Next(_L("ReplaceL Server side OOM test.\n"));
	_LIT(KCntFileToExist,"C:FileToDetect");
	CContactDatabase* dBase = NULL;
	CContactDatabase* dBaseReplace = NULL;

	// Create the named contacts file.
	TRAPD(err, dBase = CContactDatabase::ReplaceL(KCntFileToExist));
	// need the file to be present for server connection
	TESTTRUEL((err == KErrNone) && (dBase != NULL));
	CleanupStack::PushL(dBase);

	TInt allocCells(0);
 	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone)
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}
		dBase->SetCntServerHeapFailure(RHeap::EDeterministic, failAt);
		allocCells = dBase->CntServerResourceCount();    //__UHEAP_MARK;

		TRAP(ret, dBaseReplace = CContactDatabase::ReplaceL());
		if(ret == KErrNone)
			{
			delete dBaseReplace;
			}

		//__SRVHEAP_RESET;
		dBase->SetCntServerHeapFailure(RHeap::ENone, 1);

		if (ret != KErrNone)
			{
			if(ret != KErrNoMemory)
				{
				test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
				TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
				}
			else
				{
				TInt allocLatest = dBase->CntServerResourceCount();

				if (allocLatest != allocCells)
					{
				#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__	
					if(allocLatest != allocCells - 1)
				#endif	
						{
						test.Printf(_L("iteraton %d failed - initial alloc cells = %d, alloc Latest = %d\n"), 
									failAt, allocCells, allocLatest);
						TESTVALUEL(allocLatest, allocCells); //__UHEAP_MARKEND; 							
						}
					}
				}

			TESTNOERRL(CheckDatabaseIsAbsentOrOpenable());
			}


		}
	CleanupStack::PopAndDestroy(dBase);

	// Remove the default file associated with the dBase.
	TRAP(ret, CContactDatabase::DeleteDatabaseL(KCntFileToExist));

	return ETrue;
	}
	
LOCAL_C TBool DeleteDefaultFileLOOM_ServerSideL()
	{
	test.Next(_L("DeleteDefaultFileL Server side OOM test.\n"));
	_LIT(KCntFileToExist,"C:FileToDetect");

	CContactDatabase* dBase = NULL;
	CContactDatabase* dBaseDefaultFile = NULL;
	// ensure default contacts file is created
	TRAPD(err, dBaseDefaultFile = CContactDatabase::CreateL());
	// need file to be created for delete test
	TESTTRUEL( (err == KErrNone) || (err == KErrAlreadyExists))
	if (err == KErrNone)
		{
		delete dBaseDefaultFile;
		}

	// Create the named contacts file.
	TRAP(err, dBase = CContactDatabase::ReplaceL(KCntFileToExist));
	// need the file to be present for server connection
	TESTTRUEL(err == KErrNone);
	CleanupStack::PushL(dBase);

	TInt allocCells(0);
 	TInt ret = KErrNoMemory;
	TInt failAt = 0;
	while(ret != KErrNone)
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}
		dBase->SetCntServerHeapFailure(RHeap::EDeterministic, failAt);
		allocCells = dBase->CntServerResourceCount();    //__UHEAP_MARK;

		TRAP(ret, CContactDatabase::DeleteDefaultFileL());

		//__SRVHEAP_RESET;
		dBase->SetCntServerHeapFailure(RHeap::ENone, 1);			

		if	(ret == KErrNoMemory)
			{
			TInt allocLatest = dBase->CntServerResourceCount();

			if (allocLatest != allocCells)
				{
				test.Printf(_L("iteraton %d failed - initial alloc cells = %d, alloc Latest = %d\n"), 
					failAt, allocCells, allocLatest);
				TESTVALUEL(allocLatest, allocCells); //__UHEAP_MARKEND; 
				}
			}
		else if ( ret != KErrNone )
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone));
			}
		}
	CleanupStack::PopAndDestroy(dBase);
	
	// Remove the default file associated with the dBase.
	// Ensure that the default File has been removed.
	TBool dExists = EFalse;
	TRAP(ret, dExists = CContactDatabase::DefaultContactDatabaseExistsL());
	TESTNOERRL(ret);
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KCntFileToExist));

	return !dExists;
	}
	
LOCAL_C TBool GetDefaultNameLOOM_ServerSideL()
	{
	test.Next(_L("GetDefaultNameL Server side OOM test.\n"));

	CContactDatabase* dBase = NULL;
	// Create the default contacts file.
	TRAPD(err, dBase = CContactDatabase::ReplaceL());
	// need the file to be present
	TESTTRUEL((err == KErrNone) && (dBase != NULL));
	CleanupStack::PushL(dBase);


	TInt allocCells(0);
 	TInt ret = KErrNoMemory;
	TInt failAt = 0;

	TFileName filename;
	while(ret != KErrNone)
		{
		failAt++;
		if( !(failAt % 5 ))
			{
			test.Printf(_L("."));
			}
		dBase->SetCntServerHeapFailure(RHeap::EDeterministic, failAt);
		allocCells = dBase->CntServerResourceCount();    //__UHEAP_MARK;

		TRAP(ret, CContactDatabase::GetDefaultNameL(filename));

		//__SRVHEAP_RESET;
		dBase->SetCntServerHeapFailure(RHeap::ENone, 1);		

		if	(ret == KErrNoMemory)
			{
			TInt allocLatest = dBase->CntServerResourceCount();

			if (allocLatest != allocCells)
				{
				test.Printf(_L("iteraton %d failed - initial alloc cells = %d, alloc Latest = %d\n"), 
					failAt, allocCells, allocLatest);
				TESTVALUEL(allocLatest, allocCells); //__UHEAP_MARKEND;
				}
			}
		else if ( ret != KErrNone )
			{
			test.Printf(_L("iteration %d failed - non-memory error in OOM test: %d\n"), failAt, ret);
			TESTTRUEL((ret == KErrNoMemory || ret == KErrNone)); //Bad leaving code
			}
		}
	CleanupStack::PopAndDestroy(dBase);
	TRAP_IGNORE(CContactDatabase::DeleteDefaultFileL());
	return ETrue;
	}
#endif


LOCAL_C void OOMTestingL()
	{
	
	// Client side OOM Testing.
	TESTTRUE(ListDatabasesLOOM_ClientSideL());
	TESTTRUE(OpenLOOM_ClientSideL());
	TESTTRUE(DeleteDatabaseLOOM_ClientSideL());
	TESTTRUE(CreateLOOM_ClientSideL());
	TESTTRUE(GetDefaultNameLOOM_ClientSideL());
	TESTTRUE(DefaultContactDatabaseExistsLOOM_ClientSideL());
	TESTTRUE(ContactDatabaseExistsLOOM_ClientSideL());
	TESTTRUE(ReplaceLOOM_ClientSideL());
	TESTTRUE(DeleteDefaultFileLOOM_ClientSideL());

#ifdef _DEBUG
	// Server side OOM testing. Only server side debug method are
	// available in debug mode.

	TESTTRUE(CreateLOOM_ServerSideL());
	
	TESTTRUE(DeleteDatabaseLOOM_ServerSideL());
	TESTTRUE(DefaultContactDatabaseExistsLOOM_ServerSideL());
	TESTTRUE(OpenLOOM_ServerSideL());
	TESTTRUE(ContactDatabaseExistsLOOM_ServerSideL());
	TESTTRUE(ReplaceLOOM_ServerSideL());
	TESTTRUE(DeleteDefaultFileLOOM_ServerSideL());
	TESTTRUE(GetDefaultNameLOOM_ServerSideL());
#endif

	}

/**
	NegTestL
	
	Intended to identify the failing cases of the API and ensure that they behave
	carefully under these conditions.
	
	This section of the test needs more coverage.
*/
LOCAL_C void NegTestL()
	{
	TInt err (KErrNone);

	test.Next(_L("Test drive Setting and Getting\n"));

		{
		TBool dbDriveSet;
		TDriveUnit driveUnit;

		// Set the Current drive to C:
		TRAP( err, CContactDatabase::SetDatabaseDriveL(TDriveUnit(2), EFalse));
		TESTTRUEL( err == KErrNone );

		// Read the current drive back, ensuring it has been changed correctly.
		dbDriveSet = CContactDatabase::DatabaseDrive(driveUnit);
		TESTTRUEL(dbDriveSet);
		TESTTRUEL(driveUnit == 2);

		// Create and open an empty contact database
		TRAP(err, db = CContactDatabase::ReplaceL());
		TESTTRUEL( err == KErrNone && db != NULL);
		if( !err )
			{
			delete db;
			db = NULL;
			}
		err = KErrNone;
		TBool exists = EFalse;
		TRAP( err, exists = CContactDatabase::DefaultContactDatabaseExistsL());
		TESTTRUEL( exists && err == KErrNone );


		TRAP(err, CContactDatabase::DeleteDefaultFileL());
		TESTTRUEL( err == KErrNone);

		TRAP( err, exists = CContactDatabase::DefaultContactDatabaseExistsL());
		TESTTRUEL( !exists && err == KErrNone );

		// delete again should fail		
		TRAP(err, CContactDatabase::DeleteDefaultFileL());
		TESTTRUEL( err == KErrNotFound );

		db = CContactDatabase::CreateL();
		TRAP( err, exists = CContactDatabase::DefaultContactDatabaseExistsL());
		TESTTRUEL( exists );
		delete db;
		db = NULL;

		TRAP( err, CContactDatabase::SetDatabaseDriveL(TDriveUnit(5), EFalse));
		TESTTRUEL( err == KErrNone );

		TRAP( err, dbDriveSet = CContactDatabase::DatabaseDrive(driveUnit));
		TESTTRUEL( err == KErrNone );
		TESTTRUEL( dbDriveSet );
		TESTTRUEL( driveUnit == 5 );

		TRAP( err, CContactDatabase::SetDatabaseDriveL(TDriveUnit(2), EFalse));
		TESTTRUEL( err == KErrNone );

		TRAP( err, dbDriveSet = CContactDatabase::DatabaseDrive(driveUnit));
		TESTTRUEL( dbDriveSet );
		TESTTRUEL( driveUnit == 2 );
		}

	TRAP( err, db = CContactDatabase::OpenL());
	TESTTRUEL( err == KErrNone && db != NULL );
	delete db;
	db = NULL;

	test.Next(_L("Test database opening\n"));
		{
		// open twice
		db = CContactDatabase::OpenL();
		TESTTRUEL(db != NULL);
		CContactDatabase* db2 = CContactDatabase::OpenL();
		TESTTRUEL(db2 != NULL);
		TESTTRUEL(db2 != db);
		delete db;
		db = NULL;
		delete db2;
		db2 =NULL;

		// KNullDesC is used internally, but is not a valid parameter
		TRAP(err, db = CContactDatabase::OpenL(KNullDesC));
		TESTTRUEL(err == KErrBadName);
		TESTTRUEL(db == NULL);
		
		// KNullDesC is used internally, but is not a valid parameter
		TRAP(err, db = CContactDatabase::ReplaceL(KNullDesC));
		TESTTRUEL(err == KErrBadName);
		TESTTRUEL(db == NULL);

		// check for Open and Create Failing
		CContactDatabase::DeleteDefaultFileL();

		TRAP(err, db = CContactDatabase::OpenL());
		TESTTRUEL(err == KErrNotFound);
		TESTTRUEL(db == NULL);

		TRAP(err, db = CContactDatabase::CreateL());
		TESTTRUEL(err == KErrNone);
		delete db;
		db = NULL;

		TRAP(err, db = CContactDatabase::CreateL());
		TESTTRUEL(err == KErrAlreadyExists);
		TESTTRUEL(db == NULL);

		// check for Replace replacing!
		TRAP(err, db = CContactDatabase::ReplaceL());
		TESTTRUEL(err == KErrNone);
		TESTTRUEL(db != NULL);
		delete db;
		db = NULL;

		// wait for server to shutdown (6s)
		User::After(6000000);

		// open twice
		db = CContactDatabase::OpenL();
		db2 = CContactDatabase::OpenL();
		delete db;
		db = NULL;
		delete db2;
		db2 =NULL;
		}
	}

/**
	FunctionTestL
	
	Excersises: ListDatabasesL varients over 2 drives; C & F
	SetDatabaseDriveL, DatabaseDrive, GetDefaultNameL, 
	the varients of DefaultContactDatabaseExistsL, CreateL, 
	asynchronous OpenL, FindContactFile, DeleteDefaultFileL and 
	DeleteDatabaseL.
*/
LOCAL_C void FunctionTestL()
	{

	TFileName addedFile;
	test.Next(_L("FunctionTestL CContactDatabase::ListDatabasesL over multiple drives.\n"));
	// At this point there should only be 1 file which is the default.
	TFileName filename, defaultFilename;


	for(driveIdx =0;driveIdx<NoDrives; driveIdx++ )
		{
		addedFile.Copy(driveArray[driveIdx].Name());
		addedFile.Append(KNewCntTestFile);
		Tidy();

		CDesCArray* cntFiles = NULL;
		TRAPD(err, cntFiles = CContactDatabase::ListDatabasesL());
		TESTTRUEL(err == KErrNone && cntFiles != NULL);
  		NoDatabases = 0;
  		if( !err )
	  		{
	  		NoDatabases = cntFiles->Count();
			delete cntFiles;				
	  		}


  		// Check Set and Get default drive.
		TPtrC drvName(driveArray[driveIdx].Name());
		test.Printf(_L("Testing Drive %S\n"), &drvName);
		test.Next(_L("Setting Drive\n"));
		TRAP( err, CContactDatabase::SetDatabaseDriveL(driveArray[driveIdx], EFalse));
		if( err )
			{
			test.Printf(_L("CContactDatabase::SetDatabaseDriveL:Leave code:%d"),err);		
			}
		TDriveUnit myDriveUnit;
			{
			TESTTRUEL(CContactDatabase::DatabaseDrive(myDriveUnit));
			}
		TESTTRUEL(myDriveUnit == driveArray[driveIdx]);

		// Steve. To see if Drive of default filename changes.
		TRAP( err, CContactDatabase::GetDefaultNameL( filename ));
		if( err )
			{
			test.Printf(_L("CContactDatabase::GetDefaultNameL:Leave code:%d"),err);
			}
		test.Next(_L("Checking default filename.\n"));
		TESTTRUEL(CheckDefaultContactsFilename(filename));
		defaultFilename = filename;

		// Exercise all API's the look for Contacts files.


		TESTTRUEL(CheckCntFilesL( NoDatabases, KNoFiles, EFalse ));
		TESTTRUEL(CheckNoDatabaseFilesL());

		////////////////////////////// ReplaceL and OpenL /////////////////////////

		test.Next(_L("Creating/Replacing default db: ReplaceL.\n"));
		TRAP( err, db = CContactDatabase::ReplaceL() );
		TESTTRUEL( err == KErrNone && db != NULL );
		if ((err != KErrNone) && (db == NULL))
			{
			test.Printf(_L("CContactDatabase::ReplaceL failed: leave code %d, db pointer %X"), err, db);
			}
		delete db;
		db = NULL;
		TESTTRUEL(CheckCntFilesL( NoDatabases + 1, defaultFilename ));
		TESTTRUEL(DefaultDatabaseL());


		__UHEAP_MARK;
		test.Next(_L("Opening Default: OpenL().\n"));
		TRAP( err, db = CContactDatabase::OpenL());
		if ((err != KErrNone) && (db == NULL))
			{
			test.Printf(_L("CContactDatabase::OpenL failed: leave code %d, db pointer %X"), err, db);
			}
		// these tests Leave, as the database needs to be open to continue
		TESTTRUEL( db != NULL );						// db should exist..
		TESTTRUEL( err == KErrNone);

		// Add contacts to the db		
		CArrayFix<TContactItemId>* idList=new(ELeave) CArrayFixFlat<TContactItemId>(5);
		CleanupStack::PushL(idList);
		test.Next(_L("Add new contacts"));
		TRAP(err,AddNewContactsL(idList));
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
		TESTTRUEL( err == KErrNone || err == KErrDiskFull);
#else		
		TESTTRUEL( err == KErrNone );
#endif		

		// Read and modify contacts.
		test.Next(_L("Read and modify contacts"));
		TRAP(err,EditContactsL(idList));
		TESTTRUEL( err == KErrNone );

		CleanupStack::PopAndDestroy(idList);

		delete db;
		db = NULL;
		__UHEAP_MARKEND;

		//Now test opening another database, should succeed
		test.Next(_L("Creating addedFile: CreateL(addedFile).\n"));
		TRAP( err, db = CContactDatabase::CreateL(addedFile) );
		TESTTRUEL( err == KErrNone && db != NULL );
		// check that file with name filename exists.
		TESTTRUEL(CheckCntFilesL( NoDatabases + 2, addedFile ));
		TESTTRUEL(DefaultDatabaseL());

		//Add test to check that the db is opened
		delete db;
		db = NULL;

		// Ensure that RecreateSystemTemplateL does not leave
		test.Printf(_L("About to recreate template on:"));
		test.Printf(addedFile);
		
		test.Next(_L("Testing: RecreateSystemTemplateL( addedFile )"));

		err = KErrNone;
		TRAP( err, CContactDatabase::RecreateSystemTemplateL( addedFile ));
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
		test.Printf(_L("Recieved %d"),err);
		TESTTRUEL( err == KErrNone || err == KErrDiskFull);
#else		
		TESTTRUEL( err == KErrNone );
#endif	

		test.Next(_L("Opening file addedFile: OpenL( addedFile )\n"));
		TRAP( err, db = CContactDatabase::OpenL( addedFile ));
		TESTTRUEL( db != NULL );
		TESTTRUEL( err == KErrNone);

		// Files existing now: Contacts.cdb & Contacts.cdrchar
		////////////////////////////// File Deletion ////////////////////////////

		// So at this point we have a default db and another.
		// Time to start deleting them ! Start with DeleteDefaultFileL
		test.Next(_L("Deleting default file: DeleteDefaultFileL()\n"));
		TRAP( err, CContactDatabase::DeleteDefaultFileL());
		TESTTRUEL( err == KErrNone );
		// only filename should exist.
		TESTTRUEL(CheckCntFilesL( NoDatabases + 1, addedFile ));
		TESTTRUEL(CheckNotDefaultContactsFiles());

		// Check that an open file is cannot be deleted.
		// There is one remaining database which we can now remove using DeleteDatabaseL
		test.Next(_L("Deleting database addedFile, should fail first time: DeleteDatabaseL(addedFile)\n"));
		TRAP(err, CContactDatabase::DeleteDatabaseL(addedFile));
		// Given that this file is currently open it DeleteDatabaseL should fail.
		TESTTRUEL(err == KErrInUse);
		delete db;
		db = NULL;

		// We have just opened another we can now delete the one that was originally open.			
		test.Next(_L("Deleting database addedFile, should pass second time: DeleteDatabaseL(addedFile)\n"));
		TRAP(err, CContactDatabase::DeleteDatabaseL(addedFile));
		TESTTRUEL(err == KErrNone);

		// Check that the file "filename" has been deleted and that 
		// the default file exists.
		TESTTRUEL(CheckCntFilesL( NoDatabases, KContactsFilename, EFalse )); 	// exercise ListDatabasesL

		// NoFileOnPreviousDrive is used by CheckCntFilesL as once we start adding files
		// to the second drive, so ListDatabasesL() without a parameter returns the total on 
		// both drives.
		CDesCArray* driveFiles = CContactDatabase::ListDatabasesL(driveArray[driveIdx]);
		NoFileOnPreviousDrive = driveFiles->Count();
		delete driveFiles;
		}
	}
	

LOCAL_C TBool EmptyCntDatabaseDeleteL()
	{
/**
@SYMTestCaseID PIM-T-SEC-CNTDBASE-DEF049719-0001
@SYMTestType UT
@SYMTestPriority High
@SYMDEF DEF049719
@SYMTestCaseDependencies CntModel CoreAppsTest
@SYMTestCaseDesc Check that a zero length CntDatabase file can be deleted using CContactDatabase::DeleteDatabaseL()
@SYMTestActions Create an empty file where the default contact database should be. 
Try to delete it using CContactDatabase::DeleteDefaultFileL().
Check that it has been deleted.
@SYMTestExpectedResults The empty contact database should be deleted.
*/	
	test.Next(_L("@SYMTestCaseID:PIM-T-SEC-CNTDBASE-DEF049719-0001 Check zero length CntDatabase can be deleted in CContactDatabase::DeleteDatabaseL()"));
	TBool result(EFalse);
	// Create an empty file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	RFile file;
	
	RPIMTestServer serv;
        User::LeaveIfError(serv.Connect());
	serv.CreateEmptyFileL(KCdbPrivate);
	serv.Close();
	const TDesC& datab=KCdbSecure;
	
	file.Close();
	fs.Close();

	// Delete the contact database
	TRAPD(err,CContactDatabase::DeleteDatabaseL(datab));
			
	result = (err == KErrNone);	
	if(result)
		{
		// try to open the file
		User::LeaveIfError(fs.Connect());
		User::LeaveIfError(serv.Connect());
		TInt ret(serv.CheckForFile(KCdbPrivate));
		serv.Close();
		
		file.Close();
		fs.Close();
		
		// if the file is not found then it has been deleted and test is successful
		result= (ret == KErrNotFound);
		}
	return result;
	}

LOCAL_C TBool EmptyCntDatabaseOverwriteL()
	{
/**
@SYMTestCaseID PIM-T-SEC-CNTDBASE-DEF049719-0002
@SYMTestType UT
@SYMTestPriority High
@SYMDEF DEF049719
@SYMTestCaseDependencies CntModel CoreAppsTest
@SYMTestCaseDesc Check that a zero length CntDatabase file can't be overwritten using CContactDatabase::CreateL()
@SYMTestActions Create an empty file where the default contact database should be. 
Try to overwrite it using CContactDatabase::CreateL().
@SYMTestExpectedResults The empty contact database should not be overwritten.
*/	
	test.Next(_L("@SYMTestCaseID:PIM-T-SEC-CNTDBASE-DEF049719-0002 Check zero length CntDatabase can't be overwritten in CContactDatabase::CreateL()"));

	// Create an empty file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	RFile file;
	
	RPIMTestServer serv;
        User::LeaveIfError(serv.Connect());
	serv.CreateEmptyFileL(KCdbPrivate);
	serv.Close();
	const TDesC& datab=KCdbSecure;
	
	file.Close();
	fs.Close();
	
	CContactDatabase * tData=NULL;
	// create the contact database
	TRAPD(err,tData=CContactDatabase::CreateL(datab));
	CContactDatabase::DeleteDatabaseL(datab); //Delete the test file after use
	delete tData;
		
	return (err == KErrAlreadyExists);
	}

LOCAL_C void ContactDatabaseZeroL()
	{
	TESTTRUEL(EmptyCntDatabaseDeleteL());
	TESTTRUEL(EmptyCntDatabaseOverwriteL());
	}
	
/**
@SYMTestCaseID PIM-T-SEC-CNTDBASE-0001
*/
LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTestCaseID:PIM-T-SEC-CNTDBASE-0001 T_Sec_CntDBase\n"));

	User::LeaveIfError(fileServer.Connect());

	// Before we do anything else check that there is an MMC drive mapped.
	DrivesL();

	__UHEAP_MARK;
	// This test Switches drives and calls CreateL, ListDatabasesL(TDriveUnit)
	FunctionTestL();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	// This test is similar to the previous, but doesn't use ListDatabasesL(TDriveUnit)
	// It also makes use of ReplaceL()
	NegTestL();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	OOMTestingL();
	__UHEAP_MARKEND;
	
	__UHEAP_MARK;
	ContactDatabaseZeroL();
	__UHEAP_MARKEND;
	
    }

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	// The contacts database uses Active Objects to it must
	// be provided with an Active Scheduler.
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
		    TRAPD(err,DoTestsL());
			// reset default drive location
		    TRAP_IGNORE(CContactDatabase::SetDatabaseDriveL(TDriveUnit(EDriveC), EFalse));
		    test(err == KErrNone);
			delete cleanup;
			}
		delete scheduler;
		}


	test(__NB_failures == 0);

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return KErrNone;
    }
