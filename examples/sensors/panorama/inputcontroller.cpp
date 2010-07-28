
#include "inputcontroller.h"
#include "view.h"

const QString InputController::QACCELEROMETER="QAccelerometer";
const QString InputController::QORIENTATIONSENSOR = "QOrientationSensor";
const QString InputController::QMAGNETOMETER = "QMagnetometer";
const QString InputController::QROTATIONSENSOR = "QRotationSensor";
const QString InputController::QTAPSENSOR = "QTapSensor";
const QString InputController::QCOMPASS = "QCompass";
const QString InputController::QKEYS = "Keys";

int InputController::m_x =0;
int InputController::m_y =0;

InputController::InputController() {}

void InputController::keyPressEvent(QKeyEvent*){}

int InputController::getX(){return m_x;}

int InputController::getY() {return m_y;}

void InputController::setX(int x){m_x = x;}

void InputController::setY(int y){m_y = y;}

void InputController::updateCoordinates(){}
