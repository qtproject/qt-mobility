

#include "accelerometercontroller.h"

QAccelerometer AccelerometerController::m_accelerometer;

AccelerometerController::AccelerometerController(): InputController(){
    setDataRate(&m_accelerometer);
    doConnect();
}


AccelerometerController::~AccelerometerController(){
    doDisconnect();
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



void AccelerometerController::doConnect(){
    m_accelerometer.start();
    connect(&m_accelerometer, SIGNAL(readingChanged()), this, SLOT(update()));
}


void AccelerometerController::doDisconnect(){
    m_accelerometer.disconnect();
    m_accelerometer.stop();
}
