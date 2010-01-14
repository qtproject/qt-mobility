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
        : timestamp(), tapDirection(0), doubleTap(false) {}
    QTapReadingData(qtimestamp _timestamp, int _tapDirection, bool _doubleTap)
        : timestamp(_timestamp), tapDirection(_tapDirection), doubleTap(_doubleTap) {}
    QTapReadingData(const QTapReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), tapDirection(other.tapDirection), doubleTap(other.doubleTap) {}
    ~QTapReadingData() {}

    qtimestamp timestamp;
    int tapDirection;
    bool doubleTap;
};

// =====================================================================

class Q_SENSORS_EXPORT QTapReading
{
public:
    enum TapDirection {
        Undefined = 0,
        X     = 0x0001,
        Y     = 0x0002,
        Z     = 0x0004,
        X_Pos = 0x0011,
        Y_Pos = 0x0022,
        Z_Pos = 0x0044,
        X_Neg = 0x0101,
        Y_Neg = 0x0202,
        Z_Neg = 0x0404
    };

    explicit QTapReading()
    { d = new QTapReadingData; }
    explicit QTapReading(qtimestamp timestamp, TapDirection tapDirection, bool doubleTap)
    { d = new QTapReadingData(timestamp, tapDirection, doubleTap); }
    QTapReading(const QTapReading &other)
        : d(other.d) {}
    ~QTapReading() {}

    qtimestamp timestamp() const { return d->timestamp; }
    TapDirection tapDirection() const { return static_cast<TapDirection>(d->tapDirection); }
    bool isDoubleTap() const { return d->doubleTap; }

private:
    QSharedDataPointer<QTapReadingData> d;
};

typedef QTypedSensorBackend<QTapReading> QTapBackend;

// =====================================================================

class Q_SENSORS_EXPORT QTapSensor : public QSensor
{
    Q_OBJECT
public:
    explicit QTapSensor(QObject *parent = 0, const QSensorId &identifier = QSensorId());
    virtual ~QTapSensor();

    Q_PROPERTY(QTapReading currentReading READ currentReading)

    static const QString typeId;
    QString type() const { return typeId; };

    // For polling/checking the current (cached) value
    QTapReading currentReading() const { return m_backend->currentReading(); }

Q_SIGNALS:
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

