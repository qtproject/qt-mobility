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
* ContactViewEventQueue Test module
*
*/



#ifndef CContactViewEventQueue_H
#define CContactViewEventQueue_H

// INCLUDES
#include <cntview.h>
#include "cfixedqueue.h"


// CLASS DECLARATION

/**
 * Contact view event queue class.
 */ 
class CContactViewEventQueue : 
        public CTimer, public MContactViewObserver
    {
    public:
        /**
         * Creates a new event queue.
         * @param aView contact view to listen for events.
         * @param aMaxQueueSize maximum size of event queue.
         * @return a new instance of this class.
         */
        static CContactViewEventQueue* NewL
            (CContactViewBase* aView=NULL, TInt aMaxQueueSize=128);
        ~CContactViewEventQueue();

        /**
         * Waits for an event from the database.
         * @param aTimeOut  max time to wait for an event.
         * @param aEvent    the received event, undefined if this function returns false.
         * @return true if an event was received, false if timeout expired first.
         */
        TBool ListenForEvent
            (TTimeIntervalSeconds aTimeOut, 
            TContactViewEvent& aEvent);

        /**
         * Removes all previously arrived events from the queue.
         */
        inline void Flush()
            {
            iQueue.Reset();
            }

		/**
		 * Tells this observer to remove itself from the view
		 * if an ItemRemoved event is received
		 */
		inline void CloseOnItemRemoved(TBool val = ETrue)
			{
			iCloseOnItemRemoved = val;
			}

    private:  // from CTimer
        void RunL();

    private: // From MContactViewObserver.
	    void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);

    private:  // Implementation
        CContactViewEventQueue();
        void ConstructL(CContactViewBase* aView, TInt aMaxQueueSize);

    private:  // Data
        /// Ref: the view to listen for events
        CContactViewBase* iView;
        /// Own: event queue array
        CFixedQueue<TContactViewEvent> iQueue;
		/// Flag for observer response to item removed event
		TBool iCloseOnItemRemoved;
    };

#endif

// End of File

