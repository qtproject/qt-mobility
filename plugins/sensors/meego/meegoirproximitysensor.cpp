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

#include "meegoirproximitysensor.h"
#define RM680_PS "/dev/bh1770glc_ps"

char const * const meegoirproximitysensor::id("meego.irproximitysensor");
bool meegoirproximitysensor::m_initDone = false;



meegoirproximitysensor::meegoirproximitysensor(QSensor *sensor)
    : meegosensorbase(sensor)
{
    initSensor<ProximitySensorChannelInterface>(m_initDone);
    setReading<QIRProximityReading>(&m_reading);
    setDescription(QLatin1String("reflectance as percentage (%) of maximum"));
    addOutputRange(0, 100, 1);
    addDataRate(10,10);
    rangeMax = QFile::exists(RM680_PS)?255:1023;
}

#ifdef Q_WS_MAEMO_6
void meegoirproximitysensor::slotDataAvailable(const Proximity& proximity){
    m_reading.setReflectance((float)proximity.reflectance()*100 / rangeMax);
    m_reading.setTimestamp(proximity.UnsignedData().timestamp_);
    newReadingAvailable();
}
#endif


bool meegoirproximitysensor::doConnect(){
#ifdef Q_WS_MAEMO_6
    return QObject::connect(m_sensorInterface, SIGNAL(reflectanceDataAvailable(const Proximity&)),
                            this, SLOT(slotDataAvailable(const Proximity&)));
#endif
    return false;
}


QString meegoirproximitysensor::sensorName() const{
    return "proximitysensor";
}


