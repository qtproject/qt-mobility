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
#include <f32file.h>
#include <s32file.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <coreappstest/testserver.h>

#include <sbeclient.h>

#include "t_utils.h"
#include "t_utils2.h"
#include "t_cnt_backup.h"


// cleanup version of test macro
LOCAL_C void TestCondition(TBool aCondition, TInt aLineNumber);
#define TEST_CONDITION(x) TestCondition(x, __LINE__)


// test databases to create, for use by Contacts test clients
_LIT(KBackupDatabaseFile1,"c:BackupTest1.cdb");
_LIT(KBackupDatabaseFile2,"c:BackupTest2.cdb");
_LIT(KBackupDatabaseFile3,"c:BackupTest3.cdb"); 


// real location of Contacts db files
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
_LIT(KBackupDatabaseFileFullPath1,"c:\\private\\10003A73\\BackupTest1.cdb");
_LIT(KBackupDatabaseFileFullPath2,"c:\\private\\10003A73\\BackupTest2.cdb");
_LIT(KBackupDatabaseFileFullPath3,"c:\\private\\10003A73\\BackupTest3.cdb"); 
_LIT(KBackupDestinationDatabasePath1,"c:\\t_cnt_backup\\BackupTest1.cdb");
_LIT(KBackupDestinationDatabasePath2,"c:\\t_cnt_backup\\BackupTest2.cdb");
_LIT(KBackupDestinationDatabasePath3,"c:\\t_cnt_backup\\BackupTest3.cdb"); 
_LIT(KContactsBackupRegistrationDefaultDbFileFullPath, "c:\\private\\10003A73\\backup_registrationcontacts_cdb.xml");
#else
_LIT(KBackupDatabaseFileFullPath1,"c:\\private\\10003A73\\SQLite__BackupTest1.cdb");
_LIT(KBackupDatabaseFileFullPath2,"c:\\private\\10003A73\\SQLite__BackupTest2.cdb");
_LIT(KBackupDatabaseFileFullPath3,"c:\\private\\10003A73\\SQLite__BackupTest3.cdb"); 
_LIT(KBackupDestinationDatabasePath1,"c:\\t_cnt_backup\\SQLite__BackupTest1.cdb");
_LIT(KBackupDestinationDatabasePath2,"c:\\t_cnt_backup\\SQLite__BackupTest2.cdb");
_LIT(KBackupDestinationDatabasePath3,"c:\\t_cnt_backup\\SQLite__BackupTest3.cdb"); 
_LIT(KContactsBackupRegistrationDefaultDbFileFullPath, "c:\\private\\10003A73\\backup_registrationSQLite__contacts_cdb.xml");
#endif


// Contact model backup files
_LIT(KContactsBackupRegistrationIniFileFullPath, "c:\\private\\10003A73\\backup_registration.xml");
_LIT(KContactsIniBackupFileFullPath, "c:\\private\\10003A73\\Cntmodel.ini");
_LIT(KBackupDestinationDirectory, "c:\\t_cnt_backup\\");
_LIT(KBackupDestinationContactsIniPath,"c:\\t_cnt_backup\\Cntmodel.ini");
_LIT(KRestoreDamagedDatabasePath,"z:\\t_cnt_backup_Corrupted_100065FF.cdb");

// delays between Backup/Restoretest steps, to allow notifications to propagate to all components
static const TInt KWaitForBackupNotification(15000000);
static const TInt KWaitForClientToSettle(2000000);
static const TInt KWaitForSystemToSettle(20000000);


// Test databases have 10 contacts created, various contact Ids are used for different purposes
static const TInt KNumContactsInDatabase= 10;
static const TContactItemId KOpenContactId1 = 1;
static const TContactItemId KOpenContactId2 = 2;
static const TContactItemId KOpenContactId3 = 3;
static const TContactItemId KSpeedDialContactId = 4;
static const TContactItemId KReadContactId = 5;
static const TInt KSpeedDialPositionOne = 1;


_LIT(KAsyncFindMatch,"a");


// global data for unit test
_LIT(KTestName, "t_cnt_backup");
static RTest test(KTestName);
static RFs TheFsSession;


/*
 Class CBackupTestConductor drives the whole set of tests
 1. Check the Backup Registration file is present
 2. Check the Secure Backup Engine has processed it as we expect
 3. ... various Backup / Restore scenarios for contacts clients
 */

CBackupTestConductor* CBackupTestConductor::NewL()
	{
	CBackupTestConductor* self = new (ELeave) CBackupTestConductor();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBackupTestConductor::~CBackupTestConductor()
	{
	// ensure test databases are deleted
	CBackupTestConductor::DeleteDatabases();

	// cleanup backup files
	TheFsSession.Delete(KBackupDestinationContactsIniPath);
	TheFsSession.Delete(KBackupDestinationDatabasePath1);
	TheFsSession.Delete(KBackupDestinationDatabasePath2);
	TheFsSession.Delete(KBackupDestinationDatabasePath3);
	TheFsSession.RmDir(KBackupDestinationDirectory);
	}


CBackupTestConductor::CBackupTestConductor()
	{
	}


void CBackupTestConductor::ConstructL()
	{
	const TInt err = TheFsSession.MkDirAll(KBackupDestinationDirectory);
	if ((err != KErrNone) && (err != KErrAlreadyExists))
		{
		test.Printf(_L("Unable to create the directory %S\n"), &KBackupDestinationDirectory);
		User::Leave(err);
		}
	}


void CBackupTestConductor::RunTestsL()
	{
	// need databases for B&R testing
	CreateDatabasesL();
	// 1.x Check Backup Registration files
	test.Start(_L("Backup registration files exist"));
	TEST_CONDITION(CheckXMLBackupRegistrationExistsL());
	test.End();

	
	// framework that runs a Backup & Restore test cycle
	CBackupTestFramework* backupFramework = CBackupTestFramework::NewLC();
	CleanupStack::Check(backupFramework);

	// 2. ... run all Backup / Restore test situations
	backupFramework->DoBackupRestoreTestsL();

	// delete framework
	CleanupStack::PopAndDestroy(backupFramework);
	}


// demonstrates that database is populated correctly
#ifdef _DEBUG
void CBackupTestConductor::ShowFieldsL(CContactDatabase* aDb)
#else
void CBackupTestConductor::ShowFieldsL(CContactDatabase* /*aDb*/)
#endif
	{
#if _DEBUG
	CContactItem* contact = aDb->ReadContactLC(iFirstContactId);
	
	CContactItemFieldSet& fields = contact->CardFields();
	const TInt fieldCount = fields.Count();

	for (TInt i = 0; i < fieldCount; i++)
		{
		CContactItemField &field = fields[i];
		if (field.StorageType() == KStorageTypeText)
			{
			TPtrC fieldText = field.TextStorage()->Text();
			TPtrC labelText = field.Label();
		    test.Printf(_L("--Field(%d) %S: %S \n\r"), i, &labelText, &fieldText);
			}
		}
	CleanupStack::PopAndDestroy(contact);	
#endif
	}


/*
 Populate the database - add TEN contacts, based on the default template,
 setting FOUR fields. (Given name, Family name, phone number + Company name.)
 Set the Company Name field to be the same as the name of the database.
 
 @param aDb the CContactDatabase instance to add contacts to.
 @param aFileName Database file name to put in the Company name field
 */
void CBackupTestConductor::PopulateDatabaseL(CContactDatabase* aDb,const TDesC& aFileName)
	{
	test.Printf(_L("Populating database %S\n"), &aFileName);
	TInt index = 0;
	TBool allDone = EFalse;

	const TContactItemId templateId = aDb->TemplateId();
	CContactItem* templateCard = aDb->ReadContactLC(templateId);

	do
	{
		CContactCard* card = CContactCard::NewLC(templateCard);

		CContactItemFieldSet& cardFields = card->CardFields();

		TInt posLastName = cardFields.Find(KUidContactFieldGivenName);
		TInt posFirstName = cardFields.Find(KUidContactFieldFamilyName);


		switch (index)
			{ // (famous writers on computer topics)
			case 0:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Kernighan"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Brian"));
				break;

			case 1:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Ritchie"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Dennis"));
				break;

			case 2:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Thompson"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Ken"));
				break;

			case 3:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Aho"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Alfred"));
				break;

			case 4:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Weinberger"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Peter J."));
				break;

			case 5:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Bourne"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Steven"));
				break;

			case 6:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Pike"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Rob"));
				break;

			case 7:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Bellovin"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Steven"));
				break;

			case 8:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Stevens"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("W. Richard"));
				break;

			case 9:
				cardFields[posLastName].TextStorage()->SetTextL(_L("Raymond"));
				cardFields[posFirstName].TextStorage()->SetTextL(_L("Eric"));
				// last contact
				allDone = ETrue;
				break;
			}


		// Set the telephone number
		TInt posNumber = cardFields.Find(KUidContactFieldPhoneNumber);
		if (posNumber != KErrNotFound)
			{
			cardFields[posNumber].SetMapping(KUidContactFieldVCardMapTEL);
			cardFields[posNumber].TextStorage()->SetTextL(_L("1234"));
			}


		// set database file name as company name
		TInt posCompanyName = cardFields.Find(KUidContactFieldCompanyName);
		if (posCompanyName != KErrNotFound)
			{
			cardFields[posCompanyName].TextStorage()->SetTextL(aFileName);
			}


		iLastContactId = aDb->AddNewContactL(*card);
		if (index == 0)
			{
			iFirstContactId = iLastContactId;
			}

		CleanupStack::PopAndDestroy(card);
		++index;
		
		test.Printf(_L("."));
		}
		while (!allDone);

	CleanupStack::PopAndDestroy(templateCard);

	ShowFieldsL(aDb);
	}

