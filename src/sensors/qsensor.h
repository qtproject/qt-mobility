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

QTM_BEGIN_NAMESPACE

class QSensorListener;
class QSensorFilter;
class QSensorEvent;

class Q_SENSORS_EXPORT QSensor : public QObject
{
public:
    // Construct a sensor
    explicit QSensor(const QString &id, QObject *parent = 0);
    virtual ~QSensor();

    // Was a valid ID used to construct the sensor?
    bool isValid() const;

    QString id() const;
    QString name() const;

    // Types of sensors that the API supports
    enum Type {
        Orientation,
        Rotation,
        AngularAcceleration,
        Acceleration,
        DoubleTap,
        Proximity,
        MagneticNorth,
        Magnetometer,
        AmbientLight,

        // Non-standard sensor types
        UserSensor = 128
    };
    Type type() const;

    enum UpdatePolicy {
        Unknown           = 0x00, // If the sensor has no specific policy

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

    // Register a listener (that will receive sensor events as they come in)
    void addListener(QSensorListener *listener);
    void removeListener(QSensorListener *listener);

    // Add a filter to remove or modify the accleration values
    void addFilter(QSensorFilter *filter);
    void removeFilter(QSensorFilter *filter);

    // Start receiving values from the sensor
    bool start();

    // Stop receiving values from the sensor
    void stop();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSensor::UpdatePolicies);

class QSensorListener
{
public:
    virtual void sensorEvent(QSensorEvent *event) = 0;
};

// As above but uses a specific event type
template <typename T>
class QSpecificSensorListener : public QSensorListener
{
public:
    virtual void sensorEvent(T *event) = 0;
    void sensorEvent(QSensorEvent *event)
    {
        sensorEvent(static_cast<T*>(event));
    }
};

typedef quint64 QSensorTimestamp;

class Q_SENSORS_EXPORT QSensorEvent
{
public:
    explicit QSensorEvent(QSensor *sensor);

    QSensor *sensor() const;
    QSensorTimestamp timestamp;
};

class QSensorFilter
{
public:
    virtual bool filter(QSensorEvent *event) = 0;
};

// As above but uses a specific event type
template <typename T>
class QSpecificSensorFilter : public QSensorFilter
{
public:
    virtual bool filter(T *event) = 0;
    bool filter(QSensorEvent *event)
    {
        return filter(static_cast<T*>(event));
    }
};

QTM_END_NAMESPACE

#endif

