/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/
#include "inputcontroller.h"

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



void InputController::setDataRate(QSensor* sensor)
{
    qrangelist datarates = sensor->availableDataRates();
    int i = datarates.size();
    if (i==0) return;
    int minRate = datarates.at(i-1).first;
    int maxRate = datarates.at(i-1).second;
    if (minRate==maxRate) sensor->setDataRate(minRate);
    else sensor->setDataRate((int)((maxRate+minRate)/2));
};


void InputController::doConnect(){
}


void InputController::doDisconnect(){

}
