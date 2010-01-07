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

#ifndef QACCELERATIONSENSOR_H
#define QACCELERATIONSENSOR_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>
#include <QList>

QTM_BEGIN_NAMESPACE

// implementation detail
class QAccelerationReadingData : public QSharedData
{
public:
    QAccelerationReadingData()
        : timestamp(), x(0), y(0), z(0) {}
    QAccelerationReadingData(QDateTime _timestamp, int _x, int _y, int _z)
        : timestamp(_timestamp), x(_x), y(_y), z(_z) {}
    QAccelerationReadingData(const QAccelerationReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), x(other.x), y(other.y), z(other.z) {}
    ~QAccelerationReadingData() {}

    QDateTime timestamp;
    int x;
    int y;
    int z;
};

// =====================================================================

class Q_SENSORS_EXPORT QAccelerationReading
{
public:
    explicit QAccelerationReading()
    { d = new QAccelerationReadingData; }
    explicit QAccelerationReading(QDateTime timestamp, int x, int y, int z)
    { d = new QAccelerationReadingData(timestamp, x, y, z); }
    QAccelerationReading(const QAccelerationReading &other)
        : d(other.d) {}
    ~QAccelerationReading() {}

    QDateTime timestamp() const { return d->timestamp; }
    int x() const { return d->x; }
    int y() const { return d->y; }
    int z() const { return d->z; }

private:
    QSharedDataPointer<QAccelerationReadingData> d;
};

class Q_SENSORS_EXPORT QAccelerationListener
{
public:
    virtual void accelerationChanged(const QAccelerationReading &reading) = 0;
};

typedef QTypedSensorBackend<QAccelerationReading> QAccelerationBackend;

// =====================================================================

class Q_SENSORS_EXPORT QAccelerationSensor : public QSensor
{
    Q_OBJECT
public:
    explicit QAccelerationSensor(QObject *parent = 0, const QSensorId &id = QSensorId());
    virtual ~QAccelerationSensor();

    Q_PROPERTY(QAccelerationReading currentReading READ currentReading)

    static const QString typeId;
    QString type() const { return typeId; };

    // Register a listener (that will receive sensor values as they come in)
    void addListener(QAccelerationListener *listener);
    void removeListener(QAccelerationListener *listener);

    // For polling/checking the current (cached) value
    QAccelerationReading currentReading() const { return m_backend->currentReading(); }

Q_SIGNALS:
    void accelerationChanged(const QAccelerationReading &reading);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable();

    QList<QAccelerationListener*> m_listeners;
    QAccelerationBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

