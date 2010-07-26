/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/


#include "cinifilemanager.h"
#include "ccntdbmanagercontroller.h"
#include "ccntbackuprestoreagent.h"
#include "cntcurrentitemmap.h"
#include "cntspeeddials.h"

// uncomment this for debug logging
//#define INIFILE_DEBUG_LOG



const TUid KContactsIniUid = { 0x10009099 };
const TUid KContactsCurrentDatabaseUid = { 0x1000909A };
const TUid KUidContactsDefaultDatabaseLocation = { 0x1020380A };

const TInt KIniFileSaveDelay = 100000;

/**
First phase constructor.
*/
CIniFileManager::CIniFileManager(RFs& aFs, CCntDbManagerController& aDbMgrCtrlr)
	: CTimer(CActive::EPriorityStandard), iFs(aFs), iDbMgrCtrlr(aDbMgrCtrlr), iBackupFlag(ENoSaveRestore)
	{
	CActiveScheduler::Add(this);
	}


/**
Object factory method.
*/
CIniFileManager* CIniFileManager::NewL(RFs& aFs, CCntDbManagerController& aDbMgrCtrlr)
	{
	CIniFileManager* IniFileManager = new (ELeave) CIniFileManager(aFs, aDbMgrCtrlr);
	CleanupStack::PushL(IniFileManager);
	IniFileManager->ConstructL();
	CleanupStack::Pop(IniFileManager);
	return IniFileManager;
	}


/**
Second phase constructor.
*/
void CIniFileManager::ConstructL()
	{
	CTimer::ConstructL();
	iSpeedDialManager = new(ELeave) CCntServerSpeedDialManager();
	iCurrentItemMap = new(ELeave) CCntServerCurrentItemMap();
	iCurrentDb = KNullDesC;
	// Restore the current database if possible. If this fails, the 
	// current database is set to KNullDesC (i.e. zero'd)
	RequestRestoreIniFileSettingsL();
	}


CIniFileManager::~CIniFileManager()
	{
	if (iFs.Handle()!=0)
		{
		TRAP_IGNORE(SaveIniFileSettingsL(ESaveAll,EFalse));
		}
	delete iSpeedDialManager;
	delete iCurrentItemMap;
	}


/**
Update/create a map entry for the current item associated with a database file.
*/	
void CIniFileManager::SetCurrentItemForDatabaseL(const TDesC& aDatabase, TContactItemId aContactId)
	{
	// Does the specified database contain a current item already?
	TInt index = KErrNotFound;
	if	(iCurrentItemMap->EntryAvailable(aDatabase, index))
		{
		// Yes, already exists, so update it....
		iCurrentItemMap->UpdateEntryL(index, aContactId); 
		}
	else
		{
		// No, so create a new entry to represent this database
		iCurrentItemMap->AddEntryL(aDatabase, aContactId);
		}
	}

		
void CIniFileManager::RemoveCurrentItemL(const TDesC& aDatabase)
	{
	SetCurrentItemForDatabaseL(aDatabase, KNullContactId);
	}


void CIniFileManager::SetCurrentItemL(const TDesC& aDatabase, TContactItemId aNewCurrentItem)
	{
	SetCurrentItemForDatabaseL(aDatabase, aNewCurrentItem);
	}


/**
Returns the current item associated with the specified database.
*/	
TContactItemId CIniFileManager::CurrentItem(const TDesC& aDatabase) const
	{
	return iCurrentItemMap->CurrentItem(aDatabase);
	}


const TDesC& CIniFileManager::CurrentDb() const
	{ 
	return iCurrentDb; 
	}

	
void CIniFileManager::SetCurrentDb(const TDesC& aDb)
	{ 
	iCurrentDb = aDb; 
	}


CCntServerSpeedDialManager& CIniFileManager::SpeedDialManager()
	{ 
	return *iSpeedDialManager;
	}
	

