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

#include "meegogyroscope.h"

char const * const meegogyroscope::id("meego.gyroscope");
const float meegogyroscope::MILLI = 0.001;
bool meegogyroscope::m_initDone = false;

meegogyroscope::meegogyroscope(QSensor *sensor)
    : meegosensorbase(sensor)
{
    initSensor<GyroscopeSensorChannelInterface>(m_initDone);
    setDescription(QLatin1String("angular velocities around x, y, and z axis in degrees per second"));
    setRanges(MILLI);
    setReading<QGyroscopeReading>(&m_reading);
    addDataRate(10, 10);
    addDataRate(50, 50);
}

void meegogyroscope::slotDataAvailable(const XYZ& data)
{
    m_reading.setX((qreal)(data.x()*MILLI));
    m_reading.setY((qreal)(data.y()*MILLI));
    m_reading.setZ((qreal)(data.z()*MILLI));
    m_reading.setTimestamp(data.XYZData().timestamp_);
    newReadingAvailable();
}

void meegogyroscope::slotFrameAvailable(const QVector<XYZ>&  frame)
{
    for (int i=0, l=frame.size(); i<l; i++){
        slotDataAvailable(frame.at(i));
    }
}

bool meegogyroscope::doConnect(){
    if (m_bufferSize==1)
        return QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(slotDataAvailable(const XYZ&)));
    return QObject::connect(m_sensorInterface, SIGNAL(frameAvailable(const QVector<XYZ>& )),this, SLOT(slotFrameAvailable(const QVector<XYZ>& )));
}

QString meegogyroscope::sensorName() const{
    return "gyroscopesensor";
}

qreal meegogyroscope::correctionFactor() const{return MILLI;}
