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

#include <s32mem.h>
#include <cntitem.h>
#include <cntdb.h>
#include <cntfilt.h>

#include "rcntmodel.h"
#include "persistencelayer.h" // for mlplcollection.
#include "ccntnotificationmonitor.h"
#include "ccontactprivate.h" // for mprogresseventhander.
#include "ccntpackager.h"
#include <cntviewstore.h>


/** Contacts server version number. */ 
const TInt KCntServerMajorVersionNumber=1;
const TInt KCntServerMinorVersionNumber=1;
const TInt KCntServerBuildVersionNumber=1;

/** Number of attempts to try restart the server after premature termination. */
const TInt KMaxTimesToRestartServer = 3;

/** Maximum number of asynchronous IPC calls. */
const TInt KAsyncMessageSlots=6;


/**
Unlock the last locked contact item on the server.  Used by OpenLX().  If any
method which locks a contact item leaves thenccnt this method is called as the
cleanup stack unwinds, removing the lock for the last locked contact item.

@capability None
*/
void CleanupUnlockRecord(TAny *aSession)
	{
	static_cast<RCntModel*>(aSession)->UnlockLastLockedContact();
	}


/**
Unlock the last locked contact item on the server.  Used by OpenLX().

@capability None
*/
void RCntModel::UnlockLastLockedContact()
	{
	CloseContact(KNullContactId);
	}


/**
Push a Contact item unlock cleanup item on the cleanup stack.
*/
void RCntModel::PushUnlockL() const
	{
	CleanupStack::PushL( TCleanupItem(CleanupUnlockRecord, const_cast<RCntModel *>(this)));
	}


/**
RCntModel constructor.

Member variables must be initialised (zero'd) in the constructor since
RCntModel does not derive from CBase.
*/
RCntModel::RCntModel()
	:
	iDbNotifyMonitor(NULL), 
	iPackager(NULL),
	iConnectionId(0),
	iNoOfSvrStartAttempts(0)
	{
	}


/**
Get the Contacts server version number.

@return Contacts server version number.
*/
TVersion RCntModel::Version() const
	{
	return(TVersion(KCntServerMajorVersionNumber,KCntServerMinorVersionNumber,KCntServerBuildVersionNumber));
	}


/** Name of the executable for the Contacts server. */
_LIT(KCntServerExe,"CNTSRV.EXE");
/** Name used to connect a session to the Contacts server. */
_LIT(KCntServerName,"CNTSRV");


/**
Open a Contacts server session.
*/
void RCntModel::ConnectL()
	{
	// Assume the server is already running and attempt to create a session
	// with a maximum of KAsyncMessageSlots message slots.
	TInt err = CreateSession(KCntServerName,Version(),KAsyncMessageSlots);
	
	if(err == KErrNotFound) // Server not running?
		{
		// Use the RProcess API to start the server.
		RProcess server;
		User::LeaveIfError(server.Create(KCntServerExe,KNullDesC));
		
		//Enforce server to be at system default priority EPriorityForeground
		//Contact server used to set as EPriorityHigh and this caused client could
		//not get control or responsive until sorting by the idle sorter in remote 
		//view was done.
		server.SetPriority(EPriorityForeground);
		
		// Synchronise with the server.
		TRequestStatus reqStatus;
		server.Rendezvous(reqStatus);
		server.Resume();
		
		// Server will call the reciprocal static synchronisation call.
		User::WaitForRequest(reqStatus);
		server.Close();
		User::LeaveIfError(reqStatus.Int());
		
		// Create the server session.
		User::LeaveIfError(CreateSession(KCntServerName,Version(),KAsyncMessageSlots));
		}
	else
		{
		User::LeaveIfError(err);
		}

	// Create object packer/unpacker if it doesn't already exist.
	if (iPackager == NULL)	
		{
		iPackager = CCntPackager::NewL();
		}
	
	// Each session (client) will be given a unique ID known as the Connection
	// ID.  This ID forms part of the database event notification message.  This
	// ID is created during the connection to the server.
	ConnectionId();
	}

	
/**
Close session.
*/
void RCntModel::Close()
	{
	delete iPackager;
	iPackager = NULL;
	delete iDbNotifyMonitor;
	iDbNotifyMonitor = NULL;
	RHandleBase::Close();
	}


/**
Open a named contact database.

Opens the default contacts database if the default argument is used.  The given
descriptor must not contain more than KCntMaxFilePath characters.

@param aCntFile Filename (in the form drive:database).  Defaults to
KCntDefaultDrive.

@return KErrNone if success, KErrArgument if the given descriptor contains more than the 
        maximum length of 190 characters, otherwise one of the System error codes.

@capability ReadUserData
*/
TInt RCntModel::OpenDatabase(const TDesC& aCntFile) const
    {
    TInt err = SetFileName(aCntFile);
    if (err == KErrNone)
        {
        TIpcArgs args;
        args.Set(0,&aCntFile);
        err = SendReceive(ECntOpenDataBase, args);
        }

    return err;
    }


/**
Open a named contact database asynchronously.

Opens the default contacts database if the default argument is used.  The given
descriptor must not contain more than KCntMaxFilePath characters.

@param aStatus Asynchronous request object.  Request is completed when database
has been opened.
@param aCntFile Contacts database filename (in the form drive:database).
Defaults to KCntDefaultDrive.

@capability ReadUserData
*/
void RCntModel::OpenDatabaseAsyncL(TRequestStatus& aStatus, const TDesC& aCntFile)
    {
    User::LeaveIfError(SetFileName(aCntFile));

    TIpcArgs args;
    args.Set(0,&aCntFile);
    SendReceive(ECntOpenDataBase, args, aStatus);
    }


/**
Cancel last asynchronous database open request.

@capability None
*/
void RCntModel::CancelAsyncOpen() 
	{
	SendReceive(ECntCancelAsyncOpenDatabase);		
	}


/**
Handle a premature termination of the contact server process by re-connecting
the session and re-opening the database.

@capability ReadUserData
*/
void RCntModel::HandlePrematureServerTerminationL()
	{
	if (iNoOfSvrStartAttempts > KMaxTimesToRestartServer)
		{
		User::Leave(KErrServerTerminated);
		}
	ConnectL();
	User::LeaveIfError(OpenDatabase(iFileName));
	++iNoOfSvrStartAttempts;
	}

	
/**
Set the database filename. Used to re-open the database if the server is
terminated prematurely.

@param aCntFile Contacts database filename (in the form drive:database).
@return KErrNone if success, KErrArgument if the given descriptor contains more than the 
        maximum length of 190 characters.         
*/
TInt RCntModel::SetFileName(const TDesC& aCntFile) const
    {
    if(aCntFile.Length() > KCntMaxFilePath)
        {
        return KErrArgument;
        }

    iFileName = aCntFile;
    return KErrNone;
    }


/**
Close currently open database.

@capability None
*/
void RCntModel::CloseDatabase() const
	{
	(void)SendReceive(ECntCloseDataBase);
	}


/**
Get the machine ID. 

Note: This function can leave.

@return Machine ID.

@leave KErrNone The send operation is successful
@leave KErrServerTerminated The server no longer present
@leave KErrServerBusy There are no message slots available
@leave KErrNoMemory There is insufficient memory available


@capability None
*/
TInt64 RCntModel::MachineId() const
	{
	TIpcArgs args;
	TPckgBuf<TInt64> machineID;
	args.Set(0, &machineID);
	User::LeaveIfError(SendReceive(ECntMachineID, args)); // this can leave
	return machineID();	
	}


/**
Set the machine ID (debug only).

@param aMachineUniqueId New machine ID.

@capability None
*/
void RCntModel::OverrideMachineUniqueId(TInt64 aMachineUniqueId)
	{
	TIpcArgs args;
	TPckgBuf<TInt64> machineID(aMachineUniqueId);
	args.Set(0, &machineID);
	(void)SendReceive(ECntOverrideMachineID, args);	
	}


