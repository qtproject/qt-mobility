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

#ifndef QMAGNETICNORTHSENSOR_H
#define QMAGNETICNORTHSENSOR_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>

QTM_BEGIN_NAMESPACE

// implementation detail
class QMagneticNorthReadingData : public QSharedData
{
public:
    QMagneticNorthReadingData()
        : timestamp(), heading(0), calibrated(false) {}
    QMagneticNorthReadingData(QDateTime _timestamp, int _heading, bool _calibrated)
        : timestamp(_timestamp), heading(_heading), calibrated(_calibrated) {}
    QMagneticNorthReadingData(const QMagneticNorthReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), heading(other.heading), calibrated(other.calibrated) {}
    ~QMagneticNorthReadingData() {}

    QDateTime timestamp;
    int heading;
    bool calibrated;
};

// =====================================================================

class Q_SENSORS_EXPORT QMagneticNorthReading
{
public:
    explicit QMagneticNorthReading()
    { d = new QMagneticNorthReadingData; }
    explicit QMagneticNorthReading(QDateTime timestamp, int heading, bool calibrated)
    { d = new QMagneticNorthReadingData(timestamp, heading, calibrated); }
    QMagneticNorthReading(const QMagneticNorthReading &other)
        : d(other.d) {}
    ~QMagneticNorthReading() {}

    QDateTime timestamp() const { return d->timestamp; }
    int heading() const { return d->heading; }
    bool isCalibrated() const { return d->calibrated; }

private:
    QSharedDataPointer<QMagneticNorthReadingData> d;
};

typedef QTypedSensorBackend<QMagneticNorthReading> QMagneticNorthBackend;

// =====================================================================

class Q_SENSORS_EXPORT QMagneticNorthSensor : public QSensor
{
    Q_OBJECT
public:
    explicit QMagneticNorthSensor(QObject *parent = 0, const QSensorId &id = QSensorId());
    virtual ~QMagneticNorthSensor();

    Q_PROPERTY(QMagneticNorthReading currentReading READ currentReading)

    static const QString typeId;
    QString type() const { return typeId; };

    // For polling/checking the current (cached) value
    QMagneticNorthReading currentReading() const { return m_backend->currentReading(); }

Q_SIGNALS:
    void headingChanged(const QMagneticNorthReading &reading);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable()
    {
        emit headingChanged(currentReading());
    }

    QMagneticNorthBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

