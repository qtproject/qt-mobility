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


#include "persistencelayer.h"
#include "ccntdbmanager.h"
#include "ccntrequest.h"
#include "ccntstatemachine.h"
#include "ccntbackuprestoreagent.h"
#include "cinifilemanager.h"
#include "cntspeeddials.h"
#include "ccntlowdiskmanager.h"
#include "cviewsubsessions.h"

CCntDbManager::CCntDbManager(RFs& aFs, const TDesC& aCntFile, CCntBackupRestoreAgent& aBackupRestoreAgent, CIniFileManager& aIniManager) 
: iFs(aFs), iCntFile(aCntFile), iBackupRestoreAgent(aBackupRestoreAgent), iIniManager(aIniManager)
	{
	}

CCntDbManager::~CCntDbManager()
	{
	iObserverArray.Close();
	iObserverV2Array.Close();
	delete iViewManager;
 	delete iPersistenceLayer;
 	delete iStateMachine;
 	delete iLowDiskManager;
	}


const TDesC& CCntDbManager::CntFile()
	{
	return iCntFile;
	}

/**
Object factory method.

@param aFs File Server session.
@param aCntFile Filename of the Contacts database.
@param aFileMode Indicates whether to open, create or replace the Contacts
database.  If open is indicated then the actual open takes place within the
State Machine and is initiated by the CCntDbManagerController singleton.
@param aBackupRestoreAgent The Backup/Restore Agent used to monitor inform this
database of Backup/Restore events originating from the SBEngine component.
@param aIniManager The Initialisation File Manager used to manage this
database's data within cntmodel.ini.

@return CCntDbManager instance.
*/
CCntDbManager* CCntDbManager::NewLC(RFs& aFs, const TDesC& aCntFile, TCntFileMode aFileMode, CCntBackupRestoreAgent& aBackupRestoreAgent, CIniFileManager& aIniManager)
 	{
	CCntDbManager* manager = new (ELeave) CCntDbManager(aFs, aCntFile, aBackupRestoreAgent, aIniManager);
	CleanupStack::PushL(manager);
	manager->ConstructL(aFileMode);
	return manager;
	}


/**
Second phase constructor.

@param aFileMode Indicates whether to open, create or replace the Contacts
database.  If open is indicated then the actual open takes place within the
State Machine and is initiated by the CCntDbManagerController singleton.
*/
void CCntDbManager::ConstructL(TCntFileMode aFileMode)
 	{
	iPersistenceLayer = CPersistenceLayer::NewLC(iFs, this, NULL);
	CleanupStack::Pop(iPersistenceLayer);

 	// The State Machine is passed into the Persistence Layer as
 	// MContactDbObserverV2 since it is the first observer to recieve (and deal
 	// with) database events outside the Persistence Layer. 
	iPersistenceLayer->RegisterDbObserver(StateMachineL());

	// Create and replace use the same call just the overwrite is different.
	// Once created, the database has to be opened.  The open operation is
	// initiated in the State Machine by the CCntDbManagerController singleton
	// that creates this CCntDbManager instance.
	if (aFileMode == ECntFileCreate)
		{
		iPersistenceLayer->ContactsFileL().CreateL(iCntFile, MLplContactsFile::EPlLeaveIfExist);
		}
	else if (aFileMode == ECntFileReplace)
		{
		iPersistenceLayer->ContactsFileL().CreateL(iCntFile, MLplContactsFile::EPlOverwrite);
		}

	if (aFileMode == ECntFileCreate || aFileMode == ECntFileReplace)
		{
		// creating a backup registration file for the newly created or replaced database file.
		TRAPD(createErr, BackupRestoreAgent().CreateBackupRegistrationFileL(iCntFile));
		
		if (createErr != KErrNone)
			{
			//An error has occured while creating backupregistration file,
			//therefore we should delete the newly created database file.
			TRAP_IGNORE(iPersistenceLayer->ContactsFileL().DeleteL(iCntFile));
			
			//return the error recieved while creating BackupRegistration File
			User::Leave(createErr);
			}	
		}

	// Start Low Disk Space Manager.  Uses the database file name in iCntFile to
	// determine the drive to observe for low disk space.
	LowDiskManagerL().Start();
	}


