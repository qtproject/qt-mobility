/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "ccntmsghandler.h"
#include "ccntmsghandlerfptr.h"
#include "ccntfilemanagermsghandler.h"

#include "ccntserver.h"
#include "ccntsession.h"
#include "ccntipccodes.h"
#include "ccntrequest.h"
#include "ccntdbmanager.h"
#include "ccntstatemachine.h"
#include "ccntdbmanagercontroller.h"
#include "ccntpackager.h"
#include "ccntipccodes.h"
#include <cntviewstore.h>
#include "cntviewprivate.h"
#include "cviewsubsessions.h"

const TInt KCntFileManagerIpcCodes[] =
	{
	ECntOpenDataBase,
	ECntCreateDatabase,
	ECntReplaceDatabase,
	ECntCancelAsyncOpenDatabase,
	ECntCloseDataBase,
	ECntCloseDbTables,
	ECntReOpenDbTables,
	ECntDeleteDatabase,
	ECntGetDefaultDatabaseName,
	ECntDatabaseDrive,
	ECntSetDatabaseDrive,
	ECntDatabaseExists,
	ECntListDatabases,
	ECntGetDatabaseReady,
	ECntFetchTemplateIds,
	ECntFetchGroupIdLists,
	ECntSearchResultList,
	ECntPredictiveSearchList,
	ECntFilesSize,
	ECntGetDefinitionsForExistingView
	};

CCntFileManagerMsgHandler* CCntFileManagerMsgHandler::NewLC(CCntSession& aSession)
	{
	CCntFileManagerMsgHandler* CntFileManagerMsgHandler = new (ELeave) CCntFileManagerMsgHandler(aSession);
	CleanupStack::PushL(CntFileManagerMsgHandler);
	return CntFileManagerMsgHandler;
	}
	

CCntFileManagerMsgHandler::CCntFileManagerMsgHandler(CCntSession& aSession)
:CCntMsgHandler(aSession)
	{		
	}

// ----------------------------------------------------------------------------
// CntPredictiveSearch::ConstructL
// ----------------------------------------------------------------------------
void CCntFileManagerMsgHandler::ConstructL()
    {
    }

CCntFileManagerMsgHandler::~CCntFileManagerMsgHandler()
    {
    }

/**
Delegates the incoming op code to a message handling method.

First checks if this class services the op code, it then uses the lookup table and finds 
function pointer(to message handling method) mapped to the incoming message function (op code)
and finally delegates the message to handling method.

It returns KErrNotFound if op code not handled.
*/
TInt CCntFileManagerMsgHandler::HandleMessageL(const RMessage2& aMessage)
	{
	MsgHandlerFptr func_ptr = LookupHandlerMethodL(aMessage.Function(), KCntFileManagerIpcCodes, sizeof(KCntFileManagerIpcCodes)/sizeof(TInt));
	
	if(func_ptr)
		{
		FileManagerMsgHandlerFptr mem_func_ptr = static_cast<FileManagerMsgHandlerFptr>(func_ptr);
		(this->*mem_func_ptr)(aMessage);
		return (KErrNone);
		}
	
	return (KErrNotFound);
	}
	
/**
This method contains all database file open/create/replace functionality.
*/
void CCntFileManagerMsgHandler::FileOpenCreateReplaceL(const RMessage2& aMessage, TCntFileMode aMode)
	{
	// Sanity check: there can be only one CCntDbManager instance per session.
	if(iManager)
		{
		aMessage.Complete(KErrAlreadyExists);
		return;
		}
	
	CReqAsyncOpen* openReq = CReqAsyncOpen::NewLC(iSessionId, aMessage, iTimeOut);
	
	// Get an instance of the CCntDbManager class for this file.  One instance
	// is created per Contacts database file and shared among sessions using
	// that file.
	iManager = Server().Controller().GetDbManagerL(openReq->FileName(), aMode);
	
	// Register this session as a database event observer.
	iManager->RegisterDatabaseEventObserverL(iSession);
	
	// Process the open request via the State Machine.  The State Machine
	// ensures that the file (owned by the CCntDbManager instance) is in a valid
	// state to process the request.
	iManager->StateMachineL().ProcessRequestL(openReq);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.	
	CleanupStack::Pop(openReq);
	}

