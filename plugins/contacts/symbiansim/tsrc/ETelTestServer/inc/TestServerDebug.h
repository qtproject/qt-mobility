/*
* ============================================================================
*  Name     : TestServerDebug.h
*  Part of  : ETel Test Server
*  Description: Debugging utilities.
*  Version     : %version:  e002sa09#2 %
*
*  Copyright (C) 2002 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing,  adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ============================================================================
*/
#ifndef TESTSERVERDEBUG_H
#define TESTSERVERDEBUG_H

//  INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include "TestServerConfig.h"

#ifdef ETELTSERVER_ENABLE_DEBUG_PRINT
    // Debug logging is enabled

    /**
     * Macro for defining debug-only literal strings.
     */
    #define ETELT_DEBUG_STRING(s) _L(s)

    #ifdef ETELTSERVER_ENABLE_DEBUG_LOGGER
        // Print to log file
        #define ETELT_DEBUG_PRINT ETELTPrintToLog

        #include <flogger.h>

        namespace {
        inline void ETELTPrintToLog( TRefByValue<const TDesC> aText, ... )
            {
            const TInt KLogTimeFormatLength = 16;
            _LIT( KLogFolder, "tf" );
            _LIT( KLogFileName, "ETelTestServer.txt" );
            _LIT( KLogTimeFormat, "%02d.%02d:%02d:%02d ");

            VA_LIST args;
            VA_START( args, aText );

            TBuf<256> buf;
            buf.FormatList( aText, args );

            RFileLogger logger;
            TInt ret( logger.Connect() );
            if (ret==KErrNone)
                {
                logger.SetDateAndTime( EFalse,EFalse );
                logger.CreateLog( KLogFolder, KLogFileName, EFileLoggingModeAppend );
                TBuf<KLogTimeFormatLength> timeStamp;
                TTime now;
                now.HomeTime();
                TDateTime dateTime;
                dateTime = now.DateTime();
                timeStamp.Format( KLogTimeFormat,
                    dateTime.Hour(), dateTime.Minute(),
                    dateTime.Second(), dateTime.MicroSecond() );
                buf.Insert( 0, timeStamp );

                logger.Write(buf);
                }

            logger.Close();

            VA_END( args );
            }
        }  // namespace

    #else
        /**
         * Prints to debugger output.
         */
        #define ETELT_DEBUG_PRINT RDebug::Print

    #endif // #ifdef ETELTSERVER_ENABLE_DEBUG_LOGGER

#else

    /**
     * Dummy struct for checking that all ETELT_DEBUG_PRINT's define string
     * literals using space-saving ETEL_DEBUG_STRING.
     */
    struct TETELTEmptyDebugString { };

    /**
     * Macro for defining debug-only literal strings (empty release version)
     */
    #define ETELT_DEBUG_STRING(s) TETELTEmptyDebugString()

    /// Empty debug print function for release builds.
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1>
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString,T1)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2>
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString,T1,T2)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3>
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString,T1,T2,T3)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4>
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString,T1,T2,T3,T4)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5>
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString,T1,T2,T3,T4,T5)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6>
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString,T1,T2,T3,T4,T5,T6)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString,T1,T2,T3,T4,T5,T6,T7)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    inline void ETELT_DEBUG_PRINT(TETELTEmptyDebugString,T1,T2,T3,T4,T5,T6,T7,T8)
        {
        }

#endif // ETELTSERVER_ENABLE_DEBUG_PRINT

#endif // TESTSERVERDEBUG_H

// End of File