// create THREE empty contacts databases then populate each
void CBackupTestConductor::CreateDatabasesL()
	{
	// Create & close databases
	CContactDatabase* db;
	// Need to create the default data base if it does not exist to enusre
	// prescence of backup registration file.
	if (!CContactDatabase::DefaultContactDatabaseExistsL())
		{
		db = CContactDatabase::CreateL();
		delete db;		
		}
		
	test.Printf(_L("Creating database %S\n"), &KBackupDatabaseFile1);
	db = CContactDatabase::ReplaceL(KBackupDatabaseFile1);
	PopulateDatabaseL(db, KBackupDatabaseFile1);
	delete db;
	
	test.Printf(_L("Creating database %S\n"), &KBackupDatabaseFile2);
	db = CContactDatabase::ReplaceL(KBackupDatabaseFile2);
	PopulateDatabaseL(db, KBackupDatabaseFile2);
	delete db;
	
	test.Printf(_L("Creating database %S\n"), &KBackupDatabaseFile3);
	db = CContactDatabase::ReplaceL(KBackupDatabaseFile3);
	PopulateDatabaseL(db, KBackupDatabaseFile3);
	delete db;
	}


void CBackupTestConductor::DeleteDatabases()
	{
	// Delete databases
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KBackupDatabaseFile1));
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KBackupDatabaseFile2));
	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KBackupDatabaseFile3));
	}


/*
 Check that the Contact Model's Backup Registration files are present and > 0 bytes length
 */
TBool CBackupTestConductor::CheckXMLBackupRegistrationExistsL()
	{
	// Check the Contacts private area for the expected registration files
	TEntry entry;

	// See if initialisation registration file is in the directory on drive Z:
	TInt error = TheFsSession.Entry(KContactsBackupRegistrationIniFileFullPath, entry);

	if (error == KErrNotFound)
		{
		return EFalse;
		}
		
	User::LeaveIfError(error);

	if(entry.iSize == 0)
		{
		// file is present but is empty
		User::Leave(KErrCorrupt);
		}

	// See if default database registration file is in the directory on drive Z:
	error = TheFsSession.Entry(KContactsBackupRegistrationDefaultDbFileFullPath, entry);

	if (error == KErrNotFound)
		{
		return EFalse;
		}
		
	User::LeaveIfError(error);

	if(entry.iSize == 0)
		{
		// file is present but is empty
		User::Leave(KErrCorrupt);
		}

	// Found the registration files
	return ETrue;
	}
			

///////////////////////////////////////////////////////////////////////////////////////

/*
 Framework for simulating Contact Model clients
 
 @param aDatabaseName database file name to open
 @param aObserver Observer in Backup / Restore test framework to count Backup and Restore notifications
 @param aContactViewObserver Observer in Backup / Restore test framework to count View Ready and Unavailable notifications
 */
CContactsClient* CContactsClient::NewLC(const TDesC& aDatabaseName, MBackupRestoreObserver* aObserver, MContactViewObserver& aContactViewObserver, TBool aCloseForBackup)
	{
	CContactsClient* self = new (ELeave) CContactsClient(aObserver, aContactViewObserver, aCloseForBackup);
	CleanupStack::PushL(self);
	self->ConstructL(aDatabaseName);
	return self;
	}
	
	
CContactsClient::CContactsClient(MBackupRestoreObserver* aObserver, MContactViewObserver& aContactViewObserver, TBool aCloseForBackup)
	: CActive(EPriorityStandard), iObserver(aObserver), iContactViewObserver(aContactViewObserver), iDatabaseIsOpen(EFalse), iCloseForBackup(aCloseForBackup), iOpenContacts(4)
	{
	CActiveScheduler::Add(this);
	}

CContactsClient::~CContactsClient()
	{
	Cancel();
	CloseViews();
	CloseDatabase();
	iOpenContacts.ResetAndDestroy();

	delete iDbName;
	}


void CContactsClient::StartActiveClose()
	{
	// start active object to close contact/view/database
	// required as these can't be closed directly from the event handlers
	if (!IsActive() && iDatabaseIsOpen)
		{
		TRequestStatus* pS = &iStatus;
		User::RequestComplete(pS,0);
		SetActive();
		}
	}
	
void CContactsClient::DoCancel()
	{
	}
	

void CContactsClient::RunL()
	{
	if (iStatus.Int() != KErrCancel)
		{
		CloseDatabase();
		}
	}
	
void CContactsClient::CloseDatabase()
	{
	CloseContacts();
	CloseViews();
	iDatabaseIsOpen = EFalse;

	if (iIdleFinder)
		{
		delete iIdleFinder;
		iIdleFinder = NULL;
		}

	if (iItemFieldDef)
		{
		delete iItemFieldDef;
		iItemFieldDef = NULL;
		}

	if (iActiveCompress)
		{
		delete iActiveCompress;
		iActiveCompress = NULL;
		}

	if (iActiveRecover)
		{
		delete iActiveRecover;
		iActiveRecover = NULL;
		}

	delete iNotifier;
	iNotifier = NULL;
	delete iDb;
	iDb = NULL;
	}


void CContactsClient::ConstructL(const TDesC& aDatabaseName)
	{
	// remember the database name
	iDbName = aDatabaseName.AllocL();
	// open it
	OpenDatabaseL();
	}


void CContactsClient::OpenDatabaseL()
	{
	// can only open once
	if (iDatabaseIsOpen)
		{
		User::Leave(KErrInUse);
		}

	test.Printf(_L("Open database %S\n"), iDbName);
	iDb = CContactDatabase::OpenL(iDbName->Des());
	iDatabaseIsOpen = ETrue;
	iNotifier = CContactChangeNotifier::NewL(*iDb,this);
	}


const TDesC& CContactsClient::DatabaseName()
	{
	return *iDbName;
	}


TBool CContactsClient::IsOpen()
	{
	return iDatabaseIsOpen;
	}

TBool CContactsClient::IsCloseForBackup()
	{
	return iCloseForBackup;
	}	

void CContactsClient::OpenRemoteViewL()
	{
	if (iRemoteView != NULL)
		{
		User::Leave(KErrInUse);
		}
		
	RContactViewSortOrder viewSortOrder;
	
	viewSortOrder.AppendL(KUidContactFieldFamilyName);
	viewSortOrder.AppendL(KUidContactFieldGivenName);
	viewSortOrder.AppendL(KUidContactFieldCompanyName);

	iRemoteView = CContactRemoteView::NewL(iContactViewObserver, *iDb, viewSortOrder, EContactsOnly);

	viewSortOrder.Close();
	}


void CContactsClient::OpenLocalViewL()
	{
	if (iLocalView != NULL)
		{
		User::Leave(KErrInUse);
		}
		
	RContactViewSortOrder viewSortOrder;
	
	viewSortOrder.AppendL(KUidContactFieldFamilyName);
	viewSortOrder.AppendL(KUidContactFieldGivenName);
	viewSortOrder.AppendL(KUidContactFieldCompanyName);

	iLocalView = CContactLocalView::NewL(iContactViewObserver, *iDb, viewSortOrder, EContactsOnly);

	viewSortOrder.Close();
	}


void CContactsClient::CloseViews()
	{
	if (iLocalView)
		{
		iLocalView->Close(iContactViewObserver);
		iLocalView = NULL;
		}

	if (iRemoteView)
		{
		iRemoteView->Close(iContactViewObserver);
		iRemoteView = NULL;
		}
	}


