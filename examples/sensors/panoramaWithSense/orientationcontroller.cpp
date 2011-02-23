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

#include "orientationcontroller.h"


OrientationController::OrientationController(): KeyController(){
    m_delay=10;
    m_step=10;
    doStart();

}

OrientationController::~OrientationController(){
     doStop();
}

void OrientationController::updateCoordinates(){
    handleKeyPress(m_exCode);
    m_step = m_exCode!=0 ? m_step+5 : 10;
}


void OrientationController::update()
{
    switch (m_orientationSensor.reading()->orientation()){
    case QTM_NAMESPACE::QOrientationReading::TopUp:
        m_exCode = Qt::Key_Up;
        break;
    case QTM_NAMESPACE::QOrientationReading::TopDown:
        m_exCode = Qt::Key_Down;
        break;
    case QTM_NAMESPACE::QOrientationReading::LeftUp:
        m_exCode = Qt::Key_Left;
        break;
    case QTM_NAMESPACE::QOrientationReading::RightUp:
        m_exCode = Qt::Key_Right;
        break;
    default:
        m_exCode = 0;
        return;
    }
    handleKeyPress(m_exCode);
}




void OrientationController::doStart(){
    m_orientationSensor.start();
    connect(&m_orientationSensor, SIGNAL(readingChanged()), this, SLOT(update()));
}


void OrientationController::doStop(){
    m_orientationSensor.disconnect();
    m_orientationSensor.stop();

}
