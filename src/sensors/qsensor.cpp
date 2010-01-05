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
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \typedef QSensorId
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
{
}

/*!
    Destroy the sensor.
*/
QSensor::~QSensor()
{
}

/*!
    Returns true if the sensor is connected to a backend.
*/
bool QSensor::isValid() const
{
    return backend() != 0;
}

/*!
    Returns the id for the sensor.
*/
QSensorId QSensor::id() const
{
    return backend() ? backend()->id() : QSensorId();
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

    \value Undefined          The sensor has no specific update policy.
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
    if (backend())
        backend()->setUpdatePolicy(policy, interval);
}

/*!
    Returns the update policy the sensor is using.
*/
QSensor::UpdatePolicy QSensor::updatePolicy() const
{
    return backend() ? backend()->updatePolicy() : QSensor::Undefined;
}

/*!
    Returns the update interval the sensor is using (only applicable when
    using the TimedUpdates policy).
*/
int QSensor::updateInterval() const
{
    return backend() ? backend()->updateInterval() : 0;
}

/*!
    Returns the update policies that the sensor supports.
*/
QSensor::UpdatePolicies QSensor::supportedPolicies() const
{
    return backend() ? backend()->supportedPolicies() : QSensor::Undefined;
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
    return backend() ? backend()->start() : false;
}

/*!
    Stop retrieving values from the sensor.
*/
void QSensor::stop()
{
    if (backend())
        backend()->stop();
}

/*!
    \internal
*/
QSensorBackend *QSensor::connectToBackend(const QSensorId &_id)
{
    QSensorId id(_id);
    if (id.isEmpty()) {
        id = QSensorFactory::instance()->defaultSensorForType(type());
    } else {
        if (!QSensorFactory::instance()->sensorsForType(type()).contains(id)) {
            qWarning() << "Sensor with identifier" << id << "does not exist for type" << type();
            return 0;
        }
    }

    QSensorBackend *backend = QSensorManager::instance()->createBackend(id);
    if (backend)
        backend->createdFor(this, id);
    return backend;
}

/*!
    \fn QSensor::newReadingAvailable()

    This function is called to notify the sensor that a new reading is available.
*/

/*!
    \fn QSensor::backend() const

    Returns a pointer to the backend for the sensor.
*/

#include "moc_qsensor.cpp"
QTM_END_NAMESPACE

