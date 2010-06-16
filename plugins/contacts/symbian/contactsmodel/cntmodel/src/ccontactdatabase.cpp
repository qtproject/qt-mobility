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


#include <cntitem.h>
#include <f32file.h>
#include <cntfldst.h>   //for ccontactfieldstorage used in setfieldasspeeddiall
#include <phbksync.h>
#include <cntdb.h>

#include "cntstd.h"
#include "rcntmodel.h"
#include "ccontactprivate.h"

#include "clplproxyfactory.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#include "cntsyncecom.h"

const TInt KMajorVersion=1;

const TInt KMinorVersion=0;

const TInt KBuildNumber=40;
#endif

CContactDatabase::CContactDatabase():
		iDbConnectionState(EDbConnectionNotReady),
		iTablesOpen(EFalse),
		iDbViewContactType(KUidContactItem),
		iContactSynchroniser(NULL)
	{
	// Needed to ensure unicode sorting / inserting into sorted lists works the
	// same as er5 i.e. that it encludes all spaces and punctuation.
	iCollateMethod = *Mem::CollationMethodByIndex(0);
	iCollateMethod.iFlags|=TCollationMethod::EIgnoreNone;
	}


/**
Frees all resources owned by the contact database, prior to its destruction.
*/
EXPORT_C CContactDatabase::~CContactDatabase()
	{
	delete iConv;
	delete iProxyFactory;
	delete iView;
	delete iAllFieldsView;
	delete iCardTemplateIds;
	delete iGroupIds;
	delete iTemplateCache;
	delete iTextDef;
	if (iCntSvr)
		{
		iCntSvr->RemoveObserver(*iDataBaseObserver);
		iCntSvr->Close();
		}
	if (iContactSynchroniser)
		{
		iContactSynchroniser->Release();
		iContactSynchroniser = NULL;
		}
	delete iDataBaseObserver;
	delete iCntSvr;
	delete iIdleSorter;
	delete iSortedItems;
	delete iSortOrder;
	REComSession::FinalClose();
	}


/**
Create a new CContactDatabase object 
*/
CContactDatabase* CContactDatabase::NewLC()
	{
	CContactDatabase* db =new(ELeave) CContactDatabase();
	CleanupStack::PushL(db);
	db->ConstructL();
	return db;
	}


void CContactDatabase::ConstructL()
	{
	iCntSvr = new (ELeave) RCntModel;
	iCntSvr->ConnectL();
	iProxyFactory = CProxyFactory::NewL(*this);
	CreateViewDefL();
	iTemplateCache = CCntTemplateCache::NewL(*iCntSvr);
	iDataBaseObserver = CDataBaseChangeObserver::NewL(*this);
	iCntSvr->AddObserverL(*iDataBaseObserver);
	iIdleSorter = CCntIdleSorter::NewL(*this);
	}


void CContactDatabase::CreateViewDefL()
	{
	// The exact same object is created in the Session on the server. If
	// it is changed here it must also be changed in the server code.
	CContactItemViewDef* itemDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	itemDef->AddL(KUidContactFieldMatchAll);
	iView = CContactViewDef::NewL(itemDef);
	CleanupStack::Pop(itemDef);

	// iAllFieldView is not created on the server code - an exported getter method
	// allows the user to modify this member and so it cannot be mirrored on the
	// server
	iAllFieldsView = CContactItemViewDef::NewL(CContactItemViewDef::EMaskFields,CContactItemViewDef::EIncludeHiddenFields);
	}


/**
For BC.

@internalComponent
*/
EXPORT_C CContactDatabase* CContactDatabase::LockServerConnectL(const TDesC& /*aFileName*/)
	{
	return NULL;
	}


/**
For BC.

@internalComponent
*/
EXPORT_C CContactDatabase* CContactDatabase::LockServerConnectL(const TDesC& /*aFileName*/, TInt /*aOperation*/)
	{
	return NULL;
	}


/**
For BC.

@internalComponent
*/
EXPORT_C TInt CContactDatabase::LockServerCallBackL(TUint /*aServerOperation*/)
	{
	return NULL;
	}


/**
For BC.

@internalComponent
*/
EXPORT_C void CContactDatabase::LockServerCleanup()
	{
	}


/**
Gets the file name of the default contact database.

By default it is on drive C: but this can be changed using SetDatabaseDriveL().

@capability None

@param aDes On return, contains the drive and filename of the default 
contact database. From v9.0 onwards, this has the form driveletter:filename, 
in other words, it does not include a path.

@see CContactDatabase::SetDatabaseDriveL()
@see CContactDatabase::DatabaseDrive()
@see CContactDatabase::DefaultContactDatabaseExistsL()
*/
EXPORT_C void CContactDatabase::GetDefaultNameL(TDes& aDes)
	{
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->DefaultDatabase(aDes));
	CleanupStack::PopAndDestroy(db);	
	}


/**
Deletes the default contact database.

@capability WriteUserData

@leave	KErrInUse Another client has the database open.
@leave	KErrNotFound The database file was not found or it did not have the correct UIDs.

@see CContactDatabase::DeleteDatabaseL()
@see CContactDatabase::GetDefaultNameL()
@see CContactDatabase::DefaultContactDatabaseExistsL()
*/
EXPORT_C void CContactDatabase::DeleteDefaultFileL()
	{
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->DeleteDatabase(KNullDesC));
	CleanupStack::PopAndDestroy(db);
	}


/**
Gets the current database drive. The database drive is the drive on which 
the default contact database is located. Note: this function can leave.

@capability None

@param aDriveUnit On return, contains the database drive.

@return ETrue if the database drive has been set using SetDatabaseDriveL(). 
Otherwise EFalse and in this case, the function returns drive c: in aDriveUnit 
as the current drive.

@leave KErrNoMemory Out of memory.

@see CContactDatabase::SetDatabaseDrive()
@see CContactDatabase::GetDefaultNameL()
*/
EXPORT_C TBool CContactDatabase::DatabaseDrive(TDriveUnit& aDriveUnit)
	{
	CContactDatabase* db = NewLC(); // this can leave
	TBool ret = db->iCntSvr->DatabaseDrive(aDriveUnit);
	CleanupStack::PopAndDestroy(db);
	return ret;
	}


/**
Sets the contact database drive and optionally moves the default contact database 
from its current location to the new drive. This function guarantees an all 
or nothing operation. If the database is not successfully moved, the drive 
setting is not updated to reflect the change.

In v8.1 when copying the file is moved to \\system\\data on the specified 
drive, and if the destination file already exists it is replaced.

From v9.0 onwards the file copying goes to the correct data caged directory
on the destination drive. If the destination file already exists the copy 
fails.

@capability WriteUserData

@param	aDriveUnit
		The drive to which to move the database.
@param	aCopy
		ETrue moves the existing file to the specified drive. Deletion of
		the source file will fail if it is in use.
		EFalse does not move the file.

@leave KErrNotReady There is no media present in the drive.
@leave KErrInUse The destination file for the copy is already open.
@leave KErrNotFound The source file for the copy was not found.
@leave KErrAlreadyExists The destination file for the copy already exists, (v9.0).

@see CContactdatabase::DatabaseDrive()
@see CContactDatabase::GetDefaultNameL()
@see CContactDatabase::DefaultContactDatabaseExistsL()
*/
EXPORT_C void CContactDatabase::SetDatabaseDriveL(TDriveUnit aDriveUnit, TBool aCopy)
	{
	CContactDatabase* db = NewLC();
	db->iCntSvr->SetDatabaseDriveL(aDriveUnit, aCopy);
	CleanupStack::PopAndDestroy(db); 
	}


/**
Opens the default contact database.

Note: clients should not assume any knowledge of the default database name or location 
because they may be changed in future releases.

@capability ReadUserData

@param  aAccess The default (ESingleThread) allows access to the session with the 
database server from a single client thread only (as in v5 and v5.1). EMultiThread 
allows multiple client threads to share the same session with the server. As 
multi-threaded programs are uncommon in Symbian OS, this argument can be ignored 
by most C++ developers

@return Pointer to the open contact database. 

@leave	KErrNotFound The database file was not found or it did not have the correct UIDs.
@leave	KErrLocked Another client is writing to the database. 
@leave	KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C CContactDatabase* CContactDatabase::OpenL(TThreadAccess aAccess)
	{
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->OpenDatabase(KNullDesC));
	if( aAccess == EMultiThread )
		{
		db->iCntSvr->ShareAuto();
		} 
	db->FetchGroupAndTemplateListsL();
	CleanupStack::Pop(db);
	return db;
	}


/**
Opens a named contact database.

From v9.0 onwards, contact databases can only be located in the correct data caged 
subdirectory. The filenames must have no path, for example c:contacts.cdb.
The maximum length for the drive, filename and extension is 190 characters, and empty
string is not accepted.

@capability ReadUserData

@param	aFileName The filename of the database to open.
@param  aAccess The default (ESingleThread) allows access to the session with the 
database server from a single client thread only (as in v5 and v5.1). EMultiThread 
allows multiple client threads to share the same session with the server. As 
multi-threaded programs are uncommon in Symbian OS, this argument can be ignored 
by most C++ developers

@leave	KErrNotFound The database file was not found or it did not have the correct UIDs.
@leave	KErrLocked Another client is writing to the database.
@leave	KErrBadName The filename is invalid; for example it contains 
wildcard characters or the drive is missing.
@leave	KErrDiskFull The disk does not have enough free space to perform the operation.
@leave  KErrArgument if the given descriptor contains more than the maximum length 
        of 190 characters.

@return A pointer to the open contact database.
*/
EXPORT_C CContactDatabase* CContactDatabase::OpenL(const TDesC& aFileName, TThreadAccess aAccess)
	{
	__ASSERT_ALWAYS(aFileName.Length() != 0, User::Leave(KErrBadName) );
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->OpenDatabase(aFileName));
	if( aAccess == EMultiThread )
		{
		db->iCntSvr->ShareAuto();
		} 
	db->FetchGroupAndTemplateListsL();
	CleanupStack::Pop(db);
	return db;
	}


/** 
Creates and opens an empty contact database, replacing the existing default 
database. 

@capability WriteUserData

@param  aAccess The default (ESingleThread) allows access to the session with the 
database server from a single client thread only (as in v5 and v5.1). EMultiThread 
allows multiple client threads to share the same session with the server. As 
multi-threaded programs are uncommon in Symbian OS, this argument can be ignored 
by most C++ developers

@return Pointer to the new contact database.

@leave	KErrInUse Another client has an open connection to the database.
@leave	KErrDiskFull The disk does not have enough free space to perform the operation.
@leave	KErrNoMemory There is no memory to perform the operation.
*/
EXPORT_C CContactDatabase* CContactDatabase::ReplaceL(TThreadAccess aAccess)
	{
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->ReplaceDatabase(KNullDesC));
	if( aAccess == EMultiThread )
		{
		db->iCntSvr->ShareAuto();
		} 
	db->FetchGroupAndTemplateListsL();
	CleanupStack::Pop(db);
	return db;
	}
	
	
/**
Creates and opens an empty contact database, replacing any existing file with 
the same name.

From v9.0 onwards, contact databases can only be located in the correct data caged 
subdirectory. The filenames must have no path, for example c:contacts.cdb.
The maximum length for the drive, filename and extension is 190 characters.

@capability WriteUserData

@param	aFileName The filename of the database to replace.
@param  aAccess The default (ESingleThread) allows access to the session with the 
database server from a single client thread only (as in v5 and v5.1). EMultiThread 
allows multiple client threads to share the same session with the server. As 
multi-threaded programs are uncommon in Symbian OS, this argument can be ignored 
by most C++ developers

@leave	KErrBadName The filename is invalid; for example it contains wildcard characters 
or the drive letter is missing.
@leave	KErrInUse Another client has an open connection to the database.
@leave	KErrDiskFull The disk does not have enough free space to perform the operation.
@leave	KErrNoMemory There is no memory to perform the operation.
@leave  KErrArgument if the given descriptor contains more than the maximum length 
        of 190 characters.

@return A pointer to the new contact database.
*/				
EXPORT_C CContactDatabase* CContactDatabase::ReplaceL(const TDesC& aFileName, TThreadAccess aAccess)
	{
	__ASSERT_ALWAYS(aFileName.Length() != 0, User::Leave(KErrBadName) );
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->ReplaceDatabase(aFileName)); 
	if( aAccess == EMultiThread )
		{
		db->iCntSvr->ShareAuto();
		} 
	db->FetchGroupAndTemplateListsL();
	CleanupStack::Pop(db);
	return db;
	}


/** 
Creates and opens an empty contact database using the default database name. 

@capability WriteUserData

@param  aAccess The default (ESingleThread) allows access to the session with the 
database server from a single client thread only (as in v5 and v5.1). EMultiThread 
allows multiple client threads to share the same session with the server. As 
multi-threaded programs are uncommon in Symbian OS, this argument can be ignored 
by most C++ developers

@return A pointer to the new contact database. 

@leave	KErrAlreadyExists The database already exists.
@leave	KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C CContactDatabase* CContactDatabase::CreateL(TThreadAccess aAccess)
	{
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->CreateDatabase(KNullDesC)); 
	if( aAccess == EMultiThread )
		{
		db->iCntSvr->ShareAuto();
		} 
	db->FetchGroupAndTemplateListsL();
	CleanupStack::Pop  (db);
	return db;	
	}


/**
Creates and opens a named contact database. 

From v9.0 onwards, contact databases can only be located in the correct data caged 
subdirectory. The filenames must have no path, for example c:contacts.cdb.
The maximum length for the drive, filename and extension is 190 characters.

@capability WriteUserData

@param	aFileName The filename of the database to create. 
@param  aAccess The default (ESingleThread) allows access to the session with the 
database server from a single client thread only (as in v5 and v5.1). EMultiThread 
allows multiple client threads to share the same session with the server. As 
multi-threaded programs are uncommon in Symbian OS, this argument can be ignored 
by most C++ developers

@leave	KErrAlreadyExists The database already exists. 
@leave	KErrBadName The filename is invalid; for example it contains wildcard characters 
or the drive letter is missing.
@leave	KErrDiskFull The disk does not have enough free space to perform the operation.
@leave  KErrArgument if the given descriptor contains more than the maximum length 
        of 190 characters.

@return A pointer to the new contact database.
*/
EXPORT_C CContactDatabase* CContactDatabase::CreateL(const TDesC& aFileName,TThreadAccess aAccess)
	{
	__ASSERT_ALWAYS(aFileName.Length() != 0, User::Leave(KErrBadName) );
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->CreateDatabase(aFileName)); 
	if( aAccess == EMultiThread )
		{
		db->iCntSvr->ShareAuto();
		} 
	db->FetchGroupAndTemplateListsL();
	CleanupStack::Pop  (db);
	return db;	
	}


/**
This method is no longer required and should not be called.

Closes all database tables. After a rollback and recover all tables need to 
be closed and re-opened before the database can be accessed again.

@capability WriteUserData
@deprecated
*/
EXPORT_C void CContactDatabase::CloseTables()
	{  	
	TRAP_IGNORE( iCntSvr->CloseTablesL() );   
	}


/** 
This method is no longer required and should not be called.

Opens all database tables. After a rollback and recover all tables need to 
be closed and re-opened before the database can be accessed again.

@capability ReadUserData
@deprecated
*/
EXPORT_C void CContactDatabase::OpenTablesL()
	{
	iCntSvr->OpenTablesL();	
	}


/**
A static method to recreate the system template. 

From v9.0 onwards, contact databases can only be located in the correct data caged 
subdirectory. The filenames must have no path, for example c:contacts.cdb. 
The maximum length for the drive, filename and extension is 190 characters.

@publishedPartner
@released
@capability ReadUserData 
@capability WriteUserData

@param aFileName The contact database filename.

@leave KErrBadName The filename is invalid; for example it contains wildcard characters 
or the drive letter is missing.
@leave  KErrArgument if the given descriptor contains more than the maximum length 
        of 190 characters.
*/
EXPORT_C void CContactDatabase::RecreateSystemTemplateL(const TDesC& aFileName)
	{
	// Workaround for dynamic language switching because the system template may need to be recreated in a different language.
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->OpenDatabase(aFileName)); 
	User::LeaveIfError(db->iCntSvr->ReCreateTemplate());
	CleanupStack::PopAndDestroy(db); 
	}

	
/**
Sets the default view definition. The contact database takes ownership of the 
view definition specified. The default view definition is used in calls to 
ReadContactL(), ReadContactLC() and ReadContactAndAgentL() when no view definition 
is explicitly specified.

@capability WriteUserData

@param aView The view definition. This method does nothing if this is null.
*/
EXPORT_C void CContactDatabase::SetViewDefinitionL(CContactViewDef* aView)
	{
    if (aView)
        {
    	iCntSvr->SetViewDefinitionL(*aView);
    	// We also need to hold the ViewDef here - it is used in merging
    	delete iView;
    	iView = aView;
        }
	}

	
/**
Sets the text definition. The contact database takes ownership of the 
text definition specified.

@param aTextDef The new text definition. 
*/
EXPORT_C void CContactDatabase::SetTextDefinitionL(CContactTextDef* aTextDef)
	{
	delete iTextDef;
	iTextDef = aTextDef;
	}

	
/**
Gets a pointer to the text definition.

@return A pointer to the text definition. 
*/
EXPORT_C const CContactTextDef* CContactDatabase::TextDefinition() const
	{
	return iTextDef;
	}


void CContactDatabase::CleanupDatabaseRollback(TAny *aDatabase)
	{
	ASSERT(aDatabase);
	CContactDatabase* db = static_cast<CContactDatabase*>(aDatabase);
	db->DatabaseRollback();
	}


/**
Starts a new transaction. Places and leaves a cleanup item to rollback
the database on the cleanupstack. 

@publishedPartner
@released

@param aIsInTransaction Used to determine whether or not to start the transaction depending on if a transaction is currently in progress.
@leave KErrDiskFull There is insufficient disk space.
*/
EXPORT_C void CContactDatabase::DatabaseBeginLC(TBool aIsInTransaction)
	{
	if (!aIsInTransaction)
		{
		User::LeaveIfError(iCntSvr->BeginDbTransaction());	
		CleanupStack::PushL(TCleanupItem(CleanupDatabaseRollback, this));
		}
	}


/**
Commits an existing transaction, pops the rollback cleanup item off the CleanupStack.
Closes the database if the connection state is EDbConnectionNeedToCloseForRestore.

@publishedPartner
@released

@param aIsInTransaction Used to determine whether or not to commit the transaction depending on whether a transaction is currently in progress.
*/
EXPORT_C void CContactDatabase::DatabaseCommitLP(TBool aIsInTransaction)
	{
	if (!aIsInTransaction)
		{
		User::LeaveIfError(iCntSvr->CommitDbTransaction());
		CleanupStack::Pop(); // CleanupDatabaseRollback
		}
	}


/**
This function is not currently supported.
@deprecated
*/
EXPORT_C void CContactDatabase::StoreSortOrderL()
	{
	// Does nothing in the new architecture.
	}


/**
This function is not currently supported.
@deprecated
*/
EXPORT_C void CContactDatabase::RestoreSortOrderL()
	{
	//Does nothing in the new architecture
	}


/** 
 Gets the array of sort preferences.
 
 Note: This method can leave.
 
 @deprecated
 @capability None
 @return A pointer to an array of sort preferences of the contact database.
*/
EXPORT_C const CArrayFix<CContactDatabase::TSortPref>* CContactDatabase::SortOrder() const
	{
	return iCntSvr->GetSortPreferenceL(); // this can leave
	}


