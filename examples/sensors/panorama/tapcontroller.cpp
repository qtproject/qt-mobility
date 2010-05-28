

#include "tapcontroller.h"
#include <QDebug>


const qreal TapController::m_timewindow=1000;


TapController::TapController(): TimedController(), m_step(20){
    m_accelerometer.connectToBackend();
    m_accelerometer.start();
    connect(&m_accelerometer, SIGNAL(readingChanged()), this, SLOT(updateAcce()));

    m_tap.connectToBackend();
    m_tap.start();
    connect(&m_tap, SIGNAL(readingChanged()), this, SLOT(update()));

}

TapController::~TapController(){
    m_tap.stop();
    disconnect(&m_tap);
    m_accelerometer.stop();
    disconnect(&m_accelerometer);
}



void TapController::update()
{
    int direction = m_tap.reading()->tapDirection();
    switch(direction){
    case QTapReading::X:
    case QTapReading::X_Pos:
    case QTapReading::X_Neg:
        m_dy=0;
        setDx(direction);
        break;
    case QTapReading::Y:
    case QTapReading::Y_Pos:
    case QTapReading::Y_Neg:
        m_dx = 0;
        setDy(direction);
        break;
    case QTapReading::Undefined:
    case QTapReading::Z:
    case QTapReading::Z_Pos:
    case QTapReading::Z_Neg:
    default:
        m_dx = 0; m_dy = 0;
        return;
    }    

}


void TapController::updateAcce()
{
    qreal accX = m_accelerometer.reading()->x();
    qreal accY= m_accelerometer.reading()->y();
    m_now = m_accelerometer.reading()->timestamp();

    checkX(accX);
    checkY(accY);
}


void TapController::updateCoordinates(){

    if (m_now - m_timestampX > m_timewindow){
        m_absMaxX = 0;
        m_timestampX = m_now;
    }

    if (m_now - m_timestampY > m_timewindow){
        m_absMaxY = 0;
        m_timestampY = m_now;
    }

    if (m_dx!=0) m_dx = m_dx>0? m_dx-1: m_dx+1;
    if (m_dy!=0) m_dy = m_dy>0? m_dy-1: m_dy+1;

    m_x +=m_dx;
    m_y +=m_dy;
}


void TapController::checkX(qreal accX){
    if (qAbs(accX)>qAbs(m_absMaxX)){
        m_absMaxX = accX;
        m_timestampX = m_now;
    }
}


void TapController::checkY(qreal accY){
    if (qAbs(accY)>qAbs(m_absMaxY)){
        m_absMaxY = accY;
        m_timestampY = m_now;
    }
}


void TapController::setDx(int direction){
    switch(direction){
    case QTapReading::X:
        m_dx = m_absMaxX>0?m_step : -m_step;
        break;
    case QTapReading::X_Pos:
        m_dx = m_step;
        break;
    case QTapReading::X_Neg:
        m_dx = -m_step;
        break;
    }
}

void TapController::setDy(int direction){
    switch(direction){
    case QTapReading::Y:
        m_dy = m_absMaxY>0?m_step : - m_step;
        break;
    case QTapReading::Y_Pos:
        m_dy = m_step;
        break;
    case QTapReading::Y_Neg:
        m_dy = -m_step;
        break;
    }
}

