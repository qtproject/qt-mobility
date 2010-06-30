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

#ifndef CNTPROPERTIESMSGHANDLER_H
#define CNTPROPERTIESMSGHANDLER_H

class CCntMsgHandler;

/**
CCntPropertiesMsgHandler class handles all Properties operations. It uses
a message handling method to handle the incoming op code.

Properties contains all logic to handle client-side get/set operation on
server-side objects and variables.  These objects and variables are used
throughout the server and Persistence Layer.

It delegates the incoming op code to appropriate message handling method by using
a lookup table of member function pointers created by CCntServer.

@internalComponent
*/
class CCntPropertiesMsgHandler : public CCntMsgHandler
	{
public:
	static CCntPropertiesMsgHandler* NewLC(CCntSession& aSession);
    virtual ~CCntPropertiesMsgHandler();
	TInt HandleMessageL(const RMessage2& aMessage);
	void ChangeCntViewDefL(const RMessage2& aMessage);
	
private:
	CCntPropertiesMsgHandler(CCntSession& aSession);
	
public:	
	void ChangeViewDefL(const RMessage2& aMessage);
	void MachineIdL(const RMessage2& aMessage);
	void FileUniqueIdL(const RMessage2& aMessage);
	void OverrideMachineIdL(const RMessage2& aMessage);
	void ReCreateTemplateL(const RMessage2& aMessage);
	void GetPrefTemplateIdL(const RMessage2& aMessage);
	void SetPrefTemplateIdL(const RMessage2& aMessage);
	void OpsTimeOut(const RMessage2& aMessage);
	void ICCTemplateIdL(const RMessage2& aMessage);
	void PhonebookGroupIdL(const RMessage2& aMessage);
	};
	
#endif
