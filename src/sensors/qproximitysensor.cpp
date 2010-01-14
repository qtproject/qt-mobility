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

QTM_BEGIN_NAMESPACE

/*!
    \class QProximityReading
    \ingroup sensors

    \preliminary
    \brief The QProximityReading class represents one reading from the
           proximity sensor.

    The proximity sensor returns an indication of how far the user is
    away from the device using the pre-defined values found in the
    QProximityReading::Proximity enum.
*/

/*!
    \enum QProximityReading::Proximity

    This enum represents the proximity of the user.

    \value Undefined  The proximity is unknown.
    \value Close      The user is close to the device.
    \value NotClose   The user is not close to the device.
*/

/*!
    \fn QProximityReading::QProximityReading()
    \internal
*/

/*!
    \fn QProximityReading::QProximityReading(qtimestamp timestamp, Proximity proximity)
    \internal
*/

/*!
    \fn QProximityReading::QProximityReading(const QProximityReading &other)
    \internal
*/

/*!
    \fn QProximityReading::~QProximityReading()
    \internal
*/

/*!
    \fn QProximityReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QProximityReading::proximity() const

    Returns the proximity indication.
*/

// =====================================================================

/*!
    \class QProximitySensor
    \ingroup sensors

    \preliminary
    \brief The QProximitySensor class reports on user proximity.

    The proximity sensor returns an indication of how far the user is
    away from the device using the pre-defined values found in the
    QProximityReading::Proximity enum.

    \sa QProximityReading
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a identifier is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QProximitySensor::QProximitySensor(QObject *parent, const QByteArray &identifier)
    : QSensor(parent)
{
    m_backend = static_cast<QProximityBackend*>(connectToBackend(identifier));
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QProximitySensor::~QProximitySensor()
{
    stop();
}

/*!
    \property QProximitySensor::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QProximitySensor::typeId
*/
const QString QProximitySensor::typeId("qt.Proximity");

/*!
    \fn QProximitySensor::type() const
    \reimp
*/

/*!
    \fn QProximitySensor::currentReading() const

    Returns the current proximity reading.
*/

/*!
    \fn QProximitySensor::proximityChanged(const QProximityReading &reading)

    This signal is emitted when a new proximity \a reading comes in.
*/

/*!
    \fn QProximitySensor::backend() const
    \reimp
*/

#include "moc_qproximitysensor.cpp"
QTM_END_NAMESPACE