void CContactsClient::FindPhoneNumberL()
	{
	_LIT(KPhoneNumber, "1234");
	const TInt KNumMatchDigit = 4;
	CContactIdArray* array = iDb->MatchPhoneNumberL(KPhoneNumber, KNumMatchDigit);
	CleanupStack::PushL(array);
	TEST_CONDITION(array->Count() == 10);
	CleanupStack::PopAndDestroy(array);
	}


// try to open a contact for edit
void CContactsClient::OpenContactL(TContactItemId aContactId)
	{
	CContactItem* contact = iDb->OpenContactL(aContactId);
	iOpenContacts.AppendL(contact);
	}


// try to commit an open contact
void CContactsClient::CommitContactL(TContactItemId aContactId)
	{
	const TInt numOpenContacts = iOpenContacts.Count();
	
	for (TInt j = 0; j < numOpenContacts; ++j)
		{
		CContactItem* tempContact = iOpenContacts[j];
		if (tempContact->Id() == aContactId)
			{
			iDb->CommitContactL(*tempContact);
			iOpenContacts.Remove(j);
			delete tempContact;
			return;
			}
		}
	// contact isn't in open list
	TEST_CONDITION(EFalse);
	}


// close any open contacts
void CContactsClient::CloseContacts()
	{
	while (iOpenContacts.Count())
		{
		CContactItem* tempContact = iOpenContacts[0];
		TRAP_IGNORE(iDb->CloseContactL(tempContact->Id()));
		iOpenContacts.Remove(0);
		delete tempContact;
		}
	}

// try to Read the template Contact from database
// and also, check that one record exists with the Company Name field set 
// to the clients database name 
void CContactsClient::ReadContactL()
	{
	CContactItem* contact = iDb->ReadContactLC(KReadContactId);
	CleanupStack::PopAndDestroy(contact);

	// find records with the Company Name field set
	CContactItemFieldDef* fieldDef = new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	TFieldType fieldtype = KUidContactFieldCompanyName;
	fieldDef->AppendL(fieldtype);
	CContactIdArray* matchList = iDb->FindLC(iDbName->Des(),fieldDef);
  
	// all added contacts should have this field initialised
	TInt matchCount = matchList->Count();
    TEST_CONDITION(matchCount == KNumContactsInDatabase);	

    CleanupStack::PopAndDestroy(2, fieldDef);
	}


void CContactsClient::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	/** Contacts DB about to be backed up  */
	if (aEvent.iType == EContactDbObserverEventBackupBeginning)
		{
		iRestore = EFalse;
		iObserver->ContactsBackupRestoreObserver(*this, iRestore, ETrue);
		}
	/** Contacts DB about to be restored */
	else if (aEvent.iType == EContactDbObserverEventRestoreBeginning)
		{
		iRestore = ETrue;
		iObserver->ContactsBackupRestoreObserver(*this, iRestore, ETrue);
		}
	/** The backup/restore operation has completed. */
	else if (aEvent.iType == EContactDbObserverEventBackupRestoreCompleted)
		{
		iObserver->ContactsBackupRestoreObserver(*this, iRestore, EFalse);
		}
	/** The restore operation has completed but the database could not be opened. */
	else if (aEvent.iType == EContactDbObserverEventRestoreBadDatabase)
		{
		iObserver->ContactsBackupRestoreObserver(*this, iRestore, EFalse);
		}
	}


void CContactsClient::StartAsyncFindL()
	{
	if (!iItemFieldDef)
		{
		iItemFieldDef = new(ELeave) CContactItemFieldDef();
		iItemFieldDef->AppendL(KUidContactFieldGivenName);
		iItemFieldDef->AppendL(KUidContactFieldFamilyName);
		iItemFieldDef->AppendL(KUidContactFieldCompanyName);
		iItemFieldDef->AppendL(KUidContactFieldPhoneNumber);
		iItemFieldDef->AppendL(KUidContactFieldFax);
		iItemFieldDef->AppendL(KUidContactFieldEMail);
		iItemFieldDef->AppendL(KUidContactFieldUrl);
		iItemFieldDef->AppendL(KUidContactFieldAddress);
		iItemFieldDef->AppendL(KUidContactFieldLocality);
		iItemFieldDef->AppendL(KUidContactFieldRegion);
		iItemFieldDef->AppendL(KUidContactFieldPostcode);
		iItemFieldDef->AppendL(KUidContactFieldCountry);
		iItemFieldDef->AppendL(KUidContactFieldNote);
		}

	iIdleFinder = iDb->FindAsyncL(KAsyncFindMatch, iItemFieldDef, this);
	}


void CContactsClient::StartActiveCompactL()
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	if (! iActiveCompress)
		{
		iActiveCompress = iDb->CreateCompressorLC();
		CleanupStack::Pop();
		}
	iActiveCompress->SetObserver(this);
#endif
	}


void CContactsClient::StartActiveRecoverL()
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	if (! iActiveRecover)
		{
		iActiveRecover = iDb->CreateRecoverLC();
		CleanupStack::Pop();
		}
	iActiveRecover->SetObserver(this);
#endif
	}


void CContactsClient::StartIdleSorterL(TRequestStatus& aStatus)
	{
	CArrayFix<CContactDatabase::TSortPref>* sortOrder = new(ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(3);
	CleanupStack::PushL(sortOrder);

	// prepare a sort order
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldGivenName));
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldFamilyName));
	sortOrder->AppendL(CContactDatabase::TSortPref(KUidContactFieldCompanyName));

	// starts Contacts Model Idle Sorter
	iDb->SortAsyncL(sortOrder, aStatus);


	// sortOrder ownership was passed to the Contact Model
	CleanupStack::Pop(sortOrder);
	}


void CContactsClient::CompleteAsyncFindL()
	{
	// nothing to do Async Find completes in its own time
	}


void CContactsClient::CompleteActiveCompactL()
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	TInt step;
	while(iActiveCompress->Step())
		{
		step = iActiveCompress->StepsTogo();
		TEST_CONDITION(step > 0);
		}
	step = iActiveCompress->StepsTogo();
	TEST_CONDITION(step == 0);
	delete iActiveCompress;
	iActiveCompress = NULL;
#endif
	}


void CContactsClient::CompleteActiveRecoverL()
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	TInt step;
	while(iActiveRecover->Step())
		{
		step = iActiveRecover->StepsTogo();
		TEST_CONDITION(step > 0);
		}
	step = iActiveRecover->StepsTogo();
	TEST_CONDITION(step == 0);
	delete iActiveRecover;
	iActiveRecover = NULL;
#endif
	}


// class MContactUiCompactObserver
void CContactsClient::Step(TInt /*aStep*/)
	{
	}


void CContactsClient::HandleError(TInt /*aError*/)
	{
	}


// class MIdleFindObserver
void CContactsClient::IdleFindCallback()
	{
	}


// Speed Dial Check, Remove and Set - for testing cntmodel.ini is Backed up and Restored correctly
void CContactsClient::CheckSpeedDialSetL(TContactItemId aContactId, TInt aSpeedDialPosition)
	{
	HBufC* buf = HBufC::NewLC(100);	
	TPtr fieldContents = buf->Des();
	
	TContactItemId id = iDb->GetSpeedDialFieldL(aSpeedDialPosition, fieldContents);
	CleanupStack::PopAndDestroy(buf);

	TEST_CONDITION(id == aContactId);
	}


void CContactsClient::RemoveSpeedDialL(TContactItemId aContactId, TInt aSpeedDialPosition)
	{
	iDb->RemoveSpeedDialFieldL(aContactId, aSpeedDialPosition);
	}


void CContactsClient::SetSpeedDialL(TContactItemId aContactId, TInt aSpeedDialPosition)
	{
	CContactItem* speedDialItem = iDb->OpenContactLX(aContactId);
	CleanupStack::PushL(speedDialItem);
	CContactItemFieldSet& cardFields = speedDialItem->CardFields();

	TInt posNumber = cardFields.Find(KUidContactFieldPhoneNumber);

	iDb->SetFieldAsSpeedDialL(*speedDialItem, posNumber, aSpeedDialPosition);
	CleanupStack::PopAndDestroy(speedDialItem);
	CleanupStack::Pop(); // Lock Record
	}


//
// Backup and Restore Test Framework class
//

