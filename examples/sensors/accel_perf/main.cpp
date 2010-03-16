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
#include <QtTest>
#include <qaccelerometer.h>

QTM_USE_NAMESPACE

class accel_perf : public QObject
{
    Q_OBJECT
private slots:
    void accel_speed_raw();
    void accel_speed_raw_nosignal();
    void accel_speed_raw_signal();
    void accel_speed_filter();
    void accel_speed_filter_nosignal();
    void accel_speed_filter_signal();
};

class receiver : public QObject, public QAccelerometerFilter
{
    Q_OBJECT
public:
    bool filter(QAccelerometerReading *reading)
    {
        Q_UNUSED(reading)
        //qDebug() << "filter";
        return true;
    }
public slots:
    void sensorChanged()
    {
        //qDebug() << "sensorChanged";
    }
};

void accel_perf::accel_speed_raw()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    QVERIFY(sensor.connect());
    sensor.setUpdateInterval(0);
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_raw_nosignal()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    QVERIFY(sensor.connect());
    sensor.setUpdateInterval(0);
    sensor.setSignalEnabled(false);
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_raw_signal()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    QVERIFY(sensor.connect());
    sensor.setUpdateInterval(0);
    receiver r;
    connect(&sensor, SIGNAL(readingChanged()), &r, SLOT(sensorChanged()));
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_filter()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    QVERIFY(sensor.connect());
    sensor.setUpdateInterval(0);
    receiver r;
    sensor.addFilter(&r);
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_filter_nosignal()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    QVERIFY(sensor.connect());
    sensor.setUpdateInterval(0);
    sensor.setSignalEnabled(false);
    receiver r;
    sensor.addFilter(&r);
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_filter_signal()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    QVERIFY(sensor.connect());
    sensor.setUpdateInterval(0);
    receiver r;
    sensor.addFilter(&r);
    connect(&sensor, SIGNAL(readingChanged()), &r, SLOT(sensorChanged()));
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

QTEST_MAIN(accel_perf)

#include "main.moc"

