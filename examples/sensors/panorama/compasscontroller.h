#ifndef COMPASSCONTROLLER_H
#define COMPASSCONTROLLER_H

#include "inputcontroller.h"
#include <qcompass.h>

QTM_USE_NAMESPACE

class CompassController : public InputController
{
    Q_OBJECT

public:
    CompassController();
    virtual void startSensor();
    virtual void stopSensor();

private slots:
    void update();

private:
    QCompass m_compass;
    int m_width;

};



#endif // COMPASSCONTROLLER_H
