/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

    The magnetometer can report on either raw magnetic flux values or geomagnetic flux values.
    By default it returns raw magnetic flux values. The QMagnetometer::returnGeoValues property
    must be set to return geomagnetic flux values.

    The primary difference between raw and geomagnetic values is that extra processing
    is done to eliminate local magnetic interference from the geomagnetic values so they
    represent only the effect of the Earth's magnetic field. This process is not perfect
    and the accuracy of each reading may change.

    The accuracy of each reading is measured as a number from 0 to 1.
    A value of 1 is the highest level that the device can support and 0 is
    the worst.
*/

/*!
    \property QMagnetometerReading::x
    \brief the raw magnetic flux density on the X axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::x() const
{
    return d->x;
}

/*!
    Sets the raw magnetic flux density on the X axis to \a x.
*/
void QMagnetometerReading::setX(qreal x)
{
    d->x = x;
}

/*!
    \property QMagnetometerReading::y
    \brief the raw magnetic flux density on the Y axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::y() const
{
    return d->y;
}

/*!
    Sets the raw magnetic flux density on the Y axis to \a y.
*/
void QMagnetometerReading::setY(qreal y)
{
    d->y = y;
}

/*!
    \property QMagnetometerReading::z
    \brief the raw magnetic flux density on the Z axis.

    Measured as telsas.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::z() const
{
    return d->z;
}

/*!
    Sets the raw magnetic flux density on the Z axis to \a z.
*/
void QMagnetometerReading::setZ(qreal z)
{
    d->z = z;
}

/*!
    \property QMagnetometerReading::calibrationLevel
    \brief the accuracy of the reading.

    Measured as a value from 0 to 1 with higher values being better.

    Note that this only changes when measuring geomagnetic flux density.
    Raw magnetic flux readings will always have a value of 1.
    \sa {QMagnetometerReading Units}
*/

qreal QMagnetometerReading::calibrationLevel() const
{
    return d->calibrationLevel;
}

/*!
    Sets the accuracy of the reading to \a calibrationLevel.
*/
void QMagnetometerReading::setCalibrationLevel(qreal calibrationLevel)
{
    d->calibrationLevel = calibrationLevel;
}

// =====================================================================

/*!
    \class QMagnetometerFilter
    \ingroup sensors_filter

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
    \property QMagnetometer::returnGeoValues
    \brief a value indicating if geomagnetic values should be returned.

    Set to true to return geomagnetic flux density.
    Set to false (the default) to return raw magnetic flux density.

    Note that you must access this property via QObject::property() and QObject::setProperty().
    The property must be set before calling start().
*/

#include "moc_qmagnetometer.cpp"
QTM_END_NAMESPACE

