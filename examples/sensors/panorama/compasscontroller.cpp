

#include "inputcontroller.h"
#include "compasscontroller.h"
#include "view.h"

CompassController::CompassController( ): m_width(View::getImageWidth()){}



void CompassController::startSensor()
{

    m_compass.connectToBackend();
    m_compass.start();
    connect(&m_compass, SIGNAL(readingChanged()), this, SLOT(update()));
}


void CompassController::stopSensor()
{
    m_compass.stop();
}



void CompassController::update()
{
    qreal azimuth = m_compass.reading()->azimuth();
    m_x = (int)(azimuth * m_width/360);
}



