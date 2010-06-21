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

#ifndef CNTMSGHANDLER_H
#define CNTMSGHANDLER_H

#include <e32base.h>
#include "ccntdbmanager.h" // for tcntfilemode
#include "ccntmsghandlerfptr.h"

class CCntSession;
class CCntServer;
class CCntDbManager;
class CCntPackager;
class CContactViewDef;
class CEventQueue;

/**
The base class for message handler classes. CCntSession class creates
and owns message handler instances.

@internalComponent
*/
class CCntMsgHandler : public CBase
	{
public:
	virtual ~CCntMsgHandler();
	
	/** 
	Overriden by all derived classes. Delegates the incoming op code
	to a message handling method. 
	*/
	virtual TInt HandleMessageL(const RMessage2& aMessage) = 0;	
	
protected:
	CCntMsgHandler(CCntSession& aSession);
	
	// Helper methods.
	void CheckForManagerL();
	CCntServer& Server();
	void ReadL(const RMessage2& aMessage, TInt aParam, TDes& aDes);
	void WriteL(const RMessage2& aMessage, TInt aParam, const TDesC& aDes, TInt aOffset = 0);
	void UnRegisterDatabaseEventObserver();
	MsgHandlerFptr LookupHandlerMethodL(TInt aOpCode, const TInt* aOpCodes, const TInt aOpCodesLength);
	
protected:
	CCntDbManager*&	 iManager;
	CCntPackager& 	 iPackager;
	CContactViewDef*& iView;
	TInt& 			 iTimeOut;
	TUint&	     iSessionId;
	CCntSession& iSession;	
	};
	

#endif
