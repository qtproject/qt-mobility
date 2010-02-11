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

#include "qsensorbackend.h"
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
    Notify the QSensor class that a new reading is available.
*/
void QSensorBackend::newReadingAvailable()
{
    QSensorPrivate *d = m_sensor->d_func();

    // Copy the values from the device reading to the filter reading
    d->filter_reading->copyValuesFrom(d->device_reading);

    for (QFilterList::const_iterator it = d->filters.constBegin(); it != d->filters.constEnd(); ++it) {
        QSensorFilter *filter = (*it);
        if (!filter->filter(d->filter_reading))
            return;
    }

    // Copy the values from the filter reading to the cached reading
    d->cache_reading->copyValuesFrom(d->filter_reading);

    // FIXME this shouldn't be turned off (it's off for a test... should be a ifdef)
#if 0
    if (d->updatePolicy == QSensor::PolledUpdates)
        return; // We don't emit the signal if we're polling
#endif

    if (d->signalEnabled)
        Q_EMIT m_sensor->readingChanged();
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
    If the backend has lost its reference to the reading
    it can call this method to get the address.

    Note that you will need to down-cast to the appropriate
    type.

    \sa setReading()
*/
QSensorReading *QSensorBackend::reading() const
{
    QSensorPrivate *d = m_sensor->d_func();
    return d->device_reading;
}

/*!
    \fn QSensorBackend::setReading(T *reading)

    This function is called to initialize the \a reading
    classes used for a sensor.

    If your backend has already allocated a reading you
    should pass the address of this to the function.
    Otherwise you should pass 0 and the function will
    return the address of the reading your backend
    should use when it wants to notify the sensor API
    of new readings.

    Note that this is a template function so it should
    be called with the appropriate type.

    \code
    class MyBackend : public QSensorBackend
    {
        QAccelerometerReading m_reading;
    public:
        MyBackend(QSensor *sensor)
            : QSensorBackend(sensor)
        {
            setReading<QAccelerometerReading>(&m_reading);
        }

        ...
    \endcode

    Note that this function must be called or you will
    not be able to send readings to the front end.

    If you do not wish to store the address of the reading
    you may use the reading() method to get it again later.

    \code
    class MyBackend : public QSensorBackend
    {
    public:
        MyBackend(QSensor *sensor)
            : QSensorBackend(sensor)
        {
            setReading<QAccelerometerReading>(0);
        }

        void poll()
        {
            qtimestamp timestamp;
            qreal x, y, z;
            ...
            QAccelerometerReading *reading = static_cast<QAccelerometerReading*>(reading());
            reading->setTimestamp(timestamp);
            reading->setX(x);
            reading->setY(y);
            reading->setZ(z);
        }

        ...
    \endcode

    \sa reading()
*/

/*!
    \internal
*/
void QSensorBackend::setReadings(QSensorReading *device, QSensorReading *filter, QSensorReading *cache)
{
    QSensorPrivate *d = m_sensor->d_func();
    d->device_reading = device;
    d->filter_reading = filter;
    d->cache_reading = cache;
}

#include "moc_qsensorbackend.cpp"
QTM_END_NAMESPACE

