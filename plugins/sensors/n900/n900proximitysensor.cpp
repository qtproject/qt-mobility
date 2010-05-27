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

#include "n900proximitysensor.h"
#include <QFile>
#include <QDebug>
#include <string.h>
#include <time.h>

char const * const n900proximitysensor::id("n900.proximity");
char const * const n900proximitysensor::filename("/sys/bus/platform/devices/proximity/state");

n900proximitysensor::n900proximitysensor(QSensor *sensor)
    : n900filebasedsensor(sensor)
{
    setReading<QProximityReading>(&m_reading);
    addDataRate(100, 100); // 100Hz
    sensor->setDataRate(100); // default is 10Hz
}

void n900proximitysensor::start()
{
    if (!QFile::exists(QLatin1String(filename)))
        goto error;

    n900filebasedsensor::start();
    return;

error:
    sensorStopped();
}

void n900proximitysensor::poll()
{
    FILE *fd = fopen(filename, "r");
    if (!fd) return;
    char buffer[20];
    int rs = fscanf(fd, "%s", buffer);
    fclose(fd);
    if (rs != 1) return;

    bool close;
    if (strcmp(buffer, "closed") == 0) {
        close = true;
    } else {
        close = false;
    }

    if (close != m_reading.close()) {
        m_reading.setTimestamp(clock());
        m_reading.setClose(close);
        newReadingAvailable();
    }
}

