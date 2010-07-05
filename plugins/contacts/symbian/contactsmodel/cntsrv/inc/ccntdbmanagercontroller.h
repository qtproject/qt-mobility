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

#ifndef CCNTDBMANAGERCONTROLLER_H
#define CCNTDBMANAGERCONTROLLER_H

#include <e32base.h>
#include <f32file.h> // for tdriveunit.
#include "ccntserver.h" // for tcntservertype.
#include "ccntdbmanager.h" // for tcntfilemode.
#include "cinifilemanager.h" // for tsavetype.


class CCntDbManager;
class CPersistenceLayer;
class CReqAsyncOpen;
class CCntServerSpeedDialManager;
class CCntBackupRestoreAgent;
class CCntPermanentData;
		

/**
The CCntDbManagerController instantiates and controls all CCntDbManager
instances.

There is one CCntDbManagerController class per CCntServer instance.
*/
class CCntDbManagerController : public CBase
	{
public:
	static CCntDbManagerController* NewLC(TCntServerType aServerType);
	virtual ~CCntDbManagerController();

	CCntDbManager* GetDbManagerL(TDes& aCntFile, TCntFileMode aMode);
	CCntDbManager* DbManagerL(const TDesC& aCntFile) const;

	// Database management methods.
	void DeleteDatabaseL(TDes& aCntFile) const;
	void DefaultDatabaseL(TDes& aCntFile) const;
	void DatabaseDrive(TDriveUnit& aDrive) const;	
	void SetDatabaseDriveL(TDriveUnit aDrive,TBool aCopy);
	void RestoreDatabaseDrive(TDriveUnit& aDrive);
	TBool DatabaseExistsL(const TDesC& aCntFile) const;
	void ListDatabasesL(CBufFlat*& aList,TDriveUnit* aDriveUnit = NULL) const;
	void CloseDatabase(CCntDbManager& aManager);
	void SetCurrentDbL(const TDesC& aNewCurrentDb, TUint aSessionId);
	const TDesC& CurrentDb() const;
	
	void HandleBackupRestoreEventL(TContactDbObserverEventV2 aEvent);
	
	void ScheduleSaveIniFileSettings(TInt aSaveFlags, TBool aReplace=EFalse);
		
	// Access methods.
	CCntBackupRestoreAgent& BackupRestoreAgent() const;
	CIniFileManager& IniFileManager() const;

private:
	CCntDbManagerController();
	void ConstructL(TCntServerType aServerType);
	
	TInt FindExistingFileManager(const TDesC& aCntFile) const;

	void NotifyDbManagersL(TContactDbObserverEventV2 aEvent);
		
private:
	RPointerArray<CCntDbManager>	iManagers;
	CPersistenceLayer*				iPersistenceLayer;
	TDriveUnit						iDefaultDriveUnit;

	CCntBackupRestoreAgent*			iBackupRestoreAgent;
	CIniFileManager*				iIniFileManager;
	CCntPermanentData*				iPermanentData;

	RFs iFs;
	};

#endif
