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


#ifndef __CINIFILEMANAGER_H__
#define __CINIFILEMANAGER_H__

#include <e32base.h>
#include <s32file.h>

#include <cntdef.h> // for tcontactitemid.

_LIT(KContactsIniFileName, "CntModel.ini");

class CCntDbManagerController;
class CCntServerCurrentItemMap;
class CCntServerSpeedDialManager;


/**
The CIniFileManager class is responsible for storing and retreiving information
to/from the initialisation file.  Data items such as the Current Item, Current
Database and Speed Dials are stored in this file.
*/
class CIniFileManager : public CTimer
	{
public:
	/**
	Binary flags to select streams to save.
	*/
	enum TSaveType { 
		/** Save all streams. */
		ESaveAll = KMaxTUint,
		/** Save Current Item. */
		ESaveCurrentItem = 0x0001,
		/** Save Current Database. */
		ESaveCurrentDatabase = 0x0002,
		/** Save Speed Dials. */
		ESaveSpeedDials = 0x0004,
		/** Save default database drive. */
		ESaveDefaultDbDrive = 0x0008,
		};

public:
	static CIniFileManager* NewL(RFs& aFs, CCntDbManagerController& aDbMgrCtrlr);
	~CIniFileManager();

	TContactItemId CurrentItem(const TDesC& aDatabase) const;
	void SetCurrentItemForDatabaseL(const TDesC& aDatabase, TContactItemId aContactId);
	void RemoveCurrentItemL(const TDesC& aDatabase);
	void SetCurrentItemL(const TDesC& aDatabase, TContactItemId aNewCurrentItem);
	const TDesC& CurrentDb() const;
	void SetCurrentDb(const TDesC& aDb);
	CCntServerSpeedDialManager& SpeedDialManager();	

	void RetryStoreOperation();
	void SaveIniFileSettingsIfRequestedL();

	// During a backup/restore operation schedule saving till later.
	void ScheduleSaveIniFileSettings(TInt aSaveFlags, TBool aReplace=EFalse);

	TInt StartRestoreIniFileSettings();
	void RequestRestoreIniFileSettingsL();
	
	void GetIniFileNameL(TDes& aFileName, TBool aIncPrivatePath=ETrue);
	void SetDefaultDatabaseDrive(TDriveUnit aDriveUnit, TBool aDriveSet=ETrue);
	TBool& DatabaseDriveSet();	

private:
	CIniFileManager(RFs& aFs, CCntDbManagerController& aDbMgrCtrlr);
	void ConstructL();

	void RunL();
	TInt RunError(TInt aError);

	void RestoreIniFileSettingsL();
	void SaveIniFileSettingsL(TInt aSaveFlags, TBool aReplace);
	CDictionaryFileStore* IniFileStoreLC(TFileName& aIniFile);
	void RestoreCurrentDatabaseL(CDictionaryFileStore& aStore);
	void RestoreDefaultDbDriveL(CDictionaryFileStore& aStore);
	void StoreCurrentDatabaseL(CDictionaryFileStore& aStore);
	void StoreDefaultDbDriveL(CDictionaryFileStore& aStore);
	
private:
	/**
	Internal flags.
	*/
	enum TBackupFlags
		{
		ENoSaveRestore,
		/** Indicates restoring data from initialisation file. */
		EIsRestoring,
		/** There is a need to save data to the initialisation file.  If RunL()
		sees this flag it will try to save to file, and retry a number of times
		if an error is encountered. */
		ERequestSave
		};
	
private:
	RFs& iFs;
	CCntDbManagerController& iDbMgrCtrlr;
	TBackupFlags iBackupFlag;
	TInt iSaveType;
	TBool iReplace;
	CCntServerCurrentItemMap* iCurrentItemMap;
	CCntServerSpeedDialManager* iSpeedDialManager;
	TFileName	iCurrentDb;
	TBool		iDatabaseDriveSet;
	TDriveUnit	iDefaultDriveUnit;
	};

	
#endif //__CINIFILEMANAGER_H__