/**
Replace a named contact database with an empty one, replacing any database with
the same name.  Replaces the default contacts database if the default argument
is used.  The given descriptor must not contain more than KCntMaxFilePath
characters.

@param aCntFile Contacts database filename (in the form drive:database).
Defaults to KCntDefaultDrive.

@return KErrNone if success, KErrArgument if the given descriptor contains more than the 
        maximum length of 190 characters, KErrInUse if the database is currently open, 
        otherwise one of the System error codes.

@capability WriteUserData
*/
TInt RCntModel::ReplaceDatabase(const TDesC& aCntFile) const
    {
    TInt err = SetFileName(aCntFile);
    if (err == KErrNone)
        {
        TIpcArgs args;
        args.Set(0,&aCntFile);

        err = SendReceive(ECntReplaceDatabase, args);
        }

    return err;
    }


/**
Returns an array of contact item IDs for all the contact items which may contain
the specified telephone number in a telephone, fax or SMS type field.

@see CContactDatabase::MatchPhoneNumberL() for more details on the match.

@param aNumber Phone number string
@param aMatchLengthFromRight Number of digits from the right of the phone number
to use.  Up to 15 digits can be specified and it is recommended that at least 7
match digits are specified.

@return CContactIdArray of candidate matches.

@capability ReadUserData
*/
CContactIdArray* RCntModel::MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight)
	{
	TIpcArgs args;
	args.Set(0, &iPackager->GetReceivingBufferL());
	args.Set(1, MLplCollection::EMatchPhoneNos);
	args.Set(2, &aNumber);
	args.Set(3, aMatchLengthFromRight);
	
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(ECntGetCollection, args));
	
	if (newBuffSize > 0)
		{
		// If the buffer is not large enough resize the packager's internal
		// buffer and make the call again.
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntGetCollection, args));		
		}
		
	CContactIdArray* idArray = iPackager->UnpackCntIdArrayLC();	
	CleanupStack::Pop(idArray);

	return idArray;
	}
	
	

/**
Create an empty named contact database.

Creates the default contacts database if the default argument is used.  The
given descriptor must not contain more than KCntMaxFilePath characters.

@param aCntFile Contacts database filename (in the form drive:database).
Defaults to KCntDefaultDrive.

@return KErrNone if success, KErrArgument if the given descriptor contains more than the 
        maximum length of 190 characters, KErrAlreadyExists if the database already 
        exists otherwise one of the System error codes.

@capability WriteUserData
*/
TInt RCntModel::CreateDatabase(const TDesC& aCntFile) const
    {
    TInt err = SetFileName(aCntFile);
    if (err == KErrNone)
        {
        TIpcArgs args;
        args.Set(0,&aCntFile);
        err = SendReceive(ECntCreateDatabase, args);
        }
    return err;
    }


/**
Create the system template.

@return KErrNone if success otherwise one of the System error codes.
	
@capability WriteUserData
*/
TInt RCntModel::ReCreateTemplate() const
	{
	return SendReceive(ECntReCreateTemplate);
	}


/**
Get the database file UID.

@return Pointer to file UID.

@capability None
*/
TPtrC RCntModel::FileUidL() const
	{
	TIpcArgs args;
	args.Set(0, &iFileUid);
	User::LeaveIfError (SendReceive(ECntFileUniqueId, args));
    return iFileUid;
	}


/**
Determine if the database is ready.  Ready in this context means that the
database is open and readable/writable (i.e. the state machine is in
CStateWritable).

@capability None

@return ETrue if the database is ready, EFalse if the database is not ready.
*/
TBool RCntModel::DatabaseReadyL() const
    {
    TBool retVal = EFalse;

    // Although the ECntGetDatabaseReady msg. is completed with TBool values,
    // during message processing, it is possible that ServiceError() occurs,
    // causing the message to be completed with negative error codes.
    // LeaveIfError() would protect against such cases.
    User::LeaveIfError(retVal = SendReceive(ECntGetDatabaseReady));

    return retVal;
    }


/**
Tests whether a contact item's hint bit field matches a filter.

@param aBitWiseFilter The filter to compare the item against.  This is a
combination of TContactViewFilter values.
@param aContactId The ID of the item in the database.

@return ETrue if the item is of the correct type for inclusion in the database 
and its hint bit field matches the specified filter, EFalse if either of these
conditions are not met.

@capability None
*/
TBool RCntModel::ContactMatchesHintFieldL(TInt aBitWiseFilter, TContactItemId aContactId)
    {
    TIpcArgs args;
    args.Set(0, aBitWiseFilter);
    args.Set(1, aContactId);
    TBool retVal = EFalse;

    // Although the ECntMatchesHintField msg. is completed with TBool values,
    // during message processing, it is possible that ServiceError() occurs,
    // causing the message to be completed with negative error codes.
    // LeaveIfError() would protect against such cases.
    User::LeaveIfError (retVal = SendReceive(ECntMatchesHintField, args));

    return retVal;
    }


/**
Delete a named contact database.

Deletes the default contacts database if the default argument is used.  The
given descriptor must not contain more than KCntMaxFilePath characters.

@param aCntFile Contacts database filename (in the form drive:database).
Defaults to KCntDefaultDrive.

@return KErrNone if success, KErrArgument if the given descriptor contains more 
        than the maximum length of 190 characters, KErrInUse if the database is 
        in use, KErrNotFound if the database does not exist otherwise one of the 
        System error codes.

@capability WriteUserData
*/
TInt RCntModel::DeleteDatabase(const TDesC& aCntFile) const
	{
	if(aCntFile.Length() > KCntMaxFilePath)
		{
		return KErrArgument;
		}
	TIpcArgs args;
	args.Set(0,&aCntFile);
	return SendReceive(ECntDeleteDatabase,args);
	}


/**
Get the name of the default contact database file.

The given descriptor must have room for minimum of KCntMaxFilePath characters.

@param aCntFile Contains the default contact database name on return.

@return KErrNone if success, KErrArgument if the given descriptor contains more 
        than the maximum length of 190 characters, otherwise one of the System 
        error codes.

@capability None
*/
TInt RCntModel::DefaultDatabase(TDes& aCntFile) const
	{
	TIpcArgs args;
	args.Set(0,&aCntFile);
	return SendReceive(ECntGetDefaultDatabaseName,args);
	}


/**
Retrieve the current contact database drive.

@param aDriveUnit Contains the current contact database drive unit on return.

@return KErrNone if success otherwise one of the System error codes.

@capability None
*/
TInt RCntModel::DatabaseDrive(TDriveUnit& aDriveUnit) const
	{
	TInt ret = SendReceive(ECntDatabaseDrive);
	if (ret >= KErrNone)
		{
		aDriveUnit = ret;	
		}
	return ret;
	}


/**
Sets the contact database drive and optionally moves the default contact
database from its current location to the new drive.  This function guarantees
an all or nothing operation.  If the database is not successfully moved, the
drive setting is not updated to reflect the change.

@param aDriveUnit The drive to which to move the contact database.
@param aCopy ETrue moves the existing file to the specified drive.  Deletion of
the source file will fail if it is in use.  EFalse does not move the file.  The
default argument value is ETrue.

@return KErrNone if success, KErrInUse if destination file for copy is open,
KErrNotFound found if source contact database (default) cannot be found,
KErrAlreadyExists if the destination contact database file exists.

@capability WriteUserData
*/
void RCntModel::SetDatabaseDriveL(TDriveUnit aDriveUnit, TBool aCopy)
	{
	TIpcArgs args;
	args.Set(0,aDriveUnit);
	args.Set(1,aCopy);
	User::LeaveIfError(SendReceive(ECntSetDatabaseDrive,args));
	}

/** 
Gets the size of the database file in bytes.

@return The size of the contact database in bytes.

@capability None
*/
TInt RCntModel::FileSize() const
	{
	return SendReceive(ECntFilesSize);	
	}

