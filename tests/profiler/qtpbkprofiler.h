/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#undef QTPBK_ENABLE_PROFILER

#ifndef QTQTPBKPROFILE_H
#define QTPBKPROFILE_H

// Recommended by default
#include <QObject>
#include <QTime>

class QFile;
class QDebug;
/**
 * Qtphonebook profiling implementation.
 *
 */
#ifdef QTPBK_ENABLE_PROFILER

    /* profiling implementation */
    #define QTPBK_PROFILER_START(aBin) \
        QtPbkProfile::QtPbkProfiling::Instance()->StartProfile(aBin);

    #define QTPBK_PROFILER_END(aBin) \
        QtPbkProfile::QtPbkProfiling::Instance()->EndProfile(aBin);
    #define QTPBK_PROFILER_DELETE

#else // QTPBK_ENABLE_PROFILER

    /* Profiling not enabled - empty implementation */
    #define QTPBK_PROFILER_START(aBin)
    #define QTPBK_PROFILER_END(aBin)
    #define QTPBK_PROFILER_DELETE

#endif // QTPBK_ENABLE_PROFILER


namespace QtPbkProfile {

    // CONSTANTS
    enum ProfileBin
        {
        /////////////////////////////////////////////////////////////////////
        // Measures full startup.
        /////////////////////////////////////////////////////////////////////
        FullStartup                        =  1,
        // This is the last enum value
        EndProfileBins                     =  2
        };
    
    /**
     * Utility class for QTPhonebook profiling.
     */
    class QtPbkProfiling
        {
        public: // Interface

            /**
             * Returns the global shared object of this class.
             * Creates the object if it does not exist.
             *
             * @return  Instance of this class.
             */
            Q_DECL_IMPORT static QtPbkProfiling* Instance();

            /**
             * Destructor.
             */
            Q_DECL_IMPORT ~QtPbkProfiling();

            /**
             * Starts profiling.
             *
             * @param aBin  Integer identifying the profile.
             */
            Q_DECL_IMPORT void StartProfile( int bin );

            /**
             * Ends profiling.
             *
             * @param aBin  Integer identifying the profile.
             */
            Q_DECL_IMPORT void EndProfile( int bin ) const;

        private: // Implementation
            QtPbkProfiling();

        private:
            QTime   mProfileArray[EndProfileBins];            
            QFile*  mFile;
            QDebug* mDebug;
        };
}
#endif // QTQTPBKPROFILE_H
