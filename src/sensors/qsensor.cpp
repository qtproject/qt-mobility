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

#include <qsensor.h>
#include "qsensor_p.h"
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <qsensorfactory.h>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \typedef qtimestamp
    \relates QSensor

    Sensor timestamps are represented by this typedef which is a 64 bit unsigned integer.

    Timestamps values are microseconds since a fixed point.
    You can use timestamps to see how far apart two sensor readings are.

    Note that sensor timestamps from different sensors may not be directly
    comparable (as they may choose different fixed points for their reference).
*/

// =====================================================================

/*!
    \class QSensor
    \ingroup sensors

    \preliminary
    \brief The QSensor class represents a single hardware sensor.

    The life cycle of a sensor is typically:

    \list
    \o Create a sub-class of QSensor on the stack or heap.
    \o Setup as required by the application.
    \o Start receiving values.
    \o Sensor data is used by the application.
    \o Stop receiving values.
    \endlist

    Please see the individual sensor classes for details on their use.
*/

/*!
    \internal
*/
QSensor::QSensor(QObject *parent)
    : QObject(parent)
    , d(new QSensorPrivate)
{
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QSensor::~QSensor()
{
    stop();
}

/*!
    \property QSensor::isAvailable
    \brief the validity of the sensor.

    If the sensor is not valid then you cannot use it.
*/

/*!
    Returns true if the sensor is connected to a backend.
*/
bool QSensor::isAvailable() const
{
    return (d->backend != 0);
}

/*!
    \property QSensor::identifier
    \brief the unique identifer for the sensor.
*/

/*!
    Returns the identifier for the sensor.
*/
QByteArray QSensor::identifier() const
{
    return d->identifier;
}

/*!
    Foo
*/
void QSensor::setIdentifier(const QByteArray &identifier)
{
    Q_ASSERT(!d->backend);
    d->identifier = identifier;
}

/*!
    \property QSensor::type
    \brief the type of the sensor.
*/

/*!
    Returns the type of the sensor.
*/
QByteArray QSensor::type() const
{
    return d->type;
}

/*!
    Foo
*/
void QSensor::setType(const QByteArray &type)
{
    Q_ASSERT(!d->backend);
    d->type = type;
}

/*!
    Foo
*/
void QSensor::connect()
{
    Q_ASSERT(!d->backend);
    Q_ASSERT(!d->type.isEmpty());
    if (d->identifier.isEmpty()) {
        d->identifier = QSensorFactory::instance()->defaultSensorForType(d->type);
    } else {
        if (!QSensorFactory::instance()->sensorsForType(d->type).contains(d->identifier)) {
            qWarning() << "Sensor with identifier" << d->identifier << "does not exist for type" << d->type;
            return;
        }
    }

    d->backend = QSensorManager::instance()->createBackend(d->identifier, this);
    if (!d->backend)
        return;
}

/*!
    \property QSensor::active
    \brief foo
*/

/*!
    Foo
*/
bool QSensor::isActive() const
{
    return d->active;
}

/*!
    \property QSensor::signalEnabled
    \brief foo
*/

/*!
    Foo
*/
bool QSensor::isSignalEnabled() const
{
    return d->signalEnabled;
}

/*!
    Foo
*/
void QSensor::setSignalEnabled(bool enabled)
{
    d->signalEnabled = enabled;
}

/*!
    \enum QSensor::UpdatePolicy

    This enum is used to indicate to the sensor how often data will be collected.
    Note that most sensors will only support one sensitivity. Setting an update
    policy that the sensor does not support will result in undefined behaviour.
    You can determine the policies the sensor supports with the
    QSensor::supportedUpdatePolicies() method.

    \value Undefined          The sensor has no specific update policy. Updates may
                              arrive frequently or infrequently. Updates based on
                              user interaction are likely to fit into this category.
    \value OnChangeUpdates    Updates are delivered as they happen, usually based on
                              user activity.
    \value OccasionalUpdates  Updates are delivered occasionally, about one every
                              5 seconds.
    \value InfrequentUpdates  Updates are delivered infrequently, no more than once
                              per second.
    \value FrequentUpdates    Updates are delivered frequently, several per second.
    \value TimedUpdates       Updates are delivered at a specific time interval.
                              Note that not all sensors may be able to run with the
                              exact timings requested and may operate slightly faster
                              or slower.
    \value PolledUpdates      Updates are retrieved when the currentReading()
                              method is called.
*/

/*!
    Change the update \a policy of the sensor. Note that not all
    sensors support changing the update policy. If you set a
    policy that the sensor does not support the behaviour is
    undefined.

    If you wish to use the TimedUpdates policy, please call
    setUpdateInterval() with the desired interval.

    \sa supportedUpdatePolicies()
*/
void QSensor::setUpdatePolicy(UpdatePolicy policy)
{
    if (policy == TimedUpdates)
        return;

    d->updatePolicy = policy;
    d->updateInterval = 0;
}

/*!
    Change the update \a interval of the sensor. This
    requires the sensor to spport the TimedUpdates policy.
    Note that not all sensors support changing the update policy.
    If you set a policy that the sensor does not support the behaviour
    is undefined.
    \sa supportedUpdatePolicies()
*/
void QSensor::setUpdateInterval(int interval)
{
    d->updatePolicy = TimedUpdates;
    d->updateInterval = interval;
}

/*!
    \property QSensor::updatePolicy
    \brief the update policy of the sensor.
*/

/*!
    Returns the update policy the sensor is using.
*/
QSensor::UpdatePolicy QSensor::updatePolicy() const
{
    return d->updatePolicy;
}

/*!
    \property QSensor::updateInterval
    \brief the update interval of the sensor.

    This value is only useful if the QSensor::updatePolicy property is set to TimedUpdates.
*/

/*!
    Returns the update interval the sensor is using (only applicable when
    using the TimedUpdates policy).
*/
int QSensor::updateInterval() const
{
    return d->updateInterval;
}

/*!
    \property QSensor::supportedUpdatePolicies
    \brief the supported policies of the sensor.
*/

/*!
    Returns the update policies that the sensor supports.
*/
QSensor::UpdatePolicies QSensor::supportedUpdatePolicies() const
{
    return d->supportedUpdatePolicies;
}

/*!
    Poll the sensor.
*/
void QSensor::poll()
{
    if (d->updatePolicy == PolledUpdates && d->backend)
        d->backend->poll();
}

/*!
    Start retrieving values from the sensor.

    Note that some sensors require exclusive access so this function
    may fail and return false.

    Also note that some sensors may not honour settings set after
    this method is called.
*/
void QSensor::start()
{
    connect();
    if (!d->backend)
        return;
    d->active = true;
    d->backend->start();
}

/*!
    Stop retrieving values from the sensor.
*/
void QSensor::stop()
{
    if (!d->backend)
        return;
    d->active = false;
    d->backend->stop();
}

/*!
    \property QSensor::reading
    \brief foo
*/

/*!
    Foo
*/
QSensorReading *QSensor::reading() const
{
    return d->cache_reading;
}

/*!
    \internal
*/
void QSensor::newReadingAvailable()
{
    for (QFilterList::const_iterator it = d->filters.constBegin(); it != d->filters.constEnd(); ++it) {
        QSensorFilter *filter = (*it);
        if (!filter->filter(d->filter_reading))
            return;
    }

    d->cache_reading->readValuesFrom(d->filter_reading);

    if (d->signalEnabled)
        emit readingChanged(d->cache_reading);
}

/*!
    Foo \a filter
*/
void QSensor::addFilter(QSensorFilter *filter)
{
    d->filters << filter;
}

/*!
    Foo \a filter
*/
void QSensor::removeFilter(QSensorFilter *filter)
{
    d->filters.removeOne(filter);
}

/*!
    \fn QSensor::d_func() const
    \internal
*/

/*!
    \fn QSensor::readingChanged(QSensorReading *reading)

    Foo \a reading
*/

// =====================================================================

/*!
    \class QSensorFilter
    \ingroup sensors

    \preliminary
    \brief The QSensorFilter class provides an efficient
           callback facility for asynchronous notifications of
           sensor changes.

    Some sensors (eg. the accelerometer) are often accessed very frequently.
    This may be slowed down by the use of signals and slots.
    The QSensorFilter interface provides a more efficient way for the
    sensor to notify your class that the sensor has changed.
*/

/*!
    \internal
*/
QSensorFilter::QSensorFilter()
    : m_sensor(0)
{
}

/*!
    Notifies any sensors that the filter is being destroyed.
*/
QSensorFilter::~QSensorFilter()
{
    if (m_sensor)
        m_sensor->removeFilter(this);
}

/*!
    \fn QSensorFilter::filter(QSensorReading *reading)

    This function is called when the sensor \a reading changes.

    Returns true;
*/

/*!
    \internal
*/
void QSensorFilter::setSensor(QSensor *sensor)
{
    m_sensor = sensor;
}

// =====================================================================

QSensorReading::QSensorReading(QSensorReadingPrivate *_d, size_t size, void **dest)
    : d(_d)
{
    d->size = size;
    *dest = _d;
}

QSensorReading::~QSensorReading()
{
}

qtimestamp QSensorReading::timestamp() const
{
    return d->timestamp;
}

void QSensorReading::setTimestamp(qtimestamp timestamp)
{
    d->timestamp = timestamp;
}

void QSensorReading::readValuesFrom(QSensorReading *other)
{
    Q_ASSERT(d->size == other->d->size);
    memcpy(d.data(), other->d.data(), d->size);
}

#include "moc_qsensor.cpp"
QTM_END_NAMESPACE

