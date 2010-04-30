#ifndef TIMEDCONTROLLER_H
#define TIMEDCONTROLLER_H


#include <QTime>
#include <QTimer>
#include "inputcontroller.h"

class TimedController : public InputController
{
    Q_OBJECT

public:
    TimedController();
    virtual void startTimer();
    virtual void stopTimer();
    virtual void updateCoordinates();

private slots:
    void handleTimedUpdate();

protected:
    QTimer m_timer;
    QTime m_exTime;
    int m_delay;

};


#endif // TIMEDCONTROLLER_H
