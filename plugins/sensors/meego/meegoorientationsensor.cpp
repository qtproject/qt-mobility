/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "meegoorientationsensor.h"

#include <posedata.h>

char const * const meegoorientationsensor::id("meego.orientationsensor");
bool meegoorientationsensor::m_initDone = false;

meegoorientationsensor::meegoorientationsensor(QSensor *sensor)
    : meegosensorbase(sensor)
{
    initSensor<OrientationSensorChannelInterface>(m_initDone);
    setReading<QOrientationReading>(&m_reading);
}


void meegoorientationsensor::start(){
    if (m_sensorInterface) {
        Unsigned data(((OrientationSensorChannelInterface*)m_sensorInterface)->orientation());
        m_reading.setOrientation(meegoorientationsensor::getOrientation(data.x()));
        m_reading.setTimestamp(data.UnsignedData().timestamp_);
        newReadingAvailable();
    }
    meegosensorbase::start();
}


void meegoorientationsensor::slotDataAvailable(const Unsigned& data)
{
    m_reading.setOrientation(meegoorientationsensor::getOrientation(data.x()));
    m_reading.setTimestamp(data.UnsignedData().timestamp_);
    newReadingAvailable();
}

bool meegoorientationsensor::doConnect(){
    return QObject::connect(m_sensorInterface, SIGNAL(orientationChanged(const Unsigned&)),
                            this, SLOT(slotDataAvailable(const Unsigned&)));
}

QString meegoorientationsensor::sensorName() const{
    return "orientationsensor";
}

QOrientationReading::Orientation meegoorientationsensor::getOrientation(int orientation){
    switch (orientation) {
    case PoseData::BottomDown: return QOrientationReading::TopUp;
    case PoseData::BottomUp:   return QOrientationReading::TopDown;
    case PoseData::LeftUp:     return QOrientationReading::LeftUp;
    case PoseData::RightUp:    return QOrientationReading::RightUp;
    case PoseData::FaceUp:     return QOrientationReading::FaceUp;
    case PoseData::FaceDown:   return QOrientationReading::FaceDown;
    }
    return QOrientationReading::Undefined;
}