/**
Get the State Machine instance owned by this CCntDbManager.  This instance is
used to call CCntStateMachine::ProcessRequestL() which implements the Visitor
Pattern.  If the instance does not yet exist then create it (lazy
initialisation).

@return CCntStateMachine instance owned by this CCntDbManager.

@leave KErrNoMemory Out of memory.
*/
CCntStateMachine& CCntDbManager::StateMachineL()
	{
	if (!iStateMachine)
		{
		iStateMachine = CCntStateMachine::NewL(*iPersistenceLayer, *this);
		}
	return *iStateMachine;
	}

	
/**
Get the Persistence Layer instance owned by this CCntDbManager.

@return CPersistenceLayer instance owned by this CCntDbManager.
*/
CPersistenceLayer& CCntDbManager::GetPersistenceLayer()
	{
	return *iPersistenceLayer;
	}


/**
Get the Backup/Restore Agent instance owned by this CCntDbManager.

@return CCntBackupRestoreAgent instance owned by this CCntDbManager.
*/
CCntBackupRestoreAgent& CCntDbManager::BackupRestoreAgent()
	{
	return iBackupRestoreAgent;
	}


/**
Get the Initialisation File Manager instance owned by this CCntDbManager.

@return CIniFileManager instance owned by this CCntDbManager.
*/
CIniFileManager& CCntDbManager::IniFileManager()
	{
	return iIniManager;
	}


/**
Get the View Manager instance owned by this CCntDbManager.  If the instance does
not yet exist then create it (lazy initialisation).

@return CCntStateMachine instance owned by this CCntDbManager.
*/
CViewManager& CCntDbManager::ViewManagerL()
	{
	if (!iViewManager)
		{
		iViewManager = CViewManager::NewL(GetPersistenceLayer().FactoryL(), *this);
		}	
	return *iViewManager;
	}


/**
Create and start Active Object which observes disk threshold notifications.

@return CCntLowDiskManager instance owned by this CCntDbManager.
*/
CCntLowDiskManager& CCntDbManager::LowDiskManagerL()
	{
	if (!iLowDiskManager)
		{
		// Find out the drive held in iCntFile.  If a drive letter is not
		// present then the method leaves.
		TParsePtrC parseFileName(iCntFile);
		if (!parseFileName.DrivePresent())
		{
			User::Leave(KErrBadName);
		}
		TDriveUnit drive(parseFileName.Drive());

		iLowDiskManager = CCntLowDiskManager::NewL(*this, KLowDiskThreshold, drive, iFs);	
		//
		// FUTURE: In later implementation we will call RFs::ReserveDriveSpace()
		// either here or via Persistence Layer.  There is no method to un-
		// reserve this space - it is automatically cleaned up when the RFs
		// object used to reserve the space is closed.
		//
		}
	return *iLowDiskManager;
	}


	
TInt CCntDbManager::FileSizeL()
	{
	return iPersistenceLayer->ContactsFileL().FileSize();  
	}	
	

/**
Add a session (increase the number of clients accessing this database).
*/
void CCntDbManager::AddSession()
	{
	iSessionCount++;
	}


/**
Remove a session (decrease the number of clients accessing this database).
*/
void CCntDbManager::RemoveSession()
	{
	iSessionCount--;
	}


/**
Return the session count.  When the session count reaches zero this instance of
CCntDbManager can be destroyed.

@return Session count.
*/
TInt CCntDbManager::SessionCount() const
	{
	return iSessionCount;
	}


/**
Add a database event observer for this database.

@param aObserver Database event observer to add.
*/
void CCntDbManager::RegisterDatabaseEventObserverL(MContactDbObserver& aObserver)
	{
	User::LeaveIfError(iObserverArray.InsertInAddressOrder(&aObserver));
	}


/**
Remove a database event observer for this database.

@param aObserver Database event observer to remove.
*/
void CCntDbManager::UnRegisterDatabaseEventObserver(MContactDbObserver& aObserver)
	{
	TInt index = KErrNotFound;
	const TInt error = iObserverArray.FindInAddressOrder(&aObserver, index);
	if	(error != KErrNotFound)
		{
		iObserverArray.Remove(index);
		}
	}

/**
Add a database event observer for this database.

@param aObserver Database event observer to add.
*/
void CCntDbManager::RegisterDatabaseEventObserverL(MContactDbObserverV2& aObserver)
    {
    User::LeaveIfError(iObserverV2Array.InsertInAddressOrder(&aObserver));
    }


