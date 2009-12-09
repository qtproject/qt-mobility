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

QTM_USE_NAMESPACE

/*!
    \typedef QSensorID
    \relates QSensor

    An opaque type representing an sensor identifier.
    These can be obtained from the QSensorFactory class.
*/

/*!
    \class QSensor
    \ingroup sensors

    \preliminary
    \brief The QSensor class represents a single hardware sensor.

    The life cycle of a sensor is typically:

    \list
    \o Retrieved by calling QSensorFactory::defaultSensorForType().
    \o Setup as required by the application.
    \o Started.
    \o Sensor data is used by the application.
    \o Stopped.
    \endlist

    Please see the individual sensor classes for details on their use.
*/

/*!
    Construct a sensor instance and attach to the sensor indicated by \a id.
    The sensor will be deleted when \a parent is deleted.
*/
QSensor::QSensor(const QSensorID &id, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(id)
}

/*!
    Stop the sensor.
*/
QSensor::~QSensor()
{
    stop();
}

/*!
    Returns false if an invalid id was given to the constructor.
*/
bool QSensor::isValid() const
{
    return false;
}

/*!
    Returns the id for the sensor.
*/
QSensorID QSensor::id() const
{
    return QSensorID();
}

/*!
    Returns the sensor name, suitable for displaying to the user.
*/
QString QSensor::name() const
{
    return QString();
}

/*!
    Returns the type of the sensor.
*/
QString QSensor::type() const
{
    return QString();
}

/*!
    \enum QSensor::UpdatePolicy

    This enum is used to indicate to the sensor how often data will be collected.
    Note that most sensors will only support one sensitivity. Setting an update
    policy that the sensor does not support will result in undefined behaviour.
    You can determine the policies the sensor supports with the
    QSensor::supportedPolicies() method.

    \value Unknown            The sensor has no specific update policy.
    \value OccasionalUpdates  Updates are delivered only occasionally.
    \value InfrequentUpdates  Updates are delivered every now and then.
    \value FrequentUpdates    Updates are delivered frequently.
    \value TimedUpdates       Updates are delivered at the specified time interval.
    \value PolledUpdates      Updates are retrieved when the device is polled.
*/

/*!
    Change the update \a policy of the sensor. If using TimedUpdates
    you must also supply the \a interval. Note that not all sensors
    support changing the update policy. If you set a policy that
    the sensor does not support the behaviour is undefined.
    \sa supportedPolicies()
*/
void QSensor::setUpdatePolicy(UpdatePolicy policy, int interval)
{
    Q_UNUSED(policy)
    Q_UNUSED(interval)
}

/*!
    Returns the update policy the sensor is using.
*/
QSensor::UpdatePolicy QSensor::updatePolicy() const
{
    return QSensor::Unknown;
}

/*!
    Returns the update interval the sensor is using (only applicable when
    using the TimedUpdates policy).
*/
int QSensor::updateInterval() const
{
    return 0;
}

/*!
    Returns the update policies that the sensor supports.
*/
QSensor::UpdatePolicies QSensor::supportedPolicies() const
{
    return QSensor::Unknown;
}

/*!
    Add a \a listener to the sensor.
    The listener will be invoked every time the acceleration values
    change.
*/
void QSensor::addListener(QSensorListener *listener)
{
    Q_UNUSED(listener)
}

/*!
    Remove a \a listener from the sensor.
    If \a listener is 0, all listeners will be removed from the sensor.
*/
void QSensor::removeListener(QSensorListener *listener)
{
    Q_UNUSED(listener)
}

/*!
    Returns the current sensor value.
*/
QSensorValue *QSensor::currentValue() const
{
    return 0;
}

/*!
    Start retrieving values from the sensor.

    Note that some sensors require exclusive access so this function
    may fail and return false.

    Also note that some sensors may not honour settings set after
    this method is called.
*/
bool QSensor::start()
{
    return false;
}

/*!
    Stop retrieving values from the sensor.
*/
void QSensor::stop()
{
}

/*!
    \class QSensorListener
    \ingroup sensors

    \preliminary
    \brief The QSensorListener class represents an object that wishes to
           receive notifications about sensor changes.

    An instance of this class is notified by method call when
    the sensor value changes.
*/

/*!
    \fn bool QSensorListener::sensorValueUpdated(QSensorValue *value)

    A sensor has been updated and now has the passed \a value.

    Note that other objects may be waiting to receive the updated value.
    The function may modify the value as appropriate or return false
    to suppress any further update notifications.
*/

/*!
    \class QSensorValue
    \ingroup sensors

    \preliminary
    \brief The QSensorValue class represents a sensor value.

    Actual sensor data is stored in sub-classes of QSensorValue.
*/

/*!
    Constuct a sensor value for a \a sensor.
*/
QSensorValue::QSensorValue(QSensor *sensor)
{
    Q_UNUSED(sensor)
}

/*!
    Returns the sensor that generated this value.
*/
QSensor *QSensorValue::sensor() const
{
    return 0;
}

/*!
    \variable QSensorValue::timestamp

    Returns a timestamp (in milliseconds since app startup)
    showing when the sensor value was generated.
*/

