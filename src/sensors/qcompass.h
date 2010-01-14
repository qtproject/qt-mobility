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

#ifndef QCOMPASS_H
#define QCOMPASS_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>

QTM_BEGIN_NAMESPACE

// implementation detail
class QCompassReadingData : public QSharedData
{
public:
    QCompassReadingData()
        : timestamp(), azimuth(0), calibration(0) {}
    QCompassReadingData(qtimestamp _timestamp, qreal _azimuth, int _calibration)
        : timestamp(_timestamp), azimuth(_azimuth), calibration(_calibration) {}
    QCompassReadingData(const QCompassReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), azimuth(other.azimuth), calibration(other.calibration) {}
    ~QCompassReadingData() {}

    qtimestamp timestamp;
    qreal azimuth;
    int calibration;
};

// =====================================================================

class Q_SENSORS_EXPORT QCompassReading
{
public:
    enum CalibrationLevel {
        Undefined = 0,
        Low       = 1,
        Middle    = 2,
        High      = 3
    };

    explicit QCompassReading()
    { d = new QCompassReadingData; }
    explicit QCompassReading(qtimestamp timestamp, qreal azimuth, CalibrationLevel calibration)
    { d = new QCompassReadingData(timestamp, azimuth, calibration); }
    QCompassReading(const QCompassReading &other)
        : d(other.d) {}
    ~QCompassReading() {}

    qtimestamp timestamp() const { return d->timestamp; }
    qreal azimuth() const { return d->azimuth; }
    CalibrationLevel calibrationLevel() const { return static_cast<CalibrationLevel>(d->calibration); }

private:
    QSharedDataPointer<QCompassReadingData> d;
};

typedef QTypedSensorBackend<QCompassReading> QCompassBackend;

// =====================================================================

class Q_SENSORS_EXPORT QCompass : public QSensor
{
    Q_OBJECT
public:
    explicit QCompass(QObject *parent = 0, const QSensorId &identifier = QSensorId());
    virtual ~QCompass();

    Q_PROPERTY(QCompassReading currentReading READ currentReading)

    static const QString typeId;
    QString type() const { return typeId; };

    // For polling/checking the current (cached) value
    QCompassReading currentReading() const { return m_backend->currentReading(); }

Q_SIGNALS:
    void azimuthChanged(const QCompassReading &reading);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable()
    {
        emit azimuthChanged(currentReading());
    }

    QCompassBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