/**
Remove a database event observer for this database.

@param aObserver Database event observer to remove.
*/
void CCntDbManager::UnRegisterDatabaseEventObserver(MContactDbObserverV2& aObserver)
    {
    TInt index = KErrNotFound;
    const TInt error = iObserverV2Array.FindInAddressOrder(&aObserver, index);
    if  (error != KErrNotFound)
        {
        iObserverV2Array.Remove(index);
        }
    }

/**
Process a database event (typically generated in the Persistence Layer).

@param aEvent Database event.
*/
void CCntDbManager::HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent)
	{
	// Notify all registered observers of the event.
	NotifyObserversL(aEvent);	
	}


/**
Process a Backup/Restore event (typically generated by the Database Manager 
Controller via the Backup/Restore Agent).

@param aEvent Database event (limited to Backup/Restore database events).
*/
void CCntDbManager::HandleBackupRestoreEventL(TContactDbObserverEventV2 aEvent)
	{
	// Create and then process appropriate request using state machine.
	CCntRequest* request = NULL;

	switch(aEvent.iType)
		{
		case EContactDbObserverEventBackupBeginning :
		case EContactDbObserverEventRestoreBeginning :
			{
			request = CReqBackupRestoreBegin::NewLC();
			}
			break;

		case EContactDbObserverEventBackupRestoreCompleted :
		// Bad database recovery is automatically carried out in the
		// Persistence Layer as part of the opening process.
		case EContactDbObserverEventRestoreBadDatabase :
			{
			request = CReqBackupRestoreEnd::NewLC();
			}
			break;

		default :
			{
			// Misuse of API (unexpected event).
			User::Leave(KErrArgument);
			}
		}

	StateMachineL().ProcessRequestL(request);    // ownership transferred

	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.
	CleanupStack::Pop(request); 

	// Notify all registered observers of Backup/Restore event.
	NotifyObserversL(aEvent);
	}


/**
Process a low/not low disk condition.

@param aLowDisk ETrue if low disk condition, EFalse otherwise.
*/
void CCntDbManager::HandleLowDiskL(TBool aLowDisk)
	{
	// Create and then process appropriate request using state machine.
	CCntRequest* request = NULL;

	if (aLowDisk)
		{
		request = CReqDiskSpaceLow::NewLC();
		}
	else
		{
		request = CReqDiskSpaceNormal::NewLC();
		}

	StateMachineL().ProcessRequestL(request);    // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.
    CleanupStack::Pop(request);
	}


/**
Notify all registered observers (typically a client session) of a database
event.  Each registered observer implements the MContactDbObserver interface
which is comprised of the HandleDatabaseEventL() method.

@param aEvent Database event to send to registered observers.
*/
void CCntDbManager::NotifyObserversL(const TContactDbObserverEventV2 aEvent)
	{
	for(TInt i = iObserverArray.Count()-1; i>=0; --i)
		{
		iObserverArray[i]->HandleDatabaseEventL(aEvent);
		}
    for(TInt i = iObserverV2Array.Count()-1; i>=0; --i)
        {
        iObserverV2Array[i]->HandleDatabaseEventV2L(aEvent);
        }	
	}


void CCntDbManager::SetCurrentItemL(TContactItemId aContactId, TUint aConnectionId)
	{
	iIniManager.SetCurrentItemL(iCntFile, aContactId);
	TContactDbObserverEventV2 event;
	event.iType = EContactDbObserverEventCurrentItemChanged;
	event.iContactId = aContactId;
	event.iConnectionId = aConnectionId;
    event.iTypeV2 = EContactDbObserverEventV2Null;
    event.iAdditionalContactId = 0;
	// Save the changes to the contacts model ini file
	iIniManager.ScheduleSaveIniFileSettings(CIniFileManager::ESaveCurrentItem);
	NotifyObserversL(event);
	}


void CCntDbManager::RemoveCurrentItemL(TUint aConnectionId)
	{
	iIniManager.RemoveCurrentItemL(iCntFile);
	TContactDbObserverEventV2 event;
	event.iType = EContactDbObserverEventCurrentItemDeleted;
	event.iContactId = KNullContactId;
	event.iConnectionId = aConnectionId;
    event.iTypeV2 = EContactDbObserverEventV2Null;
    event.iAdditionalContactId = 0;
	// Save the changes to the contacts model ini file
	iIniManager.ScheduleSaveIniFileSettings(CIniFileManager::ESaveCurrentItem);
	NotifyObserversL(event);
	}

	
