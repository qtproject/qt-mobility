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
#include "ccntitemmsghandler.h"

#include "ccntipccodes.h"
#include "ccntrequest.h"
#include "ccntdbmanager.h"
#include "ccntstatemachine.h"
#include "ccntdbmanagercontroller.h"
#include "ccntbackuprestoreagent.h"
#include "ccntpackager.h"
#include "cntspeeddials.h"

const TInt KCntItemIpcCodes[] =
	{
	ECntItemCreate,
	ECntItemUpdate,
	ECntItemDelete,
	ECntItemRead,
	ECntItemCommit,
	ECntItemOpen,
	ECntItemClose,
	ECntConnectionId,
	ECntGetCurrentItem,
	ECntRemoveCurrentItem,
	ECntSetCurrentItem,
	ECntGetCurrentDb,
	ECntSetCurrentDb,
	ECntGetSpeedDialContactIdAndPhoneNumber,
	ECntSetSpeedDialIdForPosition,
	ECntSetOwnCard,
	ECntGetOwnCard,
	ECntGetCollection,
	ECntSetSortPrefs,
	ECntGetSortPrefs,
	ECntSetDbViewContactType,
	ECntGetDbViewContactType,
	ECntDbContactCount,
	ECntFindAsync,
	ECntFindAsyncInit,
	ECntFindAsyncTextDefInit,
	ECntFind,
	ECntFilterDatabase,
	ECntSetAsyncActivity,
	ECntResourceCount,
	ECntSetHeapFailure,
	ECntSeekContactInCollection
	};

CCntItemMsgHandler* CCntItemMsgHandler::NewLC(CCntSession& aSession)
	{	
	CCntItemMsgHandler* CntItemMsgHandler = new (ELeave) CCntItemMsgHandler(aSession);
	CleanupStack::PushL(CntItemMsgHandler);
	return CntItemMsgHandler;
	}	

CCntItemMsgHandler::CCntItemMsgHandler(CCntSession& aSession)
:CCntMsgHandler(aSession)
	{	
	}
	
CCntItemMsgHandler::~CCntItemMsgHandler()
	{
	}
	
/**
Delegates the incoming op code to a message handling method.

First checks if this class services the op code, it then uses the lookup table and finds 
function pointer(to message handling method) mapped to the incoming message function (op code)
and finally delegates the message to handling method.

It returns KErrNotFound if op code not handled.
*/
TInt CCntItemMsgHandler::HandleMessageL(const RMessage2& aMessage)
	{	
	MsgHandlerFptr func_ptr = LookupHandlerMethodL(aMessage.Function(), KCntItemIpcCodes, sizeof(KCntItemIpcCodes)/sizeof(TInt));
	
	if(func_ptr)
		{
		ItemMsgHandlerFptr mem_func_ptr = static_cast<ItemMsgHandlerFptr>(func_ptr);
		(this->*mem_func_ptr)(aMessage);
		return (KErrNone);
		}
	
	return (KErrNotFound);
	}
	
/**
Message handling methods.

N.B.: Some operations use aMessage.Complete() to return the a contact ID.  This
is not convention but saves on IPC overhead.  Only one parameter is returned as
opposed to two (error code and contact ID).

@param aMessage The message typically containing the Contact Item.
*/
void CCntItemMsgHandler::ItemCreateL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqCreateCnt* request = CReqCreateCnt::NewLC(iSessionId, aMessage, iTimeOut, iPackager);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::ItemUpdateL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqUpdateCnt* request = CReqUpdateCnt::NewLC(iSessionId, aMessage, iTimeOut, iPackager);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::ItemDeleteL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqDeleteCnt* request = CReqDeleteCnt::NewLC(iSessionId, aMessage, iTimeOut);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::ItemReadL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqReadCnt* request = CReqReadCnt::NewLC(iSessionId, aMessage, iTimeOut, iPackager, iView->ItemDef());
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::ItemCommitL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqCommitCnt* request = CReqCommitCnt::NewLC(iSessionId, aMessage, iTimeOut, iPackager);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::ItemOpenL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqOpenCnt* request = CReqOpenCnt::NewLC(iSessionId, aMessage, iTimeOut, iPackager, iView->ItemDef());
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::ItemCloseL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqCloseCnt* request = CReqCloseCnt::NewLC(iSessionId, aMessage, iTimeOut);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}

