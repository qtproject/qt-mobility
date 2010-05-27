#ifndef ROTATIONCONTROLLER_H
#define ROTATIONCONTROLLER_H


#include "timedcontroller.h"
#include <qrotationsensor.h>
#include <QTimer>
#include <QTime>

QTM_USE_NAMESPACE

class RotationController : public InputController
{
    Q_OBJECT

public:
    RotationController();
    virtual ~RotationController();
    virtual void updateCoordinates();


public slots:
    void update();

private:
    QTM_NAMESPACE::QRotationSensor m_rotationSensor;
    int m_dx, m_dy;
    qtimestamp m_exTimestamp;
    qreal m_factor;

};


#endif // ROTATIONCONTROLLER_H
