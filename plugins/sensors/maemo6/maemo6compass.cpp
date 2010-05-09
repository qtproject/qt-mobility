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

#include "maemo6compass.h"

const char *maemo6compass::id("maemo6.compass");
bool maemo6compass::m_initDone = false;

maemo6compass::maemo6compass(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    setReading<QCompassReading>(&m_reading);

    if (!m_initDone) {
        initSensor<CompassSensorChannelInterface>("compasssensor");

        if (m_sensorInterface) {
            QObject::connect(static_cast<CompassSensorChannelInterface*>(m_sensorInterface), SIGNAL(levelChanged(const int&)), this, SLOT(slotLevelChanged(const int&)));
            QObject::connect(static_cast<CompassSensorChannelInterface*>(m_sensorInterface), SIGNAL(degreesChanged(const int&)), this, SLOT(slotDegreesChanged(const int&)));
        } else {
            qWarning() << "Unable to initialize compass sensor.";            
        }

        // metadata TODO accuracy
        addDataRate(43, 43); // 43Hz
        sensor->setDataRate(43);
        addOutputRange(0, 359, 1);
        setDescription(QLatin1String("Measures compass north in degrees"));
 
        m_initDone = true;
    }
}

void maemo6compass::slotLevelChanged(const int& level)
{
    // The scale for level is [0,3], where 3 is the best
    // Qt: Measured as a value from 0 to 1 with higher values being better.
    float l = ((float) level) / 3.0;

    qreal calibrationLevel = l;
    m_reading.setCalibrationLevel(calibrationLevel);
    //m_reading.setTimestamp(level.timestamp());
    m_reading.setTimestamp(createTimestamp()); //TODO: use correct timestamp
    newReadingAvailable();
}

void maemo6compass::slotDegreesChanged(const int& degrees)
{
    // The scale for degrees from sensord is [0,359]
    // Value can be directly used as azimuth
    qreal azimuth = degrees;
    m_reading.setAzimuth(azimuth);
    //m_reading.setTimestamp(degrees.timestamp());
    m_reading.setTimestamp(createTimestamp()); //TODO: use correct timestamp
    newReadingAvailable();
}
