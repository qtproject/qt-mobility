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


#include <e32cmn.h>
#include <bautils.h>

#include "persistencelayer.h"
#include "ccntdbmanagercontroller.h"
#include "ccntrequest.h"
#include "ccntipccodes.h" // for kcntmaxfilepath.
#include "ccntbackuprestoreagent.h"
#include "cinifilemanager.h"
#include "ccntpermanentdata.h"

_LIT(KSqLiteFilePrefix, "SQLite__");

/**
Object factory method.

@param aServerType 	ETransientServer if the Server is transient,
ENonTransientServer if the Server is not transient.

@return CCntDbManagerController instance.
*/
CCntDbManagerController* CCntDbManagerController::NewLC(TCntServerType aServerType)
	{
	CCntDbManagerController* ctrl = new (ELeave) CCntDbManagerController();
	CleanupStack::PushL(ctrl);
	ctrl->ConstructL(aServerType);
	return ctrl;
	}

/**
Second phase constructor.

@param aServerType 	ETransientServer if the Server is transient,
ENonTransientServer if the Server is not transient.
*/
void CCntDbManagerController::ConstructL(TCntServerType aServerType)
	{
	User::LeaveIfError(iFs.Connect());
	iBackupRestoreAgent = CCntBackupRestoreAgent::NewL(iFs, *this);
	iIniFileManager = CIniFileManager::NewL(iFs, *this);
	
	// Controller requires one Persistence Layer instance.
	iPersistenceLayer = CPersistenceLayer::NewLC(iFs);
	CleanupStack::Pop(iPersistenceLayer);

	// Create a backup registration file for the Contacts Model initialisation
	// file.
	TFileName iniFile;
	iIniFileManager->GetIniFileNameL(iniFile, EFalse);
	iBackupRestoreAgent->CreateBackupRegistrationFileL(iniFile);
	
	// Only now safe to start the Backup/Restore Agent since it relies on the
	// iIniFileManager object being fully constructed in the case where, within
	// the StartL() method, the value of the Backup/Restore property indicates
	// either a Backup or Restore is in progress.
	iBackupRestoreAgent->StartL();
	
	// Start the pre-creation of the default application data:
	// Open the default file and create the default remote
	// view(s) so they are ready for use by client applications.
	if (aServerType == ENonTransientServer)
		{
		iPermanentData = CCntPermanentData::NewL(*this);
		// If the permanent data creation step leaves, it should not
		// cause the server to panic. Instead, TRAP it and ignore
		// it - it is not fatal to normal server operation anyway.
		TRAP_IGNORE(iPermanentData->StartDataCreationL());
		}

	// Retrieve the list of contact databases on the C: drive.
	MLplContactsFile& contactsFile = iPersistenceLayer->ContactsFileL();
	TDriveUnit cDrive(EDriveC);
	CDesCArray* list(contactsFile.ListL(&cDrive)); 
	CleanupStack::PushL(list);
	
	// Check that each database has a back up registration XML file.
	// If not, create one.  
	const TInt KListLen(list->Count());
	for (TInt i = 0; i < KListLen; ++i)
		{
		TFileName dbName((*list)[i]);
		HBufC* backupFileNameBuf = iBackupRestoreAgent->
			CreateBackupRegistrationFileNameLC(dbName.Mid(cDrive.Name().Length()));
		TFileName backupFileName;
		backupFileName.Append(cDrive.Name());
		backupFileName.Append(*backupFileNameBuf);
		CleanupStack::PopAndDestroy(backupFileNameBuf);
		if (!BaflUtils::FileExists(iFs, backupFileName))
			{
			TRAP_IGNORE(iBackupRestoreAgent->CreateBackupRegistrationFileL(dbName));
			}
		}
	CleanupStack::PopAndDestroy(list);		
	}


CCntDbManagerController::CCntDbManagerController()
	: iDefaultDriveUnit(EDriveC)
	{
	}


CCntDbManagerController::~CCntDbManagerController()
	{
	delete iPermanentData;
	iManagers.ResetAndDestroy();
	delete iPersistenceLayer;
	delete iIniFileManager;
	delete iBackupRestoreAgent;
	iFs.Close();
	}


