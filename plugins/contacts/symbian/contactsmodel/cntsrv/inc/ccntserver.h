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


#ifndef CNTSERVER_H
#define CNTSERVER_H


#include <e32base.h>
#include <e32hashtab.h> // for rhashmap
#include "ccntmsghandlerfptr.h"


class CCntDbManagerController;
class CCntPackager;
class CCntServerCurrentItemMap;


const TInt KContactsServerPriority = 0;

const TUint KLockSrvMajorVersionNumber=1;
const TUint KLockSrvMinorVersionNumber=0;
const TUint KLockSrvBuildVersionNumber=0;

const TUint KLockSrvServerThreadHeapSize=0x2000;
const TUint KLockSrvServerMaxThreadHeapSize=0x100000;

const TInt KSlot0 = 0;
const TInt KSlot1 = 1;
const TInt KSlot2 = 2;
const TInt KSlot3 = 3;

const TUint KCntNullConnectionId = 0;


// Enumerations.
enum TLockSrvClientPanics
	{ 
	ELockSrvPanicBadDescriptor,
	ELockSrvPanicBadHandle,
	ELockSrvPanicInvalidOpcode,
	ELockSrvPanicAlreadyInitialised,
	ELockSrvPanicNoReplyBuffer,
	ELockSrvPanicEventDescriptor,
	ELockSrvPanicInvalidViewIndex,
	ELockSrvPanicMultipleSortableTextAtLengthCalls,
	ELockSrvPanicInvalidSortableTextAtCall,
	// 8.1 new panic codes
	ELockSrvPanicSessionFileNameNotSet,
	};


enum TCntServerType
	{
	ETransientServer,
	ENonTransientServer
	};	


// Literal constants - shared between client & server.
_LIT(KContactsServerName, "!CntLockServer");
_LIT(KContactsBaseFilename, "Contacts.cdb");


class CServerShutdown : public CTimer
	{
public:
	static CServerShutdown* NewL();
	void Start();

private:
	CServerShutdown();
	void ConstructL();
	void RunL();

private:
	// Shut down server if no clients connected for 5 seconds.
	enum {KServerShutdownDelay = 5000000};	
	};


class CCntServer : public CPolicyServer
	{
public:
	static CCntServer* NewL();
	virtual ~CCntServer();
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	CCntDbManagerController& Controller();
	CObjectCon* NewContainerL();
	void RemoveObjectContainer(CObjectCon& aContainer);
	void SessionClosing();
	TInt SessionCount();
	void CreateMessageMapL();	
	RHashMap<TInt,MsgHandlerFptr>& MsgLut();

private:
	CCntServer();
	void ConstructL();

private:
	CCntDbManagerController*	iController;
	CCntPackager* 				iPackager;
	CObjectConIx* 				iContainerIndex;
	CServerShutdown* 			iServerShutdown;
	TCntServerType				iServerType;
	mutable TUint 				iNextSessionId;
	
    /** Message Handler Lookup table.
    Look table maps message functions (op codes) to appropriate
    message handler methods.
    */	
	RHashMap<TInt,MsgHandlerFptr> iMsgLut;
	};


#endif
