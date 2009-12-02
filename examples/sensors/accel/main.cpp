#include <QtCore>
#include <QAccelerationSensor>
#include <QSensorManager>

QTM_USE_NAMESPACE

class SAccelerationListener : public QSpecificSensorListener<QAccelerationSensorEvent>
{
public:
    void sensorEvent(QAccelerationSensorEvent *event)
    {
        qDebug() << "acceleration: "
                 << QString().sprintf("%3.2f %3.2f %3.2f",
                         event->acceleration.x,
                         event->acceleration.y,
                         event->acceleration.z);
    }
};

class AccelerationListener : public QSensorListener
{
public:
    void sensorEvent(QSensorEvent *_event)
    {
        QAccelerationSensorEvent *event = static_cast<QAccelerationSensorEvent*>(_event);
        qDebug() << "acceleration: "
                 << QString().sprintf("%3.2f %3.2f %3.2f",
                         event->acceleration.x,
                         event->acceleration.y,
                         event->acceleration.z);
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAccelerationSensor sensor(QSensorManager::instance()->defaultSensorForType(QSensor::Acceleration));
    if (!sensor.isValid()) {
        qWarning() << "No accelerometer!";
        return 1;
    }
    //sensor.addListener(new SAccelerationListener);
    sensor.addListener(new AccelerationListener);

    return app.exec();
}

