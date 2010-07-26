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
* CContactDbEventQueue Test module
*
*/


#ifndef __CContactDbEventQueue_H__
#define __CContactDbEventQueue_H__

// INCLUDES
#include <cntdef.h>
#include <cntdbobs.h>
#include "cfixedqueue.h"


// CLASS DECLARATION
class CContactChangeNotifier;
class CContactDatabase;


/**
 * Contact Engine event queue class.
 */ 
class CContactDbEventQueue : 
        public CTimer, public MContactDbObserver
    {
    public:
        /**
         * Creates a new event queue.
         * @param aDb   contact DB to listen for events.
         * @param aMaxQueueSize maximum size of event queue.
         * @return a new instance of this class.
         */
        static CContactDbEventQueue* NewL
            (CContactDatabase* aDb=NULL, TInt aMaxQueueSize=128);
        ~CContactDbEventQueue();

        /**
         * Waits for an event from the database.
         * @param aTimeOut  max time to wait for an event.
         * @param aEvent    the received event, undefined if this function returns false.
         * @return true if an event was received, false if timeout expired first.
         */
        TBool ListenForEvent
            (TTimeIntervalSeconds aTimeOut, 
            TContactDbObserverEvent& aEvent);
            
         /**
         * Pops Events from the event queue.  Required to avoid filling the queue.
         */    
        void PopEvent();           

         /**
         * Pops All Events from the event queue. Required to avoid queue overflow.
         */    
		void Clear();

        /**
         * Removes all previously arrived events from the queue.
         */
        inline void Flush()
            {
            iQueue.Reset();
            }

    protected: // From MContactDbObserver.
	    void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

    private:  
        void RunL(); // from CTimer
        CContactDbEventQueue();
        void ConstructL(CContactDatabase* aDb, TInt aMaxQueueSize);

    private:  
        /// Own: the notifier for attaching to CContactDatabase
        CContactChangeNotifier* iNotifier;
        /// Own: event queue array
        CFixedQueue<TContactDbObserverEvent> iQueue;
    };

#endif  // __CContactDbEventQueue_H__





