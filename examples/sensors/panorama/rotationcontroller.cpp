#include "inputcontroller.h"
#include "rotationcontroller.h"
#include <QTime>
#include <QDebug>
#include <math.h>


RotationController::RotationController( ): TimedController(), m_factor(0.5){}

void RotationController::startSensor()
{
    m_rotationSensor.connectToBackend();
    m_rotationSensor.start();
    connect(&m_rotationSensor, SIGNAL(readingChanged()), this, SLOT(update()));

    int dataRate = m_rotationSensor.dataRate();
    m_interval = dataRate>0?1000/m_rotationSensor.dataRate():20;
}

void RotationController::stopSensor(){ m_rotationSensor.stop();}


void RotationController::update()
{
    qreal pitch = m_rotationSensor.reading()->x();
    qreal roll= m_rotationSensor.reading()->y();

    m_dx = - m_factor* roll;
    m_dy =  - m_factor * pitch;

    updateCoordinates();
}


void RotationController::updateCoordinates(){
    m_x +=m_dx * m_delay / m_interval;
    m_y +=m_dy * m_delay / m_interval;
}


