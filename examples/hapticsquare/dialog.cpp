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

#include "dialog.h"
#include "hapticbutton.h"

#include <QGridLayout>
#include <QDebug>

//! [0]
Dialog::Dialog()
{
    m_rumble.setAttackIntensity(0.0);
    m_rumble.setAttackTime(250);
    m_rumble.setIntensity(1.0);
    m_rumble.setDuration(100);
    m_rumble.setFadeTime(250);
    m_rumble.setFadeIntensity(0.0);
//! [0]

//! [1]
    m_ocean.setAttackIntensity(0.0);
    m_ocean.setAttackTime(450);
    m_ocean.setIntensity(0.8);
    m_ocean.setDuration(150);
    m_ocean.setFadeTime(900);
    m_ocean.setFadeIntensity(0.05);
    m_ocean.setPeriod(1500);
//! [1]

//! [2]
    m_btnRumble = new HapticButton(tr("Rumble!"));
    m_btnOcean = new HapticButton(tr("Ocean"));
    m_btnOcean->setCheckable(true);
    m_btnButtonClick = new HapticButton(tr("Click"));
    m_btnNegativeEffect = new HapticButton(tr("Oops!"));
    QGridLayout *topLayout = new QGridLayout(this);
    topLayout->addWidget(m_btnRumble, 0, 0);
    topLayout->addWidget(m_btnOcean, 0, 1);
    topLayout->addWidget(m_btnButtonClick, 1, 0);
    topLayout->addWidget(m_btnNegativeEffect, 1, 1);

    connect(m_btnRumble, SIGNAL(clicked()), this, SLOT(playRumble()));
    connect(m_btnOcean, SIGNAL(toggled(bool)), this, SLOT(playOcean(bool)));
    connect(m_btnButtonClick, SIGNAL(clicked()), this, SLOT(playButtonClick()));
    connect(m_btnNegativeEffect, SIGNAL(clicked()), this, SLOT(playNegativeEffect()));
}
//! [2]

Dialog::~Dialog()
{
    delete m_btnRumble;
    delete m_btnOcean;
    delete m_btnButtonClick;
    delete m_btnNegativeEffect;
}

//! [3]
void Dialog::playRumble()
{
    m_rumble.start();
}

void Dialog::playOcean(bool toggleState)
{
    if (toggleState)
        m_ocean.start();
    else
        m_ocean.stop();
}
//! [3]

//! [4]
void Dialog::playButtonClick()
{
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBasicButton);
}

void Dialog::playNegativeEffect()
{
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeNegativeTacticon);
}
//! [4]

#include "moc_dialog.cpp"

