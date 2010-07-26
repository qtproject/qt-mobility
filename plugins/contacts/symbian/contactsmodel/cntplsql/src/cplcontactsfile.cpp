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


/**
 @file
 @internalComponent
 @released
*/

#include "persistencelayerimpl.h"
#include "clplcontactproperties.h"
#include "cntfilesearch.h"
#include <bautils.h>
#include "persistencelayer.h"

const TInt KDriveNameWidth = 2; // e.g. "C:"

_LIT(KSqLiteFilePrefix, "SQLite__");

// Transaction constants (only used by CPplContactsFile)
_LIT(KStartTransaction,"BEGIN TRANSACTION;");
_LIT(KCommitTransaction,"COMMIT;");
_LIT(KRollbackTransaction,"ROLLBACK;");

/**
@SYMPatchable
@publishedPartner
@released

Patchable optional constant that overrides the cache size of the Contacts model databases set in the 
SqlServer.cfg file in the SQL data cage.
If this value is left at 0, the cache size from the SqlServer.cfg file is used.
The cache size affects the size of the cache in Kb that opens and creates Contacts model files.
See SQLite documentation for more details.

The constant can be changed at ROM build time using patchdata OBY keyword.
*/
IMPORT_C extern const TInt KContactsModelSqliteDbCacheSize;

const TInt KCacheDataSize = 8; // The size in chars of patchable constant 'KContactsModelSqliteDbCacheSize' passed in 

//configure string used to create and open sqlite database
_LIT8(KSqliteCacheSize, "cache_size=%d");

