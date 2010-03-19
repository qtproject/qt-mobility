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

#include "orientationform.h"

OrientationForm::OrientationForm(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(&m_orientation, SIGNAL(readingChanged()), this, SLOT(orientationChanged()));
  
    // Genreic attitude sensor does't give output ranges
    // Copy-pasted from documentation
    horizontalSliderX->setRange(-90, 90);
    horizontalSliderY->setRange(-180, 180);
    horizontalSliderZ->setRange(0, 360);
    
    m_attitude.setSignalEnabled(false);
    m_attitude.addFilter(this);
    m_attitude.start();
    m_orientation.start();    
}

bool OrientationForm::filter(QAttitudeReading *reading)
{
    horizontalSliderX->setValue(reading->pitch());
    labelX->setText(QString("pitch = %1").arg(reading->pitch()));
    
    horizontalSliderY->setValue(reading->roll());
    labelY->setText(QString("roll = %1").arg(reading->roll()));

    horizontalSliderZ->setValue(reading->yaw());
    labelZ->setText(QString("yaw = %1").arg(reading->yaw()));
    
    return false;
}

void OrientationForm::orientationChanged()
{
    label->setText(mapOrientationToString(m_orientation.reading()));
}

QString OrientationForm::mapOrientationToString(QOrientationReading *reading)
{
    switch(reading->orientation())
    {
        case QOrientationReading::TopUp:
            return tr("Top up");
        case QOrientationReading::TopDown:
            return tr("Top down");
        case QOrientationReading::LeftUp:
            return tr("Left up");
        case QOrientationReading::RightUp:
            return tr("Right up");
        case QOrientationReading::FaceUp:
            return tr("Face up");
        case QOrientationReading::FaceDown:
            return tr("Face down");
        default:
            return tr("Undefined");
    }
}