void CCntItemMsgHandler::ConnectionId(const RMessage2& aMessage)
	{
	// Complete the request with the session ID.
	aMessage.Complete(iSessionId);
	}
	
void CCntItemMsgHandler::GetCurrentItemL(const RMessage2& aMessage)
	{
	CheckForManagerL();			
	TContactItemId currentItem(iManager->CurrentItem());
 	// Complete the request with the contact ID of the current item.
 	aMessage.Complete(currentItem);
	}
	
void CCntItemMsgHandler::RemoveCurrentItemL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	iManager->RemoveCurrentItemL(iSessionId);
	aMessage.Complete(KErrNone);
	}
	
void CCntItemMsgHandler::SetCurrentItemL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	TContactItemId newCurrentItem = aMessage.Int0();
	iManager->SetCurrentItemL(newCurrentItem, iSessionId);
	aMessage.Complete(KErrNone);
	}

void CCntItemMsgHandler::GetCurrentDb(const RMessage2& aMessage)
	{
	WriteL(aMessage, KSlot0, Server().Controller().CurrentDb());
	aMessage.Complete(KErrNone);
	}
	
void CCntItemMsgHandler::SetCurrentDbL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	// Slot 0 in aMessage contains the new current database.
	TFileName newCurrentDb;
	ReadL(aMessage, KSlot0, newCurrentDb);
	// Is this database already open for this session? We make it a
	// pre-condition that you cannot set the current database to be one
	// that isn't already open by this client.  This prevents errors
	// whereby a client updates the current database, but then fails to
	// open it.
	if	(newCurrentDb.CompareF(iManager->CntFile()) == 0)
		{
		Server().Controller().SetCurrentDbL(newCurrentDb, iSessionId);				
		}
	aMessage.Complete(KErrNone);
	}
	
void CCntItemMsgHandler::GetSpeedDialContactIdAndPhoneNumberL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	const TInt speedDialIndex = aMessage.Int0();		
	TSpeedDialPhoneNumber phoneNumber;
	TContactItemId contactId;
	iManager->GetSpeedDialContactIdAndPhoneNumberL(speedDialIndex, phoneNumber, contactId);
	aMessage.WriteL(KSlot1, phoneNumber);
	aMessage.Complete(contactId);
	}
	
void CCntItemMsgHandler::SetSpeedDialIdForPositionL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	// fetch the old contacts ID
	const CCntServerSpeedDialTable& table = iManager->IniFileManager().SpeedDialManager().TableL(iManager->CntFile());
	CReqSetSpeedDial* request = CReqSetSpeedDial::NewLC(iSessionId, 
														aMessage, 
														iTimeOut,
														iView->ItemDef(),
														iManager->IniFileManager().SpeedDialManager(),
														table,
														*iManager);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::SetOwnCardL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	CReqSetOwnCard* request = CReqSetOwnCard::NewLC(iSessionId, 
													aMessage, 
													iTimeOut,
													iPackager,
													iView->ItemDef(),
													*iManager);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::GetOwnCardL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	aMessage.Complete(iManager->GetPersistenceLayer().ContactProperties().OwnCardIdL());
	}
	
void CCntItemMsgHandler::GetCollectionL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	
	// If backup or restore is in progress then database cannot be accessed.
	CCntBackupRestoreAgent& burAgent = iManager->BackupRestoreAgent();
	if(burAgent.BackupInProgress() || burAgent.RestoreInProgress())
		{
		User::Leave(KErrLocked);
		}

	MLplCollection& collection = iManager->GetPersistenceLayer().FactoryL().GetCollectorL();
	MLplCollection::TLplViewType type = static_cast<MLplCollection::TLplViewType>(aMessage.Int1());
	CContactIdArray* idArray = NULL;

	switch(type)
		{
		case MLplCollection::EChangedSince :
			{
			TTime time(0);
			TPckgBuf<TTime> buf(time);
			aMessage.ReadL(2,buf);
			time = buf();
			idArray = collection.CollectionL(type,time,KNullDesC);
			}
			break;

		case MLplCollection::EFindGuid :
			{
			TTime time(0);
			TBuf<64> buf;
			aMessage.ReadL(2,buf);
			idArray = collection.CollectionL(type,time,buf);
			}
			break;

		// Maps to RCntModel::MatchPhoneNumberL().
		case MLplCollection::EMatchPhoneNos:
			{
			const TInt receivingBufLen = aMessage.GetDesLengthL(2);
			HBufC* buf = HBufC::NewLC(receivingBufLen); // digits in the number
			TPtr ptr(const_cast<TUint16*>(buf->Ptr()), receivingBufLen);
			aMessage.ReadL(2, ptr);
			idArray = collection.MatchPhoneNumberL(ptr, aMessage.Int3());
			CleanupStack::PopAndDestroy(buf);
			}
			break;	

		default :
			{
			idArray = collection.CollectionL(type);
			}
			break;
		}
	
	CleanupStack::PushL(idArray);
	TPtr8 ptr = iPackager.PackL(*idArray);
	TInt length = ptr.Length();
	
	if(aMessage.GetDesMaxLength(0) >= length)
		{
		aMessage.WriteL(0,ptr); 
		aMessage.Complete(KErrNone);
		}
	else
		{
		aMessage.Complete(length);
		}
	
	CleanupStack::PopAndDestroy(idArray);
	}
	
