

#include "inputcontroller.h"
#include "magnetometercontroller.h"
#include "view.h"
#include <qmagnetometer.h>

MagnetometerController::MagnetometerController(): m_width(View::getImageWidth()){

}


void MagnetometerController::startSensor()
{
    m_magnetometer.setProperty("returnGeoValues", true);
    m_magnetometer.connectToBackend();
    m_magnetometer.start();
    connect(&m_magnetometer, SIGNAL(readingChanged()), this, SLOT(update()));
}

void MagnetometerController::stopSensor()
{
    m_magnetometer.stop();
}



void MagnetometerController::update()
{
    qreal azimuth = -m_magnetometer.reading()->z();
    azimuth = azimuth<0?360+azimuth:azimuth;
    m_x = (int)(-azimuth * m_width/360);


    //TODO: consider provide y values based on x and y
}




