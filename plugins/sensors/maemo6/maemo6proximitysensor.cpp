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

#include "maemo6proximitysensor.h"

const char *maemo6proximitysensor::id("maemo6.proximity");
bool maemo6proximitysensor::m_initDone = false;

maemo6proximitysensor::maemo6proximitysensor(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    setReading<QProximityReading>(&m_reading);

    if (!m_initDone) {
        initSensor<ProximitySensorChannelInterface>("proximitysensor");

        if (m_sensorInterface)
            QObject::connect(static_cast<ProximitySensorChannelInterface*>(m_sensorInterface), SIGNAL(dataAvailable(const int&)), this, SLOT(slotDataAvailable(const int&)));
        else
            qWarning() << "Unable to initialize proximity sensor.";

        // close definition in meters - may be used as metadata even the sensor gives true/false values 
        addOutputRange(0, 1, 1);
        setDescription(QLatin1String("Measures if a living object is in proximity or not"));

        m_initDone = true;
    }
}

void maemo6proximitysensor::slotDataAvailable(const int& data)
{
    bool close;
    if (data)
        close = true;
    else
        close = false;

    m_reading.setClose(close);
    //m_reading.setTimestamp(data.timestamp());
    m_reading.setTimestamp(createTimestamp()); //TODO: use correct timestamp
    newReadingAvailable();
}