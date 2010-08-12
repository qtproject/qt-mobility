#include "rotationcontroller.h"


RotationController::RotationController(): InputController(), m_factor(0.5){
    setDataRate(&m_rotationSensor);
    doStart();
}

RotationController::~RotationController(){
    doStop();
}


void RotationController::update()
{
    qreal pitch = m_rotationSensor.reading()->x();
    qreal roll= m_rotationSensor.reading()->y();

    m_dx = - m_factor* roll;
    m_dy =  - m_factor * pitch;

    updateCoordinates();
}


void RotationController::updateCoordinates(){
    m_x +=m_dx;
    m_y +=m_dy;
}




void RotationController::doStart(){
    m_rotationSensor.start();
    connect(&m_rotationSensor, SIGNAL(readingChanged()), this, SLOT(update()));

}


void RotationController::doStop(){
    m_rotationSensor.disconnect();
    m_rotationSensor.stop();

}
