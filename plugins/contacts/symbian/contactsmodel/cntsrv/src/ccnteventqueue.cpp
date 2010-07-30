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


#include "ccnteventqueue.h"
#include "ccntserver.h"
#include "ccntlogger.h"


extern void DebugLogNotification(const TDesC& aMethod, const TContactDbObserverEventV2 &aEvent);


CEventQueue::CEventQueue()
	: iEvents(KCntEventGranularity)
	{
	}


CEventQueue::~CEventQueue()
	{
	iEvents.Close();
	}


void CEventQueue::QueueEvent(const TContactDbObserverEventV2 &aEvent)
	{
	// Flush the queue - there's no point in sending out all the preceeding
	// events when the an Unknown Changes event is being sent.
	// Unknown changes means that there are too many events to propagate and
	// the client should resync all its cached data.
	if (aEvent.iType == EContactDbObserverEventUnknownChanges)
		{
		Flush();	
		}
	
	// Is the queue in order, if not then return (no event will be sent)
	if (Flag(EQueueError))
		{
		return;	
		}
		
	// Can we send the event right away, i.e. do we have an outstanding request
	if (Flag(EValidEventMsg))
		{
		SendEvent(aEvent);	
		}// Is the queue full? If so set flag EQueueError and return
	else if (iEvents.Count() > KMaxNumberOfEventsInEventQueue)
		{
		
		DEBUG_PRINTDN2(__VERBOSE_DEBUG__,_L("[CNTMODEL] ->X"), aEvent);

		Flush();
		SetFlag(EQueueError);
		}// Otherwise can we add the request to the queue
	else if (iEvents.Append(aEvent)!=KErrNone)
		{
		SetFlag(EQueueError);
		}
#if defined(__VERBOSE_DEBUG__)	
	else
		{
		// If added then add it to log
		DebugLogNotification(_L("[CNTMODEL] ->Q"), aEvent);	
		}
#endif
	}


void CEventQueue::Flush()
	{
	iEvents.Reset();	
	}


void CEventQueue::RequestEvent(const RMessage2& aMessage)
	{
	__ASSERT_DEBUG(!Flag(EValidEventMsg),User::Leave(KErrCompletion));
	if (!Flag(EValidEventMsg))
		{
		iEventMsg=aMessage;
		SetFlag(EValidEventMsg);
		if (Flag(EQueueError))
			{
			TContactDbObserverEventV2 errorEvent;
			//EContactDbObserverEventUnknownChanges is indicating that the event queue is full
			errorEvent.iType = EContactDbObserverEventUnknownChanges;
			errorEvent.iContactId = KNullContactId;
			errorEvent.iConnectionId = KCntNullConnectionId;
			errorEvent.iTypeV2 = EContactDbObserverEventV2Null;
			errorEvent.iAdditionalContactId = KNullContactId;
			ClearFlag(EQueueError);
			SendEvent(errorEvent);
			}
		else if (iEvents.Count()>0)
			{
			SendEvent(iEvents[0]);
			iEvents.Remove(0);
			}
		else
			{
			//No events has happened, so there is nothing to tell the client,
			//But to be able to test the API policing we will complete the message
			//if it contains the ApiPolicingTest flag and that the message is sent by the 
			//API policing test process sid(0x101F7784)
			TInt isApiPolicingTest = aMessage.Int1();
			if (isApiPolicingTest == 666 && aMessage.SecureId() == 0x101F7784)
				{
				TContactDbObserverEventV2 nullEvent;
				nullEvent.iType = EContactDbObserverEventNull;
				nullEvent.iConnectionId = 0;
				nullEvent.iTypeV2 = EContactDbObserverEventV2Null;
				nullEvent.iAdditionalContactId = KNullContactId;
				SendEvent(nullEvent);				
				}
			}
		}
	}


void CEventQueue::CancelEventRequest()
	{
	if (Flag(EValidEventMsg))
		{
		iEventMsg.Complete(KErrCancel);
		ClearFlag(EValidEventMsg);
		}
	}


void CEventQueue::SendEvent(const TContactDbObserverEventV2 &aEvent)
	{
     DEBUG_PRINTDN2(__VERBOSE_DEBUG__,_L("[CNTMODEL] Q->"), aEvent);

	TPckgC<TContactDbObserverEventV2> event(aEvent);
	TInt err = iEventMsg.Write(0, event);
	if (err != KErrNone)
		{
		// If there is an error caused, for example, by the client dying, just log the error code.
		DEBUG_PRINT2(__VERBOSE_DEBUG__, _L("[CNTMODEL] iEventMsg.Write() error: %d"), err);
 		}
	else
		{
		iEventMsg.Complete(KErrNone);
		}
	ClearFlag(EValidEventMsg);
	}


TBool CEventQueue::Flag(TFlag aFlag)
	{
	return iFlags&aFlag;
	}


void CEventQueue::SetFlag(TFlag aFlag)
	{
	iFlags|=aFlag;
	}


void CEventQueue::ClearFlag(TFlag aFlag)
	{
	iFlags&=(~aFlag);
	}
