/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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
