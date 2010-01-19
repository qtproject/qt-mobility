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
#include "qorientationsensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QOrientationReading)

/*!
    \class QOrientationReading
    \ingroup sensors

    \preliminary
    \brief The QOrientationReading class represents one reading from the
           orientation sensor.

    The orientation sensor returns the orientation of the device using
    the pre-defined values found in the QOrientationReading::Orientation
    enum.
*/

/*!
    \enum QOrientationReading::Orientation

    This enum represents the orientation of the device.

    The parts of the phone are defined as follows.

\code
                      Top


              ----------
             /  NOKIA  /|
            /-------- / |
           //       //  /
          //       //  /
  Left   //  Face //  /    Right
        //       //  /
       //       //  /
      /---------/  /
     /    O    /  /
    /         /  /
    ----------  /
    |_________!/


      Bottom
\endcode

    \value Undefined  The orientation is unknown.
    \value BottomUp   The device is upside down.
    \value BottomDown The device is the right way up.
    \value LeftUp     The device has been rotated clockwise.
    \value RightUp    The device has been rotated counter-clockwise.
    \value FaceDown   The screen is facing down.
    \value FaceUp     The screen is facing up.
*/

/*!
    \property QOrientationReading::orientation
    \brief foo
*/

/*!
    Foo
*/
QOrientationReading::Orientation QOrientationReading::orientation() const
{
    return static_cast<QOrientationReading::Orientation>(d->orientation);
}

/*!
    Foo
*/
void QOrientationReading::setOrientation(QOrientationReading::Orientation orientation)
{
    d->orientation = orientation;
}

// =====================================================================

/*!
    \class QOrientationSensor
    \ingroup sensors

    \preliminary
    \brief The QOrientationSensor class reports on the orientation of the device.

    The orientation sensor returns the orientation of the device using
    the pre-defined values found in the QOrientationReading::Orientation
    enum.

    \sa QOrientationReading
*/

#include "moc_qorientationsensor.cpp"
QTM_END_NAMESPACE

