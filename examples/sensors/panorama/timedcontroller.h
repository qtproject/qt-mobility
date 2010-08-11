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
    virtual ~TimedController();

private slots:
    void handleTimedUpdate();

protected:
    QTimer m_timer;
    QTime m_exTime;
    QTime m_exTimestamp;
    int m_delay;
    int m_interval;

};


#endif // TIMEDCONTROLLER_H
