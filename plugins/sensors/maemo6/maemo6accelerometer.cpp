/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "maemo6accelerometer.h"

const char *maemo6accelerometer::id("maemo6.accelerometer");
bool maemo6accelerometer::m_initDone = false;

maemo6accelerometer::maemo6accelerometer(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    setReading<QAccelerometerReading>(&m_reading);

    if (!m_initDone) {
        qDBusRegisterMetaType<XYZ>();

        initSensor<AccelerometerSensorChannelInterface>("accelerometersensor");

        if (m_sensorInterface)
            QObject::connect(static_cast<AccelerometerSensorChannelInterface*>(m_sensorInterface), SIGNAL(dataAvailable(const XYZ&)), this, SLOT(slotDataAvailable(const XYZ&)));
        else
            qWarning() << "Unable to initialize accelerometer sensor.";

        // adding metadata
        addDataRate(100, 100); // 100Hz
        //addDataRate(400, 400); // 400Hz

        // accuracy - or resolution???
        // 2^8 = 256    256/2 - 1 = 127
        addOutputRange(-2*GRAVITY_EARTH, 2*GRAVITY_EARTH, 2*GRAVITY_EARTH/127); // 2G
        addOutputRange(-8*GRAVITY_EARTH, 8*GRAVITY_EARTH, 8*GRAVITY_EARTH/127); // 8G
        setDescription(QLatin1String("Measures x, y, and z axes accelerations in m/s^2"));

        m_initDone = true;
    }
}

void maemo6accelerometer::slotDataAvailable(const XYZ& data)
{
    // Convert from milli-Gs to meters per second per second
    // Using 1 G = 9.80665 m/s^2
    qreal ax = data.x() * GRAVITY_EARTH_THOUSANDTH;
    qreal ay = - data.y() * GRAVITY_EARTH_THOUSANDTH;
    qreal az = - data.z() * GRAVITY_EARTH_THOUSANDTH;

    m_reading.setX(ax);
    m_reading.setY(ay);
    m_reading.setZ(az);
    //m_reading.setTimestamp(data.timestamp());
    m_reading.setTimestamp(createTimestamp()); //TODO: use correct timestamp
    newReadingAvailable();
}

