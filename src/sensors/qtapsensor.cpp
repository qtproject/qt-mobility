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

#include <qtapsensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QTapReading
    \ingroup sensors

    \preliminary
    \brief The QTapReading class represents one reading from the
           tap sensor.

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
    \fn QTapReading::QTapReading()
    \internal
*/

/*!
    \fn QTapReading::QTapReading(QDateTime timestamp, TapDirection tapDirection, bool doubleTap)
    \internal
*/

/*!
    \fn QTapReading::QTapReading(const QTapReading &other)
    \internal
*/

/*!
    \fn QTapReading::~QTapReading()
    \internal
*/

/*!
    \fn QTapReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QTapReading::tapDirection() const

    Returns the direction of the tap.
*/

/*!
    \fn QTapReading::isDoubleTap() const

    Returns true if the tap was a double tap, false if it was a single tap.
*/

// =====================================================================

/*!
    \class QTapSensor
    \ingroup sensors

    \preliminary
    \brief The QTapSensor class reports on taps.

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

    \sa QTapReading
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a id is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QTapSensor::QTapSensor(QObject *parent, const QSensorId &id)
    : QSensor(parent)
{
    m_backend = static_cast<QTapBackend*>(connectToBackend(id));
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QTapSensor::~QTapSensor()
{
    stop();
}

/*!
    \property QTapSensor::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QTapSensor::typeId
*/
const QString QTapSensor::typeId("qt.Tap");

/*!
    \fn QTapSensor::type() const
    \reimp
*/

/*!
    \fn QTapSensor::currentReading() const

    Returns the current tap reading.
*/

/*!
    \fn QTapSensor::tapDetected(const QTapReading &reading)

    This signal is emitted when a tap \a reading comes in.
*/

/*!
    \fn QTapSensor::backend() const
    \reimp
*/

#include "moc_qtapsensor.cpp"
QTM_END_NAMESPACE

