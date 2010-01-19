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
#include "qtapsensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QTapReading)

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
    \property QTapReading::tapDirection
    \brief foo
*/

/*!
    Foo
*/
QTapReading::TapDirection QTapReading::tapDirection() const
{
    return static_cast<QTapReading::TapDirection>(d->tapDirection);
}

/*!
    Foo
*/
void QTapReading::setTapDirection(QTapReading::TapDirection tapDirection)
{
    d->tapDirection = tapDirection;
}

/*!
    \property QTapReading::doubleTap
    \brief foo
*/

/*!
    Foo
*/
bool QTapReading::isDoubleTap() const
{
    return d->doubleTap;
}

/*!
    Foo
*/
void QTapReading::setDoubleTap(bool doubleTap)
{
    d->doubleTap = doubleTap;
}

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

#include "moc_qtapsensor.cpp"
QTM_END_NAMESPACE

