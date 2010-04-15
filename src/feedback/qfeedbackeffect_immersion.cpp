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


static VibeInt32 handle(const QFeedbackDevice &device)
{
    //TODO: make that thread-safe and close the devices afterwards
    static QVector<VibeInt32> handles; //handles to devices

    while (handles.size() <= device.id())
        handles.append(-1); //-1 means no handle

    if (handles.at(device.id()) == -1)
        ImmVibeOpenDevice(device.id(), &handles[device.id()] );


    return handles.at(device.id());
}

class QFeedbackEffectPrivate : public QFeedbackEffectBasePrivate
{
public:
    QFeedbackEffectPrivate() : hEffect(-1)
    {
    }

    void checkUpdateEffect()
    {
        if (state != QFeedbackEffect::Running)
            return;

        Q_Q(QFeedbackEffect);
        ImmVibeModifyPlayingPeriodicEffect(handle(device),
                                           hEffect,
                                           q->totalDuration(),
                                           intensity / qreal(VIBE_MAX_MAGNITUDE),
                                           duration,
                                           VIBE_STYLE_STRONG,
                                           attackTime,
                                           attackIntensity,
                                           fadeTime,
                                           fadeIntensity);
    }

    VibeInt32 hEffect; //handle to the effect

private:
    Q_DECLARE_PUBLIC(QFeedbackEffect);
};

QFeedbackEffect::QFeedbackEffect(QObject *parent) : QAbstractAnimation(parent)
{
}

void QFeedbackEffect::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    Q_D(QFeedbackEffect);
    switch (newState)
    {
    case Stopped:
        ImmVibeStopPlayingEffect(handle(d->device), d->hEffect);
        break;
    case Paused:
        ImmVibePausePlayingEffect(handle(d->device), d->hEffect);
        break;
    case Running:
        ImmVibePlayPeriodicEffect(
            handle(d->device),
            totalDuration(),
            intensity() / qreal(VIBE_MAX_MAGNITUDE),
            duration(),
            VIBE_STYLE_STRONG,
            attackTime(),
            attackIntensity(),
            fadeTime(),
            fadeIntensity(),
            &d->hEffect);
        break;
    }

    QAbstractAnimation::updateState(newState, oldState);
}

int QFeedbackEffect::duration() const
{
    return d_func()->duration;
}


void QFeedbackEffect::setDuration(int msecs)
{
    Q_D(QFeedbackEffect);
    if (msecs < 0) {
        qWarning("QFeedbackEffect::setDuration: cannot set a negative duration");
        return;
    }
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

qreal QFeedbackEffect::intensity() const
{
    return d_func()->intensity;
}

void QFeedbackEffect::setAttackTime(int msecs)
{
    Q_D(QFeedbackEffect);
    if (d->attackTime == msecs)
        return;
    d->attackTime = msecs;
    d->checkUpdateEffect();
}

int QFeedbackEffect::attackTime() const
{
    return d_func()->attackTime;
}

void QFeedbackEffect::setAttackIntensity(qreal intensity)
{
    Q_D(QFeedbackEffect);
    if (d->attackIntensity == intensity)
        return;
    d->attackIntensity = intensity;
    d->checkUpdateEffect();
}

qreal QFeedbackEffect::attackIntensity() const
{
    return d_func()->attackIntensity;
}

void QFeedbackEffect::setFadeTime(int msecs)
{
    Q_D(QFeedbackEffect);
    if (d->fadeTime == msecs)
        return;
    d->fadeTime = msecs;
    d->checkUpdateEffect();
}

int QFeedbackEffect::fadeTime() const
{
    return d_func()->fadeTime;
}

void QFeedbackEffect::setFadeIntensity(qreal intensity)
{
    Q_D(QFeedbackEffect);
    if (d->fadeIntensity == intensity)
        return;
    d->fadeIntensity = intensity;
    d->checkUpdateEffect();
}

qreal QFeedbackEffect::fadeIntensity() const
{
    return d_func()->fadeIntensity;
}

void QFeedbackEffect::setPriority(int priority)
{
    d_func()->priority = priority;
}

int QFeedbackEffect::priority() const
{
    return d_func()->priority;
}

void QFeedbackEffect::setDevice(const QFeedbackDevice &device)
{
    d_func()->device = device;
}

QFeedbackDevice QFeedbackEffect::device() const
{
    return d_func()->device;
}

QTM_END_NAMESPACE