/**
Determine if the named contact database file exists.

If the aCntFile argument is the default, determines if default contact database
file exists.

@param aCntFile Contacts database filename (in the form drive:database).
@leave KErrArgument if the given descriptor contains more than the maximum length 
       of 190 characters, otherwise one of the System error codes.
@return ETrue if exists, EFalse does not exist. 
@capability None
*/
TBool RCntModel::DatabaseExistsL(const TDesC& aCntFile) const
    {
    if(aCntFile.Length() > KCntMaxFilePath)
        {
        User::Leave(KErrArgument);
        }

    TIpcArgs args;
    args.Set(0,&aCntFile);
    TBool ret = EFalse;

    // Although ECntDatabaseExists is normally completed with TBool values,
    // ServiceError() can occur which leads to a negative system error codes
    // being returned.  User::LeaveIfError will allow that to be properly handled.
    User::LeaveIfError(ret = SendReceive(ECntDatabaseExists,args));

    return ret;
    }


/**
List the contact database files for any given drive unit.  Lists all databases
on all drives if aDriveUnit is set to NULL.

@param aDriveUnit The drive unit to search.  Searches all drives if default
argument is used.

@return CDesCArray containing the database list.  Always returns list even if
empty.

@leave KErrNoMemory if not enough free memory, KErrNotReady if drive does not,
otherwise one of the System error codes.

@capability ReadUserData
*/
CDesCArray* RCntModel::ListDatabasesL(TDriveUnit* aDriveUnit) const
	{
	// Convert the drive unit to an integer for IPC.  The TDriveUnit constructor
	// does not allow values outside 0 - 25.
	TInt driveNumber;
	(aDriveUnit == NULL) ? (driveNumber = ECntAllDrives) : (driveNumber = *aDriveUnit);

	// Provide a CBufFlat for receiving the list of databases from the server.
	// Format is a (proprietary) serialized CDesCArray.
	const TInt KInitialBufferSize = 200;
	CBufFlat* listBuffer = CBufFlat::NewL(KInitialBufferSize);
	CleanupStack::PushL(listBuffer);
	listBuffer->ExpandL(0,KInitialBufferSize);

	// IPC argument list is:
	// 0 (Return param)	- Address of our receive buffer
	// 1 (Param)		- The drive number 0 - 25 or ECntAllDrives
	// 2 (Return param)	- The size of buffer required for the transfer
	TIpcArgs args;
	TPtr8 ptr(listBuffer->Ptr(0));
	args.Set(0,&ptr);
	args.Set(1,driveNumber);
	TPckgBuf<TInt> size;
	args.Set(2,&size);
	User::LeaveIfError(SendReceive(ECntListDatabases,args));

	// Check if our buffer was large enough by reading returned size argument.
	// The server will not have written any data if our povided buffer was not
	// big enough.
	if(size() > KInitialBufferSize)
		{
		// Not big enough but now we know how big the buffer needs to be.
		// Just allocate buffer again, reset the IPC args and call again.
		CleanupStack::PopAndDestroy(listBuffer);
		listBuffer = CBufFlat::NewL(size());
		CleanupStack::PushL(listBuffer);
		listBuffer->ExpandL(0,size());
		ptr.Set(listBuffer->Ptr(0));
		args.Set(0,&ptr);
		args.Set(1,driveNumber);
		args.Set(2,&size);
		User::LeaveIfError(SendReceive(ECntListDatabases,args));
		}

	// Wrap the buffer in a read stream class to internalize.
	RBufReadStream readStream;
	readStream.Open(*listBuffer);
	CleanupClosePushL(readStream);

	// Number of array elements.
	const TInt count = readStream.ReadUint32L();
	// Use count if it is not zero.
	CDesCArray* list = new(ELeave) CDesCArrayFlat(count ? count : 1);
	CleanupStack::PushL(list);
	for (TInt i=0; i<count; ++i)
		{
		TFileName path;
		TInt length=readStream.ReadInt8L();
		readStream.ReadL(path,length);
		// Add to the list.
		list->AppendL(path);
		}

	CleanupStack::Pop(list);
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(listBuffer);
	
	return list;
	}


/**
Add a new contact to the database.

@param aContact The contact to be added to the database.

@capability WriteUserData
*/
TContactItemId RCntModel::CreateContactL(CContactItem& aContact) const
	{
	// Pack the contact into the first IPC argument.
	TIpcArgs args;
	TPtr8 ptr(iPackager->PackL(aContact));
	args.Set(0,&ptr);

	TInt cntId = 0;
	User::LeaveIfError(cntId = SendReceive(ECntItemCreate, args));

	return cntId;
	}


/**
Delete a contact from the database.

@param aContact The contact to be deleted.
@param aCntEventType The event type to pass on to the observers.
@param aDecAccessCount If ETrue access count should be decremented prior to the
deletion.

@capability WriteUserData
*/
void RCntModel::DeleteContactL(TContactItemId aCntId, TCntSendEventAction aCntEventType, TBool aDecAccessCount) const
	{
	TIpcArgs args(aCntId, aCntEventType, aDecAccessCount);
	User::LeaveIfError(SendReceive(ECntItemDelete, args));
	}

/** 
Open the database tables.

@capability ReadUserData
*/
void RCntModel::OpenTablesL()
	{
	User::LeaveIfError(SendReceive(ECntReOpenDbTables));
	}


/**
Close the database tables.

@capability None
*/
void RCntModel::CloseTablesL()
	{
	User::LeaveIfError(SendReceive(ECntCloseDbTables));
	}


/**
Update and unlock an existing contact in the database.

@param aContact The contact to be updated and unlocked
@param aSendChangedEvent If ETrue the changed event should be sent after the
update/unlock.

@capability WriteUserData
*/
void RCntModel::CommitContactL(const CContactItem& aContact, TBool aSendChangedEvent) const
	{
    //check what contacts were added/removed to a group
    if (aContact.Type() == KUidContactGroup)
        {
        const CContactGroup& group = static_cast<const CContactGroup&>(aContact);
        if (group.iInitialContactIds != NULL)
            {
            group.iAddedContactIds = CContactIdArray::NewL();
            group.iRemovedContactIds = CContactIdArray::NewL();
            for (int i = 0; i < group.iItems->Count(); i++)
                {
                if (group.iInitialContactIds->Find(group.iItems->operator[](i)) == KErrNotFound)
                    {
                    group.iAddedContactIds->AddL(group.iItems->operator[](i));
                    }
                }
            for (int j = 0; j < group.iInitialContactIds->Count(); j++)
                {
                if (group.iItems->Find(group.iInitialContactIds->operator[](j)) == KErrNotFound)
                    {
                    group.iRemovedContactIds->AddL(group.iInitialContactIds->operator[](j));
                    }
                }           
            }
        }	
	
	// Pack the contact into the first IPC argument.
	TIpcArgs args; 
	TPtr8 ptr(iPackager->PackL((const_cast<CContactItem&>(aContact))));
	args.Set(0,&ptr);
	
	args.Set(1,aSendChangedEvent);

	User::LeaveIfError(SendReceive(ECntItemCommit, args));
	
    //clear list of contacts added/removed to a group
    if (aContact.Type() == KUidContactGroup)
        {
        const CContactGroup& group = static_cast<const CContactGroup&>(aContact);
        if (group.iAddedContactIds != NULL)
            {
            delete group.iAddedContactIds;
            group.iAddedContactIds = NULL;
            }
        if (group.iRemovedContactIds != NULL)
            {
            delete group.iRemovedContactIds;
            group.iRemovedContactIds = NULL;
            }
        if (group.iInitialContactIds != NULL)
            {
            delete group.iInitialContactIds;
            group.iInitialContactIds = NULL;
            }
        }	
	}