void CCntItemMsgHandler::SetSortPrefsL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	iPackager.SetBufferFromMessageL(aMessage);
	CArrayFix<CContactDatabase::TSortPref>* prefs = iPackager.UnpackCArrayFixLC();
	iManager->GetPersistenceLayer().ContactProperties().SetSortPrefsL(prefs);
	CleanupStack::Pop(prefs);
	aMessage.Complete(KErrNone);
	}
	
void CCntItemMsgHandler::GetSortPrefsL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	const CArrayFix<CContactDatabase::TSortPref>& prefs = iManager->GetPersistenceLayer().ContactProperties().SortPrefsL();
	TPtr8 ptr = iPackager.PackL(prefs);
	TInt length = ptr.Length();
	if(aMessage.GetDesMaxLength(0) >= length)
		{
		aMessage.WriteL(0,ptr); 
		aMessage.Complete(KErrNone);
		}
	else
		{
		aMessage.Complete(length);
		}
	}
	
void CCntItemMsgHandler::SetDbViewContactTypeL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	iManager->GetPersistenceLayer().ContactProperties().SetDbViewContactType(TUid::Uid(aMessage.Int0()));
	aMessage.Complete(KErrNone);
	}
	
void CCntItemMsgHandler::GetDbViewContactTypeL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	TPckg<TUid> dbViewContactType(iManager->GetPersistenceLayer().ContactProperties().GetDbViewContactType());
	aMessage.WriteL(0,dbViewContactType);
	aMessage.Complete(KErrNone);
	}
	
void CCntItemMsgHandler::DbContactCountL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	MLplCollection& collection = iManager->GetPersistenceLayer().FactoryL().GetCollectorL();
	aMessage.Complete(collection.ContactCountL());
	}
	
void CCntItemMsgHandler::FindAsyncL(const RMessage2& aMessage)
	{
	/* Maps to client side AsyncFindL().  Can and will be called iteratively by the 
	client. One of the two intitialisations below (i.e. ECntFindAsyncInit or 
	ECntFindAsyncTextDefInit will have been called prior to this call.
	**/
	
	CheckForManagerL();
			
	TBool moreToGo(EFalse);
	
	// Call Persistence Layer to do the work.  moreToGo will be set to
	// ETrue if client should call again.  CContactIdArray and TBool are
	// return parameters to the client.
	MLplCollection& collection = iManager->GetPersistenceLayer().FactoryL().GetCollectorL();
	CContactIdArray* idArray = collection.FindAsyncL(moreToGo, iSessionId);
	CleanupStack::PushL(idArray);
	
	TPckg<TBool> morePckg(moreToGo);
	aMessage.WriteL(1,morePckg);
	
	TPtr8 ptr8 = iPackager.PackL(*idArray);
	// Iterative calls rely on packager having large enough buffer.
	// Client expands it to 4k before calling.
	// Write back to client
	aMessage.WriteL(0,ptr8);
	
	CleanupStack::PopAndDestroy(idArray); 
	aMessage.Complete(KErrNone);
	}
	
