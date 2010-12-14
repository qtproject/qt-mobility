#ifndef GYROSCOPECONTROLLER_H
#define GYROSCOPECONTROLLER_H

#include "inputcontroller.h"
#include <qgyroscope.h>

QTM_USE_NAMESPACE

class GyroscopeController : public InputController
{
    Q_OBJECT

public:
    GyroscopeController();
    virtual ~GyroscopeController();
    virtual void updateCoordinates();

private slots:
    void update();

private:
    QGyroscope m_gyroscope;
    int m_dx, m_dy;

};

#endif // GYROSCOPECONTROLLER_H
