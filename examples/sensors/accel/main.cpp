#include <QtCore>
#include <QAccelerationSensor>
#include <QSensorManager>

QTM_USE_NAMESPACE

class AccelerationListener : public QSensorListener
{
public:
    bool sensorValueUpdated(QSensorValue *_value)
    {
        QAccelerationSensorValue *value = static_cast<QAccelerationSensorValue*>(_value);
        qDebug() << "acceleration: "
                 << QString().sprintf("%3.2f %3.2f %3.2f",
                         value->acceleration.x,
                         value->acceleration.y,
                         value->acceleration.z);
        return true;
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAccelerationSensor sensor(QSensorManager::instance()->defaultSensorForType(QAccelerationSensor::type));
    if (!sensor.isValid()) {
        qWarning() << "No accelerometer!";
        return 1;
    }
    sensor.addListener(new AccelerationListener);

    return app.exec();
}

