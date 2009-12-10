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

#include <qaccelerationsensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QAccelerationValue
    \ingroup sensors

    \preliminary
    \brief The QAccelerationValue class represents one value from the
           acceleration sensor.

    The values returned in QAccelerationValue are scaled so that
    they are consistent between devices. The axes are arranged as follows.

    TODO picture showing axes relative to the monoblock form factor.

    The scale of the values is in milli-Gs.
    A monoblock device sitting at rest, face up on a desk will experience
    the force of gravity as approximately -1000 on the Z axis.
*/

/*!
    \internal
*/
QAccelerationValue::QAccelerationValue()
    : QSensorValue(QAccelerationSensor::typeId)
{
}

/*!
    \variable QAccelerationValue::Acceleration::x

    Holds the acceleration force acting on the X axis.
*/

/*!
    \variable QAccelerationValue::Acceleration::y

    Holds the acceleration for the Y axis.
*/

/*!
    \variable QAccelerationValue::Acceleration::z

    Holds the acceleration for the Z axis.
*/

// =====================================================================

/*!
    \class QAccelerationSensor
    \ingroup sensors

    \preliminary
    \brief The QAccelerationSensor class reports on linear acceleration
           along the X, Y and Z axes.

    The values returned by QAccelerationSensor are scaled so that
    they are consistent between devices. The axes are arranged as follows.

    TODO picture showing axes relative to the monoblock form factor.

    The scale of the values is in milli-Gs.
    A monoblock device sitting at rest, face up on a desk will experience
    the force of gravity as approximately -1000 on the Z axis.

    \sa QAccelerationValue
*/

/*!
    \variable QAccelerationSensor::typeId
*/
const QString QAccelerationSensor::typeId("qt.Acceleration");

/*!
    \fn QAccelerationSensor::type() const
    \reimp
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a id is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QAccelerationSensor::QAccelerationSensor(QObject *parent, const QSensorId &id)
    : QSensor(parent)
{
    connectToBackend(id);
}

/*!
    \fn QAccelerationSensor::currentAcceleration() const

    Returns the current acceleration values from the sensor.
*/

/*!
    \fn QAccelerationSensor::currentXAcceleration() const

    Returns the current x acceleration value from the sensor.
*/

/*!
    \fn QAccelerationSensor::currentYAcceleration() const

    Returns the current y acceleration value from the sensor.
*/

/*!
    \fn QAccelerationSensor::currentZAcceleration() const

    Returns the current z acceleration value from the sensor.
*/

QTM_END_NAMESPACE

