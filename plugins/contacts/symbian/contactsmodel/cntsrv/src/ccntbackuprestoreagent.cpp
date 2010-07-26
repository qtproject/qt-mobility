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


#include "connect/sbdefs.h" // for conn::eburnormal etc.
#include <utf.h> // for cnvutfconverter::convertfromunicodetoutf8().

#include "ccntbackuprestoreagent.h"
#include "cinifilemanager.h"
#include "ccntdbmanagercontroller.h"
#include "ccntserver.h" // for kcntnullconnectionid.
#include "ccntlogger.h"


const TInt CCntBackupRestoreAgent::KMaxFileNameLength = 256;

_LIT(KSqLiteFilePrefix, "SQLite__");

/** Object factory method for CCntBackupRestoreAgent.
*/
CCntBackupRestoreAgent* CCntBackupRestoreAgent::NewL(RFs& aFs, CCntDbManagerController& aDbMgrCtrlr)
	{
	CCntBackupRestoreAgent* self = new(ELeave) CCntBackupRestoreAgent(aFs, aDbMgrCtrlr);
	return self;
	}


/** Destructor. 
*/
CCntBackupRestoreAgent::~CCntBackupRestoreAgent()
	{
	Cancel();
	iBackupRestoreNotification.Close();
	}


/** Process any initial notification and start monitoring for changes in the
	Backup/Restore property used by the SBEngine component.
*/
void CCntBackupRestoreAgent::StartL()
	{
	TInt newState = 0;
	
	// Check to see if a Backup/Restore is currently in progress.
	if (iBackupRestoreNotification.Get(newState) != KErrNotFound)
		{
		ProcessInitialStateL(newState);
		}

	SetActive();
	}


/** First phase default constructor.
*/
CCntBackupRestoreAgent::CCntBackupRestoreAgent(RFs& aFs, CCntDbManagerController& aDbMgrCtrlr)
	: CActive(CActive::EPriorityStandard), iFs(aFs), iDbMgrCtrlr(aDbMgrCtrlr)
	{
	CActiveScheduler::Add(this);

	iEvent.iType = EContactDbObserverEventNull;		// Changed as required.
	iEvent.iContactId =	KNullContactId;				// Always this value.
	iEvent.iConnectionId = KCntNullConnectionId;	// Always this value.

	// Attach and subscribe to the Backup/Restore property used by the SBEngine
	// component.
	iBackupRestoreNotification.Attach(KUidSystemCategory, conn::KUidBackupRestoreKey);
	iBackupRestoreNotification.Subscribe(iStatus);
	}


/** Indicates if a Backup operation is currently in progress.
	@return ETrue if operation in progress else EFalse.
*/
TBool CCntBackupRestoreAgent::BackupInProgress() const
	{
	return (iCurrentState & (conn::EBURBackupPartial | conn::EBURBackupFull));
	}


/** Indicates if a Restore operation is currently in progress.
	@return	ETrue if operation in progress else EFalse.
*/
TBool CCntBackupRestoreAgent::RestoreInProgress() const
	{
	return (iCurrentState & (conn::EBURRestorePartial | conn::EBURRestoreFull));
	}


/** Create a new XML backup registration file for the file aFileName.  The file
	will be created on the same drive as aFileName in the Contacts Model private
	data directory.

	@param aFileName File name including drive letter i.e. <drive>:<file>.
*/
void CCntBackupRestoreAgent::CreateBackupRegistrationFileL(const TDesC& aFileName)
	{
	_LIT8(KXmlFilePart1, "<?xml version=\"1.0\" standalone=\"yes\"?>\r\n"
		"<backup_registration>\r\n"
		"    <passive_backup base_backup_only=\"yes\">\r\n"
		"        <include_file name=\"");
	_LIT8(KXmlFilePart2, "\"/>\r\n"
		"    </passive_backup>\r\n"
		"</backup_registration>\r\n");
		
	// Find out the drive held in aFileName.  If a drive letter is not present
	// then the method leaves.
	TParsePtrC parseFileName(aFileName);
	User::LeaveIfError(parseFileName.DrivePresent());
	
	TPath privatePath;
	User::LeaveIfError(iFs.PrivatePath(privatePath));
	User::LeaveIfError(iFs.SetSessionPath(parseFileName.Drive()));
	User::LeaveIfError(iFs.SetSessionPath(privatePath));

	// If the path does not exist create it.
	TInt err = iFs.MkDirAll(privatePath);

	if (err != KErrAlreadyExists && err != KErrNone)
		{
		User::Leave(err);
		}

	HBufC* newFileName = CreateBackupRegistrationFileNameLC(parseFileName.NameAndExt());
	TPtr newFileNamePtr(newFileName->Des());

	// Create registration file.
	RFile file;
	CleanupClosePushL(file);

	err = file.Create(iFs, *newFileName, EFileWrite);

	// Registration file already exists so we're done.
	if (err == KErrAlreadyExists)
		{
		CleanupStack::PopAndDestroy(2, newFileName); // file, newFileName
		return;
		}

	if (err)
		{
		User::Leave(err);
		}
		
	HBufC* nameAndExt = NULL;
	nameAndExt = HBufC::NewLC(KSqLiteFilePrefix().Length() + parseFileName.NameAndExt().Length());
	nameAndExt->Des().Append(KSqLiteFilePrefix);
	nameAndExt->Des().Append(parseFileName.NameAndExt());

	// Convert filename and extension to UTF8 before writing to file.
	HBufC8* fileNameAndExt8 = HBufC8::NewLC(nameAndExt->Length());
	TPtr8 pFileNameAndExt8(fileNameAndExt8->Des());
	
	User::LeaveIfError(CnvUtfConverter::ConvertFromUnicodeToUtf8(pFileNameAndExt8,
		*nameAndExt));

	// Write data into file.
	User::LeaveIfError(file.Write(KXmlFilePart1()));
	User::LeaveIfError(file.Write(pFileNameAndExt8));
	User::LeaveIfError(file.Write(KXmlFilePart2()));
	User::LeaveIfError(file.Flush());

	CleanupStack::PopAndDestroy(4, newFileName); // fileNameAndExt8, file, nameAndExt, newFileName
	}


