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
#include "ccntviewmsghandler.h"

#include "ccntrequest.h"
#include "ccntserver.h"
#include "ccntpackager.h"
#include "ccntipccodes.h"
#include "cviewsubsessions.h"

const TInt KCntViewIpcCodes[] =
	{
	ECntItemAtL,
	ECntReadContactTextDef,
	ECntTextField, 
	ECntOpenViewSession,
	ECntCloseViewSession,
	ECntViewChangeSortOrderL,
	ECntViewBeginIterate,
	ECntViewEndIterate,
	ECntViewNextItemL,
	ECntCreateView,
	ECntCreateNamedView,
	ECntCloseView,
	ECntMatchesHintField
	};
	
CCntViewMsgHandler* CCntViewMsgHandler::NewLC(CCntSession& aSession)
	{
	CCntViewMsgHandler* self = new (ELeave) CCntViewMsgHandler(aSession);
	CleanupStack::PushL(self);
	self->ConstructorL();
	return self;
	}

void CCntViewMsgHandler::ConstructorL()
	{
	iSubSessions=CObjectIx::NewL();
	}

CCntViewMsgHandler::CCntViewMsgHandler(CCntSession& aSession)
:CCntMsgHandler(aSession)
	{		
	}
	
CCntViewMsgHandler::~CCntViewMsgHandler()
	{
	delete iSubSessions;
	
	/* Remove container from server. CServer2 instance which created session object exists! **/
	if(iContainer)
		{
		Server().RemoveObjectContainer(*iContainer);
		}
	}
	
/**
Delegates the incoming op code to a message handling method.

First checks if this class services the op code, it then uses the lookup table and finds 
function pointer(to message handling method) mapped to the incoming message function (op code)
and finally delegates the message to handling method.

It leaves with KErrNotFound if op code not handled.
*/
TInt CCntViewMsgHandler::HandleMessageL(const RMessage2& aMessage)
	{
	MsgHandlerFptr func_ptr = LookupHandlerMethodL(aMessage.Function(), KCntViewIpcCodes, sizeof(KCntViewIpcCodes)/sizeof(TInt));
	
	if(func_ptr)
		{
		ViewMsgHandlerFptr mem_func_ptr = static_cast<ViewMsgHandlerFptr>(func_ptr);
		(this->*mem_func_ptr)(aMessage);
		return (KErrNone);
		}
		
	/** Other opcodes processed by view subsession ServiceL() method.
	View subsession ServiceL() leaves with KErrNotFound if opcode not consumed.
	*/
	ViewSubSessionServiceL(aMessage);
	return (KErrNone);
	}
	
void CCntViewMsgHandler::CreateViewSubSessionL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	TObjectCleanup* cleanupData=new(ELeave) TObjectCleanup();
	CleanupStack::PushL(TCleanupItem(TObjectCleanup::Cleanup,cleanupData));
	CViewSubSession* subSession=CViewSubSession::NewL(iManager->ViewManagerL(),aMessage);
	cleanupData->iObject=subSession;
	
	if(iContainer == NULL)
 		{
 		//Have to get new container after contruction, as CSession2::Server() always
 		//returns empty pointer in contructor.
 		iContainer = Server().NewContainerL();
 		}		
	iContainer->AddL(subSession);
	TInt handle=iSubSessions->AddL(subSession);
	cleanupData->iIndex=iSubSessions;
	cleanupData->iHandle=handle;
	TPckg<TInt> handlePckg(handle);
	aMessage.WriteL(3,handlePckg);
	CleanupStack::Pop(); // The TCleanupItem.
	delete cleanupData;
	}
	
void CCntViewMsgHandler::CreateNamedViewSubSessionL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	TObjectCleanup* cleanupData=new(ELeave) TObjectCleanup();
	CleanupStack::PushL(TCleanupItem(TObjectCleanup::Cleanup,cleanupData));
	CNamedViewSubSession* subSession=CNamedViewSubSession::NewL(iManager->ViewManagerL(),aMessage);
	cleanupData->iObject=subSession;
	if(iContainer == NULL)
 		{
 		//Have to get new container after contruction, as CSession2::Server() always
 		//returns empty pointer in contructor.
 		iContainer = Server().NewContainerL();
 		}		
	iContainer->AddL(subSession);
	TInt handle=iSubSessions->AddL(subSession);
	cleanupData->iIndex=iSubSessions;
	cleanupData->iHandle=handle;
	TPckg<TInt> handlePckg(handle);
	aMessage.WriteL(3,handlePckg);
	CleanupStack::Pop(); // The TCleanupItem.
	delete cleanupData;
	}
	
void CCntViewMsgHandler::CloseViewSubSessionL(const RMessage2 &aMessage)
	{
	const TInt handle=aMessage.Int3();
	ViewFromHandleL(handle); // To panic client if passed a bad handle.
	iSubSessions->Remove(handle);
	}
	
