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

#include "qfeedbackeffect.h"
#include "qfeedbackeffect_p.h"
#include "qfeedbackplugininterfaces.h"

#include <QtCore>
#include <QDebug>


QTM_BEGIN_NAMESPACE


/*!
    \class QFeedbackEffect
    \brief The QFeedbackEffect class is the abstract base class for the feedback framework.
    \inmodule QtFeedback

    It has the concepts of duration and state. 
*/

/*!
    \enum QFeedbackEffect::State

    This enum describes the state of the effect.

    \value Stopped The feedback is not running. This is the initial state.
    The state changes to either Loading when loading a feedback or
    to Running when the feedback is started by calling start().

    \value Paused The feedback is paused. Calling start() will resume it.

    \value Running The feedback is running. You can control the current state
    by calling the state() function.

    \value Loading The feedback is loading. That can happen when loading
    is done asynchronously.

    \sa state()
*/

/*!
    \enum QFeedbackEffect::ErrorType

    This enum describes the possible errors happening on the effect.

    \value UnknownError An unknown error occurred.

    \value DeviceBusy The feedback could not start because the device is busy.

    \sa error()
*/

/*!
    \enum QFeedbackEffect::ThemeEffect

    This enum describes all possible theme effect types.

    \value ThemeBasic
    \value ThemeSensitive
    \value ThemeBasicButton
    \value ThemeSensitiveButton
    \value ThemeBasicKeypad
    \value ThemeSensitiveKeypad
    \value ThemeBasicSlider
    \value ThemeSensitiveSlider
    \value ThemeBasicItem
    \value ThemeSensitiveItem
    \value ThemeItemScroll
    \value ThemeItemPick
    \value ThemeItemDrop
    \value ThemeItemMoveOver
    \value ThemeBounceEffect
    \value ThemeCheckBox
    \value ThemeMultipleCheckBox
    \value ThemeEditor
    \value ThemeTextSelection
    \value ThemeBlankSelection
    \value ThemeLineSelection
    \value ThemeEmptyLineSelection
    \value ThemePopUp
    \value ThemePopupOpen
    \value ThemePopupClose
    \value ThemeFlick
    \value ThemeStopFlick  PopUp -> Popup
    \value ThemeMultitouchActivate
    \value ThemeRotateStep
    \value ThemeLongPress
    \value ThemePositiveTacticon
    \value ThemeNeutralTacticon
    \value ThemeNegativeTacticon
    \value NumberOfThemeEffects
    \value ThemeUser
 */

/*!
    \enum QFeedbackEffect::Duration
    This enum describes the possible effect duration types.

    \value INFINITE Infinite effect duration
  */

/*!
    \property QFeedbackEffect::state
    \brief state of the feedback.

    It has a default value of Stopped. Each time you call the state() the backend
    is called to get the actual feedback effect state.
*/

/*!
    \property QFeedbackEffect::duration
    \brief duration of the feedback.
*/

/*!
    Constructs the QFeedbackEffect base class, and passes \a parent to
    QObject's constructor.
*/
QFeedbackEffect::QFeedbackEffect(QObject *parent) : QObject(parent)
{
}

/*
    \fn void QFeedbackEffect::start()

    Starts playing the effect. If an error occurs there the
    error signal will be emitted.
*/
void QFeedbackEffect::start()
{
    setState(Running);
}

/*
    \fn void QFeedbackEffect::stop()

    Stops a playing effect. If an error occurs there the
    error signal will be emitted.

    \sa play(), pause(), setState()
*/
void QFeedbackEffect::stop()
{
    setState(Stopped);
}

/*
    \fn void QFeedbackEffect::pause()

    Pauses a playing effect. If an error occurs there the
    error signal will be emitted.
*/
void QFeedbackEffect::pause()
{
    setState(Paused);
}

/*!
    \fn QFeedbackEffect::playThemeEffect(ThemeEffect effect)
    plays instant feedback and return true if the effect could be played.

    That feedback is defined by the theme of the system.
*/
bool QFeedbackEffect::playThemeEffect(ThemeEffect effect)
{
    if (QFeedbackThemeInterface *iface = QFeedbackThemeInterface::instance())
        return iface->play(effect);
    return false;
}

/*!
    \fn QFeedbackEffect::supportsThemeEffect()

    Returns if playing themed feedback is available.

*/
bool QFeedbackEffect::supportsThemeEffect()
{
    return QFeedbackThemeInterface::instance() != 0;
}


