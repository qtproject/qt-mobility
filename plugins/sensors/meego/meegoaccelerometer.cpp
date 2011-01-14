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

#include "meegoaccelerometer.h"

char const * const meegoaccelerometer::id("meego.accelerometer");
bool meegoaccelerometer::m_initDone = false;

meegoaccelerometer::meegoaccelerometer(QSensor *sensor)
    : meegosensorbase(sensor)
{
    initSensor<AccelerometerSensorChannelInterface>(m_initDone);
    setDescription(QLatin1String("x, y, and z axes accelerations in m/s^2"));
    setRanges(GRAVITY_EARTH_THOUSANDTH);
    setReading<QAccelerometerReading>(&m_reading);
}

void meegoaccelerometer::slotDataAvailable(const XYZ& data)
{
    // Convert from milli-Gs to meters per second per second
    // Using 1 G = 9.80665 m/s^2
    m_reading.setX(-data.x() * GRAVITY_EARTH_THOUSANDTH);
    m_reading.setY(-data.y() * GRAVITY_EARTH_THOUSANDTH);
    m_reading.setZ(-data.z() * GRAVITY_EARTH_THOUSANDTH);
    m_reading.setTimestamp(data.XYZData().timestamp_);
    newReadingAvailable();
}

void meegoaccelerometer::slotFrameAvailable(const QVector<XYZ>&  frame)
{
    for (int i=0, l=frame.size(); i<l; i++){
        slotDataAvailable(frame.at(i));
    }
}

bool meegoaccelerometer::doConnect(){
    if (m_bufferSize==1?
                QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(slotDataAvailable(const XYZ&))):
                QObject::connect(m_sensorInterface, SIGNAL(frameAvailable(const QVector<XYZ>& )),this, SLOT(slotFrameAvailable(const QVector<XYZ>& ))))
        return true;
    return false;
}


const QString meegoaccelerometer::sensorName(){
    return "accelerometersensor";
}
