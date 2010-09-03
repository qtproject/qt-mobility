/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qfeedbackactuator.h>
#include "qfeedback.h"
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>

Q_EXPORT_PLUGIN2(feedback_symbian, QFeedbackSymbian)



#define VIBRA_DEVICE 0
#define TOUCH_DEVICE 1 //this one is only available if ADVANCED_TACTILE_SUPPORT is defined


//TODO: is activeWindow good enough
//or should we create a widget for that?
CCoeControl *QFeedbackSymbian::defaultWidget()
{
    QWidget *w = QApplication::activeWindow();
    return w ? w->winId() : 0;
}

#ifndef NO_TACTILE_SUPPORT
#include <touchfeedback.h>

static MTouchFeedback *touchInstance()
{
    static MTouchFeedback *ret = 0;
    if (!ret) {
        ret = MTouchFeedback::Instance();
        if (!ret)
            ret = MTouchFeedback::CreateInstanceL();
    }
    return ret;
}

static TTouchLogicalFeedback convertToSymbian(QFeedbackEffect::ThemeEffect effect)
{

    TTouchLogicalFeedback themeFeedbackSymbian = ETouchFeedbackBasic;

    switch (effect) {
    case QFeedbackEffect::ThemeBasic:
        themeFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::ThemeSensitive:
        themeFeedbackSymbian = ETouchFeedbackSensitive;
        break;
#ifdef ADVANCED_TACTILE_SUPPORT
    case QFeedbackEffect::ThemeBasicButton:
        themeFeedbackSymbian = ETouchFeedbackBasicButton;
        break;
    case QFeedbackEffect::ThemeSensitiveButton:
        themeFeedbackSymbian = ETouchFeedbackSensitiveButton;
        break;
    case QFeedbackEffect::ThemeBasicItem:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeSensitiveItem:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeBounceEffect:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemePopupOpen:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemePopupClose:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeBasicSlider:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeSensitiveSlider:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeStopFlick:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeFlick:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeEditor:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeTextSelection:
        themeFeedbackSymbian = ETouchFeedbackTextSelection;
        break;
    case QFeedbackEffect::ThemeBlankSelection:
        themeFeedbackSymbian = ETouchFeedbackBlankSelection;
        break;
    case QFeedbackEffect::ThemeLineSelection:
        themeFeedbackSymbian = ETouchFeedbackLineSelection;
        break;
    case QFeedbackEffect::ThemeEmptyLineSelection:
        themeFeedbackSymbian = ETouchFeedbackEmptyLineSelection;
        break;
    case QFeedbackEffect::ThemeCheckBox:
        themeFeedbackSymbian = ETouchFeedbackCheckbox;
        break;
    case QFeedbackEffect::ThemeMultipleCheckBox:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeSensitiveKeypad:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeBasicKeypad:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeMultitouchActivate:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeRotateStep:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeItemDrop:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeItemMoveOver:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeItemPick:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemeItemScroll:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ThemePopUp:
        themeFeedbackSymbian = ETouchFeedbackPopUp;
        break;
    case QFeedbackEffect::ThemeLongPress:
        themeFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
#endif //ADVANCED_TACTILE_SUPPORT
    default:
        break;
    }
    return themeFeedbackSymbian;
}

// This define is for the second parameter of StartFeedback, which needs to be of type
// TTouchContinuousFeedback in platforms with ADVANCED_TACTILE_SUPPORT
#define DEFAULT_CONTINUOUS_EFFECT ETouchContinuousSmooth

bool QFeedbackSymbian::play(QFeedbackEffect::ThemeEffect effect)
{
    touchInstance()->InstantFeedback(convertToSymbian(effect));
    return true; //there is no way to know if there was a failure
}

#endif //NO_TACTILE_SUPPORT

QFeedbackInterface::PluginPriority QFeedbackSymbian::pluginPriority()
{
    return PluginLowPriority;
}

QFeedbackSymbian::QFeedbackSymbian() : m_vibra(0), m_vibraActive(true)
{
}

QFeedbackSymbian::~QFeedbackSymbian()
{
    delete m_vibra;
}

CHWRMVibra *QFeedbackSymbian::vibra()
{
    if (!m_vibra)
        m_vibra = CHWRMVibra::NewL();

    return m_vibra;
}

QList<QFeedbackActuator> QFeedbackSymbian::actuators()
{
    QList<QFeedbackActuator> ret;
#ifdef ADVANCED_TACTILE_SUPPORT
    //if we don't have advanced tactile support then the MTouchFeedback doesn't really support custom effects
    if (touchInstance()->TouchFeedbackSupported()) {
        ret << createFeedbackActuator(TOUCH_DEVICE);
    }
#endif //ADVANCED_TACTILE_SUPPORT
    ret << createFeedbackActuator(VIBRA_DEVICE);
    return ret;
}

