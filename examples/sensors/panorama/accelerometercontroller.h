#ifndef ACCELEROMETERCONTROLLER_H
#define ACCELEROMETERCONTROLLER_H

#include "timedcontroller.h"
#include <qaccelerometer.h>
#include <QTimer>
#include <QTime>
#include <QDebug>



QTM_USE_NAMESPACE

class AccelerometerController : public InputController
{
    Q_OBJECT

public:
    AccelerometerController();
    virtual ~AccelerometerController();
    virtual void updateCoordinates();

public slots:
    void update();

private:
    QAccelerometer m_accelerometer; 
    int m_dx, m_dy;

};

#endif // ACCELEROMETERCONTROLLER_H
