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
#include <qaccelerometer.h>

QTM_USE_NAMESPACE

QAccelerometer *accelerometer;

class AccelerometerFilter : public QAccelerometerFilter
{
public:
    bool filter(QAccelerometerReading *reading)
    {
        qDebug() << "acceleration: "
                 << QString().sprintf("%0.2f %0.2f %0.2f",
                         reading->x(),
                         reading->y(),
                         reading->z());
        return false; // don't store the reading in the sensor
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAccelerometer sensor;
    accelerometer = &sensor;
    AccelerometerFilter filter;
    sensor.addFilter(&filter);
    sensor.setUpdateInterval(100); // as fast as the sensor can go!
    sensor.start();

    if (!sensor.isActive()) {
        qWarning("Accelerometer didn't start!");
        return 1;
    }

    return app.exec();
}

