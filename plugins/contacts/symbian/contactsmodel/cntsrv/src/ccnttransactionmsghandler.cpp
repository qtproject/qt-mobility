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
#include "ccnttransactionmsghandler.h"

#include "ccntserver.h"
#include "ccntipccodes.h"
#include "ccntrequest.h"
#include "ccntdbmanager.h"
#include "ccntstatemachine.h"

const TInt KCntTransactionIpcCodes[] =
	{
	EBeginDbTransaction,
	EEndDbTransaction,
	ERollbackDbTransaction
	};

CCntTransactionMsgHandler* CCntTransactionMsgHandler::NewLC(CCntSession& aSession)
	{
	CCntTransactionMsgHandler* CntTransactionMsgHandler = new (ELeave) CCntTransactionMsgHandler(aSession);
	CleanupStack::PushL(CntTransactionMsgHandler);
	return CntTransactionMsgHandler;
	}
	

CCntTransactionMsgHandler::CCntTransactionMsgHandler(CCntSession& aSession)
:CCntMsgHandler(aSession)
	{		
	}
	
CCntTransactionMsgHandler::~CCntTransactionMsgHandler()
	{
	}
	
/**
Delegates the incoming op code to a message handling method.

First checks if this class services the op code, it then uses the lookup table and finds 
function pointer(to message handling method) mapped to the incoming message function (op code)
and finally delegates the message to handling method.

It returns KErrNotFound if op code not handled.
*/
TInt CCntTransactionMsgHandler::HandleMessageL(const RMessage2& aMessage)
	{	
	MsgHandlerFptr func_ptr = LookupHandlerMethodL(aMessage.Function(), KCntTransactionIpcCodes, sizeof(KCntTransactionIpcCodes)/sizeof(TInt));
	
	if(func_ptr)
		{
		TransactionMsgHandlerFptr mem_func_ptr = static_cast<TransactionMsgHandlerFptr>(func_ptr);
		(this->*mem_func_ptr)(aMessage);
		return (KErrNone);
		}
	
	return (KErrNotFound);
	}
	
/**
Message handling methods.
*/
void CCntTransactionMsgHandler::BeginDbTransactionL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::BeginDbTransaction().

	CheckForManagerL();
	CReqDbBeginTrans* request = CReqDbBeginTrans::NewLC(iSessionId, aMessage, iTimeOut);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);
	}
	
void CCntTransactionMsgHandler::EndDbTransactionL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::CommitDbTransaction().

	CheckForManagerL();
	CReqDbCommitTrans* request = CReqDbCommitTrans::NewLC(iSessionId, aMessage, iTimeOut);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.	
	CleanupStack::Pop(request);	
	}
	
void CCntTransactionMsgHandler::RollbackDbTransactionL(const RMessage2& aMessage)
	{
	// Maps to RCntModel::RollbackDbTransaction().

	CheckForManagerL();
	CReqDbRollbackTrans* request = CReqDbRollbackTrans::NewLC(iSessionId, aMessage, iTimeOut);
	iManager->StateMachineL().ProcessRequestL(request);  // ownership transferred
	
	// ProcessRequestL received ownership of the request, the request only need
	// to be popped from CleanupStack.		
	CleanupStack::Pop(request);		
	}

