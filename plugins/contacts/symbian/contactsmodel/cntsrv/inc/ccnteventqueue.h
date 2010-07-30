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


#ifndef CCNTEVENTQUEUE_H
#define CCNTEVENTQUEUE_H

#include <e32base.h>
#include <cntdb.h>

#include <cntdbobs.h> // for tcontactdbobserverevent.


#define  KMaxNumberOfEventsInEventQueue 1000

#if defined(_DEBUG)	// For OOM testing set granularity to 1
	const TInt KCntEventGranularity = 1;
#else
	const TInt KCntEventGranularity = 10;
#endif


/**
The CEventQueue class is used to queue database events on the Server for later
consumption by Client-side objects which make IPC calls to read events from the
queue.  An instance of this class is owned by each CCntSession object.

When a Contacts database is openend, created or replaced in the method
CCntSession::FileOpenCreateReplaceL() then the CCntSession object is registered
as a database event observer of the associated CCntDbManager.

Then when a database event occurs the database event callback interface
implemented by CCntSession (i.e. the MContactDbObserver::HandleDatabaseEventL()
method) is called and the event placed in this queue.
*/
class CEventQueue : public CBase
	{
public:
	CEventQueue();
	~CEventQueue();
	void QueueEvent(const TContactDbObserverEventV2 &aEvent);
	void RequestEvent(const RMessage2& aMessage);
	void CancelEventRequest();

private:
	enum TFlag
		{
		EQueueError=0x0001,
		EValidEventMsg=0x0002
		};

private:
	void Flush();
	void SendEvent(const TContactDbObserverEventV2 &aEvent);
	void SetFlag(TFlag aFlag);
	void ClearFlag(TFlag aFlag);
	TBool Flag(TFlag aFlag);

private:
	RArray<TContactDbObserverEventV2> iEvents;
	RMessage2 iEventMsg;
	TInt iFlags;
	TInt iEventsInQueue;
	};


#endif