#if defined(_DEBUG)	
void CCntFileManagerMsgHandler::DefinitionsOfExistingViewsL(const RMessage2& aMessage)
	{
	TFileName fileName;
	ReadL(aMessage,KSlot0,fileName);
	if (fileName.Length() == 0)
		{
		Server().Controller().DefaultDatabaseL(fileName);
		}
	
	RPointerArray<CContactDefaultViewDefinition> viewDefs;
	CleanupResetAndDestroyPushL(viewDefs);		
	CCntDbManager* manager = Server().Controller().DbManagerL(fileName);
	if (manager)
		{
		manager->ViewManagerL().GetDefinitionsOfExistingViewsL(viewDefs);
		}
	
	// Compute the size of the buffer that is needed.
	CBufFlat* buffer = CBufFlat::NewL(32);
	CleanupStack::PushL(buffer);
	RBufWriteStream writeStream(*buffer);
	CleanupClosePushL(writeStream);
	
	const TInt32 count = viewDefs.Count();
	writeStream << count;
	for (TInt i = 0; i < count; i++)
		{
		writeStream << *viewDefs[i];
		}	

	// Check that the client-side write buffer is large enough.
	TInt length = buffer->Size();
	if(aMessage.GetDesMaxLength(1) >= length)
		{
		aMessage.WriteL(1, buffer->Ptr(0)); 
		aMessage.Complete(KErrNone);
		}
	else
		{
		aMessage.Complete(length);
		}	
	CleanupStack::PopAndDestroy(3, &viewDefs); // writeStream, buffer, viewDefs
	}
#else
void CCntFileManagerMsgHandler::DefinitionsOfExistingViewsL(const RMessage2& aMessage)
	{
	aMessage.Complete(KErrNone);
	}
	
#endif // _DEBUG
	
/**
Message handling methods.
*/

// CRUD file operations. Methods delegate to the appropriate CCntDbManagerController method.
void CCntFileManagerMsgHandler::OpenDataBaseL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::OpenDatabase().
	FileOpenCreateReplaceL(aMessage, ECntFileOpen);	
	}
void CCntFileManagerMsgHandler::CreateDatabaseL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::CreateDatabase().
	FileOpenCreateReplaceL(aMessage, ECntFileCreate);
	}
	
void CCntFileManagerMsgHandler::ReplaceDatabaseL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::ReplaceDatabase().
	FileOpenCreateReplaceL(aMessage, ECntFileReplace);
	}
	
void CCntFileManagerMsgHandler::CancelAsyncOpenDatabaseL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqCancelAsyncOpen* request = CReqCancelAsyncOpen::NewLC(iSessionId, aMessage, iTimeOut);

	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred

	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.	
	CleanupStack::Pop(request);
	}

void CCntFileManagerMsgHandler::CloseDataBaseL(const RMessage2& aMessage)
	{
	UnRegisterDatabaseEventObserver();
	// Call the controller to deallocate.  The controller may have more than
	// one session mapped to a single manager so the controller only deletes
	// the manager if this is the last session to close the database.  There
	// is no need to move to the Closed state since it is the manager which
	// processes state transition requests and the manager is about to be
	// destroyed within CloseDatatbase().
	Server().Controller().CloseDatabase(*iManager);
	iManager = NULL;
		
	aMessage.Complete(KErrNone);
	}
	
void CCntFileManagerMsgHandler::CloseDbTablesL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqCloseTables* request = CReqCloseTables::NewLC(iSessionId, aMessage, iTimeOut);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred

	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.
	CleanupStack::Pop(request);
	}
	