/**
Return a CCntDbManager for an existing Contacts database.  Creates a
CCntDbManager if no session has the file open.  The Contacts database must
exist.

@param aCntFile Contacts database filename.
@param aMode Indicates whether Contacts database is to be opened, created or
replaced.

@return CCntDbManager instance.
*/
CCntDbManager* CCntDbManagerController::GetDbManagerL(TDes& aCntFile, TCntFileMode aMode)
 	{
#ifdef __PROFILE__
	TTime now;
	now.UniversalTime();
#endif

 	// If a Backup or Restore is in progress disallow creation of manager and
 	// hence any access to database.  This is replicated behaviour from Contacts
 	// Model 1.
 	if (iBackupRestoreAgent->RestoreInProgress() || iBackupRestoreAgent->BackupInProgress())
 		{
 		User::Leave(KErrLocked);
 		}
 		
	// Deal with default Contacts database file (indicated by empty descriptor).
	if(aCntFile.Length() == 0)
		{
		DefaultDatabaseL(aCntFile);
		}

	// Free it up for a replace/create operation if 
	// permanent data is only thing holding it open.
	if (iPermanentData && (aMode == ECntFileReplace))
		{
		CCntDbManager* manager = DbManagerL(aCntFile);
		if (manager && iPermanentData->IsOnlyClientOfDbManager(manager))
			{
			iPermanentData->ReleaseDbManagerL(manager);
			}
		}

	CCntDbManager* theManager = DbManagerL(aCntFile);
	
	if (theManager != NULL) // Existing DbManager is found.
		{
		// If creating the file, leave because it is already exists.
		if (aMode == ECntFileCreate)
			{
			User::Leave(KErrAlreadyExists);
			}
		// If replacing the file, leave because it is currently in use.
		if (aMode == ECntFileReplace)
			{
			User::Leave(KErrInUse);
			}
		}
	else // The file is not opened and DbManager doesn't exist.
		{
		// Create new CCntDbManager.
		theManager = CCntDbManager::NewLC(iFs, aCntFile, aMode, *iBackupRestoreAgent, *iIniFileManager);	
		// Add to the list of managers.
		iManagers.AppendL(theManager);
		CleanupStack::Pop(theManager);
		}

	// Increase the session count.
	theManager->AddSession();

#ifdef __PROFILE__
	TTime now2;
	now2.UniversalTime();
	
	RDebug::Print(_L("[CNTMODEL] Spend %d microseconds in CCntDbManagerController::GetDbManagerL \n"), now2.MicroSecondsFrom(now).Int64());
#endif

	return theManager;
	}

	
/**
Find an existing manager for an opened Contacts database file.

@param aCntFile Contacts database filename.

@return CCntDbManager instance if the manager exists for aCntFile otherwise
returns NULL.
*/
CCntDbManager* CCntDbManagerController::DbManagerL(const TDesC& aCntFile) const
	{
	CCntDbManager* manager = NULL;
	const TInt index = FindExistingFileManager(aCntFile);
	if (index != KErrNotFound)
		{
		manager =  iManagers[index];
		}
	return manager;
	}


/**
Delete an existing Contacts database file.

@param aCntFile Contacts database filename.
*/
void CCntDbManagerController::DeleteDatabaseL(TDes& aCntFile) const
	{
	// Deal with default Contacts database file (indicated by empty descriptor).
	if(aCntFile.Length() == 0)
		{
		DefaultDatabaseL(aCntFile); // Get the default database name.
		}
		
	// Release the internal reference to the manager if the internal
	// permanent data is the only object holding it open.
	if (iPermanentData)
		{
		CCntDbManager* manager = DbManagerL(aCntFile);
		if (manager && iPermanentData->IsOnlyClientOfDbManager(manager))
			{
			iPermanentData->ReleaseDbManagerL(manager);
			}
		}
	
	// Make sure there is not an existing CCntDbManager session.
	if(FindExistingFileManager(aCntFile) != KErrNotFound)
		{
		User::Leave(KErrInUse);
		}
	else
		{
		// Delete the backup registration file associated with the Contacts
		// database file.
		iBackupRestoreAgent->DeleteBackupRegistrationFileL(aCntFile);
		// Call the Persistence Layer to delete the Contacts database file.
		iPersistenceLayer->ContactsFileL().DeleteL(aCntFile);
		}
	}


/**
Return the filename of the current default Contact database file.

@param aCntFile On return holds current default Contacts database filename.
*/
void CCntDbManagerController::DefaultDatabaseL(TDes& aCntFile) const
	{

	if ((iDefaultDriveUnit.Name().Length() + KContactsBaseFilename().Length()) > aCntFile.MaxLength())
		{
		User::Leave(KErrOverflow);		
		}
	aCntFile.Copy(iDefaultDriveUnit.Name());
	aCntFile.Append(KContactsBaseFilename);
	}


/**
Return the current default Contacts database drive.

@param aDrive On return holds the current default Contacts database drive.
*/
void CCntDbManagerController::DatabaseDrive(TDriveUnit& aDrive) const
	{
	aDrive = iDefaultDriveUnit;
	}


