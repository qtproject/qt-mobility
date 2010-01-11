#include <QtCore>
#include <QAccelerometer>
#include <QSensorFactory>

QTM_USE_NAMESPACE

class AccelerometerListener : public QAccelerometerListener
{
public:
    void accelerationChanged(const QAccelerometerReading &reading)
    {
        qDebug() << "acceleration: "
                 << QString().sprintf("%0.2f %0.2f %0.2f",
                         reading.x(),
                         reading.y(),
                         reading.z());
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAccelerometer sensor;
    if (!sensor.isValid()) {
        qWarning() << "No accelerometer!";
        return 1;
    }
    AccelerometerListener listener;
    sensor.setListener(&listener);
    sensor.setUpdatePolicy(QSensor::InfrequentUpdates);
    sensor.start();

    return app.exec();
}

