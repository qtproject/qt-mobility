

#include "keycontroller.h"
#include "orientationcontroller.h"
#include "view.h"

class QOrientationSensor;

OrientationController::OrientationController( ): KeyController(){
    m_delay= 200;
    m_step=2;
}


void OrientationController::startSensor()
{
    qDebug()<<"OrientationController::startSensor()";
    m_orientationSensor.connectToBackend();
    m_orientationSensor.start();
    connect(&m_orientationSensor, SIGNAL(readingChanged()), this, SLOT(update()));
}

void OrientationController::stopSensor()
{
    m_orientationSensor.stop();
}



void OrientationController::updateCoordinates(){
    handleKeyPress(m_exCode);
}


void OrientationController::update()
{

    qDebug()<<"OrientationController::update()";


    switch (m_orientationSensor.reading()->orientation()){
    case QTM_NAMESPACE::QOrientationReading::TopUp:
        m_exCode = Qt::Key_Up;
        return;
    case QTM_NAMESPACE::QOrientationReading::TopDown:
        m_exCode = Qt::Key_Down;
        return;
    case QTM_NAMESPACE::QOrientationReading::LeftUp:
        m_exCode = Qt::Key_Left;
        return;
    case QTM_NAMESPACE::QOrientationReading::RightUp:
        m_exCode = Qt::Key_Right;
    default:
        m_exCode = 0;
        return;
    }

    handleKeyPress(m_exCode);
}




