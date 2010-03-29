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

#include "test_sensorimpl.h"
#include <qaccelerometer.h>
#include <QDebug>

const char *testsensorimpl::id("test sensor impl");

testsensorimpl::testsensorimpl(QSensor *sensor)
    : QSensorBackend(sensor)
{
    setReading<TestSensorReading>(&m_reading);
    setDescription("sensor description");
    addOutputRange(0, 1, 0.5);
    addOutputRange(0, 2, 1);
    QString doThis = sensor->property("doThis").toString();
    if (doThis == "rates(0)") {
        setDataRates(0);
    } else if (doThis == "rates") {
        QAccelerometer *acc = new QAccelerometer(this);
        acc->connectToBackend();
        setDataRates(acc);
        if (sensor->availableDataRates().count()) {
            sensor->setDataRate(sensor->availableDataRates().first().first);
        } else {
            addDataRate(100, 100);
            sensor->setDataRate(100);
        }
    } else {
        addDataRate(100, 100);
        sensor->setDataRate(100);
    }
    reading();
}

void testsensorimpl::start()
{
    QString doThis = sensor()->property("doThis").toString();
    if (doThis == "busy")
        sensorBusy();
    else if (doThis == "stop")
        sensorStopped();
    else if (doThis == "error")
        sensorError(1);
    else if (doThis == "setFalse") {
        m_reading.setTimestamp(1);
        m_reading.setTest(false);
        newReadingAvailable();
    } else {
        m_reading.setTimestamp(2);
        m_reading.setTest(true);
        newReadingAvailable();
    }
}

void testsensorimpl::stop()
{
}