/**
Set the current default Contacts database drive and optionally moves the
default Contacts database from the current default drive to the new drive.

@param aDriveUnit New current default Contacts database drive.
@param aCopy ETrue if default Contacts database should be moved to new drive.
*/
void CCntDbManagerController::SetDatabaseDriveL(TDriveUnit aDriveUnit, TBool aCopy)
	{
	if (aDriveUnit == iDefaultDriveUnit) // Same drive?
		{ 
		aCopy = EFalse;
		if (iIniFileManager->DatabaseDriveSet())
			{
			return;
			}
		}

	// Old drive name.
	TDriveName oldDrv = iDefaultDriveUnit.Name();
	// New drive name.
	TDriveName newDrv = aDriveUnit.Name();	
	TBuf<KCntMaxFilePath> oldPath;	
	User::LeaveIfError(iFs.PrivatePath(oldPath));
	// Drive name goes before the path.
	oldPath.Insert(0,oldDrv);
	// Filename goes after the path.
	oldPath.Append(KSqLiteFilePrefix);
	oldPath.Append(KContactsBaseFilename);
	
	TBuf<KCntMaxFilePath> newPath;	
	User::LeaveIfError(iFs.PrivatePath(newPath));
	// Drive name goes before the path.
	newPath.Insert(0,newDrv);
	// Filename goes after the path.
	newPath.Append(KSqLiteFilePrefix);
	newPath.Append(KContactsBaseFilename);

	// Move the old file to the new drive location.
	CFileMan* fileManager = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileManager);
	// The new file will be overwritten if it exists.  If the file or the path
	// doesn't exist then it will be created.
	if (aCopy)
		{
		User::LeaveIfError(fileManager->Copy(oldPath, newPath, CFileMan::ERecurse | CFileMan::EOverWrite ));	
		}

	// Change the default drive to the specified drive.
	iDefaultDriveUnit = aDriveUnit;

	// Save to Contacts initialisation file.
	iIniFileManager->SetDefaultDatabaseDrive(iDefaultDriveUnit, ETrue);
	iIniFileManager->ScheduleSaveIniFileSettings(CIniFileManager::ESaveDefaultDbDrive);

	// Remove the old file.
	if (aCopy)
		{
		(void)fileManager->Delete(oldPath);
		}		
	CleanupStack::PopAndDestroy(fileManager);
	}

/**
Return the restored Contacts database drive.

@param aDrive On return holds the Contacts database drive restored from CntModel.ini.
*/
void CCntDbManagerController::RestoreDatabaseDrive(TDriveUnit& aDrive)
	{
	iDefaultDriveUnit = aDrive;
	}

/**
Return whether or not the named or default (empty descriptor) Contacts database
exists.

@param aCntFile Contacts database filename.

@return ETrue if Contacts database exists, EFalse otherwise.
*/
TBool CCntDbManagerController::DatabaseExistsL(const TDesC& aCntFile) const
	{
	if(aCntFile==KNullDesC)
		{
		TBuf<KCntMaxFilePath> path;
		DefaultDatabaseL(path);
		return (iPersistenceLayer->ContactsFileL().DatabaseExistsL(path));
		}
	return (iPersistenceLayer->ContactsFileL().DatabaseExistsL(aCntFile));		
	}


/**
Return a list of Contacts databases found on drive aDriveUnit.

@param aListBuffer Client supplied flat buffer to be filled with a serialized
CDesCArray of Contacts databases found on drive aDriveUnit.
@param aDriveUnit Drive to list Contacts databases for.  If defaulted then a
list of Contacts databases on all drives will be returned.
*/
void CCntDbManagerController::ListDatabasesL(CBufFlat*& aListBuffer, TDriveUnit* aDriveUnit) const
	{
	const TInt KInitialBufferSize = 200;
	aListBuffer = CBufFlat::NewL(KInitialBufferSize);
	CleanupStack::PushL(aListBuffer);
	
	// Create a write stream for this buffer.
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*aListBuffer);

	// Use Persistence Layer to get list of Contacts databases.  Note that the
	// ListL() method always returns a list even if empty so no NULL check for
	// list is required.
	MLplContactsFile& pl = iPersistenceLayer->ContactsFileL();
	CDesCArray* list =NULL;
	if(aDriveUnit == NULL)
		{
		list = pl.ListL(); // Retrieve Contacts databases on all drives.
		}
	else
		{
		list = pl.ListL(aDriveUnit); // Retrieve Contacts databases on named drive.
		}

	// Externalize the CDesCArray.
	CleanupStack::PushL(list);
	TInt count = list->Count();
	bufStream.WriteUint32L(count);
	
	for (TInt i=0; i<count; i++)
		{
		TInt length=(list->MdcaPoint(i)).Length();
		bufStream.WriteUint8L(length);
		bufStream.WriteL(list->MdcaPoint(i),length);
		}
	bufStream.CommitL();
	
	CleanupStack::PopAndDestroy(2, &bufStream); // list
	CleanupStack::Pop(aListBuffer);	
	}


