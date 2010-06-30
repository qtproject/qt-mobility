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

#ifndef CNTMSGHANDLERFPTR_H
#define CNTMSGHANDLERFPTR_H

class CCntMsgHandler;
class CCntItemMsgHandler;
class CCntEventMsgHandler;
class CCntTransactionMsgHandler;
class CCntFileManagerMsgHandler;
class CCntPropertiesMsgHandler;
class CCntViewMsgHandler;

typedef void (CCntMsgHandler::*MsgHandlerFptr)(const RMessage2&);

typedef void (CCntItemMsgHandler::*ItemMsgHandlerFptr)(const RMessage2&);
typedef void (CCntEventMsgHandler::*EventMsgHandlerFptr)(const RMessage2&);
typedef void (CCntTransactionMsgHandler::*TransactionMsgHandlerFptr)(const RMessage2&);
typedef void (CCntFileManagerMsgHandler::*FileManagerMsgHandlerFptr)(const RMessage2&);
typedef void (CCntPropertiesMsgHandler::*PropertiesMsgHandlerFptr)(const RMessage2&);
typedef void (CCntViewMsgHandler::*ViewMsgHandlerFptr)(const RMessage2&);



#endif