/**
Unpacks a contact item contained within the Packager's internal buffer and
creates a contact item object.  Within the server, the contact item has been
written to the Packager's internal buffer by the RMessage2.

If the buffer size was returned then the buffer was not large enough to hold a
serialised contact item.  The buffer must be expanded to the size returned by
the server (ie aBufferSize) and the read operation performed again. 

@see CCntPackager::SetBufferFromMessageL()
	
@param aBufferSize The new buffer size.
@param aArgs The argument buffer where the serialised contact item is written by
the server.

@return CContactItem unpacked from Packager's internal buffer.

@capability ReadUserData
*/
CContactItem* RCntModel::UnPackContactLC(TInt aBufferSize, TIpcArgs& aArgs) const
	{
	if(aBufferSize > 0) // Packager's internal buffer is not large enough.
		{
		// Set new extended receiving buffer.
		aArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));
		// Perform read operation again.
		User::LeaveIfError(SendReceive(ECntItemRead, aArgs));
		}
	return iPackager->UnpackCntItemLC();
	}


/**
Read an existing contact in the database.

@param aCntItemVDef The view definition to use.
@param aCntId Contact ID to read.

@return CContactItem object with contact ID aCntId.
*/
CContactItem* RCntModel::ReadContactL(const CContactItemViewDef* aCntItemVDef, TContactItemId aCntId) const
	{
	TIpcArgs args;

	if (aCntItemVDef) 
		{
		iPackager->PackL(*(const_cast<CContactItemViewDef*>(aCntItemVDef)));
		// Indicates that a user ContactItemViewDef is present in first IPC
		// argument.
		args.Set(3,TRUE);
		}
	else
		{
		iPackager->Clear();
		// Indicates that no user ContactItemViewDef is present in first IPC
		// argument.  The default view definition will be used instead.
		args.Set(3,FALSE);
		}

	args.Set(0, &iPackager->GetTransmittingBuffer());
	args.Set(1, &iPackager->GetReceivingBufferL());
	args.Set(2, aCntId);

	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(ECntItemRead, args));
	CContactItem* cntItem = UnPackContactLC(newBuffSize, args);
	CleanupStack::Pop(cntItem);

	return cntItem;
	}
	
	
/**
Open and lock an existing contact in the database.  The contact item lock is
pushed onto the cleanup stack.

@param aCntItemVDef The view definition to use.
@param aCntId Contact ID to read.

@return Opened/locked CContactItem object.
*/
CContactItem* RCntModel::OpenContactLX(const CContactItemViewDef* aCntItemVDef, TContactItemId aCntId) const
	{
	TIpcArgs args;
	if (aCntItemVDef) 
		{
		iPackager->PackL(*(const_cast<CContactItemViewDef*>(aCntItemVDef)));
		// Indicates that a user ContactItemViewDef is present in first IPC
		// argument.
		args.Set(3,TRUE);
		}
	else
		{
		iPackager->Clear();
		// Indicates that no user ContactItemViewDef is present in first IPC
		// argument.  The default view definition will be used instead.
		args.Set(3,FALSE);
		}

	args.Set(0, &iPackager->GetTransmittingBuffer());
	args.Set(1, &iPackager->GetReceivingBufferL());
	args.Set(2, aCntId);

	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(ECntItemOpen, args));

	// Push the contact item lock onto the cleanup stack.  If UnPackContactLC()
	// (or any other RCntModel method leaves after this point) then the last
	// contact to be locked will be unlocked (aka closed).  The method returns
	// with the lock remaining on the cleanup stack.
	PushUnlockL(); 

	CContactItem* cntItem = UnPackContactLC(newBuffSize, args);
	
    //save a list of contacts belonging to a group
    if (cntItem->Type() == KUidContactGroup)
        {
        CContactGroup* group = static_cast<CContactGroup*>(cntItem);
        delete group->iInitialContactIds;
        group->iInitialContactIds = NULL;
        group->iInitialContactIds = CContactIdArray::NewL(group->ItemsContained());
        }	
	
	CleanupStack::Pop(cntItem);
	
	return cntItem;		
	}


/**
Close/unLock an existing contact in the database.

@param aCntId The contact ID of the contact to close.

@return ETrue if the contact was opened before and closed after the call,
EFalse if there was no need to close the contact.
*/
TBool RCntModel::CloseContact(TContactItemId aCntId)
	{
	TInt res = SendReceive(ECntItemClose, TIpcArgs(aCntId));
	if (res == KErrNone)
		{
		return ETrue;
		}
	return EFalse;
	}


/**
Start a database transaction.
*/
TInt RCntModel::BeginDbTransaction() const
	{
	return SendReceive(EBeginDbTransaction);
	}

	
/**
Commit a database transaction.
*/
TInt RCntModel::CommitDbTransaction() const
	{
	return SendReceive(EEndDbTransaction);
	}

	
/**
Rollback a database transaction.
*/
TInt RCntModel::RollbackDbTransaction() const
	{
	return SendReceive(ERollbackDbTransaction);
	}


/**
Set the operation timeout value.  This value is the length of time that a
deferred request will remain in the request queue in the State Machine before
being completed with an error code.

@param aMicroSeconds The operation timeout in microseconds.  This timeout will
only be applied to requests sent by clients of this database after this point in
time. 
*/
void RCntModel::SetOperationTimeOutL(const TInt aMicroSeconds) const
	{
	TIpcArgs args;
	args.Set(0, aMicroSeconds);
	User::LeaveIfError(SendReceive(ECntOpsTimeOut, args));		
	}

	
/**
Change the existing contact view definition on the server.

@param aView The new contact view defintion.
*/
void RCntModel::SetViewDefinitionL(const CContactViewDef& aView)
	{
	TPtr8 ptr(iPackager->PackL(aView));
	User::LeaveIfError(SendReceive(ECntChangeViewDef, TIpcArgs(&ptr)));	
	}


/**
Add a database event observer.

@param aObserver Observer callback inteface used to send database event.
*/
void RCntModel::AddObserverL(MContactDbObserver& aObserver)
	{
	// If this is the first observer to be added then required to create the
	// database event notification monitor (lazy initialisation).
	if (!iDbNotifyMonitor)
		{
		iDbNotifyMonitor = CCntDbNotifyMonitor::NewL(*this);
		}
	iDbNotifyMonitor->AddObserverL(aObserver);
	}

	
/**
Remove a database event observer.

@param aObserver Observer callback inteface to be removed.
*/
void RCntModel::RemoveObserver(const MContactDbObserver& aObserver)
	{
	if (iDbNotifyMonitor)
		{
		iDbNotifyMonitor->RemoveObserver(aObserver);
		// If this is the last observer to be removed then dispose of the
		// database event notification monitor (no longer required).
		if (iDbNotifyMonitor->NumberOfListeners() == 0)
			{
			delete iDbNotifyMonitor;
			iDbNotifyMonitor = NULL;
			}
		}
	}

/**
Add a database event observer.

@param aObserver Observer callback inteface used to send database event.
*/
void RCntModel::AddObserverV2L(MContactDbObserverV2& aObserver)
    {
    // If this is the first observer to be added then required to create the
    // database event notification monitor (lazy initialisation).
    if (!iDbNotifyMonitor)
        {
        iDbNotifyMonitor = CCntDbNotifyMonitor::NewL(*this);
        }
    iDbNotifyMonitor->AddObserverV2L(aObserver);
    }

    
/**
Remove a database event observer.

@param aObserver Observer callback inteface to be removed.
*/
void RCntModel::RemoveObserverV2(const MContactDbObserverV2& aObserver)
    {
    if (iDbNotifyMonitor)
        {
        iDbNotifyMonitor->RemoveObserverV2(aObserver);
        // If this is the last observer to be removed then dispose of the
        // database event notification monitor (no longer required).
        if (iDbNotifyMonitor->NumberOfListeners() == 0)
            {
            delete iDbNotifyMonitor;
            iDbNotifyMonitor = NULL;
            }
        }
    }

