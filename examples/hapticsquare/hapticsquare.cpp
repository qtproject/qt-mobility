/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "hapticsquare.h"
#include "hapticbutton.h"

#include <QGridLayout>
#include <QDebug>

//! [0]
HapticSquare::HapticSquare()
{
    m_rumble.setAttackIntensity(0.1);
    m_rumble.setAttackTime(250);
    m_rumble.setIntensity(1.0);
    m_rumble.setDuration(1000);
    m_rumble.setFadeTime(250);
    m_rumble.setFadeIntensity(0.1);
//! [0]

//! [1]
    m_ocean.setAttackIntensity(0.1);
    m_ocean.setAttackTime(450);
    m_ocean.setIntensity(0.8);
    m_ocean.setDuration(6000);
    m_ocean.setFadeTime(900);
    m_ocean.setFadeIntensity(0.05);
    m_ocean.setPeriod(1500);
//! [1]

//! [2]
    m_btnRumble = new HapticButton(tr("Rumble!"));
    m_btnOcean = new HapticButton(tr("Ocean"));
    m_btnButtonClick = new HapticButton(tr("Click"));
    m_btnNegativeEffect = new HapticButton(tr("Oops!"));
    QGridLayout *topLayout = new QGridLayout(this);
    topLayout->addWidget(m_btnRumble, 0, 0);
    topLayout->addWidget(m_btnOcean, 0, 1);
    topLayout->addWidget(m_btnButtonClick, 1, 0);
    topLayout->addWidget(m_btnNegativeEffect, 1, 1);

    connect(m_btnRumble, SIGNAL(clicked()), this, SLOT(playRumble()));
    connect(m_btnOcean, SIGNAL(clicked()), this, SLOT(playOcean()));
    connect(m_btnButtonClick, SIGNAL(clicked()), this, SLOT(playButtonClick()));
    connect(m_btnNegativeEffect, SIGNAL(clicked()), this, SLOT(playNegativeEffect()));
}
//! [2]

HapticSquare::~HapticSquare()
{
    delete m_btnRumble;
    delete m_btnOcean;
    delete m_btnButtonClick;
    delete m_btnNegativeEffect;
}

//! [3]
void HapticSquare::playRumble()
{
    m_rumble.start();
}

void HapticSquare::playOcean()
{
    if (m_ocean.state() == QFeedbackEffect::Stopped) {
        m_ocean.start();
    } else {
        m_ocean.stop();
    }
}
//! [3]

//! [4]
void HapticSquare::playButtonClick()
{
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBasicButton);
}

void HapticSquare::playNegativeEffect()
{
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeNegativeTacticon);
}
//! [4]

#include "moc_hapticsquare.cpp"