/** 
Adds a new contact item to the database and returns its ID.

@capability WriteUserData

@param aContact The contact item to add to the database.

@leave KErrDiskFull The disk does not have enough free space to perform the operation.

@return The ID of the new contact item. 
*/
EXPORT_C TContactItemId CContactDatabase::AddNewContactL(CContactItem& aContact)
	{
	return doAddNewContactL(aContact, EFalse, EFalse);
	}


/** 
Adds a new contact item to the database and returns its ID.

@capability WriteUserData

@param aContact The contact item to add to the database.
@param aIsTemplate This argument should be ignored by developers.
@param aIsInTransaction This argument should be ignored by developers.

@leave KErrDiskFull The disk does not have enough free space to perform the operation.

@return The ID of the new contact item. 
*/
EXPORT_C TContactItemId CContactDatabase::doAddNewContactL(CContactItem& aContact,TBool /*aIsTemplate*/,TBool /*aIsInTransaction*/)
	{
	if (aContact.Type() == KUidContactICCEntry)
	    {
		LoadSyncPluginL();
	    }
	
	aContact.iId = iCntSvr->CreateContactL(aContact);

	if (CheckType(aContact.Type()))
		{
		InsertInSortArray(aContact);
		}

	return aContact.iId;
	}


CContactItem* CContactDatabase::doCreateContactGroupLC(const TDesC& aGroupLabel)
	{
	CContactItem* newGroup = CContactGroup::NewLC();
	newGroup->AddLabelFieldL();
	if (aGroupLabel != KNullDesC)
		STATIC_CAST(CContactGroup*, newGroup)->SetGroupLabelL(aGroupLabel);

	newGroup->iId = AddNewContactL(*newGroup);
	RespondToEventL(EContactDbObserverEventGroupAdded, newGroup->iId);
	return newGroup;
	}

	
/**
Creates a new contact group with a default label of 'Group Label' and 
adds it to the database. 

The caller takes ownership of the returned object.

@capability WriteUserData

@param aInTransaction This argument should be ignored by developers.

@return Pointer to the newly created contact group. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateContactGroupL(TBool aInTransaction)
	{
	CContactItem* newGroup = CContactDatabase::CreateContactGroupLC(aInTransaction);
	CleanupStack::Pop(newGroup);
	return newGroup;
	}


/**
Creates a new contact group with a default label of 'Group Label' and 
adds it to the database. 

The caller takes ownership of the returned object.

@capability WriteUserData

@param aInTransaction This argument should be ignored by developers.

@return Pointer to the newly created contact group. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateContactGroupLC(TBool /*aInTransaction*/)
	{
	return doCreateContactGroupLC();
	}

	
/** 
Creates a new contact group with a specified label and adds it to the database. 

The pointer to the group is left on the cleanup stack. The caller takes 
ownership of the returned object.

@capability WriteUserData

@param aGroupLabel The string to set as the group label.
@param aInTransaction This argument should be ignored by developers.

@leave KErrDiskFull The disk does not have enough free space to perform the operation.

@return Pointer to the newly created contact group. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateContactGroupLC(const TDesC& aGroupLabel, TBool /*aInTransaction*/)
	{
	return doCreateContactGroupLC(aGroupLabel);
	}


/** 
Creates a new contact group with a specified label and adds it to the database. 

The caller takes ownership of the returned object.

@capability WriteUserData

@param aGroupLabel The string to set as the group label.
@param aInTransaction This argument should be ignored by developers.

@return Pointer to the newly created contact group. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateContactGroupL(const TDesC& aGroupLabel,TBool aInTransaction)
	{
	CContactItem* newGroup = CContactDatabase::CreateContactGroupLC(aGroupLabel, aInTransaction);
	CleanupStack::Pop(newGroup);
	return newGroup;
	}

	
/** 
Creates a contact card template based on the system template and adds it to the database.

A template label must be specifed. This can be changed later using CContactCardTemplate::SetTemplateLabelL(). 

The pointer to the template is left on the cleanup stack. The caller takes ownership of the returned object.

@capability WriteUserData

@param aTempLabel The string to set as the template label. 
@param aInTransaction This argument should be ignored by developers.

@leave KErrDiskFull The disk does not have enough free space to perform the operation.

@return Pointer to the contact card template. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateContactCardTemplateLC(const TDesC& aTempLabel, TBool aInTransaction)
	{
	return CreateContactCardTemplateLC(&iTemplateCache->SystemTemplateL(), aTempLabel, aInTransaction);
	}


/** 
Creates a contact card template based on the system template and adds it to the database.

A template label must be specifed. This can be changed later using CContactCardTemplate::SetTemplateLabelL(). 

The caller takes ownership of the returned object.

@capability WriteUserData

@param aTempLabel The string to set as the template label. 
@param aInTransaction This argument should be ignored by developers.

@return Pointer to the contact card template. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateContactCardTemplateL(const TDesC& aTempLabel, TBool aInTransaction)
	{
	CContactItem *cntItem = CreateContactCardTemplateLC(&iTemplateCache->SystemTemplateL(), aTempLabel, aInTransaction);
	CleanupStack::Pop(cntItem);
	return cntItem;
	}

	
/** 
Creates a contact card template and adds it to the database.
 
The new template's field set is based on the specified contact item. This could be a 
contact card, an own card, another template or even a group. Note that no field data 
is copied into the new template. All of the new template's fields are marked as template 
fields.

A template label must be specifed. This can be changed later using CContactCardTemplate::SetTemplateLabelL(). 

The caller takes ownership of the returned object.

@capability WriteUserData

@param aTemplate Pointer to an instance of a CContactItem-derived class. This 
is used to initialise the new template's field set.
@param aTemplateLabel The string to set as the template label.
@param aInTransaction This argument should be ignored by developers.

@return Pointer to the contact card template. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateContactCardTemplateL(const CContactItem* aTemplate,const TDesC& aTempLabel,TBool aInTransaction)
	{
	CContactItem *cntItem = CreateContactCardTemplateLC(aTemplate, aTempLabel, aInTransaction);
	CleanupStack::Pop(cntItem);
	return cntItem;
	}


/** 
Creates a contact card template and adds it to the database.
 
The new template's field set is based on the specified contact item. This could be a 
contact card, an own card, another template or even a group. Note that no field data 
is copied into the new template. All of the new template's fields are marked as template 
fields.

A template label must be specifed. This can be changed later using CContactCardTemplate::SetTemplateLabelL(). 

The pointer to the object is left on the cleanup stack. The caller takes ownership of it.

@capability WriteUserData

@param aTemplate Pointer to an instance of a CContactItem-derived class. This 
is used to initialise the new template's field set.
@param aTemplateLabel The string to set as the template label.
@param aInTransaction This argument should be ignored by developers.

@leave KErrDiskFull The disk does not have enough free space to perform the operation.

@return Pointer to the contact card template. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateContactCardTemplateLC(const CContactItem* aTemplate, const TDesC& aLabel, TBool /*aInTransaction*/)
	{
	CContactItem* newTemplate = CContactCardTemplate::NewLC(aTemplate); 
	// Clear all data from the new template
	newTemplate->ClearFieldContent();

	// Add label field
	newTemplate->AddLabelFieldL();
	static_cast<CContactCardTemplate*>(newTemplate)->SetTemplateLabelL(aLabel);

	// Create the contact in the database	
	newTemplate->iId = AddNewContactL(*newTemplate);
	AddToTemplateListL(newTemplate->Id());
	return newTemplate;
	}


void CContactDatabase::AddToTemplateListL(const TContactItemId aNewTemplateId)
	{
	if (!iCardTemplateIds)
		{
		iCardTemplateIds = CContactIdArray::NewL();
		}
	RespondToEventL(EContactDbObserverEventTemplateAdded, aNewTemplateId);
	}


void CContactDatabase::RemoveFromTemplateList(const TContactItemId aTemplateId)
	{	
	if (iCardTemplateIds)
 		{
		TInt templatePos =	iCardTemplateIds->Find(aTemplateId);
		if (templatePos != KErrNotFound)
			{
			iCardTemplateIds->Remove(templatePos);
			}
 		}	
	}


/**
Gets a copy of the database's template ID list. This is a list of the IDs 
of all contact card templates which have been added to the database. The caller 
takes ownership of the returned object.

@return Pointer to a copy of the database's template ID list. This does 
not include the system template. NULL if there are no templates in the database. 
*/
EXPORT_C CContactIdArray* CContactDatabase::GetCardTemplateIdListL() const
	{
	if (iCardTemplateIds)
		{
		CContactIdArray* copyArray = CContactIdArray::NewL(iCardTemplateIds);
		return copyArray;
		}
	else
		return NULL;
	}


void CContactDatabase::ReadTemplateIdsL()
	{
	const_cast<CContactDatabase*>(this)->FetchGroupAndTemplateListsL();
	}
	

/** Determines if the System template fields are valid.  Valid means that no
fields contain data.
@internalTechnology
@released
@capability None
@param aContact Contact item representing the System template.
@return ETrue if the System template fields are valid, EFalse otherwise.
*/
TBool CContactDatabase::SystemTemplateFieldsValid( const CContactItem& aContact )
	{
	// Precondition: aContact's ID must match the System template's ID.
	__ASSERT_ALWAYS( aContact.Id() == TemplateId(), Panic(ECntPanicSystemTemplate) );

	TBool fieldsValid = ETrue;
	CContactItemFieldSet& fieldSet = aContact.CardFields();

	for( TInt ii = 0; ii < fieldSet.Count(); ii++ )
		{
		CContactItemField& field = fieldSet[ii];
		CContactFieldStorage* fieldStorage = field.Storage();
		//
		// If the field contains data (is "full") then the System template
		// fields are not valid.
		//
		if( fieldStorage->IsFull() )
			{
			fieldsValid = EFalse;
			break;
			}
		}
	
	return fieldsValid;
	}


/**
Returns a copy of the database's group ID list. This is a list which contains 
the contact item IDs for each group in the database. The caller takes ownership 
of the returned object.

@return Pointer to an array containing the contact item IDs for each group 
in the database. NULL if there are no groups in the database. 
*/
EXPORT_C CContactIdArray* CContactDatabase::GetGroupIdListL() const
	{
	const_cast<CContactDatabase*>(this)->FetchGroupAndTemplateListsL();
	CContactIdArray* copyArray = CContactIdArray::NewL(iGroupIds);
	return copyArray;
	}


/** 
Sets a contact item in the database to be a member of a contact group.

The item and group are identified by their IDs. 

@capability WriteUserData

@param aItemId The ID of the item to add to the group.
@param aGroupId  The ID of the group to which the item should be added. 

@leave KErrNotFound Either the group or the item does not exist.
@leave KErrNotSupported The group is not of type KUidContactGroup. 
@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::AddContactToGroupL(TContactItemId aItemId, TContactItemId aGroupId)
	{
	//Remember if the group was already opened, if so open it at the end of the method
	TBool isAlreadyOpened = iCntSvr->CloseContact(aGroupId);

	CContactItem* cntGroup = OpenNoMergeLCX(aGroupId); //double push
	
	if (cntGroup->Type() != KUidContactGroup)
		User::Leave(KErrNotSupported);
	AddCntToOpenedGroupL(aItemId, *cntGroup);

	CleanupStack::PopAndDestroy(cntGroup);	// cntGroup
	CleanupStack::Pop(); // Pop the lock
	cntGroup = NULL;
	if (isAlreadyOpened)
		{
		CContactItem* dummy = OpenContactL(aGroupId);
		delete dummy;				
		}
	}


/** 
Sets a contact item in the database to be a member of a contact group.

The item and group are identified by their IDs. 

@capability WriteUserData

@param aItemId The ID of the item to add to the group.
@param aGroupId  The ID of the group to which the item should be added. 
@param aInTransaction This argument should be ignored by developers.

@leave KErrNotFound Either the group or the item does not exist.
@leave KErrNotSupported The group is not of type KUidContactGroup. 
@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::AddContactToGroupL(TContactItemId aItemId, TContactItemId aGroupId, TBool /*aInTransaction*/)
	{
	AddContactToGroupL(aItemId, aGroupId);	
	}

	
/** 
Sets a contact item in the database to be a member of a contact group. 

@capability WriteUserData

@param aItem The item to add to the group.
@param aGroup  The group to which the item should be added. 

@leave KErrNotFound Either the group or the item does not exist in the database.
@leave KErrNotSupported The group is not of type KUidContactGroup.
@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::AddContactToGroupL(CContactItem& aItem, CContactItem& aGroup)
	{
	if (aGroup.Type() != KUidContactGroup)
		{
		User::Leave(KErrNotSupported);
		}

	AddContactToGroupL(aItem.Id(), aGroup.Id());

	//Update client's group object
	static_cast<CContactGroup&>(aGroup).AddContactL(aItem.Id());

	//Update client's contacts item object
	CContactItemPlusGroup& item = static_cast<CContactItemPlusGroup&>(aItem);
	if (!item.iGroups)
		{
		item.iGroups = CContactIdArray::NewL();
		}
	item.iGroups->AddL(aGroup.Id());
	}


/** 
Removes the association between a contact item and a group.

@capability WriteUserData

@param aItem The item to remove.
@param aGroup The group from which the item should be removed.

@leave KErrDiskFull The disk does not have enough free space to perform the operation. 
*/
EXPORT_C void CContactDatabase::RemoveContactFromGroupL(CContactItem& aItem, CContactItem& aGroup)
	{
	if (aGroup.Type() != KUidContactGroup)
		{
		User::Leave(KErrNotSupported);
		}
	
	RemoveContactFromGroupL(aItem.Id(), aGroup.Id()); //Perform the change in the database

	//Update client's group object
	static_cast<CContactGroup&>(aGroup).RemoveContactL(aItem.Id());

	//Update client's contacts item object
	CContactItemPlusGroup& item = static_cast<CContactItemPlusGroup&>(aItem);
	if (item.iGroups)
		{
		const TInt groupIndex = item.iGroups->Find(aGroup.Id());
		if (groupIndex>=0)
			{
			item.iGroups->Remove(groupIndex);
			}
		}
	}


void CContactDatabase::AddCntToOpenedGroupL(TContactItemId aItemId, CContactItem& aGroup)
	{
	CContactGroup& group = static_cast<CContactGroup&>(aGroup);
	if (!group.ContainsItem(aItemId))
		{
		group.AddContactL(aItemId);
	    iCntSvr->CommitContactL(aGroup);
		}
	else
		{
		iCntSvr->CloseContact(aGroup.Id());
		}
	}


/** 
Removes the association between a contact item and a group.

The item and group are identified by their IDs. 

@capability WriteUserData

@param aItemId The ID of the item to remove.
@param aGroupId The ID of the group from which the item should be removed. 

@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::RemoveContactFromGroupL(TContactItemId aItemId, TContactItemId aGroupId)
	{
	//Remember if the group was already opened, if so open it at the end of the method
	TBool isAlreadyOpened = iCntSvr->CloseContact(aGroupId);
	CContactItem* cntGroup = OpenNoMergeLCX(aGroupId); //double push
	
	if (cntGroup->Type() != KUidContactGroup)
		{
		User::Leave(KErrNotSupported);
		}
	
	CContactGroup* group = static_cast<CContactGroup*>(cntGroup);
	if (group->ContainsItem(aItemId))
		{
		group->RemoveContactL(aItemId);
	    iCntSvr->CommitContactL(*cntGroup);
		}
	else
		{
		iCntSvr->CloseContact(cntGroup->Id());
		}
		
	CleanupStack::PopAndDestroy(2); // cntGroup, CntItemClose
	cntGroup = NULL;
	
	if (isAlreadyOpened)
		{
		CContactItem* dummy = OpenContactL(aGroupId);
		delete dummy;				
		}

	}


/**
Sets a field containing a telephone number as a speed dial field. The field 
is identified by aFieldIndex within the contact item aItem. It is assigned a 
speed dial position between 1 and 9 inclusive.

The field's speed dial and user added attributes are set and the appropriate 
UID (KUidSpeedDialXxx) is added to the field's content type. The changes are 
committed to the database.

Notes:

If an item's telephone number field has already been assigned to position 
aSpeedDialPosition, that item is updated so that the speed dial attribute 
is removed from its field and the speed dial field type UID is removed from 
the field's content type, before the new speed dial field is set.

The speed dial attribute can be tested for using the CContactItemField::IsSpeedDial() 
function.

The contact item passed to this function (aItem) must have been obtained using 
one of the variants of CContactDatabase::OpenContactL(). This is because it 
is modified and committed to the database by this function - no further 
commits are necessary.

@capability ReadUserData
@capability WriteUserData

@param aItem The contact item containing the field to set as a speed dial 
field.
@param aFieldIndex Index of a field in aItem's field set to set as a speed dial field.
@param aSpeedDialPosition The speed dial position. This is an integer in the 
range 1 to 9 inclusive. If outside this range, the function leaves with KErrArgument. 

@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::SetFieldAsSpeedDialL(CContactItem& aItem, TInt aFieldIndex, TInt aSpeedDialPosition)
	{
	//
	// aItem has been merged with the appropriate template.  On the server when
	// the contact is read it is not merged with the template.  Therefore we
	// need to determine an equivalent field index for the server.  The index
	// on the client's side takes account of the template fields so to compute
	// the equivalent index iterate through the contact fields up to and
	// including aFieldIndex and count the number of non-empty (i.e. non-
	// template) fields.  The total number is the equivalent index.
	//
	CContactItemFieldSet &fieldSet = aItem.CardFields();
	TInt svrFieldIndex = -1;
	if(fieldSet.Count() == 0) 
		{
		User::Leave(KErrArgument);
		}
	for( TInt fieldIndex = 0; fieldIndex <= aFieldIndex; fieldIndex++ )
		{
		CContactItemField& field = fieldSet[fieldIndex];
		if( field.Storage()->IsFull() )
			{
			svrFieldIndex++;
			}
		}
		
	
	CContactItemField& field = fieldSet[aFieldIndex];	
	if (field.StorageType() == KStorageTypeText)
		{
		iCntSvr->SetFieldAsSpeedDialL(aItem.Id(), svrFieldIndex, aSpeedDialPosition);
		}
	else	
		{
		User::Leave(KErrArgument);
		}


	//
	// The contact has been read from the database and modified on the server.
	// These changes have not been made to the CContactItem passed in to this
	// function.  We need to make these changes here for consistency with the
	// old model.  A future BC break will disallow this behaviour by changing
	// the parameter from CContactItem& to const CContactItem& thus forcing the
	// client to pass a constant object.
	//

	// Get the field containing the number to be associated with the
	// speed dial.  Note that we use the original index passed in not the
	// equivalent server index.
	CContactItemField& speedDialField = aItem.CardFields()[aFieldIndex];
	// Add speed dial attributes to the contact item field.
	TUid fieldTypeUid = CContactDatabase::SpeedDialFieldUidFromSpeedDialPosition(aSpeedDialPosition);
	if (!speedDialField.ContentType().ContainsFieldType(fieldTypeUid))
		{
		speedDialField.AddFieldTypeL(fieldTypeUid);
		}
	speedDialField.SetUserAddedField(ETrue);
	speedDialField.SetSpeedDial(ETrue);
	}


/**
Returns the field UID for the given speed dial position.  This method is
copied from CCntServerSpeedDialManager::SpeedDialFieldUidFromSpeedDialPosition()
rather than export this method for use here and on the server.  Once the BC
break referred to in SetFieldAsSpeedDialL() has been made this method should be
removed.

@internalTechnology
@released

@param aSpeedDialPosition The speed dial position for which we want the field
UID.

@return The field UID corresponding to aSpeedDialPosition.
*/
TUid CContactDatabase::SpeedDialFieldUidFromSpeedDialPosition(TInt aSpeedDialPosition)
	{
	__ASSERT_ALWAYS(aSpeedDialPosition >= KCntMinSpeedDialIndex && aSpeedDialPosition <= KCntMaxSpeedDialIndex, Panic(ECntPanicInvalidSpeedDialIndex));

	TUid fieldTypeUid = KNullUid;
	switch (aSpeedDialPosition)
		{
	case 1:
		fieldTypeUid = KUidSpeedDialOne;
		break;
	case 2:
		fieldTypeUid = KUidSpeedDialTwo;
		break;
	case 3:
		fieldTypeUid = KUidSpeedDialThree;
		break;
	case 4:
		fieldTypeUid = KUidSpeedDialFour;
		break;
	case 5:
		fieldTypeUid = KUidSpeedDialFive;
		break;
	case 6:
		fieldTypeUid = KUidSpeedDialSix;
		break;
	case 7:
		fieldTypeUid = KUidSpeedDialSeven;
		break;
	case 8:
		fieldTypeUid = KUidSpeedDialEight;
		break;
	case 9:
		fieldTypeUid = KUidSpeedDialNine;
		break;
		}

	return fieldTypeUid; 
	}


