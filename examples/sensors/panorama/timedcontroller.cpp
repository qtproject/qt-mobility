

#include "inputcontroller.h"
#include "timedcontroller.h"
#include <QTime>
#include <QDebug>

TimedController::TimedController(): m_delay(10), m_interval(0){
    m_exTime = QTime::currentTime();
    m_exTimestamp =QTime::currentTime();
    m_timer.setSingleShot(false);
    m_timer.start(m_delay);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimedUpdate()));

}

TimedController::~TimedController(){
    m_timer.stop();
    disconnect(&m_timer);
}


void TimedController::handleTimedUpdate()
{
    int timeDiff = m_exTime.msecsTo(QTime::currentTime());
    if (timeDiff<  m_delay) return;
    updateCoordinates();
    m_exTime = QTime::currentTime();
}


