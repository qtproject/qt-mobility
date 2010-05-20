

#include "inputcontroller.h"
#include "compasscontroller.h"
#include "view.h"

CompassController::CompassController(): m_width(View::m_imageWidth){
    m_compass.connectToBackend();
    m_compass.start();
    connect(&m_compass, SIGNAL(readingChanged()), this, SLOT(update()));
}

CompassController::~CompassController(){
    m_compass.stop();
    disconnect(&m_compass);
}



void CompassController::update()
{
    qreal azimuth = m_compass.reading()->azimuth();
    m_x = (int)(azimuth * m_width/360);
}