/** 
Returns the ID of the contact item whose telephone number field is mapped to 
the speed dial position specified. This function is provided so that information 
may be displayed about a contact item whose telephone number is being dialled 
using speed dialling.

The function also retrieves the telephone number stored in the field.

@capability ReadUserData

@param aSpeedDialPosition The speed dial position. This is an integer in the 
range 1 to 9 inclusive. If outside this range, the function leaves with KErrArgument.
@param aPhoneNumber On return, contains the telephone number which is mapped 
to the speed dial position specified. Returns KNullDesC if the speed dial 
position requested has not been set.

@return The ID of the contact item for which the speed dial has been set. 
*/
EXPORT_C TContactItemId CContactDatabase::GetSpeedDialFieldL(TInt aSpeedDialPosition, TDes& aPhoneNumber)
	{
	return iCntSvr->GetSpeedDialFieldL(aSpeedDialPosition, aPhoneNumber);
	}


/** 
Removes the mapping between a contact item field and a speed dial position.

Removes the KUidSpeedDialXxx UID from the field's content type, removes the 
field's speed dial attribute and commits the changes to the item.

@capability ReadUserData
@capability WriteUserData

@param aContactId The ID of the contact item containing the speed dial field.
@param aSpeedDialPosition The speed dial position. This is an integer in the 
range 1 to 9 inclusive. If outside this range, the function leaves with KErrArgument. 

@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::RemoveSpeedDialFieldL(TContactItemId aContactId, TInt aSpeedDialPosition)
	{
	iCntSvr->RemoveSpeedDialFieldL(aContactId, aSpeedDialPosition);	
	}

	
/**
Reads a contact item without locking it.

This function uses the default view definition (as set by SetViewDefinitionL()). The 
caller takes ownership of the returned object.

@capability ReadUserData

@param aContactId The ID of the contact item to read.

@return Pointer to the contact item.

@leave KErrNotFound The specified contact item does not exist in the database. */
EXPORT_C CContactItem* CContactDatabase::ReadContactL(TContactItemId aContactId)
	{
	CContactItem* cntItem = ReadContactLC(aContactId);
	CleanupStack::Pop(cntItem);
	return cntItem;
	}


/**
Reads a contact item without locking it.

This function uses the view definition specified. The caller takes ownership 
of the returned object.

@capability ReadUserData

@param aContactId The ID of the contact item to read.
@param aViewDef The view definition to use. 

@return Pointer to the contact item.

@leave KErrNotFound The specified contact item does not exist in the database. */
EXPORT_C CContactItem* CContactDatabase::ReadContactL(TContactItemId aContactId, const CContactItemViewDef& aViewDef)
	{
	CContactItem* cntItem = ReadContactLC(aContactId, aViewDef);
	CleanupStack::Pop(cntItem);
	return cntItem;
	}


/**
Reads a contact item without locking it.

This function uses the default view definition (as set by SetViewDefinitionL()). The 
caller takes ownership of the returned object.

@capability ReadUserData

@param aContactId The ID of the contact item to read.

@return Pointer to the contact item. This is left on the cleanup stack.

@leave KErrNotFound The specified contact item does not exist in the database. 
*/
EXPORT_C CContactItem* CContactDatabase::ReadContactLC(TContactItemId aContactId)
	{
	CContactItem* cntItem = iCntSvr->ReadContactL(&iView->ItemDef(), aContactId);
	CleanupStack::PushL(cntItem);

	iTemplateCache->MergeWithTemplateL(*cntItem, &iView->ItemDef());
	
	return cntItem;
	}


/**
Reads a contact item without locking it.

This function uses the view definition specified. The caller takes ownership 
of the returned object.

@capability ReadUserData

@param aContactId The ID of the contact item to read.
@param aViewDef The view definition to use. 

@return Pointer to the contact item. This is left on the cleanup stack.

@leave KErrNotFound The specified contact item does not exist in the database. 
*/
EXPORT_C CContactItem* CContactDatabase::ReadContactLC(TContactItemId aContactId,const CContactItemViewDef& aViewDef)
	{
	CContactItem* cntItem = iCntSvr->ReadContactL(&aViewDef, aContactId);
	CleanupStack::PushL(cntItem);
	iTemplateCache->MergeWithTemplateL(*cntItem, &aViewDef);
	return cntItem;
	}


/**
Reads a contact item and an agent if the item has an agent field. The item 
and agent (if present) are returned in an array. The function uses the database's 
default view definition (as set by SetViewDefinitionL()). The caller takes 
ownership of the returned object.

@capability ReadUserData

@param aContactId The ID of the contact item to read.
@leave KErrNotFound The specified contact item cannot be found in the database.

@return Pointer to an array containing the contact item and agent, if present.
*/
EXPORT_C CArrayPtr<CContactItem>* CContactDatabase::ReadContactAndAgentL(TContactItemId aContactId)
	{
	CArrayPtr<CContactItem>* cntItemsArray = new (ELeave)CArrayPtrFlat<CContactItem>(4);
	CleanupStack::PushL(cntItemsArray);
	CContactItem* contactItem = ReadContactLC(aContactId);
	cntItemsArray->AppendL(contactItem);
	CleanupStack::Pop(contactItem);	// from ReadContactLC
	
	// Determine whether the contactItem needs an agent
	TInt agentIndex = (*cntItemsArray)[0]->Agent();
	if (agentIndex != KErrNotFound)
  		{
  		CContactItem* contactItemAgent = ReadContactLC(agentIndex);
  		cntItemsArray->AppendL(contactItemAgent); // Will use the Viewdef on the server - no need to marshal ViewDef
		CleanupStack::Pop(contactItemAgent);// from ReadContactLC
  		}
	CleanupStack::Pop(cntItemsArray);	// cntItemsArray
	return cntItemsArray;
	}


/**
Reads a contact item (contact card, own card, template, or contact group), 
but does not read the group or template information. 

This function is identical to the variant of ReadContactL() which uses the database's 
default view definition, except that this function does not read:

- the list of group members and the group label (if the item is a CContactGroup)
- the template label (if the item is a CContactCardTemplate)
- the list of groups to which the item belongs, if any (not applicable to templates)
- any fields inherited from a non-system template, if any (not applicable if 
the item is a CContactCardTemplate)

Notes:

This function is faster than the standard reading function (ReadContactL()), 
which needs to match the template fields and groups etc.

The caller takes ownership of the returned object.

@capability ReadUserData

@param aContactId The ID of the contact to read.

@leave KErrNotFound The specified contact item cannot be found in the database.

@return Pointer to the contact whose ID is aContactId.
*/
EXPORT_C CContactItem* CContactDatabase::ReadMinimalContactL(TContactItemId aContactId)
	{
	return iCntSvr->ReadContactL(NULL, aContactId);
	}


/**
Reads a contact item (contact card, own card, template, or contact group), 
but does not read the group or template information. 

This function is identical to the variant of ReadContactLC() which uses the server's 
default view definition, except that this function does not read:
	
- the list of group members and the group label (if the item is a CContactGroup)
- the template label (if the item is a CContactCardTemplate)
- the list of groups to which the item belongs, if any (not applicable to templates)
- any fields inherited from a non-system template, if any (not applicable if 
the item is a CContactCardTemplate)

Notes:

This function is faster than the standard reading function (ReadContactLC()), 
which needs to match the template fields and groups etc.

The caller takes ownership of the returned object.

@capability ReadUserData

@param aContactId The ID of the contact to read.

@leave KErrNotFound The specified contact item cannot be found in the database.

@return Pointer to the contact whose ID is aContactId. The contact is left 
on the cleanup stack. 
*/
EXPORT_C CContactItem* CContactDatabase::ReadMinimalContactLC(TContactItemId aContactId)
	{
	CContactItem* cntItem = iCntSvr->ReadContactL(NULL, aContactId);
	CleanupStack::PushL(cntItem);
	return cntItem;
	}

	
/**
Gets the content type of the template field which a specified field maps onto. 
If the field does not map onto a field in a template, then its own content 
type is returned.

Note: this function can leave.

@param aField The field of interest. 

@return The content type of the field. 
*/
EXPORT_C const CContentType& CContactDatabase::TemplateContentType(const CContactItemField& aField) const
	{
	return(aField.TemplateContentType(iTemplateCache->SystemTemplateL().CardFields())); // this can leave
	}


//
// Logic cut and paste with minor edit from old CContactTables.
// Create a descriptor with field values separated by the 'separator' from a
// CContactItemFieldSet.
// No database read.
//
void CContactDatabase::DoReadContactTextDefL(const CContactItemFieldSet* aFieldSet,TDes& aResult,CContactTextDef* aTextDef)
	{
	TBool firstText=ETrue;
	if (aTextDef)
		{
		TBuf<KMaxContactTextSeperator> nextSeperator;
		for(TInt loop=0;loop<aTextDef->Count();loop++)
			{
			CContactDatabase::TTextFieldMinimal textFieldMin;
			TContactTextDefItem textDefItem=(*aTextDef)[loop];
			const TInt startOfFieldSet=0;
			aFieldSet->FieldText(textDefItem.iFieldType,textFieldMin,startOfFieldSet);
			if (textFieldMin.Length()>0)
				{
				if (!firstText && aResult.MaxLength()>aResult.Length())
					aResult.Append(nextSeperator.Left(Min(aResult.MaxLength()-aResult.Length(),nextSeperator.Length())));
				aResult.Append(textFieldMin.Left(Min(aResult.MaxLength()-aResult.Length(),textFieldMin.Length())));
				firstText=EFalse;
				}
			nextSeperator=textDefItem.iSeperator;
			}
		}
	if (firstText)
		{
		if(aTextDef && aTextDef->ExactMatchOnly()!=EFalse)
			return;//text def specifies an exact match only
		CContactDatabase::TTextFieldMinimal textFieldMin;
		if (aTextDef)
			{
			TFieldType fieldType=aTextDef->iFallbackFieldType;
			if (fieldType!=KUidContactFieldNone)
				{
				const TInt startOfFieldSet=0;
				aFieldSet->FieldText(fieldType,textFieldMin,startOfFieldSet);
				}
			}
		if (textFieldMin.Length()==0)
			{
			TInt findPos=KContactFieldSetSearchAll;
			do
				{
				findPos=aFieldSet->FindNext(KUidContactFieldMatchAll,findPos+1);
				if (findPos<0)
					break;
				(*aFieldSet)[findPos].GetFieldText(textFieldMin);
				} while(textFieldMin.Length()==0);
		}
		aResult.Append(textFieldMin.Left(Min(aResult.MaxLength(),textFieldMin.Length())));
		}
	}


/**
Reads text into a descriptor from a pre-loaded contact item.

This function uses the database's current text definition (as set using 
CContactDatabase::SetTextDefinitionL()).

@capability ReadUserData

@param aItem The contact item to read.
@param aResult On return, contains the text read from the contact item aItem, 
using the database's current text definition. 
*/
EXPORT_C void CContactDatabase::ReadContactTextDefL(const CContactItem& aItem, TDes& aResult)
	{
	DoReadContactTextDefL(&aItem.CardFields(),aResult,iTextDef);
	}


/**
Reads text into a descriptor from a pre-loaded contact item, using the specified
text definition.

@capability ReadUserData

@param aItem The contact item to read.
@param aResult On return, contains the text read from the contact item aItem, 
using the text definition specified in aTextDef.
@param aTextDef The text definition to use. 
*/
EXPORT_C void CContactDatabase::ReadContactTextDefL(const CContactItem& aItem, TDes& aResult,CContactTextDef* aTextDef)
	{
	DoReadContactTextDefL(&aItem.CardFields(),aResult,aTextDef);
	}


/**
Reads text from a contact item stored in the database into a descriptor.

This function uses the database's currently set text definition (as set using 
CContactDatabase::SetTextDefinitionL()).

@capability ReadUserData

@param aContactId The ID of the contact to read.
@param aResult On return, contains the text read from the contact item identified by aContactId, using 
the database's current text definition.

@leave KErrNotFound The specified contact item cannot be found in the database. 
*/
EXPORT_C void CContactDatabase::ReadContactTextDefL(TContactItemId aContactId, TDes& aResult)
	{
	ReadContactTextDefL(aContactId,aResult,iTextDef);
	}


/**
Reads text from a contact item stored in the database into a descriptor 
using the specified text definition.

@capability ReadUserData

@param aContactId The ID of the contact to read.
@param aResult On return, contains the text read from the contact item identified by aContactId, using 
the text definition specified in aTextDef.
@param aTextDef The text definition to use.

@leave KErrNotFound The specified contact item cannot be found in the database. 
*/
EXPORT_C void CContactDatabase::ReadContactTextDefL(TContactItemId aContactId, TDes& aResult,CContactTextDef* aTextDef)
	{
	CContactTextDef* textDef = NULL;
	if(aTextDef == NULL)
		{
		textDef = CContactTextDef::NewLC();
		}
	else
		{
		textDef = aTextDef;
		}
	MLplViewIteratorManager& manager = FactoryL()->GetViewIteratorManagerL();
	manager.ReadContactTextDefL(aContactId,aResult,*textDef);
	if(aTextDef == NULL)
		{
		CleanupStack::PopAndDestroy(textDef);
		}
	}


CContactIdArray* CContactDatabase::SortLC(const CArrayFix<CContactDatabase::TSortPref>* aSortOrder, const CContactIdArray* aIdArray)
	{
	CContactIdArray* sortedItems=CContactIdArray::NewLC();
	if (aSortOrder->Count()==0)
		{
		TContactItemId currentId(1);
		TContactItemId actualId;
		TUid contactType;
		TBool deleted;
		MLplCollection& collection = FactoryL()->GetCollectorL();
		while(collection.SeekContactL(currentId,actualId,contactType,deleted))
			{
			if(CheckType(contactType) && !deleted)
				{
				sortedItems->AddL(actualId);
				}
			currentId = actualId+1;
			}
		}
	else
		{
		CContactTextDef* textDef;
		TUid fieldType=(*aSortOrder)[0].iFieldType;
		if (fieldType==KUidContactFieldDefinedText)
			{
			textDef=iTextDef;
			}
		else
			{
			//We are sorting on aSortOrder.
			textDef=CContactTextDef::NewLC();
			TInt sortOrderCount=aSortOrder->Count();
			for (TInt sortIndex=0;sortIndex<sortOrderCount;sortIndex++)
				{
				textDef->AppendL(TContactTextDefItem((*aSortOrder)[sortIndex].iFieldType));
				}
			}
		CSortArray* sortedList=new(ELeave) CSortArray;
		CleanupStack::PushL(sortedList);
		TContactItemId currentId(1);
		TInt arrayIndex(0);
		for(;;)
			{
			TUid contactType;
			TContactItemId actualId;
			TBool deleted;
			if(aIdArray)
				{
				if (arrayIndex==aIdArray->Count())
					break;
				MLplCollection& collection = FactoryL()->GetCollectorL();
				if(!collection.SeekContactL((*aIdArray)[arrayIndex],actualId,contactType,deleted) || actualId != (*aIdArray)[arrayIndex])
					{
					User::Leave(KErrNotFound);	
					}
				++arrayIndex;
				}
			else
				{
				MLplCollection& collection = FactoryL()->GetCollectorL();
				if(!collection.SeekContactL(currentId,actualId,contactType,deleted))
					{
					break;	
					}
				currentId = actualId+1;
				}
			if(CheckType(contactType) && !deleted)
				{
				CContactDatabase::TTextFieldMinimal textFieldMin;
				ReadContactTextDefL(actualId,textFieldMin,textDef);
				sortedList->AppendL(textFieldMin,actualId);
				}
			}
		sortedList->SortL((*aSortOrder)[0].iOrder);
		SortDuplicatesL(*aSortOrder,*sortedList,1);
		const TInt count=sortedList->Count();
		for (TInt ii=0;ii<count;ii++)
			sortedItems->AddL(sortedList->Id(ii));
		CleanupStack::PopAndDestroy(); // sortedList ,
		if (fieldType!=KUidContactFieldDefinedText)
			{
			CleanupStack::PopAndDestroy();//textdef
			}
		}
	CContactIdArray* newSortedItems=CContactIdArray::NewL(sortedItems);
	CleanupStack::PopAndDestroy(sortedItems);
	CleanupStack::PushL(newSortedItems);
	User::Heap().Compress();
	return(newSortedItems);		
	}


void CContactDatabase::ReSortL(CArrayFix<TSortPref>* aSortOrder)
	{
	CContactIdArray* sortedItems=SortLC(aSortOrder,NULL);
	delete iSortedItems;
	iSortedItems=sortedItems;
	CleanupStack::Pop(sortedItems);
	}


void CContactDatabase::SortDuplicatesL(const CArrayFix<TSortPref>& aSortOrder,
											CSortArray& aList,TInt aSortIndex)
	{ // is this too heavy on stack usage to be called recursively ???
	__ASSERT_DEBUG(&aSortOrder!=NULL,Panic(ECntPanicNullPointer));
	__ASSERT_DEBUG(&aList!=NULL,Panic(ECntPanicNullPointer));
	if (aSortIndex<aSortOrder.Count())
		{
		const TInt count=aList.Count();
		if (count<=1)
			return;
		TInt ii=0;
		HBufC *text1=aList.Text(ii);
		TInt startPos=KErrNotFound;
		TBool checkedList=EFalse;
		while (!checkedList)
			{
			checkedList=(++ii==count-1);
			HBufC *text2=aList.Text(ii);
			const TInt compare=text1->CompareC(*text2,3,&iCollateMethod);
			if (compare==0)
				{
				if (startPos==KErrNotFound)
					startPos=ii-1;
				}
			else if (startPos!=KErrNotFound)
				{
				SortDuplicatesL(aSortOrder,aList,aSortIndex,startPos,ii);
				startPos=KErrNotFound;
				}
			text1=text2;
			}
		if (startPos!=KErrNotFound)
			SortDuplicatesL(aSortOrder,aList,aSortIndex,startPos,ii+1);
		}
	}


void CContactDatabase::SortDuplicatesL(const CArrayFix<TSortPref>& /*aSortOrder*/,CSortArray& /*aList*/,
											TInt /*aIndex*/,TInt /*aStartPos*/,TInt /*aEndPos*/)
	{ 
	}


