#include <qsensor.h>

QTM_USE_NAMESPACE

void start()
{
//! [Starting a sensor]
// start the sensor
QSensor sensor;
sensor.setType("QAccelerometer");
sensor.start();

// later
QSensorReading *reading = sensor.reading();
qreal x = reading->property("x").value<qreal>();
qreal y = reading->value(1).value<qreal>();
//! [Starting a sensor]
}
