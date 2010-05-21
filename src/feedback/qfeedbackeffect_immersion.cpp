/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#include "qfeedbackeffect_p.h"
#include "qfeedbackdevice.h"

#include <ImmVibe.h>


QTM_BEGIN_NAMESPACE

extern VibeInt32 qHandleForDevice(const QFeedbackDevice &device);

static VibeInt32 convertedDuration(int duration)
{
    //this allows to manage the infinite durations
    if (duration == -1)
        return VIBE_TIME_INFINITE;
    return duration;
}

void QFeedbackEffectPrivate::checkUpdateEffect()
{
    if (VIBE_IS_INVALID_EFFECT_HANDLE(m_hEffect))
        return;

    Q_Q(QFeedbackEffect);
    if (period > 0)
        ImmVibeModifyPlayingPeriodicEffect(qHandleForDevice(device),
                                           m_hEffect,
                                           convertedDuration(q->duration()),
                                           intensity / qreal(VIBE_MAX_MAGNITUDE),
                                           period,
                                           VIBE_DEFAULT_STYLE,
                                           attackTime,
                                           attackIntensity,
                                           fadeTime,
                                           fadeIntensity);
    else
        ImmVibeModifyPlayingMagSweepEffect(qHandleForDevice(device),
                                           m_hEffect,
                                           convertedDuration(q->duration()),
                                           intensity / qreal(VIBE_MAX_MAGNITUDE),
                                           VIBE_DEFAULT_STYLE,
                                           attackTime,
                                           attackIntensity,
                                           fadeTime,
                                           fadeIntensity);
}


void QFeedbackEffect::updateCurrentTime(int currentTime)
{
    Q_UNUSED(currentTime);
    Q_D(QFeedbackEffect);

    VibeInt32 effectState = VIBE_EFFECT_STATE_NOT_PLAYING;
    ImmVibeGetEffectState(qHandleForDevice(d->device), d->m_hEffect, &effectState);

    //here we detect changes in the state of the effect
    switch(effectState)
    {
    case VIBE_EFFECT_STATE_NOT_PLAYING:
        stop();
        break;
    case VIBE_EFFECT_STATE_PAUSED:
        pause();
        break;
    case VIBE_EFFECT_STATE_PLAYING:
        start();
        break;
    }

}

void QFeedbackEffect::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    Q_D(QFeedbackEffect);
    switch (newState)
    {
    case Stopped:
        ImmVibeStopPlayingEffect(qHandleForDevice(d->device), d->m_hEffect);
        d->m_hEffect = VIBE_INVALID_EFFECT_HANDLE_VALUE;
        break;
    case Paused:
        ImmVibePausePlayingEffect(qHandleForDevice(d->device), d->m_hEffect);
        break;
    case Running:
        if (oldState == Paused)
            ImmVibeResumePausedEffect(qHandleForDevice(d->device), d->m_hEffect);
        else if (d->period > 0) {
            ImmVibePlayPeriodicEffect(qHandleForDevice(d->device), convertedDuration(duration()),
                intensity() / qreal(VIBE_MAX_MAGNITUDE), period(),
                VIBE_DEFAULT_STYLE, attackTime(), attackIntensity(),
                fadeTime(), fadeIntensity(), &d->m_hEffect);
        } else {
            ImmVibePlayMagSweepEffect(qHandleForDevice(d->device), convertedDuration(duration()),
                intensity() / qreal(VIBE_MAX_MAGNITUDE),
                VIBE_DEFAULT_STYLE, attackTime(), attackIntensity(),
                fadeTime(), fadeIntensity(), &d->m_hEffect);
        }
        break;
    }

    QAbstractAnimation::updateState(newState, oldState);
}

void QFeedbackEffect::setDuration(int msecs)
{
    Q_D(QFeedbackEffect);
    if (d->duration == msecs)
        return;
    d->duration = msecs;
    d->checkUpdateEffect();

}

void QFeedbackEffect::setIntensity(qreal intensity)
{
    Q_D(QFeedbackEffect);
    if (d->intensity == intensity)
        return;
    d->intensity = intensity;
    d->checkUpdateEffect();
}

void QFeedbackEffect::setAttackTime(int msecs)
{
    Q_D(QFeedbackEffect);
    if (d->attackTime == msecs)
        return;
    d->attackTime = msecs;
    d->checkUpdateEffect();
}


void QFeedbackEffect::setAttackIntensity(qreal intensity)
{
    Q_D(QFeedbackEffect);
    if (d->attackIntensity == intensity)
        return;
    d->attackIntensity = intensity;
    d->checkUpdateEffect();
}

void QFeedbackEffect::setFadeTime(int msecs)
{
    Q_D(QFeedbackEffect);
    if (d->fadeTime == msecs)
        return;
    d->fadeTime = msecs;
    d->checkUpdateEffect();
}

void QFeedbackEffect::setFadeIntensity(qreal intensity)
{
    Q_D(QFeedbackEffect);
    if (d->fadeIntensity == intensity)
        return;
    d->fadeIntensity = intensity;
    d->checkUpdateEffect();
}

void QFeedbackEffect::play(InstantEffect effect)
{
    //TODO: we should use the platform/theme APIs for that
    Q_UNUSED(effect);
}



QTM_END_NAMESPACE
