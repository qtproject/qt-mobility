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

#include <qcompass.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QCompassReading
    \ingroup sensors

    \preliminary
    \brief The QCompassReading class represents one reading from a
           compass.

    The compass returns the azimuth of the device as degrees from
    magnetic north in a clockwise direction based on the top of the device.
    Note that the top of the device is a fixed point and may not represent the
    orientation that the user is holding the device in.
    There is also a value to indicate the calibration status of the device.
    If the device is not calibrated the azimuth may not be accurate.

    Digital compasses are susceptible to magnetic interference and may need
    calibration after being placed near anything that emits a magnetic force.
    Accuracy of the compass can be affected by any ferrous materials that are nearby.
*/

/*!
    \enum QCompassReading::CalibrationLevel

    \value Undefined The calibration level is not defined or is too low.
    \value Low       The reported azimuth may be off by up to 15.4 degrees.
    \value Middle    The reported azimuth may be off by up to 11.8 degrees.
    \value High      The reported azimuth may be off by up to 7.4 degrees.
*/

/*!
    \fn QCompassReading::QCompassReading()
    \internal
*/

/*!
    \fn QCompassReading::QCompassReading(qtimestamp timestamp, qreal azimuth, CalibrationLevel calibration)
    \internal
*/

/*!
    \fn QCompassReading::QCompassReading(const QCompassReading &other)
    \internal
*/

/*!
    \fn QCompassReading::~QCompassReading()
    \internal
*/

/*!
    \fn QCompassReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QCompassReading::azimuth() const

    Returns the azimuth of the device.
*/

/*!
    \fn QCompassReading::calibrationLevel() const

    Returns the calibration level of the reading. The higher the calibration, the more accurate the measurement is.
*/

// =====================================================================

/*!
    \class QCompass
    \ingroup sensors

    \preliminary
    \brief The QCompass class reports on the azimuth of the device.

    The compass returns the azimuth of the device as degrees from
    magnetic north in a clockwise direction based on the top of the device.
    Note that the top of the device is a fixed point and may not represent the
    orientation that the user is holding the device in.
    There is also a value to indicate the calibration status of the device.
    If the device is not calibrated the azimuth may not be accurate.

    Digital compasses are highly susceptible to magnetic interference and
    may need calibration after being placed near anything that emits a magnetic
    force.
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a identifier is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QCompass::QCompass(QObject *parent, const QSensorId &identifier)
    : QSensor(parent)
{
    m_backend = static_cast<QCompassBackend*>(connectToBackend(identifier));
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QCompass::~QCompass()
{
    stop();
}

/*!
    \property QCompass::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QCompass::typeId
*/
const QString QCompass::typeId("qt.Compass");

/*!
    \fn QCompass::type() const
    \reimp
*/

/*!
    \fn QCompass::currentReading() const

    Returns the current compass reading.
*/

/*!
    \fn QCompass::azimuthChanged(const QCompassReading &reading)

    This signal is emitted when a new compass \a reading comes in.
*/

/*!
    \fn QCompass::backend() const
    \reimp
*/

#include "moc_qcompass.cpp"
QTM_END_NAMESPACE

