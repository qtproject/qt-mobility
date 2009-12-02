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
    \class QAccelerationSensor
    \ingroup sensors

    \preliminary
    \brief The QAccelerationSensor class reports on linear acceleration
           along the X, Y and Z axes.

    Foo bar baz.
*/

/*!
    Construct a sensor instance and attach to the sensor indicated by \a id.
    The sensor will be deleted when \a parent is deleted.
*/
QAccelerationSensor::QAccelerationSensor(const QString &id, QObject *parent)
    : QSensor(id, parent)
{
}

/*!
    \class QAccelerationSensor::Acceleration

    \preliminary
    \brief The Acceleration class holds acceleration values from
           QAccelerationSensor.

    Foo bar baz.
*/

/*!
    \fn QAccelerationSensor::Acceleration::Acceleration(qreal,qreal,qreal)
    \internal
*/

/*!
    Returns the current acceleration values from the sensor.
*/
QAccelerationSensor::Acceleration QAccelerationSensor::currentAcceleration() const
{
    return QAccelerationSensor::Acceleration(0.0, 0.0, 0.0);
}

/*!
    Returns the current x acceleration value from the sensor.
*/
qreal QAccelerationSensor::currentXAcceleration() const
{
    return 0.0;
}

/*!
    Returns the current y acceleration value from the sensor.
*/
qreal QAccelerationSensor::currentYAcceleration() const
{
    return 0.0;
}

/*!
    Returns the current z acceleration value from the sensor.
*/
qreal QAccelerationSensor::currentZAcceleration() const
{
    return 0.0;
}

/*!
    \class QAccelerationSensorEvent
    \ingroup sensors

    \preliminary
    \brief The QAccelerationSensorEvent class represents an acceleration event.

    Foo bar baz.
*/

