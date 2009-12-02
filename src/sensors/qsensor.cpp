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
    \o Opened.
    \o Sensor data is used by the application.
    \o Closed.
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
    \enum QSensor::Sensitivity

    This enum is used to indicate to the sensor how often data will be collected.
    Note that most sensors will only support one sensitivity. Refer to the documentation
    on the sensor you are using to see if it supports multiple sensitivities.

    \value OccasionalUpdates  Updates are delivered only occasionally.
    \value InfrequentUpdates  Updates are delivered every now and then.
    \value FrequentUpdates    Updates are delivered frequently.
    \value TimedUpdates       Updates are delivered at the specified time interval.
    \value RealtimeUpdates    Updates are retrieved when the device is polled.
*/

/*!
    \fn QSensor::open()

    Open the sensor. Data will be made available as soon as possible.
    Note that some sensors require exclusive access so this function
    may fail and return false.
*/

/*!
    \fn QSensor::close()

    Close the sensor. For sensors that require exclusive access, this
    will release the sensor. The state of the sensor will be retained
    so sensitivity, filters, listeners etc. will be preserved.
*/

/*!
    Change the \a sensitivity of the sensor. If using TimedUpdates
    you must also supply the \a interval. Note that not all sensors
    support changing the sensitivity. If you set a sensitivity that
    the sensor does not support the sensor will ignore the action
    and continue to use the sensitivity it was previously using.
*/
void QSensor::setSensitivity(Sensitivity sensitivity, int interval)
{
    Q_UNUSED(sensitivity)
    Q_UNUSED(interval)
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

