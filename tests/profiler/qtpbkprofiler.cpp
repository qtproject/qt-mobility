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


#include <qtpbkprofiler.h>
#include <QThreadStorage>
#include <QFile>
#include <QDebug>

namespace QtPbkProfile {
	
// TLS Object definiton
QThreadStorage<QtPbkProfiling*> tls;

Q_DECL_EXPORT QtPbkProfiling* QtPbkProfiling::Instance()
    {
    QtPbkProfiling* self = 0;
    
    //Check Thread local storage
    if (!tls.hasLocalData())
        {
        // create new provider
        self = new QtPbkProfiling;
        // and store it to TLS
        tls.setLocalData(self);
        }
    else
        {
        // otherwise get data from TLS
        self = tls.localData();
        }
    // return a pointer of "this" object 
    return self;
    }

Q_DECL_EXPORT QtPbkProfiling::~QtPbkProfiling()
    {
    delete mDebug;
    if (mFile)
        mFile->close();
    
    delete mFile;
    }

Q_DECL_EXPORT void QtPbkProfiling::StartProfile(int bin)
    {
    // Starting time for profile "bin"
    QTime startTime = QTime::currentTime();
    mProfileArray[bin] = startTime;
    }

Q_DECL_EXPORT void QtPbkProfiling::EndProfile(int bin) const
    {
    // Ending time for profile "bin"
    QTime endTime = QTime::currentTime();
    // Time difference
    int duration = endTime.msec() - mProfileArray[bin].msec();
    
    // Write to file
    *mDebug << "Profile " << bin << " duration: "
                            << duration / 1000 << "s" << duration % 1000 << "ms\n";
    }

Q_DECL_EXPORT QtPbkProfiling::QtPbkProfiling()
    {
    QString absolutePath("C:/logs/qtphonebook/profiling.txt");
    mFile = new QFile(absolutePath);
    
    if (mFile->open(QIODevice::Text | QIODevice::WriteOnly))
        mDebug = new QDebug(mFile);
    }

}  // namespace QtPbkProfile

// End of File
