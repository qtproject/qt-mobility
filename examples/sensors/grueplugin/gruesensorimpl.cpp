/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gruesensorimpl.h"
#include <QDebug>

const char *gruesensorimpl::id("grue.gruesensor");

gruesensorimpl::gruesensorimpl(QSensor *sensor)
    : QSensorBackend(sensor)
{
    lightSensor = new QAmbientLightSensor(this);
    lightSensor->addFilter(this);
    lightSensor->connectToBackend();

    setReading<GrueSensorReading>(&m_reading);
    setDataRates(lightSensor);
    addOutputRange(0, 1, 0.1);
    setDescription(QLatin1String("Grue Sensor"));
}

void gruesensorimpl::start()
{
    lightSensor->setDataRate(sensor()->dataRate());
    lightSensor->start();
}

void gruesensorimpl::stop()
{
    lightSensor->stop();
}

bool gruesensorimpl::filter(QAmbientLightReading *reading)
{
    qreal chance;

    switch (reading->lightLevel()) {
    case QAmbientLightReading::Undefined:
        chance = 0.5; // No idea... call it 50/50
        break;
    case QAmbientLightReading::Dark:
        chance = 1.0;
        break;
    case QAmbientLightReading::Twilight:
        chance = 0.1;
        break;
    default:
        chance = 0.0;
        break;
    }

    if (chance != m_reading.chanceOfBeingEaten()) {
        m_reading.setTimestamp(reading->timestamp());
        m_reading.setChanceOfBeingEaten(chance);

        newReadingAvailable();
    }

    return true;
}

