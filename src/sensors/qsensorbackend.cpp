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

#include <qsensorbackend.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QSensorBackend
    \ingroup sensors_backend

    \preliminary
    \brief The QSensorBackend class is a sensor implementation.

    Sensors on a device will be represented by sub-classes of
    QSensorBackend.
*/

/*!
    \internal
*/
QSensorBackend::QSensorBackend()
    : m_policy(QSensor::Undefined), m_interval(0), m_sensor(0)
{
}

/*!
    \internal
*/
QSensorBackend::~QSensorBackend()
{
}

/*!
    \fn QSensorBackend::identifier() const

    Returns the identifier for the sensor.
*/

/*!
    \fn QSensorBackend::updatePolicy() const

    Returns the update policy the sensor is using.
*/

/*!
    \fn QSensorBackend::updateInterval() const

    Returns the update interval the sensor is using (only applicable when
    using the TimedUpdates policy).
*/

/*!
    \internal
*/
void QSensorBackend::createdFor(QSensor *sensor, const QSensorId &identifier)
{
    m_sensor = sensor;
    m_identifier = identifier;
}

/*!
    Notify the QSensor class that a new reading is available.
*/
void QSensorBackend::newReadingAvailable()
{
    m_sensor->newReadingAvailable();
}

/*!
    \internal
*/
void QSensorBackend::setUpdatePolicy(QSensor::UpdatePolicy policy, int interval)
{
    m_policy = policy;
    m_interval = interval;
}

/*!
    Returns the suggested interval to use for the current update policy.
*/
int QSensorBackend::suggestedInterval()
{
    switch (m_policy) {
        case QSensor::OccasionalUpdates:
            return 5000;
        case QSensor::InfrequentUpdates:
            return 1000;
        case QSensor::FrequentUpdates:
            return 100;
        case QSensor::TimedUpdates:
            return m_interval;
        default:
            return 0;
    }
}

/*!
    \fn QSensorBackend::supportedPolicies() const

    Returns the update policies that the sensor supports.
*/

/*!
    \fn QSensorBackend::start()

    Start reporting values. Returns true if the sensor was able to start.
*/

/*!
    \fn QSensorBackend::stop()

    Stop reporting values.
*/

#include "moc_qsensorbackend.cpp"
QTM_END_NAMESPACE