/*!
    \class QFeedbackHapticsEffect
    \brief The QFeedbackHapticsEffect class allows to play a haptics feedback on an actuator.

    It is possible to set the duration, intensity, envelope and period of the effect.
    It is a subclass of QFeedbackEffect (subclass of QObject), which makes it 
    inherit its properties: duration and state.
    It can also be started, stopped or paused.

    You can set the duration to INFINITE. It is then up to the program to stop the effect.

    A feedback effect always works on a feedback actuator.

    it can report errors through the error signal.

    \sa QFeedbackActuator
*/

/*!
    Constructs the QFeedbackHatpicsEffect class, and passes \a parent to
    QObject's constructor.
*/
QFeedbackHapticsEffect::QFeedbackHapticsEffect(QObject *parent) : QFeedbackEffect(parent), priv(new QFeedbackHapticsEffectPrivate)
{
}


/*!
    Stops the feedback if it is running.
*/
QFeedbackHapticsEffect::~QFeedbackHapticsEffect()
{
    stop();
}

/*!
    \property QFeedbackHapticsEffect::duration
    \brief the expected duration of the effect.

    This property defines the duration of the feedback effect.
*/
int QFeedbackHapticsEffect::duration() const
{
    return priv->duration;
}
void QFeedbackHapticsEffect::setDuration(int msecs)
{
    if (priv->duration == msecs)
        return;
    priv->duration = msecs;
    QFeedbackHapticsInterface::instance()->updateEffectProperty(this, QFeedbackHapticsInterface::Duration);
}

/*!
    \property QFeedbackHapticsEffect::intensity
    \brief the intensity of the effect.

    This property defines the intensity of the feedback effect.
    The value can be between 0. and 1.
*/
qreal QFeedbackHapticsEffect::intensity() const
{
    return priv->intensity;
}
void QFeedbackHapticsEffect::setIntensity(qreal intensity)
{
    if (priv->intensity == intensity)
        return;
    priv->intensity = intensity;
    QFeedbackHapticsInterface::instance()->updateEffectProperty(this, QFeedbackHapticsInterface::Intensity);
}

/*!
    \property QFeedbackHapticsEffect::attackTime
    \brief the duration of the fade-in effect.

    This property defines the duration of the fade-in effect in msecs.
*/
int QFeedbackHapticsEffect::attackTime() const
{
    return priv->attackTime;
}
void QFeedbackHapticsEffect::setAttackTime(int msecs)
{
    if (priv->attackTime == msecs)
        return;
    priv->attackTime = msecs;
    QFeedbackHapticsInterface::instance()->updateEffectProperty(this, QFeedbackHapticsInterface::AttackTime);
}

/*!
    \property QFeedbackHapticsEffect::attackIntensity
    \brief the initial intensity of the effect.

    This property defines the initial intensity of the effect, before it fades in.
    It is usually lower than intensity.
*/
qreal QFeedbackHapticsEffect::attackIntensity() const
{
    return priv->attackIntensity;
}
void QFeedbackHapticsEffect::setAttackIntensity(qreal intensity)
{
    if (priv->attackIntensity == intensity)
        return;
    priv->attackIntensity = intensity;
    QFeedbackHapticsInterface::instance()->updateEffectProperty(this, QFeedbackHapticsInterface::AttackIntensity);
}

/*!
    \property QFeedbackHapticsEffect::fadeTime
    \brief the duration of the fade-out effect.

    This property defines the duration of the fade-out effect in msecs.
*/
int QFeedbackHapticsEffect::fadeTime() const
{
    return priv->fadeTime;
}
void QFeedbackHapticsEffect::setFadeTime(int msecs)
{
    if (priv->fadeTime == msecs)
        return;
    priv->fadeTime = msecs;
    QFeedbackHapticsInterface::instance()->updateEffectProperty(this, QFeedbackHapticsInterface::FadeTime);
}

/*!
    \property QFeedbackHapticsEffect::fadeIntensity
    \brief the final intensity of the effect.

    This property defines the final intensity of the effect, after it fades out.
    It is usually lower than intensity.
*/
qreal QFeedbackHapticsEffect::fadeIntensity() const
{
    return priv->fadeIntensity;
}
void QFeedbackHapticsEffect::setFadeIntensity(qreal intensity)
{
    if (priv->fadeIntensity == intensity)
        return;
    priv->fadeIntensity = intensity;
    QFeedbackHapticsInterface::instance()->updateEffectProperty(this, QFeedbackHapticsInterface::FadeIntensity);
}

/*!
    \property QFeedbackHapticsEffect::actuator
    \brief the actuator on which the effect operates.

    This property defines the actuator on which the effect operates.
*/
QFeedbackActuator QFeedbackHapticsEffect::actuator() const
{
    return priv->actuator;
}
void QFeedbackHapticsEffect::setActuator(const QFeedbackActuator &actuator)
{
    if (state() != Stopped) {
        qWarning("QFeedbackHapticsEffect::setActuator: The effect is not stopped");
        return;
    }

    priv->actuator = actuator;
}

