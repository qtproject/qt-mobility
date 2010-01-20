#include <QtCore>
#include <qaccelerometer.h>
#include <qsensorfactory.h>

QTM_USE_NAMESPACE

QAccelerometer *accelerometer;

class AccelerometerFilter : public QAccelerometerFilter
{
public:
    bool filter(QAccelerometerReading *reading)
    {
#if 0
        QAccelerometerReading *lastReading = accelerometer->reading();
        qDebug() << "acceleration: "
                 << QString().sprintf("%0.2f (%0.2f) %0.2f (%0.2f) %0.2f (%0.2f)",
                         reading->x(),
                         lastReading->x() - reading->x(),
                         reading->y(),
                         lastReading->y() - reading->y(),
                         reading->z(),
                         lastReading->z() - reading->z());
        return true; // so the last reading is available!
#else
        qDebug() << "acceleration: "
                 << QString().sprintf("%0.2f %0.2f %0.2f",
                         reading->x(),
                         reading->y(),
                         reading->z());
        return false; // don't store the reading in the sensor
#endif
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAccelerometer sensor;
    accelerometer = &sensor;
    sensor.connect();
    if (!sensor.isAvailable()) {
        qWarning("No Accelerometer available!");
        return 1;
    }
    AccelerometerFilter filter;
    sensor.setSignalEnabled(false);
    sensor.addFilter(&filter);
    //sensor.setUpdatePolicy(QSensor::InfrequentUpdates);
    sensor.setUpdateInterval(100); // as fast as the sensor can go!
    sensor.start();

    return app.exec();
}

