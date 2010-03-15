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

#include "qproximitysensor.h"
#include "qproximitysensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QProximityReading)

/*!
    \class QProximityReading
    \ingroup sensors_reading

    \preliminary
    \brief The QProximityReading class represents one reading from the
           proximity sensor.

    \target QProximityReading_Units
    The proximity sensor measures the distance to the user in meters.

    Note that the maximum value supported by the sensor is reported if the user is
    out of range of the sensor.

    Some proximity sensors can only detect close/not close with a threshold. These
    sensors will set the maximum to their threshold and their accuracy to the same
    value. They will return 0 to indicate close and their maximum to indicate not close.

    \sa QSensor::outputRanges
*/

/*!
    \property QProximityReading::distance
    \brief the distance of the user.

    Measured in meters.
    \sa QProximityReading_Units
*/

qreal QProximityReading::distance() const
{
    return d->distance;
}

/*!
    Sets the \a distance to the user.
*/
void QProximityReading::setDistance(qreal distance)
{
    d->distance = distance;
}

// =====================================================================

/*!
    \class QProximityFilter
    \ingroup sensors_filter

    \preliminary
    \brief The QProximityFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QProximityReading
    instead of QSensorReading.
*/

/*!
    \fn QProximityFilter::filter(QProximityReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *QProximitySensor::type("QProximitySensor");

/*!
    \class QProximitySensor
    \ingroup sensors_type

    \preliminary
    \brief The QProximitySensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QProximityReading instead of a QSensorReading.

    For details about how the sensor works, see \l QProximityReading.

    \sa QProximityReading
*/

/*!
    \fn QProximitySensor::QProximitySensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QProximitySensor::~QProximitySensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QProximitySensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

#include "moc_qproximitysensor.cpp"
QTM_END_NAMESPACE

