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

#include "qattitudesensor.h"
#include "qattitudesensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QAttitudeReading)

/*!
    \class QAttitudeReading
    \ingroup sensors_reading

    \preliminary
    \brief The QAttitudeReading class represents one reading from the
           attitude sensor.

    \section2 QAttitudeReading Units
    The attitude sensor returns the attitude of the device. The terms used come
    from \l{http://en.wikipedia.org/wiki/Flight_dynamics}{flight dynamics}.
    The pitch of the device is the rotation around the X axis, the roll of the device
    is teh rotation around the Y axis and the yaw of the device is the rotation
    around the Z axis.

    \image Flight_dynamics_with_text.png

    \l{http://en.wikipedia.org/wiki/File:Flight_dynamics_with_text.png}

    All three values are measured in degrees but their range is limited so that a given
    attitude can be represented uniquely.

    \list
    \o Pitch: -180 to 180
    \o Roll: -90 to 90
    \o Yaw: 0 to 360
    \endlist

    Pitch and Roll will be at 0 when the phone is lying flat and face up. Yaw will
    be at 0 when the top of the phone is pointing towards the reference point.

    Note that yaw requires a fixed, external reference point that the device can
    detect. For example, devices with a compass may use magnetic north as
    a reference point. Devices that cannot detect a fixed, external reference
    point will always return 0 for yaw.
*/

/*!
    \property QAttitudeReading::pitch
    \brief the pitch of the device.

    Measured as degrees.
    \sa {QAttitudeReading Units}
*/

qreal QAttitudeReading::pitch() const
{
    return d->pitch;
}

/*!
    Sets the \a pitch of the device.
*/
void QAttitudeReading::setPitch(qreal pitch)
{
    d->pitch = pitch;
}

/*!
    \property QAttitudeReading::roll
    \brief the roll of the device.

    Measured as degrees.
    \sa {QAttitudeReading Units}
*/

qreal QAttitudeReading::roll() const
{
    return d->roll;
}

/*!
    Sets the \a roll of the device.
*/
void QAttitudeReading::setRoll(qreal roll)
{
    d->roll = roll;
}

/*!
    \property QAttitudeReading::yaw
    \brief the yaw of the device.

    Measured as degrees.
    \sa {QAttitudeReading Units}
*/

qreal QAttitudeReading::yaw() const
{
    return d->yaw;
}

/*!
    Sets the \a yaw of the device.
*/
void QAttitudeReading::setYaw(qreal yaw)
{
    d->yaw = yaw;
}

// =====================================================================

/*!
    \class QAttitudeFilter
    \ingroup sensors_filter

    \preliminary
    \brief The QAttitudeFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QAttitudeReading
    instead of QSensorReading.
*/

/*!
    \fn QAttitudeFilter::filter(QAttitudeReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *QAttitudeSensor::type("QAttitudeSensor");

/*!
    \class QAttitudeSensor
    \ingroup sensors_type

    \preliminary
    \brief The QAttitudeSensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QAttitudeReading instead of a QSensorReading.

    For details about how the sensor works, see \l QAttitudeReading.

    \sa QAttitudeReading
*/

/*!
    \fn QAttitudeSensor::QAttitudeSensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QAttitudeSensor::~QAttitudeSensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QAttitudeSensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

/*!
    \property QAttitudeSensor::yawAvailable
    \brief a value indicating if yaw is available.

    Returns true if yaw is available.
    Returns false if yaw is not available.
*/

#include "moc_qattitudesensor.cpp"
QTM_END_NAMESPACE

