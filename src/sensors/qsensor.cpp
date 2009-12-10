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
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <qsensorfactory.h>

QTM_BEGIN_NAMESPACE

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
    \o Retrieved by calling QSensorFactory::createDefaultSensorForType()
       or by directly instantiating a sub-class of QSensor.
    \o Setup as required by the application.
    \o Started.
    \o Sensor data is used by the application.
    \o Stopped.
    \endlist

    Please see the individual sensor classes for details on their use.
*/

/*!
    \internal
*/
QSensor::QSensor(QObject *parent)
    : QObject(parent)
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
    Returns true if the sensor is connected to a backend.
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
    return m_backend->id();
}

/*!
    Returns the sensor name, suitable for displaying to the user.
*/
QString QSensor::name() const
{
    return m_backend->name();
}

/*!
    \fn QSensor::type() const

    Returns the type of the sensor.
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
    Change the update \a policy of the sensor. If using TimedUpdates
    you must also supply the \a interval. Note that not all sensors
    support changing the update policy. If you set a policy that
    the sensor does not support the behaviour is undefined.
    \sa supportedPolicies()
*/
void QSensor::setUpdatePolicy(UpdatePolicy policy, int interval)
{
    m_backend->setUpdatePolicy(policy, interval);
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
    Subclasses should implement this method to emit signals when
    the value has changed.
*/
void QSensor::valueUpdated()
{
}

/*!
    \internal
*/
void QSensor::connectToBackend(const QSensorID &_id)
{
    QSensorID id(_id);
    if (id.isEmpty())
        id = QSensorFactory::instance()->defaultSensorForType(type());

    m_backend = QSensorManager::instance()->createBackend(id);
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
    \brief The QSensorValue class represents a sensor value update.

    Actual sensor data is stored in sub-classes of QSensorValue.
*/

/*!
    Constuct a sensor value for a \a type.
*/
QSensorValue::QSensorValue(const QString &type)
    : m_type(type)
{
}

/*!
    \fn QSensorValue::type() const

    Returns the type of value that this is.
*/

/*!
    \variable QSensorValue::timestamp

    Returns a timestamp (in milliseconds since app startup)
    showing when the sensor value was generated.
*/

QTM_END_NAMESPACE

