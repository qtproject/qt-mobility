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

#include <qmagnetometer.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QMagnetometerReading
    \ingroup sensors

    \preliminary
    \brief The QMagnetometerReading class represents one reading from the
           magnetometer.

    The magnetometer returns magnetic flux density values along 3 axes.
    The scale of the values is teslas. The axes are arranged as follows.

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
    \enum QMagnetometerReading::CalibrationLevel

    \value Undefined The calibration level is not defined or is too low.
    \value Low       The calibrated values may be inaccurate by up to 3 micro Teslas.
    \value Middle    The calibrated values may be inaccurate by up to 2 micro Teslas.
    \value High      The calibrated values may be inaccurate by up to 1 micro Tesla.
*/

/*!
    \fn QMagnetometerReading::QMagnetometerReading()
    \internal
*/

/*!
    \fn QMagnetometerReading::QMagnetometerReading(QDateTime timestamp, qreal x, qreal y, qreal z, qreal cx, qreal cy, qreal cz, CalibrationLevel calibration)
    \internal
*/

/*!
    \fn QMagnetometerReading::QMagnetometerReading(const QMagnetometerReading &o)
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

    Returns the magnetic flux density for the X axis.
*/

/*!
    \fn QMagnetometerReading::y() const

    Returns the magnetic flux density for the Y axis.
*/

/*!
    \fn QMagnetometerReading::z() const

    Returns the magnetic flux density for the Z axis.
*/

/*!
    \fn QMagnetometerReading::calibrated_x() const

    Returns the calibrated magnetic flux density for the Z axis.
*/

/*!
    \fn QMagnetometerReading::calibrated_y() const

    Returns the calibrated magnetic flux density for the Y axis.
*/

/*!
    \fn QMagnetometerReading::calibrated_z() const

    Returns the calibrated magnetic flux density for the Z axis.
*/

/*!
    \fn QMagnetometerReading::calibrationLevel() const

    Returns the calibration level of the reading. The higher the calibration, the more accurate the measurement is.
*/

// =====================================================================

/*!
    \class QMagnetometerListener
    \ingroup sensors

    \preliminary
    \brief The QMagnetometerListener class provides an efficient
           callback facility for asynchronous notifications of
           magnetic flux density changes.

    Since the magnetometer is typically accessed very frequently it may
    not by suitable to use signals and slots. The QMagnetometerListener
    interface provides a way for the sensor to notify your class that a
    new magnetometer reading is available more efficiently.
*/

/*!
    \internal
*/
QMagnetometerListener::QMagnetometerListener()
{
}

/*!
    Notifies any sensors that the listener is being destroyed.
*/
QMagnetometerListener::~QMagnetometerListener()
{
    if (m_sensor)
        m_sensor->setListener(0);
}

/*!
    \fn QMagnetometerListener::fluxDensityChanged(const QMagnetometerReading &reading)

    This function is called when a new magnetometer \a reading is available.
*/

/*!
    \internal
*/
void QMagnetometerListener::setSensor(QMagnetometer *sensor)
{
    m_sensor = sensor;
}

/*!
    \fn QMagnetometerListener::sensor() const
    \internal
*/

// =====================================================================

class QSignallingMagnetometerListener : public QObject, public QMagnetometerListener
{
    Q_OBJECT
public:
    QSignallingMagnetometerListener()
    {
    }

    void setSensor(QMagnetometer *_sensor)
    {
        if (sensor())
            disconnect(this, SIGNAL(fluxDensityChanged(QMagnetometerReading)), sensor(), SIGNAL(fluxDensityChanged(QMagnetometerReading)));
        QMagnetometerListener::setSensor(_sensor);
        if (_sensor)
            connect(this, SIGNAL(fluxDensityChanged(QMagnetometerReading)), _sensor, SIGNAL(fluxDensityChanged(QMagnetometerReading)));
        else
            delete this; // remove us automatically
    }

signals:
    void fluxDensityChanged(const QMagnetometerReading &reading);
};

// =====================================================================

/*!
    \class QMagnetometer
    \ingroup sensors

    \preliminary
    \brief The QMagnetometer class reports on magnetic flux density
           along the X, Y and Z axes.

    The magnetometer returns magnetic flux density values along 3 axes.
    The scale of the values is teslas. The axes are arranged as follows.

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

    \sa QMagnetometerReading
*/

/*!
    Construct a sensor instance with specified \a parent.
    If the \a identifier is passed the sensor will connect to that
    specific sensor, otherwise the default will be used.
*/
QMagnetometer::QMagnetometer(QObject *parent, const QSensorId &identifier)
    : QSensor(parent)
    , m_listener(0)
{
    m_backend = static_cast<QMagnetometerBackend*>(connectToBackend(identifier));
    setListener(0);
}

/*!
    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/
QMagnetometer::~QMagnetometer()
{
    stop();
    if (m_listener)
        m_listener->setSensor(0);
}

/*!
    \property QMagnetometer::currentReading
    \brief the current reading from the sensor.
*/

/*!
    \variable QMagnetometer::typeId
*/
const QString QMagnetometer::typeId("qt.Magnetometer");

/*!
    \fn QMagnetometer::type() const
    \reimp
*/

/*!
    Set the \a listener for the sensor. Pass 0 to clear a previously-set
    listener.
    The listener will be invoked every time a new reading is available.

    Note that the sensor does not take ownership of the listener although
    the listener interface will notify the magnetometer if it is destroyed.

    Also note that setting a listener will prevent the fluxDensityChanged()
    signal from being emitted.
*/
void QMagnetometer::setListener(QMagnetometerListener *listener)
{
    if (listener == m_listener)
        return;
    if (m_listener)
        m_listener->setSensor(0);
    if (listener)
        m_listener = listener;
    else
        m_listener = new QSignallingMagnetometerListener;
    m_listener->setSensor(this);
}

/*!
    \fn QMagnetometer::currentReading() const

    Returns the current ambient light reading.
*/

/*!
    \fn QMagnetometer::fluxDensityChanged(const QMagnetometerReading &reading)

    This signal may be emitted when a new magnetometer \a reading comes in.

    This signal is only emitted if no listener has been set. If a listener has been
    set via setListener() then this signal will not be emitted.

    Note that this signal should not be used if you are requesting
    high-frequency updates as signal delivery is quite slow.

    \sa setListener()
*/

/*!
    \fn QMagnetometer::backend() const
    \reimp
*/

#include "moc_qmagnetometer.cpp"
#include "qmagnetometer.moc"
QTM_END_NAMESPACE

