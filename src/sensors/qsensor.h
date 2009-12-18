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
#include <QString>
#include <QTime>

QTM_BEGIN_NAMESPACE

class QSensorBackend;

typedef QByteArray QSensorId;

class Q_SENSORS_EXPORT QSensor : public QObject
{
    Q_OBJECT
public:
    explicit QSensor(QObject *parent = 0);
    virtual ~QSensor();

    // May have been initialized with an invalid id
    bool isValid() const;

    QSensorId id() const;

    virtual QString type() const = 0;

    enum UpdatePolicy {
        Undefined         = 0x00, // If the sensor has no specific policy

        // These use pre-determined timing intervals, as set by the sensor
        OccasionalUpdates = 0x01, // When the system feels like it
        InfrequentUpdates = 0x02, // Every now and then
        FrequentUpdates   = 0x04, // Often (eg. for gaming controls)

        // For more control
        TimedUpdates      = 0x08, // Every x milliseconds (may not be supported by all sensors)
        PolledUpdates     = 0x10  // As often as polled (may not be supported by all sensors)
    };
    Q_DECLARE_FLAGS(UpdatePolicies, UpdatePolicy)

    // Set the desired update policy (default is defined by the sensor)
    // Use documentation to determine the policies that the sensor
    // supports.
    void setUpdatePolicy(UpdatePolicy policy, int interval = 0);

    // Retrieve the policy
    UpdatePolicy updatePolicy() const;
    int updateInterval() const;

    // What policies does the sensor support
    UpdatePolicies supportedPolicies() const;

    // A new reading is available
    virtual void newReadingAvailable() = 0;

public slots:
    // Start receiving values from the sensor
    bool start();

    // Stop receiving values from the sensor
    void stop();

protected:
    QSensorBackend *connectToBackend(const QSensorId &id);

    virtual QSensorBackend *backend() const = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSensor::UpdatePolicies);

QTM_END_NAMESPACE

#endif

