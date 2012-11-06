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

#include "meegomagnetometer.h"

char const * const meegomagnetometer::id("meego.magnetometer");
bool meegomagnetometer::m_initDone = false;
const float meegomagnetometer::NANO = 0.000000001;


meegomagnetometer::meegomagnetometer(QSensor *sensor)
    : meegosensorbase(sensor)
{
    initSensor<MagnetometerSensorChannelInterface>(m_initDone);
    setDescription(QLatin1String("magnetic flux density in teslas (T)"));
    setRanges(NANO);
    setReading<QMagnetometerReading>(&m_reading);
}

void meegomagnetometer::start(){
    QVariant v = sensor()->property("returnGeoValues");
    if (!(v.isValid())){
        sensor()->setProperty("returnGeoValues", false); //Set to false (the default) to return raw magnetic flux density
        m_isGeoMagnetometer=false;
    }
    else m_isGeoMagnetometer =  v.toBool();

    meegosensorbase::start();
}

void meegomagnetometer::slotDataAvailable(const MagneticField& data)
{
    //nanoTeslas given, divide with 10^9 to get Teslas
    m_reading.setX( NANO * (m_isGeoMagnetometer?data.x():data.rx()));
    m_reading.setY( NANO * (m_isGeoMagnetometer?data.y():data.ry()));
    m_reading.setZ( NANO * (m_isGeoMagnetometer?data.z():data.rz()));
    m_reading.setCalibrationLevel( m_isGeoMagnetometer?((float) data.level()) / 3.0 :1);
    m_reading.setTimestamp(data.timestamp());
    newReadingAvailable();
}


void meegomagnetometer::slotFrameAvailable(const QVector<MagneticField>&   frame)
{
    for (int i=0, l=frame.size(); i<l; i++){
        slotDataAvailable(frame.at(i));
    }
}

bool meegomagnetometer::doConnect(){
    if (m_bufferSize==1)
        return QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const MagneticField&)), this, SLOT(slotDataAvailable(const MagneticField&)));
     return QObject::connect(m_sensorInterface, SIGNAL(frameAvailable(const QVector<MagneticField>& )),this, SLOT(slotFrameAvailable(const QVector<MagneticField>& )));
}

QString meegomagnetometer::sensorName() const{
    return "magnetometersensor";
}

qreal meegomagnetometer::correctionFactor() const{return meegomagnetometer::NANO;}