CViewSubSessionBase& CCntViewMsgHandler::ViewFromHandleL(TUint aHandle)
    {
	CViewSubSessionBase* subSession = STATIC_CAST(CViewSubSessionBase*,iSubSessions->At(aHandle));
	if (subSession==NULL)
		{
		User::Leave(KErrBadHandle); // Will result in client getting panic'd.
		}
	return *subSession;
    }
   
 void CCntViewMsgHandler::CompleteMessage(TInt aRet, const RMessage2& aMessage)
 	{
 	if(aRet != KErrNoComplete)
		{
		aMessage.Complete(aRet);
		}
 	}

/**
Message handling methods.
*/
void CCntViewMsgHandler::ReadContactTextDefL(const RMessage2& aMessage)
	{
	TInt ret(KErrNone);
	CheckForManagerL();
	const TInt receivingBufLen = aMessage.GetDesMaxLength(1);
	// We cannot return anything, so we can complete the request
	// immediately.
	if (receivingBufLen == 0)
		{
		aMessage.Complete(ret);
		return;
		}
	// Read CContactTextDef from packager buffer.
	iPackager.SetBufferFromMessageL(aMessage);
	// Packager reads from message slot 0.
	CContactTextDef* textDef = iPackager.UnpackCntTextDefLC();
	
	// Get the iterator manager reference from Persistence Layer.
	MLplViewIteratorManager& iterManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();

	HBufC* buf = HBufC::NewLC(receivingBufLen);
	TPtr ptr(const_cast<TUint16*>(buf->Ptr()), receivingBufLen);
	
    TInt contactId = aMessage.Int2();	
	TUid typeUid = iterManager.ReadContactTextDefL(contactId, ptr, *textDef);
	if(typeUid == KUidContactICCEntry)
		{
		MLplPersistenceLayerFactory& factory = iManager->GetPersistenceLayer().FactoryL();
		User::LeaveIfError(factory.GetContactSynchroniserL(iSessionId).ValidateContact(MContactSynchroniser::ERead, contactId));
		}

	aMessage.WriteL(1,ptr);
	CleanupStack::PopAndDestroy(2, textDef); //buf
	aMessage.Complete(ret);
	}

void CCntViewMsgHandler::TextFieldL(const RMessage2& aMessage)
	{
	TInt ret(KErrNone);
	CheckForManagerL();
	MLplViewIteratorManager& iterManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();
	TPckgBuf<TFieldType> package;
	aMessage.ReadL(1, package);
	TBuf<64> field;
	iterManager.TextFieldL(aMessage.Int0(),package(),field);
	aMessage.WriteL(2,field);
	aMessage.Complete(ret);
	}

void CCntViewMsgHandler::ValidateViewContactL(const CViewContact& aViewContact, TInt aSessionId)
	{
	// If the contact is an ICC entry (held on the phone's SIM), ensure we have
	// read permission.
	if(aViewContact.ContactTypeUid() == KUidContactICCEntry)
		{
		MLplPersistenceLayerFactory& factory = iManager->GetPersistenceLayer().FactoryL();
		User::LeaveIfError(factory.GetContactSynchroniserL(aSessionId).ValidateContact(MContactSynchroniser::ERead, aViewContact.Id()));
		}
	}

/* 
Maps to RCntModel::ItemAtL() method.  Use real Persistence Layer this side. 
Call the Persistence Layer view item manager directly. 
**/
void CCntViewMsgHandler::ItemAtL(const RMessage2& aMessage)
	{
	TInt ret(KErrNone);
	CheckForManagerL();
	
	// Get the pl view item manager reference from persistence layer
	MLplViewIteratorManager& viewManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();			
	
	// Persistence layer call
	CViewContact* viewContact = viewManager.ItemAtL(aMessage.Int0(), aMessage.Int1());
	// Don't write NULL back across IPC.
	// Leave KErrNotFound will be picked up by client and propagated as NULL to caller
	if(viewContact == NULL)
		{
		User::Leave(KErrNotFound);
		}
	
	CleanupStack::PushL(viewContact);

	ValidateViewContactL(*viewContact, iSessionId);
	
	// Compare with maximum length of descriptor argument in the client's process.
	if(viewContact->ExternalizedSize() > aMessage.GetDesMaxLength(2) )
		{
		// Return required size of buffer to the client.
		aMessage.Complete(viewContact->ExternalizedSize());
		ret = KErrNoComplete;
		}
	else
		{
		// Packager's buffer large enough so write back CViewContact.
		TPtr8 ptr = iPackager.PackL(*viewContact);
		aMessage.WriteL(2,ptr);
		}
		
	CleanupStack::PopAndDestroy(viewContact);
	CompleteMessage(ret, aMessage);	
	}
	
/* 
Maps to RCntModel::OpenViewL() method.  Use real Persistence Layer this side. 
Call the Persistence Layer view item manager directly. 
**/
void CCntViewMsgHandler::OpenViewSessionL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	
	// Read CContactTextDef from packager buffer
	iPackager.SetBufferFromMessageL(aMessage);
	// Packager reads from message slot 0
	CContactTextDef* textDef = iPackager.UnpackCntTextDefLC();
	
	// Get the pl view item manager reference from persistence layer
	MLplViewIteratorManager& viewManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();			
	
	// Persistence layer call
	TInt viewSessionId = viewManager.OpenViewL(*textDef, static_cast<TContactViewPreferences>(aMessage.Int1()));
	CleanupStack::PopAndDestroy(textDef);

	TPckg<TInt> idPckg(viewSessionId);
	aMessage.WriteL(2,idPckg);
	
	aMessage.Complete(KErrNone);	
	}
	
