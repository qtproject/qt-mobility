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

#include "ccntsession.h"
#include "ccntserver.h"
#include "ccntbackuprestoreagent.h"
#include "ccntpackager.h"

#include "ccntstatemachine.h"
#include "ccntdbmanagercontroller.h"
	
CCntMsgHandler::CCntMsgHandler(CCntSession& aSession)
:iManager(aSession.iManager), iPackager(aSession.iPackager), iView(aSession.iView), iTimeOut(aSession.iTimeOut), iSessionId(aSession.iSessionId), iSession(aSession)
	{
	}

CCntMsgHandler::~CCntMsgHandler()
	{
	}
	

// Helper methods

/**
Many operations require a CCntDbManager instance.  This method checks that it
exists.
*/
void CCntMsgHandler::CheckForManagerL()
	{
	if(iManager == NULL) 
		{
		User::Leave(KErrNotReady);
		}	
	}
	
/**
Get reference to the parent CServer2 derived class.
*/
CCntServer& CCntMsgHandler::Server()
	{
	return iSession.Server();
	}
	
/**
Read message.

@leave KErrBadDescriptor Error occurred with aDes.
*/
void CCntMsgHandler::ReadL(const RMessage2& aMessage, TInt aParam, TDes& aDes)
	{
	const TInt ret = aMessage.Read(aParam, aDes);
	if (ret != KErrNone)
		{
		User::Leave(KErrBadDescriptor);
		}
	}
	
/**
Read message.

@leave KErrBadDescriptor Error occurred with aDes.
*/
void CCntMsgHandler::WriteL(const RMessage2& aMessage, TInt aParam, const TDesC& aDes, TInt aOffset)
	{
	const TInt ret = aMessage.Write(aParam, aDes, aOffset);
	if	(ret != KErrNone)
		{
		User::Leave(KErrBadDescriptor);
		}
	}

/** 
Session is not going to be interested in database event notifications 
so un-register it.
*/
void CCntMsgHandler::UnRegisterDatabaseEventObserver()
	{
	iSession.UnRegisterDatabaseEventObserver();
	}
	
/** 
Get function pointer to message handling method. 
*/
MsgHandlerFptr CCntMsgHandler::LookupHandlerMethodL(TInt aOpCode, const TInt* aOpCodes, const TInt aOpCodesLength)
	{	
	for(TInt index = 0; index < aOpCodesLength; index++)
		{		
		if(aOpCode == aOpCodes[index])
			{
			const RHashMap<TInt,MsgHandlerFptr>& msgLut = Server().MsgLut();
			MsgHandlerFptr func_ptr = msgLut.FindL(aOpCode);
			return func_ptr;
			}
		}
	
	return NULL;
	}
