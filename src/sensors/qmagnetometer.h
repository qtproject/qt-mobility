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

#ifndef QMAGNETOMETER_H
#define QMAGNETOMETER_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>

QTM_BEGIN_NAMESPACE

class QMagnetometerReadingPrivate;

class Q_SENSORS_EXPORT QMagnetometerReading : public QSensorReading
{
    Q_OBJECT
    Q_ENUMS(CalibrationLevel)
    Q_PROPERTY(qreal x READ x)
    Q_PROPERTY(qreal y READ y)
    Q_PROPERTY(qreal z READ z)
    Q_PROPERTY(qreal calibrated_x READ calibrated_x)
    Q_PROPERTY(qreal calibrated_y READ calibrated_y)
    Q_PROPERTY(qreal calibrated_z READ calibrated_z)
    Q_PROPERTY(CalibrationLevel calibrationLevel READ calibrationLevel)
    DECLARE_READING(QMagnetometerReading)
public:
    enum CalibrationLevel {
        Undefined = 0,
        Low       = 1,
        Middle    = 2,
        High      = 3
    };

    qreal x() const;
    void setX(qreal x);

    qreal y() const;
    void setY(qreal y);

    qreal z() const;
    void setZ(qreal z);

    qreal calibrated_x() const;
    void setCalibrated_x(qreal calibrated_x);

    qreal calibrated_y() const;
    void setCalibrated_y(qreal calibrated_y);

    qreal calibrated_z() const;
    void setCalibrated_z(qreal calibrated_z);

    CalibrationLevel calibrationLevel() const;
    void setCalibrationLevel(CalibrationLevel calibrationLevel);
};

// begin generated code

class Q_SENSORS_EXPORT QMagnetometerFilter : public QSensorFilter
{
public:
    virtual bool filter(QMagnetometerReading *reading) = 0;
private:
    bool filter(QSensorReading *reading) { return filter(static_cast<QMagnetometerReading*>(reading)); }
};

class Q_SENSORS_EXPORT QMagnetometer : public QSensor
{
    Q_OBJECT
public:
    explicit QMagnetometer(QObject *parent = 0) : QSensor(parent)
    { setType("QMagnetometer"); }
    virtual ~QMagnetometer() {}
    QMagnetometerReading *reading() const { return static_cast<QMagnetometerReading*>(QSensor::reading()); }
};
// end generated code

QTM_END_NAMESPACE

#endif

