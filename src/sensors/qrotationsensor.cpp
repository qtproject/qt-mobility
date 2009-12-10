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

#include <qrotationsensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QRotationValue
    \ingroup sensors

    \preliminary
    \brief The QRotationValue class represents an rotation value.

    Foo bar baz.
*/

/*!
    \variable QRotationValue::x

    Holds the rotation for the X axis.
*/

/*!
    \variable QRotationValue::y

    Holds the rotation for the Y axis.
*/

/*!
    \variable QRotationValue::z

    Holds the rotation for the Z axis.
*/

// =====================================================================

/*!
    \class QRotationSensor
    \ingroup sensors

    \preliminary
    \brief The QRotationSensor class reports on rotation
           on the X, Y and Z axes.

    Foo bar baz.
*/

/*!
    \variable QRotationSensor::TYPE
*/
const QString QRotationSensor::TYPE("qt.Rotation");

/*!
    \fn QRotationSensor::type() const
    \reimp
*/

/*!
    Construct a sensor instance and attach to the sensor indicated by \a id.
    The sensor will be deleted when \a parent is deleted.
*/
QRotationSensor::QRotationSensor(const QSensorID &id, QObject *parent)
    : QSensor(parent)
{
    connectToBackend(id);
}

/*!
    \fn QRotationSensor::currentXRotation() const

    Returns the current x rotation value from the sensor.
*/

/*!
    \fn QRotationSensor::currentYRotation() const

    Returns the current y rotation value from the sensor.
*/

/*!
    \fn QRotationSensor::currentZRotation() const

    Returns the current z rotation value from the sensor.
*/

QTM_END_NAMESPACE

