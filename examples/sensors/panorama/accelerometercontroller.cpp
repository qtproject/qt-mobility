

#include "inputcontroller.h"
#include "accelerometercontroller.h"
#include <QTime>
#include <QDebug>

AccelerometerController::AccelerometerController( ):
        TimedController(){
}

void AccelerometerController::startSensor()
{

    m_accelerometer.connectToBackend();
    m_accelerometer.start();
    connect(&m_accelerometer, SIGNAL(readingChanged()), this, SLOT(update()));
}

void AccelerometerController::stopSensor()
{
    m_accelerometer.stop();
}


void AccelerometerController::update()
{

    qreal accX = m_accelerometer.reading()->x();
    qreal accY= m_accelerometer.reading()->y();
    m_dx = (int)(5*accX);
    m_dy= (int)(-5*accY);
    updateCoordinates();
}


void AccelerometerController::updateCoordinates(){
    m_x +=m_dx;
    m_y +=m_dy;
}

