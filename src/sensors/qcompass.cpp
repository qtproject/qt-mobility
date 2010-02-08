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

#include "qcompass.h"
#include "qcompass_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QCompassReading)

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
    There is also an uncertainty arc. This will be larger when
    compass readings are less accurate.

    Digital compasses are susceptible to magnetic interference and may need
    calibration after being placed near anything that emits a magnetic force.
    Accuracy of the compass can be affected by any ferrous materials that are nearby.
*/

/*!
    \property QCompassReading::azimuth
    \brief the azimuth of the device.
*/

/*!
    Returns the azimuth of the device.
*/
qreal QCompassReading::azimuth() const
{
    return d->azimuth;
}

/*!
    Sets the \a azimuth of the device.
*/
void QCompassReading::setAzimuth(qreal azimuth)
{
    d->azimuth = azimuth;
}

/*!
    \property QCompassReading::uncertaintyArc
    \brief the angle of uncertainty.

    The arc will increase as the accuracty of the compass drops.
*/

/*!
    Returns the uncertainty arc.
*/
qreal QCompassReading::uncertaintyArc() const
{
    return d->uncertaintyArc;
}

/*!
    Sets the uncertainty arc to \a uncertaintyArc.
*/
void QCompassReading::setUncertaintyArc(qreal uncertaintyArc)
{
    d->uncertaintyArc = uncertaintyArc;
}

// =====================================================================

// begin generated code

/*!
    \class QCompassFilter
    \ingroup sensors_helpers

    \preliminary
    \brief The QCompassFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QCompassReading
    instead of QSensorReading.
*/

/*!
    \fn QCompassFilter::filter(QCompassReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *QCompass::type("QCompass");

/*!
    \class QCompass
    \ingroup sensors_helpers

    \preliminary
    \brief The QCompass class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QCompassReading instead of a QSensorReading.

    For details about how the sensor works, see \l QCompassReading.

    \sa QCompassReading
*/

/*!
    \fn QCompass::QCompass(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QCompass::~QCompass()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QCompass::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/
// end generated code

#include "moc_qcompass.cpp"
QTM_END_NAMESPACE