//
void CCntItemMsgHandler::FindAsyncInitL(const RMessage2& aMessage)
	{
	/* Asynchronous find can use 2 types of initialisation which map to the 
	two construction types in the old Contacts Model. One uses a CContactTextDef 
	and CDesCArray and the other a CContactItemFieldDef and text descriptor. 
	Once initialisation is complete the client-side CIdleFinder::doFindL() method 
	calls AsyncFind().

	Initialisation for asynchronous find using CContactItemFileldDef and TDesC.
	**/
	
	CheckForManagerL();

	// Read descriptor into HBuf.
	HBufC* text = HBufC::NewLC(aMessage.GetDesLengthL(0));
	TPtr ptr(text->Des());
	aMessage.ReadL(0,ptr);

	CContactItemFieldDef* fieldDef = NULL;
	if(aMessage.GetDesLengthL(1) > 0)
		{
		// Use packager to unpack CContactItemFieldDef.
		iPackager.SetBufferFromMessageL(aMessage,1);
		fieldDef = static_cast<CContactItemFieldDef*> (iPackager.UnpackTUidArrayLC());
		CleanupStack::Pop(fieldDef);
		}
	
	// Persistence Layer does the actual initialisation and also takes
	// ownership of fielDef.
	MLplCollection& collection = iManager->GetPersistenceLayer().FactoryL().GetCollectorL();
	collection.FindAsyncInitL(text->Des(),fieldDef);
	
	CleanupStack::PopAndDestroy(text);
	aMessage.Complete(KErrNone);
	}
	
void CCntItemMsgHandler::FindAsyncTextDefInitL(const RMessage2& aMessage)
	{
	/* Initialisation for asynchronous find using CContactTextDef and array of 
	"find words" constructed on the client-side and required on the server-side 
	for matching.
	**/
	
	CheckForManagerL();

	// CContactTextDef can be NULL in which case the client will have
	// passed an empty descriptor.
	CContactTextDef* textDef = NULL;
	// Assuming that a KNullDesC parameter results in max length of 0.
	if(aMessage.GetDesLengthL(1) > 0)
		{
		// Use packager to unpack CContactTextDef.
		iPackager.SetBufferFromMessageL(aMessage,1);
		textDef = iPackager.UnpackCntTextDefLC();
		CleanupStack::Pop(textDef);
		}

	// Unpack the CDesCArray manually as the packager does not package
	// both.			
	CBufFlat* bufFlat = CBufFlat::NewL(1 << 8);
	CleanupStack::PushL(bufFlat);
	bufFlat->ExpandL(0,aMessage.GetDesLengthL(0));
	TPtr8 ptr8(bufFlat->Ptr(0));
	aMessage.ReadL(0,ptr8);
	RBufReadStream readStream;
	readStream.Open(*bufFlat);
	TInt count = readStream.ReadInt32L();
	CDesCArray* unpacked = new (ELeave) CDesCArrayFlat(8);
	CleanupStack::PushL(unpacked);
	for(TInt i=0;i<count;++i)
		{
		TBuf<256> buf;
		TInt length = readStream.ReadInt32L();
		readStream.ReadL(buf,length);
		unpacked->AppendL(buf);
		}
	readStream.Close();
	
	// Persistence Layer does the actual initialisation and also takes
	// ownership of textDef.
	MLplCollection& collection = iManager->GetPersistenceLayer().FactoryL().GetCollectorL();
	collection.FindAsyncTextDefInitL(*unpacked,textDef);
	
	CleanupStack::PopAndDestroy(unpacked);
	CleanupStack::PopAndDestroy(bufFlat);
	aMessage.Complete(KErrNone);
	}
	
void CCntItemMsgHandler::SeekContactL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	MLplCollection& collection = iManager->GetPersistenceLayer().FactoryL().GetCollectorL();
	
	TContactItemId actualId;
	TUid contactType;
	TBool deleted(EFalse);
	// SeekContactL returns the Deleted Flag, the Contact ID and the
	// contact type.
	TBool seekRet = collection.SeekContactL(aMessage.Int0(),actualId,contactType,deleted);
	
	if(seekRet)
		{
		TPckg<TInt> idPckg(actualId);
		aMessage.WriteL(1,idPckg);
		TPckg<TUid> typePckg(contactType);
		aMessage.WriteL(2,typePckg);
		TPckg<TInt> deletePckg(deleted);
		aMessage.WriteL(3,deletePckg);				
		}
		
	aMessage.Complete(seekRet);	
	}
	
