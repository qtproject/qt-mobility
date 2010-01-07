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

#ifndef QROTATIONSENSOR_H
#define QROTATIONSENSOR_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>

QTM_BEGIN_NAMESPACE

// implementation detail
class QRotationReadingData : public QSharedData
{
public:
    QRotationReadingData() {}
    QRotationReadingData(QDateTime _timestamp, qreal _x, qreal _y, qreal _z)
        : timestamp(_timestamp), x(_x), y(_y), z(_z) {}
    QRotationReadingData(const QRotationReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), x(other.x), y(other.y), z(other.z) {}
    ~QRotationReadingData() {}

    QDateTime timestamp;
    qreal x;
    qreal y;
    qreal z;
};

// =====================================================================

class Q_SENSORS_EXPORT QRotationReading
{
public:
    explicit QRotationReading()
    { d = new QRotationReadingData; }
    explicit QRotationReading(QDateTime timestamp, qreal x, qreal y, qreal z)
    { d = new QRotationReadingData(timestamp, x, y, z); }
    QRotationReading(const QRotationReading &other)
        : d(other.d) {}
    ~QRotationReading() {}

    QDateTime timestamp() const { return d->timestamp; }
    qreal x() const { return d->x; }
    qreal y() const { return d->y; }
    qreal z() const { return d->z; }

private:
    QSharedDataPointer<QRotationReadingData> d;
};

typedef QTypedSensorBackend<QRotationReading> QRotationBackend;

// =====================================================================

class Q_SENSORS_EXPORT QRotationSensor : public QSensor
{
    Q_OBJECT
public:
    explicit QRotationSensor(QObject *parent = 0, const QSensorId &id = QSensorId());

    static const QString typeId;
    QString type() const { return typeId; };

    // For polling/checking the current (cached) value
    QRotationReading currentReading() const { return m_backend->currentReading(); }

signals:
    void rotationChanged(const QRotationReading &reading);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable()
    {
        emit rotationChanged(currentReading());
    }

    QRotationBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