TBool CContactDatabase::CheckType(TUid aUid) const
	{
	// checks the view on the db whether the uid supplied complies to the rules below
	if (aUid == KUidContactTemplate)  // do not include golden template;
		return EFalse;
	if (iDbViewContactType == KUidContactItem)  // any type of contact;
		return ETrue;
	if (iDbViewContactType == KUidContactCardOrGroup &&
		(aUid == KUidContactCard || aUid == KUidContactGroup)) // cards and/or groups
		return ETrue;
	if (iDbViewContactType == KUidContactCard && aUid == KUidContactOwnCard)
		// if the card being checked is an own card - allow it to be included
		return ETrue;
	else if (aUid == iDbViewContactType)		// only a specific match
		return ETrue;
	return EFalse;
	}


/**
Sets the template id on the contact item to that of the SystemTemplate
after each update or commit operation.
*/
void CContactDatabase::CheckTemplateField(CContactItem& aCnt)
	{
	if (aCnt.iTemplateRefId == KNoValueSet)	
		{
		aCnt.iTemplateRefId = KGoldenTemplateId;	
		}
	if (aCnt.iAccessCount == (TUint32)KNoValueSet)	
		{
		aCnt.iAccessCount = 0;	
		}
	}


/** 
Opens a contact item for editing using a specified view definition. 

The returned contact item is locked and left open until either CommitContactL() or CloseContactL() 
is called.

Note: care should be taken when specifying a view definition because when committing 
the contact item, any fields not loaded by the view definition are deleted 
from the item.

The caller takes ownership of the returned object.

@deprecated
@capability WriteUserData

@param aContactId The ID of the contact item to open.

@leave KErrInUse The contact item is already open.
@leave KErrNotFound The contact item is not present in the database. 

@return The open, locked contact. 
*/
EXPORT_C CContactItem* CContactDatabase::OpenContactLX(TContactItemId aContactId)
	{
	// Since the AllFieldsView method returns a modifiable ptr to the iAllFieldsView
	// we must always send it across the IPC as it may have changed.
	return OpenContactLX(aContactId, *iAllFieldsView);
	}


/**
Opens a contact item for editing.

The returned contact item is locked and left open until either CommitContactL() 
or CloseContactL() is called.

This function uses a view definition that loads every field. If you need to 
specify your own view definition use the other overload of this function.

The caller takes ownership of the returned object.

@capability WriteUserData

@param aContactId The ID of the contact item to open. 

@return The open, locked contact.

@leave KErrInUse The contact item is already open.
@leave KErrNotFound The contact item is not present in the database. 
*/
EXPORT_C CContactItem* CContactDatabase::OpenContactL(TContactItemId aContactId)
	{
	CContactItem* cntItem = OpenContactLX(aContactId);
	CleanupStack::Pop(); // Pop the lock
	return cntItem;
	}


/** 
Opens a contact item for editing, leaving the lock record on the cleanup stack.

The returned item is locked and left open until either CommitContactL() or 
CloseContactL() is called.

This function uses the specified view definition. Note: Care should be taken 
when specifying a view definition because when committing the contact item 
any fields not loaded by the view definition are deleted from the item.

The caller takes ownership of the returned object.

@deprecated
@capability WriteUserData

@param aContactId The ID of the contact item to open.
@param aViewDef The view definition.

@return The open, locked contact item.

@leave KErrInUse The contact item is already open
@leave KErrNotFound The contact item is not present in the database. 
*/
EXPORT_C CContactItem* CContactDatabase::OpenContactL(TContactItemId aContactId, const CContactItemViewDef& aViewDef)
	{
	CContactItem* cntItem = OpenContactLX(aContactId, aViewDef);
	CleanupStack::Pop();
	return cntItem;
	}


/** 
Opens a contact item for editing, leaving the lock record on the cleanup stack.

The returned item is locked and left open until either CommitContactL() or 
CloseContactL() is called.

This function uses the specified view definition. Note: Care should be taken 
when specifying a view definition because when committing the contact item 
any fields not loaded by the view definition are deleted from the item.

The caller takes ownership of the returned object.

@deprecated
@capability WriteUserData

@param aContactId The ID of the contact item to open.
@param aViewDef The view definition.

@return The open, locked contact item.

@leave KErrInUse The contact item is already open
@leave KErrNotFound The contact item is not present in the database. 
*/
EXPORT_C CContactItem* CContactDatabase::OpenContactLX(TContactItemId aContactId, const CContactItemViewDef& aViewDef)
	{
	CContactItem* cntItem = iCntSvr->OpenContactLX(&aViewDef, aContactId);

	CleanupStack::PushL(cntItem);
	iTemplateCache->MergeWithTemplateL(*cntItem, &aViewDef);

	CleanupStack::Pop(cntItem);	
	return cntItem;
	}


CContactItem* CContactDatabase::OpenNoMergeLCX(TContactItemId aContactId)
	{
	CContactItem* cntItem = iCntSvr->OpenContactLX(NULL, aContactId);
	CleanupStack::PushL(cntItem);
	
	return cntItem;
	}


/** 
Gets the ID of the system template. This can then be read, opened and committed 
like any other contact item.

@return ID of the system template. 
*/
EXPORT_C TContactItemId CContactDatabase::TemplateId() const
	{
	return iTemplateCache->TemplateId();
	}


/** 
Creates a default own card based on the system template and adds it to the 
database. This is set as the database's current own card, replacing any existing 
current own card. The caller takes ownership of the returned object.

@capability WriteUserData

@return Pointer to the new default own card. The pointer is left on the cleanup 
stack.
*/
EXPORT_C CContactItem* CContactDatabase::CreateOwnCardLC()
	{
	CContactCard* ownCard = CContactCard::NewLC(&iTemplateCache->SystemTemplateL());
	TInt cntId = AddNewContactL(*ownCard);
	ownCard->iId = cntId;
	// set and persist newowncard id
	iCntSvr->SetOwnCardL(*ownCard);
	CleanupStack::PopAndDestroy(ownCard);
    return ReadContactLC(cntId);	
	}


/** 
Creates a default own card based on the system template and adds it to the 
database. This is set as the database's current own card, replacing any existing 
current own card. The caller takes ownership of the returned object.

@capability WriteUserData

@return Pointer to the new default own card. 
*/
EXPORT_C CContactItem* CContactDatabase::CreateOwnCardL()
	{
	CContactItem* ownCard = CreateOwnCardLC();
	CleanupStack::Pop(ownCard);
	return ownCard;
	}

	
/**
Returns the ID of the database's current own card. 

Having obtained this ID, the client may then open the own card in the same 
way as an ordinary contact card (using ReadContactL() or OpenContactL()).

@capability None

@return The ID of the database's current own card. KNullContactId if the own 
card has been deleted or has not yet been set.
*/
EXPORT_C TContactItemId CContactDatabase::OwnCardId() const
	{
	return iCntSvr->OwnCard();
	}


/**
Returns the ID of the database's preferred template, as set by SetPrefTemplateL(). 
KNullContactId if not set. The preferred template is for clients who may have 
multiple templates but want to identify one as preferred.

@capability None

@return The ID of the database's current preferred template.
*/
EXPORT_C TContactItemId CContactDatabase::PrefTemplateId() const
	{
	return(iCntSvr->PrefTemplateId());
	}


/** 
Sets an existing contact item to be the database's current own card.

@capability WriteUserData

@param aContact The contact item to set as the database's current own card. 
It must already exist in the database. It cannot be a group or a template.

@leave KErrNotFound aContact does not exist in the database. 
@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::SetOwnCardL(const CContactItem& aContact)
	{
	iCntSvr->SetOwnCardL(aContact);	
	}

	
/**
Sets the time out of operations on the database server.

This API allows the behaviour of the Contacts Model to be tuned for scenarios
where clients either expect to encounter or know they will encounter operations
failing primarily due to the database being locked.

Not all clients will find they need to use this API.  By default the operation
timeout is 1 second.

The timeout only needs to be set once per session.  Multiple clients using the
same database can specify independent values for the operation timeout.

@capability None

@publishedPartner

@released

@param aMicroSeconds The operation timeout in microseconds.  This timeout will
only be applied to requests sent by clients of this database after this point in
time.

@leave KErrArgument If aMicroSeconds is less than 0 or is greater than 300000000
(equivalent to 5 minutes). 
*/	
EXPORT_C void CContactDatabase::SetOperationTimeOutL(const TInt aMicroSeconds) const
	{
	if (aMicroSeconds < 0 || aMicroSeconds > KFiveMins)
		{
		User::Leave(KErrArgument);
		}
	iCntSvr->SetOperationTimeOutL(aMicroSeconds);
	}


/** 
Sets the database's preferred template.

The preferred template's ID persists when the database is opened and closed. 
If the preferred template is subsequently deleted, the preferred template 
ID is set to KNullContactId.

@capability WriteUserData

@param aContact The contact card template to set as the database's preferred 
template.

@leave KErrNotSupported The item is not a template (of type KUidContactCardTemplate).
@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::SetPrefTemplateL(const CContactItem& aContact)
	{
	TUid aContactType = aContact.Type();
	__ASSERT_ALWAYS(aContactType==KUidContactCardTemplate,User::Leave(KErrNotSupported));
	if (aContact.Id()==PrefTemplateId())
		{
		// leave quietly if already set as the preferred template
		return;
		}
	iCntSvr->SetPrefTemplateL(aContact.Id());
	}


/** 
Sets the ID of the current item and persists it in the database. The current 
item is provided for use by clients who want to identify one contact item 
in the database as the currently selected item.

@capability WriteUserData

@param aContactId The ID of the current item. 
*/
EXPORT_C void CContactDatabase::SetCurrentItem(const TContactItemId aContactId)
	{
	iCntSvr->SetCurrentItem(aContactId);	
	}


/** 
Gets the ID of the current item, as set by SetCurrentItem(). The current item 
ID is initialised to KNullContactId when the database is opened.

@capability None

@return The ID of the current item. 
*/	
EXPORT_C TContactItemId CContactDatabase::GetCurrentItem() const
	{	
	return iCntSvr->CurrentItem();
	}


/**
Updates a contact identified by aContactId with the data in aNewContact.
All empty fields are deleted.

@deprecated
@publishedPartner

@capability WriteUserData
@capability ReadUserData 

@param aContactId This argument should be ignored by developers.
@param aNewContact The contact item to replace it with.

@leave KErrDiskFull The disk does not have enough free space to perform the operation.

@return The contact item after the update.
*/
EXPORT_C CContactItem* CContactDatabase::UpdateContactLC(TContactItemId aContactId,CContactItem* aNewContact)
	{
	CContactItemViewDef* viewDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	viewDef->AddL(KUidContactFieldMatchAll);

	CContactItem* cntItem = OpenContactLX(aContactId, *viewDef);
	CleanupStack::PushL(cntItem);

	CContactItemFieldSet& newfieldSet = aNewContact->CardFields();

	TInt count=newfieldSet.Count();
	for (TInt ii=0; ii<count; ++ii)
		{
		//In the case there are duplicate fields in the field Set
		//matchCount need to be increased, so the previous field won't be 
		//overwritten by the later one. 
		TInt matchCount =1;
		for (TInt jj=ii-1; jj>=0; jj--)
			{
			if(newfieldSet[ii].ContentType()==newfieldSet[jj].ContentType())
			matchCount++;
			}
			
		cntItem->CardFields().UpdateFieldL(newfieldSet[ii],matchCount);
		}
		
	doCommitContactL(*cntItem,ETrue,ETrue);
	if (!count)
		{
		doDeleteContactL(aContactId,ETrue,ETrue);
		delete cntItem;
		cntItem = NULL;
		}

	if(cntItem != NULL)
		{
		CleanupStack::Pop(cntItem);
		CleanupStack::Pop(); // Pop the lock
		CleanupStack::PopAndDestroy(viewDef);	
		CheckTemplateField(*cntItem);
	
		CleanupStack::PushL(cntItem);	
		}
	return(cntItem);
	}


/**
Closes the contact item, allowing other applications to access it. Specifying 
a contact item that is not open, or cannot be found, is harmless. This function 
does not commit any changes made to the item. Despite the trailing L in the 
function's name, this function cannot leave.

@capability None

@param aContactId The ID of the contact to close.
*/
EXPORT_C void CContactDatabase::CloseContactL(TContactItemId aContactId)
	{
	iCntSvr->CloseContact(aContactId);
	}


/**
Overwrites a contact item with the values contained in aContact. The contact 
item is also closed by this call.

@capability ReadUserData
@capability WriteUserData 

@param aContact Contains the new values for the contact item.

@leave KErrAccessDenied The contact item is not locked by the caller.
@leave KErrNotFound The contact item's ID is not present in the database. 
@leave KErrDiskFull The disk does not have enough free space to perform the operation.
@leave KErrNotSupported The contact item cannot be committed because it contains
invalid data.
*/
EXPORT_C void CContactDatabase::CommitContactL(const CContactItem& aContact)
	{
	// If the contact item being committed is the System template then check if
	// the fields are valid.
	if (aContact.Id()==TemplateId())
		{
		if (!SystemTemplateFieldsValid(aContact))
			{
			User::Leave(KErrNotSupported);
			}
		}

	iCntSvr->CommitContactL(aContact);
	RespondToEventL(EContactDbObserverEventTemplateChanged, aContact.Id());
	}

	
/** Updates the existing contact information.
@publishedPartner
@released

@capability ReadUserData
@capability WriteUserData 

@param aContact Contains the new values for the contact item.
@param aIsInTransaction This argument should be ignored by developers.
@param aSendChangedEvent This argument should be ignored by developers.

@leave KErrAccessDenied The contact item is not locked by the caller.
@leave KErrNotFound The contact item's ID is not present in the database. 
@leave KErrNotSupported The contact item cannot be committed because it contains
invalid data.
*/	
EXPORT_C void CContactDatabase::doCommitContactL(const CContactItem& aContact,TBool /*aIsInTransaction*/, TBool aSendChangedEvent)
	{
	// If the contact item being committed is the System template then check if
	// the fields are valid.
	if (aContact.Id()==TemplateId())
		{
		if (!SystemTemplateFieldsValid(aContact))
			{
			User::Leave(KErrNotSupported);
			}
		}

	iCntSvr->CommitContactL(aContact, aSendChangedEvent);
	MoveInSortArray(aContact);
	}


/**
Deletes an array of contacts. 

The function commits the changes for every 32 (for 9.5 onwards it commits after every 50)
contacts deleted, and compresses the database as required. A changed message 
is not sent for every contact deleted, instead a single unknown change event 
message (EContactDbObserverEventUnknownChanges) is sent after all the contacts 
have been deleted and the changes committed.

@capability WriteUserData 
@capability ReadUserData 

@param aContactIds An array of contacts to delete.

@leave KErrNotFound A contact item ID contained in the array is not present 
in the database.
@leave KErrInUse One or more of the contact items is open. 
@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::DeleteContactsL(const CContactIdArray& aContactIds)
	{
    const TInt KDeleteTransactionGranularity = 50;
	TInt count = aContactIds.Count();
	if (count == 0)
		{
		return;
		}

	// Clone the ContactIdArray so it can be sorted into ascending order
	CContactIdArray* sortedIdArray = CContactIdArray::NewLC(&aContactIds);
	sortedIdArray->Sort();

    if (count > 1)
	    {	    
	    for(TInt ii = 0; ii < count-1; ++ii)
		    {
		    if ((ii % KDeleteTransactionGranularity) == 0)
			    {
			    if (ii > 0)
				    {
				    DatabaseCommitLP(EFalse);
				    }
			    DatabaseBeginLC(EFalse);			
			    }
		    // Delete the contact but do not trigger any event		
		    DeleteContactSendEventActionL((*sortedIdArray)[ii], EDeferEvent);
		    }
	    }
	else	 
		{ // edge case
		DatabaseBeginLC(EFalse);	
		}		
		
	// Delete the last item and trigger EContactDbObserverEventUnknownChanges 
	DeleteContactSendEventActionL((*sortedIdArray)[count-1], ESendUnknownChangesEvent);	
			
	DatabaseCommitLP(EFalse);		

	CleanupStack::PopAndDestroy(sortedIdArray);	
	}


/**
Deletes a contact item.

Note: if the contact's access count is greater than zero, the contact is not fully 
deleted from the database. A 'skeleton' of the contact is left, containing only 
basic information, and no field data. The skeleton contact can still be accessed 
if a record of its contact ID has been retained (or call DeletedContactsLC()). 
The skeleton is removed when the access count is zero.

@capability ReadUserData
@capability WriteUserData

@param aContactId The contact item ID of the contact to delete.

@leave KErrNotFound aContactId is not present in the database.
@leave KErrInUse The contact item is open.
@leave KErrDiskFull The disk does not have enough free space to perform the operation.
*/
EXPORT_C void CContactDatabase::DeleteContactL(TContactItemId aContactId)
	{
	iCntSvr->DeleteContactL(aContactId, ESendEvent);	
	//Now we check if the contact belonged to the sort array, if so 
	//remove it from iSortedItems
    RemoveFromSortArray(aContactId);
	//Now we check if the contact belonged to the template list, if so 
	//remove it from iCardTemplateIds
	RemoveFromTemplateList(aContactId);
	//Now we check if the contact belonged to the Group Id list, if so 
	//remove it from iGroupIds	
	RemoveFromGroupIds(aContactId);
	}


/**
Deletes a contact item. 

Note: if the contact's access count is greater than zero, the contact is not fully 
deleted from the database. A 'skeleton' of the contact is left, containing only 
basic information, and no field data. The skeleton contact can still be accessed 
if a record of its contact ID has been retained (or call DeletedContactsLC()). 
The skeleton is removed when the access count is zero.

@publishedPartner
@released

@capability ReadUserData
@capability WriteUserData

@param aContactId The contact item ID of the contact to delete.
@param aIsInTransaction This argument should be ignored by developers.
@param aSendChangedEvent This argument should be ignored by developers.
@param aDecAccessCount This argument should be ignored by developers.

@leave KErrNotSupported An attempt has been made to delete the system template.
@leave KErrNotFound aContactId is not present in the database.
@leave KErrInUse The contact item is open.
*/
EXPORT_C void CContactDatabase::doDeleteContactL(TContactItemId aContactId, TBool /*aIsInTransaction*/, TBool aSendChangedEvent, TBool aDecAccessCount)
	{
	TCntSendEventAction action = aSendChangedEvent ? ESendEvent : EDeferEvent;
	iCntSvr->DeleteContactL(aContactId, action, aDecAccessCount);	
	//Now we check if the contact belonged to the sort array, if so 
	//remove it from iSortedItems
    RemoveFromSortArray(aContactId);
	//Now we check if the contact belonged to the template list, if so 
	//remove it from iCardTemplateIds
	RemoveFromTemplateList(aContactId);
	//Now we check if the contact belonged to the Group Id list, if so 
	//remove it from iGroupIds	
	RemoveFromGroupIds(aContactId);
	}

	
/**
Returns the full view definition, that loads every field,
the returned pointer is owned by the CContactDatabase object.

@publishedPartner
@released

@capability None

@return The definition for a full view with all the contact item fields.
*/	
EXPORT_C CContactItemViewDef* CContactDatabase::AllFieldsView()
	{
	return iAllFieldsView;
	}


