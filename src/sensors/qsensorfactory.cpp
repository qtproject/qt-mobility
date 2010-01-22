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

#include <qsensorfactory.h>
#include <qsensormanager.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QSensorFactory
    \ingroup sensors

    \preliminary
    \brief The QSensorFactory class returns the sensors on a device.

    A given device will have a variety of sensors. The sensors are
    categorized by type. Each sensor has a ::typeId value that holds the
    unique type for that sensor. eg. QAccelerationSensor::typeId, QProximitySensor::typeId.
*/

/*!
    Returns the sensor factory.
*/
QSensorFactory *QSensorFactory::instance()
{
    // FIXME should be using Q_GLOBAL_STATIC or something
    static QSensorFactory *instance = 0;
    if (!instance)
        instance = new QSensorFactory;
    return instance;
}

/*!
    Returns the identifier of the default sensor for \a type.
    If there is no sensor of that type available, returns a null string.
*/
QByteArray QSensorFactory::defaultSensorForType(const QByteArray &type) const
{
    return QSensorManager::defaultSensorForType(type);
}

/*!
    Returns a list of ids for each of the sensors for \a type.
    If there are no sensors of that type available the list will be empty.
*/
QList<QByteArray> QSensorFactory::sensorsForType(const QByteArray &type) const
{
    Q_UNUSED(type)
    return QList<QByteArray>();
}

/*!
    Returns a list of ids for each of the sensors.
*/
QList<QByteArray> QSensorFactory::sensorList() const
{
    return QList<QByteArray>();
}

/*!
    Returns a list of all sensor types.
*/
QList<QByteArray> QSensorFactory::sensorTypes() const
{
    return QList<QByteArray>();
}

/*!
    Create an instance of a sensor for \a identifier.
    If there is no sensor with the selected \a identifier, returns null.

    Note that this method requires down-casting the returned pointer to do anything useful.
    Instead, consider instantiating a QSensor sub-class directly, passing the identifier as the
    second argument.
*/
QSensor *QSensorFactory::createSensor(const QByteArray &identifier) const
{
    Q_UNUSED(identifier)
    return 0;
}

QTM_END_NAMESPACE

