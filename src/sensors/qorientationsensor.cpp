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
    \class QOrientationSensor
    \ingroup sensors

    \preliminary
    \brief The QOrientationSensor class reports on the orientation of the screen.

    The orientation sensor provides discrete orientation information. See the
    QOrientationSensor::Orientation enum for
*/

/*!
    \enum QOrientationSensor::OrientationFlag

    This enum represents the orientation of the device.

    \value Unknown    The orientation is unknown. For example, the
                      device may be lying face down.
    \value Portrait   The device is in the portrait orientation.
    \value Landscape  The device is in the landscape orientation.
    \value Inverted   Combined with Portrait or Landscape to indicate
                      the device is upside down.
    \value Default    Combined with Portrait or Landscape to indicate
                      the device is in the default orientation.

    Note that Portrait and Landscape may be combined with Inverted to
    indicate that the device is upside down.
*/

/*!
    Returns the current orientation of the device.
*/
QOrientationSensor::Orientation QOrientationSensor::currentOrientation() const
{
    return QOrientationSensor::Unknown;
}

/*!
    \class QOrientationSensorEvent
    \ingroup sensors

    \preliminary
    \brief The QOrientationSensorEvent class represents an orientation event.

    Foo bar baz.
*/

