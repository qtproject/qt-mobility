#include <QtCore>
#include <qaccelerometer.h>
#include <qsensorfactory.h>

QTM_USE_NAMESPACE

#if 0
class AccelerometerFilter : public QSensorFilter
{
public:
    bool filter(QSensorReading *_reading)
    {
        QAccelerometerReading *reading = static_cast<QAccelerometerReading*>(_reading);
        qDebug() << "acceleration: "
                 << QString().sprintf("%0.2f %0.2f %0.2f",
                         reading->x(),
                         reading->y(),
                         reading->z());
        return false; // don't store the reading in the sensor
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QSensor sensor;
    sensor.setType("QAccelerometer");
    sensor.connect();
    if (!sensor.isAvailable()) {
        qWarning("No Accelerometer available!");
        return 1;
    }
    AccelerometerFilter filter;
    sensor.addFilter(&filter);
    sensor.setUpdatePolicy(QSensor::InfrequentUpdates);
    sensor.start();

    return app.exec();
}
#else
class AccelerometerFilter : public QAccelerometerFilter
{
public:
    bool filter(QAccelerometerReading *reading)
    {
        qDebug() << "acceleration: "
                 << QString().sprintf("%0.2f %0.2f %0.2f",
                         reading->x(),
                         reading->y(),
                         reading->z());
        return false; // don't store the reading in the sensor
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAccelerometer sensor;
    sensor.connect();
    if (!sensor.isAvailable()) {
        qWarning("No Accelerometer available!");
        return 1;
    }
    AccelerometerFilter filter;
    sensor.addFilter(&filter);
    sensor.setUpdatePolicy(QSensor::InfrequentUpdates);
    sensor.start();

    return app.exec();
}
#endif

