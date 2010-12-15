/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../../testqgeosatelliteinfosource_p.h"
#include <QDebug>
#include <QProcess>
#include <QProcessEnvironment>

// #ifdef TST_GYPSY_MOCK todo

// Temporary define to test the concept
#define MOCK_LIB_DIR "/QT/meego-location-qt-mobility/tests/auto/qgeosatelliteinfosource/gypsymock/libgypsy.so.0"

static bool gypsyMockDisabled()
{
    QByteArray v = qgetenv("TST_DISABLE_GYPSY_MOCK");
    return (!v.isEmpty() && v != "0" && v != "false");
}
// #endif // todo


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestQGeoSatelliteInfoSource *test = TestQGeoSatelliteInfoSource::createDefaultSourceTest();

    qDebug() << "*** running the main in pid: " <<  app.applicationPid();
    qDebug() << "*** applicationFilePath is: " << QCoreApplication::applicationFilePath ();

    if (gypsyMockDisabled()) {
        qDebug("*** Gypsy mock is disabled, executing against real libraries.");
        return QTest::qExec(test, argc, argv);
    } else {
        // Reinvoke the testprogram, this time preloaded with mocked library
        QProcess process;
        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

        if (environment.contains("TST_QGEOSATELLITEINFOSOURCE_ALREADY_LAUNCHED")) {
            qDebug("*** I am a relaunched executable, launching the actual testcase.");
            return QTest::qExec(test, argc, argv);
        } else {
            qDebug("*** I am a fresh executable, relaunching myself.");
            environment.insert("TST_QGEOSATELLITEINFOSOURCE_ALREADY_LAUNCHED", "true");
            environment.insert("LD_PRELOAD", "/QT/meego-location-qt-mobility/tests/auto/qgeosatelliteinfosource/gypsymock/libgypsy.so.0");
            process.setProcessEnvironment(environment);
            process.setProcessChannelMode(QProcess::ForwardedChannels);
            process.start(QCoreApplication::applicationFilePath ());
            qDebug() << "*** laundched from pid: " <<  app.applicationPid();
            qDebug() << "*** the pid of the new process is: " <<  process.pid();
            if (process.waitForFinished(-1))
                qDebug() << "*** The process finished ok.";
            else
                qDebug("*** The process failed to finish.");
            qDebug() << "*** The testcases return value is: " << process.exitCode();
            return process.exitCode(); // this is the return value of the actual testcase
        }
    }
}
