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
    \class QAmbientLightValue
    \ingroup sensors

    \preliminary
    \brief The QAmbientLightValue class represents an ambient light reading.

    The ambient light sensor returns the intensity of the ambient light in lux.
*/

/*!
    \internal
*/
QAmbientLightValue::QAmbientLightValue()
    : QSensorValue(QAmbientLightSensor::typeId)
{
}

/*!
    \variable QAmbientLightValue::lux

    Holds the intensity of the ambient light.
*/

// =====================================================================

/*!
    \class QAmbientLightSensor
    \ingroup sensors

    \preliminary
    \brief The QAmbientLightSensor class reports on ambient light levels.

    The ambient light sensor returns the intensity of the ambient light in lux.
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
    Construct a sensor instance with specified \a parent.
    If the \a id is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QAmbientLightSensor::QAmbientLightSensor(QObject *parent, const QSensorId &id)
    : QSensor(parent)
{
    connectToBackend(id);
}

/*!
    \fn QAmbientLightSensor::currentAmbientLight() const

    Returns the current ambient light value.
*/

/*!
    \fn QAmbientLightSensor::ambientLightChanged(int lux)

    Emitted when the ambient light \a lux value changes.
*/

#include "moc_qambientlightsensor.cpp"
QTM_END_NAMESPACE

