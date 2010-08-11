

#include "inputcontroller.h"
#include "accelerometercontroller.h"

AccelerometerController::AccelerometerController(): InputController(){
    m_accelerometer.connectToBackend();
    m_accelerometer.start();
    connect(&m_accelerometer, SIGNAL(readingChanged()), this, SLOT(update()));
}

AccelerometerController::~AccelerometerController(){
    m_accelerometer.stop();
    disconnect(&m_accelerometer);
}


void AccelerometerController::update()
{
    qreal accX = m_accelerometer.reading()->x();
    qreal accY= m_accelerometer.reading()->y();
    m_dx = accX*3;
    m_dy= -accY*3;
    updateCoordinates();

}


void AccelerometerController::updateCoordinates(){
    m_x +=m_dx;
    m_y +=m_dy;

}

