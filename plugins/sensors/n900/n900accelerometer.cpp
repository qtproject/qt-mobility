/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    addDataRate(400, 400); // 400Hz
    addOutputRange(-22.418, 22.418, 0.17651); // 2G
    addOutputRange(-89.672, 89.672, 0.70608); // 8G
    setDescription(QLatin1String("lis302dl"));
}

void n900accelerometer::start()
{
    FILE *fd;

    if (!QFile::exists(QLatin1String(filename)))
        goto error;

    // Configure the range
    if (sensor()->outputRange() != -1) {
        fd = fopen(range, "w");
        if (!fd) goto error;
        if (sensor()->outputRange() == 0)
            fprintf(fd, "normal\n");
        else
            fprintf(fd, "full\n");
        fclose(fd);
    }

    // Configure the rate
    if (sensor()->dataRate() != 0) {
        fd = fopen(rate, "w");
        if (!fd) goto error;
        fprintf(fd, "%d\n", sensor()->dataRate());
        fclose(fd);
    }

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

