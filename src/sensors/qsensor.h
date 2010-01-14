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

#ifndef QSENSOR_H
#define QSENSOR_H

#include <qmobilityglobal.h>
#include <QObject>
#include <QByteArray>

QTM_BEGIN_NAMESPACE

class QSensorBackend;

typedef quint64 qtimestamp;

class Q_SENSORS_EXPORT QSensor : public QObject
{
    friend class QSensorBackend;

    Q_OBJECT
public:
    explicit QSensor(QObject *parent = 0);
    virtual ~QSensor();

    Q_PROPERTY(QByteArray identifier READ identifier)
    Q_PROPERTY(QByteArray type READ type)
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(UpdatePolicies supportedUpdatePolicies READ supportedUpdatePolicies)
    Q_PROPERTY(UpdatePolicy updatePolicy READ updatePolicy WRITE setUpdatePolicy)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)

    // May have been initialized with an invalid identifier
    bool isValid() const;

    QByteArray identifier() const;

    virtual QByteArray type() const = 0;

    enum UpdatePolicy {
        Undefined         = 0x01, // If the sensor has no specific policy

        // These use pre-determined timing intervals, as set by the sensor
        OccasionalUpdates = 0x02,
        InfrequentUpdates = 0x04,
        FrequentUpdates   = 0x08,

        // For more control
        TimedUpdates      = 0x10, // Every x milliseconds (may not be supported by all sensors)
        PolledUpdates     = 0x11  // As often as polled (may not be supported by all sensors)
    };
    Q_DECLARE_FLAGS(UpdatePolicies, UpdatePolicy)

    // Set the desired update policy (default is defined by the sensor)
    // Use documentation to determine the policies that the sensor
    // supports.
    void setUpdatePolicy(UpdatePolicy policy);
    void setUpdateInterval(int interval);

    // Retrieve the policy
    UpdatePolicy updatePolicy() const;
    int updateInterval() const;

    // What policies does the sensor support
    UpdatePolicies supportedUpdatePolicies() const;

public slots:
    // Start receiving values from the sensor
    bool start();

    // Stop receiving values from the sensor
    void stop();

protected:
    QSensorBackend *connectToBackend(const QByteArray &identifier);

    virtual QSensorBackend *backend() const = 0;

    // A new reading is available
    virtual void newReadingAvailable() = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSensor::UpdatePolicies);

QTM_END_NAMESPACE

#endif

