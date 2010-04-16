/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "accelerationform.h"

AccelerationForm::AccelerationForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    m_accelerationSensor.connectToBackend();
    
    // Usually sensor operates -2g-2g range on Symbian platforms
    qoutputrange range = {-2.0f*9.80665f, -2.0f*9.80665f, 0.0f}; 
    if(m_accelerationSensor.outputRange()<-1)
        range = m_accelerationSensor.outputRanges()[m_accelerationSensor.outputRange()];    
     
    horizontalSliderX->setRange(range.minimum, range.maximum);
    horizontalSliderY->setRange(range.minimum, range.maximum);
    horizontalSliderZ->setRange(range.minimum, range.maximum);
    m_accelerationSensor.addFilter(this);
    m_accelerationSensor.start();
}

bool AccelerationForm::filter(QAccelerometerReading *reading)
{
    horizontalSliderX->setValue(reading->x());
    labelX->setText(QString("X = %1").arg(reading->x()));
    
    horizontalSliderY->setValue(reading->y());
    labelY->setText(QString("Y = %1").arg(reading->y()));

    horizontalSliderZ->setValue(reading->z());
    labelZ->setText(QString("Z = %1").arg(reading->z()));

    return false;
}
