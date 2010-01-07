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

#include <qmagneticnorthsensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QMagneticNorthReading
    \ingroup sensors

    \preliminary
    \brief The QMagneticNorthReading class represents one reading from a
           magnetic north sensor (compass).

    The magnetic north sensor returns the heading of the device as degrees from
    magnetic north in a clockwise direction based on the top of the device.
    Note that the top of the device is a fixed point and may not represent the
    orientation that the user is holding the device in.
    There is also a value to indicate the calibration status of the device.
    If the device is not calibrated the heading may not be accurate.

    Digital compasses are highly susceptible to magnetic interference and
    may need calibration after being placed near anything that emits a magnetic
    force.
*/

/*!
    \fn QMagneticNorthReading::QMagneticNorthReading()
    \internal
*/

/*!
    \fn QMagneticNorthReading::QMagneticNorthReading(QDateTime timestamp, int heading, bool calibrated)
    \internal
*/

/*!
    \fn QMagneticNorthReading::QMagneticNorthReading(const QMagneticNorthReading &other)
    \internal
*/

/*!
    \fn QMagneticNorthReading::~QMagneticNorthReading()
    \internal
*/

/*!
    \fn QMagneticNorthReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QMagneticNorthReading::heading() const

    Returns the heading of the device.
*/

/*!
    \fn QMagneticNorthReading::isCalibrated() const

    Returns \c true if the device was calibrated when the reading
    was made and \c false otherwise. You should not trust a reading
    from an uncalibrated device.
*/

// =====================================================================

/*!
    \class QMagneticNorthSensor
    \ingroup sensors

    \preliminary
    \brief The QMagneticNorthSensor class reports on the heading of the device.

    The magnetic north sensor returns the heading of the device as degrees from
    magnetic north in a clockwise direction based on the top of the device.
    Note that the top of the device is a fixed point and may not represent the
    orientation that the user is holding the device in.
    There is also a value to indicate the calibration status of the device.
    If the device is not calibrated the heading may not be accurate.

    Digital compasses are highly susceptible to magnetic interference and
    may need calibration after being placed near anything that emits a magnetic
    force.
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a id is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QMagneticNorthSensor::QMagneticNorthSensor(QObject *parent, const QSensorId &id)
    : QSensor(parent)
{
    m_backend = static_cast<QMagneticNorthBackend*>(connectToBackend(id));
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QMagneticNorthSensor::~QMagneticNorthSensor()
{
    stop();
}

/*!
    \property QMagneticNorthSensor::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QMagneticNorthSensor::typeId
*/
const QString QMagneticNorthSensor::typeId("qt.MagneticNorth");

/*!
    \fn QMagneticNorthSensor::type() const
    \reimp
*/

/*!
    \fn QMagneticNorthSensor::currentReading() const

    Returns the current magnetic north reading.
*/

/*!
    \fn QMagneticNorthSensor::headingChanged(const QMagneticNorthReading &reading)

    This signal is emitted when a new magnetic north \a reading comes in.
*/

/*!
    \fn QMagneticNorthSensor::backend() const
    \reimp
*/

#include "moc_qmagneticnorthsensor.cpp"
QTM_END_NAMESPACE