CBackupHelper* CBackupHelper::NewL(RPIMTestServer& aTestServer)
	{
    CBackupHelper* self = new(ELeave) CBackupHelper(aTestServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
	
CBackupHelper::CBackupHelper(RPIMTestServer& aTestServer) : 
    iTestServ (aTestServer)
	{
	}
	
	
void CBackupHelper::ConstructL()
	{
	/* Initialise the drive list to empty and then get drive list data from
	 File Server.
	 Required before Backup and Restore testing
	 */
	iDriveList.FillZ();
	User::LeaveIfError(TheFsSession.DriveList(iDriveList));

	iBackupClient = conn::CSBEClient::NewL();
	}


CBackupHelper::~CBackupHelper()
	{
	delete iBackupClient;
	}
	
	
void CBackupHelper::StartBackupL(conn::TBURPartType aBURPartType, conn::TBackupIncType aBackupIncType)
	{
	/* Use the Secure Backup Engine API to "start" a backup.  Ultimate effect
	 is to cause CContactsClient::HandleDatabaseEventL() to be called.
	 This happens indirectly through CLockSrvServer::BackupBeginningL()/
	 BackupCompletedL()/RestoreBeginningL()/RestoreCompletedL() methods.
	 These methods are implementations for MBackupRestoreNotificatioObserver
	 which monitors changes in the P&S property conn::KUidBackupRestoreKey.
	 This P&S property is changed by calling the Secure Backup Engine API
	 below.
	 */	
	iBackupClient->SetBURModeL(iDriveList, aBURPartType, aBackupIncType);
	}


void CBackupHelper::EndBackupL()
	{
	/* Use the Secure Backup Engine API to "end" a backup.  Ultimate effect
	 is to cause CContactsClient::HandleDatabaseEventL() to be called.
	 This happens indirectly through CLockSrvServer::BackupBeginningL()/
	 BackupCompletedL()/RestoreBeginningL()/RestoreCompletedL() methods.
	 These methods are implementations for MBackupRestoreNotificatioObserver
	 which monitors changes in the P&S property conn::KUidBackupRestoreKey.
	 This P&S property is changed by calling the Secure Backup Engine API
	 below.
	 */
	iBackupClient->SetBURModeL(iDriveList, conn::EBURNormal, conn::ENoBackup);
	}


void CBackupHelper::StartRestoreL()
	{
	/* Use the Secure Backup Engine API to "start" a restore.  Ultimate effect
	 is to cause CContactsClient::HandleDatabaseEventL() to be called.
	 This happens indirectly through CLockSrvServer::BackupBeginningL()/
	 BackupCompletedL()/RestoreBeginningL()/RestoreCompletedL() methods.
	 These methods are implementations for MBackupRestoreNotificatioObserver
	 which monitors changes in the P&S property conn::KUidBackupRestoreKey.
	 This P&S property is changed by calling the Secure Backup Engine API
	 below.
	 */
	iBackupClient->SetBURModeL(iDriveList, conn::EBURRestoreFull, conn::EBackupBase);
	}


void CBackupHelper::EndRestoreL()
	{
	/* Use the Secure Backup Engine API to "end" a restore.  Ultimate effect
	 is to cause CContactsClient::HandleDatabaseEventL() to be called.
	 This happens indirectly through CLockSrvServer::BackupBeginningL()/
	 BackupCompletedL()/RestoreBeginningL()/RestoreCompletedL() methods.
	 These methods are implementations for MBackupRestoreNotificatioObserver
	 which monitors changes in the P&S property conn::KUidBackupRestoreKey.
	 This P&S property is changed by calling the Secure Backup Engine API
	 below.
	 */
	iBackupClient->SetBURModeL(iDriveList, conn::EBURNormal, conn::ENoBackup);
	}




//
// Simulate Backup of .ini file. Indicating if the operation was a success.
//
TBool CBackupHelper::SimulateIniBackup()
	{
    // Backup CntModel.ini file.
    TRAPD(error, iTestServ.CopyFileL(KContactsIniBackupFileFullPath,
        KBackupDestinationContactsIniPath))
	
	if (error != KErrNone)
		{
		test.Printf(_L("Copy failed with error %i"), error);
		return EFalse;
		}
	return ETrue;
	}


//
// Simulate Backup of database(s). Indicating if the operation was a success.
//
TBool CBackupHelper::SimulateDbBackup()
	{
    // backup databases
    TRAPD(error, iTestServ.CopyFileL(KBackupDatabaseFileFullPath1, KBackupDestinationDatabasePath1))

	if (error == KErrNone)
		{
        TRAP(error, iTestServ.CopyFileL(KBackupDatabaseFileFullPath2, KBackupDestinationDatabasePath2))
		}

	if (error == KErrNone)
		{
        TRAP(error, iTestServ.CopyFileL(KBackupDatabaseFileFullPath3, KBackupDestinationDatabasePath3))
		}

	if (error != KErrNone)
		{
		test.Printf(_L("Copy failed with error %i"), error);
		return EFalse;
		}

	return ETrue;
	}


//
// Simulate restore of .ini file. Indicating if the operation was a success.
//
TBool CBackupHelper::SimulateIniRestore()
	{
	// Restore CntModel.ini file.
    TRAPD(error, iTestServ.CopyFileL(KBackupDestinationContactsIniPath, KContactsIniBackupFileFullPath))

	if (error != KErrNone)
		{
		test.Printf(_L("Copy failed with error %i"), error);
		return EFalse;
		}
	return ETrue;
	}


//
// Simulate restore of database(s). Indicating if the operation was a success.
//
TBool CBackupHelper::SimulateDbRestore()
	{
	// restore databases
    TRAPD(error, iTestServ.CopyFileL(KBackupDestinationDatabasePath1, KBackupDatabaseFileFullPath1))

    if (error == KErrNone)
		{
        TRAP(error, iTestServ.CopyFileL(KBackupDestinationDatabasePath2, KBackupDatabaseFileFullPath2))
		}

	if (error == KErrNone)
		{
        TRAP(error, iTestServ.CopyFileL(KBackupDestinationDatabasePath3, KBackupDatabaseFileFullPath3))
		}

	if (error != KErrNone)
		{
		test.Printf(_L("Copy failed with error %i"), error);
		return EFalse;
		}

	return ETrue;
	}

//
// Simulate restoring a corrupted database.
//
TBool CBackupHelper::SimulateDamagedDbRestore()
    {
    // Restore first one from a damaged and the rest from a good copy.		
    TRAPD(error, iTestServ.CopyFileL(KRestoreDamagedDatabasePath, KBackupDatabaseFileFullPath1))	

    if (error == KErrNone)
        {
        TRAP(error, iTestServ.CopyFileL(KBackupDestinationDatabasePath2, KBackupDatabaseFileFullPath2))
        }

    if (error == KErrNone)
        {
        TRAP(error, iTestServ.CopyFileL(KBackupDestinationDatabasePath3, KBackupDatabaseFileFullPath3))
        }
	
    if (error != KErrNone)
        {
        test.Printf(_L("Copy failed with error %i"), error);
        return EFalse;
        }

    return ETrue;
    }

//
// Backup and Restore Test Framework class
//
static const CBackupTestFramework::TBackupRestoreTestCase BackupTestCases[] =
	{
		/* iTestTitle, iNumDatabases, iClientsPerDatabase, iActivity, iCloseForBackup, iExpectedBackupSuccess */

		/*
		 Test client activities that DO NOT BLOCK Backup / Restore
		 Backup and Restore should always succeed, tests are in pairs:
		 1. does not respond to Backup
		 2. closes all resources when Backup or Restore begin
		*/

		// Single client per database - Backup and Restore should always succeed
		{
		(TText*) L"Clients that keep CContactDatabase Open",
		3, 1, CBackupTestFramework::EIdleClient, ETrue, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Clients that close CContactDatabase",
		3, 1, CBackupTestFramework::EIdleClient, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
    	},

        {
		(TText*) L"Clients that keep CContactDatabase Open; Restore fails",
		3, 1, CBackupTestFramework::ERestoreCorruptDbTest, ETrue, CBackupTestFramework::EAllOpenForBackup , conn::EBackupBase
		},

		// clients open a contact Local View - Backup and Restore should always succeed
		{
		(TText*) L"Clients that keep a Local View Open",
		1, 1, CBackupTestFramework::EOpenLocalView, ETrue, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Clients that close Local View and database",
		1, 1, CBackupTestFramework::EOpenLocalView, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},

		// clients open a contact Remote View - Backup and Restore should always succeed
		{
		(TText*) L"Clients that keep a Remote View Open",
		1, 1, CBackupTestFramework::EOpenRemoteView, ETrue, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Clients that close Remote View and database",
		1, 1, CBackupTestFramework::EOpenRemoteView, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},

		// Multiple clients per database - Backup and Restore should always succeed
		{
		(TText*) L"Multiple clients per database, all keep CContactDatabase Open",
		3, 3, CBackupTestFramework::EIdleClient, ETrue, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Multiple clients per database, all close CContactDatabase",
		3, 3, CBackupTestFramework::EIdleClient, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},
		
				
		{
		(TText*) L"Two clients per database, one close and one keep CContactDatabase open",
		2, 2, CBackupTestFramework::EIdleClient, ETrue, CBackupTestFramework::EHalfCloseForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Two clients per database, one closes its contact item and one keeps its contact item open",
		1, 2, CBackupTestFramework::EMultipleOpenOneContactForEdit, EFalse, CBackupTestFramework::EHalfCloseForBackup, conn::EBackupBase
        },

		/*
		 Test client activities that should BLOCK Backup / Restore
		 Tests are in pairs:
		 1. does not respond to Backup or Restore - (tests that Backup & Restore fail)
		 2. closes all resources when Backup or Restore begin - (tests that Backup & Restore succeed)
		*/

		// Client opens contact for edit - Backup and Restore succeed when resources are freed
		{
		(TText*) L"Client opens contact for edit, resources kept open",
		1, 1, CBackupTestFramework::EOpenOneContactForEdit, EFalse, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Client opens contact for edit, resources released",
		1, 1, CBackupTestFramework::EOpenOneContactForEdit, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},

		// Open two contacts for edit - Backup and Restore succeed when resources are freed
		{
		(TText*) L"Open two contacts for edit, resources kept open",
		1, 1, CBackupTestFramework::EOpenTwoContactsForEdit, EFalse, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Open two contacts for edit, resources released",
		1, 1, CBackupTestFramework::EOpenTwoContactsForEdit, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},


#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
		// Active Compact - Backup and Restore succeed when resources are free
		{
		(TText*) L"Active Compact, resources kept open",
		1, 1, CBackupTestFramework::EContactsActiveCompact, EFalse, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Active Compact, resources released",
		1, 1, CBackupTestFramework::EContactsActiveCompact, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},

		// Active Recover - Backup and Restore succeed when resources are free
		{
		(TText*) L"Active Recover, resources kept open",
		1, 1, CBackupTestFramework::EContactsActiveRecover, EFalse, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Active Recover, resources released",
		1, 1, CBackupTestFramework::EContactsActiveRecover, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},
#endif

		// Async Find - Backup and Restore succeed when resources are freed
		{
		(TText*) L"Async Find - Blocks Backup / Restore until it completes",
		1, 1, CBackupTestFramework::EContactsAsyncFind, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},

		// Contacts Idle Sorter - Backup and Restore succeed when it completes and resources are freed
		{
		(TText*) L"Contacts Idle Sorter - Blocks Backup / Restore until it completes",
		1, 1, CBackupTestFramework::EContactsIdleSorter, ETrue, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},

		// Contacts client updates SpeedDial and checks this is Backed-up and Restored
		{
		(TText*) L"Contacts test for .Ini file Backup and Restore (clients don't close)",
		1, 1, CBackupTestFramework::EContactsIniFileTest, ETrue, CBackupTestFramework::EAllOpenForBackup, conn::EBackupBase
		},
		// Open Remote View and open a contact - Backup and Restore succeed when it completes and resources are freed
 		{
		(TText*) L"Multiple clients that create a remote view and open a contact(clients close)",
		1, 2, CBackupTestFramework::EOpenRemoteViewOneContactForEdit, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},
		{
		(TText*) L"Contacts test for .Ini file Backup and Restore(clients close",
		1, 1, CBackupTestFramework::EContactsIniFileTest, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupBase
		},

		//////////
		// Incremental Backup and Restore. Should always succeed.
		{
		(TText*) L"INCREMENTAL: Clients that keep CContactDatabase Open",
		3, 1, CBackupTestFramework::EIdleClient, ETrue, CBackupTestFramework::EAllOpenForBackup, conn::EBackupIncrement
		},
		{
		(TText*) L"INCREMENTAL: Client opens contact for edit, resources released",
		1, 1, CBackupTestFramework::EOpenOneContactForEdit, ETrue, CBackupTestFramework::EAllCloseForBackup, conn::EBackupIncrement
 		}
	};


CBackupTestFramework* CBackupTestFramework::NewLC()
	{
	CBackupTestFramework* self = new(ELeave) CBackupTestFramework();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
	
CBackupTestFramework::CBackupTestFramework() : CTimer(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}
	
	
void CBackupTestFramework::ConstructL()
	{
	CTimer::ConstructL();
	
    // Test server used for copying files around.    
    User::LeaveIfError(iTestServ.Connect());
    iBackupHelper = CBackupHelper::NewL(iTestServ);
	}


CBackupTestFramework::~CBackupTestFramework()
	{
	// cleanup from current test case
	CTimer::Cancel();

    iTestServ.Close();

	delete iBackupHelper;
	iContactClientSession.ResetAndDestroy();
	}
	
	
void CBackupTestFramework::DoBackupRestoreTestsL()
	{
	const TInt testCount = sizeof(BackupTestCases) / sizeof(CBackupTestFramework::TBackupRestoreTestCase);

	test.Printf(_L("Iterate through all %i Backup and Restore test cases"), testCount);

	for (TInt testIndex = 0; testIndex < testCount; ++testIndex)
		{
		iTestCase = &BackupTestCases[testIndex];
		BackupRestoreTestL();
		}
	}



// Method to setup the Use Case under test
// returns ETrue if TrequestStatus was used
TBool CBackupTestFramework::PrepareBackupClientTestCaseL(TRequestStatus& aStatus)
	{
	TBool asyncRequest = EFalse;

	// for every database we are required to open
	for (TInt dbCount = 1; (dbCount <= iTestCase->iNumDatabases) && (!asyncRequest); ++dbCount)
		{
		switch (dbCount)
			{
			case 1:
				asyncRequest = PrepareTestCaseL(KBackupDatabaseFile1(), aStatus);
				break; 

			case 2:
				asyncRequest = PrepareTestCaseL(KBackupDatabaseFile2(), aStatus);
				break;

			case 3:
				asyncRequest = PrepareTestCaseL(KBackupDatabaseFile3(), aStatus);
				break;

			default: // something is wrong with test code
				User::Invariant();
			}
		}

	return asyncRequest;
	}
	

// Method to implement the Use Case under test
TBool CBackupTestFramework::PrepareTestCaseL(const TDesC& aDatabaseName, TRequestStatus& aStatus)
	{
	TBool asyncRequest = EFalse;

	// at least one, possibly more, clients per database
	for (TInt clClient = 1; clClient <= iTestCase->iClientsPerDatabase; ++clClient)
		{
		// open database
				TBool isCloseForBackup = ETrue;

		switch(iTestCase->iCloseForBackup)
			{
			case EAllOpenForBackup:
				isCloseForBackup = EFalse;
				break;
			case EHalfCloseForBackup:
				if(clClient > iTestCase->iClientsPerDatabase/2)
					{
					isCloseForBackup = EFalse;
					}
				break;
			case EAllCloseForBackup:
			default:
				break;
			}

		CContactsClient* db = NewDatabaseClientL(aDatabaseName, isCloseForBackup);


		// specific test case behaviour
		switch (iTestCase->iActivity)
			{
			// activities that do not block backup / restore
			case EIdleClient:
			case ERestoreCorruptDbTest:
				// demonstrate database is open, read from it
				test.Next(_L("Read a contact from database"));
				db->ReadContactL();
				break;

			case EOpenLocalView:
				test.Next(_L("open contact Local View"));
				db->OpenLocalViewL();
				break;

			case EOpenRemoteView:
				test.Next(_L("open contact Remote View"));
				db->OpenRemoteViewL();
				break;

			// activities that can block backup / restore
			case EOpenOneContactForEdit:
				test.Next(_L("Open a contact from database"));
				db->OpenContactL(KOpenContactId1);
				break;

			case EOpenTwoContactsForEdit:
				test.Next(_L("Open 2 contacts from database"));
				db->OpenContactL(KOpenContactId1);
				db->OpenContactL(KOpenContactId2);
				break;

			case EContactsAsyncFind:
				test.Next(_L("Start Async Find on database"));
				db->StartAsyncFindL();
				break;

			case EContactsActiveCompact:
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
				test.Next(_L("Start Active Compact on database"));
				db->StartActiveCompactL();
#endif
				break;

			case EContactsActiveRecover:
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
				test.Next(_L("Start Active Recover on database"));
				db->StartActiveRecoverL();
#endif
				break;

			case EContactsIdleSorter:
				test.Next(_L("Start Contacts Idle Sorter"));
				db->StartIdleSorterL(aStatus);
				asyncRequest = ETrue;
				break;

			case EContactsIniFileTest:
				// demonstrate database is open, read from it
				test.Next(_L("Read a contact from database"));
				db->ReadContactL();
				break;
				
						case EOpenRemoteViewOneContactForEdit:
			    test.Next(_L("Open a contact from database & open contact Remote View"));
			    if(clClient == 1) //There are two clients for this DB, first opens a view, second edits a contact
			        {
			    	db->OpenRemoteViewL();
			        }
			    else
			        {
			    	db->OpenContactL(KOpenContactId1);
			        }
			    break;
			case EMultipleOpenOneContactForEdit:
			    test.Next(_L("Multiple clients, one closes contact item one leaves open"));
			    if (isCloseForBackup) //ETrue
			        {
			    	db->OpenContactL(KOpenContactId1);
			        }  
				else //EFalse
				    {
					db->OpenContactL(KOpenContactId2);
				    }
				    
				break;

			}
		}

	return asyncRequest;
	}



// Reconnect disconnected client after Backup/Restore
void CBackupTestFramework::ReconnectBackupClientTestCaseL()
	{
	// check that databases can be re-opened and are still useable
	const TInt numDatabases = iContactClientSession.Count();
	for (TInt i = 0; i < numDatabases; i++)
		{
		if (!iContactClientSession[i]->IsOpen())
			{
			test.Printf(_L("reopen database client # %i after backup/restore\n\r"), i + 1);
			iContactClientSession[i]->OpenDatabaseL();
			}
			
		// read from db
		test.Next(_L("Read a contact through database client"));
		iContactClientSession[i]->ReadContactL();
		}
	}


// test action if client didn't close database
void CBackupTestFramework::CheckConnectionAfterBackupForTestCaseL()
	{
	const TInt numDatabases = iContactClientSession.Count();

	// specific test case behaviour
	switch (iTestCase->iActivity)
		{
		// activities that do not block backup / restore
		case EIdleClient:
		case EOpenLocalView:
		case EOpenRemoteView:
		case EContactsIniFileTest:
			for (TInt i = 0; i < numDatabases; i++)
				{
				// read from each db
				test.Next(_L("Read a contact through database client"));
				iContactClientSession[i]->ReadContactL();
				}
			break;

		case ERestoreCorruptDbTest:
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
			for (TInt i = 0; i < numDatabases; i++)
				{
				// read from each db
				test.Next(_L("Read a contact through database client"));
				TRAPD(err, iContactClientSession[i]->ReadContactL());
				
				// The restore is complete. The corrupted db file was damaged
                // but should have been recovered after the restore completed. 
                // However, the contact doesn't exist in the db. If the 
                // corrupted db file was totally unrecognizable then the recover
                // wouldn't have been attempted and the db would be closed. This
                // would then return KErrNotReady.
				if((iTestStep == EPostRestore) &&
				    (iContactClientSession[i]->DatabaseName()==KBackupDatabaseFile1))
					{
					TEST_CONDITION(err==KErrNotFound);
					}
				else //others should not Leave, so propagate Leave if it occurs.
					{
					if(err != KErrNone)
						{
						User::Leave(err);
						}
					}
				}
#endif
			break;

		// activities that can block backup / restore
		case EOpenOneContactForEdit:
			for (TInt i = 0; i < numDatabases; i++)
				{
				test.Next(_L("Commit contact to database"));
				iContactClientSession[i]->CommitContactL(KOpenContactId1);
				}
			break;

		case EOpenTwoContactsForEdit:
			for (TInt i = 0; i < numDatabases; i++)
				{
				test.Next(_L("Commit 2 contacts to database"));
				iContactClientSession[i]->CommitContactL(KOpenContactId1);
				iContactClientSession[i]->CommitContactL(KOpenContactId2);
				}
			break;

		case EContactsAsyncFind:
			for (TInt i = 0; i < numDatabases; i++)
				{
				test.Next(_L("Complete Async Find on database"));
				iContactClientSession[i]->CompleteAsyncFindL();
				}
			break;

		case EContactsActiveCompact:
			for (TInt i = 0; i < numDatabases; i++)
				{
				test.Next(_L("Complete Active Compact on database"));
				iContactClientSession[i]->CompleteActiveCompactL();
				}
			break;

		case EContactsActiveRecover:
			for (TInt i = 0; i < numDatabases; i++)
				{
				test.Next(_L("Complete Active Recover on database"));
				iContactClientSession[i]->CompleteActiveRecoverL();
				}
			break;

		case EContactsIdleSorter:
			// nothing to do for this Use Case
			break;
		}
	}



// support methods for testing .Ini file Backup and Restore

// any test case action required immediately before Backup start notifiction
void CBackupTestFramework::PreBackupNotificationActionsL()
	{
	const TInt numDatabases = iContactClientSession.Count();

	// specific test case behaviour
	switch (iTestCase->iActivity)
		{
		case EIdleClient:
        case ERestoreCorruptDbTest:
		case EOpenLocalView:
		case EOpenRemoteView:
		case EOpenOneContactForEdit:
		case EOpenTwoContactsForEdit:
		case EContactsAsyncFind:
		case EContactsActiveCompact:
		case EContactsActiveRecover:
		case EContactsIdleSorter:
			// nothing to do for this Use Case
			break;

		case EContactsIniFileTest:
			// make change to Contacts .ini file that should be Backed-up
			for (TInt i = 0; i < numDatabases; i++)
				{
				// read from each db
				test.Next(_L("Set Speed Dial 1 for database"));
				iContactClientSession[i]->SetSpeedDialL(KSpeedDialContactId, KSpeedDialPositionOne);
				}
			break;
		}
	}


// any test case action required after Backup completion
void CBackupTestFramework::PostBackupActionsL()
	{
	const TInt numDatabases = iContactClientSession.Count();

	// specific test case behaviour
	switch (iTestCase->iActivity)
		{
		case EIdleClient:
        case ERestoreCorruptDbTest:
		case EOpenLocalView:
		case EOpenRemoteView:
		case EOpenOneContactForEdit:
		case EOpenTwoContactsForEdit:
		case EContactsAsyncFind:
		case EContactsActiveCompact:
		case EContactsActiveRecover:
		case EContactsIdleSorter:
		case EOpenRemoteViewOneContactForEdit:
		case EMultipleOpenOneContactForEdit:

			// nothing to do for this Use Case
			break;

		case EContactsIniFileTest:
			// check Contacts .ini is correct
			for (TInt i = 0; i < numDatabases; i++)
				{
				// read from each db
				test.Next(_L("Check Speed Dial 1 still set for database"));
				iContactClientSession[i]->CheckSpeedDialSetL(KSpeedDialContactId, KSpeedDialPositionOne);
				}
			break;
		}
	}


// any test case action required immediately before Restore start notifiction
void CBackupTestFramework::PreRestoreNotificationActionsL()
	{
	const TInt numDatabases = iContactClientSession.Count();

	// specific test case behaviour
	switch (iTestCase->iActivity)
		{
		case EIdleClient:
        case ERestoreCorruptDbTest:
		case EOpenLocalView:
		case EOpenRemoteView:
		case EOpenOneContactForEdit:
		case EOpenTwoContactsForEdit:
		case EContactsAsyncFind:
		case EContactsActiveCompact:
		case EContactsActiveRecover:
		case EContactsIdleSorter:
		case EOpenRemoteViewOneContactForEdit:
		case EMultipleOpenOneContactForEdit:

			// nothing to do for this Use Case
			break;

		case EContactsIniFileTest:
			// make change to Contacts .ini file before Restore
			for (TInt i = 0; i < numDatabases; i++)
				{
				// read from each db
				test.Next(_L("Remove Speed Dial 1 from database"));
				iContactClientSession[i]->RemoveSpeedDialL(KSpeedDialContactId, KSpeedDialPositionOne);
				}
			break;
		}
	}

// any test case action required after Restore completion
void CBackupTestFramework::PostRestoreActionsL()
	{
	const TInt numDatabases = iContactClientSession.Count();

	// specific test case behaviour
	switch (iTestCase->iActivity)
		{
		case EIdleClient:
        case ERestoreCorruptDbTest:
		case EOpenLocalView:
		case EOpenRemoteView:
		case EOpenOneContactForEdit:
		case EOpenTwoContactsForEdit:
		case EContactsAsyncFind:
		case EContactsActiveCompact:
		case EContactsActiveRecover:
		case EContactsIdleSorter:
		case EOpenRemoteViewOneContactForEdit:
		case EMultipleOpenOneContactForEdit:

			// nothing to do for this Use Case
			break;

		case EContactsIniFileTest:
			// check Restored Contacts .ini file is the same as was Backed-up
			for (TInt i = 0; i < numDatabases; i++)
				{
				// read from each db
				test.Next(_L("Check Restored Speed Dial 1 from database"));
				iContactClientSession[i]->CheckSpeedDialSetL(KSpeedDialContactId, KSpeedDialPositionOne);
				}
			break;
		}
	}




// Backup and Restore test with chosen Use Case
void CBackupTestFramework::BackupRestoreTestL()
	{
	// need to do type conversion in 2 steps
	TPtrC title(iTestCase->iTestTitle);
	test.Next(title);


	// start backup & restore tests in Active Scheduler
	iTestStep = EPreBackup;
	After(0);
	CActiveScheduler::Start();

	// ensure databases closed & release resources
	iContactClientSession.ResetAndDestroy();

	// any error from active scheduler?
	User::LeaveIfError(iTestError);
	}



void CBackupTestFramework::RunL()
	{
	TBool waitForAsyncRequest = EFalse;
	
	if (iStatus.Int() != KErrCancel)
		{
		switch (iTestStep)
			{
		// Backup test
		case EPreBackup:
			test.Start(_L("Backup test"));

			test.Start(_L("Prepare Backup test case, open database(s) etc..."));
			waitForAsyncRequest = PrepareBackupClientTestCaseL(iStatus);

			// reset count number of Backup notifications
			iClientNotifications = 0;
			iViewUnavailableCount = 0;
			break;
				
		case ENotifyBackupStart:
			// any test case action required immediately before Backup start notifiction
			PreBackupNotificationActionsL();

			if(conn::EBackupIncrement == iTestCase->iBackupIncType)
				{
				test.Next(_L("notify start of incremental backup"));
				iBackupHelper->StartBackupL(conn::EBURBackupPartial, conn::EBackupIncrement);
				}
			else
				{
				test.Next(_L("notify start of backup"));
				iBackupHelper->StartBackupL(conn::EBURBackupFull, conn::EBackupBase);
				}

			break;

		case ECheckBackupStartNotifications:
			if(conn::EBackupIncrement == iTestCase->iBackupIncType)
				{
				test.Next(_L("check number of incremental backup start notifications"));					
				}
			else
				{
				test.Next(_L("check number of backup start notifications"));	
				}
			TEST_CONDITION(iContactClientSession.Count() == iClientNotifications);

			// test case has views?
			if ((iTestCase->iActivity == EOpenLocalView) || (iTestCase->iActivity == EOpenRemoteView))
				{
				if (iTestCase->iCloseForBackup)
					{
					// Contacts clients close for Backup or Restore
					test.Next(_L("check not too many Contact View unavailable notifications"));
					TEST_CONDITION((iTestCase->iNumDatabases * iTestCase->iClientsPerDatabase) >= iViewUnavailableCount);
					}
				else
					{
					// Contacts clients open for Backup or Restore
					test.Next(_L("check the number of Contact View unavailable notifications"));
					TEST_CONDITION((iTestCase->iNumDatabases * iTestCase->iClientsPerDatabase) == iViewUnavailableCount);
					}
				}
			break;

		case EDoBackup:
			if(conn::EBackupIncrement == iTestCase->iBackupIncType)
				{
				test.Next(_L("attempt incremental backup of database(s)"));					
				}
			else
				{
				test.Next(_L("attempt backup of database(s)"));		
				}

			// Clients have not closed for Backup?
			if (iTestCase->iCloseForBackup != EAllCloseForBackup)
				{
				const TInt numDatabases = iContactClientSession.Count();

				// find open database sessions
				test.Next(_L("Check that new Read and Open.(Write) operations cannot start"));
				for (TInt i = 0; i < numDatabases; i++)
					{
					if (! iContactClientSession[i]->IsCloseForBackup())
					    {
					    test.Next(_L("check that database read is locked"));
					    TRAPD(error, iContactClientSession[i]->ReadContactL());
					    TEST_CONDITION(KErrLocked == error);

					    test.Next(_L("check that database write is locked"));
					    TRAP(error, iContactClientSession[i]->OpenContactL(KOpenContactId3));
					    TEST_CONDITION(KErrLocked == error);

					    test.Next(_L("check that phone number lookup (database read) is locked"));
					    TRAP(error, iContactClientSession[i]->FindPhoneNumberL());
					    TEST_CONDITION(KErrLocked == error);
					    }
					}
				}

			if(iTestCase->iActivity == EContactsIniFileTest)
				{
			    test.Next(_L("checking that backup of cntmodel.ini file succeeds"));
		 	    TEST_CONDITION(iBackupHelper->SimulateIniBackup());
				}

			if (iTestCase->iExpectedBackupSuccess)
				{
				test.Next(_L("checking that backup of databases succeeds"));
				TEST_CONDITION(iBackupHelper->SimulateDbBackup());
				}
			else
				{
				test.Next(_L("checking that backup of databases fails"));
				TEST_CONDITION(! iBackupHelper->SimulateDbBackup());
				}

			// reset count for end of backup notifications		
			iClientNotifications = 0;
			iViewReadyCount = 0;
			break;
			
		case ENotifyBackupEnd:
			test.Next(_L("notify end of backup"));
			iBackupHelper->EndBackupL();
			break;

		case ECheckBackupEndNotifications:
			// if database connections are kept open
			if (iTestCase->iCloseForBackup == EAllOpenForBackup)
				{
				test.Next(_L("check number of backup end notifications"));
				TEST_CONDITION(iContactClientSession.Count() == iClientNotifications);

				// test case kept views open?
				if ((iTestCase->iActivity == EOpenLocalView) || (iTestCase->iActivity == EOpenRemoteView))
					{
					test.Next(_L("check the number of Contact View ready notifications"));
					TEST_CONDITION((iTestCase->iNumDatabases * iTestCase->iClientsPerDatabase) == iViewReadyCount);
					}
				}
			break;

		case EPostBackup:
			test.Next(_L("post backup verify database(s) useable"));
			if (iTestCase->iCloseForBackup != EAllOpenForBackup)
				{
				ReconnectBackupClientTestCaseL();
				}
			else
				{
				CheckConnectionAfterBackupForTestCaseL();
				}

			// any test case action required after Backup completion
			PostBackupActionsL();
			break;
			
		case EFinishedBackup:
			test.Next(_L("Cleanup all client resources for test case"));
			// cleanup after backup test
			iContactClientSession.ResetAndDestroy();
			test.End();
			break;

		// Successful Restore test
		case EPreRestore:
			test.Next(_L("Restore test"));

			test.Start(_L("pre-restore access database(s)"));
			waitForAsyncRequest = PrepareBackupClientTestCaseL(iStatus);

			// reset count of Restore notifications
			iClientNotifications = 0;
			iViewUnavailableCount = 0;
			break;
			
		case ENotifyRestoreStart:
			// any test case action required immediately before Restore start notifiction
			PreRestoreNotificationActionsL();

			test.Next(_L("notify start of restore"));
			iBackupHelper->StartRestoreL();
			break;

		case ECheckRestoreStartNotifications:
			test.Next(_L("check number of restore start notifications"));
			TEST_CONDITION(iContactClientSession.Count() == iClientNotifications);

			// test case has views?
			if ((iTestCase->iActivity == EOpenLocalView) || (iTestCase->iActivity == EOpenRemoteView))
				{
				if (iTestCase->iCloseForBackup == EAllOpenForBackup)
					{
					// Contacts clients open for Backup or Restore
					test.Next(_L("check the number of Contact View unavailable notifications"));
					TEST_CONDITION((iTestCase->iNumDatabases * iTestCase->iClientsPerDatabase) == iViewUnavailableCount);
					}
				else
					{
					// Contacts clients close for Backup or Restore
					test.Next(_L("check not too many Contact View unavailable notifications"));
					TEST_CONDITION((iTestCase->iNumDatabases * iTestCase->iClientsPerDatabase) >= iViewUnavailableCount);
					}
				}
			break;

		case EDoRestore:
			test.Next(_L("attempt restore of database(s)"));

			// Clients have not closed for Backup?
				if (iTestCase->iCloseForBackup == EAllOpenForBackup)

				{
				const TInt numDatabases = iContactClientSession.Count();

				// find all open database sessions
				for (TInt i = 0; i < numDatabases; i++)
					{
					test.Next(_L("check that database read is Locked"));
					TRAPD(error, iContactClientSession[i]->ReadContactL());
					TEST_CONDITION(KErrLocked == error);

					test.Next(_L("check that database write is locked"));
					TRAP(error, iContactClientSession[i]->OpenContactL(KOpenContactId3));
					TEST_CONDITION(KErrLocked == error);

				    test.Next(_L("check that phone number lookup (database read) is locked"));
				    TRAP(error, iContactClientSession[i]->FindPhoneNumberL());
				    TEST_CONDITION(KErrLocked == error);
					}
				}
 			if(iTestCase->iActivity == EContactsIniFileTest)
				{
			    test.Next(_L("checking that restore of ini file succeeds"));
			    TEST_CONDITION(iBackupHelper->SimulateIniRestore());
				}

			if (iTestCase->iExpectedBackupSuccess)
				{
				if (iTestCase->iActivity == ERestoreCorruptDbTest)
					{
					test.Next(_L("checking that restore of databases fails on the corrupted one"));
					TEST_CONDITION(iBackupHelper->SimulateDamagedDbRestore());
					}
				else
					{
    				test.Next(_L("checking that restore of databases succeeds"));
    				TEST_CONDITION(iBackupHelper->SimulateDbRestore());
                    }
				}
			else
				{
				test.Next(_L("checking that restore of databases fails"));
				TEST_CONDITION(! iBackupHelper->SimulateDbRestore());
				}

			// reset count for end of Restore notifications
			iClientNotifications = 0;
			iViewReadyCount = 0;
			break;
			
		case ENotifyRestoreEnd:
			test.Next(_L("notify end of restore"));
			iBackupHelper->EndRestoreL();
			break;

		case ECheckRestoreEndNotifications:
			// if database connections are kept open
			if (iTestCase->iCloseForBackup == EAllOpenForBackup)
				{
				test.Next(_L("check number of restore end notifications"));
				TEST_CONDITION(iContactClientSession.Count() == iClientNotifications);

				// test case kept views open?
				if ((iTestCase->iActivity == EOpenLocalView) || (iTestCase->iActivity == EOpenRemoteView))
					{
					test.Next(_L("check the number of Contact View ready notifications"));
					TEST_CONDITION((iTestCase->iNumDatabases * iTestCase->iClientsPerDatabase) == iViewReadyCount);
					}
				}
			break;

		case EPostRestore:
			test.Next(_L("post-restore verify database(s) useable"));
			if (iTestCase->iCloseForBackup)
				{
				ReconnectBackupClientTestCaseL();
				}
			else
				{
				CheckConnectionAfterBackupForTestCaseL();
				}

			// any test case action required after Restore completion
			PostRestoreActionsL();
			break;

		case EFinishedRestore:
			test.Next(_L("Cleanup all client resources for test case"));
			// cleanup after backup test
			iContactClientSession.ResetAndDestroy();
			// end x.y.z restore test
			test.End();
			// end x.y test case
			test.End();

            // overwrite corrupted database, other tests don't use it
            if(iTestCase->iActivity == ERestoreCorruptDbTest)
                {			
                // Copy a non-corrupted db over the corrupted one for other tests to use.
                iBackupHelper->SimulateDbRestore();
                }

			// All Done
			CActiveScheduler::Stop();
			return;
			}


		// no pause required between steps?
		if (waitForAsyncRequest)
			// waits for an asynchronous request to complete
			{
			SetActive();
			}
		else if ((iTestStep == EPreBackup) || (iTestStep == EPreRestore))
			{
			// start of Backp or Restore test cycle - allow contacts client to settle (e.g. contacts view to sort)
			test.Printf(_L("Wait %d ms to allow contacts client to settle (e.g. contacts view to sort)\n"), KWaitForClientToSettle);
			After(KWaitForClientToSettle);
			}
		else if ((iTestStep == EPostBackup) || (iTestStep == EPostRestore))
			{
			// end of Backup or Restore test cycle - allow all system components to settle
			test.Printf(_L("Wait %d ms to allow all system components to settle\n"), KWaitForSystemToSettle);
			After(KWaitForSystemToSettle);
			}
		else if ((iTestStep == ENotifyBackupStart) || (iTestStep == ENotifyBackupEnd) || (iTestStep == ENotifyRestoreStart) || (iTestStep == ENotifyRestoreEnd))
			{
			// allow notification to propagate
			test.Printf(_L("Wait %d ms to allow notification to propagate\n"), KWaitForBackupNotification);
			After(KWaitForBackupNotification);
			}
		else
			{
			// minimum wait required
			After(1);
			}
			
		// next step
		iTestStep = (TBackupRestoreSteps) (iTestStep + 1);
		}
	}


TInt CBackupTestFramework::RunError(TInt aError)
	{
	// RunL is verbose and prints details of each step
	// so just print error info
	test.Printf(_L("Backup and Restore test error %d (at step %d)\n\r"), aError, iTestStep);
	
	// and state info
	const TInt clientCount = iContactClientSession.Count();
	for (TInt j = 0; j < clientCount; j++)
		{
		test.Printf(_L("client %i, IsOpen = %i, name \"%S\"\n\r"), 
			j, iContactClientSession[j]->IsOpen(), &iContactClientSession[j]->DatabaseName());
		}
	
	// propagate error
	iTestError = aError;
	
	CActiveScheduler::Stop();
	return KErrNone;
	}


/////////////////

CContactsClient* CBackupTestFramework::NewDatabaseClientL(const TDesC& aDatabaseName, TBool aCloseForBackup)
	{
	test.Printf(_L("open database \"%S\"\n\r"), &aDatabaseName);

	// open a Contacts Model client for test case
	CContactsClient* newClient = CContactsClient::NewLC(aDatabaseName, this, *this, aCloseForBackup);
	CleanupStack::Check(newClient);

	// remember for later cleanup
	iContactClientSession.AppendL(newClient);
	CleanupStack::Pop(newClient);
	return newClient;
	}


void CBackupTestFramework::ContactsBackupRestoreObserver(CContactsClient& aContactClient, const TBool aRestore, const TBool aStart)
	{
	iClientNotifications ++;
	
	// test case should respond to Backup / Restore notification ?
	if (aStart && aContactClient.IsCloseForBackup())
		{
		test.Next(aRestore ? _L("Restore start notification - closing client") : _L("Backup start notification - closing client"));
		aContactClient.CloseViews();
		aContactClient.StartActiveClose();	
		}
	}



/*
 Contact Views become EUnavailable when Backup or Restore start,
 if the View is kept open it becomes EReady again afterwards.
 */
void CBackupTestFramework::HandleContactViewEvent(const CContactViewBase& /*aView*/,const TContactViewEvent& aEvent)
	{
	if (aEvent.iEventType == TContactViewEvent::EUnavailable)
		{
		iViewUnavailableCount++;
		// reset Ready count for end of Backup or Restore
		iViewReadyCount = 0;
		}
	else if (aEvent.iEventType == TContactViewEvent::EReady)
		{
		iViewReadyCount++;
		}
	}




////////////////////////////////////////////////////////////


LOCAL_C void TestCondition(TBool aCondition, TInt aLineNumber)
	{
	// if the test is about to fail, cleanup files first
	if (!aCondition)
		{
		CBackupTestConductor::DeleteDatabases();
		}
	test.operator()(aCondition, aLineNumber);
	}


LOCAL_C void DoMainL(CActiveScheduler* scheduler)
    {
	CActiveScheduler::Install(scheduler);

	CBackupTestConductor* tester = CBackupTestConductor::NewL();
	CleanupStack::PushL(tester);

 	tester->RunTestsL();
	CleanupStack::PopAndDestroy(tester); 
	}
/**

@SYMTestCaseID     PIM-T-CNT-BACKUP-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(_L("SYMTestCaseID:PIM-T-CNT-BACKUP-0001 Contacts Backup and Restore"));
	CActiveScheduler* scheduler = new CActiveScheduler;
	test(scheduler != NULL);
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	User::LeaveIfError(TheFsSession.Connect());

	test.Printf(_L("SECURE_DATA mode\r\n"));
	User::After(KWaitForSystemToSettle);

	TRAPD(ret,DoMainL(scheduler));
	if (ret != KErrNone)
		{
		test.Printf(_L("Something in the test left with error code %d\n"), ret);
		}
	TEST_CONDITION(ret == KErrNone);

	TheFsSession.Close();
	delete theCleanup;	
	delete scheduler;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
    }
