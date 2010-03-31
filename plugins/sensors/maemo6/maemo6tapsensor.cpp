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

#include "maemo6tapsensor.h"

#include "sensord/tapsensor_i.h"
#include "sensord/filters/tapdata.h"

const char *maemo6tapsensor::id("maemo6.tapsensor");
bool maemo6tapsensor::m_initDone = false;

maemo6tapsensor::maemo6tapsensor(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    setReading<QTapReading>(&m_reading);

    if (!m_initDone) {
        qDBusRegisterMetaType<Tap>();
        initSensor<TapSensorChannelInterface>("tapsensor");

        if (m_sensorInterface)
            QObject::connect(static_cast<TapSensorChannelInterface*>(m_sensorInterface), SIGNAL(dataAvailable(const Tap&)), this, SLOT(slotDataAvailable(const Tap&)));
        else
            qWarning() << "Unable to initialize tap sensor.";

        // metadata
        addOutputRange(1, 2, 1);    //TODO: valid values?
        setDescription(QLatin1String("Measures single and double taps and gives tap direction"));

        m_initDone = true;
    }

}

void maemo6tapsensor::slotDataAvailable(const Tap& data)
{
    // Set tap type (single/double)
    bool doubleTap = false;
    TapData::Type type = data.type();
    if (TapData::DoubleTap == type) {
        doubleTap = true;
    } else if (TapData::SingleTap == type) {
        doubleTap = false;
    }
    m_reading.setDoubleTap(doubleTap);

    // Set tap direction
    QTapReading::TapDirection tapDirection = QTapReading::Undefined;
    TapData::Direction direction = data.direction();
    if (TapData::X == direction) {
        tapDirection = QTapReading::X;
    } else if (TapData::Y == direction) {
        tapDirection = QTapReading::Y;
    } else if (TapData::Z == direction) {
        tapDirection = QTapReading::Z;
    } else if (TapData::LeftRight == direction) {
        tapDirection = QTapReading::X_Pos;
    } else if (TapData::RightLeft == direction) {
        tapDirection = QTapReading::X_Neg;
    } else if (TapData::TopBottom == direction) {
        tapDirection = QTapReading::Z_Neg;
    } else if (TapData::BottomTop == direction) {
        tapDirection = QTapReading::Z_Pos;
    } else if (TapData::FaceBack == direction) {
        tapDirection = QTapReading::Y_Pos;
    } else if (TapData::BackFace == direction) {
        tapDirection = QTapReading::Y_Neg;
    }
    //m_reading.setTimestamp(data.timestamp());
    m_reading.setTimestamp(createTimestamp()); //TODO: use correct timestamp
    m_reading.setTapDirection(tapDirection);

    newReadingAvailable();
}
