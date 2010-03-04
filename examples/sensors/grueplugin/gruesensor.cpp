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

#include "gruesensor.h"
#include "gruesensor_p.h"

IMPLEMENT_READING(GrueSensorReading)

/*!
    \class GrueSensorReading
    \ingroup sensors_reading

    \preliminary
    \brief The GrueSensorReading class holds readings from the Grue sensor.

    The Grue Sensor informs you of your chance of being eaten by a Grue.

    Grues love the dark so as long as your surroundings are relatively light
    you are safe.
*/

/*!
    \property GrueSensorReading::chanceOfBeingEaten
    \brief holds your chance of being eaten.

    The value is the probability (from 0 to 1) that a Grue will eat you.
    A probability of 1 means you will be eaten. The darker it is, the more
    likely you are to be eaten by a Grue. If you are in a lit room, the
    probability will be 0.
*/

qreal GrueSensorReading::chanceOfBeingEaten() const
{
    return d->chanceOfBeingEaten;
}

void GrueSensorReading::setChanceOfBeingEaten(qreal chanceOfBeingEaten)
{
    d->chanceOfBeingEaten = chanceOfBeingEaten;
}

// =====================================================================

// begin generated code

/*!
    \class GrueFilter
    \ingroup sensors_filter

    \preliminary
    \brief The GrueFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to GrueSensorReading
    instead of QSensorReading.
*/

/*!
    \fn GrueFilter::filter(GrueSensorReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *GrueSensor::type("GrueSensor");

/*!
    \class GrueSensor
    \ingroup sensors_type

    \preliminary
    \brief The GrueSensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a GrueSensorReading instead of a QSensorReading.

    For details about how the sensor works, see \l GrueSensorReading.

    \sa GrueSensorReading
*/

/*!
    \fn GrueSensor::GrueSensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn GrueSensor::~GrueSensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn GrueSensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/
// end generated code

#include "moc_gruesensor.cpp"
