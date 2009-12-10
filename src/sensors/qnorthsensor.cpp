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

#include <qnorthsensor.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QNorthValue
    \ingroup sensors

    \preliminary
    \brief The QNorthValue class represents a magnetic compass reading.

    The north sensor returns the offset from magnetic north. There is
    also a boolean indicator of the calibration status of the sensor.
    If the sensor is not calibrated the offset values may not be accurate.
*/

/*!
    \internal
*/
QNorthValue::QNorthValue()
    : QSensorValue(QNorthSensor::typeId)
{
}

/*!
    \variable QNorthValue::offset

    Holds the offset from magnetic north.
*/

/*!
    \variable QNorthValue::calibrated

    Holds the calibration status of the sensor.
*/

// =====================================================================

/*!
    \class QNorthSensor
    \ingroup sensors

    \preliminary
    \brief The QNorthSensor class reports on the offset from magnetic north.

    The north sensor returns the offset from magnetic north. The offset
    is measured in degrees.
*/

/*!
    \variable QNorthSensor::typeId
*/
const QString QNorthSensor::typeId("qt.North");

/*!
    \fn QNorthSensor::type() const
    \reimp
*/

/*!
    Construct a sensor instance and attach to the sensor indicated by \a id.
    The sensor will be deleted when \a parent is deleted.
*/
QNorthSensor::QNorthSensor(const QSensorId &id, QObject *parent)
    : QSensor(parent)
{
    connectToBackend(id);
}

/*!
    \fn QNorthSensor::currentOffset() const

    Returns the current offset from magnetic north.
*/

/*!
    \fn QNorthSensor::currentCalibration() const

    Returns the current calibration status.
*/

/*!
    \fn QNorthSensor::offsetChanged(int offset, bool calibrated)

    Emitted when the \a offset or \a calibrated value changes.
*/

#include "moc_qnorthsensor.cpp"
QTM_END_NAMESPACE

