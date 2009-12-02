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
    \class QSensor
    \ingroup sensors

    \preliminary
    \brief The QSensor class represents a single hardware sensor.

    The life cycle of a sensor is typically:

    \list
    \o Retrieved by calling QSensorManager::defaultSensorForType().
    \o Setup as required by the application.
    \o Started.
    \o Sensor data is used by the application.
    \o Stopped.
    \endlist

    Please see the individual sensor classes for details on their use.
*/

/*!
    \enum QSensor::Type

    This enum is used when requesting a sensor and denotes the type of the sensor.

    \value Orientation          Reports on portrait vs landscape mode.
    \value Rotation             Reports on rotation around the X, Y and Z axes.
    \value AngularAcceleration  Reports on angular acceleration around the X, Y and Z axes.
    \value Acceleration         Reports on linear acceleration around the X, Y and Z axes.
    \value DoubleTap            Reports on a double tap in the X, Y or Z direction.
    \value Proximity            Reports on the distance of the user from the device.
    \value MagneticNorth        Indicates the deviation from magnetic north.
    \value Magnetometer         Flux density values along the X, Y and Z axes.
    \value AmbientLight         Reports on the intensity of the ambient light.
    \value UserSensor           Non-standard sensor types must use sensor types greater than this value.
*/

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
    \fn QSensor::start()

    Open the sensor. Data will be made available as soon as possible.
    Note that some sensors require exclusive access so this function
    may fail and return false.
*/

/*!
    \fn QSensor::stop()

    Close the sensor. For sensors that require exclusive access, this
    will release the sensor. The state of the sensor will be retained
    so sensitivity, filters, listeners etc. will be preserved.
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
    \fn QSensor::UpdatePolicies QSensor::supportedPolicies() const
    Returns the update policies that the sensor supports.
*/

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
    Add a \a filter to the sensor.
*/
void QSensor::addFilter(QSensorFilter *filter)
{
    Q_UNUSED(filter)
}

/*!
    Remove a \a filter from the sensor.
    If \a filter is 0, all filters will be removed from the sensor.
*/
void QSensor::removeFilter(QSensorFilter *filter)
{
    Q_UNUSED(filter)
}

/*!
    \class QSensorListener
    \ingroup sensors

    \preliminary
    \brief The QSensorListener class represents an object that wishes to
           receive notifications about sensor changes.

    Foo bar baz.
*/

/*!
    \fn void QSensorListener::sensorEvent(QSensorEvent *event)

    A sensor \a event has occurred.
*/

/*!
    \class QSensorEvent
    \ingroup sensors

    \preliminary
    \brief The QSensorEvent class represents a sensor event.

    Foo bar baz.
*/

/*!
    Returns the type of sensor that generated this event.
*/
QSensor::Type QSensorEvent::type() const
{
    return QSensor::Orientation;
}

/*!
    \class QSensorFilter
    \ingroup sensors

    \preliminary
    \brief The QSensorFilter class represents a filter.

    Foo bar baz.
*/

/*!
    \fn bool QSensorFilter::filter(QSensorEvent *event)

    Filter the \a event. The filter may modify any values
    in the event. If the filter returns false the event
    will be ignored.
*/