/**
CPplContactsFile NewL.
*/
CPplContactsFile* CPplContactsFile::NewL(CLplContactProperties& aProps, MContactDbObserverV2* aObserver)
	{
	CPplContactsFile* self = new (ELeave) CPplContactsFile(aProps, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
CPplContactsFile ConstructL.
*/	
void CPplContactsFile::ConstructL()
	{
	iItemManager = CPplContactItemManager::NewL(iDatabase, *this, iContactProperties, iIccContactStore);
	iContactProperties.SetContactItemManagerL(*iItemManager);
	
	iConfigureStr = NULL;
	if(KContactsModelSqliteDbCacheSize > 0)
		{
		//Create configure string to be used when creating/opening database
		iConfigureStr = HBufC8::NewL(KSqliteCacheSize().Length() + KCacheDataSize);
		TPtr8 ptrConfigureStr = iConfigureStr->Des();
		ptrConfigureStr.Format(KSqliteCacheSize(), KContactsModelSqliteDbCacheSize);
		}
	}
	
	
/**
CPplContactsFile constructor.
*/
CPplContactsFile::CPplContactsFile(CLplContactProperties& aProps, MContactDbObserverV2* aObserver)
	:
	iActive(EFalse),
 	iDbObserver(aObserver),
 	iContactProperties(aProps),
 	iIccContactStore(aProps)
	{
	}

/**
CPplContactsFile destructor.
*/
CPplContactsFile::~CPplContactsFile()
	{
  	Close();
	delete iItemManager;
	delete iConfigureStr;
	}


/**
Set the database observer for event propagation.
*/
void CPplContactsFile::RegisterDbObserver(MContactDbObserverV2& aDbObserver)
	{
	iDbObserver = &aDbObserver;	  
	}

/**
Utility method used to construct physical path for a given file name

@param aPhysicalFileName in/out paramter; file name to which physical path will be append
@param aSecuredFileName secured file name
*/
void CPplContactsFile::GetPhysicalFileNameL(TDes& aPhysicalFileName, const TDesC& aSecuredFileName)
	{
	GetPhysicalPathL(aPhysicalFileName, aSecuredFileName);
	aPhysicalFileName.Append(KSqLiteFilePrefix);
	aPhysicalFileName.Append(aSecuredFileName.Right(aSecuredFileName.Length() - KDriveNameWidth));
	}

/**
Utility method used to construct physical path for a given file name

@param aPhysicalFileName in/out paramter; file name to which physical path will be append
@param aSecuredFileName secured file name
*/
void CPplContactsFile::GetPhysicalPathL(TDes& aPhysicalFileName, const TDesC& aSecuredFileName)
	{
	const TUint16 KDriveDelimiter = ':';

	// Check for the drive delimiter.
	__ASSERT_ALWAYS(aSecuredFileName.Length() > 2 &&
		aSecuredFileName[1] == KDriveDelimiter, User::Leave(KErrBadName)); 

	// Check for malformed file name (path included explicitly).
	__ASSERT_ALWAYS(aSecuredFileName.Locate('\\') == KErrNotFound, User::Leave(KErrBadName)); 

	// Get the private path from the file session.
	const TInt KMaxPrivatePathLength = 32;
	TBuf<KMaxPrivatePathLength> privatePath;
	
  	LocalFsL();
  	iLocalFs.PrivatePath(privatePath);
	
	aPhysicalFileName = aSecuredFileName.Left(KDriveNameWidth);
		
	// Set current drive.
	iDatabaseDrive = aPhysicalFileName;

	aPhysicalFileName.Append(privatePath);

  	// Make sure private path exists.
  	TInt err = iLocalFs.MkDirAll(aPhysicalFileName);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	}

/**
Open the given database file.
*/
void CPplContactsFile::OpenL(const TDesC& aFileName, TBool aNotify)
	{
	// Contact databases are in the Contact model private directory.  
	TFileName fileName;
	GetPhysicalFileNameL(fileName, aFileName);
	
	iDatabase.Close();
	iDatabase.OpenL(fileName, iConfigureStr);

    iItemManager->SynchronizePredSearchTableL();
	
	iFileIsOpen = ETrue;
	GenerateNotificationEventL(aNotify);	
	}

/**
Utility method used to generate EContactDbObserverEventTablesOpened event
*/
void CPplContactsFile::GenerateNotificationEventL(TBool aNotify)
	{
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CPplContactsFile::GenerateNotificationEventL"));
	#endif 
	
	// Generate an event - this event is generated if this was an explicit call from the
 	// server to opentables or recover database - ie A call on the contactsDatabase API
 	if (iDbObserver && aNotify)
 		{
 		TContactDbObserverEventV2 event;
		event.iType = EContactDbObserverEventTablesOpened;
		event.iContactId = 0;
 		event.iConnectionId = 0;
 		event.iTypeV2 = EContactDbObserverEventV2Null;
 		event.iAdditionalContactId = 0;
 		TRAP_IGNORE(iDbObserver->HandleDatabaseEventV2L(event));
 		}
	}


/**
  Open the database tables (a state change, no action on file necessary).
*/
void CPplContactsFile::OpenTablesL(TBool aNotify)
  	{
  	// Do nothing but notify
  	
  	#if defined(__PROFILE_DEBUG__)
  	    _LIT(KMessage, "[CNTMODEL] MTD: CPplContactsFile::OpenTablesL");
  		RDebug::Print(KMessage);
  	#endif 
  		
	iFileIsOpen = ETrue;
 	GenerateNotificationEventL(aNotify);		  
   	}

	
/**	
  Close the database tables leaving the iDatabase itself open.
  Despite the trailing 'L', this method cannot leave.
*/
void CPplContactsFile::CloseTablesL(TBool aNotify)
	{
	// Do nothing but notify
  
 	if (iDbObserver && aNotify) // Observer is optional.
  		{
  		// Notify observers that table is closed.
  		TContactDbObserverEventV2 event;
  		event.iType = EContactDbObserverEventTablesClosed;
  		event.iContactId = 0;
  		event.iConnectionId = 0;
        event.iTypeV2 = EContactDbObserverEventV2Null;
        event.iAdditionalContactId = 0;
  		TRAP_IGNORE(iDbObserver->HandleDatabaseEventV2L(event));	
  		}
  	}
	

/**
File cleanup class used if creating database and its tables leaves.
*/
class TFileCleanup
	{
public:
	TFileCleanup(RSqlDatabase& aDb, RFs& aFs, const TDesC& aFileName) : iDb(aDb),iFs(aFs),iFileName(aFileName) {}
	static void Cleanup(TAny* aSelf) { STATIC_CAST(TFileCleanup*,aSelf)->DoCleanup(); }

private:
	void DoCleanup()
		{
		iDb.Close();
		(void)iFs.Delete(iFileName);
		}

public:
	RSqlDatabase& iDb;
	RFs& iFs;
	const TDesC& iFileName;
	};


/**
Create a new database.
*/
EXPORT_C void CPplContactsFile::CreateL(const TDesC& aFileName, TPlCreateMode aMode)
	{
	TFileName fileName;
	GetPhysicalFileNameL(fileName, aFileName); 

	TUint attVal;
	LocalFsL();
	TInt err = iLocalFs.Att(fileName, attVal);
	TBool fileExists = (err == KErrNone);
	
	if (fileExists)
		{
		switch (aMode)
			{
			case EPlLeaveIfExist:
				User::Leave(KErrAlreadyExists);
				break;
				
			case EPlOverwrite:
				err = iLocalFs.Delete(fileName);
				break;
			}
		}
	
	// If the database is not created propertly delete the database file using
	// the cleanup item.
	TFileCleanup cleanupData(iDatabase, iLocalFs, fileName);
	CleanupStack::PushL(TCleanupItem(TFileCleanup::Cleanup,&cleanupData)); 

	if ((err != KErrNone) && (err != KErrNotFound)) 
		{
		User::LeaveIfError(err);
		}
	
	User::LeaveIfError(iDatabase.Create(fileName, iConfigureStr));
  	iItemManager->CreateTablesL();
  	iContactProperties.SystemTemplateManager().RecreateSystemTemplateL();

	CleanupStack::Pop(); // The TCleanupItem.
	
	iDatabase.Close();  
	}


/**
Delete the given database.
*/
void CPplContactsFile::DeleteL(const TDesC& aFileName)
	{
	TFileName fileName;
	GetPhysicalFileNameL(fileName, aFileName);	
	
	User::LeaveIfError(iLocalFs.Delete(fileName));  
	iFileIsOpen = EFalse;
	}


/**
Get a list of the contacts database on the given drive.
*/
CDesCArray* CPplContactsFile::ListL(TDriveUnit* aDriveUnit)  
	{
 	LocalFsL();
  	return CCntFileScanner::ListFilesL(iLocalFs, aDriveUnit);
	}


/**
Close the database.
*/
void CPplContactsFile::Close(TBool aNotify)
	{	 		
  	iDatabase.Close(); 
  	iFileIsOpen = EFalse;
  
  	REComSession::FinalClose(); // This line is necessary to make sure the plug-in is unloaded properly
  	iLocalFs.Close(); //we now use a local File Session	
	
	iIccContactStore.Close();
	
	TRAP_IGNORE(CloseTablesL(aNotify) ); // CloseTablesL() cannot leave anyway but still 
										 // trap in case implementation changes later
	}


/**
Starts a new transaction if one is not currently active otherwise leave.
*/
void CPplContactsFile::StartTransactionL()
	{
	if (iActive)  
		{
		User::Leave(KErrInUse);
		}
	User::LeaveIfError(iDatabase.Exec(KStartTransaction));
	iIccContactStore.StartTransactionL();   
	iActive = ETrue;
	}


/**
Commit all changes made since the transaction was started.
*/
void CPplContactsFile::CommitCurrentTransactionL(TUint aSessionId)
	{

	if (!iActive)  
		{
		User::Leave(KErrNotReady);
		}
	iIccContactStore.CommitCurrentTransactionL(aSessionId);  
	User::LeaveIfError(iDatabase.Exec(KCommitTransaction));
	iActive = EFalse;	
	}


/**
Rollback all changes made since the active the transaction was started.
*/
void CPplContactsFile::RollbackCurrentTransactionL(TUint aSessionId)
	{
	// Check that the database is open before rolling back.
	if (iFileIsOpen && iActive)
		{
		iActive = EFalse;
		iIccContactStore.RollbackCurrentTransaction(aSessionId); 
		User::LeaveIfError(iDatabase.Exec(KRollbackTransaction)); 
		}
	}
	
/**
Gets the size of the database file in bytes.

@return The size of the contact database.
*/
TInt CPplContactsFile::FileSize() const
 	{
  	return iDatabase.Size();
  	}	
	

/**
Gets the current database drive.  The database drive is the drive on which 
the default contact database is located.

@param aDriveUnit On return, contains the database drive.
*/
void CPplContactsFile::DatabaseDrive(TDriveUnit& aDriveUnit)
	{
	aDriveUnit = iDatabaseDrive;
	}


/**
A static method to determine if a given Contacts database exists.

@param	aFileName The Contacts database to search for.

@return ETrue if the Contacts database is found, EFalse otherwise.
*/
TBool CPplContactsFile::DatabaseExistsL(const TDesC& aFileName)
	{
	TFileName fileName;
	GetPhysicalFileNameL(fileName, aFileName);
	LocalFsL();
	return BaflUtils::FileExists(iLocalFs, fileName);
	}

