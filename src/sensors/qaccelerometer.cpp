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

#include <qaccelerometer.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QAccelerometerReading
    \ingroup sensors

    \preliminary
    \brief The QAccelerometerReading class represents one reading from the
           accelerometer.

    The accelerometer returns linear acceleration values along 3 axes.
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
*/

/*!
    \fn QAccelerometerReading::QAccelerometerReading()
    \internal
*/

/*!
    \fn QAccelerometerReading::QAccelerometerReading(QDateTime timestamp, int x, int y, int z)
    \internal
*/

/*!
    \fn QAccelerometerReading::QAccelerometerReading(const QAccelerometerReading &other)
    \internal
*/

/*!
    \fn QAccelerometerReading::~QAccelerometerReading()
    \internal
*/

/*!
    \fn QAccelerometerReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QAccelerometerReading::x() const

    Returns the acceleration for the X axis.
*/

/*!
    \fn QAccelerometerReading::y() const

    Returns the acceleration for the Y axis.
*/

/*!
    \fn QAccelerometerReading::z() const

    Returns the acceleration for the Z axis.
*/

// =====================================================================

/*!
    \class QAccelerometerListener
    \ingroup sensors

    \preliminary
    \brief The QAccelerometerListener class provides an efficient
           callback facility for asynchronous notifications of
           acceleration changes.

    Since the accelerometer is typically accessed very frequently it may
    not by suitable to use signals and slots. The QAccelerometerListener
    interface provides a way for the sensor to notify your class that a
    new accelerometer reading is available more efficiently.
*/

/*!
    \fn QAccelerometerListener::accelerationChanged(const QAccelerometerReading &reading)

    This function is called when a new accelerometer \a reading is available.
*/

// =====================================================================

/*!
    \class QAccelerometer
    \ingroup sensors

    \preliminary
    \brief The QAccelerometer class reports on linear acceleration
           along the X, Y and Z axes.

    The accelerometer returns acceleration values along 3 axes.
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

    \sa QAccelerometerReading
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a identifier is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QAccelerometer::QAccelerometer(QObject *parent, const QSensorId &identifier)
    : QSensor(parent)
{
    m_backend = static_cast<QAccelerometerBackend*>(connectToBackend(identifier));
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QAccelerometer::~QAccelerometer()
{
    stop();
}

/*!
    \property QAccelerometer::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QAccelerometer::typeId
*/
const QString QAccelerometer::typeId("qt.Accelerometer");

/*!
    \fn QAccelerometer::type() const
    \reimp
*/

/*!
    Add a \a listener to the sensor.
    The listener will be invoked every time a new reading is available.

    Note that the sensor does not take ownership of the listener.
    It is the caller's responsibility to ensure the listener remains valid
    until the sensor is destroyed or the listener is removed via
    QAccelerometer::removeListener().
*/
void QAccelerometer::addListener(QAccelerometerListener *listener)
{
    m_listeners.append(listener);
}

/*!
    Remove a \a listener from the sensor.
    If \a listener is 0, all listeners will be removed.
*/
void QAccelerometer::removeListener(QAccelerometerListener *listener)
{
    if (listener)
        m_listeners.removeOne(listener);
    else
        m_listeners.clear();
}

/*!
    \fn QAccelerometer::currentReading() const

    Returns the current ambient light reading.
*/

/*!
    \fn QAccelerometer::accelerationChanged(const QAccelerometerReading &reading)

    This signal is emitted when a new accelerometer \a reading comes in.

    Note that this signal should not be used if you are requesting
    high-frequency updates as signal delivery is quite slow.
*/

/*!
    \fn QAccelerometer::backend() const
    \reimp
*/

void QAccelerometer::newReadingAvailable()
{
    QAccelerometerReading reading = currentReading();
    for (QList<QAccelerometerListener*>::const_iterator iter = m_listeners.constBegin();
            iter != m_listeners.constEnd();
            ++iter) {
        (*iter)->accelerationChanged(reading);
    }
}

#include "moc_qaccelerometer.cpp"
QTM_END_NAMESPACE

