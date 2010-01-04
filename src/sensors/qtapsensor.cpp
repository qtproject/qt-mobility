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

    The tap sensor registers tap and double tap events along one of the
    three axes. The axes are arranged as follows.
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

    Foobar

    \value X
    \value Y
    \value Z
*/

/*!
    \fn QTapReading::QTapReading()
    \internal
*/

/*!
    \fn QTapReading::QTapReading(QTime timestamp, TapDirection tapDirection, int tapCount)
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
    \fn QTapReading::tapCount() const

    Returns 1 for a tap, 2 for a double tap.
*/

// =====================================================================

/*!
    \class QTapSensor
    \ingroup sensors

    \preliminary
    \brief The QTapSensor class reports on taps.

    The tap sensor registers tap and double tap events along one of the
    three axes. The axes are arranged as follows.
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

