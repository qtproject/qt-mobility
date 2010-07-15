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


#include "ccontactprivate.h"
#include "ccntnotificationmonitor.h"
#include "rcntmodel.h"


extern void DebugLogNotification(const TDesC& aMethod, const TContactDbObserverEvent &aEvent);


CCntNotificationMonitor::CCntNotificationMonitor(RCntModel& aSession) 
	: CActive(EPriorityStandard), iSession(aSession)
	{}


CCntNotificationMonitor::~CCntNotificationMonitor()
	{
	Cancel();
	}


void CCntNotificationMonitor::DoCancel() 
	{
	// TRequestStatus* status = &iStatus;
	// User::RequestComplete(status, KErrNone);
	}	


/**
This method is called each time the request for an event from the Server is
completed.
*/
void CCntDbNotifyMonitor::RunL()
	{
	if ( iStatus == KErrDied || iStatus == KErrServerTerminated )
		{
		// We have lost our connection to the Server.  Attempt to restart it,
		// reconnect the session and open the Contacts database.
		iSession.HandlePrematureServerTerminationL();
		// Request another event.
		Start();
		return;
		}
	
	

	
	TInt count = iObserverArray.Count();
	TContactDbObserverEventV2& event = iEventMsg();
	
#if defined(__VERBOSE_DEBUG__)	
	DebugLogNotification(_L("[CNTMODEL] CCntDbNotifyMonitor::RunL"), event);	
#endif

	// Notify all registered observers of the event.
	for (TInt i = 0 ; i < count ; ++i)
		{
		iCurrentProcessedObserver = i;
        // Ignore any leave so we can notify all registered observers.
	    TRAP_IGNORE(iObserverArray[i]->HandleDatabaseEventL(event));
		// The call to HandleDatabaseEventL() above may remove or/add observer/observers
		// so get the count again to cover this scenario. Also we have to update current
		// loop counter.
		i = iCurrentProcessedObserver;
		count = iObserverArray.Count();
		}
	
    // Notify all registered observers of the exnteded (V2) event.
	if (event.iTypeV2 != EContactDbObserverEventV2Null)
	    {
        TInt v2Count = iObserverV2Array.Count();
        for (TInt i = 0 ; i < v2Count ; ++i)
            {
            iCurrentProcessedObserver = i;
            if (event.iTypeV2 != EContactDbObserverEventV2Null)
                {
                // Ignore any leave so we can notify all registered observers.
                TRAP_IGNORE(iObserverV2Array[i]->HandleDatabaseEventV2L(event));
                }
            // The call to HandleDatabaseEventL() above may remove or/add observer/observers
            // so get the count again to cover this scenario. Also we have to update current
            // loop counter.
            i = iCurrentProcessedObserver;
            v2Count = iObserverV2Array.Count();
            }
	    }
	
	// Request another event.
	Start();
	}

	
/**
First phase constructor.
*/
CCntDbNotifyMonitor::CCntDbNotifyMonitor(RCntModel& aSession) 
	: CCntNotificationMonitor(aSession), iCurrentProcessedObserver(KErrNotFound)
	{}

	
/**
Second phase constructor.

@param aSession Client-side session handle for Contacts Model Server.
*/
CCntDbNotifyMonitor* CCntDbNotifyMonitor::NewL(RCntModel& aSession)
	{
	CCntDbNotifyMonitor* self = new (ELeave) CCntDbNotifyMonitor(aSession);
	CActiveScheduler::Add(self);
	return(self);
	}


CCntDbNotifyMonitor::~CCntDbNotifyMonitor()
	{
	iObserverArray.Close();
	iObserverV2Array.Close();
	}

	
/**
This method is called to request a Contacts database event from the Server.
The RunL() method will be called when an event is available (i.e. when the
Server completes the request.
*/
void CCntDbNotifyMonitor::Start()
	{
	// Check if a request for a Contacts database event from the Server has
	// already been issued.
	if (IsActive())
		{
		return;
		}
	iStatus=KRequestPending;
	iSession.StartNotificationTransfer(iStatus, iEventMsg);	
	SetActive();
	}


