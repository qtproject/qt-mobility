/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlightsensor.h"
#include "qlightsensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QLightReading)

/*!
    \class QLightReading
    \ingroup sensors_reading
    \inmodule QtSensors

    \since 1.2

    \brief The QLightReading class represents one reading from the
           light sensor.

    \section2 QLightReading Units
    The light sensor returns the intensity of the light in lux.
*/

/*!
    \property QLightReading::lux
    \brief the light level.

    Measured as lux.
    \sa {QLightReading Units}
    \since 1.2
*/

qreal QLightReading::lux() const
{
    return d->lux;
}

/*!
    Sets the light level to \a lux.
    \since 1.2
*/
void QLightReading::setLux(qreal lux)
{
    d->lux = lux;
}

// =====================================================================

/*!
    \class QLightFilter
    \ingroup sensors_filter
    \inmodule QtSensors

    \brief The QLightFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QLightReading
    instead of QSensorReading.
    \since 1.2
*/

/*!
    \fn QLightFilter::filter(QLightReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
    \since 1.2
*/

char const * const QLightSensor::type("QLightSensor");

/*!
    \class QLightSensor
    \ingroup sensors_type
    \inmodule QtSensors

    \brief The QLightSensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QLightReading instead of a QSensorReading.

    For details about how the sensor works, see \l QLightReading.

    \sa QLightReading
    \since 1.2
*/

/*!
    \fn QLightSensor::QLightSensor(QObject *parent)

    Construct the sensor as a child of \a parent.
    \since 1.2
*/

/*!
    \fn QLightSensor::~QLightSensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
    \since 1.2
*/

/*!
    \fn QLightSensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
    \since 1.2
*/

/*!
    \property QLightSensor::fieldOfView
    \brief a value indicating the field of view.

    This is an angle that represents the field of view of the sensor.
    \since 1.2
*/

#include "moc_qlightsensor.cpp"
QTM_END_NAMESPACE

