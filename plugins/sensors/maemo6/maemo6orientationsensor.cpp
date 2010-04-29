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

#include "maemo6orientationsensor.h"

#include <posedata.h>

const char *maemo6orientationsensor::id("maemo6.orientationsensor");
bool maemo6orientationsensor::m_initDone = false;

maemo6orientationsensor::maemo6orientationsensor(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    setReading<QOrientationReading>(&m_reading);

    if (!m_initDone) {
        initSensor<OrientationSensorChannelInterface>("orientationsensor");

        if (m_sensorInterface)
            QObject::connect(static_cast<OrientationSensorChannelInterface*>(m_sensorInterface), SIGNAL(orientationChanged(const Unsigned&)), this, SLOT(slotOrientationChanged(const Unsigned&)));
        else
            qWarning() << "Unable to initialize orientation sensor.";

        // metadata
        addDataRate(142, 142); // 142Hz
        sensor->setDataRate(142);
        addOutputRange(0, 6, 1);
        setDescription(QLatin1String("Measures orientation of the device screen as 6 pre-defined levels"));

        m_initDone = true;
    }
}

void maemo6orientationsensor::slotOrientationChanged(const Unsigned& data)
{
    QOrientationReading::Orientation o;
    switch (data.x()) {
        case PoseData::BottomDown: o = QOrientationReading::TopUp;     break;
        case PoseData::BottomUp:   o = QOrientationReading::TopDown;   break;
        case PoseData::LeftUp:     o = QOrientationReading::LeftUp;    break;
        case PoseData::RightUp:    o = QOrientationReading::RightUp;   break;
        case PoseData::FaceUp:     o = QOrientationReading::FaceUp;    break;
        case PoseData::FaceDown:   o = QOrientationReading::FaceDown;  break;
        default:                   o = QOrientationReading::Undefined;
    }
    m_reading.setOrientation(o);
    m_reading.setTimestamp(data.UnsignedData().timestamp_);
    newReadingAvailable();
}
