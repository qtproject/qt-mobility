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

#ifndef QACCELEROMETER_H
#define QACCELEROMETER_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>
#include <QList>

QTM_BEGIN_NAMESPACE

// implementation detail
class QAccelerometerReadingData : public QSharedData
{
public:
    QAccelerometerReadingData()
        : timestamp(), x(0), y(0), z(0) {}
    QAccelerometerReadingData(QDateTime _timestamp, int _x, int _y, int _z)
        : timestamp(_timestamp), x(_x), y(_y), z(_z) {}
    QAccelerometerReadingData(const QAccelerometerReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), x(other.x), y(other.y), z(other.z) {}
    ~QAccelerometerReadingData() {}

    QDateTime timestamp;
    int x;
    int y;
    int z;
};

// =====================================================================

class Q_SENSORS_EXPORT QAccelerometerReading
{
public:
    explicit QAccelerometerReading()
    { d = new QAccelerometerReadingData; }
    explicit QAccelerometerReading(QDateTime timestamp, int x, int y, int z)
    { d = new QAccelerometerReadingData(timestamp, x, y, z); }
    QAccelerometerReading(const QAccelerometerReading &other)
        : d(other.d) {}
    ~QAccelerometerReading() {}

    QDateTime timestamp() const { return d->timestamp; }
    int x() const { return d->x; }
    int y() const { return d->y; }
    int z() const { return d->z; }

private:
    QSharedDataPointer<QAccelerometerReadingData> d;
};

class Q_SENSORS_EXPORT QAccelerometerListener
{
public:
    virtual void accelerationChanged(const QAccelerometerReading &reading) = 0;
};

typedef QTypedSensorBackend<QAccelerometerReading> QAccelerometerBackend;

// =====================================================================

class Q_SENSORS_EXPORT QAccelerometer : public QSensor
{
    Q_OBJECT
public:
    explicit QAccelerometer(QObject *parent = 0, const QSensorId &identifier = QSensorId());
    virtual ~QAccelerometer();

    Q_PROPERTY(QAccelerometerReading currentReading READ currentReading)

    static const QString typeId;
    QString type() const { return typeId; };

    // Register a listener (that will receive sensor values as they come in)
    void addListener(QAccelerometerListener *listener);
    void removeListener(QAccelerometerListener *listener);

    // For polling/checking the current (cached) value
    QAccelerometerReading currentReading() const { return m_backend->currentReading(); }

Q_SIGNALS:
    void accelerationChanged(const QAccelerometerReading &reading);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable();

    QList<QAccelerometerListener*> m_listeners;
    QAccelerometerBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

