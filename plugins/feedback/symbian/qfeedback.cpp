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

#include <qfeedbackdevice.h>
#include "qfeedback.h"
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>

Q_EXPORT_PLUGIN2(feedback_symbian, QFeedbackSymbian)

//TODO: is activeWindow good enough
//or should we create a widget for that?
CCoeControl *QFeedbackSymbian::defaultWidget()
{
    QWidget *w = QApplication::activeWindow();
    return w ? w->winId() : 0;
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

QFeedbackDevice QFeedbackSymbian::defaultDevice(QFeedbackDevice::Type t)
{
    return createFeedbackDevice(t);
}

QList<QFeedbackDevice> QFeedbackSymbian::devices()
{
    QList<QFeedbackDevice> ret;
    ret << createFeedbackDevice(QFeedbackDevice::Device);
    if (QTouchFeedback::Instance()->TouchFeedbackSupported()) {
        ret << createFeedbackDevice(QFeedbackDevice::PrimaryDisplay);
    }
    return ret;
}

QString QFeedbackSymbian::deviceName(const QFeedbackDevice &dev)
{
    switch(dev.id())
    {
    case QFeedbackDevice::Device:
        return QLatin1String("Whole Device");
    case QFeedbackDevice::PrimaryDisplay:
        return QLatin1String("Primary Display");
    }
    return QString();
}

QFeedbackDevice::State QFeedbackSymbian::deviceState(const QFeedbackDevice &dev)
{
    QFeedbackDevice::State ret = QFeedbackDevice::Unknown;
    switch(dev.id())
    {
    case QFeedbackDevice::Device:
        switch (vibra()->VibraStatus())
        {
        case CHWRMVibra::EVibraStatusStopped:
            ret = QFeedbackDevice::Ready;
            break;
        case CHWRMVibra::EVibraStatusOn:
            ret = QFeedbackDevice::Busy;
            break;
        default:
            break;
        }
        break;
    case QFeedbackDevice::PrimaryDisplay:
        //there is no way of getting the state of the device!
        break;
    default:
        break;
    }
    return ret;
}

QFeedbackDevice::Capabilities QFeedbackSymbian::supportedCapabilities(const QFeedbackDevice &)
{
    return 0;
}

QFeedbackDevice::Type QFeedbackSymbian::type(const QFeedbackDevice &dev)
{
    return QFeedbackDevice::Type(dev.id());
}

bool QFeedbackSymbian::isEnabled(const QFeedbackDevice &dev)
{
    switch(dev.id())
    {
    case QFeedbackDevice::Device:
        return m_vibraActive;
    case QFeedbackDevice::PrimaryDisplay:
        return QTouchFeedback::Instance()->FeedbackEnabledForThisApp();
    }
    return false;
}

void QFeedbackSymbian::setEnabled(const QFeedbackDevice &dev, bool enabled)
{
    switch(dev.id())
    {
    case QFeedbackDevice::Device:
        m_vibraActive = enabled;
        break;
    case QFeedbackDevice::PrimaryDisplay:
        return QTouchFeedback::Instance()->SetFeedbackEnabledForThisApp(enabled);
    }
}

QFeedbackEffect::ErrorType QFeedbackSymbian::updateEffectProperty(const QFeedbackEffect *effect, EffectProperty prop)
{

    switch(prop)
    {
    case Intensity:
        if (effect->state() != QAbstractAnimation::Running)
            break;

        switch(effect->device().id())
        {
        case QFeedbackDevice::Device:
            vibra()->StartVibraL(effect->duration() - effect->currentTime(), qRound(100 * effect->intensity()));
            break;
        case QFeedbackDevice::PrimaryDisplay:
            QTouchFeedback::Instance()->ModifyFeedback(defaultWidget(), qRound(100 * effect->intensity()));
            break;
        default:
            break;
        }
        break;

    }

    return QFeedbackEffect::NoError;

}

QFeedbackEffect::ErrorType QFeedbackSymbian::updateEffectState(const QFeedbackEffect *effect)
{
    switch(effect->device().id())
    {
    case QFeedbackDevice::Device:
        switch(effect->state())
        {
        case QAbstractAnimation::Stopped:
        case QAbstractAnimation::Paused:
            vibra()->StopVibraL();
            break;
        case QAbstractAnimation::Running:
            vibra()->StartVibraL(effect->duration() - effect->currentTime(), qRound(100 * effect->intensity()));
            break;
        }
        break;
    case QFeedbackDevice::PrimaryDisplay:
        switch(effect->state())
        {
        case QAbstractAnimation::Paused:
        case QAbstractAnimation::Stopped:
            QTouchFeedback::Instance()->StopFeedback(defaultWidget());
            break;
        case QAbstractAnimation::Running:
            QTouchFeedback::Instance()->StartFeedback(defaultWidget(),
                                         0, //what effect
                                         0, qRound(effect->intensity() * 100), qMax(0, effect->duration()*1000));
            break;
        }
        break;
    default:
        break;
    }
    return QFeedbackEffect::NoError;
}

QAbstractAnimation::State QFeedbackSymbian::actualEffectState(const QFeedbackEffect *effect)
{
    return effect->state();
}

void QFeedbackSymbian::play(QFeedbackEffect::InstantEffect effect)
{
    QTouchFeedback::Instance()->InstantFeedback(convertToSymbian(effect));
}

/*
#ifdef ADVANCED_TACTILE_SUPPORT
TTouchContinuousFeedback QFeedbackSymbian::convertToSymbian(QFeedbackEffect::ContinuousEffect effect)
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
*/

TTouchLogicalFeedback QFeedbackSymbian::convertToSymbian(QFeedbackEffect::InstantEffect effect)
{

    TTouchLogicalFeedback instantFeedbackSymbian = ETouchFeedbackBasic;

    switch (effect) {
    case QFeedbackEffect::InstantNone:
        instantFeedbackSymbian = ETouchFeedbackNone;
        break;
    case QFeedbackEffect::InstantBasic:
        instantFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case QFeedbackEffect::InstantSensitive:
        instantFeedbackSymbian = ETouchFeedbackSensitive;
        break;
#ifdef ADVANCED_TACTILE_SUPPORT
    case QFeedbackEffect::InstantBasicButton:
        instantFeedbackSymbian = ETouchFeedbackBasicButton;
        break;
    case QFeedbackEffect::InstantSensitiveButton:
        instantFeedbackSymbian = ETouchFeedbackSensitiveButton;
        break;
    case QFeedbackEffect::InstantBasicItem:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantSensitiveItem:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantBounceEffect:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantPopupOpen:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantPopupClose:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantBasicSlider:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantSensitiveSlider:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantStopFlick:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantFlick:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantEditor:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantTextSelection:
        instantFeedbackSymbian = ETouchFeedbackTextSelection;
        break;
    case QFeedbackEffect::InstantBlankSelection:
        instantFeedbackSymbian = ETouchFeedbackBlankSelection;
        break;
    case QFeedbackEffect::InstantLineSelection:
        instantFeedbackSymbian = ETouchFeedbackLineSelection;
        break;
    case QFeedbackEffect::InstantEmptyLineSelection:
        instantFeedbackSymbian = ETouchFeedbackEmptyLineSelection;
        break;
    case QFeedbackEffect::InstantCheckbox:
        instantFeedbackSymbian = ETouchFeedbackCheckbox;
        break;
    case QFeedbackEffect::InstantMultipleCheckbox:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantSensitiveKeypad:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantBasicKeypad:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantMultitouchActivate:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantRotateStep:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::InstantItemDrop:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ItemMoveOver:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::ItemPick:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect:::ItemScroll:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
    case QFeedbackEffect::PopUp:
        instantFeedbackSymbian = ETouchFeedbackPopUp;
        break;
    case QFeedbackEffect::LongPress:
        instantFeedbackSymbian = ETouchFeedbackBasic; // Effects changing in 10.1 are mapped to basic.
        break;
#endif //ADVANCED_TACTILE_SUPPORT
    default:
        break;
    }
    return instantFeedbackSymbian;
}

