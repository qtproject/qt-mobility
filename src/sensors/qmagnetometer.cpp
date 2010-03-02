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

#include "qmagnetometer.h"
#include "qmagnetometer_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QMagnetometerReading)

/*!
    \class QMagnetometerReading
    \ingroup sensors_reading

    \preliminary
    \brief The QMagnetometerReading class represents one reading from the
           magnetometer.

    \section2 QMagnetometerReading Units
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
    \property QMagnetometerReading::x
    \brief the raw flux density on the X axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::x() const
{
    return d->x;
}

/*!
    Sets the raw flux density on the X axis to \a x.
*/
void QMagnetometerReading::setX(qreal x)
{
    d->x = x;
}

/*!
    \property QMagnetometerReading::y
    \brief the raw flux density on the Y axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::y() const
{
    return d->y;
}

/*!
    Sets the raw flux density on the Y axis to \a y.
*/
void QMagnetometerReading::setY(qreal y)
{
    d->y = y;
}

/*!
    \property QMagnetometerReading::z
    \brief the raw flux density on the Z axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::z() const
{
    return d->z;
}

/*!
    Sets the raw flux density on the Z axis to \a z.
*/
void QMagnetometerReading::setZ(qreal z)
{
    d->z = z;
}

/*!
    \property QMagnetometerReading::calibrated_x
    \brief the calibrated flux density on the X axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::calibrated_x() const
{
    return d->calibrated_x;
}

/*!
    Sets the calibrated flux density on the X axis to \a calibrated_x.
*/
void QMagnetometerReading::setCalibrated_x(qreal calibrated_x)
{
    d->calibrated_x = calibrated_x;
}

/*!
    \property QMagnetometerReading::calibrated_y
    \brief the calibrated flux density on the Y axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::calibrated_y() const
{
    return d->calibrated_y;
}

/*!
    Sets the calibrated flux density on the Y axis to \a calibrated_y.
*/
void QMagnetometerReading::setCalibrated_y(qreal calibrated_y)
{
    d->calibrated_y = calibrated_y;
}

/*!
    \property QMagnetometerReading::calibrated_z
    \brief the calibrated flux density on the Z axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::calibrated_z() const
{
    return d->calibrated_z;
}

/*!
    Sets the calibrated flux density on the Z axis to \a calibrated_z.
*/
void QMagnetometerReading::setCalibrated_z(qreal calibrated_z)
{
    d->calibrated_z = calibrated_z;
}

/*!
    \property QMagnetometerReading::calibrationLevel
    \brief the calibration level of the reading.

    The higher the calibration, the more accurate the measurement is.
    \sa {QMagnetometerReading Units}
*/

QMagnetometerReading::CalibrationLevel QMagnetometerReading::calibrationLevel() const
{
    return static_cast<QMagnetometerReading::CalibrationLevel>(d->calibrationLevel);
}

/*!
    Sets the calibration level of the reading to \a calibrationLevel.
*/
void QMagnetometerReading::setCalibrationLevel(QMagnetometerReading::CalibrationLevel calibrationLevel)
{
    d->calibrationLevel = calibrationLevel;
}

// =====================================================================

/*!
    \class QMagnetometerFilter
    \ingroup sensors_filter

    \preliminary
    \brief The QMagnetometerFilter class is a convenience wrapper around QSensorFilter.

    The only difference is that the filter() method features a pointer to QMagnetometerReading
    instead of QSensorReading.
*/

/*!
    \fn QMagnetometerFilter::filter(QMagnetometerReading *reading)

    Called when \a reading changes. Returns false to prevent the reading from propagating.

    \sa QSensorFilter::filter()
*/

const char *QMagnetometer::type("QMagnetometer");

/*!
    \class QMagnetometer
    \ingroup sensors_type

    \preliminary
    \brief The QMagnetometer class is a convenience wrapper around QSensor.

    The only behavioural difference is that this class sets the type properly.

    This class also features a reading() function that returns a QMagnetometerReading instead of a QSensorReading.

    For details about how the sensor works, see \l QMagnetometerReading.

    \sa QMagnetometerReading
*/

/*!
    \fn QMagnetometer::QMagnetometer(QObject *parent)

    Construct the sensor as a child of \a parent.
*/

/*!
    \fn QMagnetometer::~QMagnetometer()

    Destroy the sensor. Stops the sensor if it has not already been stopped.
*/

/*!
    \fn QMagnetometer::reading() const

    Returns the reading class for this sensor.

    \sa QSensor::reading()
*/

/*!
    \property QMagnetometer::returnRawValues
    \brief a value indicating if raw magnetometer values should be returned.

    Set to true to return raw magnetometer values.
    Set to false to suppress raw magnetometer values.

    Note that you must access this property via QObject::property() and QObject::setProperty().
    The property must be set before calling start().
*/

/*!
    \property QMagnetometer::returnCalibratedValues
    \brief a value indicating if calibrated magnetometer values should be returned.

    Set to true to return calibrated magnetometer values.
    Set to false to suppress calibrated magnetometer values.

    Note that you must access this property via QObject::property() and QObject::setProperty().
    The property must be set before calling start().
*/

#include "moc_qmagnetometer.cpp"
QTM_END_NAMESPACE

