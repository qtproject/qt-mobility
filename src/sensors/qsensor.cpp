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
static int qtimestamp_id = qRegisterMetaType<QtMobility::qtimestamp>("QtMobility::qtimestamp");
static int qrange_id = qRegisterMetaType<QtMobility::qrange>("QtMobility::qrange");
static int qrangelist_id = qRegisterMetaType<QtMobility::qrangelist>("QtMobility::qrangelist");
static int qoutputrangelist_id = qRegisterMetaType<QtMobility::qoutputrangelist>("QtMobility::qoutputrangelist");

// =====================================================================

/*!
    \class QSensor
    \ingroup sensors_main

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
    Construct the \a type sensor as a child of \a parent.
*/
QSensor::QSensor(const QByteArray &type, QObject *parent)
    : QObject(parent)
    , d(new QSensorPrivate)
{
    d->type = type;
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QSensor::~QSensor()
{
    stop();
    Q_FOREACH (QSensorFilter *filter, d->filters)
        filter->setSensor(0);
    delete d->backend;
    d->backend = 0;
    // owned by the backend
    d->device_reading = 0;
    d->filter_reading = 0;
    d->cache_reading = 0;
}

/*!
    \property QSensor::connectedToBackend
    \brief a value indicating if the sensor has connected to a backend.

    A sensor that has not been connected to a backend cannot do anything useful.

    Call the connectToBackend() method to force the sensor to connect to a backend
    immediately. This is automatically called if you call start() so you only need
    to do this if you need access to sensor properties (ie. to poll the sensor's
    meta-data before you use it).
*/

bool QSensor::isConnectedToBackend() const
{
    return (d->backend != 0);
}

/*!
    \property QSensor::sensorid
    \brief the backend identifier for the sensor.

    Note that the identifier is filled out automatically
    when the sensor is connected to a backend. If you want
    to connect a specific backend, you should call
    setIdentifier() before connectToBackend().
*/

QByteArray QSensor::identifier() const
{
    return d->identifier;
}

void QSensor::setIdentifier(const QByteArray &identifier)
{
    if (d->backend) {
        qWarning() << "ERROR: Cannot call QSensor::setIdentifier while connected to a backend!";
        return;
    }
    d->identifier = identifier;
}

/*!
    \property QSensor::type
    \brief the type of the sensor.
*/

QByteArray QSensor::type() const
{
    return d->type;
}

/*!
    Try to connect to a sensor backend.

    Returns true if a suitable backend could be found, false otherwise.

    The type must be set before calling this method if you are using QSensor directly.

    \sa isConnectedToBackend()
*/
bool QSensor::connectToBackend()
{
    if (d->backend)
        return true;

    int rate = d->dataRate;
    d->backend = QSensorManager::createBackend(this);
    if (rate != 0)
        setDataRate(rate);
    return (d->backend != 0);
}

/*!
    \property QSensor::busy
    \brief a value to indicate if the sensor is busy.

    Some sensors may be on the system but unavailable for use.
    This function will return true if the sensor is busy. You
    will not be able to start() the sensor.

    Note that this function does not return true if you
    are using the sensor, only if another process is using
    the sensor.

    \sa busyChanged()
*/

bool QSensor::isBusy() const
{
    return d->busy;
}

/*!
    \fn QSensor::busyChanged()

    This signal is emitted when the busy state changes. This can
    be used to grab a sensor when it becomes available.
*/

/*!
    \property QSensor::active
    \brief a value to indicate if the sensor is active.

    This is true if the sensor is active (returning values). This is false otherwise.
*/

bool QSensor::isActive() const
{
    return d->active;
}

/*!
    \property QSensor::availableDataRates
    \brief the data rates that the sensor supports.

    This is a list of the data rates that the sensor supports.
    Measured in Hertz.

    Entries in the list can represent discrete rates or a
    continuous range of rates.
    A discrete rate is noted by having both values the same.

    See the sensor_explorer example for an example of how to interpret and use
    this information.

    \sa QSensor::dataRate
*/

qrangelist QSensor::availableDataRates() const
{
    return d->availableDataRates;
}

/*!
    \property QSensor::dataRate
    \brief the data rate that the sensor should be run at.

    Measured in Hertz.

    The default value is determined by the backend.

    This should be set before calling start() because the sensor may not
    notice changes to this value while it is running.

    \sa QSensor::availableDataRates
*/

int QSensor::dataRate() const
{
    return d->dataRate;
}

void QSensor::setDataRate(int rate)
{
    bool warn = true;
    Q_FOREACH (const qrange &range, d->availableDataRates) {
        if (rate >= range.first && rate <= range.second) {
            warn = false;
            d->dataRate = rate;
            break;
        }
    }
    if (warn) {
        qWarning() << "setDataRate: rate" << rate << "is not supported by the sensor.";
    }
}

/*!
    Start retrieving values from the sensor.
    Returns true if the sensor was started, false otherwise.

    Note that the sensor may fail to start for several reasons.

    \sa QSensor::busy
*/
bool QSensor::start()
{
    if (d->active)
        return true;
    if (!connectToBackend())
        return false;
    if (d->availableDataRates.count() == 0)
        return false;
    // Set these flags to their defaults
    d->active = true;
    d->busy = false;
    // Backend will update the flags appropriately
    d->backend->start();
    return d->active;
}

/*!
    Stop retrieving values from the sensor.

    This releases the sensor so that other processes can use it.

    \sa QSensor::busy
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

    Note that this will return 0 until a sensor backend is connected to a backend.

    \sa isConnectedToBackend()
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
    if (!filter) {
        qWarning() << "addFilter: passed a null filter!";
        return;
    }
    d->filters << filter;
}

/*!
    Remove \a filter from the sensor.

    \sa QSensorFilter
*/
void QSensor::removeFilter(QSensorFilter *filter)
{
    if (!filter) {
        qWarning() << "removeFilter: passed a null filter!";
        return;
    }
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

/*!
    \property QSensor::outputRanges
    \brief a list of output ranges the sensor supports.

    A sensor may have more than one output range. Typically this is done
    to give a greater measurement range at the cost of lowering accuracy.

    \sa QSensor::outputRange
*/

qoutputrangelist QSensor::outputRanges() const
{
    return d->outputRanges;
}

/*!
    \property QSensor::outputRange
    \brief the output range in use by the sensor.

    A sensor may have more than one output range. Typically this is done
    to give a greater measurement range at the cost of lowering accuracy.

    \sa QSensor::outputRanges
*/

int QSensor::outputRange() const
{
    return d->outputRange;
}

void QSensor::setOutputRange(int index)
{
    if (index < 0 || index >= d->outputRanges.count()) {
        qWarning() << "ERROR: Output range" << index << "is not valid";
        return;
    }
    d->outputRange = index;
}

/*!
    \property QSensor::description
    \brief a descriptive string for the sensor.
*/

QString QSensor::description() const
{
    return d->description;
}

/*!
    \property QSensor::error
    \brief the last error code set on the sensor.

    Note that error codes are sensor-specific.
*/

int QSensor::error() const
{
    return d->error;
}

/*!
    \fn QSensor::sensorError(int error)

    This signal is emitted when an \a error code is set on the sensor.
    Note that some errors will cause the sensor to stop working.
    You should call isActive() to determine if the sensor is still running.
*/

// =====================================================================

/*!
    \class QSensorFilter
    \ingroup sensors_main

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
    \ingroup sensors_main

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

    Here is an example of getting a property via the different mechanisms available.

    Accessing directly provides the best performance but requires compile-time knowledge
    of the data you are accessing.

    \code
    QAccelerometerReading *reading = ...;
    qreal x = reading->x();
    \endcode

    You can also access a property by name. To do this you must call QObject::property().

    \code
    qreal x = reading->property("x").value<qreal>();
    \endcode

    Finally, you can access values via numeric index.

    \code
    qreal x = reading->value(0).value<qreal>();
    \endcode

    Note that value() can only access properties declared with Q_PROPERTY() in sub-classes
    of QSensorReading.

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

