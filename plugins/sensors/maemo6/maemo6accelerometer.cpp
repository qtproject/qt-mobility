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

#include "maemo6accelerometer.h"

char const * const maemo6accelerometer::id("maemo6.accelerometer");
bool maemo6accelerometer::m_initDone = false;

maemo6accelerometer::maemo6accelerometer(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    const QString sensorName = "accelerometersensor";
    initSensor<AccelerometerSensorChannelInterface>(sensorName, m_initDone);


    if (m_sensorInterface){
        if (!(QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const XYZ&)),
                               this, SLOT(slotDataAvailable(const XYZ&)))))
            qWarning() << "Unable to connect "<< sensorName;
    }
    else
        qWarning() << "Unable to initialize "<<sensorName;

    setReading<QAccelerometerReading>(&m_reading);

}

void maemo6accelerometer::slotDataAvailable(const XYZ& data)
{
    // Convert from milli-Gs to meters per second per second
    // Using 1 G = 9.80665 m/s^2
    qreal ax = -data.x() * GRAVITY_EARTH_THOUSANDTH;
    qreal ay = -data.y() * GRAVITY_EARTH_THOUSANDTH;
    qreal az = -data.z() * GRAVITY_EARTH_THOUSANDTH;

    m_reading.setX(ax);
    m_reading.setY(ay);
    m_reading.setZ(az);
    m_reading.setTimestamp(data.XYZData().timestamp_);
    newReadingAvailable();
}
