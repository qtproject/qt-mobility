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

#include "qsensor.h"
#include "qsensor_p.h"
#include "qsensorbackend.h"
#include "qsensormanager.h"
#include <QDebug>
#include <QMetaProperty>

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

// A bit of a hack to call qRegisterMetaType when the library is loaded.
static int qtimestamp_id = qRegisterMetaType<qtimestamp>("qtimestamp");

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

    The sensor data is delivered via QSensorData and its sub-classes.
*/

/*!
    Construct the sensor as a child of \a parent.
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
    foreach (QSensorFilter *filter, d->filters)
        filter->setSensor(0);
    delete d->backend;
    d->backend = 0;
    // owned by the backend
    d->device_reading = 0;
    d->filter_reading = 0;
    d->cache_reading = 0;
}

/*!
    \property QSensor::connected
    \brief a value indicating if the sensor has connected to a backend.

    A sensor that has not been connected to a backend cannot do anything useful.

    Call the connect() method to force the sensor to connect to a backend immediately.
*/

/*!
    Returns true if the sensor is connected to a backend.
*/
bool QSensor::isConnected() const
{
    return (d->backend != 0);
}

/*!
    \property QSensor::sensorid
    \brief the backend identifier for the sensor.

    Note that the identifier is filled out automatically
    when the sensor is connected to a backend.
*/

/*!
    Returns the backend identifier for the sensor.
*/
QByteArray QSensor::identifier() const
{
    return d->identifier;
}

/*!
    Sets the backend \a identifier to use.

    This must be done before connect() is called.
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
    Sets the \a type of the sensor.

    Note that this can only be used if you are using QSensor directly.
    Sub-classes of QSensor call this automatically for you.
*/
void QSensor::setType(const QByteArray &type)
{
    Q_ASSERT(!d->backend);
    Q_ASSERT(QLatin1String(metaObject()->className()) == QLatin1String("QSensor") || QLatin1String(metaObject()->className()) == QLatin1String(type));
    d->type = type;
}

/*!
    Try to connect to a sensor backend.

    Returns true if a suitable backend could be found, false otherwise.

    The type must be set before calling this method if you are using QSensor directly.

    \sa isConnected()
*/
bool QSensor::connect()
{
    if (d->backend)
        return true;

    if (d->type.isEmpty()) {
        qWarning() << "QSensor::connect - Cannot call this method unless the type is set.";
        return false;
    }

    d->backend = QSensorManager::createBackend(this);
    return (d->backend != 0);
}

/*!
    \property QSensor::running
    \brief controls the running state of the sensor.

    This is provided for QML, set running: true to cause the sensor
    to start on.
*/

/*!
    Returns true if the sensor is active (returning values).
*/
bool QSensor::isActive() const
{
    return d->active;
}

/*!
    Start the sensor \a running or stop it.
    \sa start(), stop()
*/
void QSensor::setActive(bool running)
{
    if (d->complete) {
        if (running)
            start();
        else
            stop();
    }
}

/*!
    Returns true if the readingChanged() signal will be emitted.
*/
bool QSensor::isSignalEnabled() const
{
    return d->signalEnabled;
}

/*!
    Call with \a enabled as false to turn off the readingChanged() signal.

    You might want to do this for performance reasons. If you are polling
    the sensor or using a filter in a performance-critical application
    then the overhead of emitting the signal may be too high even if nothing
    is connected to it.
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

    Note that this will return QSensor::Undefined until a sensor backend is connected.

    \sa isConnected()
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
    if (!connect())
        return;
    if (d->updatePolicy == PolledUpdates)
        d->backend->poll();
}

/*!
    Start retrieving values from the sensor.
*/
void QSensor::start()
{
    if (d->active)
        return;
    if (!connect())
        return;
    d->active = true;
    d->backend->start();
}

/*!
    Stop retrieving values from the sensor.
*/
void QSensor::stop()
{
    if (!d->active || !d->backend)
        return;
    d->active = false;
    d->backend->stop();
}

/*!
    \property QSensor::reading
    \brief the reading class.

    The reading class provides access to sensor readings.

    Note that this will return 0 until a sensor backend is connected.

    \sa isConnected()
*/

/*!
    Returns the reading class for this sensor.
*/
QSensorReading *QSensor::reading() const
{
    return d->cache_reading;
}

/*!
    Add a \a filter to the sensor.

    The sensor does not take ownership of the filter.
    QSensorFilter will inform the sensor if it is destroyed.

    \sa QSensorFilter
*/
void QSensor::addFilter(QSensorFilter *filter)
{
    d->filters << filter;
}

/*!
    Remove \a filter from the sensor.

    \sa QSensorFilter
*/
void QSensor::removeFilter(QSensorFilter *filter)
{
    d->filters.removeOne(filter);
    filter->setSensor(0);
}

/*!
    \fn QSensor::d_func() const
    \internal
*/

/*!
    \fn QSensor::readingChanged()

    This signal is emitted when the reading has changed.
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

    Additionally, multiple filters can be added to a sensor. They are called
    in order and each filter has the option to modify the values in the reading
    or to suppress the reading altogether.

    Note that the values in the class returned by QSensor::reading() will
    not be updated until after the filters have been run.

    \sa filter()
*/