/**
Request a database event from the server.

@param aStatus Completed when database event is available.
@param aEvent When aStatus is completed contains the database event.
*/
void RCntModel::StartNotificationTransfer(TRequestStatus& aStatus, TDes8& aEvent)
	{	
	SendReceive(ECntRequestEvent, TIpcArgs(&aEvent), aStatus);	
	}


/**
End (cancel) request for database event from server.
*/
void RCntModel::EndNotificationTransfer()
	{	
	SendReceive(ECntCancelEventRequest);	
	}
	
/** 
Sets the ID of the current item and persists it in the database.  The current 
item is provided for use by clients who want to identify one contact item in the
database as the currently selected item.

@param aContactId The ID of the current item. 

@capability WriteUserData
*/
void RCntModel::SetCurrentItem(TContactItemId aContactId) const
	{
	TIpcArgs args(aContactId);
	(void)SendReceive(ECntSetCurrentItem, args);	
	}

	
/** 
Gets the ID of the current item, as set by SetCurrentItem().  The current item 
ID is initialised to KNullContactId when the database is opened.

@return The ID of the current item. 

@capability None
*/
TContactItemId RCntModel::CurrentItem() const
	{		
	 TContactItemId id = SendReceive(ECntGetCurrentItem);
	 if (id < 0)
	 	{
		return KNullContactId;
		}
	 else
	 	{
		return id;
		}
	}


/** 
When there are multiple contact databases on a device this method can be used to
enquire which database is the current one.

The current database functions are provided as part of current item
functionality.  In order to pass a current item from one contacts model client
to another, the receiving client needs to be using the same database.

The current database is a path and filename, set using SetCurrentDatabase(),
which is persisted by the contacts server.

@deprecated

@param aDatabase The path and filename of the current database.  KNullDesC 
if no current database has been set.

@return KErrNone if success otherwise one of the System error codes. 

@capability None
*/
TInt RCntModel::GetCurrentDatabase(TDes& aDatabase) const
	{
	TIpcArgs args(&aDatabase);
	return SendReceive(ECntGetCurrentDb, args);
	}


/** 
When there are multiple contact databases on a device this method can be used to
set a database as the current one.

Note: this function simply updates a file name which is stored by the Contacts
server and its use is optional.  It is provided as part of current item
functionality. 

In order to pass a current item from one contacts model client to another, 
the receiving client needs to be using the same database.

@deprecated

@param aDatabase The path and filename of the current database.

@return KErrNone if success otherwise one of the System error codes. 

@capability WriteUserData
*/
TInt RCntModel::SetCurrentDatabase(const TDesC& aDatabase) const
	{
	TIpcArgs args(&aDatabase);
	return SendReceive(ECntSetCurrentDb, args);
	}


/** 
Returns the ID of the contact item whose telephone number field is mapped to 
the speed dial position specified.

This function is provided so that information may be displayed about a contact
item whose telephone number is being dialled using speed dialling.

The function also retrieves the telephone number stored in the field.

@param aSpeedDialPosition The speed dial position.  This is an integer in the 
range 1 to 9 inclusive.  If outside this range, the function leaves with
KErrArgument.
@param aPhoneNumber On return contains the telephone number which is mapped to
the speed dial position specified.  Returns KNullDesC if the speed dial position
requested has not been set.

@return The ID of the contact item to which the speed dial is mapped. 

@capability ReadUserData
*/
TContactItemId RCntModel::GetSpeedDialFieldL(TInt aSpeedDialPosition, TDes& aPhoneNumber)
	{
	__ASSERT_ALWAYS(aSpeedDialPosition >= KCntMinSpeedDialIndex && aSpeedDialPosition <= KCntMaxSpeedDialIndex , User::Leave(KErrArgument));	
	TPckgBuf<TContactItemId> contact(KNullContactId);
	TIpcArgs args(aSpeedDialPosition,&aPhoneNumber);
	return (TContactItemId)SendReceive(ECntGetSpeedDialContactIdAndPhoneNumber, args);
	}

	
/**
Sets a field containing a telephone number as a speed dial field.  The field 
is identified by aFieldIndex within the contact item aItem.  It is assigned a 
speed dial position between 1 and 9 inclusive.

The field's speed dial and user added attributes are set and the appropriate 
UID (KUidSpeedDialXxx) is added to the field's content type.  The changes are 
committed to the database.

Notes:

- If an item's telephone number field has already been assigned to position 
aSpeedDialPosition, that item is updated so that the speed dial attribute 
is removed from its field and the speed dial field type UID is removed from 
the field's content type, before the new speed dial field is set.

- The speed dial attribute can be tested for using the
CContactItemField::IsSpeedDial() method.

- The contact item passed to this function (aItem) must have been obtained using 
one of the variants of CContactDatabase::OpenContactL().  This is because it 
is modified and committed to the database by this function - no further 
commits are necessary.

@param aItem The contact item containing the field to set as a speed dial 
field.
@param aFieldIndex Index of a field in aItem's field set to set as a speed dial
field.
@param aSpeedDialPosition The speed dial position.  This is an integer in the 
range 1 to 9 inclusive.  If outside this range, the function leaves with
KErrArgument. 

@leave KErrDiskFull The disk does not have enough free space to perform the
operation.

@capability ReadUserData
@capability WriteUserData
*/
void RCntModel::SetFieldAsSpeedDialL(TContactItemId aContactId, TInt aFieldIndex, TInt aSpeedDialPosition)
	{
	__ASSERT_ALWAYS(aSpeedDialPosition >= KCntMinSpeedDialIndex && aSpeedDialPosition <= KCntMaxSpeedDialIndex , User::Leave(KErrArgument));
	TIpcArgs args(aSpeedDialPosition, aContactId, aFieldIndex);
	User::LeaveIfError(SendReceive(ECntSetSpeedDialIdForPosition, args));
	}


/** 
Removes the mapping between a contact item field and a speed dial position.

Removes the KUidSpeedDialXxx UID from the field's content type, removes the 
field's speed dial attribute and commits the changes to the item.

@param aContactId The ID of the contact item containing the speed dial field.
@param aSpeedDialPosition The speed dial position.  This is an integer in the 
range 1 to 9 inclusive.  If outside this range, the function leaves with
KErrArgument. 

@leave KErrDiskFull The disk does not have enough free space to perform the operation.

@capability ReadUserData
@capability WriteUserData
*/
void RCntModel::RemoveSpeedDialFieldL(TContactItemId aContactId, TInt aSpeedDialPosition)
	{
	__ASSERT_ALWAYS(aSpeedDialPosition >= KCntMinSpeedDialIndex && aSpeedDialPosition <= KCntMaxSpeedDialIndex , User::Leave(KErrArgument));
	// Tell the server that this speed dial slot is now free.  Third argument
	// (i.e. field index) is not used.
	TIpcArgs args(aSpeedDialPosition, aContactId, -1);
	User::LeaveIfError(SendReceive(ECntSetSpeedDialIdForPosition, args));		
	}


/**
Returns the ID of the template that should be used with CContactICCEntry items.
If aPhonebookUid is set, the ID of the template belonging to the phonebook with 
TUid aPhonebookUid is returned.

@param aPhonebookUid The phonebook ID with default KNullUid.

@return A template ID.
*/
TContactItemId RCntModel::ICCTemplateIdL(TUid aPhonebookUid)
	{
	TIpcArgs args(aPhonebookUid.iUid);
	return SendReceive(ECntICCTemplateId, args);
	}


/**
Returns the ID of the contacts model group which represents the ADN phonebook.

@return ADN phonebook group ID.
*/ 
TContactItemId RCntModel::PhonebookGroupIdL()
	{
	return SendReceive(ECntPhonebookGroupId);
	}


