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

#include "maemo6rotationsensor.h"

const char *maemo6rotationsensor::id("maemo6.rotationsensor");
bool maemo6rotationsensor::m_initDone = false;

maemo6rotationsensor::maemo6rotationsensor(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    const QString sensorName = "rotationsensor";
    if (!m_initDone) {
        //qDBusRegisterMetaType<XYZ>();        initSensor<RotationSensorChannelInterface>("rotationsensor");
        m_remoteSensorManager->loadPlugin(sensorName);
        m_remoteSensorManager->registerSensorInterface<RotationSensorChannelInterface>(sensorName);
        m_initDone = true;
    }
    m_sensorInterface = RotationSensorChannelInterface::controlInterface(sensorName);
    if (!m_sensorInterface)
        m_sensorInterface = const_cast<RotationSensorChannelInterface*>(RotationSensorChannelInterface::listenInterface(sensorName));
    if (m_sensorInterface)
        QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(slotDataAvailable(const XYZ&)));
    else
        qWarning() << "Unable to initialize rotation sensor.";
    setReading<QRotationReading>(&m_reading);
    // metadata
    addDataRate(0, 130); // 43 Hz
    addOutputRange(-179, 180, 1);
    setDescription(QLatin1String("Measures x, y, and z axes rotation in degrees"));
    sensor->setProperty("hasZ", true);
}

void maemo6rotationsensor::slotDataAvailable(const XYZ& data)
{
    m_reading.setX(data.x());
    m_reading.setY(data.y());
    m_reading.setZ(data.z());
    m_reading.setTimestamp(data.XYZData().timestamp_);
    newReadingAvailable();
}