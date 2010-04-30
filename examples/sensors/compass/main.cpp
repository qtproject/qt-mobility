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

#include <QtCore>
#include <qcompass.h>

QTM_USE_NAMESPACE

class CompassFilter : public QCompassFilter
{
public:
    bool filter(QCompassReading *reading)
    {
        int diff = ( reading->timestamp() - stamp );
        stamp = reading->timestamp();
        QTextStream out(stdout);
        out << QString("Compass heading: %1").arg(reading->azimuth(), 3, 'f', 0)
            << QString(" calibration: %1").arg(reading->calibrationLevel(), 3, 'f', 1)
            << QString(" (%1 ms since last, %2 Hz)").arg(diff / 1000, 4).arg( 1000000.0 / diff, 4, 'f', 1) << endl;
        return true;
    }
private:
    qtimestamp stamp;
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QCompass sensor;
    if (!sensor.connectToBackend()) {
        qWarning("No compass available!");
        return 1;
    }
    CompassFilter filter;
    sensor.addFilter(&filter);
    sensor.start();
    if (!sensor.isActive()) {
        qWarning("Compasssensor didn't start!");
        return 1;
    }
    return app.exec();
}
