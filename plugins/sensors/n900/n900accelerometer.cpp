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

#include "n900accelerometer.h"
#include <QFile>
#include <QDebug>
#include <time.h>
#include <stdio.h>

char const * const n900accelerometer::id("n900.accelerometer");
char const * const n900accelerometer::filename("/sys/class/i2c-adapter/i2c-3/3-001d/coord");
char const * const n900accelerometer::range("/sys/class/i2c-adapter/i2c-3/3-001d/scale");
char const * const n900accelerometer::rate("/sys/class/i2c-adapter/i2c-3/3-001d/rate");

n900accelerometer::n900accelerometer(QSensor *sensor)
    : n900filebasedsensor(sensor)
{
    setReading<QAccelerometerReading>(&m_reading);
    // Details derived from the kernel driver
    addDataRate(100, 100); // 100Hz
    addOutputRange(-22.418, 22.418, 0.17651); // 2G
    setDescription(QLatin1String("lis302dl"));
}

void n900accelerometer::start()
{
    if (!QFile::exists(QLatin1String(filename)))
        goto error;

    n900filebasedsensor::start();
    return;

error:
    sensorStopped();
}

void n900accelerometer::poll()
{
    // Note that this is a rather inefficient way to generate this data.
    // Ideally the kernel would scale the hardware's values to m/s^2 for us
    // and give us a timestamp along with that data.

    FILE *fd = fopen(filename, "r");
    if (!fd) return;
    int x, y, z;
    int rs = fscanf(fd, "%i %i %i", &x, &y, &z);
    fclose(fd);
    if (rs != 3) return;

    // Convert from milli-Gs to meters per second per second
    // Using 1 G = 9.80665 m/s^2
    qreal ax = x * 0.00980665;
    qreal ay = y * -0.00980665;
    qreal az = z * -0.00980665;

    m_reading.setTimestamp(clock());
    m_reading.setX(ax);
    m_reading.setY(ay);
    m_reading.setZ(az);

    newReadingAvailable();
}