/**
Imports one or more vCards from a read stream. The vCards are converted into 
contact items, and added to the database. If at least one contact item was 
successfully imported, aImportSuccessful is set to ETrue. If EImportSingleContact 
is specified in aOption, the read stream marker is left at the next position, 
ready to read the next contact item. The caller takes ownership of the returned 
object.

@capability WriteUserData

@param aFormat Indicates the format for imported and exported contacts. Its 
value must be KUidVCardConvDefaultImpl.
@param aReadStream The stream to read from.
@param aImportSuccessful On return, ETrue if at least one contact was successfully 
imported. EFalse if not.
@param aOption Indicates the options for import and export. See the TOptions 
enum.

@leave KErrNotSupported aFormat.iUid is not KUidVCardConvDefaultImpl.

@return The array of contact items imported.
*/
EXPORT_C CArrayPtr<CContactItem>* CContactDatabase::ImportContactsL(const TUid& aFormat,RReadStream& aReadStream,TBool& aImportSuccessful,TInt aOption)
	{
	const TBool importSingleContact= aOption & EImportSingleContact;
	return ConverterL(aFormat).ImportL(*this,aReadStream,aImportSuccessful,aOption,importSingleContact);
	}


/**
Converts an array of contact items to vCards and exports them to a write stream.

The default character set CVersitParser::EUTF8CharSet is used to convert into. If 
you need a different character set, use the other overload of this function.

@capability ReadUserData

@param aFormat Indicates the format for imported and exported contacts. Must 
have a value of KUidVCardConvDefaultImpl.
@param aSelectedContactIds Array of contact IDs to export.
@param aWriteStream The stream to write to.
@param aOption Indicates the options for import and export. See the TOptions 
enum.
@param aExportPrivateFields ETrue exports fields marked as private. EFalse 
does not export fields marked as private. See CContactItemField::SetPrivate().

@leave KErrNotSupported aFormat.iUid is not KUidVCardConvDefaultImpl.
@leave KErrNotFound One or more of the contact items does not exist in the 
database.
*/
EXPORT_C void CContactDatabase::ExportSelectedContactsL(const TUid& aFormat,const CContactIdArray& aSelectedContactIds, RWriteStream& aWriteStream, TInt aOption, TBool aExportPrivateFields)
	{
	ExportSelectedContactsL(aFormat,aSelectedContactIds,aWriteStream,aOption,Versit::EUTF8CharSet,aExportPrivateFields);
	}


/**
Converts an array of contact items to vCards and exports them to a write 
stream using the specified character set.

@capability ReadUserData

@param aFormat Indicates the format for imported and exported contacts. Must 
have a value of KUidVCardConvDefaultImpl.
@param aSelectedContactIds Array of contact IDs to export.
@param aWriteStream The stream to write to.
@param aOption Indicates the options for import and export. See the TOptions 
enum.
@param aCharSet The character set to convert into.
@param aExportPrivateFields ETrue exports fields marked as private. EFalse 
does not export fields marked as private. See CContactItemField::SetPrivate().

@leave KErrNotSupported aFormat.iUid is not KUidVCardConvDefaultImpl.
@leave KErrNotFound One or more of the contact items does not exist in the 
database.
*/
EXPORT_C void CContactDatabase::ExportSelectedContactsL(const TUid& aFormat,const CContactIdArray& aSelectedContactIds, RWriteStream& aWriteStream,TInt aOption,const Versit::TVersitCharSet aCharSet, TBool aExportPrivateFields)
	{
	ConverterL(aFormat).ExportL(*this,aSelectedContactIds,aWriteStream,aOption,aCharSet,aExportPrivateFields);
	}


/**
Converts an array of contact items to PBAP compliant vCards following vCard2.1 and vCard3.0 specifications and exports them to a write 
stream using UTF-8 as the character set. It also provides support for exporting contacts as standard vCard2.1.

@capability ReadUserData
@internalTechnology
@released 

@param aFormat Indicates the format for imported and exported contacts. It should have a value of KUidPBAPVCardConvImpl if user
wants to export contacts as PBAP specific vCards and KUidVCardConvDefaultImpl for standard vCard2.1.
@param aSelectedContactIds Array of contact IDs to export.
@param aWriteStream The stream to write to.
@param aOption Indicates the options for import and export. See the TOptions 
enum.
@param aContactFieldFilter 64-bit filter,specifies the contact fields to export, argument value not considered for standard vCard2.1 export.
@param aCallback Calls client which has to implement class MConverterCallBack, used to add intra-contact properties,
argument value not considered for standard vCard2.1 export.
@param aVersion TVCardVersion specifies vCard version to which contacts should be exported.
@param aExportTel If TEL property should be exported, it should be set to ETrue, argument value not considered for standard vCard2.1 export.
@param aCharSet The character set to convert into.Must be UTF-8 for PBAP export, provided as default value.
@param aExportPrivateFields ETrue exports fields marked as private. EFalse does not export fields marked as private. See CContactItemField::SetPrivate().

@leave KErrNotSupported aFormat.iUid is not KUidPBAPVCardConvImpl for PBAP export.
@leave KErrNotSupported aFormat.iUid is not KUidVCardConvDefaultImpl for standard vCard2.1 export.
@leave KErrNotSupported aCharSet is other than UTF-8 for PBAP export. 
@leave KErrNotFound One or more of the contact items does not exist in the database.
*/
EXPORT_C void CContactDatabase::ExportSelectedContactsL(const TUid& aFormat, const CContactIdArray& aSelectedContactIds, RWriteStream& aWriteStream, TInt aOption, const TInt64 aContactFieldFilter, MConverterCallBack* aCallback, const TVCardVersion aVersion, const TBool aExportTel, Versit::TVersitCharSet aCharSet, TBool aExportPrivateFields)
	{
	if(aVersion == EVCard21)
		{
		//client wants to export contacts as standard vCard2.1.
		ConverterL(aFormat).ExportL(*this, aSelectedContactIds, aWriteStream, aOption, aCharSet, aExportPrivateFields);		
		}
	else
		{
		if(aCharSet != Versit::EUTF8CharSet)
			{
			User::Leave(KErrNotSupported);	
			}
		ConverterL(aFormat, aContactFieldFilter, aCallback, aVersion, aExportTel).ExportL(*this, aSelectedContactIds, aWriteStream, aOption, aCharSet, aExportPrivateFields);		
		}
	}

CContactConverter& CContactDatabase::ConverterL(const TUid& aFormat, const TInt64 aContactFieldFilter, MConverterCallBack* aCallback, const TVCardVersion aVersion, const TBool aExportTel)
	{
	if(aFormat.iUid != KUidPBAPVCardConvImpl)
		{
		User::Leave(KErrNotSupported);
		}
	//since version, filter and boolean TEL are initialised only when PBAP plug-in is created,
	//if there is any change in these arguments provided by Client, The plugin should be loaded once again.
	if(iConv && (iConv->GetCurrentVersion() != aVersion || iConv->GetPBAPFilter() != aContactFieldFilter || iConv->GetExportTel() != aExportTel) )
		{
		delete iConv;
		iConv = NULL;
		}
	if(!iConv)
		{
		iConv = CPrivConverter::NewL(aFormat, aContactFieldFilter, aCallback, aVersion, aExportTel);	
		}
	return *iConv->Converter();
	}


CContactConverter& CContactDatabase::ConverterL(const TUid& aFormat)
	{
	if(iConv && iConv->GetCurrentVersion() != EVCard21)
		{
		delete iConv;
		iConv = NULL;				
		}

	switch (aFormat.iUid)
		{
		case KVersitEntityUidVCard:
		case KUidVCardConvDefaultImpl:
		if (!iConv)
			{
			if(aFormat.iUid == KVersitEntityUidVCard)
				{
				iConv=CPrivConverter::NewL(	TUid::Uid(KUidVCardConvDefaultImpl));
				}
			else
				{
				iConv=CPrivConverter::NewL(aFormat);
				}
			}
		break;
		default:
		User::Leave(KErrNotSupported);
		};

	return *iConv->Converter();
	}


/**
Searches the contact tables for the contact described by aGuid.

@publishedPartner
@released
@capability ReadUserData

@param aGuid Describes the contact item to be found.

@leave KErrNotReady The database is not yet ready to read from, could be because an asynchronous open is in progress 
or because a recover is required after a rollback.
@leave KErrBadHandle An asynchronous open either failed or was cancelled.
@leave KErrLocked The database has been closed for a restore.

@return The unique id of the contact item within the database.
*/
EXPORT_C TContactItemId CContactDatabase::ContactIdByGuidL(const TDesC& aGuid)
	{
	TTime time(0);
	MLplCollection& collection = FactoryL()->GetCollectorL();
	CContactIdArray* idArray = collection.CollectionL(MLplCollection::EFindGuid,time,aGuid);
	// Assume a contact with aGuid will not be found.
	TContactItemId id = KNullContactId;
	// Check there is an entry in idArray before attempting to access it.
	if(idArray->Count() > 0)
		{
		// Since aGuid is unique only one contact can match it therefore always
		// expect one and only one entry in the array.
		id = (*idArray)[0];
		}
	delete idArray;
	return id;
	}


/** 
Returns a pointer to the array of contact items which have been sorted by either 
SortByTypeL() or SortL(). This pointer is valid until a change is made to 
the database or until the database's active object is allowed to run. If 
the array is required after one of the above two events has occurred, a copy 
of the array must first be made.

@deprecated
@capability None

@return A pointer to the array of sorted items. The caller does not take ownership 
of this object.
*/
EXPORT_C const CContactIdArray* CContactDatabase::SortedItemsL()
	{
	if(!iSortedItems)
		{
		CArrayFix<TSortPref>* sortPref = iCntSvr->GetSortPreferenceL();
		CleanupStack::PushL(sortPref);
		iSortedItems = SortLC(sortPref,NULL);
		CleanupStack::Pop(iSortedItems);
		CleanupStack::PopAndDestroy(sortPref);
		}
	return iSortedItems;
	}


/**
Gets the database's UID. This value is used to identify a particular contact 
database. The database UID is generated when the database is first created.

Note: This method can leave.

@capability None

@return Descriptor containing the database UID.
*/
EXPORT_C TPtrC CContactDatabase::FileUid()
	{
	return iCntSvr->FileUidL(); // this can leave
	}


/**
Sorts an array of contact IDs. The sort uses the same logic as SortL(). The 
caller takes ownership of the returned object.

@deprecated
@capability None

@param aIdArray Pointer to array of contact IDs to sort.
@param aSortOrder Sort order array.

@return Pointer to sorted array of contact IDs.
*/
EXPORT_C CContactIdArray* CContactDatabase::SortArrayL(const CContactIdArray* aIdArray, const CArrayFix<TSortPref>* aSortOrder)
	{
	CContactIdArray* sortedItems = SortArrayLC(aIdArray, aSortOrder);
	CleanupStack::Pop(sortedItems);
	return (sortedItems);
	}


/**
Sorts an array of contact IDs. The sort uses the same logic as SortL(). The 
returned array is left on the cleanup stack. The caller takes ownership of 
the returned object.

@deprecated
@capability None

@param aIdArray Pointer to array of contact IDs to sort.
@param aSortOrder Sort order array.

@return Pointer to sorted array of contact IDs.
*/
EXPORT_C CContactIdArray* CContactDatabase::SortArrayLC(const CContactIdArray* aIdArray, const CArrayFix<TSortPref>* aSortOrder)
	{
	return SortLC(aSortOrder,aIdArray);
	}


/**
Sorts the database using the view type value set by SetDbViewContactType(). 
The database takes ownership of the sort order array passed in.

The sort uses the same logic as SortL(). The two functions have the same 
effect.

After calling this function, use CContactDatabase::SortedItemsL() to retrieve 
the sorted array of contact IDs.

@deprecated
@capability None

@param aSortOrder Sort order array. 
*/
EXPORT_C void CContactDatabase::SortByTypeL(CArrayFix<TSortPref>* aSortOrder)
	{
	CContactIdArray* sortedItems=SortLC(aSortOrder,NULL);
	CleanupStack::Pop(sortedItems);
	delete iSortedItems;
	iSortedItems=sortedItems;
	iCntSvr->SetSortPreferenceL(*aSortOrder);
	delete aSortOrder;
	}


/**
Sorts the database. The sort only includes items of the type set by SetDbViewContactType(). 
The database takes ownership of the sort order array passed in. Contacts are 
sorted using the first TSortPref in the array. Any identical matches are then 
sorted using the next TSortPref and so on. When there are no more TSortPrefs 
to use, any remaining unsorted contacts are left in the original database order.

Note: after calling this function, use CContactDatabase::SortedItemsL() to retrieve 
the sorted array of contact IDs.

@deprecated
@capability None

@param aSortOrder Sort order array. If the array's count is zero, no sorting 
takes place. 
*/
EXPORT_C void CContactDatabase::SortL(CArrayFix<TSortPref>* aSortOrder)
	{
	TBool reverse=ETrue;
	CArrayFix<TSortPref>* sortPref = iCntSvr->GetSortPreferenceL();
	CleanupStack::PushL(sortPref);
	if (aSortOrder->Count()!=sortPref->Count())
		reverse=EFalse;
	else for(TInt loop=0;loop<aSortOrder->Count();loop++)
		{
		TSortPref oldSort=(*sortPref)[loop];
		TSortPref newSort=(*aSortOrder)[loop];
		if (oldSort.iFieldType!=newSort.iFieldType || oldSort.iOrder==newSort.iOrder)
			{
			reverse=EFalse;
			break;
			}
		}
	if (reverse)
		{
		if (iSortedItems)
			iSortedItems->ReverseOrder();
		}
	else
		{
		ReSortL(aSortOrder);
		}

	iCntSvr->SetSortPreferenceL(*aSortOrder);
	CleanupStack::PopAndDestroy(sortPref);

	// Having taken ownership, we deleted aSortOrder here after storing the sort
	// preferences in iCntSrv. Earlier versions of cntmodel stored it in
	// iSortOrder. Some 3rd party apps wrongly use aSortOrder after calling us.
	// As it was held until deletion in the destructor, they used to get away
	// with it. Since the new way broke these apps, we've reverted to the old
	// delayed deletion to maintain compatibility, reinstating iSortOrder.	
	delete iSortOrder;
	iSortOrder = aSortOrder;
	}

	
/**
Gets the number of CContactItems in the database. The count includes non-system 
template items. It does not include items marked as deleted.

@deprecated
@capability None

@return The number of contact items in the database.
*/
EXPORT_C TInt CContactDatabase::CountL()
	{
	MLplCollection& collection = FactoryL()->GetCollectorL();
	return collection.ContactCountL();
	}

	
/**
Sets the type of contact items to be included in sorted views of the database. 

See also SortL() and SortByTypeL().

This value is initialised to KUidContactItem when the database is opened. 
This means that all CContactItem-derived types (cards, non-system templates, 
groups, own cards) are included in database views.

@deprecated
@capability None

@param aUid Specifies a contact type. One of the following: KUidContactCard 
(contact cards), KUidContactGroup (contact item groups), KUidContactOwnCard 
(own cards), KUidContactCardTemplate (templates which are not system templates, 
in other words, which have been added to the database), KUidContactItem (all of the above)
*/
EXPORT_C void CContactDatabase::SetDbViewContactType(const TUid aUid)
	{
	iDbViewContactType = aUid;
	iCntSvr->SetDbViewContactType(aUid);
	}


/**
Gets the type of contact items which are included in sorted views of the database, 
as set by SetDbViewContactType(). 

@deprecated

@return Specifies a contact type. One of the following: KUidContactCard 
(contact cards), KUidContactGroup (contact item groups), KUidContactOwnCard 
(own cards), KUidContactCardTemplate (templates which are not system, in other words, 
which have been added to the database), KUidContactItem (all of the above)
*/
EXPORT_C TUid CContactDatabase::GetDbViewContactType() const
	{
	return iDbViewContactType;
	}


/** 
This function does nothing and has been deprecated.

@capability WriteUserData
@deprecated
*/
EXPORT_C void CContactDatabase::CompactL()
	{
	}


/**
This function is deprecated. It returns an object whose functions do nothing.

@capability WriteUserData
@return Pointer to an active compressor.
@deprecated
*/
EXPORT_C CContactActiveCompress* CContactDatabase::CreateCompressorLC()
	{
	CContactActiveCompress* compressor = new (ELeave) CContactActiveCompress;
	CleanupStack::PushL(compressor);
	return compressor;
	}


/**
This function is deprecated. It returns an object whose functions do nothing.

@capability WriteUserData
@return Pointer to an active recoverer.
@deprecated
*/
EXPORT_C CContactActiveRecover* CContactDatabase::CreateRecoverLC()
	{
	CContactActiveRecover* recover = new(ELeave) CContactActiveRecover;
	CleanupStack::PushL(recover);		
	return recover;
	}


/**
Recovers the database from a rollback. It first closes all tables and then reopens 
them after the recover. 

@capability WriteUserData
*/
EXPORT_C void CContactDatabase::RecoverL()
	{
	}


/**
This function is deprecated. It always returns EFalse.

@capability None
@return EFalse
@deprecated
*/
EXPORT_C TBool CContactDatabase::CompressRequired()
	{
    return EFalse;
	}


/** 
This function is deprecated. It always returns EFalse.

@capability None
@return EFalse
@deprecated
*/
EXPORT_C TBool CContactDatabase::IsDamaged() const
	{
	return EFalse;
	}


/** 
Debug Only

@internalTechnology
@released 
@capability WriteUserData
*/
EXPORT_C void CContactDatabase::DamageDatabaseL(TInt /*aSecretCode*/)
	{
	}


/** 
Gets the size of the database file in bytes.

@capability None

@return The size of the contact database. 
*/
EXPORT_C TInt CContactDatabase::FileSize() const
	{
	return iCntSvr->FileSize();
	}


/** 
This function is deprecated and always returns 0.

@capability None
@return The wasted space in the contacts database.
@deprecated
*/
EXPORT_C TInt CContactDatabase::WastedSpaceInBytes() const
	{	
    return 0;  
	}


/**
Filters the database. On return, aFilter contains an array of filtered contact 
item IDs.

@capability ReadUserData

@param aFilter The filter to use. On return, contains a filtered array of 
contact item IDs.
*/
EXPORT_C void CContactDatabase::FilterDatabaseL(CCntFilter& aFilter)
	{
	iCntSvr->FilterDatabaseL(aFilter);
	}


/**
Gets an array of contacts modified since the specified date/time.  The array
includes those contacts that have changed since the beginning of the specified
micro-second.  The caller takes ownership of the returned object. 

@capability ReadUserData

@param aTime The date/time of interest.

@return Pointer to the array of contacts modified since the specified time.
*/
EXPORT_C CContactIdArray* CContactDatabase::ContactsChangedSinceL(const TTime& aTime)
	{
	MLplCollection& collection = FactoryL()->GetCollectorL();
	return collection.CollectionL(MLplCollection::EChangedSince,aTime);
	}


/**
Sets the date/time the database was last synchronised. 

This overload sets the last synchronised date/time where the sync ID is not
known, and returns the sync ID that was created (a sync ID identifies a machine
with which the database has been synchronised).

@deprecated

@param aSyncDate The database's new last synchronised date/time.

@return The sync ID created by the function.
*/
EXPORT_C TContactSyncId CContactDatabase::SetLastSyncDateL(const TTime& aSyncDate)
	{
	//
	// Quick workaround for these APIs rather than implement IPC call to server
	// and a Sync table class.  If we can establish that these APIs are never
	// used (they have been deprecated for a long time) then this workaround can
	// be made permanent.
	//
	iSyncDate = aSyncDate;
	return 0;
	}


/**
Sets the date/time the database was last synchronised. 

This overload is for a known sync ID and updates the database's last 
synchronised date/time for that ID.

@deprecated

@param aSyncId This argument should be ignored by developers.
@param aSyncDate The database's new last synchronised date/time.

@leave KErrNotFound The specified sync ID is not found.
*/
EXPORT_C void CContactDatabase::SetLastSyncDateL(TContactSyncId /*aSyncId*/, const TTime& aSyncDate)
	{
	//
	// Quick workaround for these APIs rather than implement IPC call to server
	// and a Sync table class.  If we can establish that these APIs are never
	// used (they have been deprecated for a long time) then this workaround can
	// be made permanent.
	//
	iSyncDate = aSyncDate;
	}


