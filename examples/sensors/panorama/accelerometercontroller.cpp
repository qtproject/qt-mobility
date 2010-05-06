

#include "inputcontroller.h"
#include "accelerometercontroller.h"
#include <QTime>
#include <QDebug>

AccelerometerController::AccelerometerController( ): TimedController(){}

void AccelerometerController::startSensor()
{
    m_accelerometer.connectToBackend();
    m_accelerometer.start();
    connect(&m_accelerometer, SIGNAL(readingChanged()), this, SLOT(update()));
    int dataRate = m_accelerometer.dataRate();
    m_interval = dataRate>0?1000/m_accelerometer.dataRate():20;
}

void AccelerometerController::stopSensor(){ m_accelerometer.stop();}

void AccelerometerController::update()
{
    qreal accX = m_accelerometer.reading()->x();
    qreal accY= m_accelerometer.reading()->y();
    m_dx = (int)(2*accX);
    m_dy= (int)(-2*accY);
    updateCoordinates();
}


void AccelerometerController::updateCoordinates(){
    m_x +=m_dx * m_delay / m_interval;
    m_y +=m_dy * m_delay / m_interval;
}

