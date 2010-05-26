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

#include "n900lightsensor.h"
#include <QFile>
#include <QDebug>
#include <time.h>

char const * const n900lightsensor::id("n900.ambientlight");
char const * const n900lightsensor::filename("/sys/class/i2c-adapter/i2c-2/2-0029/lux");

n900lightsensor::n900lightsensor(QSensor *sensor)
    : n900filebasedsensor(sensor)
{
    setReading<QAmbientLightReading>(&m_reading);
    // Sensor takes 12-400ms to complete one reading and is triggered by
    // a read of the /sys file (no interrupt/timing loop/etc. is used).
    // Since no continuous operation is possible, don't set a data rate.
    addDataRate(2, 2); // Close enough to 2 Hz
    sensor->setDataRate(2);
    setDescription(QLatin1String("tsl2563"));
}

void n900lightsensor::start()
{
    if (!QFile::exists(QLatin1String(filename)))
        goto error;

    n900filebasedsensor::start();
    return;

error:
    sensorStopped();
}

struct lux_limit {
    int min;
    int max;
};

// Defines the min and max lux values that a given level has.
// These are used to add histeresis to the sensor.
// If the previous level is below a level, the lux must be at or above the minimum.
// If the previous level is above a level, the lux muyt be at or below the maximum.
static lux_limit limits[] = {
    { 0,    0    }, // Undefined (not used)
    { 0,    5    }, // Dark
    { 10,   50   }, // Twilight
    { 100,  200  }, // Light
    { 500,  2000 }, // Bright
    { 5000, 0    }  // Sunny
};

#if 0
// Used for debugging
static QString light_level(int level)
{
    switch (level) {
    case 1:
        return QLatin1String("Dark");
    case 2:
        return QLatin1String("Twilight");
    case 3:
        return QLatin1String("Light");
    case 4:
        return QLatin1String("Bright");
    case 5:
        return QLatin1String("Sunny");
    default:
        return QLatin1String("Undefined");
    }
}
#endif

void n900lightsensor::poll()
{
    FILE *fd = fopen(filename, "r");
    if (!fd) return;
    int lux;
    int rs = fscanf(fd, "%i", &lux);
    fclose(fd);
    if (rs != 1) return;

    // It's unweildly dealing with these constants so make some
    // local aliases that are shorter. This makes the code below
    // much easier to read.
    enum {
        Undefined = QAmbientLightReading::Undefined,
        Dark = QAmbientLightReading::Dark,
        Twilight = QAmbientLightReading::Twilight,
        Light = QAmbientLightReading::Light,
        Bright = QAmbientLightReading::Bright,
        Sunny = QAmbientLightReading::Sunny
    };

    int lightLevel = m_reading.lightLevel();
    // Check for change direction to allow for histeresis
    if      (lightLevel < Sunny    && lux >= limits[Sunny   ].min) lightLevel = Sunny;
    else if (lightLevel < Bright   && lux >= limits[Bright  ].min) lightLevel = Bright;
    else if (lightLevel < Light    && lux >= limits[Light   ].min) lightLevel = Light;
    else if (lightLevel < Twilight && lux >= limits[Twilight].min) lightLevel = Twilight;
    else if (lightLevel < Dark     && lux >= limits[Dark    ].min) lightLevel = Dark;
    else if (lightLevel > Dark     && lux <= limits[Dark    ].max) lightLevel = Dark;
    else if (lightLevel > Twilight && lux <= limits[Twilight].max) lightLevel = Twilight;
    else if (lightLevel > Light    && lux <= limits[Light   ].max) lightLevel = Light;
    else if (lightLevel > Bright   && lux <= limits[Bright  ].max) lightLevel = Bright;

    //qDebug() << "lightLevel" << light_level(lightLevel) << "lux" << lux;

    if (static_cast<int>(m_reading.lightLevel()) != lightLevel) {
        m_reading.setTimestamp(clock());
        m_reading.setLightLevel(static_cast<QAmbientLightReading::LightLevel>(lightLevel));

        newReadingAvailable();
    }
}