/**
Add a Contacts database event observer.

@param aObserver The Contacts database event observer to add.
*/
void CCntDbNotifyMonitor::AddObserverL(MContactDbObserver& aObserver)
	{
	// Observers can be added from HandleDatabaseEventL() called from CCntDbNotifyMonitor::RunL()
	// To avoid overcomplicated algorithms for concurent access to handlers array, new observer will
	// be added always at the end of array. Whilst duplications are not allowed, a search is 
	// needed to check if observer is already added to observers array
	
	const TInt index = iObserverArray.Find(&aObserver);
	if( index != KErrNotFound )
		{
		User::Leave( KErrAlreadyExists );
		}
	
	User::LeaveIfError( iObserverArray.Append(&aObserver) );
	
	// At least one observer now added so start request for Contacts database
	// event from the Server (if it hasn't already been started i.e. if the AO
	// is not already active).
	Start();
	}


/**
Remove a Contacts database event observer.

@param aObserver The Contacts database event observer to remove.
*/
void CCntDbNotifyMonitor::RemoveObserver(const MContactDbObserver& aObserver)
	{
	TInt index = iObserverArray.Find(&aObserver);
	if (index != KErrNotFound)
		{
		iObserverArray.Remove(index);
		
		// Observers can be removed from HandleDatabaseEventL() called from CCntDbNotifyMonitor::RunL()
		// In order to notify correctly remaining observers, we need to update current processed observer.
		if( index <= iCurrentProcessedObserver )
			{
			--iCurrentProcessedObserver;
			}
			
		}
	// If there are no more observers then cancel the event request in the
	// Server.
	if (iObserverArray.Count() + iObserverV2Array.Count() == 0)
		{
		iSession.EndNotificationTransfer();
		}
	}
	
/**
Add a Contacts database event observer.

@param aObserver The Contacts database group event observer to add.
*/
void CCntDbNotifyMonitor::AddObserverV2L(MContactDbObserverV2& aObserver)
    {
    // Observers can be added from HandleDatabaseEventV2L() called from CCntDbNotifyMonitor::RunL()
    // To avoid overcomplicated algorithms for concurent access to handlers array, new observer will
    // be added always at the end of array. Whilst duplications are not allowed, a search is 
    // needed to check if observer is already added to observers array
    
    const TInt index = iObserverV2Array.Find(&aObserver);
    if( index != KErrNotFound )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    User::LeaveIfError( iObserverV2Array.Append(&aObserver) );
    
    // At least one observer now added so start request for Contacts database
    // event from the Server (if it hasn't already been started i.e. if the AO
    // is not already active).
    Start();
    }


/**
Remove a Contacts database event observer.

@param aObserver The Contacts database group event observer to remove.
*/
void CCntDbNotifyMonitor::RemoveObserverV2(const MContactDbObserverV2& aObserver)
    {
    TInt index = iObserverV2Array.Find(&aObserver);
    if (index != KErrNotFound)
        {
        iObserverV2Array.Remove(index);
        
        // Observers can be removed from HandleDatabaseGroupEventL() called from CCntDbNotifyMonitor::RunL()
        // In order to notify correctly remaining observers, we need to update current processed observer.
        if( index <= iCurrentProcessedObserver )
            {
            --iCurrentProcessedObserver;
            }
            
        }
    // If there are no more observers then cancel the event request in the
    // Server.
    if (iObserverArray.Count() + iObserverV2Array.Count() == 0)
        {
        iSession.EndNotificationTransfer();
        }
    }

/**
Get the number of registered observers.

@return The number of registered observers.
*/
TInt CCntDbNotifyMonitor::NumberOfListeners() const
	{
	return iObserverArray.Count() + iObserverV2Array.Count();
	}


TInt CCntDbNotifyMonitor::RunError(TInt /*aError*/)
	{
#if defined(__VERBOSE_DEBUG__)	
	TContactDbObserverEvent& event = iEventMsg();
	DebugLogNotification(_L("[CNTMODEL] CCntDbNotifyMonitor::RunError"), event);		
#endif

	Start();
		
	return KErrNone;
	}