/**
Gets the date/time the database was last synchronised with a particular 
sync ID, as set by SetLastSyncDateL().

@deprecated

@param aSyncId This argument should be ignored by developers.
@param aSyncDate On return contains the date/time the database was last
synchronised with the sync ID specified.

@leave KErrNotFound The ID cannot be found in the database.
*/
EXPORT_C void CContactDatabase::GetLastSyncDateL(TContactSyncId /*aSyncId*/, TTime& aSyncDate)
	{
	//
	// Quick workaround for these APIs rather than implement IPC call to server
	// and a Sync table class.  If we can establish that these APIs are never
	// used (they have been deprecated for a long time) then this workaround can
	// be made permanent.
	//
	aSyncDate = iSyncDate;
	}


/**
Gets the ID of the connection to the Contacts server. 

This can be compared with the connection IDs of incoming messages to identify 
which connection generated the message.

@capability None

@return The ID of the connection to the Contacts server.
*/
EXPORT_C TUint CContactDatabase::ConnectionId() const
	{
	return iCntSvr->ConnectionId();
	}


/** 
Debug only.

@internalTechnology
@released 

@capability None

@return return the heap size usage of the server in debug mode, 0 in release mode.
*/
EXPORT_C TInt CContactDatabase::CntServerResourceCount()
	{
	return(iCntSvr->ResourceCount());
	}
	

/** 
Debug only.

@internalTechnology
@released 

@capability None
*/	
EXPORT_C void CContactDatabase::SetCntServerHeapFailure(TInt aTAllocFailType, TInt aRate)
	{
	iCntSvr->SetHeapFailure((RHeap::TAllocFail)aTAllocFailType,aRate);
	}


/** 
Debug only.

@internalTechnology
@released 

@capability None

@param aMachineUniqueId The Machine ID to set.
*/
EXPORT_C void CContactDatabase::OverrideMachineUniqueId(TInt64 aMachineUniqueId)
	{
	iCntSvr->OverrideMachineUniqueId(aMachineUniqueId);	
	}


/** 
Gets the contact model's version number.

@return The version number of the contacts model. 
*/
EXPORT_C TVersion CContactDatabase::Version() const
	{
	return(TVersion(KMajorVersion,KMinorVersion,KBuildNumber));
	}


/**
Returns a number unique to the contacts database. This value may be passed 
to CContactItem::UidStringL().

@capability None

@return The database's unique ID.
*/
EXPORT_C TInt64 CContactDatabase::MachineId() const
	{
	return iCntSvr->MachineId();
	}


/**
Gets an array of IDs for contact items that still exist in the database, but 
are marked as deleted. These are contact items which have been deleted, but 
which have a non-zero access count. The caller takes ownership of the returned 
object.

Debug only.

@internalTechnology
@released
@capability ReadUserData

@return Pointer to the array of contacts marked as deleted. 
*/
EXPORT_C CContactIdArray* CContactDatabase::DeletedContactsLC()
	{
#if defined(_DEBUG)
	MLplCollection& collection = FactoryL()->GetCollectorL();
	CContactIdArray* deletedContactsIdArray =
		collection.CollectionL(MLplCollection::EDeleted);
	CleanupStack::PushL(deletedContactsIdArray);
	return deletedContactsIdArray;
#else
	// Do nothing (except avoid compiler errors)
	return NULL;
#endif
	}


/** 
Requests that the server reset all its speed dials to a NULL state.
Needed so that T_NOMACH works (since deleting the db no longer resets the speed dials)

Debug only.

@internalTechnology
@released 
@capability WriteUserData
*/
EXPORT_C void CContactDatabase::ResetServerSpeedDialsL()
	{	
#if defined(_DEBUG)
	for(TInt i = KCntMinSpeedDialIndex; i <= KCntMaxSpeedDialIndex; i++)
		{
		// Pass -1 as field index to indicate that speed dial position is to
		// be reset.
		iCntSvr->SetFieldAsSpeedDialL(KNullContactId, -1, i);
		}
#endif
	}
	
	
/** 
Where there are multiple contact databases on a device, this function can be 
used to enquire which database is the current one. The current database functions 
are provided as part of current item functionality. In order to pass a current 
item from one contacts model client to another, the receiving client needs 
to be using the same database.

The current database is a path and filename, set using SetCurrentDatabase() 
which is persisted by the contacts server.

@deprecated
@capability None

@param aDatabase The path and filename of the current database. KNullDesC 
if no current database has been set.

@return KErrNone if the function completed successfully, otherwise one of the 
standard error codes. 
*/
EXPORT_C TInt CContactDatabase::GetCurrentDatabase(TDes& aDatabase) const
	{
	return iCntSvr->GetCurrentDatabase(aDatabase);
	}


/** 
Where there are multiple contact databases on a device, this function can be 
used to set a database as the current one.

Note: this function simply updates a file name which is stored by the contacts server 
and its use is optional. It is provided as part of current item functionality. 
In order to pass a current item from one contacts model client to another, 
the receiving client needs to be using the same database.

@deprecated
@capability WriteUserData

@param aDatabase The path and filename of the current database.

@return KErrNone if the function completed successfully, otherwise one of the 
standard error codes. 
*/
EXPORT_C TInt CContactDatabase::SetCurrentDatabase(const TDesC& aDatabase) const
	{
	return iCntSvr->SetCurrentDatabase(aDatabase);
	}


/**
Starts a new transaction, without placing a cleanup item to rollback
the database onto the cleanupstack. This is to enable clients to call
contacts methods from an active object.

@publishedPartner
@released
@capability WriteUserData

@param aIsInTransaction ETrue if transaction already started

@leave KErrDiskFull if used storage space above threshold
*/
EXPORT_C void CContactDatabase::DatabaseBeginL(TBool aIsInTransaction)
	{
	if (!aIsInTransaction)
		{
		User::LeaveIfError(iCntSvr->BeginDbTransaction());	
		}
	}
	

/**
Commits an existing transaction, without popping a cleanup item.

@publishedPartner
@released
@capability WriteUserData

@param aIsInTransaction ETrue if transaction already started
*/
EXPORT_C void CContactDatabase::DatabaseCommitL(TBool aIsInTransaction)
	{
	if (!aIsInTransaction)
		{
		User::LeaveIfError(iCntSvr->CommitDbTransaction());
		}
	}
	

/**
Force a rollback of the database.

@publishedPartner
@released
@capability WriteUserData
*/
EXPORT_C void CContactDatabase::DatabaseRollback()
	{
	iCntSvr->RollbackDbTransaction();	
	}
	
	
/**
This method allows clients of contacts model to set the sorted item list 
and sort order from a default sort order server as proposed for Crystal 6.0

Note: This method can leave.

@param aSortedItems Specifies an array sorted items
@param aSortOrder Specifies the sort order
@deprecated
@capability None
*/
EXPORT_C void CContactDatabase::SetSortedItemsList(CContactIdArray* aSortedItems, CArrayFix<TSortPref>* aSortOrder)
	{
	__ASSERT_DEBUG(aSortedItems!=NULL,Panic(ECntPanicNullPointer));
	__ASSERT_DEBUG(aSortOrder!=NULL,Panic(ECntPanicNullPointer));

	delete iSortedItems;
	iSortedItems=aSortedItems;
	iCntSvr->SetSortPreferenceL(*aSortOrder); // this can leave
	delete aSortOrder;
	}


/**
aSortOrder is owned by the idle sorter.
@param aSortOrder Specifies the sort order
@param aStatus The request status for the asynchronous phase request.
@deprecated
*/
EXPORT_C void CContactDatabase::SortAsyncL(CArrayFix<TSortPref>* aSortOrder, TRequestStatus& aStatus)
	{
	iIdleSorter->ResetL();
	iIdleSorter->StartSortingL(aSortOrder, aStatus);
	}


/**
aSortOrder is owned by the idle sorter.
@param aSortOrder Specifies the sort order
@param aStatus The request status for the asynchronous phase request.
@deprecated
*/
EXPORT_C void CContactDatabase::SortAsyncL(CArrayFix<TSortPref>* aSortOrder, TRequestStatus& aStatus, MContactSortObserver& aObserver)
	{
	iIdleSorter->ResetL();
	iIdleSorter->StartSortingL(aSortOrder, aStatus, aObserver);
	}


/**
Cancel the CCntIdleSorter object and clean up resources.

@deprecated
*/
EXPORT_C void CContactDatabase::CancelAsyncSort()
	{
	iIdleSorter->Cancel();
	}


/**
Tests whether a contact item's hint bit field matches a filter.

For a match to occur, the item must be of the correct type for inclusion in 
the database (as returned by GetDbViewContactType()) and its hint bit field 
(which indicates whether the item contains a work or home telephone number, 
fax number or email address) must match the filter, according to the rules 
described in TContactViewFilter.

@internalTechnology
@capability None

@param aBitWiseFilter The filter to compare the item against. This is a combination 
of TContactViewFilter values.
@param aContactId The ID of the item in the database.

@return ETrue if the item is of the correct type for inclusion in the database, 
and its hint bit field matches the specified filter, EFalse if either of these 
conditions are not met.
*/
EXPORT_C TBool CContactDatabase::ContactMatchesHintFieldL(TInt aBitWiseFilter,TContactItemId aContactId)
	{//Returns ETrue if the contact Hint field matches any of the aBitWiseFilter fields
	return iCntSvr->ContactMatchesHintFieldL(aBitWiseFilter,aContactId);
	}


/**
Returns the ID of the template that should be used with CContactICCEntry items.

@capability None

@return A template ID.
*/
EXPORT_C TContactItemId CContactDatabase::ICCTemplateIdL()
	{
	LoadSyncPluginL();
	return iCntSvr->ICCTemplateIdL(KUidIccGlobalAdnPhonebook);
	}


/**
Returns the ID of the template that should be used with CContactICCEntry items 
belonging to the phonebook with TUid aPhonebookUid.

@capability None

@param aPhonebookUid The phonebook ID.

@return A template ID.
*/
EXPORT_C TContactItemId CContactDatabase::ICCTemplateIdL(TUid aPhonebookUid)
 	{
 	LoadSyncPluginL();
 	return iCntSvr->ICCTemplateIdL(aPhonebookUid);
 	}


/**
Returns the ID of the contacts model group which represents the ADN phonebook.

@capability None

@return Group ID.
*/ 
EXPORT_C TContactItemId CContactDatabase::PhonebookGroupIdL()
	{
	LoadSyncPluginL();
	return iCntSvr->PhonebookGroupIdL();
	}


/**
Returns a list of 'unfiled' contacts. These are contacts which do not belong to any group.

@publishedPartner
@released
@capability ReadUserData

@return The list of 'unfiled' contacts.
*/
EXPORT_C CContactIdArray* CContactDatabase::UnfiledContactsL()
	{
	MLplCollection& collection = FactoryL()->GetCollectorL();
	return collection.CollectionL(MLplCollection::EUnfiled);
	}


/**
Opens the default contact database asynchronously.

The Contacts server is asked to prepare the database to be opened. This may include
cleaning up incomplete writes from when the device was last switched off, or updating the 
database format.

If an error is encountered starting the asynchronous open the return value is NULL and the
error is returned in the TRequestStatus parameter.

Errors from the asynchronous open include:
KErrNotFound The database file was not found or it did not have the correct UIDs.
KErrLocked The file is in use by another client.
Other system wide error codes.

If the return value is not NULL the ownership of the CContactOpenOperation object is passed 
to the client. This may be deleted before the asynchronous open completes.

When the client supplied TRequestStatus is completed with KErrNone the TakeDatabase() method
of CContactOpenOperation is called to pass ownership of the open database to the client.

@publishedAll
@released
@capability ReadUserData

@param	aStatus
		On return, the request status for the asynchronous phase request.
		The Open() action can fail with one of the system wide error codes. In this case, the
		CContactDatabase object cannot access the database and must be deleted.
@param	aAccess
		This argument should be ignored by developers.

@return NULL if there is an error starting the asynhchronous open, otherwise a pointer to an
		active object that manages the open operation.

@see CContactOpenOperation
*/
EXPORT_C CContactOpenOperation* CContactDatabase::Open(TRequestStatus& aStatus, TThreadAccess /*aAccess*/)
	{
	aStatus = KRequestPending;
	CContactOpenOperation* openOperation = NULL;
	
	TRAPD(newError, openOperation = CContactOpenOperation::NewL(aStatus));
	// failed? return the error in the TRequestStatus
	if (newError != KErrNone)
		{
		TRequestStatus* ptrStatus = &aStatus;
		User::RequestComplete(ptrStatus, newError);
		}
	return openOperation;
	}


/**
Opens a named contact database asynchronously.

The Contacts server is asked to prepare the database to be opened. This may include
cleaning up incomplete writes from when the device was last switched off, or updating the 
database format.

In v8.1, contact databases can be located in any directory on any writeable drive, and the 
format of the filename must include an absolute directory path such as 
c:\\system\\data\\contacts.cdb.

From v9.0 onwards, contact databases can only be located in the correct data caged 
subdirectory. The filenames must have no path, for example c:contacts.cdb.
The maximum length for the drive, filename and extension is 190 characters.

If an empty path is entered, it will be treated as a request to open the default contact 
database.

If an error is encountered starting the asynchronous open the return value is NULL and the
error is returned in the TRequestStatus parameter.

Errors from the asynchronous open include:
KErrNotFound The database file was not found or it did not have the correct UIDs.
KErrLocked The file is in use by another client.
KErrBadName The filename is invalid; for example it includes wildcard characters
or the drive is missing.
Other system wide error codes.

If the return value is not NULL the ownership of the CContactOpenOperation object is passed 
to the client. This may be deleted before the asynchronous open completes.

When the client supplied TRequestStatus is completed with KErrNone the TakeDatabase() method
of CContactOpenOperation is called to pass ownership of the open database to the client.

@publishedAll
@released
@capability ReadUserData

@param	aFileName
		The filename of the database to open.
@param	aStatus
		On return, the request status for the asynchronous phase request.
		The Open() action can fail with one of the system wide error codes. In this case the
		CContactDatabase object cannot access the database and must be deleted.
@param	aAccess
		This argument should be ignored by developers.

@return NULL if there is an error starting the asynhchronous open, otherwise a pointer to an
		active object that manages the open operation.

@see CContactOpenOperation
*/
EXPORT_C CContactOpenOperation* CContactDatabase::Open(const TDesC& aFileName, TRequestStatus& aStatus, TThreadAccess /*aAccess*/)
	{
	TRequestStatus* ptrStatus = &aStatus;
	CContactOpenOperation* openOperation = NULL;

	TRAPD(newError, openOperation = CContactOpenOperation::NewL(aFileName, aStatus));
	// failed? return the error in the TRequestStatus
	if (newError != KErrNone)
		{
		User::RequestComplete(ptrStatus, newError);
		}

	return openOperation;
	}


/**
A static method to list the contact databases on all drives.

In v8.1, this function finds contact databases located anywhere on the drives, 
and the format of the returned filenames is c:\\system\\data\\contacts.cdb.

From v9.0 onwards, this function finds contact databases only in the correct 
data caged subdirectory. The returned filenames have no path, for example 
c:contacts.cdb. The maximum length for the drive, filename and extension is 190 
characters.

In either case, the filenames returned are in the correct format for Open(), 
OpenL(), CreateL(), ReplaceL() and DeleteDatabaseL().

@publishedAll
@released
@capability ReadUserData

@return An array containing zero or more contact database names.

@leave KErrNoMemory Out of memory.
*/
EXPORT_C CDesCArray* CContactDatabase::ListDatabasesL()
	{
	CContactDatabase* db = NewLC();
	CDesCArray* list = db->iCntSvr->ListDatabasesL();
	CleanupStack::PopAndDestroy(db);
	return list;
	}


/**
A static method to list the contact databases on a specified drive.

In v8.1, this function finds contact databases located anywhere on the drive, 
and the format of the returned filenames is c:\\system\\data\\contacts.cdb.

From v9.0 onwards, this function finds contact databases only in the correct 
data caged subdirectory. The returned filenames have no path, for example 
c:contacts.cdb. The maximum length for the drive, filename and extension is 190 
characters.

In either case, the filenames returned are in the correct format for Open(), 
OpenL(), CreateL(), ReplaceL() and DeleteDatabaseL().

@publishedAll
@released
@capability ReadUserData

@param	aDriveUnit
		The drive unit to search for contact databases.

@return An array containing zero or more contact database names.

@leave	KErrNoMemory Out of memory.
*/
EXPORT_C CDesCArray* CContactDatabase::ListDatabasesL(TDriveUnit aDriveUnit)
	{
	CContactDatabase* db = NewLC();
	CDesCArray* list = db->iCntSvr->ListDatabasesL(&aDriveUnit);
	CleanupStack::PopAndDestroy(db);
	return list;
	}


/**
A static method to delete a named contact database.

If the file is found, it is tested for the correct UIDs.

In v8.1, contact databases can be located in any directory on any writeable drive, 
and the format of the filename must include an absolute directory path such as 
c:\\system\\data\\contacts.cdb.

From v9.0 onwards, contact databases can only be located in the correct data caged 
subdirectory. The filenames must have no path, for example c:contacts.cdb.
The maximum length for the drive, filename and extension is 190 characters.

@publishedAll
@released
@capability WriteUserData

@param	aFileName
		The contact database file to delete.

@leave	KErrBadName The filename is invalid; for example it contains 
wildcard characters or the drive is missing.
@leave	KErrInUse Another client has the database open.
@leave	KErrNotFound The database file was not found or it did not have the correct UIDs.
@leave  KErrArgument if the given descriptor contains more than the maximum length 
        of 190 characters.

@see CContactDatabase::DeleteDefaultFileL()
*/
EXPORT_C void CContactDatabase::DeleteDatabaseL(const TDesC& aFileName)
	{
	CContactDatabase* db = NewLC();
	User::LeaveIfError(db->iCntSvr->DeleteDatabase(aFileName));
	CleanupStack::PopAndDestroy(db);
	}


/**
A static method to determine if the default contact database exists. 

It searches the drive set by SetDatabaseDriveL(), or if no drive has been 
set, it searches drive c:.

If the file is found, it is tested for the correct UIDs.

@publishedAll
@released
@capability None

@return ETrue if the file is found, EFalse otherwise.
@leave	KErrNotReady There is no media present in the drive.
@leave	KErrNotFound The database file was not found or it did not have the correct UIDs.
@leave	KErrCorrupt The file is not a valid database 

@see CContactDatabase::ContactDatabaseExistsL()
*/
EXPORT_C TBool CContactDatabase::DefaultContactDatabaseExistsL()
	{	
	CContactDatabase* db = NewLC();
	TBool theFlag = db->iCntSvr->DatabaseExistsL();
	CleanupStack::PopAndDestroy(db);	
	return theFlag;
	}


