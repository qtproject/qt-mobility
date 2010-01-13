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

#include "n900accelerometer.h"
#include <QDebug>

n900accelerometer::n900accelerometer()
    : m_timerid(0)
    , m_filename(ACCELEROMETER_FILE)
{
}

QSensor::UpdatePolicies n900accelerometer::supportedPolicies() const
{
    return (QSensor::OccasionalUpdates |
            QSensor::InfrequentUpdates |
            QSensor::FrequentUpdates |
            QSensor::TimedUpdates |
            QSensor::PolledUpdates);
}

bool n900accelerometer::start()
{
    if (m_timerid)
        return false;

    if (suggestedInterval())
        m_timerid = startTimer(suggestedInterval());
    return true;
}

void n900accelerometer::stop()
{
    if (m_timerid) {
        killTimer(m_timerid);
        m_timerid = 0;
    }
}

void n900accelerometer::timerEvent(QTimerEvent * /*event*/)
{
    poll();
}

void n900accelerometer::poll()
{
    // Note that this is a rather inefficient way to generate this data.
    // Ideally the kernel would scale the hardware's values to m/s^2 for us
    // and give us a timestamp along with that data.

    QDateTime timestamp = QDateTime::currentDateTime();
    FILE *fd = fopen(m_filename, "r");
    if (!fd) return;
    int x, y, z;
    int rs = fscanf(fd, "%i %i %i", &x, &y, &z);
    fclose(fd);
    if (rs != 3) return;

    // Convert from milli-Gs to meters per second per second
    // Using 1 G = 9.80665 m/s^2
    float ax = x * 0.00980665;
    float ay = y * 0.00980665;
    float az = z * 0.00980665;
    m_lastReading = QAccelerometerReading(timestamp, ax, ay, az);
    if (updatePolicy() != QSensor::PolledUpdates)
        newReadingAvailable();
}

QAccelerometerReading n900accelerometer::currentReading()
{
    if (updatePolicy() == QSensor::PolledUpdates)
        poll();
    return m_lastReading;
}