/** 
Sets an existing contact item to be the database's current own card.

@param aContact The contact item to set as the database's current own card. 
It must already exist in the database.  It cannot be a group or a template.

@leave KErrNotFound aContact does not exist in the database. 
@leave KErrDiskFull The disk does not have enough free space to perform the
operation.

@capability None
*/
void RCntModel::SetOwnCardL(const CContactItem& aContact)
	{
	TIpcArgs args; 
	TPtr8 ptr(iPackager->PackL(aContact));
	args.Set(0,&ptr);
	User::LeaveIfError(SendReceive(ECntSetOwnCard, args));	
	}


/**
Returns the ID of the database's current own card. 

Having obtained this ID, the client may then open the own card in the same 
way as an ordinary contact card (using ReadContactL() or OpenContactL()).

@return The ID of the database's current own card.  KNullContactId if the own 
card has been deleted or has not yet been set.
*/
TContactItemId RCntModel::OwnCard() const
	{
	return static_cast<TContactItemId>(SendReceive(ECntGetOwnCard));
	}

	
/**
Returns the connection ID of the current client session.

@return Connection ID of the current client session.
*/
TInt RCntModel::ConnectionId() const
	{
	// If asking for the connection ID the first time, then we ask the server
	// about the session id (lazy initialisation).
	if (!iConnectionId)
		{
		iConnectionId = SendReceive(ECntConnectionId); 	
		}
	return iConnectionId;		
	}


/** 
Debug only.

@capability None
*/
void RCntModel::SetHeapFailure(RHeap::TAllocFail aType, TInt aRate)
	{	
	TIpcArgs args(aType,aRate);
	SendReceive(ECntSetHeapFailure,args);
	}

	
/** 
Debug only.

@return The heap size usage of the server in debug mode, 0 in release mode.

@capability None
*/
TInt RCntModel::ResourceCount()
	{
	return(SendReceive(ECntResourceCount));
	}	

	
/**
Returns the ID of the database's preferred template, as set by the method
SetPrefTemplateL().  The preferred template is for clients who may have 
multiple templates but want to identify one as preferred.

@return The ID of the database's current preferred template.  KNullContactId if
not set.
*/
TContactItemId RCntModel::PrefTemplateId() const
	{
	return(SendReceive(ECntGetPrefTemplateId));	
	}


/** 
Sets the database's preferred template.

The preferred template's ID persists when the database is opened and closed. 
If the preferred template is subsequently deleted, the preferred template 
ID is set to KNullContactId.

@param aContact The contact card template to set as the database's preferred 
template.

@leave KErrNotSupported The item is not a template (i.e. is not of type
KUidContactCardTemplate).
@leave KErrDiskFull The disk does not have enough free space to perform the
operation.

@capability WriteUserData
*/
void RCntModel::SetPrefTemplateL(const TContactItemId aContactId)
	{
	TIpcArgs args(aContactId);
	User::LeaveIfError(SendReceive(ECntSetPrefTemplateId, args));
	}

	
/**
Get a list of template IDs from the server.

@return Array of template IDs.
*/
CContactIdArray* RCntModel::FetchTemplateListIdsL()
{
	TIpcArgs args;
	args.Set(0, &iPackager->GetReceivingBufferL());

	TInt newBuffSize;
	User::LeaveIfError(newBuffSize = SendReceive(ECntFetchTemplateIds, args));
	if (newBuffSize > 0)
		{
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntFetchTemplateIds, args));		
		}

	CContactIdArray* idArray = iPackager->UnpackCntIdArrayLC();	
	CleanupStack::Pop(idArray);

	return idArray;	
}


/**
Get a list of group IDs from the server.

@return Array of group IDs.
*/
CContactIdArray* RCntModel::FetchGroupListIdsL()
{
	TIpcArgs args;
	args.Set(0, &iPackager->GetReceivingBufferL());

	TInt newBuffSize;
	User::LeaveIfError(newBuffSize = SendReceive(ECntFetchGroupIdLists, args));
	if (newBuffSize > 0)
		{
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntFetchGroupIdLists, args));		
		}

	CContactIdArray* idArray = iPackager->UnpackCntIdArrayLC();	
	CleanupStack::Pop(idArray);

	return idArray;	
}

/**
Get a collection of contact IDs using the specified collection parameters.

@param aCollectionType Type of collection.
@param aTime Used if collection type is "changed since".
@param aGuid Used if collection type is "find GUID".

@return Contact ID array of those contacts which match the specified collection
parameters.
*/
CContactIdArray* RCntModel::CollectionL(TInt aCollectionType, TTime aTime,const TDesC& aGuid)
	{
	CContactIdArray* idArray = NULL;
	switch(aCollectionType)
	{
	case MLplCollection::EChangedSince :		
		{
		idArray = DoGetCollectionChangedSinceL(aTime);
		}
		break;
	case MLplCollection::EDeleted :
	case MLplCollection::EUnfiled :
		{
		idArray = DoGetCollectionL(aCollectionType);
		}
		break;
	case MLplCollection::EFindGuid :
		{
		idArray = DoGetCollectionGuidL(aGuid);
		}
		break;
	default:
		{
		User::Leave(KErrNotFound);
		}
		break;
	}	
	return idArray;
	}


/**
Get a collection of contact IDs which have changed since the given time.

@param aTime Changed since time.

@return Contact ID array of those contacts which have changed since the given
time.
*/
CContactIdArray* RCntModel::DoGetCollectionChangedSinceL(TTime aTime)
	{
	TIpcArgs args;
	args.Set(0, &iPackager->GetReceivingBufferL());
	args.Set(1,MLplCollection::EChangedSince);
	TPckgBuf<TTime> pckg(aTime);
	args.Set(2,&pckg);

	TInt newBuffSize;
	User::LeaveIfError(newBuffSize = SendReceive(ECntGetCollection, args));
	if (newBuffSize > 0)
		{
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntGetCollection, args));		
		}

	CContactIdArray* idArray = iPackager->UnpackCntIdArrayLC();	
	CleanupStack::Pop(idArray);

	return idArray;
	}

	
/**
Get a collection of contact IDs for the given collection type.

@param aCollectionType Collection type.

@return Contact ID array of those contacts which meet the given collection type
criteria.
*/
CContactIdArray* RCntModel::DoGetCollectionL(TInt aCollectionType)
	{
	TIpcArgs args;
	args.Set(0, &iPackager->GetReceivingBufferL());
	args.Set(1,aCollectionType);

	TInt newBuffSize;
	User::LeaveIfError(newBuffSize = SendReceive(ECntGetCollection, args));
	if (newBuffSize > 0)
		{
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntGetCollection, args));		
		}

	CContactIdArray* idArray = iPackager->UnpackCntIdArrayLC();	
	CleanupStack::Pop(idArray);

	return idArray;
	}


/**
Get a collection of contact IDs which have the given GUID.  Since GUIDs are
unique (i.e. no two contacts can have the same GUID) there will only ever be
one contact ID in this collection.

@param aGuid Contact GUID.

@return Contact ID array containing the contact with the given GUID.
*/
CContactIdArray* RCntModel::DoGetCollectionGuidL(const TDesC& aGuid)
	{
	TIpcArgs args;
	args.Set(0, &iPackager->GetReceivingBufferL());
	args.Set(1,MLplCollection::EFindGuid);
	args.Set(2,&aGuid);

	TInt newBuffSize;
	User::LeaveIfError(newBuffSize = SendReceive(ECntGetCollection, args));
	if (newBuffSize > 0)
		{
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntGetCollection, args));		
		}

	CContactIdArray* idArray = iPackager->UnpackCntIdArrayLC();	
	CleanupStack::Pop(idArray);

	return idArray;
	}


/**
Set the sort preferences in the server.

@param aSortOrder Sort order preferences.
*/
void RCntModel::SetSortPreferenceL(const CArrayFix<CContactDatabase::TSortPref>& aSortOrder)
	{
	TPtr8 ptr(iPackager->PackL(aSortOrder));
	TIpcArgs args;
	args.Set(0,&ptr);
	User::LeaveIfError(SendReceive(ECntSetSortPrefs,args));	
	}


