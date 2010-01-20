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
#include <QMetaType>

QTM_BEGIN_NAMESPACE

class QSensorPrivate;
class QSensorBackend;
class QSensorReading;
class QSensorReadingPrivate;
class QSensorFilter;

typedef quint64 qtimestamp;

class Q_SENSORS_EXPORT QSensor : public QObject
{
    friend class QSensorBackend;

    Q_OBJECT
public:
    explicit QSensor(QObject *parent = 0);
    virtual ~QSensor();

    Q_PROPERTY(QByteArray identifier READ identifier WRITE setIdentifier)
    QByteArray identifier() const;
    void setIdentifier(const QByteArray &identifier);

    Q_PROPERTY(QByteArray type READ type WRITE setType)
    QByteArray type() const;
    void setType(const QByteArray &type);

    Q_INVOKABLE void connect();
    Q_PROPERTY(bool isAvailable READ isAvailable)
    bool isAvailable() const;

    Q_PROPERTY(bool active READ isActive)
    bool isActive() const;

    Q_PROPERTY(bool signalEnabled READ isSignalEnabled WRITE setSignalEnabled)
    bool isSignalEnabled() const;
    void setSignalEnabled(bool enabled);

    enum UpdatePolicy {
        Undefined         = 0x00, // If the sensor has no specific policy

        // Used by irregularly updating sensors
        OnChangeUpdates   = 0x01,

        // Used by continuously updating sensors
        OccasionalUpdates = 0x02,
        InfrequentUpdates = 0x04,
        FrequentUpdates   = 0x08,

        // For more control
        TimedUpdates      = 0x10, // Every x milliseconds (may not be supported by all sensors)
        PolledUpdates     = 0x20  // As often as polled (may not be supported by all sensors)
    };
    Q_ENUMS(UpdatePolicy)
    Q_FLAGS(UpdatePolicies)
    Q_DECLARE_FLAGS(UpdatePolicies, UpdatePolicy)

    // What policies does the sensor support
    Q_PROPERTY(UpdatePolicies supportedUpdatePolicies READ supportedUpdatePolicies)
    UpdatePolicies supportedUpdatePolicies() const;

    Q_PROPERTY(UpdatePolicy updatePolicy READ updatePolicy WRITE setUpdatePolicy)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)

    // Set the desired update policy (default is defined by the sensor)
    // Use documentation to determine the policies that the sensor
    // supports.
    void setUpdatePolicy(UpdatePolicy policy);
    void setUpdateInterval(int interval);

    // Retrieve the policy
    UpdatePolicy updatePolicy() const;
    int updateInterval() const;

    // Filters modify the reading
    void addFilter(QSensorFilter *filter);
    void removeFilter(QSensorFilter *filter);

    // Poll for sensor change (only if using PolledUpdates)
    void poll();

    // The readings are exposed via this object
    Q_PROPERTY(QSensorReading* reading READ reading)
    QSensorReading *reading() const;

public slots:
    // Start receiving values from the sensor
    void start();

    // Stop receiving values from the sensor
    void stop();

Q_SIGNALS:
    void readingChanged(QSensorReading *reading);

protected:
    // called by the back end
    void newReadingAvailable();
    QSensorPrivate *d_func() const { return d.data(); }

private:
    QScopedPointer<QSensorPrivate> d;
    Q_DISABLE_COPY(QSensor)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSensor::UpdatePolicies);

class Q_SENSORS_EXPORT QSensorFilter
{
    friend class QSensor;
public:
    QSensorFilter();
    virtual ~QSensorFilter();
    virtual bool filter(QSensorReading *reading) = 0;
protected:
    virtual void setSensor(QSensor *sensor);
    QSensor *m_sensor;
};

class Q_SENSORS_EXPORT QSensorReading : public QObject
{
    friend class QSensor;

    Q_OBJECT
public:
    virtual ~QSensorReading();

    Q_PROPERTY(qtimestamp timestamp READ timestamp WRITE setTimestamp)
    qtimestamp timestamp() const;
    void setTimestamp(qtimestamp timestamp);

protected:
    explicit QSensorReading(QSensorReadingPrivate *d, size_t size, void **dest);

private:
    void readValuesFrom(QSensorReading *other);
    QScopedPointer<QSensorReadingPrivate> d;
    Q_DISABLE_COPY(QSensorReading)
};

#define DECLARE_READING(classname)\
        DECLARE_READING_D(classname, classname ## Private)

#define DECLARE_READING_D(classname, pclassname)\
    public:\
        classname();\
        virtual ~classname();\
    private:\
        pclassname *d;

#define IMPLEMENT_READING(classname)\
        IMPLEMENT_READING_D(classname, classname ## Private)

#define IMPLEMENT_READING_D(classname, pclassname)\
    classname::classname()\
        : QSensorReading(new pclassname, sizeof(pclassname), (void**)&d) {}\
    classname::~classname() {}

template <typename Reading>
class Q_SENSORS_EXPORT QTypedSensorFilter : public QSensorFilter
{
public:
    virtual bool filter(Reading *reading) = 0;
private:
    bool filter(QSensorReading *reading) { return filter(static_cast<Reading*>(reading)); }
};

#define DECLARE_FILTER(FilterName, ReadingName)\
    typedef QTypedSensorFilter<ReadingName> FilterName

#define DECLARE_SENSOR(SensorName, FilterName, ReadingName)\
    public:\
        explicit SensorName(QObject *parent = 0)\
            : QSensor(parent)\
        { setType(#SensorName); }\
        virtual ~SensorName() {}\
        /* These methods shadow the ones in QSensor on purpose */\
        ReadingName *reading() const { return static_cast<ReadingName*>(QSensor::reading()); }\
        void addFilter(FilterName *filter) { QSensor::addFilter(filter); }\
        void removeFilter(FilterName *filter) { QSensor::removeFilter(filter); }\

QTM_END_NAMESPACE

#endif

