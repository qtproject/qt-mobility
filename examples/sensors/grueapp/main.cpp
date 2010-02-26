#include <QtCore>
#include <qsensor.h>

QTM_USE_NAMESPACE

class Filter : public QSensorFilter
{
public:
    bool filter(QSensorReading *reading)
    {
        int percent = reading->property("chanceOfBeingEaten").value<qreal>() * 100;
        qDebug() << "Your chance of being eaten by a Grue:" << percent << "percent.";
        return false;
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QSensor sensor;
    sensor.setType("GrueSensor");
    if (!sensor.connect()) {
        qWarning("Grue sensor is not available!");
        return 1;
    }

    Filter filter;
    sensor.addFilter(&filter);
    sensor.start();

    return app.exec();
}

