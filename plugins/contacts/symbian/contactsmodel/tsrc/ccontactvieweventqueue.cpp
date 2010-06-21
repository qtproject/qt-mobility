/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ccontactvieweventqueue.h"

namespace {

// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicPreCond_HandleContactViewEvent = 1,
    EPanicEventQueueFull
    };

// LOCAL FUNCTIONS
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CContactViewEventQueue");
    User::Panic(KPanicText,aReason);
    }

void ResetEvent(TContactViewEvent& aEvent)
    {
    aEvent.iEventType = TContactViewEvent::TEventType(-1);
    aEvent.iInt = -1;
    aEvent.iContactId = KNullContactId;
    }

}


// ================= MEMBER FUNCTIONS =======================

CContactViewEventQueue* CContactViewEventQueue::NewL
        (CContactViewBase* aView, TInt aMaxQueueSize)
    {
    CContactViewEventQueue* self = new(ELeave) CContactViewEventQueue;
    CleanupStack::PushL(self);
    self->ConstructL(aView,aMaxQueueSize);
    CleanupStack::Pop(self);
    return self;
    }

CContactViewEventQueue::~CContactViewEventQueue()
	{
	CTimer::Cancel();
	if (iView)
		{
		iView->Close(*this);
		}
	}

TBool CContactViewEventQueue::ListenForEvent(TTimeIntervalSeconds aTimeOut, TContactViewEvent& aEvent)
	{
	ResetEvent(aEvent);
	CTimer::Cancel();

	if (iQueue.IsEmpty())
		{
		CTimer::After(aTimeOut.Int() * 1000000);
		// wait for event or for the timer to expire
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

void CContactViewEventQueue::RunL()
    {
    // Timer expired
    CActiveScheduler::Stop();
    }

void CContactViewEventQueue::HandleContactViewEvent
        (const CContactViewBase& aView, const TContactViewEvent& aEvent)
    {
    __ASSERT_ALWAYS(!iView || iView == &aView, Panic(EPanicPreCond_HandleContactViewEvent));

	TBool eventPushed = iQueue.Push(aEvent);
    __ASSERT_ALWAYS(eventPushed,Panic(EPanicEventQueueFull));

    // If we receive item removed event, it is a signal for the this
    // view observer to stop observing.
    // The observer could have been for example a memory entry representation 
    // and the memory entry has now been deleted -> the observer must close and exit.
	if(iCloseOnItemRemoved && (aEvent.iEventType == TContactViewEvent::EItemRemoved))
        {
        iView->Close(*this);
        }

    if (IsActive())
    	{
    	CTimer::Cancel();
	    CActiveScheduler::Stop();
    	}
    }

CContactViewEventQueue::CContactViewEventQueue()
    : CTimer(CActive::EPriorityStandard)
    {
    }

void CContactViewEventQueue::ConstructL
        (CContactViewBase* aView, TInt aMaxQueueSize)
    {
	iCloseOnItemRemoved = EFalse;
    CTimer::ConstructL();
    iQueue.ConstructL(aMaxQueueSize);
    CActiveScheduler::Add(this);
    if (aView)
        {
        aView->OpenL(*this);
        iView = aView;
        }
    }


// End of File
