#ifndef TAPCONTROLLER_H
#define TAPCONTROLLER_H
#include "timedcontroller.h"
#include <qtapsensor.h>
#include <qaccelerometer.h>

QTM_USE_NAMESPACE

class TapController : public TimedController
{
    Q_OBJECT

public:
    TapController();
    virtual ~TapController();
    virtual void updateCoordinates();

private slots:
    void update();
    void updateAcce();

private:
    void checkX(qreal);
    void checkY(qreal);
    void setDx(int);
    void setDy(int);
    QTapSensor m_tap;
    QAccelerometer m_accelerometer;
    qreal m_absMaxX, m_absMaxY;
    qtimestamp m_timestampX, m_timestampY, m_now;
    static const qreal m_timewindow;
    int m_dx,m_dy;
    int m_step;

};

#endif // TAPCONTROLLER_H
