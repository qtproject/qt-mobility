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

#ifndef QTAPSENSOR_H
#define QTAPSENSOR_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>

QTM_BEGIN_NAMESPACE

// implementation detail
class QTapReadingData : public QSharedData
{
public:
    QTapReadingData()
        : timestamp(), tapDirection(0), tapCount(0) {}
    QTapReadingData(QTime _timestamp, int _tapDirection, int _tapCount)
        : timestamp(_timestamp), tapDirection(_tapDirection), tapCount(_tapCount) {}
    QTapReadingData(const QTapReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), tapDirection(other.tapDirection), tapCount(other.tapCount) {}
    ~QTapReadingData() {}

    QTime timestamp;
    int tapDirection;
    int tapCount;
};

// =====================================================================

class Q_SENSORS_EXPORT QTapReading
{
public:
    enum TapDirection {
        X,
        Y,
        Z
    };

    explicit QTapReading()
    { d = new QTapReadingData; }
    explicit QTapReading(QTime timestamp, TapDirection tapDirection, int tapCount)
    { d = new QTapReadingData(timestamp, tapDirection, tapCount); }
    QTapReading(const QTapReading &other)
        : d(other.d) {}
    ~QTapReading() {}

    QTime timestamp() const { return d->timestamp; }
    TapDirection tapDirection() const { return static_cast<TapDirection>(d->tapDirection); }
    int tapCount() const { return d->tapCount; }

private:
    QSharedDataPointer<QTapReadingData> d;
};

typedef QTypedSensorBackend<QTapReading> QTapBackend;

// =====================================================================

class Q_SENSORS_EXPORT QTapSensor : public QSensor
{
    Q_OBJECT
public:
    explicit QTapSensor(QObject *parent = 0, const QSensorId &id = QSensorId());

    static const QString typeId;
    QString type() const { return typeId; };

    // For polling/checking the current (cached) value
    QTapReading currentReading() const { return m_backend->currentReading(); }

signals:
    void tapDetected(const QTapReading &tap);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable()
    {
        emit tapDetected(currentReading());
    }

    QTapBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

