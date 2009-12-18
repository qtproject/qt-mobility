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

#include <qaccelerationsensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QAccelerationReading
    \ingroup sensors

    \preliminary
    \brief The QAccelerationReading class represents one reading from the
           acceleration sensor.

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
*/

/*!
    \fn QAccelerationReading::QAccelerationReading()
    \internal
*/

/*!
    \fn QAccelerationReading::QAccelerationReading(QTime timestamp, int x, int y, int z)
    \internal
*/

/*!
    \fn QAccelerationReading::QAccelerationReading(const QAccelerationReading &other)
    \internal
*/

/*!
    \fn QAccelerationReading::~QAccelerationReading()
    \internal
*/

/*!
    \fn QAccelerationReading::timestamp() const

    Returns the time when the reading was made.
*/

/*!
    \fn QAccelerationReading::x() const

    Returns the acceleration for the X axis.
*/

/*!
    \fn QAccelerationReading::y() const

    Returns the acceleration for the Y axis.
*/

/*!
    \fn QAccelerationReading::z() const

    Returns the acceleration for the Z axis.
*/

// =====================================================================

/*!
    \class QAccelerationListener
    \ingroup sensors

    \preliminary
    \brief The QAccelerationListener class provides an efficient
           callback facility for asynchronous notifications of
           acceleration changes.

    Since the accelerometer is typically accessed very frequently it may
    not by suitable to use signals and slots. The QAccelerationListener
    interface provides a way for the sensor to notify your class that a
    new acceleration reading is available more efficiently.
*/

/*!
    \fn QAccelerationListener::accelerationChanged(const QAccelerationReading &reading)

    This function is called when a new acceleration \a reading is available.
*/

// =====================================================================

/*!
    \class QAccelerationSensor
    \ingroup sensors

    \preliminary
    \brief The QAccelerationSensor class reports on linear acceleration
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

    \sa QAccelerationReading
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a id is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QAccelerationSensor::QAccelerationSensor(QObject *parent, const QSensorId &id)
    : QSensor(parent)
{
    m_backend = static_cast<QAccelerationBackend*>(connectToBackend(id));
}

/*!
    \variable QAccelerationSensor::typeId
*/
const QString QAccelerationSensor::typeId("qt.Acceleration");

/*!
    \fn QAccelerationSensor::type() const
    \reimp
*/

/*!
    Add a \a listener to the sensor.
    The listener will be invoked every time a new reading is available.

    Note that the sensor does not take ownership of the listener.
    It is the caller's responsibility to ensure the listener remains valid
    until the sensor is destroyed or the listener is removed via
    QAccelerationSensor::removeListener().
*/
void QAccelerationSensor::addListener(QAccelerationListener *listener)
{
    m_listeners.append(listener);
}

/*!
    Remove a \a listener from the sensor.
    If \a listener is 0, all listeners will be removed.
*/
void QAccelerationSensor::removeListener(QAccelerationListener *listener)
{
    if (listener)
        m_listeners.removeOne(listener);
    else
        m_listeners.clear();
}

/*!
    \fn QAccelerationSensor::accelerationChanged(const QAccelerationReading &reading)

    This signal is emitted when a new acceleration \a reading comes in.

    Note that this signal should not be used if you are requesting
    high-frequency updates as signal delivery is quite slow.
*/

void QAccelerationSensor::newReadingAvailable()
{
    QAccelerationReading reading = currentReading();
    for (QList<QAccelerationListener*>::const_iterator iter = m_listeners.constBegin();
            iter != m_listeners.constEnd();
            ++iter) {
        (*iter)->accelerationChanged(reading);
    }
}

#include "moc_qaccelerationsensor.cpp"
QTM_END_NAMESPACE