void QFeedbackSymbian::setActuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop, const QVariant &value)
{
    switch(prop)
    {
    case Enabled:
        switch(actuator.id())
        {
        case VIBRA_DEVICE:
            m_vibraActive = value.toBool();
            break;
#ifdef ADVANCED_TACTILE_SUPPORT
        case TOUCH_DEVICE:
            touchInstance()->SetFeedbackEnabledForThisApp(value.toBool());
            break;
#endif
        default:
            break;
        }
    }
}

QVariant QFeedbackSymbian::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    switch(prop)
    {
    case Name:
        switch(actuator.id())
        {
        case VIBRA_DEVICE:
            return QLatin1String("Vibra");
        case TOUCH_DEVICE:
            return QLatin1String("Touch");
        default:
            return QString();
        }
    case State:
        {
            QFeedbackActuator::State ret = QFeedbackActuator::Unknown;
            switch(actuator.id())
            {
            case VIBRA_DEVICE:
                switch (vibra()->VibraStatus())
                {
                case CHWRMVibra::EVibraStatusStopped:
                    return QFeedbackActuator::Ready;
                case CHWRMVibra::EVibraStatusOn:
                    return QFeedbackActuator::Busy;
                default:
                    return QFeedbackActuator::Unknown;
                }
            case TOUCH_DEVICE:
                //there is no way of getting the state of the device!
            default:
                return QFeedbackActuator::Unknown;
            }
            return ret;
        }
    case Enabled:
        switch(actuator.id())
        {
        case VIBRA_DEVICE:
            return m_vibraActive;
#ifdef ADVANCED_TACTILE_SUPPORT
        case TOUCH_DEVICE:
            return touchInstance()->FeedbackEnabledForThisApp();
#endif
        default:
            return false;
        }
    default:
        return QVariant();
    }

}

bool QFeedbackSymbian::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability)
{
    return false;
}


void QFeedbackSymbian::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty prop)
{

    switch(prop)
    {
    case Intensity:
        if (!m_elapsed.contains(effect) || m_elapsed[effect].isPaused())
            break;

        switch(effect->actuator().id())
        {
        case VIBRA_DEVICE:
            vibra()->StartVibraL(effect->duration() - m_elapsed[effect].elapsed(), qRound(100 * effect->intensity()));
            break;
#ifdef ADVANCED_TACTILE_SUPPORT
        case TOUCH_DEVICE:
            touchInstance()->ModifyFeedback(defaultWidget(), qRound(100 * effect->intensity()));
            break;
#endif //ADVANCED_TACTILE_SUPPORT
        default:
            break;
        }
        break;

    }
}

void QFeedbackSymbian::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State newState)
{
    switch(effect->actuator().id())
    {
    case VIBRA_DEVICE:
        switch(newState)
        {
        case QFeedbackEffect::Stopped:
            if (m_elapsed.contains(effect)) {
                vibra()->StopVibraL();
                m_elapsed.remove(effect);
            }
            break;
        case QFeedbackEffect::Paused:
            if (m_elapsed.contains(effect)) {
                vibra()->StopVibraL();
                m_elapsed[effect].pause();
            }
            break;
        case QFeedbackEffect::Running:
            if (m_elapsed[effect].elapsed() >= effect->duration())
                m_elapsed.remove(effect); //we reached the end. it's time to restart
            vibra()->StartVibraL(effect->duration() - m_elapsed[effect].elapsed(), qRound(100 * effect->intensity()));
            m_elapsed[effect].start();
            break;
        }
        break;
#ifdef ADVANCED_TACTILE_SUPPORT
    case TOUCH_DEVICE:
        switch(newState)
        {
        case QFeedbackEffect::Stopped:
            if (m_elapsed.contains(effect)) {
                touchInstance()->StopFeedback(defaultWidget());
                m_elapsed.remove(effect);
            }
            break;
        case QFeedbackEffect::Paused:
            if (m_elapsed.contains(effect)) {
                touchInstance()->StopFeedback(defaultWidget());
                m_elapsed[effect].pause();
            }
            break;
        case QFeedbackEffect::Running:
            if (m_elapsed[effect].elapsed() >= effect->duration())
                m_elapsed.remove(effect); //we reached the end. it's time to restart
            touchInstance()->StartFeedback(defaultWidget(),
                                         DEFAULT_CONTINUOUS_EFFECT,
                                         0, qRound(effect->intensity() * 100), qMax(0, (effect->duration() - m_elapsed[effect].elapsed()) * 1000));
            m_elapsed[effect].start();
            break;
        }
        break;
#endif //ADVANCED_TACTILE_SUPPORT
    default:
        break;
    }
}

QFeedbackEffect::State QFeedbackSymbian::effectState(const QFeedbackHapticsEffect *effect)
{
    if (m_elapsed.contains(effect) && m_elapsed[effect].elapsed() < effect->duration()) {

        return m_elapsed[effect].isPaused() ? QFeedbackEffect::Paused : QFeedbackEffect::Running;
    }
    
    return QFeedbackEffect::Stopped;
}
