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

class QSignallingAccelerometerListener;

/*!
    \class QAccelerometerReading
    \ingroup sensors

    \preliminary
    \brief The QAccelerometerReading class represents one reading from the
           accelerometer.

    The accelerometer returns linear acceleration values along 3 axes.
    The scale of the values is meters per second squared. The axes are arranged as follows.

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
    the force of Earth's gravity as approximately -9.8 on the Z axis.
*/

/*!
    \fn QAccelerometerReading::QAccelerometerReading()
    \internal
*/

/*!
    \fn QAccelerometerReading::QAccelerometerReading(QDateTime timestamp, qreal x, qreal y, qreal z)
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
    \internal
*/
QAccelerometerListener::QAccelerometerListener()
{
}

/*!
    Notifies any sensors that the listener is being destroyed.
*/
QAccelerometerListener::~QAccelerometerListener()
{
    if (m_sensor)
        m_sensor->setListener(0);
}

/*!
    \fn QAccelerometerListener::accelerationChanged(const QAccelerometerReading &reading)

    This function is called when a new accelerometer \a reading is available.
*/

/*!
    \internal
*/
void QAccelerometerListener::setSensor(QAccelerometer *sensor)
{
    m_sensor = sensor;
}

/*!
    \fn QAccelerometerListener::sensor() const
    \internal
*/

// =====================================================================

class QSignallingAccelerometerListener : public QObject, public QAccelerometerListener
{
    Q_OBJECT
public:
    QSignallingAccelerometerListener()
    {
    }

    void setSensor(QAccelerometer *_sensor)
    {
        if (sensor())
            disconnect(this, SIGNAL(accelerationChanged(QAccelerometerReading)), sensor(), SIGNAL(accelerationChanged(QAccelerometerReading)));
        QAccelerometerListener::setSensor(_sensor);
        if (_sensor)
            connect(this, SIGNAL(accelerationChanged(QAccelerometerReading)), _sensor, SIGNAL(accelerationChanged(QAccelerometerReading)));
        else
            delete this; // remove us automatically
    }

signals:
    void accelerationChanged(const QAccelerometerReading &reading);
};

// =====================================================================

/*!
    \class QAccelerometer
    \ingroup sensors

    \preliminary
    \brief The QAccelerometer class reports on linear acceleration
           along the X, Y and Z axes.

    The accelerometer returns acceleration values along 3 axes.
    The scale of the values is meters per second squared. The axes are arranged as follows.

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
    the force of Earth's gravity as approximately -9.8 on the Z axis.

    \sa QAccelerometerReading
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a identifier is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QAccelerometer::QAccelerometer(QObject *parent, const QSensorId &identifier)
    : QSensor(parent)
    , m_listener(0)
{
    m_backend = static_cast<QAccelerometerBackend*>(connectToBackend(identifier));
    setListener(0);
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QAccelerometer::~QAccelerometer()
{
    stop();
    if (m_listener)
        m_listener->setSensor(0);
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
    Set the \a listener for the sensor. Pass 0 to clear a previously-set
    listener.
    The listener will be invoked every time a new reading is available.

    Note that the sensor does not take ownership of the listener although
    the listener interface will notify the accelerometer if it is destroyed.

    Also note that setting a listener will prevent the accelerationChanged()
    signal from being emitted.
*/
void QAccelerometer::setListener(QAccelerometerListener *listener)
{
    if (listener == m_listener)
        return;
    if (m_listener)
        m_listener->setSensor(0);
    if (listener)
        m_listener = listener;
    else
        m_listener = new QSignallingAccelerometerListener;
    m_listener->setSensor(this);
}

/*!
    \fn QAccelerometer::currentReading() const

    Returns the current ambient light reading.
*/

/*!
    \fn QAccelerometer::accelerationChanged(const QAccelerometerReading &reading)

    This signal may be emitted when a new accelerometer \a reading comes in.

    This signal is only emitted if no listener has been set. If a listener has been
    set via setListener() then this signal will not be emitted.

    Note that this signal should not be used if you are requesting
    high-frequency updates as signal delivery is quite slow.

    \sa setListener()
*/

/*!
    \fn QAccelerometer::backend() const
    \reimp
*/

#include "moc_qaccelerometer.cpp"
#include "qaccelerometer.moc"
QTM_END_NAMESPACE

