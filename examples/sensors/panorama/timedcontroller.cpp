

#include "inputcontroller.h"
#include "timedcontroller.h"
#include <QTime>
#include <QDebug>

TimedController::TimedController( ):
        m_delay(100){

    m_exTime = QTime::currentTime();
}

void TimedController::startTimer()
{
    m_timer.setSingleShot(false);
    m_timer.start(m_delay);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimedUpdate()));
}

void TimedController::stopTimer()
{
    m_timer.stop();
}


void TimedController::handleTimedUpdate()
{
//    qDebug()<<"handleTimedUpdate timeDiff1=";
    int timeDiff = m_exTime.msecsTo(QTime::currentTime());
    qDebug()<<"handleTimedUpdate timeDiff2="<<timeDiff;

    if (timeDiff< 2* m_delay) return;
    updateCoordinates();
    m_exTime = QTime::currentTime();
}


void TimedController::updateCoordinates(){}

