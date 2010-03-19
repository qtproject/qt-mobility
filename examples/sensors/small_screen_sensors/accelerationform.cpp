/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "accelerationform.h"

AccelerationForm::AccelerationForm(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    m_accelerationSensor.connect();
    
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
