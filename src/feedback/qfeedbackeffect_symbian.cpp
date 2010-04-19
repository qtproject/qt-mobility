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

#include <QtGui/QApplication>
#include <QtGui/QWidget>

#include "qfeedbackeffect.h"
#include "qfeedbackeffect_p.h"

#include <hwrmvibra.h>
#include <touchfeedback.h>

QTM_BEGIN_NAMESPACE

static CCoeControl *defaultWidget()
{
    QWidget *w = QApplication::activeWindow();
    return w ? w->winId() : 0;

}

#ifdef ADVANCED_TACTILE_SUPPORT
static TTouchContinuousFeedback convertToSymbian(ContinuousEffect effect)
{
    TTouchContinuousFeedback continuousFeedbackSymbian = ETouchContinuousSmooth;
    switch (effect) {
        case ContinuousSmooth:
            continuousFeedbackSymbian = ETouchContinuousSmooth;
            break;
        case ContinuousSlider:
            continuousFeedbackSymbian = ETouchContinuousSlider;
            break;
        case ContinuousPopup:
            continuousFeedbackSymbian = ETouchContinuousSmooth;
            break;
        case ContinuousInput:
            continuousFeedbackSymbian = ETouchContinuousSmooth;
            break;
        case ContinuousPinch:
            continuousFeedbackSymbian = ETouchContinuousSmooth;
            break;
        default:
            break;
    }
    return continuousFeedbackSymbian;
}

#endif

static TTouchLogicalFeedback convertToSymbian(InstantEffect effect)
{
    TTouchLogicalFeedback instantFeedbackSymbian = ETouchFeedbackBasic;
    switch (effect) {
    case InstantNone:
        instantFeedbackSymbian = ETouchFeedbackNone;
        break;
    case InstantBasic:
        instantFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case InstantSensitive:
        instantFeedbackSymbian = ETouchFeedbackSensitive;
        break;
#ifdef ADVANCED_TACTILE_SUPPORT
    case InstantBasicButton:
        instantFeedbackSymbian = ETouchFeedbackBasicButton;
        break;
    case InstantSensitiveButton:
        instantFeedbackSymbian = ETouchFeedbackSensitiveButton;
        break;
    case InstantBounceEffect:
        instantFeedbackSymbian = ETouchFeedbackBoundaryList;
        break;
    case InstantPopupOpen:
        instantFeedbackSymbian = ETouchFeedbackIncreasingPopUp;
        break;
    case InstantPopupClose:
        instantFeedbackSymbian = ETouchFeedbackDecreasingPopUp;
        break;
    case InstantBasicSlider:
        instantFeedbackSymbian = ETouchFeedbackSlider;
        break;
    case InstantSensitiveSlider:
        instantFeedbackSymbian = ETouchFeedbackSlider;
        break;
    case InstantCheckbox:
        instantFeedbackSymbian = ETouchFeedbackCheckbox;
        break;
#endif
    default:
        break;
    }
    return instantFeedbackSymbian;
}

#ifdef ADVANCED_TACTILE_SUPPORT
typedef QTouchFeedback MTouchFeedback;
#else
class QTouchFeedback : public MTouchFeedback
{
public:

    static QTouchFeedback *Instance()
    {
        return static_cast<QTouchFeedback*>(MTouchFeedback::Instance());
    }

    void StopFeedback(const CCoeControl* dummy)
    {
        Q_UNUSED(dummy);
    }

    void ModifyFeedback(const CCoeControl* dummy, int intensity)
    {
        Q_UNUSED(dummy);
        Q_UNUSED(intensity);
    }

    void StartFeedback(const CCoeControl* dummy, int effect, void *event, int intensity, int duration)
    {
        Q_UNUSED(dummy);
        Q_UNUSED(effect);
        Q_UNUSED(event);
        Q_UNUSED(intensity);
        Q_UNUSED(duration);
        //if there is no advanced feedback, we just do the normal one and return -1
        MTouchFeedback::Instance()->InstantFeedback(ETouchFeedbackSensitive);
    }
};
#endif

QTouchFeedback *QFeedbackEffectPrivate::feedback()
{
    if (!m_feedback)
        m_feedback = QTouchFeedback::Instance();
    return m_feedback;
}

CHWRMVibra *QFeedbackEffectPrivate::vibra()
{
    if (!m_vibra)
        m_vibra = CHWRMVibra::NewL();
    return m_vibra;
}

void QFeedbackEffect::updateCurrentTime(int currentTime)
{
    Q_UNUSED(currentTime);
    //no random access for feedback
}

void QFeedbackEffect::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    Q_UNUSED(oldState);
    Q_D(QFeedbackEffect);
    switch(d->device.id())
    {
    case QFeedbackDevice::Vibra:
        switch(newState)
        {
        case Stopped:
        case Paused:
            d->vibra()->StopVibraL();
            break;
        case Running:
            d->vibra()->StartVibraL(d->duration - d->currentTime, qRound(100 * d->intensity));
            break;
        }
        break;

        break;
    case QFeedbackDevice::Touch:
        switch(newState)
        {
        case Stopped:
            d->feedback()->StopFeedback(defaultWidget()); //TODO: is activeWindow good enough?
            break;
        case Paused:
            //not supported, we call stop
            stop();
            break;
        case Running:
            d->feedback()->StartFeedback(defaultWidget(), //we apparently need a widget
                                         0, //what effect
                                         0, qRound(d->intensity * 100), d->duration);
            break;
        }
        break;
    default:
        break;
    }
}

void QFeedbackEffect::setDuration(int msecs)
{
    d_func()->duration = msecs;
}

void QFeedbackEffect::setIntensity(qreal intensity)
{
    Q_D(QFeedbackEffect);
    if (d->intensity == intensity)
        return;

    d->intensity = intensity;
    if (state() != Running)
        return;

    switch(d->device.id())
    {
    case QFeedbackDevice::Vibra:
        d->vibra()->StartVibraL(d->duration - d->currentTime, qRound(100 * intensity));
        break;
    case QFeedbackDevice::Touch:
        d->feedback()->ModifyFeedback(defaultWidget(), qRound(100 * intensity));
        break;
    default:
        break;
    }

}

//envelope is not supported on S60
void QFeedbackEffect::setAttackTime(int msecs)
{
    Q_UNUSED(msecs);
}

void QFeedbackEffect::setAttackIntensity(qreal intensity)
{
    Q_UNUSED(intensity);
}

void QFeedbackEffect::setFadeTime(int msecs)
{
    Q_UNUSED(msecs);
}

void QFeedbackEffect::setFadeIntensity(qreal intensity)
{
    Q_UNUSED(intensity);
}

QTM_END_NAMESPACE
