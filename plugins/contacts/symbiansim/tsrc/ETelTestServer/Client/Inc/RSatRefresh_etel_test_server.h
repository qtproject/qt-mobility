/*
* ==============================================================================
*  Name        : RSatRefresh.h
*  Part of     : SatClient / SatServer
*  Interface   : Internal, RSatRefresh
*  Description : API for getting refresh notifications.
*  Version     : 1.0
*
*  Copyright © 2002-2004 Nokia. All rights reserved.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia. All rights are reserved. Copying, including
*  reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia.
* ==============================================================================
*/


#ifndef RSATREFRESH_H
#define RSATREFRESH_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
//#include <Etelsat.h>
#include <MSatRefreshObserver_etel_test_server.h>

// FORWARD DECLARATIONS
class RSatSession;
class CSatAllowRefreshMonitor;
class CSatRefreshMonitor;

// CLASS DECLARATION

/**
*  Sub-session for getting refresh notifies.
*
*  @lib SatClient.lib
*/
class RSatRefresh : public RSubSessionBase
    {
    public:  // Constructors and destructor

		/**
        * C++ default constructor.
		* @param aObserver Observer, which is notified about
		* refresh.
        */
        RSatRefresh( MSatRefreshObserver& aObserver );

        /// THIS API IS NOT OFFICIAL RSatSession API but only
        /// for testing purposes. Can be used to from the unit test
        /// to only launch SAT refresh
        RSatRefresh() {}

    public: // New functions

        /**
        * Opens the sub-session.
		* @param aSession Opens the sub-session.
        */
        void OpenL( const RSatSession& aSession );

		/**
		* Closes the sub-session. Cancels
		* the notification if active.
		*/
		void Close();

		/**
		* Starts the waiting for refresh events.
		* !!! Test server support only one NotifyFileChangeL at a time so
		* the two versions of NotifyFileChangeL can not be called at same time
		*/
		void NotifyFileChangeL();

        /**
        * Starts the waiting for refresh events.
        * @param aObservedFiles List of files to be observed for refresh.
        */
        void NotifyFileChangeL(
            const TSatRefreshFiles& aObservedFiles );

		/**
		* Cancels the notification.
		*/
		void Cancel() const;

        void RefreshEFRead( TBool aAdditionEFRead );

        /// THIS API IS NOT OFFICIAL RSatSession API but
        // for testing purposes
        void LaunchNotification( TSatRefreshType aType,
            TSatRefreshFiles& aFiles );

    private:

        // Prohibit copy constructor if not deriving from CBase.
         RSatRefresh( const RSatRefresh& );
        // Prohibit assigment operator if not deriving from CBase.
         RSatRefresh& operator=( const RSatRefresh& );

	private:

		// Ref: Observer of refresh events.
		MSatRefreshObserver* iObserver;

		// Active object to monitor refresh query.
		CSatAllowRefreshMonitor* iAllowMonitor;

		// Active object to monitor refresh.
		CSatRefreshMonitor* iRefreshMonitor;

		// for test simulator
		CPeriodic* iPeriodic;
		TBool      iRefresh;
		TInt       iCount;

        /// A reference to the main session used by this sub session
        const RSatSession* iSatSession;

        class CNotifier;
        CNotifier* iNotifier;
    };

#endif      // RSATREFRESH_H

// End of File
