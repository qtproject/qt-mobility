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

#include <qmagneticnorthsensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QMagneticNorthValue
    \ingroup sensors

    \preliminary
    \brief The QMagneticNorthValue class represents a magnetic compass reading.

    The magnetic north sensor returns the heading of the device. This is measured as
    degrees from magnetic north.
*/

/*!
    \internal
*/
QMagneticNorthValue::QMagneticNorthValue()
    : QSensorValue(QMagneticNorthSensor::typeId)
{
}

/*!
    \variable QMagneticNorthValue::heading

    Holds the heading of the device.
*/

// =====================================================================

/*!
    \class QMagneticNorthSensor
    \ingroup sensors

    \preliminary
    \brief The QMagneticNorthSensor class reports on the heading of the device.

    The magnetic north sensor returns the heading of the device. This is measured as
    degrees from magnetic north.
*/

/*!
    \variable QMagneticNorthSensor::typeId
*/
const QString QMagneticNorthSensor::typeId("qt.MagneticNorth");

/*!
    \fn QMagneticNorthSensor::type() const
    \reimp
*/

/*!
    Construct a sensor instance and attach to the sensor indicated by \a id.
    The sensor will be deleted when \a parent is deleted.
*/
QMagneticNorthSensor::QMagneticNorthSensor(const QSensorId &id, QObject *parent)
    : QSensor(parent)
{
    connectToBackend(id);
}

/*!
    \fn QMagneticNorthSensor::currentHeading() const

    Returns the current heading of the device.
*/

/*!
    Returns true if the device has been calibrated.
    If the device has not been calibrated the heading
    values may not be accurate.
*/
bool QMagneticNorthSensor::isCalibrated() const
{
    return false;
}

/*!
    \fn QMagneticNorthSensor::headingChanged(int heading)

    Emitted when the \a heading value changes.
*/

#include "moc_qmagneticnorthsensor.cpp"
QTM_END_NAMESPACE

