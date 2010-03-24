/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