void CIniFileManager::ScheduleSaveIniFileSettings(TInt aSaveFlags, TBool aReplace)
	{
	// make sure all requested writes are saved
	iSaveType |= aSaveFlags;
	iReplace = aReplace;

	// make sure change isn't due to Internalize
	if (iBackupFlag != EIsRestoring)
		{
		iBackupFlag = ERequestSave;
		//iNumberOfAttemptedRetries = 0;

		// set the time to RunL if not already set to run
		// check that no Backup or Restore is in progress, 
		// For Backup And Restore
		if (!(iDbMgrCtrlr.BackupRestoreAgent().BackupInProgress()) &&
			!(iDbMgrCtrlr.BackupRestoreAgent().RestoreInProgress()))
			{
			if (!IsActive())
				{
#ifdef INIFILE_DEBUG_LOG
				RDebug::Print(_L("\n[CNTMODEL] CIniFileManager::ScheduleSaveIniFileSettings(aSaveFlags = %i, aReplace %i)\r\n"),aSaveFlags, aReplace);
#endif
				After(KIniFileSaveDelay);	
				}
			}
		}
	}


void CIniFileManager::SaveIniFileSettingsIfRequestedL()
	{
	if (iBackupFlag == ERequestSave)
		{
		// Don't RunL
		Cancel();

		SaveIniFileSettingsL(iSaveType, iReplace);
		iBackupFlag = ENoSaveRestore;
		}
	}


void CIniFileManager::RequestRestoreIniFileSettingsL()
	{
	if (!(iDbMgrCtrlr.BackupRestoreAgent().RestoreInProgress()))
		{
		TInt ret = StartRestoreIniFileSettings();
		switch (ret)
			{
		case KErrNone:
			break;
		// If file does not exist create ini file immediately.
		case KErrPathNotFound:
			SaveIniFileSettingsL(ESaveAll, EFalse);
			break;
		// If file is damaged replace ini file.
		case KErrEof:
			SaveIniFileSettingsL(ESaveAll, ETrue);
			break;
		default:
#if	defined(_DEBUG)
			User::Leave(KErrNotSupported);
#endif
			break;  //Would not build in UREL without it
			}
		}
	}

	
TInt CIniFileManager::StartRestoreIniFileSettings()
	{
	iBackupFlag = EIsRestoring;
	TInt err;
	TRAP(err,RestoreIniFileSettingsL());
	iBackupFlag = ENoSaveRestore;
	return err ;
	}


void CIniFileManager::GetIniFileNameL(TDes& aFileName, TBool aIncPrivatePath)
	{
	if (aIncPrivatePath)
		{
		User::LeaveIfError(iFs.PrivatePath(aFileName));
		}
	// Drive name goes before the path.
	aFileName.Insert(0,TDriveUnit(EDriveC).Name());
	// Filename goes after the path.
	aFileName.Append(KContactsIniFileName);
	}


/**
Restore server-wide settings from the contacts ini file.
*/
void CIniFileManager::RestoreIniFileSettingsL()
	{
	// Initial location for Default database
	SetDefaultDatabaseDrive(TDriveUnit(EDriveC),EFalse);

	TFileName iniFile;
	GetIniFileNameL(iniFile);
	CDictionaryFileStore* iniStore = IniFileStoreLC(iniFile);
	
	// Load settings
	iCurrentItemMap->RestoreL(*iniStore);
	iSpeedDialManager->RestoreL(*iniStore);

	// Current database
	RestoreCurrentDatabaseL(*iniStore);

	// Default database location
	RestoreDefaultDbDriveL(*iniStore);

	// Tidy up
	CleanupStack::PopAndDestroy(iniStore);
	}

	
void CIniFileManager::SetDefaultDatabaseDrive(TDriveUnit aDriveUnit, TBool aDriveSet)
	{
	iDefaultDriveUnit = aDriveUnit;
	iDatabaseDriveSet = aDriveSet;
	}

	
/**
Restore the current database filename from the ini file.
*/
void CIniFileManager::RestoreCurrentDatabaseL(CDictionaryFileStore& aStore)
	{
	if	(aStore.IsPresentL(KContactsCurrentDatabaseUid))
		{
		RDictionaryReadStream stream;
		stream.OpenLC(aStore, KContactsCurrentDatabaseUid);
		stream >> iCurrentDb;
		CleanupStack::PopAndDestroy(); // stream
		}
	}

	