/*!
    \property QFeedbackHapticsEffect::period
    \brief set the period for the effect.

    It has a default value of -1, which mean that it is not a periodic effect.
    Note: not all actuators support periodic effects
*/
int QFeedbackHapticsEffect::period() const
{
    return priv->period;
}
void QFeedbackHapticsEffect::setPeriod(int msecs)
{
    if (state() != Stopped) {
        qWarning("QFeedbackHapticsEffect::setPeriod: the period can only be changed if the effect is stopped");
        return;
    }
    priv->period = msecs;
}

/*
\reimp
*/
void QFeedbackHapticsEffect::setState(State state)
{
    QFeedbackHapticsInterface::instance()->setEffectState(this, state);
}

/*
\reimp
*/
QFeedbackEffect::State QFeedbackHapticsEffect::state() const
{
    return QFeedbackHapticsInterface::instance()->effectState(this);
}

/*!
    \class QFeedbackFileEffect
    \brief The QFeedbackFileEffect class allows to play a haptics feedback from a file.

    You can load and unload the file at will to free resources or be as fast as possible.
    It is a subclass of QFeedbackEffect (subclass of QObject), which makes it 
    inherit its properties: duration and state.
    It can also be started, stopped or paused.

    it can report errors through the error signal.
*/


/*!
    \internal
*/
void QFeedbackFileEffectPrivate::loadFinished(bool success)
{
    loaded = success;
    if( !success)
        backendUsed = -1;
}


/*!
    Constructs the QFeedbackFileEffect class, and passes \a parent to
    QObject's constructor.
*/
QFeedbackFileEffect::QFeedbackFileEffect(QObject *parent) : QFeedbackEffect(parent), priv(new QFeedbackFileEffectPrivate(this))
{
}

/*!
    Stops the feedback and  unloads the file if necessary.
*/
QFeedbackFileEffect::~QFeedbackFileEffect()
{
    setLoaded(false); //ensures we unload the file and frees resources
}

/*
    \reimp
*/
int QFeedbackFileEffect::duration() const
{
    return QFeedbackFileInterface::instance()->effectDuration(this);
}

/*!
    \property QFeedbackFileEffect::fileName
    \brief the name of the file that is loaded.

    Setting that property will automatically unload the previous file and load the new one.
*/
QString QFeedbackFileEffect::fileName() const
{
    return priv->fileName;
}
void QFeedbackFileEffect::setFileName(const QString &fileName)
{
    if (state() != QFeedbackEffect::Stopped) {
        qWarning("QFeedbackFileEffect::setFileName: can't set the file while the feedback is running");
        return;
    }
    setLoaded(false);
    priv->fileName = fileName;
    setLoaded(true);
}

/*!
    \property QFeedbackFileEffect::loaded
    \brief determines if the file has been successfully loaded.
*/
bool QFeedbackFileEffect::isLoaded() const
{
    return priv->loaded;
}
void QFeedbackFileEffect::setLoaded(bool load)
{
    if (priv->loaded == load)
        return;

    if (state() != QFeedbackEffect::Stopped) {
        qWarning() << "QFeedbackFileEffect::setLoaded: can't load/unload a file while the effect is not stopped";
        return;
    }

    QFeedbackFileInterface::instance()->setLoaded(this, load);
}


/*!
    \fn void QFeedbackFileEffect::load()

    makes sure that the file associated with the feedback object is loaded.
    It will be automatically loaded when setFileName or start functions
    are called.
*/
void QFeedbackFileEffect::load()
{
    setLoaded(true);
}

/*!
    \fn void QFeedbackFileEffect::unload()

    makes sure that the file associated with the feedback object is unloaded.
    It will be automatically loaded when the setFileName function is called with
    another file or the object is destructed.
*/
void QFeedbackFileEffect::unload()
{
    setLoaded(false);
}


/*!
    \fn QStringList QFeedbackFileEffect::supportedMimeTypes()

    returns the MIME types supported for playing effects from file.
*/
QStringList QFeedbackFileEffect::supportedMimeTypes()
{
    return QFeedbackFileInterface::instance()->supportedMimeTypes();
}


/*
\reimp
*/
void QFeedbackFileEffect::setState(State newState)
{
    if (newState != Stopped && state() == Stopped)
        load(); // makes sure the file is loaded
    QFeedbackFileInterface::instance()->setEffectState(this, newState);
}

/*
\reimp
*/
QFeedbackEffect::State QFeedbackFileEffect::state() const
{
    return QFeedbackFileInterface::instance()->effectState(this);
}

#include "moc_qfeedbackeffect.cpp"

QTM_END_NAMESPACE
