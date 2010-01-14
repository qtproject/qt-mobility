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

// implementation detail
class QMagnetometerReadingData : public QSharedData
{
public:
    QMagnetometerReadingData()
        : timestamp(), x(0), y(0), z(0), cx(0), cy(0), cz(0), calibration(0) {}
    QMagnetometerReadingData(qtimestamp _timestamp, qreal _x, qreal _y, qreal _z, qreal _cx, qreal _cy, qreal _cz, int _calibration)
        : timestamp(_timestamp), x(_x), y(_y), z(_z), cx(_cx), cy(_cy), cz(_cz), calibration(_calibration) {}
    QMagnetometerReadingData(const QMagnetometerReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), x(other.x), y(other.y), z(other.z), cx(other.cx), cy(other.cy), cz(other.cz), calibration(other.calibration) {}
    ~QMagnetometerReadingData() {}

    qtimestamp timestamp;
    qreal x;
    qreal y;
    qreal z;
    qreal cx;
    qreal cy;
    qreal cz;
    int calibration;
};

// =====================================================================

class Q_SENSORS_EXPORT QMagnetometerReading
{
public:
    enum CalibrationLevel {
        Undefined = 0,
        Low       = 1,
        Middle    = 2,
        High      = 3
    };

    explicit QMagnetometerReading()
    { d = new QMagnetometerReadingData; }
    explicit QMagnetometerReading(qtimestamp timestamp, qreal x, qreal y, qreal z, qreal cx, qreal cy, qreal cz, CalibrationLevel calibration)
    { d = new QMagnetometerReadingData(timestamp, x, y, z, cx, cy, cz, calibration); }
    QMagnetometerReading(const QMagnetometerReading &other)
        : d(other.d) {}
    ~QMagnetometerReading() {}

    qtimestamp timestamp() const { return d->timestamp; }
    qreal x() const { return d->x; }
    qreal y() const { return d->y; }
    qreal z() const { return d->z; }
    qreal calibrated_x() const { return d->cx; }
    qreal calibrated_y() const { return d->cy; }
    qreal calibrated_z() const { return d->cz; }
    CalibrationLevel calibrationLevel() const { return static_cast<CalibrationLevel>(d->calibration); }

private:
    QSharedDataPointer<QMagnetometerReadingData> d;
};

typedef QTypedSensorBackend<QMagnetometerReading> QMagnetometerBackend;

// =====================================================================

class QMagnetometer;

class Q_SENSORS_EXPORT QMagnetometerListener
{
    friend class QMagnetometer;
public:
    QMagnetometerListener();
    virtual ~QMagnetometerListener();
    virtual void fluxDensityChanged(const QMagnetometerReading &reading) = 0;
protected:
    void setSensor(QMagnetometer *sensor);
    QMagnetometer *sensor() const { return m_sensor; }
private:
    QMagnetometer *m_sensor;
};

// =====================================================================

class Q_SENSORS_EXPORT QMagnetometer : public QSensor
{
    Q_OBJECT
public:
    explicit QMagnetometer(QObject *parent = 0, const QSensorId &identifier = QSensorId());
    virtual ~QMagnetometer();

    Q_PROPERTY(QMagnetometerReading currentReading READ currentReading)

    static const QString typeId;
    QString type() const { return typeId; };

    // Register a listener (that will receive sensor values as they come in)
    void setListener(QMagnetometerListener *listener);

    // For polling/checking the current (cached) value
    QMagnetometerReading currentReading() const { return m_backend->currentReading(); }

Q_SIGNALS:
    void fluxDensityChanged(const QMagnetometerReading &reading);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable()
    {
        m_listener->fluxDensityChanged(currentReading());
    }

    QMagnetometerListener *m_listener;
    QMagnetometerBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