/**
A method to determine if a named contact database exists.

If the file is found, it is tested for the correct UIDs.

In v8.1, contact databases can be located in any directory on any writeable drive, 
and the format of the filename must include an absolute directory path such as 
c:\\system\\data\\contacts.cdb.

From v9.0 onwards, contact databases can only be located in the correct data caged
subdirectory. The filenames must have no path, for example c:contacts.cdb.
The maximum length for the drive, filename and extension is 190 characters.

@publishedAll
@released
@capability None

@param	aFileName
		The contact database to search for.

@return ETrue if the file is found, EFalse otherwise.

@leave	KErrNotReady There is no media present in the drive.
@leave	KErrBadName The filename is invalid; for example it contains 
wildcard characters or the drive is missing.
@leave	KErrNotFound The database file was not found or it did not have the correct UIDs.
@leave	KErrCorrupt The file is not a valid database 
@leave  KErrArgument if the given descriptor contains more than the maximum length 
        of 190 characters.

@see CContactDatabase::DefaultContactDatabaseExistsL()
@see CContactDatabase::ListDatabasesL()
*/
EXPORT_C TBool CContactDatabase::ContactDatabaseExistsL(const TDesC& aFileName)
	{
	// Leave if the filename format is bad
	TParse parseName;
	User::LeaveIfError(parseName.SetNoWild(aFileName,NULL,NULL));	
	CContactDatabase* db = NewLC();
	TBool dbExists = db->iCntSvr->DatabaseExistsL(aFileName);
	CleanupStack::PopAndDestroy(db);
	
	return dbExists;
	}


/**
Searches the database for a text string.  The function searches the fields
contained in the field definition.  The caller takes ownership of the returned
object. There is a limit of 255 characters on the search string length, due to
the implementation of the DBMS API, which also has a search string length
restriction of 255 chars. If the search string passed in is over 255 characters
this method will leave with KErrArgument.
   
@param aText The text to search for.
@param aFieldDef Specifies the fields to search.

@return Array of contact IDs identifying the contact items which contain the 
specified text.
*/
EXPORT_C CContactIdArray* CContactDatabase::FindLC(const TDesC& aText,const CContactItemFieldDef* aFieldDef)
	{
	CContactIdArray* idArray = iCntSvr->FindL(aText,aFieldDef);
	CleanupStack::PushL(idArray);
	return idArray;
	}


/**
Searches the database asynchronously for a text string. The function searches 
the fields contained in the field definition asynchronously using the MIdleFindObserver 
and CIdleFinder classes. The caller takes ownership of the returned object.

@param aText The text to search for.
@param aFieldDef Specifies the fields to search.
@param aObserver Implements the callback function IdleFindCallback(). NULL 
if no observer is needed.

@return A CIdle-derived object which provides information about the progress 
of the operation, and which can be used to retrieve an array of contact IDs.
*/
// !!! assert that the field definition doesn't contain any fields not included in the view?
// !!! should change to set the column found also
EXPORT_C CIdleFinder* CContactDatabase::FindAsyncL(const TDesC& aText,const CContactItemFieldDef *aFieldDef, MIdleFindObserver *aObserver)
	{
	return(CIdleFinder::NewL(*this,aText,aFieldDef,aObserver));
	}


/**
Enables the user to search the database for a string containing 
text that is stored in one or more fields.

The string is specified as an array of words.

For example, a user might want to search for the string "John Smith". 
To the user the string is a single item, but the text which makes up 
the string is stored in two separate fields in the database.

The caller of this function needs to provide an array of words to find 
(aFindWords), and a function to break down the text in the contact item 
into a list of words (aWordParserCallback).

The array of words to find would typically not contain punctuation. For 
example if the user searches for 'Smith, John' this would be passed to 
this function as an array of two words: 'Smith' and 'John', with the 
separator being discarded.

For a match to succeed, all words in the aFindWords array must match 
words in the array generated from the contact item by the aWordParserCallback 
function. To match, the word generated from the contact item must begin with 
the search word, i.e. a search for "Sm" would find any word beginning in "Sm". 
If a word is specified twice in the aFindWords array, then it must exist in 
two separate places in the contact item.

The function only searches the fields contained in the currently set text 
definition.

The caller takes ownership of the returned object.

@param aFindWords An array of words to find.
@param aWordParserCallback A function supplied by the caller to break the text 
in the contact down into a list of words.

@return Array of contact IDs.
*/
EXPORT_C CContactIdArray* CContactDatabase::FindInTextDefLC(const MDesCArray& aFindWords, const TCallBack &aWordParserCallback)
	{
	return(FindInTextDefLC(aFindWords,iTextDef,aWordParserCallback));
	}


/**
Enables the user to search the database for a string containing 
text that is stored in one or more fields.

The string is specified as an array of words.

For example, a user might want to search for the string "John Smith". 
To the user the string is a single item, but the text which makes up 
the string is stored in two separate fields in the database.

The caller of this function needs to provide an array of words to find 
(aFindWords), and a function to break down the text in the contact item 
into a list of words (aWordParserCallback).

The array of words to find would typically not contain punctuation. For 
example if the user searches for 'Smith, John' this would be passed to 
this function as an array of two words: 'Smith' and 'John', with the 
separator being discarded.

For a match to succeed, all words in the aFindWords array must match 
words in the array generated from the contact item by the aWordParserCallback 
function. To match, the word generated from the contact item must begin with 
the search word, i.e. a search for "Sm" would find any word beginning in "Sm". 
If a word is specified twice in the aFindWords array, then it must exist in 
two separate places in the contact item.

The function only searches the fields contained in the text definition aTextDef.

The caller takes ownership of the returned object.

@param aFindWords An array of words to find.
@param aTextDef The text definition.
@param aWordParserCallback A function supplied by the caller to break the text 
in the contact down into a list of words.

@return Array of contact IDs.
*/
EXPORT_C CContactIdArray* CContactDatabase::FindInTextDefLC(const MDesCArray& aFindWords,CContactTextDef* aTextDef, const TCallBack &aWordParserCallback)
	{
	CIdleFinder* idleFinder=FindInTextDefAsyncL(aFindWords,aTextDef,NULL,aWordParserCallback);
	CleanupStack::PushL(idleFinder);
	while(idleFinder->doFindL()) {};
	User::LeaveIfError(idleFinder->Error());
	CContactIdArray *ids=idleFinder->TakeContactIds();
	CleanupStack::PopAndDestroy();	// idleFinder
	CleanupStack::PushL(ids);
	return(ids);
	}


/**
Asynchronously searches the database for an array of words. 

This function works in the same way as its corresponding variant in FindInTextDefLC(), 
except that it operates asynchronously using the MIdleFindObserver and CIdleFinder 
classes. The caller takes ownership of the returned object.

@param aFindWords An array of words to find.
@param aObserver Implements the callback function IdleFindCallback(). May be NULL if 
no observer is needed.
@param aWordParserCallback A function to break the text in the contact down into a 
list of words.

@return A CIdle-derived object which provides information about the progress of the 
operation, and which can be used to retrieve an array of contact IDs.
*/
EXPORT_C CIdleFinder* CContactDatabase::FindInTextDefAsyncL(const MDesCArray& aFindWords, MIdleFindObserver *aObserver, const TCallBack &aWordParserCallback)
	{
	return(FindInTextDefAsyncL(aFindWords,iTextDef,aObserver,aWordParserCallback));
	}


/**
Asynchronously searches the database for an array of words. 

This function works in the same way as its corresponding variant in FindInTextDefLC(), 
except that it operates asynchronously using the MIdleFindObserver and CIdleFinder 
classes. The caller takes ownership of the returned object.

@param aFindWords An array of words to find.
@param aTextDef The text definition.
@param aObserver Implements the callback function IdleFindCallback(). May be NULL if 
no observer is needed.
@param aWordParserCallback A function to break the text in the contact down into a 
list of words.

@return A CIdle-derived object which provides information about the progress of the 
operation, and which can be used to retrieve an array of contact IDs.
*/
EXPORT_C CIdleFinder* CContactDatabase::FindInTextDefAsyncL(const MDesCArray& aFindWords,const CContactTextDef* aTextDef, MIdleFindObserver *aObserver, const TCallBack &aWordParserCallback)
	{
	return(CIdleFinder::NewL(*this,&aFindWords,aTextDef,aObserver,aWordParserCallback));
	}


/**
This function is not currently supported.
@param aFormat This parameter should be ignored.
@deprecated
*/
EXPORT_C void CContactDatabase::SetDateFormatTextL(const TDesC& /*aFormat*/)
	{//Does nothing, Parameter should be ignored
	}


/**
Returns an array of contact item IDs for all the contact items which may contain
the specified telephone number in a telephone, fax or SMS type field.

The comparison method used is not exact. The number is compared starting from
the right side of the number and the method returns an array of candidate 
matches. Punctuation (eg. spaces) and other alphabetic characters are 
ignored when comparing.

Additionally, if the contacts model phone parser (CNTPHONE.DLL) is available, 
then any DTMF digits are also excluded from the comparision.

Notes:
Due to the way numbers are stored in the database, it is recommended that at
least 7 match digits are specified even when matching a number containing fewer
digits. Failure to follow this guideline may (depending on the database contents)
mean that the function will not return the expected Contacts Id set.
  
@released
@capability ReadUserData

@param aNumber Phone number string. If the length of phone number string is greater than 
KCntMaxTextFieldLength then only the first KCntMaxTextFieldLength characters are used 
in the match.
@param aMatchLengthFromRight Number of digits from the right of the phone number to use
Up to 15 digits can be specified, and it is recommended that at least 7 match digits are
specified.

@return CContactIdArray of candidate matches
*/
EXPORT_C CContactIdArray* CContactDatabase::MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight)
  	{
  	return iCntSvr->MatchPhoneNumberL(aNumber, aMatchLengthFromRight);
	}


TInt CContactDatabase::ContactPosL(TContactItemId aContactId) //for cntiter
	{
	return SortedItemsL()->Find(aContactId);
	}


TInt CContactDatabase::DoGotoL(TContactItemId /*aContactId*/)
	{
    return KErrNone; 
	}


EXPORT_C void CContactDatabase::AddObserverL(MContactDbObserver& aChangeNotifier)
	{
	iCntSvr->AddObserverL(aChangeNotifier);
	}


EXPORT_C void CContactDatabase::RemoveObserver(const MContactDbObserver& aChangeNotifier)
	{
	iCntSvr->RemoveObserver(aChangeNotifier);
	}
	
EXPORT_C void CContactDatabase::AddObserverV2L(MContactDbObserverV2& aChangeNotifier)
    {
    iCntSvr->AddObserverV2L(aChangeNotifier);
    }


EXPORT_C void CContactDatabase::RemoveObserverV2(const MContactDbObserverV2& aChangeNotifier)
    {
    iCntSvr->RemoveObserverV2(aChangeNotifier);
    }

void CContactDatabase::CancelNotifyRequestL()
	{
	}


TBool CContactDatabase::IsICCSynchronisedL()
	{
	return EFalse;
	}


MLplPersistenceLayerFactory* CContactDatabase::FactoryL()
	{
	if(iProxyFactory == NULL)
		{
		iProxyFactory = CProxyFactory::NewL(*this);
		}
	return iProxyFactory;
	}
	

void CContactDatabase::FetchGroupAndTemplateListsL()
	{
	delete iCardTemplateIds;
	// Set the iCardTemplateIds to null in order to avoid a corruption
	// should the FetchTemplateListIdsL leave. 
	iCardTemplateIds = NULL;
	iCardTemplateIds = iCntSvr->FetchTemplateListIdsL(); 
	
	delete iGroupIds;
	iGroupIds = NULL; // See previous comment
	iGroupIds = iCntSvr->FetchGroupListIdsL();	
	}
	
void CContactDatabase::LoadSyncPluginL()
    {
	if (iContactSynchroniser == NULL)
	    {
	    //Instantiate a CContactSynchroniser object which loads the plugin.
	    //This is required to avoid a deadlock in the contacts server code.
		iContactSynchroniser = CContactSynchroniser::NewL();
	    }
    }

/**
@capability ReadUserData
*/
CContactOpenOperation* CContactOpenOperation::NewL(TRequestStatus& aPtrStatus)
	{
	CContactOpenOperation* self = new (ELeave) CContactOpenOperation(aPtrStatus);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
@capability ReadUserData
*/
CContactOpenOperation* CContactOpenOperation::NewL(const TDesC& aFileName, TRequestStatus& aPtrStatus)
	{
	CContactOpenOperation* self = new (ELeave) CContactOpenOperation(aPtrStatus);
	
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);
	return self;
	}


CContactOpenOperation::CContactOpenOperation(TRequestStatus& aClientStatus) : 
CActive(EPriorityIdle), iClientStatus(&aClientStatus)
	{}	
	
/**
@capability ReadUserData
@leave  KErrArgument if the given descriptor contains more than the maximum length 
        of 190 characters.
*/
void CContactOpenOperation::ConstructL(const TDesC& aFileName)
	{
	iContactDatabase = CContactDatabase::NewLC();
	CleanupStack::Pop(iContactDatabase);
	iContactDatabase->iCntSvr->OpenDatabaseAsyncL(*iClientStatus, aFileName);
	}


/**
For BC only, CActive is not used here.
*/
void CContactOpenOperation::RunL()
	{
	}


/**
For BC only, CActive is not used here.
*/
TInt CContactOpenOperation::RunError(TInt)
	{
	return KErrNone;
	}


/**
For BC only, CActive is not used here.
*/
void CContactOpenOperation::DoCancel()
	{
	}


/**
Takes ownership of the contact database.

Ownership of the contact database is passed to the client.
Subsequent calls return NULL.

@return A pointer to the CContactDatabase on the first call after 
		the asynchronous open has succeeded. Otherwise NULL.

@see CContactDatabase::Open()
*/
EXPORT_C CContactDatabase* CContactOpenOperation::TakeDatabase()
	{
	// async open must have succeeded in server AND RunL must have completed
	CContactDatabase* contactDatabase = iContactDatabase;
	// prevent deletion of the database.
	iContactDatabase = NULL;
	return contactDatabase;
	}


/**
Deletes the active object.

If the asynchronous open is still in progress it is cancelled.

If the TakeDatabase() API has not been called and the database 
has been opened it is closed.

@capability None
*/
EXPORT_C CContactOpenOperation::~CContactOpenOperation()
	{
	if (iContactDatabase)
		{
		iContactDatabase->iCntSvr->CancelAsyncOpen();
		delete iContactDatabase;
		iContactDatabase = NULL;
		}
	}


CDataBaseChangeObserver* CDataBaseChangeObserver::NewL(MContactDbPrivObserver& aPrivateObserver)
	{
	CDataBaseChangeObserver* temp = new (ELeave) CDataBaseChangeObserver(aPrivateObserver);
	return temp;
	}


CDataBaseChangeObserver::CDataBaseChangeObserver(MContactDbPrivObserver& aPrivateObserver) : iPrivateObserver(aPrivateObserver)
	{}	


void CDataBaseChangeObserver::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	iPrivateObserver.HandleDatabaseEventL(aEvent);
	}


CDataBaseChangeObserver::~CDataBaseChangeObserver()
	{}	


void CContactDatabase::HandleDiskSpaceEvent(TInt)
/** Default behaviour for handling a low disk event - This function is unimplemented. */
	{}//Do nothing.
	

void CContactDatabase::RespondToEventL(const TContactDbObserverEventType aEventType, const TContactItemId aContactId)
	{
	switch(aEventType)
		{
		case EContactDbObserverEventContactChanged:
		case EContactDbObserverEventGroupChanged:
			HandleDbObserverEventGroupOrContactChangedL(aContactId);
			break;
		
		case EContactDbObserverEventContactAdded:
		case EContactDbObserverEventGroupAdded:
			HandleDbObserverEventGroupOrContactAddedL(aEventType, aContactId);
			break;

		case EContactDbObserverEventSpeedDialsChanged:
			break;
			
		case EContactDbObserverEventOwnCardDeleted:
		case EContactDbObserverEventContactDeleted:
            RemoveFromSortArray(aContactId);
			break;
		
		case EContactDbObserverEventGroupDeleted:
			HandleDbObserverEventGroupDeletedL(aContactId);
			break;
		
		case EContactDbObserverEventTemplateAdded:
			{
			if (!iCardTemplateIds)
				{
				//if database was opened async, iCardTemplate is null.
				//keep same behaviour like CContactDatabase::AddToTemplateListL
				iCardTemplateIds = CContactIdArray::NewL();
				}
			iCardTemplateIds->AddL(aContactId);
			}
			break;

		case EContactDbObserverEventTemplateChanged:
			{
			iTemplateCache->RemoveTemplate(aContactId);
			}
			break;

		case EContactDbObserverEventTemplateDeleted:
			{
			iTemplateCache->RemoveTemplate(aContactId);
			RemoveFromTemplateList(aContactId);
			}
			break;
		
		case EContactDbObserverEventOwnCardChanged:
			break;

		case EContactDbObserverEventBackupBeginning:
		case EContactDbObserverEventRestoreBeginning:
		case EContactDbObserverEventBackupRestoreCompleted:
		case EContactDbObserverEventRestoreBadDatabase:
			// NB this handler decides on the event to send to observers
		//	HandleBackupOrRestoreEvent(aEvent);
			return;

		case EContactDbObserverEventTablesOpened:
			iTablesOpen = ETrue;
			break;
			
		case EContactDbObserverEventTablesClosed:
			iTablesOpen = EFalse;
			break;

		case EContactDbObserverEventUnknownChanges:
			{
			FetchGroupAndTemplateListsL();
			// Reset the template cache
			delete iTemplateCache;
            iTemplateCache = NULL;
			iTemplateCache = CCntTemplateCache::NewL(*iCntSvr); 
			
			TInt err=KErrGeneral;
			if (iTablesOpen)
				{
				CArrayFix<TSortPref>* sortOrder = const_cast<CArrayFix<TSortPref>*>(iCntSvr->GetSortPreferenceL());
				CleanupStack::PushL(sortOrder);
				TRAP(err,ReSortL(sortOrder));
				CleanupStack::PopAndDestroy(sortOrder);
				}
			CheckSortError(err);
			}
			break;
		default:;
		}	
	}


/**  
Handle the Database event
@internalTechnology
@param aEvent Database change event
*/
EXPORT_C void CContactDatabase::HandleDatabaseEventL(const TContactDbObserverEvent& aEvent)
	{
	//Just respond to events created by other sessions
	if (ConnectionId()!=aEvent.iConnectionId)
		{
		RespondToEventL(aEvent.iType, aEvent.iContactId);
		}
	}
	

void CContactDatabase::RemoveFromSortArray(TContactItemId aContactId)
	{
	if (iSortedItems)
		{
		const TInt pos=iSortedItems->Find(aContactId);
		if (pos!=KErrNotFound)
			iSortedItems->Remove(pos);
		}
	}
	

void CContactDatabase::RemoveFromGroupIds(const TContactItemId aContactId)
	{
     if (iGroupIds) 
         {
          TInt pos = iGroupIds->Find(aContactId); 
          if ( pos != KErrNotFound ) 
              { 
              iGroupIds->Remove( pos ); 
              }
         } 
	}


void CContactDatabase::HandleDbObserverEventGroupDeletedL(const TContactItemId aContactId)
    {
	 RemoveFromGroupIds(aContactId);
     RespondToEventL(EContactDbObserverEventContactDeleted, aContactId);
    }


TBool CContactDatabase::AddContactToSortListL(TContactItemId aReqId, TContactItemId& aActualId,CBase* aItems, TUid& aFieldType, TBool aHasSortOrder)
	{
	TUid contactType;
	TBool deleted;
	
	if(!FactoryL()->GetCollectorL().SeekContactL(aReqId,aActualId,contactType,deleted))
		{
		return EFalse;
		}
	
	if(deleted || contactType != KUidContactCard)
		{
		return ETrue;
		}
	if (!aHasSortOrder)
		{
		((CContactIdArray*)aItems)->AddL(aActualId);
		}
	else
		{
		CContactDatabase::TTextFieldMinimal textFieldMin;
		if	(aFieldType == KUidContactFieldDefinedText)
			{
			ReadContactTextDefL(aActualId,textFieldMin,iTextDef);
			}
		else
			{
			FactoryL()->GetViewIteratorManagerL().TextFieldL(aActualId,aFieldType,textFieldMin);
			}
		((CSortArray*) aItems)->AppendL(textFieldMin,aActualId);
		}
	return ETrue;
	}


