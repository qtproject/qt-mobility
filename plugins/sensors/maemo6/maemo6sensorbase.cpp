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

#include "maemo6sensorbase.h"
#include <ctime>

SensorManagerInterface* maemo6sensorbase::m_remoteSensorManager = 0;
const float maemo6sensorbase::GRAVITY_EARTH = 9.80665;
const float maemo6sensorbase::GRAVITY_EARTH_THOUSANDTH = 0.00980665;

maemo6sensorbase::maemo6sensorbase(QSensor *sensor)
    : QSensorBackend(sensor), m_sensorRunning(false)
{
    if (!m_remoteSensorManager)
        m_remoteSensorManager = &SensorManagerInterface::instance();
}

maemo6sensorbase::~maemo6sensorbase()
{
   if (m_sensorInterface) {
        stop();
        QObject::disconnect(m_sensorInterface);
        delete m_sensorInterface, m_sensorInterface = 0;
    }
}

void maemo6sensorbase::start()
{
    if (m_sensorRunning)
        return;
    if (m_sensorInterface) {
        int dataRate = sensor()->dataRate();
        int interval = 1000 / dataRate;
        // for testing max speed
        //interval = 1;
        //dataRate = 1000;
        if (dataRate > 0) {
            qDebug() << "Setting data rate" << dataRate << "Hz (interval" << interval << "ms) for" << m_sensorInterface->id();
            m_sensorInterface->setInterval(interval);
        } else {
            qDebug() << "Sensor data rate" << dataRate << "Hz";
        }
        m_sensorInterface->start();
    }
    m_sensorRunning = true;
}

void maemo6sensorbase::stop()
{
    if (!m_sensorRunning)
        return;
    if (m_sensorInterface)
        m_sensorInterface->stop();
    m_sensorRunning = false;
}

qtimestamp maemo6sensorbase::createTimestamp()
{
    timespec stamp;
    clock_gettime(CLOCK_MONOTONIC, &stamp);
    qtimestamp data = stamp.tv_sec;
    data = data * 1000000;
    data = stamp.tv_nsec / 1000 + data;
    return data;
}
