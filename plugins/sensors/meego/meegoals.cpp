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

#include "meegoals.h"

char const * const meegoals::id("meego.als");
bool meegoals::m_initDone = false;

meegoals::meegoals(QSensor *sensor)
    : meegosensorbase(sensor)
{
    initSensor<ALSSensorChannelInterface>(m_initDone);
    setReading<QAmbientLightReading>(&m_reading);
    // metadata
    setDescription(QLatin1String("ambient light intensity given as 5 pre-defined levels"));
    addOutputRange(0, 5, 1);
    addDataRate(10,10);
}

void meegoals::start(){
    Unsigned data(((ALSSensorChannelInterface*)m_sensorInterface)->lux());
    m_reading.setLightLevel(getLightLevel(data.x()));
    m_reading.setTimestamp(data.UnsignedData().timestamp_);
    newReadingAvailable();
    meegosensorbase::start();
}


void meegoals::slotDataAvailable(const Unsigned& data)
{
    QAmbientLightReading::LightLevel level = getLightLevel(data.x());
    if (level != m_reading.lightLevel()) {
        m_reading.setLightLevel(level);
        m_reading.setTimestamp(data.UnsignedData().timestamp_);
        newReadingAvailable();
    }
}

bool meegoals::doConnect(){
    return QObject::connect(m_sensorInterface, SIGNAL(ALSChanged(const Unsigned&)),
                           this, SLOT(slotDataAvailable(const Unsigned&)));
}


QString meegoals::sensorName() const{
    return "alssensor";
}


QAmbientLightReading::LightLevel meegoals::getLightLevel(int lux){
    // Convert from integer to fixed levels
    if (lux < 0) {
        return QAmbientLightReading::Undefined;
    } else if (lux < 10) {
        return QAmbientLightReading::Dark;
    } else if (lux < 50) {
        return QAmbientLightReading::Twilight;
    } else if (lux < 100) {
        return QAmbientLightReading::Light;
    } else if (lux < 150) {
        return QAmbientLightReading::Bright;
    } else {
        return QAmbientLightReading::Sunny;
    }

}