TInt CContactDatabase::NextExistingL(TInt aIndex)
	{
	TInt ret=KErrNotFound;
	const CContactIdArray* sortedItems=SortedItemsL();
	const TInt count=sortedItems->Count();
	TContactItemId contactId=KNullContactId;
	while ((ret==KErrNotFound) && (aIndex<count-1))
		{
		contactId=(*sortedItems)[++aIndex];
		ret=DoGotoL(contactId);
		}
	if (ret==KErrNotFound)
		return ret;
	return aIndex;
	}


TInt CContactDatabase::PreviousExistingL(TInt aIndex)
	{
	TInt ret=KErrNotFound;
	TContactItemId contactId=KNullContactId;
	const CContactIdArray* sortedItems=SortedItemsL();
	while (ret==KErrNotFound && aIndex>=1)
		{
		contactId=(*sortedItems)[--aIndex];
		ret=DoGotoL(contactId);
		}
	if (ret==KErrNotFound)
		return ret;
	return aIndex+1;
	}


TInt CContactDatabase::NewSortIndexL(const CContactItem& aContact, TInt aMin, TInt aMax)
	{
	__ASSERT_DEBUG(&aContact!=NULL,Panic(ECntPanicNullPointer));
	FOREVER
		{
		if (aMin==aMax)
			return(aMin);
		TInt index=(aMax-aMin)/2+aMin;
		User::LeaveIfError(DoGotoL((*iSortedItems)[index]));
		TInt compare=CompareSortFieldsL(aContact);
		if (compare<=0)
			{
			if (aMin==index)
				aMin++;
			else
				aMin=index;
			}
		else if (compare>0)
			aMax=index;
		}
	}


TBool CContactDatabase::CheckSortError(TInt aError)
	{
	if (aError!=KErrNone)
		{
		delete iSortedItems;
		iSortedItems=NULL;
		return(ETrue);
		}
	return(EFalse);
	}


void CContactDatabase::MoveInSortArray(const CContactItem& aContact)
	{
	TRAPD(err,MoveInSortArrayL(aContact));
	CheckSortError(err);
	}


void CContactDatabase::InsertInSortArray(const CContactItem& aContact)
	{
	TRAPD(err,InsertInSortArrayL(aContact));
	CheckSortError(err);
	}


void CContactDatabase::InsertInSortArrayL(const CContactItem& aContact)
	{
	__ASSERT_DEBUG(&aContact!=NULL,Panic(ECntPanicNullPointer));
	if (CheckType(aContact.Type()) && iSortedItems)
		{
		const TContactItemId id=aContact.Id();
		if (iSortedItems->Find(id)==KErrNotFound)
			{
			const TInt index=NewSortIndexL(aContact,0,iSortedItems->Count());
			if (index<iSortedItems->Count())
				iSortedItems->InsertL(index,id);
			else
				iSortedItems->AddL(id);
			}
		}
	}


void CContactDatabase::MoveInSortArrayL(const CContactItem& aContact)
	{
	__ASSERT_DEBUG(&aContact!=NULL,Panic(ECntPanicNullPointer));
	if (iSortedItems && CheckType(aContact.Type()))
		{
		const TInt pos=iSortedItems->Find(aContact.Id());
		TInt compare=0;
		if (pos!=0 && PreviousExistingL(pos)!=KErrNotFound)
			compare=CompareSortFieldsL(aContact);
		TInt index;
		TInt start=0;
		TInt end=pos;
		if (compare<=0)
			{
			if (NextExistingL(pos)!=KErrNotFound)
				compare=CompareSortFieldsL(aContact);
			if (compare>=0)
				return;
			start=pos;
			end=iSortedItems->Count();
			}
		index=NewSortIndexL(aContact,start,end);
		iSortedItems->MoveL(pos,index>pos?index-1:index);
		}
	}


TInt CContactDatabase::CompareSortFieldsL(const CContactItem& aContact)
	{
	TInt compare=0;
	TInt index=0;
	CArrayFix<TSortPref>* sortOrder = iCntSvr->GetSortPreferenceL();
	CleanupStack::PushL(sortOrder);
	const TInt sortDefs=sortOrder->Count();
	while (compare==0 && index<sortDefs)
		{
		TUid fieldType=(*sortOrder)[index++].iFieldType;
		TTextFieldMinimal textFieldMin;
		TTextFieldMinimal textFieldMin2;
		if (fieldType==KUidContactFieldDefinedText)
			{
			// Reads from database.
			ReadContactTextDefL(aContact.Id(),textFieldMin);
			// Does not read from database.
			ReadContactTextDefL(aContact,textFieldMin2);
			}
		else
			{
			MLplViewIteratorManager& manager = FactoryL()->GetViewIteratorManagerL();
			manager.TextFieldL(aContact.Id(),fieldType,textFieldMin);
			aContact.CardFields().NonZeroFieldText(fieldType,textFieldMin2);
			}
		compare=textFieldMin.CompareC(textFieldMin2,3,&iCollateMethod);		
		}
	if (index>0)
		--index;
	
	TInt retVal = ((sortDefs==0) || ((*sortOrder)[index].iOrder==CContactDatabase::TSortPref::EAsc)? compare : -compare);
	CleanupStack::PopAndDestroy(); //sortOrder
	return retVal;
	}


/**
Looks at the sort order and identifes what tables are required.
*/
void CContactDatabase::ConstructTableUsageFlagsFromSortOrderL(TInt& aFlags)
	{
	CContactTextDef* textDef=CContactTextDef::NewLC();
	CArrayFix<TSortPref>* sortOrder = iCntSvr->GetSortPreferenceL();
	CleanupStack::PushL(sortOrder);
	TInt sortOrderCount=sortOrder->Count();
	TInt columns=0;
	aFlags=0;
	for (TInt sortIndex=0;sortIndex<sortOrderCount;sortIndex++)
		textDef->AppendL(TContactTextDefItem((*sortOrder)[sortIndex].iFieldType));
	MLplCollection& collection = FactoryL()->GetCollectorL();
	collection.ConstructBitwiseFlagsFromTextDef(aFlags,columns,textDef);
	CleanupStack::PopAndDestroy(sortOrder);
	CleanupStack::PopAndDestroy(textDef);
	}


/**
Update sorted items list using contact card/group change event.

@internalTechnology
@since 7.0

@param aEvent Database change event
*/ 
void CContactDatabase::HandleDbObserverEventGroupOrContactChangedL(const TContactItemId aContactId)
	{
	if (iSortedItems)
		{
	 	CArrayFix<TSortPref>* sortOrder = iCntSvr->GetSortPreferenceL();
		CleanupStack::PushL(sortOrder);
		if(sortOrder!=NULL && sortOrder->Count() > 0)
			{
			if (iTablesOpen)
				{
				CContactItem* contact = NULL;
				TRAPD(error, contact = ReadContactL(aContactId));
				if (CheckSortError(error)==EFalse && contact != NULL)
					{
					MoveInSortArray(*contact);
					delete contact;
					}
				}
			else
				{
				CheckSortError(KErrGeneral);
				}
			}
		CleanupStack::PopAndDestroy(sortOrder);
		}
	}


/**
Add new contact card/group to sorted items list using add event.

@internalTechnology
@since 7.0

@param aEvent Database change event
*/ 
void CContactDatabase::HandleDbObserverEventGroupOrContactAddedL(const TContactDbObserverEventType aEventType, const TContactItemId aContactId)
	{
	if (aEventType == EContactDbObserverEventGroupAdded)
		{
		if(iGroupIds == NULL)
			{
			//if database was opened async, iGroups is null, so we have to fetch the group ids first
			iGroupIds = iCntSvr->FetchGroupListIdsL();	
			if(iGroupIds->Find(aContactId) == KErrNotFound)
				{
				iGroupIds->AddL(aContactId);	
				}
			}
		else 
			{
			iGroupIds->AddL(aContactId);		
			}	
		}

	if (iSortedItems)
		{
		if (iTablesOpen)
			{
			CContactItem* contact = NULL;
			TInt error = KErrGeneral;
			CArrayFix<TSortPref>* sortOrder = iCntSvr->GetSortPreferenceL();
			CleanupStack::PushL(sortOrder);
			if(sortOrder!= NULL && sortOrder->Count() > 0)
				//There is a sort order, so the contact must be read
				//to find out where to insert it into the sorted list
				{
				TRAP(error, contact = ReadContactL(aContactId));
				CheckSortError(error);
				if (error == KErrNone && contact != NULL)
					{
					InsertInSortArray(*contact);
					delete contact;
					}
				}
			else
				{
				// No sort order is defined only the contact's type is needed.
				TContactItemId actualId;
				TUid contactType = KNullUid;
				TBool deleted = EFalse;
				TBool found = EFalse;
	
        		MLplCollection& collection = FactoryL()->GetCollectorL();
				TRAP(error, found = collection.SeekContactL(aContactId,actualId,contactType,deleted));
				
				if (found && CheckSortError(error)==EFalse)
					{
					if (CheckType(contactType))
						{
						iSortedItems->AddL(aContactId);
						}
					}
				}
			CleanupStack::PopAndDestroy(sortOrder);
			}
		else // iTablesOpen == EFalse
			{
			CheckSortError(KErrGeneral);
			}
		}
	}


/**
@internalTechnology
*/
void CContactDatabase::StartAsyncActivityL()
	{
	// starting: Recover, Async Find, Compress
	++iAsyncActivityCount;

	// At least one async activity in progress, tell server.
	if (iAsyncActivityCount == 1)
		{
		iCntSvr->SetAsyncActivityL(ETrue);
		}
	}
 
 
/**
@internalTechnology
*/
void CContactDatabase::EndAsyncActivityL()
	{
	// ending: Recover, Async Find, Compress
	--iAsyncActivityCount;
	
	// All async activities finished, tell server this so that it can process
	// any pending database close.
	if (iAsyncActivityCount == 0)
		{
		iCntSvr->SetAsyncActivityL(EFalse);
		}
	}


/**
Determine if the database is ready.  Ready in this context means that the
database is open and readable/writable (i.e. the state machine is in
CStateWritable).

@internalTechnology
@capability None

@return ETrue if the database is ready, EFalse if the database is not ready.
*/
TBool CContactDatabase::DatabaseReadyL() const
	{
	return iCntSvr->DatabaseReadyL();
	}


CCntIdleSorter::CCntIdleSorter(CContactDatabase &aContactDatabase)
:	iDb(aContactDatabase), iState(EReadContacts)
	{
	}


CCntIdleSorter::~CCntIdleSorter()
	{
	if (iSortStarted)
		{
		// async Sort blocks Backup or Restore closing the database handles
		TRAP_IGNORE(iDb.EndAsyncActivityL());
		iSortStarted = EFalse;
		}

	if	(iIdle)
		iIdle->Cancel();
	delete iIdle;
	delete iSortOrder;
	delete iSortedList;
	delete iFinalItems;
	}


void CCntIdleSorter::ConstructL()
	{
	iIdle		= CIdle::NewL(CActive::EPriorityIdle);
	iSortedList	= new(ELeave) CSortArray();
	iFinalItems	= CContactIdArray::NewL();

	// Must call this to ensure the system template is loaded
//	Not required in client for cntmodel
//	iDb.SystemTemplateL();
	}


CCntIdleSorter* CCntIdleSorter::NewL(CContactDatabase &aContactDatabase)
	{
	CCntIdleSorter* self= new(ELeave) CCntIdleSorter(aContactDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/**
Prepare and begin the sort.
*/
void CCntIdleSorter::StartSortingL(CArrayFix<CContactDatabase::TSortPref>* aSortOrder, TRequestStatus& aStatus)
	{
	// Take ownership of sort order
	iSortOrder = aSortOrder;

	// Save the TRequestStatus so we can signal the client
	iStatus = &aStatus;

	// Set up how many records we will have to read
	TotalCount() = iDb.CountL();

	// async Sort uses database tables, so Backup or Restore handling must wait for the async activity to finish
	iDb.StartAsyncActivityL();
	iSortStarted = ETrue;

	// Indicate that the sort is in progress. This will result in the client's
	// RunL (if it's using an AO framework) to be called when the sort is completed
	// or cancelled.
	*iStatus = KRequestPending;
	
	// Start the idle sorter.
	iIdle->Start(TCallBack(CCntIdleSorter::SortCallBack, this));
	}


/**
Sort with observer.
*/
void CCntIdleSorter::StartSortingL(CArrayFix<CContactDatabase::TSortPref>* aSortOrder, TRequestStatus& aStatus, MContactSortObserver& aObserver)
	{
	iObserver = &aObserver;
	HasObserver() = ETrue;
	StartSortingL(aSortOrder, aStatus);
	}


/**
Initialise values to defaults.
*/
void CCntIdleSorter::ResetL()
	{
//	iDb.iItemTable->iTable.BeginningL();
	iState = EReadContacts;
	iIdle->Cancel();

	delete iSortOrder;
	iSortOrder = NULL;

	delete iSortedList;
	iSortedList = NULL;
	iSortedList	= new(ELeave) CSortArray();

	if (iSortStarted)
		{
		// async Sort blocks Backup or Restore closing the database handles
		iDb.EndAsyncActivityL();
		iSortStarted = EFalse;
		}

	delete iFinalItems;
	iFinalItems = NULL;
	iFinalItems	= CContactIdArray::NewL();
	
	iObserver = NULL;
	iStatus = NULL;
	HasObserver() = EFalse;
	TotalCount() = 0;
	ReadSoFar() = 0;
	iCurrentId = 0;
	}


/**
Callback function for idle object.
*/
TInt CCntIdleSorter::SortCallBack(TAny* aThis)
	{
	return static_cast<CCntIdleSorter*>(aThis)->PeformSortStep();
	}


/**
Read a single contact.
*/
TBool CCntIdleSorter::ReadContactsL(TInt aNumber)
	{
	// Do we have to sort using specified order criteria?
	TBool moreToDo		= ETrue;
	TBool haveSortOrder	= (iSortOrder->Count() > 0);
	TUid fieldType		= (haveSortOrder) ? iSortOrder->At(0).iFieldType : KNullUid;
	CBase* array =  (haveSortOrder) ? (CBase*) iSortedList : (CBase*)iFinalItems;

	// Only retrieve aNumber contacts from the contact store before yeilding
	for (TInt i = 0; i < aNumber; i++)
		{
		++ReadSoFar();
		TContactItemId actualId(0);
		moreToDo = iDb.AddContactToSortListL(iCurrentId,actualId,array, fieldType, haveSortOrder) && moreToDo;
		iCurrentId = actualId+1;
		if(!moreToDo)
			break;
		}

	// Report progress to observer (if there is one)
	ReportProgressL();

	return moreToDo;
	}


/**
This function is called once the reading from the database is complete.
It replaces the contact database's sorted list (takes ownership of iFinalItems).
*/
void CCntIdleSorter::SortListAndSaveL()
	{
	iSortedList->SortL(iSortOrder->At(0).iOrder);
	iDb.SortDuplicatesL(*iSortOrder, *iSortedList, 1);
	const TInt count = iSortedList->Count();
	for (TInt i=0; i<count; i++)
		iFinalItems->AddL(iSortedList->Id(i));

	// Safe to do this now...
	delete iDb.iSortedItems;
	iDb.iSortedItems = iFinalItems;
	iFinalItems = NULL; // just so we don't delete it

	if (iSortStarted)
		{
		// async Sort blocks Backup or Restore closing the database handles
		iDb.EndAsyncActivityL();
		iSortStarted = EFalse;
		}

	// Set the sort order in the server.
	iDb.iCntSvr->SetSortPreferenceL(*iSortOrder);
	}


/**
Called as a result of a cancellation request by the CContactDatabase class.
*/
void CCntIdleSorter::Cancel() 
	{
	iIdle->Cancel();
	CompleteRequest(KErrCancel);
	TRAP_IGNORE(ResetL());
	}


/**
Complete the client's request status with the specified error value (KErrNone by default).
*/
void CCntIdleSorter::CompleteRequest(TInt aError)
	{
	if	(iStatus)
		User::RequestComplete(iStatus, aError);
	}


/**
if we have an observer, then we report progress as the sort is performed (in reality
we actually only report progress during the read stage, not the sort itself).
*/
void CCntIdleSorter::ReportProgressL()
	{
	if	(iObserver && HasObserver())
		iObserver->HandleSortEventL(Max(TotalCount(), ReadSoFar()), TotalCount());
	}


/**
Idle object callback which performs appropriate action based on internal state machine.
*/
TInt CCntIdleSorter::PeformSortStep()
	{
	TInt err = KErrNone;
	TBool moreToDo = EFalse;

	// Based on the current state machine value we perform either a read or a sort
	switch (iState)
		{
	case EReadContacts:
		// Read all contacts & when complete move to next state
		TRAP(err, moreToDo = ReadContactsL())
		if	(err)
			{
			// There was an error sorting
			CompleteRequest(err);
			moreToDo = EFalse;
			}
		else if	(!moreToDo) 
			{
			// Indicate that the reading is complete
			iState = ESortContacts;

			// Set this back to ETrue because we now have to do the sort...
			moreToDo = ETrue;
			}

		break;

	case ESortContacts:
		// Contact reading finished, now perform the sort
		TRAP(err, SortListAndSaveL());
		CompleteRequest(err);
		moreToDo = EFalse;
		break;
		}

	// Return a boolean indicating whether we want the idle to call this dispatch
	// function again in future.
	return moreToDo;
	}


/**
Allocates and constructs a new contact database change notifier.

@param aDatabase The contact database to observe.
@param aObserver The observer for aDatabase. Its HandleDatabaseEventL() function 
is called whenever a change occurs to the database.

@return Pointer to the newly created contact database change notifier.
*/
EXPORT_C CContactChangeNotifier* CContactChangeNotifier::NewL(CContactDatabase& aDatabase, MContactDbObserver* aObserver)
	{
	CContactChangeNotifier *self=new(ELeave) CContactChangeNotifier(aDatabase, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
	}
	

CContactChangeNotifier::CContactChangeNotifier(CContactDatabase& aDatabase, MContactDbObserver *aObserver) :
	iDatabase(aDatabase), iObserver(aObserver)
	{}


/**
Removes the observer from the contact database.
*/
EXPORT_C CContactChangeNotifier::~CContactChangeNotifier()
	{
    if (iObserver)
        {
	    iDatabase.RemoveObserver(*iObserver);
        }
	}

	
void CContactChangeNotifier::ConstructL()
	{
    if (iObserver)
        {
	    iDatabase.AddObserverL(*iObserver);
        }
	}


/**
Deletes a contact item. 
See doDeleteContactL() for details
*/
void CContactDatabase::DeleteContactSendEventActionL(TContactItemId aContactId, TCntSendEventAction aActionType)
	{
	iCntSvr->DeleteContactL(aContactId, aActionType);
	//Now we check if the contact belonged to the sort array, if so 
	//remove it from iSortedItems
	RemoveFromSortArray(aContactId);
	//Now we check if the contact belonged to the template list, if so 
	//remove it from iCardTemplateIds
	RemoveFromTemplateList(aContactId);
	//Now we check if the contact belonged to the Group Id list, if so 
	//remove it from iGroupIds	
	RemoveFromGroupIds(aContactId);
	}

