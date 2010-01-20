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
#include "qrotationsensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QRotationReading)

/*!
    \class QRotationReading
    \ingroup sensors

    \preliminary
    \brief The QRotationReading class represents one reading from the
           rotation sensor.

    The rotation sensor returns the rotation of the device along the X, Y and Z
    axes. The scale of the values is radians. The axes are arranged as follows.

\code
             +z
              |
              |      +y
              |     /
              |----/----
             /| NOKIA  /|
            //|--/--- / |
           // | /   //  /
          //  |/   //  /
         //   '--------------- +x
        //       //  /
       //       //  /
      /---------/  /
     /    O    /  /
    /         /  /
    ----------  /
    |_________!/
\endcode

    Note that the values for the rotation sensor come from an accelerometer
    so a device resting on its back will not be able to detect rotation around the
    Z axis. Rotation can only be detected when it happens relative to gravity.
*/

/*!
    \property QRotationReading::x
    \brief the rotation on the X axis.
*/

/*!
    Returns the rotation on the X axis.
*/
qreal QRotationReading::x() const
{
    return d->x;
}

/*!
    Sets the rotation on the X axis to \a x.
*/
void QRotationReading::setX(qreal x)
{
    d->x = x;
}

/*!
    \property QRotationReading::y
    \brief the rotation on the Y axis.
*/

/*!
    Returns the rotation on the Y axis.
*/
qreal QRotationReading::y() const
{
    return d->y;
}

/*!
    Sets the rotation on the Y axis to \a y.
*/
void QRotationReading::setY(qreal y)
{
    d->y = y;
}

/*!
    \property QRotationReading::z
    \brief the rotation on the Z axis.
*/

/*!
    Returns the rotation on the Z axis.
*/
qreal QRotationReading::z() const
{
    return d->z;
}

/*!
    Sets the rotation on the Z axis to \a z.
*/
void QRotationReading::setZ(qreal z)
{
    d->z = z;
}

// =====================================================================

// begin generated code

/*!
    \class QRotationFilter
    \ingroup sensors_helpers

    \preliminary
    \brief The QRotationFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QRotationReading
    instead of QSensorReading.
*/

/*!
    \fn QRotationFilter::filter(QRotationReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

/*!
    \class QRotationSensor
    \ingroup sensors_helpers

    \preliminary
    \brief The QRotationSensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QRotationReading instead of a QSensorReading.

    For details about how the sensor works, see \l QRotationReading.

    \sa QRotationReading
*/

/*!
    \fn QRotationSensor::QRotationSensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QRotationSensor::~QRotationSensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QRotationSensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/
// end generated code

#include "moc_qrotationsensor.cpp"
QTM_END_NAMESPACE

