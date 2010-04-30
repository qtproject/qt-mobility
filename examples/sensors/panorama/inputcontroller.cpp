
#include "inputcontroller.h"
#include "view.h"

const QString InputController::QACCELEROMETER="QAccelerometer";
const QString InputController::QORIENTATIONSENSOR = "QOrientationSensor";
const QString InputController::QMAGNETOMETER = "QMagnetometer";
const QString InputController::QROTATIONSENSOR = "QRotationSensor";
const QString InputController::QTAPSENSOR = "QTapSensor";
const QString InputController::QCOMPASS = "QCompass";
const QString InputController::QKEYS = "Keys";



InputController::InputController() : m_x(0), m_y(0) {
}

void InputController::keyPressEvent(QKeyEvent *e){}


void InputController::start(){

    startSensor();
    startTimer();
}


void InputController::stop(){
    stopSensor();
    stopTimer();
}


int InputController::getX(){
    int w = View::getImageWidth();
    m_x = qAbs(m_x) < w ? m_x : m_x % w;
    return m_x;
}

int InputController::getY() {
    return m_y;
}


void InputController::setY(int y){
    m_y = y;
}




void InputController::startTimer(){}
void InputController::stopTimer(){}
void InputController::startSensor(){}
void InputController::stopSensor(){}

