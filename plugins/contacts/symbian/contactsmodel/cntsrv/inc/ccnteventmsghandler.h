/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CNTEVENTMSGHANDLER_H
#define CNTEVENTMSGHANDLER_H

class CCntMsgHandler;

/**
CCntEventMsgHandler class handles Event related operations. It uses
a message handling method to handle the incoming op code.

It delegates the incoming op code to appropriate message handling method by using
a lookup table of member function pointers created by CCntServer.

@internalComponent
*/
class CCntEventMsgHandler : public CCntMsgHandler
	{
public:
	static CCntEventMsgHandler* NewLC(CCntSession& aSession);
	virtual ~CCntEventMsgHandler();
	TInt HandleMessageL(const RMessage2& aMessage);
	
private:
	CCntEventMsgHandler(CCntSession& aSession);
	void ConstructL();
	
public:	
	void RequestEvent(const RMessage2& aMessage);
	void CancelEventRequest(const RMessage2& aMessage);
	void QueueEvent(const TContactDbObserverEventV2 aEvent);
	
private:
	CEventQueue* iEventQueue;
	};
	
#endif
