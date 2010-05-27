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

#include "maemo6tapsensor.h"

char const * const maemo6tapsensor::id("maemo6.tapsensor");
bool maemo6tapsensor::m_initDone = false;

maemo6tapsensor::maemo6tapsensor(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    const QString sensorName = "tapsensor";
    if (!m_initDone) {
        //qDBusRegisterMetaType<Tap>();        initSensor<TapSensorChannelInterface>("tapsensor");
        m_remoteSensorManager->loadPlugin(sensorName);
        m_remoteSensorManager->registerSensorInterface<TapSensorChannelInterface>(sensorName);
        m_initDone = true;
    }
    m_sensorInterface = TapSensorChannelInterface::controlInterface(sensorName);
    if (!m_sensorInterface)
        m_sensorInterface = const_cast<TapSensorChannelInterface*>(TapSensorChannelInterface::listenInterface(sensorName));
    if (m_sensorInterface)
        QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const Tap&)), this, SLOT(slotDataAvailable(const Tap&)));
    else
        qWarning() << "Unable to initialize tap sensor.";
    setReading<QTapReading>(&m_reading);
    // metadata
    addDataRate(0, 0);
    addDataRate(130, 130);
    addDataRate(0, 130); // TODO: this is for testing only
    addOutputRange(0, 9, 1);
    setDescription(QLatin1String("Measures single and double taps and gives tap direction"));
}

void maemo6tapsensor::slotDataAvailable(const Tap& data)
{
    // Set tap type (single/double)
    bool doubleTap;
    switch (data.type()) {
        case TapData::DoubleTap: doubleTap = true; break;
        case TapData::SingleTap: doubleTap = false; break;
        default:                 doubleTap = false;
    }
    QVariant v = sensor()->property("returnDoubleTapEvents");
    if (v.isValid() && v.toBool() == false) {
        if (doubleTap)
            return;
    } else {
        if (! doubleTap)
            return;
    }
    m_reading.setDoubleTap(doubleTap);

    // Set tap direction
    QTapReading::TapDirection o;
    switch (data.direction()) {
        case TapData::X:         o = QTapReading::X;         break;
        case TapData::Y:         o = QTapReading::Y;         break;
        case TapData::Z:         o = QTapReading::Z;         break;
        case TapData::LeftRight: o = QTapReading::X_Pos;     break;
        case TapData::RightLeft: o = QTapReading::X_Neg;     break;
        case TapData::TopBottom: o = QTapReading::Z_Neg;     break;
        case TapData::BottomTop: o = QTapReading::Z_Pos;     break;
        case TapData::FaceBack:  o = QTapReading::Y_Pos;     break;
        case TapData::BackFace:  o = QTapReading::Y_Neg;     break;
        default:                 o = QTapReading::Undefined;
    }
    m_reading.setTapDirection(o);

    m_reading.setTimestamp(data.tapData().timestamp_);
    newReadingAvailable();
}
