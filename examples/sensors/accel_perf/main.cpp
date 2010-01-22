#include <QtCore>
#include <QtTest>
#include <qaccelerometer.h>

QTM_USE_NAMESPACE

class accel_perf : public QObject
{
    Q_OBJECT
private slots:
    void accel_speed_raw();
    void accel_speed_raw_nosignal();
    void accel_speed_raw_signal();
    void accel_speed_filter();
    void accel_speed_filter_nosignal();
    void accel_speed_filter_signal();
};

class receiver : public QObject, public QAccelerometerFilter
{
    Q_OBJECT
public:
    bool filter(QAccelerometerReading *reading)
    {
        Q_UNUSED(reading)
        //qDebug() << "filter";
        return true;
    }
public slots:
    void sensorChanged()
    {
        //qDebug() << "sensorChanged";
    }
};

void accel_perf::accel_speed_raw()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    sensor.connect();
    QVERIFY(sensor.isAvailable());
    sensor.setUpdatePolicy(QSensor::PolledUpdates);
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_raw_nosignal()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    sensor.connect();
    QVERIFY(sensor.isAvailable());
    sensor.setUpdatePolicy(QSensor::PolledUpdates);
    sensor.setSignalEnabled(false);
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_raw_signal()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    sensor.connect();
    QVERIFY(sensor.isAvailable());
    sensor.setUpdatePolicy(QSensor::PolledUpdates);
    receiver r;
    connect(&sensor, SIGNAL(readingChanged()), &r, SLOT(sensorChanged()));
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_filter()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    sensor.connect();
    QVERIFY(sensor.isAvailable());
    sensor.setUpdatePolicy(QSensor::PolledUpdates);
    receiver r;
    sensor.addFilter(&r);
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_filter_nosignal()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    sensor.connect();
    QVERIFY(sensor.isAvailable());
    sensor.setUpdatePolicy(QSensor::PolledUpdates);
    sensor.setSignalEnabled(false);
    receiver r;
    sensor.addFilter(&r);
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

void accel_perf::accel_speed_filter_signal()
{
    QAccelerometer sensor;
    //sensor.setIdentifier("dummy.accelerometer"); // We want the dummy accelerometer
    sensor.connect();
    QVERIFY(sensor.isAvailable());
    sensor.setUpdatePolicy(QSensor::PolledUpdates);
    receiver r;
    sensor.addFilter(&r);
    connect(&sensor, SIGNAL(readingChanged()), &r, SLOT(sensorChanged()));
    sensor.start();
    QBENCHMARK { sensor.poll(); }
}

QTEST_MAIN(accel_perf)

#include "main.moc"

