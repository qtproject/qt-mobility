/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qfeedbackactuator.h>
#include "qfeedback_symbian.h"
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>

#ifdef HAS_THEME_EFFECTS
#include <touchfeedback.h>
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
// This define is for the second parameter of StartFeedback, which needs to be of type
// TTouchContinuousFeedback in platforms with HAS_ADVANCED_TACTILE_SUPPORT
#define DEFAULT_CONTINUOUS_EFFECT ETouchContinuousSmooth
#endif //HAS_ADVANCED_TACTILE_SUPPORT
#endif // HAS_THEME_EFFECTS

#ifdef USE_CHWRMVIBRA_PLZ
#include <hwrmvibra.h>
#endif //USE_CHWRMVIBRA_PLZ

CHWRMVibra *QFeedbackSymbian::vibra()
{
    if (!m_vibra)
        QT_TRAP_THROWING(m_vibra = CHWRMVibra::NewL());

    return m_vibra;
}

void QFeedbackSymbian::setActuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop, const QVariant &value)
{
    switch (prop)
    {
    case Enabled:
        switch (actuator.id())
        {
        case VIBRA_DEVICE:
            m_vibraActive = value.toBool();
            break;
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
        case TOUCH_DEVICE:
            TRAP_IGNORE(touchInstance()->SetFeedbackEnabledForThisApp(value.toBool()));
            break;
#endif //HAS_ADVANCED_TACTILE_SUPPORT
        default:
            break;
        }
    }
}

QVariant QFeedbackSymbian::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    switch (prop)
    {
    case Name:
        switch (actuator.id())
        {
        case VIBRA_DEVICE:
            return QLatin1String("Vibra");
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
        case TOUCH_DEVICE:
            return QLatin1String("Touch");
#endif //HAS_ADVANCED_TACTILE_SUPPORT
        default:
            return QString();
        }
    case State:
        {
            QFeedbackActuator::State ret = QFeedbackActuator::Unknown;
            switch (actuator.id())
            {
            case VIBRA_DEVICE:
                {
                    TInt status = 0;
                    TRAP_IGNORE(status = vibra()->VibraStatus());
                    switch (status)
                    {
                    case CHWRMVibra::EVibraStatusStopped:
                        return QFeedbackActuator::Ready;
                    case CHWRMVibra::EVibraStatusOn:
                        return QFeedbackActuator::Busy;
                    default:
                        return QFeedbackActuator::Unknown;
                    }
                }
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
            case TOUCH_DEVICE:
                //there is no way of getting the state of the device!
#endif //HAS_ADVANCED_TACTILE_SUPPORT
            default:
                return QFeedbackActuator::Unknown;
            }
            return ret;
        }
    case Enabled:
        switch (actuator.id())
        {
        case VIBRA_DEVICE:
            return m_vibraActive;
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
        case TOUCH_DEVICE:
            {
                bool ret = false;
                TRAP_IGNORE(ret = touchInstance()->FeedbackEnabledForThisApp());
                return ret;
            }
#endif //HAS_ADVANCED_TACTILE_SUPPORT
        default:
            return false;
        }
    default:
        return QVariant();
    }

}

bool QFeedbackSymbian::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability cap)
{
    switch(cap)
    {
    case QFeedbackActuator::Envelope:
#ifdef HAS_ENVELOPE_SUPPORT
        return true;
#else
        return false;
#endif //HAS_ENVELOPE_SUPPORT
    case QFeedbackActuator::Period:
        return true;
    default:
        break;
    }
    return false;
}


void QFeedbackSymbian::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty prop)
{
    if (!effect->actuator()) {
        return;
    }
    TInt err = KErrNone;
    switch (prop)
    {
    case Intensity:
        if (!m_elapsed.contains(effect) || m_elapsed[effect].isPaused())
            break;

        switch (effect->actuator()->id())
        {
        case VIBRA_DEVICE:
            TRAP(err, vibra()->StartVibraL(effect->duration() - m_elapsed[effect].elapsed(), qRound(100 * effect->intensity())));
            break;
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
        case TOUCH_DEVICE:
            TRAP(err, touchInstance()->ModifyFeedback(defaultWidget(), qRound(100 * effect->intensity())));
            break;
#endif //HAS_ADVANCED_TACTILE_SUPPORT
        default:
            break;
        }
        break;

    }
    if (err != KErrNone)
        reportError(effect, QFeedbackEffect::UnknownError);
}

void QFeedbackSymbian::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State newState)
{
    TInt err = KErrNone;
    if (!effect->actuator()) {
        return;
    }
    switch (effect->actuator()->id())    {
    case VIBRA_DEVICE:
        switch (newState)
        {
        case QFeedbackEffect::Stopped:
            if (m_elapsed.contains(effect)) {
                TRAP(err, vibra()->StopVibraL());
                m_elapsed.remove(effect);
            }
            break;
        case QFeedbackEffect::Paused:
            if (m_elapsed.contains(effect)) {
                TRAP(err, vibra()->StopVibraL());
                m_elapsed[effect].pause();
            }
            break;
        case QFeedbackEffect::Running:
            if (m_elapsed[effect].elapsed() >= effect->duration())
                m_elapsed.remove(effect); //we reached the end. it's time to restart
            int millis = effect->duration() - m_elapsed[effect].elapsed();
            TRAP(err, vibra()->StartVibraL(millis, qRound(100 * effect->intensity())));
            m_elapsed[effect].start(effect, millis);
            break;
        }
        break;
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
    case TOUCH_DEVICE:
        switch (newState)
        {
        case QFeedbackEffect::Stopped:
            if (m_elapsed.contains(effect)) {
                TRAP(err, touchInstance()->StopFeedback(defaultWidget()));
                m_elapsed.remove(effect);
            }
            break;
        case QFeedbackEffect::Paused:
            if (m_elapsed.contains(effect)) {
                TRAP(err, touchInstance()->StopFeedback(defaultWidget()));
                m_elapsed[effect].pause();
            }
            break;
        case QFeedbackEffect::Running:
            if (m_elapsed[effect].elapsed() >= effect->duration())
                m_elapsed.remove(effect); //we reached the end. it's time to restart
            int millis = effect->duration() - m_elapsed[effect].elapsed();
            TRAP(err, touchInstance()->StartFeedback(defaultWidget(),
                                         DEFAULT_CONTINUOUS_EFFECT,
                                         0, qRound(effect->intensity() * 100), qMax(0, millis * 1000)));
            m_elapsed[effect].start(effect, millis);
            break;
        }
        break;
#endif //HAS_ADVANCED_TACTILE_SUPPORT
    default:
        break;
    }
    if (err != KErrNone)
        reportError(effect, QFeedbackEffect::UnknownError);
}

QFeedbackEffect::State QFeedbackSymbian::effectState(const QFeedbackHapticsEffect *effect)
{
    if (m_elapsed.contains(effect) && m_elapsed[effect].elapsed() < effect->duration()) {

        if (m_elapsed[effect].isPaused())
            return QFeedbackEffect::Paused;
        // If there is a timer, and it's elapsed, it should be stopped
        if (m_elapsed[effect].isTimerActive())
            return QFeedbackEffect::Running;
        // Otherwise, the timer has elapsed or never started, so fall through to Stopped
    }

    return QFeedbackEffect::Stopped;
}
