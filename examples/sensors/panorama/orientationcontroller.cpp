

#include "keycontroller.h"
#include "orientationcontroller.h"
#include "view.h"


OrientationController::OrientationController( ): KeyController(){
    m_delay=20;
    m_step=7;
}


void OrientationController::startSensor()
{
    m_orientationSensor.connectToBackend();
    m_orientationSensor.start();
    connect(&m_orientationSensor, SIGNAL(readingChanged()), this, SLOT(update()));
}

void OrientationController::stopSensor(){ m_orientationSensor.stop();}


void OrientationController::updateCoordinates(){
    handleKeyPress(m_exCode);
    m_step = m_exCode!=0? m_step+2 : 5;
}


void OrientationController::update()
{
    switch (m_orientationSensor.reading()->orientation()){
    case QTM_NAMESPACE::QOrientationReading::TopUp:
        m_exCode = Qt::Key_Up;
        break;
    case QTM_NAMESPACE::QOrientationReading::TopDown:
        m_exCode = Qt::Key_Down;
        break;
    case QTM_NAMESPACE::QOrientationReading::LeftUp:
        m_exCode = Qt::Key_Left;
        break;
    case QTM_NAMESPACE::QOrientationReading::RightUp:
        m_exCode = Qt::Key_Right;
        break;
    default:
        m_exCode = 0;
        return;
    }
    handleKeyPress(m_exCode);
}




