#include <QtCore>
#include <QAccelerationSensor>
#include <QSensorFactory>

QTM_USE_NAMESPACE

class AccelerationListener : public QSensorListener
{
public:
    bool sensorValueUpdated(QSensorValue *_value)
    {
        QAccelerationSensorValue *value = static_cast<QAccelerationSensorValue*>(_value);
        qDebug() << "acceleration: "
                 << QString().sprintf("%3.2f %3.2f %3.2f",
                         value->x,
                         value->y,
                         value->z);
        return true;
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAccelerationSensor sensor(QSensorFactory::instance()->defaultSensorForType(QAccelerationSensor::type));
    if (!sensor.isValid()) {
        qWarning() << "No accelerometer!";
        return 1;
    }
    sensor.addListener(new AccelerationListener);

    return app.exec();
}

