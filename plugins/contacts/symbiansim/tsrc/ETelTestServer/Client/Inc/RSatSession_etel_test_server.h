/*
* ==============================================================================
*  Name        : RSatSession.h
*  Part of     : SatClient / SatServer
*  Interface   : Internal, RSatSession
*  Description : Common session for accessing SatServer services.
*  Version     : 0.1
*
*  Copyright (c) 2004 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/


#ifndef RSATSESSION_H
#define RSATSESSION_H

//  INCLUDES
#include <e32std.h>
#include <etelmm_etel_test_server.h>

// CLASS DECLARATION

/**
*  Handles the connection to SatServer. This class
*  is used with the sub-sessions.
*
*  @lib SatClient
*/
class RSatSession : public RSessionBase
    {
    public: // New functions

		/**
		* C++ default constructor.
		*/
		RSatSession();

        /**
        * Opens session to SatServer
        */
        void ConnectL();

        void Close();

		/**
		* The client API version.
		* @returns Client API version.
		*/
		TVersion Version() const;

        /**
        * Returns the RTelServer session that is used to emulate RSatSession
        */
        inline const RTelServer& TelServer() const;

    private:

        // Prohibit copy constructor if not deriving from CBase.
        RSatSession( const RSatSession& );

        // Prohibit assigment operator if not deriving from CBase.
        RSatSession& operator=( const RSatSession& );

    private:    // Data
        /// There is no emulated SAT server but the tel server is used for this purpose.
        RTelServer iTelServer;
    };

// INLINE FUNCTIONS
inline const RTelServer& RSatSession::TelServer() const
    {
    return iTelServer;
    }

#endif      // RSATSESSION_H

// End of File