void CCntItemMsgHandler::FindL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::FindL() method.
	
	CheckForManagerL();
						
	// CContactItemFieldDef can be NULL in which case the client will
	// have passed an empty descriptor.
	CContactItemFieldDef* fieldDef = NULL;
	// Assuming that a KNullDesC parameter results in max length of 0.
	if(aMessage.GetDesLengthL(1) != 0)
		{
		// Slot 1 contains a serialized CArrayFixFlat<TUid>.  Since
		// CContactItemFieldDef derives from it we can use the packager
		// to unpack it.
		iPackager.SetBufferFromMessageL(aMessage,1);
		fieldDef = static_cast<CContactItemFieldDef*> (iPackager.UnpackTUidArrayLC());
		}
	
	// Get the search text from slot 2
	HBufC* text = HBufC::NewLC(aMessage.GetDesLengthL(2));
	TPtr ptr(text->Des());
	aMessage.ReadL(2,ptr);

	MLplCollection& collection = iManager->GetPersistenceLayer().FactoryL().GetCollectorL();
	collection.Reset();
	
	// Call the Persistence Layer to perform the find.
	CContactIdArray* idArray = collection.FindL(ptr,fieldDef, iSessionId);
	CleanupStack::PopAndDestroy(text);

	// Package the contact ID array for return.
	CleanupStack::PushL(idArray);
	TPtr8 ptr8 = iPackager.PackL(*idArray);
	TInt length = ptr8.Length();
	
	// Check the descriptor from the client is large enough.
	if(aMessage.GetDesMaxLength(0) >= length)
		{
		// Write back to client.
		aMessage.WriteL(0,ptr8); 
		aMessage.Complete(KErrNone);
		}
	else
		{
		// Return required size of buffer to the client.
		aMessage.Complete(length);
		}
	CleanupStack::PopAndDestroy(idArray);
	if(fieldDef)
		{
		CleanupStack::PopAndDestroy(fieldDef);
		}	
	}
	
void CCntItemMsgHandler::FilterDatabaseL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	iPackager.SetBufferFromMessageL(aMessage);
	CCntFilter* filter = iPackager.UnpackCntFilterLC();
	MLplCollection& collection = iManager->GetPersistenceLayer().FactoryL().GetCollectorL();
	CContactIdArray* idArray = collection.FilterDatabaseL(*filter);
	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PushL(idArray);
	TPtr8 ptr8 = iPackager.PackL(*idArray);
	TInt length = ptr8.Length();
	// Check the descriptor from the client is large enough.
	if(aMessage.GetDesMaxLength(0) >= length)
		{
		// Write back to client.
		aMessage.WriteL(0,ptr8); 
		aMessage.Complete(KErrNone);
		}
	else
		{
		// Return required size of buffer to the client.
		aMessage.Complete(length);
		}
	CleanupStack::PopAndDestroy(idArray);
	}
	
void CCntItemMsgHandler::SetAsyncActivityL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	TBool asyncActivity = TBool(aMessage.Int0());
	// Create and then process appropriate request using state machine.
	CCntRequest* request = NULL;
	if (asyncActivity)
		{
		request = CReqAsyncActivity::NewLC(iSessionId, aMessage, iTimeOut);
		}
	else
		{
		request = CReqNoAsyncActivity::NewLC(iSessionId, aMessage, iTimeOut);
		}
	iManager->StateMachineL().ProcessRequestL(request); // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntItemMsgHandler::ResourceCount(const RMessage2& aMessage)
	{
	#if defined(_DEBUG)
	TInt totalAllocSize(0);
	User::Heap().AllocSize(totalAllocSize);
	RDebug::Print(_L("[CNTMODEL] Heap count: %d cells, total heap size: %d bytes"), User::Heap().Count(), User::Heap().Size());
	RDebug::Print(_L("[CNTMODEL] cntsrv heap size: %d bytes"), totalAllocSize);
	aMessage.Complete(User::Heap().Count());
	#else
	aMessage.Complete(KErrNone);
	#endif
	}
	
void CCntItemMsgHandler::SetHeapFailure(const RMessage2& aMessage)
	{
	#if defined(_DEBUG)
	User::__DbgSetAllocFail(RHeap::EUser,(RHeap::TAllocFail)aMessage.Int0(),aMessage.Int1());
	#endif
	aMessage.Complete(KErrNone);
	}
	
