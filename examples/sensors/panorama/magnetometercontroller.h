#ifndef MAGNETOMETERCONTROLLER_H
#define MAGNETOMETERCONTROLLER_H

#include "inputcontroller.h"
#include <qmagnetometer.h>



class MagnetometerController : public InputController
{
    Q_OBJECT

public:
    MagnetometerController();
    virtual void startSensor();
    virtual void stopSensor();

private slots:
    void update();

private:
    QTM_NAMESPACE::QMagnetometer  m_magnetometer;
    int m_width;
    int m_height;

};



#endif // MAGNETOMETERCONTROLLER_H
