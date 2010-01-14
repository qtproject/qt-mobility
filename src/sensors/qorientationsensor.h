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

#ifndef QORIENTATIONSENSOR_H
#define QORIENTATIONSENSOR_H

#include <qsensor.h>
#include <QtGlobal>
#include <QSharedData>
#include <qsensorbackend.h>

QTM_BEGIN_NAMESPACE

// implementation detail
class QOrientationReadingData : public QSharedData
{
public:
    QOrientationReadingData() {}
    QOrientationReadingData(qtimestamp _timestamp, int _orientation)
        : timestamp(_timestamp), orientation(_orientation) {}
    QOrientationReadingData(const QOrientationReadingData &other)
        : QSharedData(other), timestamp(other.timestamp), orientation(other.orientation) {}
    ~QOrientationReadingData() {}

    qtimestamp timestamp;
    int orientation;
};

// =====================================================================

class Q_SENSORS_EXPORT QOrientationReading
{
public:
    enum Orientation {
        Undefined = 0,
        BottomUp,
        BottomDown,
        LeftUp,
        RightUp,
        FaceDown,
        FaceUp
    };

    explicit QOrientationReading()
    { d = new QOrientationReadingData; }
    explicit QOrientationReading(qtimestamp timestamp, Orientation orientation)
    { d = new QOrientationReadingData(timestamp, orientation); }
    QOrientationReading(const QOrientationReading &other)
        : d(other.d) {}
    ~QOrientationReading() {}

    qtimestamp timestamp() const { return d->timestamp; }
    Orientation orientation() const { return static_cast<Orientation>(d->orientation); }

private:
    QSharedDataPointer<QOrientationReadingData> d;
};

typedef QTypedSensorBackend<QOrientationReading> QOrientationBackend;

// =====================================================================

class Q_SENSORS_EXPORT QOrientationSensor : public QSensor
{
    Q_OBJECT
public:
    explicit QOrientationSensor(QObject *parent = 0, const QSensorId &identifier = QSensorId());
    virtual ~QOrientationSensor();

    Q_PROPERTY(QOrientationReading currentReading READ currentReading)

    static const QString typeId;
    QString type() const { return typeId; };

    // For polling/checking the current (cached) value
    QOrientationReading currentReading() const { return m_backend->currentReading(); }

Q_SIGNALS:
    void orientationChanged(const QOrientationReading &reading);

protected:
    QSensorBackend *backend() const { return m_backend; }

private:
    void newReadingAvailable()
    {
        emit orientationChanged(currentReading());
    }

    QOrientationBackend *m_backend;
};

QTM_END_NAMESPACE

#endif

