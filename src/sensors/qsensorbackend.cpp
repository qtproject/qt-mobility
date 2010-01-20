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
#include "qsensor_p.h"

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
QSensorBackend::QSensorBackend(QSensor *sensor)
    : m_sensor(sensor)
{
}

/*!
    \internal
*/
QSensorBackend::~QSensorBackend()
{
}

/*!
    Set the supported \a policies for this sensor.
    If this isn't called the sensor will not report
    any supported policies to the user.
*/
void QSensorBackend::setSupportedUpdatePolicies(QSensor::UpdatePolicies policies)
{
    QSensorPrivate *d = m_sensor->d_func();
    d->supportedUpdatePolicies = policies;
}

/*!
    \internal
*/
void QSensorBackend::setReadings(QSensorReading *filter_reading, QSensorReading *cache_reading)
{
    QSensorPrivate *d = m_sensor->d_func();
    d->filter_reading = filter_reading;
    d->cache_reading = cache_reading;
}

/*!
    Notify the QSensor class that a new reading is available.
*/
void QSensorBackend::newReadingAvailable()
{
    if (m_sensor->updatePolicy() != QSensor::PolledUpdates)
        m_sensor->newReadingAvailable();
}

/*!
    \fn QSensorBackend::start()

    Start reporting values.
*/

/*!
    \fn QSensorBackend::stop()

    Stop reporting values.
*/

/*!
    \fn QSensorBackend::poll()

    Poll the sensor for a reading.
*/

/*!
    \fn QSensorBackend::setReading(T *reading)

    A template function. It tells the sensor what \a reading
    class the backend will write values into.

    The backend must not lose this reference as it cannot get
    this reference back from the sensor. This is not
    the same value that is returned by QSensor::reading().

    This function should be called like this.

    \code
    setReading<MyReadingClass>(&my_reading_class);
    \endcode
*/

#include "moc_qsensorbackend.cpp"
QTM_END_NAMESPACE

