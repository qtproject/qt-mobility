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


#ifndef CCNTSESSION_H
#define CCNTSESSION_H


#include "ccntdbmanager.h" // for tcntfilemode.
#include "ccntserver.h"

class CCntCrud;
class CCntDbManager;
class CCntServer;
class CCntPackager;
class CContactViewDef;
class CViewSubSessionBase;
struct TContactDbObserverEvent;
class CEventQueue;
class CCntMsgHandler;

// Message handler type.
enum TCntMsgHandlerType
	{
	ECntItemMsgHandler,
	ECntEventMsgHandler,
	ECntTransactionMsgHandler,
	ECntFileManagerMsgHandler,
	ECntPropertiesMsgHandler,
	ECntViewMsgHandler
	};
	
/**
Implementation of the CCntServer session.

Each session maps to a client-side RCntModel object.  The RCntModel::Connect()
method calls the RSessionBase::CreateSession() method which ultimately causes an
instance of this object to be created.
*/
class CCntSession : public CSession2, public MContactDbObserverV2
	{
public:
	virtual ~CCntSession();
	static	CCntSession* NewL(CCntPackager& aPackager, TUint aSessionId);

	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage, TInt aError);

	// From MContactDbObserverV2.
	void HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent);
	
	friend class CCntMsgHandler;
		
private:
	void ConstructL();
	CCntSession(CCntPackager& aPackager, TUint aSessionId);
		
	inline CCntServer& Server();
	void SessionUnlockL();
	void UnRegisterDatabaseEventObserver();

private:
	CCntDbManager*	 iManager;
	CCntPackager& 	 iPackager;
	CContactViewDef* iView;
	const RMessage2* iMessage;
	TInt 			 iTimeOut;
	TUint	     iSessionId;
	
	// Message Handlers	
	RPointerArray<CCntMsgHandler> iMsgHandlerArray;
 	};

#include "ccntsession.inl"

#endif
