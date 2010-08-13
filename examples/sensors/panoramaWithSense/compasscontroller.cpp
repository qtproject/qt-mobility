

#include "compasscontroller.h"
#include "view.h"

CompassController::CompassController(): m_width(View::m_imageWidth){
    setDataRate(&m_compass);
    doConnect();
}

CompassController::~CompassController(){
    doDisconnect();
}


void CompassController::update()
{
    qreal azimuth = m_compass.reading()->azimuth();
    m_x = (int)(azimuth * m_width/360);
}



void CompassController::doConnect(){
    m_compass.start();
    connect(&m_compass, SIGNAL(readingChanged()), this, SLOT(update()));

}


void CompassController::doDisconnect(){
    m_compass.disconnect();
    m_compass.stop();
}
