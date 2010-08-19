#ifndef ACCELEROMETERCONTROLLER_H
#define ACCELEROMETERCONTROLLER_H

#include "inputcontroller.h"
#include <qaccelerometer.h>


QTM_USE_NAMESPACE

class AccelerometerController : public InputController
{
    Q_OBJECT

public:
    AccelerometerController();
    virtual ~AccelerometerController();
    virtual void updateCoordinates();


public slots:
    virtual void doConnect();
    virtual void doDisconnect();

private slots:
    void update();

private:
    static QAccelerometer m_accelerometer;
    int m_dx, m_dy;

};

#endif // ACCELEROMETERCONTROLLER_H