/**
Get the sort preferences from the server.

@return Sort order preferences.
*/
CArrayFix<CContactDatabase::TSortPref>* RCntModel::GetSortPreferenceL() const
	{
	TIpcArgs args;
	args.Set(0, &iPackager->GetReceivingBufferL());

	TInt newBuffSize;
	User::LeaveIfError(newBuffSize = SendReceive(ECntGetSortPrefs, args));
	if (newBuffSize > 0)
		{
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntGetSortPrefs, args));		
		}

	CArrayFix<CContactDatabase::TSortPref>* prefs = iPackager->UnpackCArrayFixLC();	
	CleanupStack::Pop(prefs);

	return prefs;
	}


/**
Get the number of contacts in the database.

@return Number of contacts in the database if successful, otherwise it leaves.
*/
TInt RCntModel::ContactCountL() const
	{
    TInt numContacts = SendReceive( ECntDbContactCount );
    User::LeaveIfError( numContacts );
    return numContacts;
	}

/**
Seek to the given contact ID.

@param aContactId Contact ID to seek to.
@param aId On return the contact ID that the seek found.  This will be the
nearest contact ID if the given aContactID cannot be found.
@param aContactType On return the contact type of the contact that the seek
found.
@param aDeleted On return ETrue if the contact that the seek found is marked as
deleted, EFalse otherwise.

@return ETrue if the seek was successful, EFalse otherwise.
*/
TBool RCntModel::SeekContactL(TContactItemId aContactId,TContactItemId& aId,TUid& aContactType, TBool& aDeleted)
    {
    TPckgBuf<TInt> id;
    TPckgBuf<TUid> type;
    TPckgBuf<TBool> deleted;
    TIpcArgs args(aContactId,&id,&type,&deleted);
    TInt ret = SendReceive(ECntSeekContactInCollection,args);

    // Although ECntSeekContact/ECntSeekContactInCollection is normally completed
    // with TBool values, ServiceError() can occur which leads to a negative system
    // error codes being returned.  User::LeaveIfError will allow that to be properly
    // handled.
    User::LeaveIfError(ret);

    if(ret)
        {
        aContactType = type();
        aId = id();
        aDeleted = deleted();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


void RCntModel::TextFieldL(TInt aCntItemId,TFieldType aFieldType, TDes& aText)
	{
	TPckgBuf<TFieldType> fieldType(aFieldType);
	TIpcArgs args(aCntItemId,&fieldType,&aText);
	User::LeaveIfError(SendReceive(ECntTextField,args));
	}


void RCntModel::ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,const CContactTextDef& aTextDef)
	{
	iPackager->PackL( const_cast<CContactTextDef&>(aTextDef));
	TIpcArgs args(&iPackager->GetTransmittingBuffer(), &aResult,aContactId);
	User::LeaveIfError(SendReceive(ECntReadContactTextDef, args));
	}


CContactIdArray* RCntModel::FindL(const TDesC& aText,const CContactItemFieldDef* aFieldDef)
	{
	TIpcArgs args;

	// Parameter can be NULL so send down an empty descriptor to the server.
	// Server can detect this and pass NULL parameter to the Persistence Layer
	// FindL() method.
	if(aFieldDef != NULL)
		{
		// Packager's sending buffer in slot 1 (contains packed field
		// definition).
		iPackager->PackL(*aFieldDef);
		args.Set(1,&iPackager->GetTransmittingBuffer());
		}
	else
		{
		// Frigged NULL.
		args.Set(1,&KNullDesC);
		}

	// Packager's receiving buffer in slot 0.
	args.Set(0, &iPackager->GetReceivingBufferL());

	// Text in slot 2.
	args.Set(2,&aText);

	// Ensure receiving buffer is big enough.
	TInt newBuffSize;
	User::LeaveIfError(newBuffSize = SendReceive(ECntFind, args));
	if (newBuffSize > 0)
		{
		// Not big enough so resize and call again.
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntFind, args));
		}

	CContactIdArray* idArray = iPackager->UnpackCntIdArrayLC();	
	CleanupStack::Pop(idArray);

	return idArray;	
	}


void RCntModel::SetDbViewContactType(const TUid aUid)
	{
	TIpcArgs args(aUid.iUid);
	(void)SendReceive(ECntSetDbViewContactType,args);
	}


TUid RCntModel::GetDbViewContactType() const
	{
	TPckgBuf<TUid> type;
	TIpcArgs args(&type);
	(void)SendReceive(ECntSetDbViewContactType,args);
	return type();	
	}


/**
Initialise the Persistence Layer collection class ready for iterative calls to
the FindAsyncL() method.  This form of initialisation is for an asynchronous
find which uses a text definition and an array of "find words".

@param aWords "Find words" array.
@param aTextDef Text definition to use in find.
*/
void RCntModel::FindAsyncTextDefInitL(const CDesCArray& aWords,CContactTextDef* aTextDef)
	{
	TIpcArgs args;
	if(aTextDef != NULL)
		{
		// Pack the text definition.
		iPackager->PackL(*aTextDef);
		args.Set(1,&iPackager->GetTransmittingBuffer());
		}
	else
		{
		// No user supplied text definition.
		args.Set(1, &KNullDesC);
		}
	
	// Do the CDesCArray manually as Packager can't do both.
	RBufWriteStream writeStream;
	CBufFlat* buf = CBufFlat::NewL(1 << 8);
	CleanupStack::PushL(buf);

	writeStream.Open(*buf);
	TInt count = aWords.Count();
	writeStream.WriteInt32L(count);	
	TInt length = 0;
	for(TInt i=0; i<count;++i)
		{
		length = aWords[i].Length();
		writeStream.WriteInt32L(length);		
		writeStream.WriteL(aWords[i],length);
		}
	writeStream.CommitL();
	writeStream.Close();

	TPtr8 ptr8(buf->Ptr(0));
	args.Set(0,&ptr8);

	User::LeaveIfError(SendReceive(ECntFindAsyncTextDefInit,args));

	CleanupStack::PopAndDestroy(buf);
	}


/**
Initialise the Persistence Layer collection class ready for iterative calls to
the FindAsyncL() method.  This form of initialisation is for an asynchronous
find which uses text and a field definition.

@param aText Find text.
@param aFieldDef Field definition to use in find.
*/
void RCntModel::FindAsyncInitL(const TDesC& aText,CContactItemFieldDef* aFieldDef)
	{
	TIpcArgs args;
	args.Set(0,&aText);
	if(aFieldDef != NULL)
		{
		// Pack the field definition.
		iPackager->PackL(*aFieldDef);
		args.Set(1,&iPackager->GetTransmittingBuffer());
		}
	else
		{
		// No user supplied field definition.
		args.Set(1, &KNullDesC);
		}

	User::LeaveIfError(SendReceive(ECntFindAsyncInit,args));
	}


/**
Perform an asynchronous find iteration.  Either the FindAsyncTextDefInitL() or
the FindAsyncInitL() method above must have first been called.

@param aOrigIdArray Set of contacts IDs from previous calls to FindAsyncL().
Will be NULL if this is the first iteration.

@return ETrue if further find iterations are required, EFalse otherwise.
*/
TBool RCntModel::FindAsyncL(CContactIdArray*& aOrigIdArray)
	{
	TBool moreToGo;
	TPckg<TBool> pckg(moreToGo);
	// Iterations can't fail so expand buffer to 4Kb.  4096 contacts should be
	// enough.  Rather than this brutal approach we could call ContactCountL()
	// and add a contingency in case another client adds some.
	TIpcArgs args(&iPackager->GetReceivingBufferL(4096),&pckg);

	User::LeaveIfError(SendReceive(ECntFindAsync,args));

	CContactIdArray* newIdArray = iPackager->UnpackCntIdArrayLC();	
	if(aOrigIdArray != NULL)
		{
		// Append to the old array if it was not NULL.
		for(TInt i=0;i<newIdArray->Count();++i)
			{
			aOrigIdArray->AddL((*newIdArray)[i]);
			}
		CleanupStack::PopAndDestroy(newIdArray);
		}
	else
		{
		CleanupStack::Pop(newIdArray);
		aOrigIdArray = newIdArray;
		}

	return pckg();
	}


