#ifndef ROTATIONCONTROLLER_H
#define ROTATIONCONTROLLER_H


#include "inputcontroller.h"
#include <qrotationsensor.h>

QTM_USE_NAMESPACE

class RotationController : public InputController
{
    Q_OBJECT

public:
    RotationController();
    virtual ~RotationController();
    virtual void updateCoordinates();

public slots:
    virtual void doStart();
    virtual void doStop();

private slots:
    void update();

private:
    QTM_NAMESPACE::QRotationSensor m_rotationSensor;
    int m_dx, m_dy;
    qtimestamp m_exTimestamp;
    qreal m_factor;

};


#endif // ROTATIONCONTROLLER_H