void CCntFileManagerMsgHandler::ReOpenDbTablesL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::OpenTablesL().
	CheckForManagerL();
	CReqReOpen* request = CReqReOpen::NewLC(iSessionId, aMessage, iTimeOut);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred

	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.
	CleanupStack::Pop(request);
	}
	
void CCntFileManagerMsgHandler::DeleteDatabaseL(const RMessage2& aMessage)
	{
	// Non file-specific.  Does not require manager instance.
	// Full name and path.  Path length is validated on client-side.
	TBuf<KCntMaxFilePath> path;
	aMessage.ReadL(0,path);
	Server().Controller().DeleteDatabaseL(path);
	aMessage.Complete(KErrNone);
	}
	
void CCntFileManagerMsgHandler::GetDefaultDatabaseNameL(const RMessage2& aMessage)
	{
	// Non file-specific.  Does not require manager instance.
	TBuf<KCntMaxFilePath> path;
	// Call controller to retrieve default database name.
	Server().Controller().DefaultDatabaseL(path);
	aMessage.WriteL(0,path);
	aMessage.Complete(KErrNone);
	}
	
void CCntFileManagerMsgHandler::DatabaseDrive(const RMessage2& aMessage)
	{
	// Non file-specific. Does not require manager instance.
	TDriveUnit drive;
	// Call controller to retrieve default database drive.
	Server().Controller().DatabaseDrive(drive);
	aMessage.Complete(drive);
	}
	
void CCntFileManagerMsgHandler::SetDatabaseDriveL(const RMessage2& aMessage)
	{
	// Non file-specific. Does not require manager instance.
	// Parameter 1 is the integer drive 0-25.
	// Parameter 2 in the copy boolean.
	Server().Controller().SetDatabaseDriveL(TDriveNumber(aMessage.Int0()),TBool(aMessage.Int1()));
	aMessage.Complete(KErrNone);
	}
	
void CCntFileManagerMsgHandler::DatabaseExistsL(const RMessage2& aMessage)
	{
	// Non file-specific. Does not require manager instance.
	// Full name and path.  Path length is validated on client-side.
	TBuf<KCntMaxFilePath> path;
	aMessage.ReadL(0,path);
	aMessage.Complete(Server().Controller().DatabaseExistsL(path));
	}
	
void CCntFileManagerMsgHandler::ListDatabasesL(const RMessage2& aMessage)
	{
	// Non file-specific. Does not require manager instance.
	// Client request to list contact databases on a specific or all system
	// drives.
	// Return parameter for serialized CDesCArray.
	CBufFlat* listBuffer;
	// IPC args thus:
	// 0 (Return param)	- Address of our return buffer
	// 1 (Param)		- The drive number 0 - 25 or ECntAllDrives
	// 2 (Return param)	- The size of buffer we are returning or want to return
	TInt driveNumber = aMessage.Int1();
	// Call controller to retrieve list of databases.
	if(driveNumber == ECntAllDrives)
		{
		Server().Controller().ListDatabasesL(listBuffer);
		}
	else
		{
		Server().Controller().ListDatabasesL(listBuffer,(TDriveUnit*)&driveNumber);
		}
	// Controller will have filled the listBuffer with a serialized
	// CDesCArray.
	CleanupStack::PushL(listBuffer);
	// Size of the buffer we want to return to client.
	TInt size = listBuffer->Size(); 
	TPckg<TInt> pckg(size);
	// Write return buffer size to client.
	aMessage.WriteL(2,pckg);
	// Only write if client has provided large enough buffer.  Client can
	// call us again with a larger buffer if contents are too large.
	if(aMessage.GetDesMaxLength(0) >= size)
		{
		aMessage.WriteL(0,listBuffer->Ptr(0));
		}
	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(listBuffer);	
	}
	
void CCntFileManagerMsgHandler::GetDatabaseReadyL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	TBool databaseReady = iManager->StateMachineL().DatabaseReady();
	aMessage.Complete(databaseReady);
	}
	
