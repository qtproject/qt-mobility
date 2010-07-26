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

#include "ccntsession.h"
#include "ccntdbmanagercontroller.h"
#include "ccntstatemachine.h"
#include "ccntbackuprestoreagent.h"
#include "ccntpackager.h"
#include "cviewsubsessions.h"
#include "ccntipccodes.h"
#include "ccntrequest.h"
#include "ccnteventqueue.h"
#include <cntdef.h>  // for tcontactitemid.
#include "cinifilemanager.h"
#include "cntspeeddials.h"
#include "cntviewprivate.h"
#include <cntviewstore.h>
#include "ccntlogger.h"
#include "ccntmsghandler.h"
#include "ccntitemmsghandler.h"
#include "ccnteventmsghandler.h"
#include "ccnttransactionmsghandler.h"
#include "ccntfilemanagermsghandler.h"
#include "ccntpropertiesmsghandler.h"
#include "ccntviewmsghandler.h"


#define KPhoneBookServerUid 0x102030A1  //Pbk2 server secureid

extern void DebugLogIPC(const TDesC& aMethod, TInt aOpCode, TUint aSessionId, TInt aErrCode);


CCntSession::CCntSession(CCntPackager& aPackager, TUint aSessionId)
	: iPackager(aPackager),
	iTimeOut(KOneSecond),
	iSessionId(aSessionId)
	{
	}

	
CCntSession* CCntSession::NewL(CCntPackager& aPackager, TUint aSessionId)
	{
	CCntSession* CntSession = new (ELeave) CCntSession(aPackager, aSessionId);
	CleanupStack::PushL(CntSession);
	CntSession->ConstructL();
	CleanupStack::Pop(CntSession);
	return CntSession;
	}
	
	
void CCntSession::ConstructL()
	{
	CContactItemViewDef* itemDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EIncludeHiddenFields);
	itemDef->AddL(KUidContactFieldMatchAll);
	iView = CContactViewDef::NewL(itemDef);
	CleanupStack::Pop(itemDef);
	
	// Message handlers
    CCntItemMsgHandler* itemMsgHandler = CCntItemMsgHandler::NewLC(*this);
	CCntEventMsgHandler* eventMsgHandler = CCntEventMsgHandler::NewLC(*this);
	CCntTransactionMsgHandler* transMsgHandler = CCntTransactionMsgHandler::NewLC(*this);
	CCntFileManagerMsgHandler* fileMsgHandler = CCntFileManagerMsgHandler::NewLC(*this);
	CCntPropertiesMsgHandler* propMsgHandler = CCntPropertiesMsgHandler::NewLC(*this);
	CCntViewMsgHandler* viewMsgHandler = CCntViewMsgHandler::NewLC(*this);
	
	/** 
	iMsgHandlerArray takes ownership of Message Handlers.
	*/	
	iMsgHandlerArray.InsertL(itemMsgHandler, ECntItemMsgHandler);
	iMsgHandlerArray.InsertL(eventMsgHandler, ECntEventMsgHandler);
	iMsgHandlerArray.InsertL(transMsgHandler, ECntTransactionMsgHandler);
	iMsgHandlerArray.InsertL(fileMsgHandler, ECntFileManagerMsgHandler);
	iMsgHandlerArray.InsertL(propMsgHandler, ECntPropertiesMsgHandler);
	iMsgHandlerArray.InsertL(viewMsgHandler, ECntViewMsgHandler);
	
	// Pop Message handlers on CleanupStack
	CleanupStack::Pop(6, itemMsgHandler);
	}


/**
Maps to RSessionBase::Close() called on client-side.
*/
CCntSession::~CCntSession()
	{
	if(iManager) // CCntDbManager instance only allocated on open/replace/create
		{
		// Unlock any contact items that have not been explicity unlocked by this
		// session prior to this point.
		TRAP_IGNORE(SessionUnlockL());
		iManager->GetPersistenceLayer().ContactProperties().SessionDeleted(iSessionId);
		
		// Un-register for database event notifications.
		iManager->UnRegisterDatabaseEventObserver(*this);

		if (CSession2::Server())
			{
			// Call the Database Manager Controller to close the database and
			// destroy the CCntDbManager instance associated with this session.
			Server().Controller().CloseDatabase(*iManager);
			}

		}

 	delete iView;
 	
	iMsgHandlerArray.ResetAndDestroy();
	iMsgHandlerArray.Close();

	if (CSession2::Server())
		{
		Server().SessionClosing();
		}
	}


void CCntSession::SessionUnlockL()
	{
	CReqInternalSessionUnlock* request = CReqInternalSessionUnlock::NewLC(iSessionId);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}


/**
Top level client session function code parser. Delegates to message handlers,
if the op code is not handled by the 1st message handler, it is delegated to the 2nd
message handler.  If the op code is not handled by the 2nd message handler, it
delegates to the 3rd and so on.

The delegation to message handlers is prioritised. This results in quick handling of critical
operations.

Message handlers return KErrNotFound if op code is not handled. If KErrNotFound then op code is
delegated to the next handler. The last message handler leaves with KErrNotFound if op code not
handled. The leave is caught in the ServiceError() method.
*/
void CCntSession::ServiceL(const RMessage2& aMessage)
    {
    #if defined(__PROFILE_DEBUG__)
        RDebug::Print(_L("[CNTMODEL] MTD: CCntSession::ServiceL"));
    #endif 
        
    // The most likely operation is a ContactItem CRUD operation.
    
     DEBUG_PRINT4(__VERBOSE_DEBUG__,_L("[CNTMODEL] IPC"), aMessage.Function(), iSessionId, KErrNone);

    TSecureId ClientSID = aMessage.SecureId();
    if (ClientSID == KPhoneBookServerUid && iManager)
        {
        iManager->GetPersistenceLayer().ContactProperties().DisableSynchroniser(iSessionId);            
        }
        
    // Delegate to message handlers.
    for (TInt index = 0; index < iMsgHandlerArray.Count(); index++) 
        {
        TInt retCode = iMsgHandlerArray[index]->HandleMessageL(aMessage);
        
        /** Message handled or if message handler returned KErrNotFound,
        delegate to next handler.
        */
        if(retCode == KErrNone)
            {
            break;
            }
        }
    }

/**
We just complete the message with the error code.
*/
void CCntSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	
	DEBUG_PRINT4(__VERBOSE_DEBUG__,_L("[CNTMODEL] IPC_ERROR"), aMessage.Function(), iSessionId, aError);

	aMessage.Complete(aError);
	}

	
/**
Process a database event.  At this level we queue the event for consumption by
the client.
*/
void CCntSession::HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent)
	{
	/**
	Delegate to CntEventMsgHandler.
	*/	
	CCntEventMsgHandler* eventMsgHandler = static_cast<CCntEventMsgHandler*>(iMsgHandlerArray[ECntEventMsgHandler]);
	eventMsgHandler->QueueEvent(aEvent);
	}


void CCntSession::UnRegisterDatabaseEventObserver()
	{
	// This session is not going to be interested in database event
	// notifications so un-register it.
	
	if(iManager == NULL) 
		{
		User::Leave(KErrNotReady);
		}
		
	iManager->UnRegisterDatabaseEventObserver(*this);
	}

