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

#include <qambientlightsensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QAmbientLightReading
    \ingroup sensors

    \preliminary
    \brief The QAmbientLightReading class represents one reading from the
           ambient light sensor.

    The ambient light sensor returns the intensity of the ambient light
    using the pre-defined values found in the QAmbientLightReading::LightLevel
    enum.
*/

/*!
    \enum QAmbientLightReading::LightLevel

    This enum represents the ambient light level.

    \value Undefined  The light level is unknown.
    \value Dark       It is dark.
    \value Twilight   It is darkish.
    \value Light      It is light (eg. internal lights).
    \value Bright     It is bright (eg. shade).
    \value Sunny      It is very bright (eg. direct sunlight).
*/

/*!
    \fn QAmbientLightReading::QAmbientLightReading()
    \internal
*/

/*!
    \fn QAmbientLightReading::QAmbientLightReading(qtimestamp timestamp, LightLevel lightLevel)
    \internal
*/

/*!
    \fn QAmbientLightReading::QAmbientLightReading(const QAmbientLightReading &other)
    \internal
*/

/*!
    \fn QAmbientLightReading::~QAmbientLightReading()
    \internal
*/

/*!
    \fn QAmbientLightReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QAmbientLightReading::lightLevel() const

    Returns the ambient light level.
*/

// =====================================================================

/*!
    \class QAmbientLightSensor
    \ingroup sensors

    \preliminary
    \brief The QAmbientLightSensor class reports on ambient light levels.

    The ambient light sensor returns the intensity of the ambient light
    using the pre-defined values found in the QAmbientLightReading::LightLevel
    enum.

    \sa QAmbientLightReading
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a identifier is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QAmbientLightSensor::QAmbientLightSensor(QObject *parent, const QSensorId &identifier)
    : QSensor(parent)
{
    m_backend = static_cast<QAmbientLightBackend*>(connectToBackend(identifier));
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QAmbientLightSensor::~QAmbientLightSensor()
{
    stop();
}

/*!
    \property QAmbientLightSensor::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QAmbientLightSensor::typeId
*/
const QString QAmbientLightSensor::typeId("qt.AmbientLight");

/*!
    \fn QAmbientLightSensor::type() const
    \reimp
*/

/*!
    \fn QAmbientLightSensor::currentReading() const

    Returns the current ambient light reading.
*/

/*!
    \fn QAmbientLightSensor::ambientLightChanged(const QAmbientLightReading &reading)

    This signal is emitted when a new ambient light \a reading comes in.
*/

/*!
    \fn QAmbientLightSensor::backend() const
    \reimp
*/

#include "moc_qambientlightsensor.cpp"
QTM_END_NAMESPACE

