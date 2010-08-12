

#include "magnetometercontroller.h"
#include "view.h"

MagnetometerController::MagnetometerController(): m_width(View::m_imageWidth){
    setDataRate(&m_magnetometer);
    m_magnetometer.setProperty("returnGeoValues", true);
    doStart();
}


MagnetometerController::~MagnetometerController(){
    disconnect(&m_magnetometer);
}


void MagnetometerController::update()
{
    qreal azimuth = -m_magnetometer.reading()->z();
    azimuth = azimuth<0?360+azimuth:azimuth;
    m_x = (int)(-azimuth * m_width/360);


    //TODO: consider provide y values based on x and y
}




void MagnetometerController::doStart(){
    m_magnetometer.start();
    connect(&m_magnetometer, SIGNAL(readingChanged()), this, SLOT(update()));
}


void MagnetometerController::doStop(){
    m_magnetometer.disconnect();
    m_magnetometer.stop();
}
