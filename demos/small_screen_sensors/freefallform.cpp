/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "freefallform.h"

const int KTreshold = 2;

FreeFallForm::FreeFallForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    dropItLabel->setText("");

    m_accelerationSensor.addFilter(this);
    m_accelerationSensor.start();
    m_freeFallState = StateNotStarted;
}

void FreeFallForm::on_setupButton_clicked()
{
    setupButton->setDisabled(true);
    dropItLabel->setText("Drop it!");
    freeFallTimeLabel->setText("-");
    freeFallHeightLabel->setText("-");
    m_freeFallState = StateSetup;
}

bool FreeFallForm::filter(QAccelerometerReading *reading)
{
    switch (m_freeFallState) {
    case StateSetup:
        if (abs(reading->x()) < KTreshold && abs(reading->y()) < KTreshold && abs(reading->z()) < KTreshold) {
            m_freeFallTimer.start();
            m_freeFallState = StateFalling;
        }
        break;
    case StateFalling:
        if (abs(reading->x()) > KTreshold || abs(reading->y()) > KTreshold || abs(reading->z()) > KTreshold) {
            freeFallTimeLabel->setNum(m_freeFallTimer.elapsed());
            qreal t = (qreal(m_freeFallTimer.elapsed()) / 1000);
            freeFallHeightLabel->setNum(9.81 / 2 * t * t);
            m_freeFallState = StateNotStarted;
            setupButton->setEnabled(true);
            setupButton->setFocus();
            dropItLabel->setText("");
        }
        break;
    default:
        break;
    }

    return false;
}
