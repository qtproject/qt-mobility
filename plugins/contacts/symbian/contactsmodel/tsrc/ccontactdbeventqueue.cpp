/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ccontactdbeventqueue.h"
#include <cntdb.h>

CContactDbEventQueue* CContactDbEventQueue::NewL
        (CContactDatabase* aDb, TInt aMaxQueueSize)
    {
    CContactDbEventQueue* self = new(ELeave) CContactDbEventQueue;
    CleanupStack::PushL(self);
    self->ConstructL(aDb,aMaxQueueSize);
    CleanupStack::Pop(self);
    return self;
    }

CContactDbEventQueue::~CContactDbEventQueue()
    {
    CTimer::Cancel();
    if (iNotifier)
        {
        delete iNotifier;
        }
    }
    
void CContactDbEventQueue::PopEvent()
	{	
	 if (!iQueue.IsEmpty())
     	{
     	iQueue.PopHead();
     	}
	}

void CContactDbEventQueue::Clear()
	{	
	 while(!iQueue.IsEmpty())
     	{
     	iQueue.PopHead();
     	}
	}

TBool CContactDbEventQueue::ListenForEvent(TTimeIntervalSeconds aTimeOut, TContactDbObserverEvent& aEvent)
    {
    CTimer::Cancel();
    if (iQueue.IsEmpty())
        {
        CTimer::After(aTimeOut.Int() * 1000000);
        CActiveScheduler::Start();
        }
    if (!iQueue.IsEmpty())
        {
        aEvent = iQueue.Head();
        iQueue.PopHead();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

void CContactDbEventQueue::RunL()
    {
    // Timer expired
    CActiveScheduler::Stop();
    }

void CContactDbEventQueue::HandleDatabaseEventL
        (TContactDbObserverEvent aEvent)
    {
    const TBool KTimerActive = CTimer::IsActive();
    CTimer::Cancel();
	TBool eventPushed = iQueue.Push(aEvent);
	__ASSERT_ALWAYS(eventPushed,User::Invariant());  // ensure push was successful
    
    if (KTimerActive)
        {
        CActiveScheduler::Stop();
        }
    }

CContactDbEventQueue::CContactDbEventQueue()
    : CTimer(CActive::EPriorityStandard)
    {
    }

void CContactDbEventQueue::ConstructL
        (CContactDatabase* aDb, TInt aMaxQueueSize)
    {
    CTimer::ConstructL();
    iQueue.ConstructL(aMaxQueueSize);
    CActiveScheduler::Add(this);
    
    if (aDb)
        {
        iNotifier = CContactChangeNotifier::NewL(*aDb, this);
        }
    }




