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

#include "q3dcompass.h"
#include "q3dcompass_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(Q3dCompassReading)

/*!
    \class Q3dCompassReading
    \ingroup sensors

    \preliminary
    \brief The Q3dCompassReading class represents one reading from the
           3D compass.

    The 3D compass returns magnetic flux density values along 3 axes.
    Unlike the \l{QMagnetometerReading}{Magnetometer}, the 3D compass
    represents a filtered and processed view that aims to reflect only
    on the magnetic field of the earth.
    The scale of the values is teslas. The axes are arranged as follows.

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
*/

/*!
    \property Q3dCompassReading::x
    \brief the flux density on the X axis.
*/

/*!
    Returns the flux density on the X axis.
*/
qreal Q3dCompassReading::x() const
{
    return d->x;
}

/*!
    Sets the flux density on the X axis to \a x.
*/
void Q3dCompassReading::setX(qreal x)
{
    d->x = x;
}

/*!
    \property Q3dCompassReading::y
    \brief the flux density on the Y axis.
*/

/*!
    Returns the flux density on the Y axis.
*/
qreal Q3dCompassReading::y() const
{
    return d->y;
}

/*!
    Sets the flux density on the Y axis to \a y.
*/
void Q3dCompassReading::setY(qreal y)
{
    d->y = y;
}

/*!
    \property Q3dCompassReading::z
    \brief the flux density on the Z axis.
*/

/*!
    Returns the flux density on the Z axis.
*/
qreal Q3dCompassReading::z() const
{
    return d->z;
}

/*!
    Sets the flux density on the Z axis to \a z.
*/
void Q3dCompassReading::setZ(qreal z)
{
    d->z = z;
}

// =====================================================================

// begin generated code

/*!
    \class Q3dCompassFilter
    \ingroup sensors_helpers

    \preliminary
    \brief The Q3dCompassFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to Q3dCompassReading
    instead of QSensorReading.
*/

/*!
    \fn Q3dCompassFilter::filter(Q3dCompassReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *Q3dCompass::type("Q3dCompass");

/*!
    \class Q3dCompass
    \ingroup sensors_helpers

    \preliminary
    \brief The Q3dCompass class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a Q3dCompassReading instead of a QSensorReading.

    For details about how the sensor works, see \l Q3dCompassReading.

    \sa Q3dCompassReading
*/

/*!
    \fn Q3dCompass::Q3dCompass(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn Q3dCompass::~Q3dCompass()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn Q3dCompass::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/
// end generated code

#include "moc_q3dcompass.cpp"
QTM_END_NAMESPACE

