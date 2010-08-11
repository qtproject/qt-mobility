#include <QtCore>
#include <qaccelerometer.h>

QTM_USE_NAMESPACE

namespace arrows{
    static const QString ARROW_UP="^", ARROW_DOWN="v", ARROW_LEFT="<",ARROW_RIGHT=">";
}

class AccelerometerFilter : public QAccelerometerFilter
{
public:
    bool filter(QAccelerometerReading *reading)
    {
        QString arrow = getArrowKey(reading->x(), reading->y());
        if (arrow!=exArrow){
            qDebug() << "direction:" << arrow;
            exArrow = arrow;
        }
        return false; // don't store the reading in the sensor
    }

private:
    QString exArrow;
    QString getArrowKey(qreal x, qreal y){
        // axis_x: LEFT or RIGHT
        if (abs(x)>abs(y)) return x>0?(arrows::ARROW_LEFT):(arrows::ARROW_RIGHT);
        // axis_y: UP or DOWN
        return y>0?(arrows::ARROW_DOWN):(arrows::ARROW_UP);
    }
    static qreal abs(qreal value) {return value<0?-value:value;}
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QAccelerometer sensor;
    if (!sensor.connectToBackend()) {
        qWarning("No Accelerometer available!");
        return EXIT_FAILURE;
    }
    AccelerometerFilter filter;
    sensor.addFilter(&filter);

    qrangelist datarates = sensor.availableDataRates();
    int i = datarates.size();
    sensor.setDataRate(datarates.at(i-1).second);
    sensor.start();

    return app.exec();
}