/*!
    \internal
*/
QSensorFilter::QSensorFilter()
    : m_sensor(0)
{
}

/*!
    Notifies the attached sensor (if any) that the filter is being destroyed.
*/
QSensorFilter::~QSensorFilter()
{
    if (m_sensor)
        m_sensor->removeFilter(this);
}

/*!
    \fn QSensorFilter::filter(QSensorReading *reading)

    This function is called when the sensor \a reading changes.

    The filter can modify the reading.

    Returns true to allow the next filter to receive the value.
    If this is the last filter, returning true causes the signal
    to be emitted and the value is stored in the sensor.

    Returns false to drop the reading.
*/

/*!
    \internal
*/
void QSensorFilter::setSensor(QSensor *sensor)
{
    m_sensor = sensor;
}

// =====================================================================

/*!
    \class QSensorReading
    \ingroup sensors

    \preliminary
    \brief The QSensorReading class holds the readings from the sensor.

    Note that QSensorReading is not particularly useful by itself. The interesting
    data for each sensor is defined in a sub-class of QSensorReading.
*/

/*!
    \internal
*/
QSensorReading::QSensorReading(QObject *parent, QSensorReadingPrivate *_d)
    : QObject(parent)
    , d(_d)
{
}

/*!
    \fn QSensorReading::d_ptr()
    \internal
*/

/*!
    \internal
*/
QSensorReading::~QSensorReading()
{
}

/*!
    \property QSensorReading::timestamp
    \brief the timestamp of the reading.

    \sa qtimestamp
*/

/*!
    Returns the timestamp of the reading.
*/
qtimestamp QSensorReading::timestamp() const
{
    return d->timestamp;
}

/*!
    Sets the \a timestamp of the reading.
*/
void QSensorReading::setTimestamp(qtimestamp timestamp)
{
    d->timestamp = timestamp;
}

/*!
    Returns the number of extra properties that the reading has.

    Note that this does not count properties declared in QSensorReading.

    As an example, this returns 3 for QAccelerometerReading because
    there are 3 properties defined in that class.
*/
int QSensorReading::valueCount() const
{
    const QMetaObject *mo = metaObject();
    return mo->propertyCount() - mo->propertyOffset();
}

/*!
    Returns the value of the property at \a index.

    Note that this function is slower than calling the data function directly.
    Consider the following statement that provides the best performance.

    \code
    QAccelerometerReading *reading = ...;
    qreal x = reading->x();
    \endcode

    The slowest way to access a property is via name. To do this you must call
    QObject::property().

    \code
    qreal x = reading->property("x").value<qreal>();
    \endcode

    This is about 20 times slower than simply calling x(). There are 3 costs here.

    \list
    \o The cost of the string comparison.
    \o The cost of using the meta-object system.
    \o The cost of converting to/from QVariant.
    \endlist

    By looking up the property via numeric index, the string comparison cost is
    removed.

    \code
    qreal x = reading->value(0).value<qreal>();
    \endcode

    While faster than name-based lookup this is still about 20 times slower than
    simply calling x().

    Reading classes can opt to re-implement this function and bypass the
    meta-object system. If this is done this function will be about 3 times slower
    than simply calling x().

    \sa valueCount(), QObject::property()
*/
QVariant QSensorReading::value(int index) const
{
    // get them meta-object
    const QMetaObject *mo = metaObject();

    // determine the index of the property we want
    index += mo->propertyOffset();

    // get the meta-property
    QMetaProperty property = mo->property(index);

    // read the property
    return property.read(this);
}

/*
    \fn QSensorReading::value(int index) const

    Returns the value of the property at \a index.
*/

/*!
    \fn QSensorReading::copyValuesFrom(QSensorReading *other)
    \internal

    Copy values from other into this reading. Implemented by sub-classes
    using the DECLARE_READING() and IMPLEMENT_READING() macros.

    Note that this method should only be called by QSensorBackend.
*/

/*!
    \macro DECLARE_READING(classname)
    \relates QSensorReading
    \brief The DECLARE_READING macro adds some required methods to a reading class.

    This macro should be used for all reading classes. Pass the \a classname of your reading class.

    \code
    class MyReading : public QSensorReading
    {
        Q_OBJECT
        Q_PROPERTY(qreal myprop READ myprop)
        DECLARE_READING(MyReading)
    public:
        qreal myprop() const;
        vod setMyprop(qreal myprop);
    };
    \endcode

    \sa IMPLEMENT_READING()
*/

/*!
    \macro IMPLEMENT_READING(classname)
    \relates QSensorReading
    \brief The IMPLEMENT_READING macro implements the required methods for a reading class.

    This macro should be used for all reading classes. It should be placed into a single compilation
    unit (source file), not into a header file. Pass the \a classname of your reading class.

    \code
    IMPLEMENT_READING(MyReading)
    \endcode

    \sa DECLARE_READING()
*/

#include "moc_qsensor.cpp"
QTM_END_NAMESPACE