/**
Return the index of the CCntDbManager instance which has a Contacts database
filename equal to aCntFile.  If no CCntDbManager instance has this filename then
KErrNotFound is returned.

@param aCntFile Contacts database filename.

@return Index of CCntDbManager instance which has a Contacts database filename
equal to aCntFile or KErrNotFound if no match.
*/
TInt CCntDbManagerController::FindExistingFileManager(const TDesC& aCntFile) const
	{
	for(TInt i=iManagers.Count()-1; i>=0; --i)
		{
		if(aCntFile.CompareF(iManagers[i]->CntFile()) == 0)
			{
			return i;
			}
		}
	return KErrNotFound;
	}


/**
Close a session for the given CCntDbManager instance.  If no more session are
open for this CCntDbManager instance (i.e. last client session for this Contacts
database has closed) then the instance is destroyed.

@param aManager CCntDbManager instance to close session on.
*/
void CCntDbManagerController::CloseDatabase(CCntDbManager& aManager)
	{
	aManager.RemoveSession();
	if(aManager.SessionCount() == 0)
		{
		for(TInt i=0;i<iManagers.Count();++i)
			{
			if(&aManager == iManagers[i])
				{
				iManagers.Remove(i);
				delete &aManager;
				break;
				}
			}
		}
	}


/**
Get the "system wide" current Contacts database.

@return "System wide" current Contacts database.
*/
const TDesC& CCntDbManagerController::CurrentDb() const
	{
	return iIniFileManager->CurrentDb();
	}


/**
Set the "system wide" current Contacts database.  Also notify all CCntDbManager
instances of the change of current Contacts database.

@param aNewCurrentDb New current Contacts database.
@param aSessionId Session ID of the client making the change.

@leave KErrNoMemory Out of memory.
@leave KErrNotFound
@leave KErrGeneral
*/
void CCntDbManagerController::SetCurrentDbL(const TDesC& aNewCurrentDb, TUint aSessionId)
	{
	iIniFileManager->SetCurrentDb(aNewCurrentDb);
	iIniFileManager->ScheduleSaveIniFileSettings(CIniFileManager::ESaveCurrentDatabase);
	TContactDbObserverEventV2 event;
	event.iType = EContactDbObserverEventCurrentDatabaseChanged;
	event.iContactId = KNullContactId;
	event.iConnectionId = aSessionId;
    event.iTypeV2 = EContactDbObserverEventV2Null;
    event.iAdditionalContactId = 0;
	NotifyDbManagersL(event);	
	}
	

/**
Process a Backup/Restore event generated by the Backup/Restore Agent.  Simply
notfiy all CCntDbManager instances of the event which they will process in
turn.

@param aEvent Backup/Restore event.
*/
void CCntDbManagerController::HandleBackupRestoreEventL(TContactDbObserverEventV2 aEvent)
	{
	TInt count = iManagers.Count();
	for (TInt i = 0 ; i < count ; ++i)
		{
		iManagers[i]->HandleBackupRestoreEventL(aEvent);
		}
	}


/**
Notify all CCntDbManager instances of the database event aEvent.  The
CCntDbManager instances will in turn notify all registered observers of the
event.

@param aEvent Database event.
*/
void CCntDbManagerController::NotifyDbManagersL(TContactDbObserverEventV2 aEvent)
	{
	TInt count = iManagers.Count();
	for (TInt i = 0 ; i < count ; ++i)
		{
		iManagers[i]->NotifyObserversL(aEvent);
		}
	}


/**
Schedule a save of the initialisation file.
*/
void CCntDbManagerController::ScheduleSaveIniFileSettings(TInt aSaveFlags, TBool aReplace)
	{
	iIniFileManager->ScheduleSaveIniFileSettings(aSaveFlags, aReplace);	
	}


/**
Get the Backup/Restore Agent instance owned by CCntDbManagerController.

@return CCntBackupRestoreAgent instance owned by CCntDbManagerController.
*/
CCntBackupRestoreAgent& CCntDbManagerController::BackupRestoreAgent() const
	{
	return *iBackupRestoreAgent;
	}
	

/**
Get the Initialisation File Manager instance owned by CCntDbManagerController.

@return CIniFileManager instance owned by CCntDbManagerController.
*/
CIniFileManager& CCntDbManagerController::IniFileManager() const
	{
	return *iIniFileManager;
	}
