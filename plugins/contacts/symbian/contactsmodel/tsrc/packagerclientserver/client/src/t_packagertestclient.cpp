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


#include <e32std.h>
#include <e32math.h>
#include <cntitem.h>
#include <cntfilt.h>
#include "t_packagertestclient.h"
#include "t_packagertestdef.h"


EXPORT_C void RPackagerTestClient::ConnectL()
/** Check and create server process if it is not already running,
then create a server session. Also constructs packager member. */
	{
	// Assume the server is already running and attempt to create a session
	// 4 message slots
	TInt err = CreateSession(KPackagerServerTestName,TVersion(1,1,1),4);
	if(err == KErrNotFound)
		{
		// Server not running
		// Use the RProcess API to start it.
		RProcess server;
		User::LeaveIfError(server.Create(KPackagerServerTestExe,KNullDesC));

		// Synchronise with the server
		TRequestStatus reqStatus;
		server.Rendezvous(reqStatus);
		// Start the test harness
		server.Resume();
		// Server will call the reciprocal static synchronise cal User::WaitForRequest(reqStatus);
		User::WaitForRequest(reqStatus);
		server.Close();
		User::LeaveIfError(reqStatus.Int() != KErrNone);
		// Create the root server session
		User::LeaveIfError(CreateSession(KPackagerServerTestName,TVersion(0,0,0),4));
		}		
		
	iPackager = CCntPackager::NewL();
	}
	
EXPORT_C void RPackagerTestClient::Close()
/** Frees all resources owned by the client. */
	{
	delete iPackager;
	iPackager = NULL;
	RHandleBase::Close();
	}
	
EXPORT_C void RPackagerTestClient::PackAndSendL(const CContactItem& anItem, TRequestStatus& aStatus)
/** Packs and sends a CContactItem object to the server.

@param anItem The CContactItem object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCContactItem, args, aStatus);
	}	
	
EXPORT_C void RPackagerTestClient::PackAndSendL(const CContentType& anItem, TRequestStatus& aStatus)
/** Packs and sends a CContentType object to the server.

@param anItem The CContentType object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCContentType, args, aStatus);	
	}

EXPORT_C void RPackagerTestClient::PackAndSendL(const CContactTextDef& anItem, TRequestStatus& aStatus)
/** Packs and sends a CContactTextDef object to the server.

@param anItem The CContactTextDef object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCContactTextDef, args, aStatus);	
	}

EXPORT_C void RPackagerTestClient::PackAndSendL(const CContactItemViewDef& anItem, TRequestStatus& aStatus)
/** Packs and sends a CContactItemViewDef object to the server.

@param anItem The CContactItemViewDef object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCContactItemViewDef, args, aStatus);	
	}

EXPORT_C void RPackagerTestClient::PackAndSendL(const CContactViewDef& anItem, TRequestStatus& aStatus)
/** Packs and sends a CContactViewDef object to the server.

@param anItem The CContactViewDef object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCContactViewDef, args, aStatus);	
	}

EXPORT_C void RPackagerTestClient::PackAndSendL(const CContactIdArray& anItem, TRequestStatus& aStatus)
/** Packs and sends a CContactIdArray object to the server.

@param anItem The CContactIdArray object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCContactIdArray, args, aStatus);	
	}

EXPORT_C void RPackagerTestClient::PackAndSendL(const CCntFilter& anItem, TRequestStatus& aStatus)
/** Packs and sends a CCntFilter object to the server.

@param anItem The CCntFilter object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCCntFilter, args, aStatus);	
	}
	
EXPORT_C void RPackagerTestClient::PackAndSendL(const CViewContact& anItem, TRequestStatus& aStatus)
/** Packs and sends a CViewContact object to the server.

@param anItem The CViewContact object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCViewContact, args, aStatus);	
	}	

EXPORT_C void RPackagerTestClient::PackAndSendL(const CDesCArray& anItem, TRequestStatus& aStatus)
/** Packs and sends a CDesCArray object to the server.

@param anItem The CDesCArray object to be sent.
@param aStatus The TRequestStatus required for making the asynchronous call. */
	{
	TIpcArgs args;
	iPackager->PackL(anItem);
	PrepareArgumentsL(args);
	SendReceive(EProcessCDesCArray, args, aStatus);	
	}
	

void RPackagerTestClient::PrepareArgumentsL(TIpcArgs& anArgs) const
/** Sets a TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.

@param anArgs The TIpcArgs object to be set. */
	{
	anArgs.Set(EIPCPackagerTransmitBuffer, &iPackager->GetTransmittingBuffer());
	anArgs.Set(EIPCPackagerReceiveBuffer, &iPackager->GetReceivingBufferL());		
	}	

