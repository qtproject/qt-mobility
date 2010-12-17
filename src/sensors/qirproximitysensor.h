
#ifndef QIRPROXIMITYSENSOR_H
#define QIRPROXIMITYSENSOR_H

#include <qsensor.h>

QTM_BEGIN_NAMESPACE

class QIRProximityReadingPrivate;

class Q_SENSORS_EXPORT QIRProximityReading : public QSensorReading
{
    Q_OBJECT
    Q_PROPERTY(qreal reflectance READ reflectance)
    DECLARE_READING(QIRProximityReading)
public:
    qreal reflectance() const;
    void setReflectance(qreal reflectance);
};

class Q_SENSORS_EXPORT QIRProximityFilter : public QSensorFilter
{
public:
    virtual bool filter(QIRProximityReading *reading) = 0;
private:
    bool filter(QSensorReading *reading) { return filter(static_cast<QIRProximityReading*>(reading)); }
};

class Q_SENSORS_EXPORT QIRProximitySensor : public QSensor
{
    Q_OBJECT
public:
    explicit QIRProximitySensor(QObject *parent = 0) : QSensor(QIRProximitySensor::type, parent) {}
    virtual ~QIRProximitySensor() {}
    QIRProximityReading *reading() const { return static_cast<QIRProximityReading*>(QSensor::reading()); }
    static char const * const type;

    // for calibrating in games or whatever
    qreal minimumDistance() const;
    qreal maximumDistance() const;

    // control the transmission power
    int maxPowerLevel() const;
    int powerLevel() const;
    void setPowerLevel(int level);
};

QTM_END_NAMESPACE

#endif
