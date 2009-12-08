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

#include <qproximitysensor.h>

QTM_USE_NAMESPACE

/*!
    \class QProximitySensorValue
    \ingroup sensors

    \preliminary
    \brief The QProximitySensorValue class represents a proximity reading.

    The proximity sensor returns the distance to the user. The distance
    is measured in millimeters.
*/

/*!
    \variable QProximitySensorValue::distance

    Holds the distance of the user from the phone.
*/

// =====================================================================

/*!
    \class QProximitySensor
    \ingroup sensors

    \preliminary
    \brief The QProximitySensor class reports on user proximity.

    The proximity sensor returns the distance to the user. The distance
    is measured in millimeters.

    Note that some proximity sensors may only return boolean results
    (zero and greater than zero).
*/

/*!
    \variable QProximitySensor::type
*/
const QString QProximitySensor::type("qt.Proximity");

/*!
    Construct a sensor instance and attach to the indicated \a sensor.
*/
QProximitySensor::QProximitySensor(QSensor *sensor)
    : QSensor(sensor->id(), sensor->parent())
{
}

/*!
    Construct a sensor instance and attach to the sensor indicated by \a id.
    The sensor will be deleted when \a parent is deleted.
*/
QProximitySensor::QProximitySensor(const QSensorID &id, QObject *parent)
    : QSensor(id, parent)
{
}

/*!
    \fn QProximitySensor::currentProximity() const

    Returns the current proximity value.
*/

/*!
    \fn QProximitySensor::proximityChanged(int distance)

    Emitted when the \a distance value changes.
*/

QTM_BEGIN_NAMESPACE
#include "moc_qproximitysensor.cpp"
QTM_END_NAMESPACE