/**
Set the asynchronous activity flag in the server.  Any asynchronous activity
such as asynchronous find or asynchronous sort prevent the database from
being closed and should set this flag.  When all asynchronous activites are
finished this flag should be cleared.

@capability None

@param aAsyncActivity ETrue if at least one asynchronous activity, EFalse if
no asynchronous activities.
*/
void RCntModel::SetAsyncActivityL(TBool aAsyncActivity)
	{
	TIpcArgs args;
	args.Set(0,aAsyncActivity);
	User::LeaveIfError(SendReceive(ECntSetAsyncActivity,args));
	}


/**
Filter the database using the given filter.

@param aFilter Filter to use.  On return the contact IDs in this filter will be
augmented with those contact IDs which match the given filter.
*/
void RCntModel::FilterDatabaseL(CCntFilter& aFilter)
	{
	CContactIdArray* origIdArray = aFilter.iIds;
	aFilter.iIds = NULL;

	TIpcArgs args;
	iPackager->PackL(aFilter);
	args.Set(1,&iPackager->GetTransmittingBuffer());	
	args.Set(0, &iPackager->GetReceivingBufferL());

	TInt newBuffSize(0);
	User::LeaveIfError(newBuffSize = SendReceive(ECntFilterDatabase, args));
	if (newBuffSize > 0)
		{
		// Not big enough so resize and call again.
		args.Set(0, &iPackager->GetReceivingBufferL(newBuffSize));
		User::LeaveIfError(newBuffSize = SendReceive(ECntFilterDatabase, args));
		}
	
	CContactIdArray* newIdArray = iPackager->UnpackCntIdArrayLC();
	if(origIdArray != NULL)
		{
		for(TInt i=0;i<newIdArray->Count();++i)
			{
			origIdArray->AddL((*newIdArray)[i]);
			}
		aFilter.iIds = origIdArray;
		CleanupStack::PopAndDestroy(newIdArray);
		}
	else
		{
		CleanupStack::Pop(newIdArray);
		aFilter.iIds = newIdArray;
		}
	}

	
TInt RCntModel::OpenViewL(const CContactTextDef& aTextDef, const TInt aViewPrefs)
	{
	TPckgBuf<TInt> viewSessionId;
	iPackager->PackL(const_cast<CContactTextDef&>(aTextDef));
	TIpcArgs args(&iPackager->GetTransmittingBuffer(), aViewPrefs, &viewSessionId);
	
	TInt ret = SendReceive(ECntOpenViewSession,args);
	User::LeaveIfError(ret);
	
	return 	viewSessionId();
	}
	
	
void RCntModel::CloseView(TInt aViewId)
	{
	TIpcArgs args(aViewId);
	SendReceive(ECntCloseViewSession,args);
	}
	
	
void RCntModel::ChangeSortOrderL(TInt aViewId, const CContactTextDef& aTextDef)
	{
	// Serialize the text def using packager overload.
	iPackager->PackL( const_cast<CContactTextDef&>(aTextDef));
	

	TIpcArgs args(aViewId, &iPackager->GetTransmittingBuffer());
	TInt ret = SendReceive(ECntViewChangeSortOrderL, args);
	
	User::LeaveIfError(ret);
	}
	
	
void RCntModel::BeginIterateL(TInt aViewId)
	{
	TIpcArgs args(aViewId);
	TInt ret = SendReceive(ECntViewBeginIterate,args);
	User::LeaveIfError(ret);
	}
	
	
void RCntModel::EndIterateL(TInt aViewId)
	{
	TIpcArgs args(aViewId);
	TInt ret = SendReceive(ECntViewEndIterate,args);
	User::LeaveIfError(ret);
	}
	
	
CViewContact* RCntModel::NextItemL(TInt aViewId, const TInt aViewPrefs)
	{
	TIpcArgs args(aViewId, aViewPrefs, &iPackager->GetReceivingBufferL());
	TInt size = SendReceive(ECntViewNextItemL, args);

	// KErrNotFound is returned if Persistence Layer view item manager 
	// can't reach end of the contacts.
	if(size == KErrNotFound)
		{
		// Not an error just no match.
		return NULL;
		}

	// Leave on genuine errors.
	User::LeaveIfError(size);

	if(size != 0) // Server session requires larger packager buffer.
		{
		// Increase the buffer size and call again.  Just in case some other
		// client has deleted in between calls do the KErrNotFound check.
		TIpcArgs args(aViewId, aViewPrefs, &iPackager->GetReceivingBufferL(size));
		TInt ret = SendReceive(ECntViewNextItemL, args);
		if(ret == KErrNotFound)
			{
			return NULL;
			}
		// Leave on genuine errors
		User::LeaveIfError(ret);
		}

	// Got it so unpack and return.
	CViewContact* viewContact = iPackager->UnpackViewContactLC();

	CleanupStack::Pop(viewContact);

	return viewContact;
	}

	
CViewContact* RCntModel::ItemAtL(TContactItemId aContactId, TInt aViewId)
	{
	TIpcArgs args(aContactId, aViewId, &iPackager->GetReceivingBufferL());
	TInt size = SendReceive(ECntItemAtL, args);

	// KErrNotFound is returned if Persistence Layer view item manager cannot 
	// find the view contact by given aContactId.
	if(size == KErrNotFound)
		{
		// Not an error just no match.
		return NULL;
		}

	// Leave on genuine errors.
	User::LeaveIfError(size);

	if(size != 0) // Server session requires larger packager buffer.
		{
		// Increase the buffer size and call again.  Just in case some other
		// client has deleted in between calls do the KErrNotFound check.
		TIpcArgs args(aContactId, aViewId, &iPackager->GetReceivingBufferL(size));
		TInt ret = SendReceive(ECntItemAtL, args);
		if(ret == KErrNotFound)
			{
			return NULL;
			}
		// Leave on genuine errors
		User::LeaveIfError(ret);
		}

	// Got it so unpack and return.
	CViewContact* viewContact = iPackager->UnpackViewContactLC();

	CleanupStack::Pop(viewContact);

	return viewContact;
	}
	

#if defined(_DEBUG)	
void RCntModel::GetDefinitionsOfExistingViewsL(const TDesC& aDbName, RPointerArray<CContactDefaultViewDefinition>& aViewDefs)
	{
	RBuf8 buf;
	CleanupClosePushL(buf);
	buf.Create(256);
	TIpcArgs args(&aDbName, &buf);

	TInt newBufSize;
	User::LeaveIfError(newBufSize = SendReceive(ECntGetDefinitionsForExistingView, args));
	if (newBufSize > 0)
		{
		buf.ReAllocL(newBufSize);
		args.Set(1, &buf);
		User::LeaveIfError(newBufSize = SendReceive(ECntGetDefinitionsForExistingView, args));		
		}

	RDesReadStream readStream(buf);
	CleanupClosePushL(readStream);
	TInt32 count;
	readStream >> count;
	CContactDefaultViewDefinition* view;
	for (TInt i = 0; i < count; i++)
		{
		view = CContactDefaultViewDefinition::NewLC(readStream);
		aViewDefs.AppendL(view);
		CleanupStack::Pop(view);
		}
		
	CleanupStack::PopAndDestroy(2); // readStream, buf	
	}
#else
void RCntModel::GetDefinitionsOfExistingViewsL(const TDesC& /*aDbName*/, RPointerArray<CContactDefaultViewDefinition>& /*aViewDefs*/)
	{	
	}
#endif // _DEBUG
