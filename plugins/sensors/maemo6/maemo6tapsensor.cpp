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

const char *maemo6tapsensor::id("maemo6.tapsensor");
bool maemo6tapsensor::m_initDone = false;

maemo6tapsensor::maemo6tapsensor(QSensor *sensor)
    : maemo6sensorbase(sensor)
{
    char const * const maemo6tapsensor::id("maemo6.tapsensor");
    initSensor<TapSensorChannelInterface>(sensorName, m_initDone);

    if (m_sensorInterface){
        if (!(QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const Tap&)),
                               this, SLOT(slotDataAvailable(const Tap&)))))
            qWarning() << "Unable to connect "<< sensorName;
    }
    else
        qWarning() << "Unable to initialize "<<sensorName;

    setReading<QTapReading>(&m_reading);
    // metadata
    addDataRate(130, 130);
    addDataRate(1, 130); // TODO: this is for testing only
    addOutputRange(0, 9, 1);
    setDescription(QLatin1String("Measures either single or double taps and gives tap direction"));

}


void maemo6tapsensor::start(){
    maemo6sensorbase::start();
    QVariant v = sensor()->property("returnDoubleTapEvents");
    m_isDoubleTapSensor =  v.isValid() && v.toBool()? true: false;
    // Set tap type (single/double)
    m_reading.setDoubleTap(m_isDoubleTapSensor);
}


void maemo6tapsensor::slotDataAvailable(const Tap& data)
{

    if (data.type() == TapData::DoubleTap){
        if (!m_isDoubleTapSensor) return;
    }
    else if (m_isDoubleTapSensor) return;

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