void CCntFileManagerMsgHandler::FetchTemplateIdsL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CContactIdArray& ArrayPtr = iManager->GetPersistenceLayer().ContactProperties().CardTemplateIdsL();
	TPtr8 cntIDBuff = iPackager.PackL(ArrayPtr); 
	TInt length = cntIDBuff.Length();
	// Write data if client buffer is large enough otherwise return the
	// required buffer size.
	if(aMessage.GetDesMaxLength(0) >= length)
		{ 
		aMessage.WriteL(0, cntIDBuff); 
		aMessage.Complete(KErrNone);
		}
	else
		{ 
		aMessage.Complete(length);	
		}
	}
	
void CCntFileManagerMsgHandler::FetchGroupIdListsL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CContactIdArray& arrayPtr = iManager->GetPersistenceLayer().ContactProperties().GroupIdListL();
	TPtr8 cntIDBuff = iPackager.PackL(arrayPtr); 
	TInt length = cntIDBuff.Length();
	// Write data if client buffer is large enough otherwise return the
	// required buffer size.
	if(aMessage.GetDesMaxLength(0) >= length)
		{ 
		aMessage.WriteL(0, cntIDBuff); 
		aMessage.Complete(KErrNone);
		}
	else
		{ 
		aMessage.Complete(length);	
		}
	}
	
void CCntFileManagerMsgHandler::FetchPredictiveSearchResultsL(const RMessage2& aMessage)
    {
    const TInt KSqlQueryMaxLen = aMessage.GetDesLengthL(1); 
    HBufC* searchPattern = HBufC::NewLC(KSqlQueryMaxLen);
    TPtr searchPatternPtr(searchPattern->Des());
    aMessage.ReadL(1, searchPatternPtr);
    
    CheckForManagerL();
    CBufSeg* buffer = iManager->GetPersistenceLayer().ContactProperties().DetailsListPredictiveL(searchPattern->Des());
    if (aMessage.GetDesMaxLength(0) >= buffer->Size())
        {
        TInt offset = 0;
        while (offset < buffer->Size())
            {
            TPtr8 ptr = buffer->Ptr(offset);
            aMessage.WriteL(0, ptr, offset);
            offset += ptr.Size();
            }
        aMessage.Complete(KErrNone);
        }
    else
        {
        aMessage.Complete(buffer->Size());
        }

    delete buffer;
    CleanupStack::PopAndDestroy( searchPattern ); 
    }

void CCntFileManagerMsgHandler::FetchSearchResultsL(const RMessage2& aMessage)
    {
    const TInt KSqlQueryMaxLen = aMessage.GetDesLengthL(1); 
    HBufC* searchQuery = HBufC::NewLC(KSqlQueryMaxLen);
    TPtr searchQueryPtr(searchQuery->Des());
    aMessage.ReadL(1, searchQueryPtr);
    
    CheckForManagerL();

    CBufSeg* buffer = iManager->GetPersistenceLayer().ContactProperties().DetailsListL(searchQuery->Des());
    if (aMessage.GetDesMaxLength(0) >= buffer->Size())
        {
        TInt offset = 0;
        while (offset < buffer->Size())
            {
            TPtr8 ptr = buffer->Ptr(offset);
            aMessage.WriteL(0, ptr, offset);
            offset += ptr.Size();
            }
        aMessage.Complete(KErrNone);
        }
    else
        {
        aMessage.Complete(buffer->Size());
        }

    delete buffer;
    CleanupStack::PopAndDestroy(); //searchQuery
    }
    

void CCntFileManagerMsgHandler::GetDefinitionsForExistingViewL(const RMessage2& aMessage)
	{
	DefinitionsOfExistingViewsL(aMessage);
	}


void CCntFileManagerMsgHandler::FilesSizeL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	aMessage.Complete(iManager->FileSizeL());
	}
