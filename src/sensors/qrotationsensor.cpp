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

QTM_BEGIN_NAMESPACE

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
    \fn QRotationReading::QRotationReading()
    \internal
*/

/*!
    \fn QRotationReading::QRotationReading(qtimestamp timestamp, qreal x, qreal y, qreal z)
    \internal
*/

/*!
    \fn QRotationReading::QRotationReading(const QRotationReading &other)
    \internal
*/

/*!
    \fn QRotationReading::~QRotationReading()
    \internal
*/

/*!
    \fn QRotationReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QRotationReading::x() const

    Returns the rotation for the X axis.
*/

/*!
    \fn QRotationReading::y() const

    Returns the rotation for the Y axis.
*/

/*!
    \fn QRotationReading::z() const

    Returns the rotation for the Z axis.
*/

// =====================================================================

/*!
    \class QRotationSensor
    \ingroup sensors

    \preliminary
    \brief The QRotationSensor class reports on rotation
           on the X, Y and Z axes.

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
    Construct a sensor instance with specified \a parent.
    If the \a identifier is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QRotationSensor::QRotationSensor(QObject *parent, const QByteArray &identifier)
    : QSensor(parent)
{
    m_backend = static_cast<QRotationBackend*>(connectToBackend(identifier));
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QRotationSensor::~QRotationSensor()
{
    stop();
}

/*!
    \property QRotationSensor::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QRotationSensor::typeId
*/
const QString QRotationSensor::typeId("qt.Rotation");

/*!
    \fn QRotationSensor::type() const
    \reimp
*/

/*!
    \fn QRotationSensor::currentReading() const

    Returns the current rotation reading.
*/

/*!
    \fn QRotationSensor::rotationChanged(const QRotationReading &reading)

    This signal is emitted when a new rotation \a reading comes in.
*/

/*!
    \fn QRotationSensor::backend() const
    \reimp
*/

#include "moc_qrotationsensor.cpp"
QTM_END_NAMESPACE