/**
Restore the current database filename from the ini file.
*/
void CIniFileManager::RestoreDefaultDbDriveL(CDictionaryFileStore& aStore)
	{
	if	(aStore.IsPresentL(KUidContactsDefaultDatabaseLocation))
		{
		TDriveUnit driveUnit;
		RDictionaryReadStream stream;

		stream.OpenLC(aStore, KUidContactsDefaultDatabaseLocation);
		driveUnit = stream.ReadInt32L();
		CleanupStack::PopAndDestroy(); // stream

		SetDefaultDatabaseDrive(driveUnit);
		iDbMgrCtrlr.RestoreDatabaseDrive(driveUnit); // restore drive setting in DBManagerController
		}
	}


void CIniFileManager::RetryStoreOperation()
	{
	if (iBackupFlag == ERequestSave && !IsActive())
		{
		// can RunL now
		After(0);
		}
	}


void CIniFileManager::RunL()
	{
	const TInt error = iStatus.Int();

#ifdef INIFILE_DEBUG_LOG
	RDebug::Print(_L("[CNTMODEL] CIniFileManager::RunL() - error %i, iBackupFlag %i\r\n"), error, iBackupFlag);
#endif

	// operation isn't cancelled
	if	(error == KErrNone && iBackupFlag == ERequestSave)
		{
		SaveIniFileSettingsL(iSaveType, iReplace);
		iBackupFlag = ENoSaveRestore;
		}
	}


TInt CIniFileManager::RunError(TInt aError)
   	{
#ifdef INIFILE_DEBUG_LOG
	RDebug::Print(_L("[CNTMODEL] CIniFileManager::RunError(aError = %i)\r\n"), aError);
#else
	aError = aError; // remove compiler warning for unreferenced parameter
#endif
	// cannot do anything about the error
	// - unless it occurred during a Backup, when it will already be retried

	return KErrNone;
	}

	
/**
Save server-wide settings to the contacts model ini file.
*/
void CIniFileManager::SaveIniFileSettingsL(TInt aSaveFlags, TBool aReplace)
	{
	TFileName	iniFile;
	GetIniFileNameL(iniFile);
	iFs.MkDirAll(iniFile);

	if (aReplace)
		{
		iFs.Delete(iniFile);
		}
	
	// Get the ini store
	CDictionaryFileStore* iniStore = IniFileStoreLC(iniFile);

	// Save settings
	if ((aSaveFlags & ESaveCurrentItem) && iCurrentItemMap)
		{
		iCurrentItemMap->StoreL(*iniStore);
		}

	if ((aSaveFlags & ESaveSpeedDials) && iSpeedDialManager)
		{
		iSpeedDialManager->StoreL(*iniStore);
		}

	// Current database
	if (aSaveFlags & ESaveCurrentDatabase)
		{
		StoreCurrentDatabaseL(*iniStore);
		}

	// Default database location
	if (aSaveFlags & ESaveDefaultDbDrive)
		{
		StoreDefaultDbDriveL(*iniStore);
		}

	// Commit
	iniStore->CommitL();

	// Tidy up
	CleanupStack::PopAndDestroy(); // iniStore
	}


/**
Return a pointer to the contacts ini file.
*/
CDictionaryFileStore* CIniFileManager::IniFileStoreLC(TFileName& aIniFile)
	{
	return CDictionaryFileStore::OpenLC(iFs, aIniFile, KContactsIniUid);
	}

	
/**
Save the current database filename to the ini file.
*/
void CIniFileManager::StoreCurrentDatabaseL(CDictionaryFileStore& aStore)
	{
	RDictionaryWriteStream stream;
	stream.AssignLC(aStore, KContactsCurrentDatabaseUid);
	stream << iCurrentDb;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	}


/**
Save the default database drive to the ini file.
*/
void CIniFileManager::StoreDefaultDbDriveL(CDictionaryFileStore& aStore)
	{
	if (iDatabaseDriveSet)
		{
		RDictionaryWriteStream stream;
		stream.AssignLC(aStore, KUidContactsDefaultDatabaseLocation);
		stream.WriteInt32L(iDefaultDriveUnit);
		stream.CommitL();
		CleanupStack::PopAndDestroy(); // stream
		}
	}


TBool& CIniFileManager::DatabaseDriveSet()
	{
	return iDatabaseDriveSet;
	}