EXPORT_C CContactItem* RPackagerTestClient::GetAndUnpackCntItemLC() const
/** Retrieves and unpacks a CContactItem from the server.

@return the unpacked CContactItem object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCContactItem, args));
		
	return DoUnpackCntItemLC(newBuffSize, args);
	}
	
EXPORT_C CContentType* RPackagerTestClient::GetAndUnpackCntContentTypeLC() const
/** Retrieves and unpacks a CContentType from the server.

@return the unpacked CContentType object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCContentType, args));
		
	return DoUnpackCntContentTypeLC(newBuffSize, args);
	}
	
EXPORT_C CContactTextDef* RPackagerTestClient::GetAndUnpackCntTextDefLC() const
/** Retrieves and unpacks a CContactTextDef from the server.

@return the unpacked CContactTextDef object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCContactTextDef, args));
		
	return DoUnpackCntTextDefLC(newBuffSize, args);
	}
EXPORT_C CContactItemViewDef* RPackagerTestClient::GetAndUnpackCntItemViewDefLC() const
/** Retrieves and unpacks a CContactItemViewDef from the server.

@return the unpacked CContactItemViewDef object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCContactItemViewDef, args));
		
	return DoUnpackCntItemViewDefLC(newBuffSize, args);
	}
	
EXPORT_C CContactViewDef* RPackagerTestClient::GetAndUnpackCntViewDefLC() const
/** Retrieves and unpacks a CContactViewDef from the server.

@return the unpacked CContactViewDef object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCContactViewDef, args));
		
	return DoUnpackCntViewDefLC(newBuffSize, args);
	}
	
EXPORT_C CContactIdArray* RPackagerTestClient::GetAndUnpackCntIdArrayLC() const
/** Retrieves and unpacks a CContactIdArray from the server.

@return the unpacked CContactIdArray object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCContactIdArray, args));
		
	return DoUnpackCntIdArrayLC(newBuffSize, args);
	}
EXPORT_C CCntFilter* RPackagerTestClient::GetAndUnpackCntFilterLC() const
/** Retrieves and unpacks a CCntFilter from the server.

@return the unpacked CCntFilter object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCCntFilter, args));
		
	return DoUnpackCntFilterLC(newBuffSize, args);
	}
	
EXPORT_C CViewContact* RPackagerTestClient::GetAndUnpackViewCntLC() const
/** Retrieves and unpacks a CViewContact from the server.

@return the unpacked CViewContact object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCViewContact, args));
		
	return DoUnpackViewCntLC(newBuffSize, args);
	}	
EXPORT_C CDesCArray* RPackagerTestClient::GetAndUnpackDesCArrayLC() const
/** Retrieves and unpacks a CDesCArray from the server.

@return the unpacked CDesCArray object. */
	{
	TIpcArgs args;
	PrepareArgumentsL(args);
	TInt newBuffSize = 0;
	User::LeaveIfError(newBuffSize = SendReceive(EReceiveCDesCArray, args));
		
	return DoUnpackDesCArrayLC(newBuffSize, args);
	}
	
EXPORT_C CContactItem* RPackagerTestClient::DoUnpackCntItemLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CContactItem from server. 
If not, increase size of buffer and re-request the CContactItem, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CContactItem object. */
	{
	if(aBufferSize > 0) 
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCContactItem, anArgs));
		}
	return iPackager->UnpackCntItemLC();
	}
	
EXPORT_C CContentType* RPackagerTestClient::DoUnpackCntContentTypeLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CContentType from server. 
If not, increase size of buffer and re-request the CContentType, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CContentType object. */
	{
	if(aBufferSize > 0)
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCContentType, anArgs));
		}
	return iPackager->UnpackCntContentTypeLC();
	}
	
EXPORT_C CContactTextDef* RPackagerTestClient::DoUnpackCntTextDefLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CContactTextDef from server. 
If not, increase size of buffer and re-request the CContactTextDef, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CContactTextDef object. */
	{
	if(aBufferSize > 0)
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCContactTextDef, anArgs));
		}
	return iPackager->UnpackCntTextDefLC();
	}
	
EXPORT_C CContactItemViewDef* RPackagerTestClient::DoUnpackCntItemViewDefLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CContactItemViewDef from server. 
If not, increase size of buffer and re-request the CContactItemViewDef, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CContactItemViewDef object. */
	{
	if(aBufferSize > 0) 
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCContactItemViewDef, anArgs));
		}
	return iPackager->UnpackCntItemViewDefLC();
	}
	
EXPORT_C CContactViewDef* RPackagerTestClient::DoUnpackCntViewDefLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CContactViewDef from server. 
If not, increase size of buffer and re-request the CContactViewDef, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CContactViewDef object. */
	{
	if(aBufferSize > 0) 
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCContactViewDef, anArgs));
		}
	return iPackager->UnpackCntViewDefLC();
	}
	
EXPORT_C CContactIdArray* RPackagerTestClient::DoUnpackCntIdArrayLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CContactIdArray from server. 
If not, increase size of buffer and re-request the CContactIdArray, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CContactIdArray object. */
	{
	if(aBufferSize > 0)
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCContactIdArray, anArgs));
		}
	return iPackager->UnpackCntIdArrayLC();
	}
	
EXPORT_C CCntFilter* RPackagerTestClient::DoUnpackCntFilterLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CCntFilter from server. 
If not, increase size of buffer and re-request the CCntFilter, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CCntFilter object. */
	{
	if(aBufferSize > 0)
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCCntFilter, anArgs));
		}
	return iPackager->UnpackCntFilterLC();
	}
	
EXPORT_C CViewContact* RPackagerTestClient::DoUnpackViewCntLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CViewContact from server. 
If not, increase size of buffer and re-request the CViewContact, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CViewContact object. */
	{
	if(aBufferSize > 0) 
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCViewContact, anArgs));
		}
	return iPackager->UnpackViewContactLC();
	}	
	
EXPORT_C CDesCArray* RPackagerTestClient::DoUnpackDesCArrayLC(TInt aBufferSize, TIpcArgs& anArgs) const
/** Checks if current packager buffer is large enough to accept incoming CDesCArray from server. 
If not, increase size of buffer and re-request the CDesCArray, and unpack it.

@param aBufferSize The required buffer size.
@param anArgs The TIpcArgs object with the packager's buffer details necessary
for inter-process data exchange.
@return the unpacked CDesCArray object. */
	{
	if(aBufferSize > 0)
		{
		anArgs.Set(1, &iPackager->GetReceivingBufferL(aBufferSize));

		User::LeaveIfError(SendReceive(EReceiveCDesCArray, anArgs));
		}
	return iPackager->UnpackDesCArrayLC();
	}


