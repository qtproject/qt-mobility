#include <QtCore>
#include <QAccelerationSensor>
#include <QSensorFactory>

QTM_USE_NAMESPACE

class AccelerationListener : public QSensorListener
{
public:
    bool sensorValueUpdated(QSensorValue *_value)
    {
        QAccelerationValue *value = static_cast<QAccelerationValue*>(_value);
        qDebug() << "acceleration: "
                 << QString().sprintf("%i %i %i",
                         value->acceleration.x,
                         value->acceleration.y,
                         value->acceleration.z);
        return true;
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
    sensor.addListener(new AccelerationListener);

    return app.exec();
}

