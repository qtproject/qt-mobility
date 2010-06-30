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

#ifndef CCNTDBMANAGER_H
#define CCNTDBMANAGER_H

#include "persistencelayer.h"
#include "ccntlowdiskmanager.h"
#include <e32base.h>


enum TCntFileMode
	{
	ECntFileOpen,
	ECntFileCreate,
	ECntFileReplace
	};

class CPersistenceLayer;
class CCntStateMachine;
class CCntRequest;
class CCntBackupRestoreAgent;
class CIniFileManager;
class CViewManager;


/**
Low disk threshold is set at 128Kb.  Below this threshold only database
operations which decrease the size of the database file are allowed.
*/
const TInt64 KLowDiskThreshold = 0x20000;


/**
The CCntDbManager class represents the most abstract level of access to the
Contacts database.

There is one CCntDbManager class per Contacts database.  Multiple sessions
(CCntSession objects) can be mapped to one CCntDbManager.

Multiple instances of the CCntDbManager class are controlled by the one
CCntDbManagerController instance (owned by CCntServer2).
*/
class CCntDbManager : public CBase, public MContactDbObserverV2, public MContactLowDiskObserver, public MIniFileManager
	{
public:
	static CCntDbManager* NewLC(RFs& aFs, const TDesC& aCntFile, TCntFileMode aFileMode, CCntBackupRestoreAgent& aBackupRestoreAgent, CIniFileManager& aIniManager);
	virtual ~CCntDbManager();

	void AddSession();
	void RemoveSession();
	TInt SessionCount() const;

	// Access methods.
	CPersistenceLayer& GetPersistenceLayer();
	CCntStateMachine& StateMachineL();
	CCntBackupRestoreAgent& BackupRestoreAgent();
	CIniFileManager& IniFileManager();
	CViewManager& ViewManagerL();	

	// Database file methods.
	const TDesC& CntFile();
	TInt FileSizeL();
	
	void RegisterDatabaseEventObserverL(MContactDbObserver& aObserver);
	void UnRegisterDatabaseEventObserver(MContactDbObserver& aObserver);
	void RegisterDatabaseEventObserverL(MContactDbObserverV2& aObserver);
	void UnRegisterDatabaseEventObserver(MContactDbObserverV2& aObserver);

	// From MContactDbObserver.
	void HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent);
	void HandleBackupRestoreEventL(TContactDbObserverEventV2 aEvent);
	void HandleLowDiskL(TBool aLowDisk);

	void NotifyObserversL(const TContactDbObserverEventV2 aEvent);

	// Current item methods.
	void SetCurrentItemL(TContactItemId aContactId, TUint aConnectionId);
	TContactItemId CurrentItem() const;
	void RemoveCurrentItemL(TUint aConnectionId);
	void DeleteNotifyL(TContactItemId aContactId);
	
	// Speed dial methods.
	void GetSpeedDialContactIdAndPhoneNumberL(const TInt aSpeedDialIndex, TSpeedDialPhoneNumber& aPhoneNumber, TContactItemId& aContactId);
	TContactItemId SetSpeedDialIdForPositionL(const TInt aSpeedDialIndex, const TContactItemId aContactId,  const TSpeedDialPhoneNumber& aPhoneNumber, TUint aConnectionId, TBool aSendNotification);
	CArrayFix<TInt>* SpeedDialIndicesForContactIdLC(TContactItemId aContactId); 

	void SetCardTemplatePrefIdL(TContactItemId aCardTemplatePrefId, TInt aConnectionId);
	void RecreateSystemTemplateL();

private:
	void ConstructL(TCntFileMode aFileMode);
	CCntDbManager(RFs& aFs, const TDesC& aCntFile, CCntBackupRestoreAgent& aBackupRestoreAgent, CIniFileManager& aIniManager);

	// Access methods.
	CCntLowDiskManager&	LowDiskManagerL();

private:
	CCntStateMachine*		iStateMachine;
	CCntLowDiskManager*		iLowDiskManager;	
	CPersistenceLayer*		iPersistenceLayer;
	CViewManager* 			iViewManager;

	RFs&					iFs;
	TFileName				iCntFile;
	
	CCntBackupRestoreAgent& iBackupRestoreAgent;
	CIniFileManager&		iIniManager;

	TInt 					iSessionCount;
	
	// Array of registered database event observers.
	RPointerArray<MContactDbObserver> iObserverArray;
	RPointerArray<MContactDbObserverV2> iObserverV2Array;
 	};

	
#endif
