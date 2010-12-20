/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qirproximitysensor.h>
#include "qirproximitysensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QIRProximityReading)

/*!
    \class QIRProximityReading
    \ingroup sensors_reading
    \inmodule QtSensors

    \brief The QIRProximityReading class holds readings from the IR proximity sensor.

    The IR (infra-red) proximity sensor detects proximity by beaming out infra-red light
    and detecting how much of the light returns.

    The biggest limitation of this technology is that there is no reliable way to turn the
    reflectance values into distances unless both the item being detected and the ambient
    conditions are known.

    \section2 QIRProximityReading Units

    The sensor reports reflectance as a percentage. That is, 0.0 indicates nothing was detected
    within the range of the sensor and 1.0 indicates the infra-red signal returned at the full
    power level that it was sent at.
*/

/*!
    \property QIRProximityReading::reflectance
    \brief Holds the reflectance value.

    The reflectance is a percentage (from 0.0 to 1.0) indicating how much of the transmitted
    infra-red light was returned.

    \sa {QIRProximityReading Units}
*/

qreal QIRProximityReading::reflectance() const
{
    return d->reflectance;
}

/*!
    Sets the reflectance value to \a reflectance.
*/
void QIRProximityReading::setReflectance(qreal reflectance)
{
    d->reflectance = reflectance;
}

// =====================================================================

/*!
    \class QIRProximityFilter
    \ingroup sensors_filter
    \inmodule QtSensors

    \brief The QIRProximityFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QIRProximityReading
    instead of QSensorReading.
*/

/*!
    \fn QIRProximityFilter::filter(QIRProximityReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

char const * const QIRProximitySensor::type("QIRProximitySensor");

/*!
    \class QIRProximitySensor
    \ingroup sensors_type
    \inmodule QtSensors

    \brief The QIRProximitySensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QIRProximityReading instead of a QSensorReading.

    For details about how the sensor works, see \l QIRProximityReading.

    \sa QIRProximityReading
*/

/*!
    \fn QIRProximitySensor::QIRProximitySensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QIRProximitySensor::~QIRProximitySensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QIRProximitySensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

/*!
    Returns the minimum distance that the sensor can detect.

    Note that this value is approximate.
*/
qreal QIRProximitySensor::minimumDistance() const
{
    return 0.002; // 2cm
}

/*!
    Returns the maximum distance that the sensor can detect.

    Note that this value is approximate.
*/
qreal QIRProximitySensor::maximumDistance() const
{
    return 0.02; // 20cm
}

/*!
    Returns the maximum power level the sensor supports.
*/
int QIRProximitySensor::maxPowerLevel() const
{
    return 0;
}

/*!
    Returns the current power level the sensor is using.
*/
int QIRProximitySensor::powerLevel() const
{
    return 0;
}

/*!
    Set the sensor's power level to \a level.

    Note that a higher level may enable more accurate detection but will consume more power.
*/
void QIRProximitySensor::setPowerLevel(int level)
{
    Q_UNUSED(level)
}

#include "moc_qirproximitysensor.cpp"
QTM_END_NAMESPACE
