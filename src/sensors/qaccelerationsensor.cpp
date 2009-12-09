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

QTM_USE_NAMESPACE

/*!
    \class QAccelerationSensorValue
    \ingroup sensors

    \preliminary
    \brief The QAccelerationSensorValue class represents one value from the
           acceleration sensor.

    The values returned in QAccelerationSensorValue are normalized so that
    they are consistent between devices. The axes are arranged as follows.

    TODO picture showing axes relative to the monoblock form factor.

    The scale of the values is in metres per second per second. A monoblock
    device sitting at rest, face up on a desk will experience the force of
    gravity as approximately -9.8 on the Z axis.
*/

/*!
    \variable QAccelerationSensorValue::x

    Holds the acceleration for the X axis.
*/

/*!
    \variable QAccelerationSensorValue::y

    Holds the acceleration for the Y axis.
*/

/*!
    \variable QAccelerationSensorValue::z

    Holds the acceleration for the Z axis.
*/

// =====================================================================

/*!
    \class QAccelerationSensor
    \ingroup sensors

    \preliminary
    \brief The QAccelerationSensor class reports on linear acceleration
           along the X, Y and Z axes.

    The values returned by QAccelerationSensor are normalized so that
    they are consistent between devices. The axes are arranged as follows.

    TODO picture showing axes relative to the monoblock form factor.

    The scale of the values is in metres per second per second. A monoblock
    device sitting at rest, face up on a desk will experience the force of
    gravity as approximately -9.8 on the Z axis.

    \sa QAccelerationSensorValue
*/

/*!
    \variable QAccelerationSensor::type
*/
const QString QAccelerationSensor::type("qt.Acceleration");

/*!
    Construct a sensor instance and attach to the sensor indicated by \a id.
    The sensor will be deleted when \a parent is deleted.
*/
QAccelerationSensor::QAccelerationSensor(const QSensorID &id, QObject *parent)
    : QSensor(id, parent)
{
}

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

