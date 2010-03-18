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

#include "freefallform.h"

const int KTreshold = 2;

FreeFallForm::FreeFallForm(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    dropItLabel->setText("");

    m_accelerationSensor.setSignalEnabled(false);
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
    switch (m_freeFallState)
    {
        case StateSetup:
        {
            if (abs(reading->x()) < KTreshold && abs(reading->y()) < KTreshold && abs(reading->z()) < KTreshold)
            {
                m_freeFallTimer.start();
                m_freeFallState = StateFalling;
            }
            break;
        }
        case StateFalling:
        {
            if (abs(reading->x()) > KTreshold || abs(reading->y()) > KTreshold || abs(reading->z()) > KTreshold)
            {
                freeFallTimeLabel->setNum(m_freeFallTimer.elapsed());
                qreal t = (qreal(m_freeFallTimer.elapsed()) / 1000);
                freeFallHeightLabel->setNum(9.81 / 2 * t * t);
                m_freeFallState = StateNotStarted;
                setupButton->setEnabled(true);
                setupButton->setFocus();
                dropItLabel->setText("");
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return false;
}