/* 
Maps to RCntModel::CloseView() method.  Use real Persistence Layer this side. 
Call the Persistence Layer view item manager directly. 
**/
void CCntViewMsgHandler::CloseViewSessionL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	
	// Get the pl view item manager reference from persistence layer
	MLplViewIteratorManager& viewManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();			
	
	// Persistence layer call
	viewManager.CloseView(aMessage.Int0());
	aMessage.Complete(KErrNone);	
	}
	
void CCntViewMsgHandler::ChangeSortOrderL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	
	// Read CContactTextDef from packager buffer, packager reads from message slot 1
	iPackager.SetBufferFromMessageL(aMessage, 1);
	CContactTextDef* textDef = iPackager.UnpackCntTextDefLC();
	
	// Get the pl view item manager reference from persistence layer
	MLplViewIteratorManager& viewManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();			
	
	// Persistence layer call
	viewManager.ChangeSortOrderL(aMessage.Int0(), *textDef);
	CleanupStack::PopAndDestroy(textDef);

	aMessage.Complete(KErrNone);	
	}
	
void CCntViewMsgHandler::BeginIterateL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	
	// Get the pl view item manager reference from persistence layer
	MLplViewIteratorManager& viewManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();			
	viewManager.BeginIterateL(aMessage.Int0());
	aMessage.Complete(KErrNone);	
	}
	
void CCntViewMsgHandler::EndIterateL(const RMessage2& aMessage)
	{
	CheckForManagerL();
	
	// Get the pl view item manager reference from persistence layer
	MLplViewIteratorManager& viewManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();			
	viewManager.EndIterateL(aMessage.Int0());
	aMessage.Complete(KErrNone);	
	}
	
void CCntViewMsgHandler::NextItemL(const RMessage2& aMessage)
	{
	TInt ret(KErrNone);
	CheckForManagerL();
	
	// Get the pl view item manager reference from persistence layer
	MLplViewIteratorManager& viewManager = iManager->GetPersistenceLayer().FactoryL().GetViewIteratorManagerL();			
	
	// Persistence layer call
	CViewContact* viewContact = viewManager.NextItemL(aMessage.Int0(), static_cast<TContactViewPreferences>(aMessage.Int1()));
	
	// Don't write NULL back across IPC.
	// Leave KErrNotFound will be picked up by client and propagated as NULL to caller
	if(viewContact == NULL)
		{
		User::Leave(KErrNotFound);
		}
	
	CleanupStack::PushL(viewContact);
	
	ValidateViewContactL(*viewContact, iSessionId);
	
	// Compare with maximum length of descriptor argument in the client's process.
	if(viewContact->ExternalizedSize() > aMessage.GetDesMaxLength(2) )
		{
		// Return required size of buffer to the client.
		aMessage.Complete(viewContact->ExternalizedSize());
		ret = KErrNoComplete;
		}
	else
		{
		// Packager's buffer large enough so write back CViewContact.
		TPtr8 ptr = iPackager.PackL(*viewContact);
		aMessage.WriteL(2,ptr);
		}
		
	CleanupStack::PopAndDestroy(viewContact);
	CompleteMessage(ret, aMessage);	
	}	

void CCntViewMsgHandler::CreateViewL(const RMessage2& aMessage)
	{
	// Subsession create anonymous view from client.  Create subsession
	// objects on demand.
	TInt ret(KErrNone);
	CreateViewSubSessionL(aMessage);
	aMessage.Complete(ret);
	}
	
void CCntViewMsgHandler::CreateNamedViewL(const RMessage2& aMessage)
	{
	// Subsession create named view from client.  Create subsession
	// objects on demand.
	TInt ret(KErrNone);
	CreateNamedViewSubSessionL(aMessage);
	aMessage.Complete(ret);
	}
	
void CCntViewMsgHandler::CloseViewL(const RMessage2& aMessage)
	{
	// Subsession close view from client.
	
	TInt ret(KErrNone);
	CloseViewSubSessionL(aMessage);
	aMessage.Complete(ret);
	}
	
void CCntViewMsgHandler::MatchesHintFieldL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::ContactMatchesHintFieldL().
	
	TInt ret(KErrNone);
	CheckForManagerL();
	TInt bitWiseFilter   = aMessage.Int0();
	TContactItemId cntId = aMessage.Int1();
	ret = iManager->GetPersistenceLayer().FactoryL().GetCollectorL().ContactMatchesHintFieldL(bitWiseFilter, cntId);
	CompleteMessage(ret, aMessage);
	}
	
void CCntViewMsgHandler::ViewSubSessionServiceL(const RMessage2& aMessage)
	{
	// Other opcodes processed by view subsession ServiceL() method.
	
	TInt ret(KErrNone);
	ret = ViewFromHandleL(aMessage.Int3()).ServiceL(aMessage);
	CompleteMessage(ret, aMessage);
	}

