#ifndef ORIENTATIONCONTROLLER_H
#define ORIENTATIONCONTROLLER_H


#include "keycontroller.h"
#include <qorientationsensor.h>
#include <QTime>
#include <QTimer>


//QTM_USE_NAMESPACE

class OrientationController : public KeyController
{
    Q_OBJECT

public:
    OrientationController();
    virtual ~OrientationController();
    virtual void updateCoordinates();

private slots:
    void update();

private:
    QTM_NAMESPACE::QOrientationSensor m_orientationSensor;

};



#endif // ORIENTATIONCONTROLLER_H
