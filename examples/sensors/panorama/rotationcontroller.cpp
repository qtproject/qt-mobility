#include "inputcontroller.h"
#include "rotationcontroller.h"
#include <QTime>
#include <QDebug>
#include <math.h>


RotationController::RotationController(): InputController(), m_factor(0.5){
    m_rotationSensor.connectToBackend();
    m_rotationSensor.start();
    connect(&m_rotationSensor, SIGNAL(readingChanged()), this, SLOT(update()));
}

RotationController::~RotationController(){
    m_rotationSensor.stop();
    disconnect(&m_rotationSensor);
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


