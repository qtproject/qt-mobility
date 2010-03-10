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

#include "qtapsensor.h"
#include "qtapsensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QTapReading)

/*!
    \class QTapReading
    \ingroup sensors_reading

    \preliminary
    \brief The QTapReading class represents one reading from the
           tap sensor.

    \section2 QTapReading Units
    The tap sensor registers tap and double tap events in one of the six directions.
    There are 3 axes that originate from the phone. They are arranged as follows.
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
    \enum QTapReading::TapDirection

    The tap direction is reported as one of the six directions (X, Y, Z, positive and negative).
    There are 3 flags that you can use if you only care about the axis in use.

    \value Undefined This value means that the direction is unknown.
    \value X     This flag is set if the tap was along the X axis.
    \value Y     This flag is set if the tap was along the Y axis.
    \value Z     This flag is set if the tap was along the Z axis.
    \value X_Pos This value is returned if the tap was towards the positive X direction.
    \value Y_Pos This value is returned if the tap was towards the positive Y direction.
    \value Z_Pos This value is returned if the tap was towards the positive Z direction.
    \value X_Neg This value is returned if the tap was towards the negative X direction.
    \value Y_Neg This value is returned if the tap was towards the negative Y direction.
    \value Z_Neg This value is returned if the tap was towards the negative Z direction.
*/

/*!
    \property QTapReading::tapDirection
    \brief the direction of the tap.

    \sa {QTapReading Units}
*/

QTapReading::TapDirection QTapReading::tapDirection() const
{
    return static_cast<QTapReading::TapDirection>(d->tapDirection);
}

/*!
    Sets the tap direction to \a tapDirection.
*/
void QTapReading::setTapDirection(QTapReading::TapDirection tapDirection)
{
    d->tapDirection = tapDirection;
}

/*!
    \property QTapReading::doubleTap
    \brief a value indicating if there was a single or double tap.

    \list
    \o true - double tap
    \o false - single tap
    \endlist
    \sa {QTapReading Units}
*/

bool QTapReading::isDoubleTap() const
{
    return d->doubleTap;
}

/*!
    Sets the double tap status of the reading to \a doubleTap.
*/
void QTapReading::setDoubleTap(bool doubleTap)
{
    d->doubleTap = doubleTap;
}

// =====================================================================

/*!
    \class QTapFilter
    \ingroup sensors_filter

    \preliminary
    \brief The QTapFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QTapReading
    instead of QSensorReading.
*/

/*!
    \fn QTapFilter::filter(QTapReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *QTapSensor::type("QTapSensor");

/*!
    \class QTapSensor
    \ingroup sensors_type

    \preliminary
    \brief The QTapSensor class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QTapReading instead of a QSensorReading.

    For details about how the sensor works, see \l QTapReading.

    \sa QTapReading
*/

/*!
    \fn QTapSensor::QTapSensor(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QTapSensor::~QTapSensor()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QTapSensor::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

/*!
    \property QTapSensor::returnTapEvents
    \brief a value indicating if tap events should be reported.

    Set to true to have the sensor report on tap events.
    Set to false to have the sensor suppress tap events.

    Note that you must access this property via QObject::property() and QObject::setProperty().
    The property must be set before calling start().
*/

/*!
    \property QTapSensor::returnDoubleTapEvents
    \brief a value indicating if double tap events should be reported.

    Set to true to have the sensor report on double tap events.
    Set to false to have the sensor suppress double tap events.

    Note that you must access this property via QObject::property() and QObject::setProperty().
    The property must be set before calling start().
*/

#include "moc_qtapsensor.cpp"
QTM_END_NAMESPACE

