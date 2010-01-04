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

#ifndef QPROXIMITYSENSOR_H
#define QPROXIMITYSENSOR_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>

QTM_BEGIN_NAMESPACE

// implementation detail
class QProximityReadingData : public QSharedData
{
public:
    QProximityReadingData() {}
    QProximityReadingData(QTime _timestamp, int _proximity)
        : timestamp(_timestamp), proximity(_proximity) {}
    QProximityReadingData(const QProximityReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), proximity(other.proximity) {}
    ~QProximityReadingData() {}

    QTime timestamp;
    int proximity;
};

// =====================================================================

class Q_SENSORS_EXPORT QProximityReading
{
public:
    enum Proximity {
        Undefined = 0,
        Close,
        NotClose
    };

    explicit QProximityReading()
    { d = new QProximityReadingData; }
    explicit QProximityReading(QTime timestamp, Proximity proximity)
    { d = new QProximityReadingData(timestamp, proximity); }
    QProximityReading(const QProximityReading &other)
        : d(other.d) {}
    ~QProximityReading() {}

    QTime timestamp() const { return d->timestamp; }
    Proximity proximity() const { return static_cast<Proximity>(d->proximity); }

private:
    QSharedDataPointer<QProximityReadingData> d;
};

typedef QTypedSensorBackend<QProximityReading> QProximityBackend;

// =====================================================================

class Q_SENSORS_EXPORT QProximitySensor : public QSensor
{
    Q_OBJECT
public:
    explicit QProximitySensor(QObject *parent = 0, const QSensorId &id = QSensorId());

    static const QString typeId;
    QString type() const { return typeId; };

    // For polling/checking the current (cached) value
    QProximityReading currentReading() const { return m_backend->currentReading(); }

signals:
    void proximityChanged(const QProximityReading &reading);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable()
    {
        emit proximityChanged(currentReading());
    }

    QProximityBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

