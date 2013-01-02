/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "meegorotationsensor.h"

char const * const meegorotationsensor::id("meego.rotationsensor");
bool meegorotationsensor::m_initDone = false;

meegorotationsensor::meegorotationsensor(QSensor *sensor)
    : meegosensorbase(sensor)
{
    initSensor<RotationSensorChannelInterface>(m_initDone);
    setReading<QRotationReading>(&m_reading);
    sensor->setProperty("hasZ", true);
}

void meegorotationsensor::slotDataAvailable(const XYZ& data)
{
    m_reading.setX(data.x());
    m_reading.setY(data.y());
    m_reading.setZ(data.z());
    m_reading.setTimestamp(data.XYZData().timestamp_);
    newReadingAvailable();
}

void meegorotationsensor::slotFrameAvailable(const QVector<XYZ>&  frame)
{
    for (int i=0, l=frame.size(); i<l; i++){
        slotDataAvailable(frame.at(i));
    }
}

bool meegorotationsensor::doConnect(){
    if (m_bufferSize==1)
       return QObject::connect(m_sensorInterface, SIGNAL(dataAvailable(const XYZ&)), this, SLOT(slotDataAvailable(const XYZ&)));
    return QObject::connect(m_sensorInterface, SIGNAL(frameAvailable(const QVector<XYZ>& )),this, SLOT(slotFrameAvailable(const QVector<XYZ>& )));
}

QString meegorotationsensor::sensorName() const{
    return "rotationsensor";
}