TContactItemId CCntDbManager::CurrentItem() const
	{
	return iIniManager.CurrentItem(iCntFile);
	}


void CCntDbManager::DeleteNotifyL(TContactItemId aContactId)
	{
	if (aContactId == CurrentItem())
		{
		// Tell the server that the current item has just been deleted.
		RemoveCurrentItemL(0);
		}
	}


/**
Return the speed dial phone number at the specified speed dial table index.
*/
void CCntDbManager::GetSpeedDialContactIdAndPhoneNumberL(const TInt aSpeedDialIndex, TSpeedDialPhoneNumber& aPhoneNumber, TContactItemId& aContactId)
	{
	const CCntServerSpeedDialTable& table = iIniManager.SpeedDialManager().TableL(iCntFile);
	aContactId = table.SpeedDialContactItem(aSpeedDialIndex, aPhoneNumber);
	}


/**
Set the phone number and Contact ID for the specified speed dial table index.
*/
TContactItemId CCntDbManager::SetSpeedDialIdForPositionL(const TInt aSpeedDialIndex, const TContactItemId aContactId,  const TSpeedDialPhoneNumber& aPhoneNumber, TUint aConnectionId, TBool aSendNotification)
	{
	// Get the speed dial table
	CCntServerSpeedDialTable& table = iIniManager.SpeedDialManager().TableL(iCntFile);
	// Grab data from the speed dial table
	TContactItemId contactIdToBroadcast;
	if ( aContactId == KNullContactId )
		 {
		 TSpeedDialPhoneNumber phoneNumberFromSpeedDialTable;
		 contactIdToBroadcast = table.SpeedDialContactItem(aSpeedDialIndex, phoneNumberFromSpeedDialTable);
		 }
	else
		 {
 		 contactIdToBroadcast = aContactId;
 		 }

	// Update speed dial array
	table.SetSpeedDialL(aSpeedDialIndex, aContactId, aPhoneNumber);	
	// Save the changes to the contacts model ini file
	iIniManager.ScheduleSaveIniFileSettings(CIniFileManager::ESaveSpeedDials);
	if (aSendNotification)
		{	
		//BroadCast the SpeedDialsChanged event
		TContactDbObserverEventV2 event;
		event.iType = EContactDbObserverEventSpeedDialsChanged;
		event.iContactId = contactIdToBroadcast;
		event.iConnectionId = aConnectionId;
	    event.iTypeV2 = EContactDbObserverEventV2Null;
	    event.iAdditionalContactId = 0;
		NotifyObserversL(event);
		}
	return contactIdToBroadcast;	
	}

	
CArrayFix<TInt>* CCntDbManager::SpeedDialIndicesForContactIdLC(TContactItemId aContactId)
	{
	CCntServerSpeedDialTable& table = iIniManager.SpeedDialManager().TableL(iCntFile);	
	return table.SpeedDialIndicesForContactIdLC(aContactId);
	}

	
void CCntDbManager::SetCardTemplatePrefIdL(TContactItemId aCardTemplatePrefId, TInt aConnectionId)
	{
	// This is effectively a Write operation which should not be allowed in low
	// disk condition.
	if (LowDiskManagerL().LowDiskL())
		{
		User::Leave(KErrDiskFull);
		}
	iPersistenceLayer->ContactProperties().SetCardTemplatePrefIdL(aCardTemplatePrefId);
	//Notify clients here
	TContactDbObserverEventV2 event;
	event.iType = EContactDbObserverEventPreferredTemplateChanged;
	event.iContactId = aCardTemplatePrefId;
	event.iConnectionId = aConnectionId;
    event.iTypeV2 = EContactDbObserverEventV2Null;
    event.iAdditionalContactId = 0;
	NotifyObserversL(event);	
	}


void CCntDbManager::RecreateSystemTemplateL()
	{
	// This is effectively a Write operation which should not be allowed in low
	// disk condition.
	if (LowDiskManagerL().LowDiskL())
		{
		User::Leave(KErrDiskFull);
		}
	iPersistenceLayer->ContactProperties().RecreateSystemTemplateL();
	}
