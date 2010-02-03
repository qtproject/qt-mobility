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

#include "qfeedbackeffect.h"

QTM_BEGIN_NAMESPACE

QFeedbackEffect::QFeedbackEffect()
{
}

QFeedbackEffect::~QFeedbackEffect()
{
}

void QFeedbackEffect::setOwningWindow(QWidget *w)
{
    m_owner = w;
}

QWidget *QFeedbackEffect::owningWindow() const
{
    return m_owner;
}

//the default timeout is 250 (like for the animations
QContinuousEffect::QContinuousEffect() : m_timeout(250), m_intensity(1), m_effect(ContinuousNone)
{
}

QContinuousEffect::~QContinuousEffect()
{
}

EffectType QContinuousEffect::effectType() const
{
    return ContinuousFeedback;
}

void QContinuousEffect::setTimeout(int msecs)
{
    m_timeout = msecs;
}

int QContinuousEffect::timeout() const
{
    return m_timeout;
}

//the intensity. Value should be between 0 and 1
void QContinuousEffect::setIntensity(qreal intensity)
{
    m_intensity = intensity;
}

qreal QContinuousEffect::intensity() const
{
    return m_intensity;
}

void QContinuousEffect::setContinuousEffect(ContinuousEffect effect)
{
    m_effect = effect;
}

ContinuousEffect QContinuousEffect::continuousEffect() const
{
    return m_effect;
}

QInstantEffect::QInstantEffect() : m_effect(InstantNone)
{
}

QInstantEffect::~QInstantEffect()
{
}

EffectType QInstantEffect::effectType() const
{
    return InstantFeedback;
}

void QInstantEffect::setInstantEffect(InstantEffect effect)
{
    m_effect = effect;
}

InstantEffect QInstantEffect::instantEffect() const
{
    return m_effect;
}

QTacticonEffect::QTacticonEffect() : m_effect(TacticonNone)
{
}

QTacticonEffect::~QTacticonEffect()
{
}

EffectType QTacticonEffect::effectType() const
{
    return TacticonFeedback;
}

void QTacticonEffect::setTacticonEffect(TacticonEffect effect)
{
    m_effect = effect;
}

TacticonEffect QTacticonEffect::tacticonEffect() const
{
    return m_effect;
}


QTM_END_NAMESPACE
