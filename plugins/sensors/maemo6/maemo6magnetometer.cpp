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

#include "maemo6magnetometer.h"

const char *maemo6magnetometer::id("maemo6.magnetometer");
bool maemo6magnetometer::m_initDone = false;

maemo6magnetometer::maemo6magnetometer(QSensor *sensor)
    : maemo6sensorbase(sensor), m_sensor(sensor)
{
    setReading<QMagnetometerReading>(&m_reading);

    if (!m_initDone) {
        qDBusRegisterMetaType<MagneticField>();
        initSensor<MagnetometerSensorChannelInterface>("magnetometersensor");

        if (m_sensorInterface)
            QObject::connect(static_cast<const MagnetometerSensorChannelInterface*>(m_sensorInterface), SIGNAL(dataAvailable(const MagneticField&)), this, SLOT(slotDataAvailable(const MagneticField&)));
        else
            qWarning() << "Unable to initialize magnetometer sensor.";

        // metadata
        setDescription(QLatin1String("Magnetic flux density measured in teslas"));

        m_initDone = true;
    }
}

void maemo6magnetometer::slotDataAvailable(const MagneticField& data)
{
    QVariant v = m_sensor->property("returnGeoValues");
    if (v.isValid() && v.toBool()) {
        m_reading.setX(data.x());
        m_reading.setY(data.y());
        m_reading.setZ(data.z());
        m_reading.setCalibrationLevel(data.level());
    } else {
        m_reading.setX(data.rx());
        m_reading.setY(data.ry());
        m_reading.setZ(data.rz());
        m_reading.setCalibrationLevel(1);
    }
    //m_reading.setTimestamp(data.timestamp());
    m_reading.setTimestamp(createTimestamp()); //TODO: use correct timestamp
    newReadingAvailable();
}
