#include <QWidget>
#include <qaccelerometer.h>
#include <qorientationsensor.h>

QTM_USE_NAMESPACE

class MyWidget : public QWidget
{
    void create();
};

void MyWidget::create()
{
//! [Creating a sensor]
// On the heap (deleted when this object is deleted)
QAccelerometer *sensor = new QAccelerometer(this);

// On the stack (deleted when the current scope ends)
QOrientationSensor orient_sensor;
//! [Creating a sensor]
}

