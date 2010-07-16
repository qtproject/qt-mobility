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


#ifndef __CCNTBACKUPRESTOREAGENT_H__
#define __CCNTBACKUPRESTOREAGENT_H__


#include <e32base.h> // for cactive.
#include <e32property.h> // for rproperty.

#include <cntdbobs.h> // for tcontactdbobserverevent 


// Forward class references.
class CCntDbManagerController;


/**
	All interaction with the SBEngine component is through this class.  This
	class is owned by CCntDbManagerController primarily due to the need to
	propagate notification events from the SBEngine to all of the CCntDbManager.

	This class implements the behaviours required by the SBEngine component:

	- SBEngine Notification Observer
	- Registration File Management

	In addition the following behaviours are implemented:
		
	- Store/recall of Contacts Model initialisation file using CIniFileManager
	  at appropriate times during Backup/Restore

	@internalComponent
	@released
*/
class CCntBackupRestoreAgent : public CActive
	{
public:
	static CCntBackupRestoreAgent* NewL(RFs& aFs, CCntDbManagerController& aDbMgrCtrlr);
	~CCntBackupRestoreAgent();

	void StartL();
	
	TBool BackupInProgress() const;
	TBool RestoreInProgress() const;
	
	void CreateBackupRegistrationFileL(const TDesC& aFileName);
	void DeleteBackupRegistrationFileL(const TDesC& aFileName);
	HBufC* CreateBackupRegistrationFileNameLC(const TDesC& aFileName);
	
private:
	CCntBackupRestoreAgent(RFs& aFs, CCntDbManagerController& aDbMgrCtrlr);
	
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

	void ReceivedNotificationL(TInt aNewState);
	void ProcessInitialStateL(TInt aNewState);

	void BackupBeginningL();
	void BackupCompletedL();
	void RestoreBeginningL();
	void RestoreCompletedL(TInt aRestoreResult);

		
private:
	RFs& iFs;
	
	/**
	Used to subscribe to the SBEngine P&S property KUidBackupRestoreKey.  The
	SBEngine uses the property KUidBackupRestoreKey to notify subscribers of
	Backup/Restore events.
	*/
	RProperty iBackupRestoreNotification;

	/**
	Current state of B&R.  Used to store the B&R flags supplied by the value
	of the property KUidBackupRestoreKey.
	*/
	TInt iCurrentState;

	/**
	Contacts database observer event.  The event type is changed depending on
	the notification received from the SBEngine before being forwarded to all
	Contacts Model clients (via the CCntDbManagerController singleton).
	*/
	TContactDbObserverEventV2 iEvent;
	
	/**
	Used to broadcast Backup/Restore events to all instances of CCntDbManager
	and to access CIniFileManager singleton owned by CCntDbManagerController
	singleton so that operations on Contacts Model initialisation file can
	be carried out as part of Backup/Restore activity.
	*/
	CCntDbManagerController& iDbMgrCtrlr;
	
private:
	static const TInt KMaxFileNameLength;
	};

	
#endif //__CCCNTBACKUPRESTOREAGENT_H__