/** Delete the XML backup registration file associated with the filename.  The
	registration file will be found on the same drive as aFileName.

	@param aFileName File name including drive letter i.e. <drive>:<file>.
*/
void CCntBackupRestoreAgent::DeleteBackupRegistrationFileL(const TDesC& aFileName)
	{
	TParsePtrC parseFileName(aFileName);
	
	// Find out the drive held in aFileName.  If a drive letter is not present
	// then the method leaves.
	User::LeaveIfError(parseFileName.DrivePresent());
	
	TPath privatePath;
	User::LeaveIfError(iFs.PrivatePath(privatePath));
	User::LeaveIfError(iFs.SetSessionPath(parseFileName.Drive()));
	User::LeaveIfError(iFs.SetSessionPath(privatePath));

	HBufC* regFileName = CreateBackupRegistrationFileNameLC(parseFileName.NameAndExt());
	TPtrC regFileNamePtr(regFileName->Des());

	// Check if registration file exists and if so delete it.
	TEntry entry;
	TInt err = iFs.Entry(regFileNamePtr, entry);

	if((err == KErrNone) && !entry.IsDir())
		{
		User::LeaveIfError(iFs.Delete(regFileNamePtr));
		}

	CleanupStack::PopAndDestroy(regFileName);
	}


/** Handle change in value of the property KUidBackupRestoreKey (used to notify
	subscribers of Backup/Restore events).
	@see CActive
*/
void CCntBackupRestoreAgent::RunL()
	{
	// Resubscribe before dealing with the current notification.
	iBackupRestoreNotification.Subscribe(iStatus);
	SetActive();

	TInt newState = 0;
	
	// Flag updated.  Decide what to do in ReceivedNotificationL().
	if (iBackupRestoreNotification.Get(newState) != KErrNotFound)
		{
		ReceivedNotificationL(newState);
		}
	}


/** Recover if RunL() leaves.
	@see CActive
*/
TInt CCntBackupRestoreAgent::RunError(TInt /*aError*/)
	{
	
	DEBUG_PRINT1(__VERBOSE_DEBUG__,_L("[CNTMODEL] CCntBackupRestoreAgent::RunError(): RunL() has left!\n")); 

	return (KErrNone);
	}


/**
	@see CActive
*/
void CCntBackupRestoreAgent::DoCancel()
	{
	iBackupRestoreNotification.Cancel();
	}


/** Determines what operation is starting or has completed and calls the
	relevant observer function.

	@param aNewState Flags indicating what the new Backup/Restore state is:

	1. No Backup/Restore is taking place = EBURNormal | ENoBackup		
	2. Backup operation starting = (EBURBackupFull || EBURBackupPartial) &&
	   EBackupBase
	3. Restore operation starting = (EBURRestoreFull || EBURRestorePartial) &&
	   EBackupBase
*/
void CCntBackupRestoreAgent::ReceivedNotificationL(TInt aNewState)
	{
	// We are starting a a new operation...
	// Was there a previous Restore which failed to complete?
	if (!(aNewState & (conn::EBURNormal | conn::EBURUnset)) && RestoreInProgress())
		{
		iCurrentState = aNewState;
		RestoreCompletedL(KErrAbort);
		}
	
	// A Base Backup or Restore taking place.
	if (aNewState & (conn::EBURBackupPartial | conn::EBURBackupFull) )
		{			
		// Backup is taking place (either partial or full) 
		iCurrentState = aNewState;
		BackupBeginningL();
		}
	else if (aNewState & (conn::EBURRestorePartial | conn::EBURRestoreFull)) 
		{
		// Restore is taking place (either partial or full)
		iCurrentState = aNewState;
		RestoreBeginningL();
		}
	else if (aNewState & (conn::EBURNormal | conn::EBURUnset))
		{
		// The state has changed to no Backup/Restore.  Decide which operation
		// has just completed.
		if (BackupInProgress())
			{
			iCurrentState = aNewState;
			BackupCompletedL();
			}
		else if (RestoreInProgress())
			{
			iCurrentState = aNewState;
			RestoreCompletedL(KErrNone);
			}		
		}
	}


