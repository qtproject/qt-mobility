

#include "timedcontroller.h"

TimedController::TimedController(): m_delay(10), m_interval(0){
    m_exTime = QTime::currentTime();
    m_exTimestamp =QTime::currentTime();
    m_timer.setSingleShot(false);
    doStart();
}

TimedController::~TimedController(){
    doStop();
}


void TimedController::handleTimedUpdate()
{
    int timeDiff = m_exTime.msecsTo(QTime::currentTime());
    if (timeDiff<  m_delay) return;
    updateCoordinates();
    m_exTime = QTime::currentTime();
}



void TimedController::doStart(){
    m_timer.start(m_delay);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimedUpdate()));
}


void TimedController::doStop(){
    disconnect(&m_timer);
    m_timer.stop();
}
