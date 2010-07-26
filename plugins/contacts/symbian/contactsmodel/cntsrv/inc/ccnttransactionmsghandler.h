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

#ifndef CNTTRANSACTIONMSGHANDLER_H
#define CNTTRANSACTIONMSGHANDLER_H

class CCntMsgHandler;

/**
CCntTransactionMsgHandler class handles all Transaction related operations. It uses
a message handling method to handle the incoming op code.

It delegates the incoming op code to appropriate message handling method by using
a lookup table of member function pointers created by CCntServer.

@internalComponent
*/
class CCntTransactionMsgHandler : public CCntMsgHandler
	{
public:
	static CCntTransactionMsgHandler* NewLC(CCntSession& aSession);
    virtual ~CCntTransactionMsgHandler();
	TInt HandleMessageL(const RMessage2& aMessage);
	
private:
	CCntTransactionMsgHandler(CCntSession& aSession);
	
public:
	void BeginDbTransactionL(const RMessage2& aMessage);
	void EndDbTransactionL(const RMessage2& aMessage);
	void RollbackDbTransactionL(const RMessage2& aMessage);
	};
	
#endif
