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
#include "ccnteventmsghandler.h"
#include "ccnteventqueue.h"
#include "ccntipccodes.h"
#include "ccntserver.h"

const TInt KCntEventIpcCodes[] =
	{
	ECntRequestEvent,
	ECntCancelEventRequest
	};

CCntEventMsgHandler* CCntEventMsgHandler::NewLC(CCntSession& aSession)
	{
	CCntEventMsgHandler* CntEventMsgHandler = new (ELeave) CCntEventMsgHandler(aSession);
    CleanupStack::PushL(CntEventMsgHandler);	
    CntEventMsgHandler->ConstructL();	
	return CntEventMsgHandler;
	}
	

CCntEventMsgHandler::CCntEventMsgHandler(CCntSession& aSession)
:CCntMsgHandler(aSession)
	{	
	}
	
void CCntEventMsgHandler::ConstructL()
	{
	iEventQueue = new (ELeave) CEventQueue;
	}
	
CCntEventMsgHandler::~CCntEventMsgHandler()
	{
	delete iEventQueue;
	}
	
/**
Delegates the incoming op code to a message handling method.

First checks if this class services the op code, it then uses the lookup table and finds 
function pointer(to message handling method) mapped to the incoming message function (op code)
and finally delegates the message to handling method.

It returns KErrNotFound if op code not handled.
*/
TInt CCntEventMsgHandler::HandleMessageL(const RMessage2& aMessage)
	{	
	MsgHandlerFptr func_ptr = LookupHandlerMethodL(aMessage.Function(), KCntEventIpcCodes, sizeof(KCntEventIpcCodes)/sizeof(TInt));
	
	if(func_ptr)
		{
		EventMsgHandlerFptr mem_func_ptr = static_cast<EventMsgHandlerFptr>(func_ptr);
		(this->*mem_func_ptr)(aMessage);
		return (KErrNone);
		}
	
	return (KErrNotFound);
	}
	
/**
Message handling methods.
*/
void CCntEventMsgHandler::RequestEvent(const RMessage2& aMessage)
	{
	iEventQueue->RequestEvent(aMessage);
	}
	
void CCntEventMsgHandler::CancelEventRequest(const RMessage2& aMessage)
	{
	// Complete the asynchronous request sent by the client-side
	// database event consumer (the CCntDbNotifyMonitor class).
	iEventQueue->CancelEventRequest();
	// Complete the cancelling request (sent to server when we got
	// here).
	aMessage.Complete(KErrNone);
	}
	
void CCntEventMsgHandler::QueueEvent(const TContactDbObserverEventV2 aEvent)
	{
	iEventQueue->QueueEvent(aEvent);
	}

