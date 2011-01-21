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

#include "meegotapsensor.h"

char const * const meegotapsensor::id("meego.tapsensor");
bool meegotapsensor::m_initDone = false;

meegotapsensor::meegotapsensor(QSensor *sensor)
    : meegosensorbase(sensor), m_isOnceStarted(false)
{
    initSensor<TapSensorChannelInterface>(m_initDone);
    setReading<QTapReading>(&m_reading);
}


void meegotapsensor::start(){
    QVariant v = sensor()->property("returnDoubleTapEvents");
    bool isDoubleTapSensor = m_isDoubleTapSensor;
    if (!(v.isValid())){
        sensor()->setProperty("returnDoubleTapEvents", true); //by default doubles
        m_isDoubleTapSensor = true;
    }
    else m_isDoubleTapSensor = v.toBool();

    if (!m_isOnceStarted || (m_isOnceStarted && isDoubleTapSensor != m_isDoubleTapSensor))
        ((TapSensorChannelInterface*)m_sensorInterface)->
                setTapType(m_isDoubleTapSensor?TapSensorChannelInterface::Double:TapSensorChannelInterface::Single);

    meegosensorbase::start();
    // Set tap type (single/double)
    m_reading.setDoubleTap(m_isDoubleTapSensor);
    m_isOnceStarted = true;
}


void meegotapsensor::slotDataAvailable(const Tap& data)
{
    if (data.type() == TapData::DoubleTap){
        if (!m_isDoubleTapSensor) return;
    }
    else if (m_isDoubleTapSensor) return;

    // Set tap direction
    QTapReading::TapDirection o;
    switch (data.direction()) {
    case TapData::X:         o = QTapReading::X_Both;    break;
    case TapData::Y:         o = QTapReading::Y_Both;    break;
    case TapData::Z:         o = QTapReading::Z_Both;    break;
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


bool meegotapsensor::doConnect(){
    if (!(QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const Tap&)),
                           this, SLOT(slotDataAvailable(const Tap&))))){
        return false;
    }
    return true;
}


const QString meegotapsensor::sensorName(){
    return "tapsensor";
}
