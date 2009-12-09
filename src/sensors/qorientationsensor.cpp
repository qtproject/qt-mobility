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

#include <qorientationsensor.h>

QTM_USE_NAMESPACE

/*!
    \class QOrientationSensorValue
    \ingroup sensors

    \preliminary
    \brief The QOrientationSensorValue class represents an orientation value.

    The orientation is expressed as either portrait or landscape with an
    inverted flag indicating the phone is upside down.
*/

/*!
    \enum QOrientationSensorValue::OrientationFlag

    This enum represents the orientation of the device.

    \value Unknown    The orientation is unknown.
    \value Portrait   The device is in the portrait orientation.
    \value Landscape  The device is in the landscape orientation.
    \value Inverted   The device is upside down.

    Note that Portrait and Landscape may be combined with Inverted to
    indicate that the device is upside down.
*/

/*!
    \variable QOrientationSensorValue::orientation

    Holds the orientation flags.
*/

// =====================================================================

/*!
    \class QOrientationSensor
    \ingroup sensors

    \preliminary
    \brief The QOrientationSensor class reports on the orientation of the screen.

    The orientation sensor provides discrete orientation information. See the
    QOrientationSensor::Orientation enum for
*/

/*!
    \variable QOrientationSensor::type
*/
const QString QOrientationSensor::type("qt.Orientation");

/*!
    Construct a sensor instance and attach to the sensor indicated by \a id.
    The sensor will be deleted when \a parent is deleted.
*/
QOrientationSensor::QOrientationSensor(const QSensorID &id, QObject *parent)
    : QSensor(id, parent)
{
}

/*!
    \fn QOrientationSensor::currentOrientation() const

    Returns the current orientation of the device.
*/

/*!
    \fn QOrientationSensor::orientationChanged(QOrientationSensorValue::Orientation orientation)

    Emitted when the \a orientation value changes.
*/

QTM_BEGIN_NAMESPACE
#include "moc_qorientationsensor.cpp"
QTM_END_NAMESPACE

