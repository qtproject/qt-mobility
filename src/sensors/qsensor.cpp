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
    \property QSensor::id
    \brief the unique identifer for the sensor.
*/

/*!
    \property QSensor::isValid
    \brief the validity of the sensor.

    If the sensor is not valid then you cannot use it.
*/

/*!
    \property QSensor::supportedPolicies
    \brief the supported policies of the sensor.
*/

/*!
    \property QSensor::type
    \brief the type of the sensor.
*/

/*!
    \property QSensor::updatePolicy
    \brief the update policy of the sensor.
*/

/*!
    \property QSensor::updateInterval
    \brief the update interval of the sensor.

    This value is only useful if the QSensor::updatePolicy property is set to TimedUpdates.
*/

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

    \value Undefined          The sensor has no specific update policy. Updates may
                              arrive frequently or infrequently. Updates based on
                              user interaction are likely to fit into this category.
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

    \sa supportedPolicies()
*/
void QSensor::setUpdatePolicy(UpdatePolicy policy)
{
    if (policy == TimedUpdates)
        return;

    if (backend()) {
        backend()->setUpdatePolicy(policy, 0);
    }
}

/*!
    Change the update \a interval of the sensor. This
    requires the sensor to spport the TimedUpdates policy.
    Note that not all sensors support changing the update policy.
    If you set a policy that the sensor does not support the behaviour
    is undefined.
    \sa supportedPolicies()
*/
void QSensor::setUpdateInterval(int interval)
{
    if (backend()) {
        backend()->setUpdatePolicy(TimedUpdates, interval);
    }
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
    \fn QSensor::backend() const

    Returns a pointer to the backend for the sensor.
*/

/*!
    \fn QSensor::newReadingAvailable()

    This function is called to notify the sensor that a new reading is available.
*/

#include "moc_qsensor.cpp"
QTM_END_NAMESPACE