void CCntBackupRestoreAgent::ProcessInitialStateL(TInt aNewState)
	{
	// A Base Backup or Restore is taking place.
	if (aNewState & (conn::EBURBackupPartial | conn::EBURBackupFull) )
		{
		// Backup is taking place (either partial or full).
		BackupBeginningL();
		}
	else if (aNewState & (conn::EBURRestorePartial | conn::EBURRestoreFull)) 
		{
		// Restore is taking place (either partial or full).
		RestoreBeginningL();
		}
	iCurrentState = aNewState;
	}


void CCntBackupRestoreAgent::BackupBeginningL()
	{
	iEvent.iType = EContactDbObserverEventBackupBeginning;
	iDbMgrCtrlr.HandleBackupRestoreEventL(iEvent);
	iDbMgrCtrlr.IniFileManager().SaveIniFileSettingsIfRequestedL();
	}


void CCntBackupRestoreAgent::BackupCompletedL()
	{
	iEvent.iType = EContactDbObserverEventBackupRestoreCompleted;
	iDbMgrCtrlr.HandleBackupRestoreEventL(iEvent);
	// In case Store has been waiting for backup to finish.  Now attempt to
	// perform deferred operation.
	iDbMgrCtrlr.IniFileManager().RetryStoreOperation();
	}


void CCntBackupRestoreAgent::RestoreBeginningL()
	{
	iEvent.iType = EContactDbObserverEventRestoreBeginning;
	iDbMgrCtrlr.HandleBackupRestoreEventL(iEvent);
	iDbMgrCtrlr.IniFileManager().SaveIniFileSettingsIfRequestedL();
	}


void CCntBackupRestoreAgent::RestoreCompletedL(TInt aRestoreResult)
	{
	if (aRestoreResult == KErrNone)
		{
		iDbMgrCtrlr.IniFileManager().StartRestoreIniFileSettings();
		iEvent.iType = EContactDbObserverEventBackupRestoreCompleted;
		iDbMgrCtrlr.HandleBackupRestoreEventL(iEvent);
		}
	else
		{
		iEvent.iType = EContactDbObserverEventRestoreBadDatabase;
		iDbMgrCtrlr.HandleBackupRestoreEventL(iEvent);
		// A restore of the Contacts Model initialisation file failed so save
		// initialisation file.
		iDbMgrCtrlr.IniFileManager().ScheduleSaveIniFileSettings(CIniFileManager::ESaveAll);
		}
	}
	

/** Create an XML backup registration file name using aFileName.

	@param aFileName File name without drive letter.
*/
HBufC* CCntBackupRestoreAgent::CreateBackupRegistrationFileNameLC(const TDesC& aFileName)
	{	
	_LIT(KContactsBackupFileName, "backup_registration");
	_LIT(KContactsBackupFileExtension, ".xml");

	const TInt newFileNameLength = KContactsBackupFileName().Length() +	
	KSqLiteFilePrefix().Length() +
	aFileName.Length() + KContactsBackupFileExtension().Length();
	
	if (newFileNameLength > KMaxFileNameLength)
		{
		User::Leave(KErrArgument);
		}
		
	HBufC* newFileName = HBufC::NewL(newFileNameLength);
	CleanupStack::PushL(newFileName);

	TPtr newFileNamePtr(newFileName->Des());

	// New filename will be of the form backup_registration<aFileName>.xml.
	// If aFileName has an extension then the '.' will be replaced with '_'.
	newFileNamePtr.Append(KContactsBackupFileName);
	
	// if the it is the cntModel.ini file
	// then do not append it in the file name
	// this ensures that there is one file
	// with the name backup_registration.xml file
	// which is required for the Backup & Restore engine.
	if (aFileName.Compare(KContactsIniFileName) != 0) 
	    {
	    newFileNamePtr.Append(KSqLiteFilePrefix);
		newFileNamePtr.Append(aFileName);             
	    }                                             
	                                                  
	                                                  
	TInt dotPos = newFileNamePtr.LocateReverse('.');
	if (dotPos != KErrNotFound )
		{
		newFileNamePtr[dotPos] = '_';
		}
	newFileNamePtr.Append(KContactsBackupFileExtension);

	return newFileName;
	}
