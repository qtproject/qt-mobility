#include <QtCore>
#include <QAccelerationSensor>
#include <QSensorFactory>

QTM_USE_NAMESPACE

class AccelerationListener : public QAccelerationListener
{
public:
    void accelerationChanged(const QAccelerationReading &reading)
    {
        qDebug() << "acceleration: "
                 << QString().sprintf("%i %i %i",
                         reading.x(),
                         reading.y(),
                         reading.z());
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAccelerationSensor sensor;
    if (!sensor.isValid()) {
        qWarning() << "No accelerometer!";
        return 1;
    }
    AccelerationListener listener;
    sensor.addListener(&listener);

    return app.exec();
}

