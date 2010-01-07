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

#include <qmagnetometersensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QMagnetometerReading
    \ingroup sensors

    \preliminary
    \brief The QMagnetometerReading class represents one reading from the
           magnetometer sensor.

    The magnetometer sensor returns magnetic flux values along 3 axes.
    The scale of the values is unknown. The axes are arranged as follows.

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

    Similarly to the accelerometer, the magnetometer can be used to find
    magnetic north. The strongest reading can be obtained for an axis by
    pointing that axis directly at magnetic north.
*/

/*!
    \fn QMagnetometerReading::QMagnetometerReading()
    \internal
*/

/*!
    \fn QMagnetometerReading::QMagnetometerReading(QDateTime timestamp, int x, int y, int z)
    \internal
*/

/*!
    \fn QMagnetometerReading::QMagnetometerReading(const QMagnetometerReading &other)
    \internal
*/

/*!
    \fn QMagnetometerReading::~QMagnetometerReading()
    \internal
*/

/*!
    \fn QMagnetometerReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QMagnetometerReading::x() const

    Returns the flux density for the X axis.
*/

/*!
    \fn QMagnetometerReading::y() const

    Returns the flux density for the Y axis.
*/

/*!
    \fn QMagnetometerReading::z() const

    Returns the flux density for the Z axis.
*/

// =====================================================================

/*!
    \class QMagnetometerListener
    \ingroup sensors

    \preliminary
    \brief The QMagnetometerListener class provides an efficient
           callback facility for asynchronous notifications of
           flux density changes.

    Since the magnetometer may be accessed very frequently it may
    not by suitable to use signals and slots. The QMagnetometerListener
    interface provides a way for the sensor to notify your class that a
    new flux density reading is available more efficiently.
*/

/*!
    \fn QMagnetometerListener::magnetometerChanged(const QMagnetometerReading &reading)

    This function is called when a new flux desity \a reading is available.
*/

// =====================================================================

/*!
    \class QMagnetometerSensor
    \ingroup sensors

    \preliminary
    \brief The QMagnetometerSensor class reports on flux density
           along the X, Y and Z axes.

    The acceleration sensor returns acceleration values along 3 axes.
    The scale of the values is milli-Gs. The axes are arranged as follows.

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

    A monoblock device sitting at rest, face up on a desk will experience
    the force of gravity as approximately -1000 on the Z axis.

    \sa QMagnetometerReading
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a id is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QMagnetometerSensor::QMagnetometerSensor(QObject *parent, const QSensorId &id)
    : QSensor(parent)
    , m_listener(0)
{
    m_backend = static_cast<QMagnetometerBackend*>(connectToBackend(id));
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QMagnetometerSensor::~QMagnetometerSensor()
{
    stop();
}

/*!
    \property QMagnetometerSensor::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QMagnetometerSensor::typeId
*/
const QString QMagnetometerSensor::typeId("qt.Magnetometer");

/*!
    \fn QMagnetometerSensor::type() const
    \reimp
*/

/*!
    Set the \a listener for the sensor. Pass 0 to clear a previously-set
    listener.
    The listener will be invoked every time a new reading is available.

    Note that the sensor does not take ownership of the listener.
    It is the caller's responsibility to ensure the listener remains valid
    until the sensor is destroyed or the listener is removed.
*/
void QMagnetometerSensor::setListener(QMagnetometerListener *listener)
{
    m_listener = listener;
}

/*!
    \fn QMagnetometerSensor::currentReading() const

    Returns the current ambient light reading.
*/

/*!
    \fn QMagnetometerSensor::accelerationChanged(const QMagnetometerReading &reading)

    This signal is emitted when a new acceleration \a reading comes in.

    Note that this signal should not be used if you are requesting
    high-frequency updates as signal delivery is quite slow.
*/

/*!
    \fn QMagnetometerSensor::backend() const
    \reimp
*/

void QMagnetometerSensor::newReadingAvailable()
{
    QMagnetometerReading reading = currentReading();
    if (m_listener)
        m_listener->accelerationChanged(reading);
    emit accelerationChanged(reading);
}

#include "moc_qaccelerationsensor.cpp"
QTM_END_NAMESPACE

