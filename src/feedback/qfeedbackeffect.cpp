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
    \brief The QFeedbackEffect class is the abstract base class for feedback effects.
    \ingroup feedback
    \inmodule QtFeedback

    It represents a tactile feedback effect (i.e., an effect that
    affect human senses). The technology available today usually only
    provides haptic effects, which deal with the sense of touch. The
    QFeedbackHapticsEffect and QFeedbackFileEffect are implementations
    of haptic effects and can be used to control a mobile device's
    vibrator.

    Feedback effects have a duration, which is measured in
    milliseconds. Subclasses reimplement duration() to inform how long
    the effect lasts.

    \omit There is no way to get informed of state changes? \endomit
    At any given time, a feedback effect is in one of four states:
    Loading, Stopped, Running, or Paused. See the
    \l{QFeedbackEffect::}{State} enum documentation for further
    details. Subclasses must reimplement state() to report which
    state an effect is in, and setState() to receive state change
    requests. The start(), pause(), and stop() slots calls
    setState() with the corresponding new \l{QFeedbackEffect::}{State}.

    A system often has a set of standard feedback effects for user
    interface interaction (e.g., button clicks). The
    \l{QFeedbackEffect::}{ThemeEffect} describes the standard effects
    that QFeedbackEffect supports. It is named so because the effects
    often depend on the theme of the user interface. You can play
    these effects using the playThemeEffect() function.

    \code
        QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBasicButton);
    \endcode

    The playThemeEffect() function returns true if the effect was
    played successfully. An effect may not be played if the system does
    not support it or if an error occurred.
*/

/*!
    \fn void QFeedbackEffect::error(QFeedbackEffect::ErrorType error) const

    This signal is emitted by subclasses if an \a error occurred during
    playback of an effect. The \l{QFeedbackEffect::}{ErrorType} enum
    describes the errors that can be reported.

*/

/*!
    \enum QFeedbackEffect::State

    This enum describes the state of the effect. An effect will be in
    one of these states.

    \value Stopped The effect is not running. This is the initial
    state. The state changes to either Loading when loading an effect
    or to Running when the effect is started by calling start().  When
    an effect has finished playing, it will enter the Stopped state
    again.

    \value Paused The effect is paused. Calling start() will resume it.

    \value Running The effect is running. You can control the current state
    by calling the state() function.

    \value Loading The effect is loading. That can happen when loading
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
    \value ThemeStopFlick
    \value ThemeMultiPointTouchActivate
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

    \value QFeedbackEffect::Duration::INFINITE Infinite effect duration
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

/*!
    \fn void QFeedbackEffect::start()

    Starts playing the effect. If an error occurs there the
    error signal will be emitted.
*/
void QFeedbackEffect::start()
{
    setState(Running);
}

/*!
    \fn void QFeedbackEffect::stop()

    Stops a playing effect. If an error occurs there the
    error signal will be emitted.

    \sa start(), pause(), setState()
*/
void QFeedbackEffect::stop()
{
    setState(Stopped);
}

/*!
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

    This function plays \a effect instantly and returns true if the
    effect could be played; otherwise, returns false.
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
    \ingroup feedback
    \inmodule QtFeedback
    \brief The QFeedbackHapticsEffect class allows you to play a haptics effect.

    A haptics effect is an effect that takes advantage of the sense of
    touch. Most mobile devices today supports one such effect,
    vibration, which will then be the default when you create a
    QFeedbackHatpicsEffect.

    A haptics effect has a few parameters that must be set up before
    it can be played:

    \list
        \o duration(): The duration of the effect in milliseconds.
        \o intensity(): The intensity, e.g., how hard the device will vibrate.
    \endlist

    An effect can, for example, be set up as follows:

    \omit I'm right that this is all that is required to be set? \endomit
    \code
        QFeedbackHapticsEffect rumble;
        rumble.setIntensity(1.0);
        rumble.setDuration(100);
    \endcode

    You can now start() the effect.

    \code
        rumble.start();
    \endcode

    At any given time, the effect is in one of four states:
    \l{QFeedbackEffect::}{Stopped}, \l{QFeedbackEffect::}{Paused},
    \l{QFeedbackEffect::}{Running}, or \l{QFeedbackEffect::}{Loading}.
    You can request a state change by calling start(), pause(), or
    stop(). The state is queried with state().

    The haptics effect also supports a fade-in of the effect. For
    vibration, this means that the vibration will grow (or sink) in
    intensity from when the effect starts until intensity() is
    reached. You can set that up as follows:

    \code
        rumble.setAttackIntensity(0.0);
        rumble.setAttackTime(250);
    \endcode

    Attack intensity is the start intensity and attack time is the
    duration of the fade-in. We have a similar fade-out:

    \code
        rumble.setFadeTime(250);
        rumble.setFadeIntensity(0.0);
    \endcode

    When using fade-in and fade-out the total duration of the haptics
    effect will be: attackTime() + duration() + fadeTime().

    A QFeedbackHatpicsEffect is played on an
    \l{QFeedbackHapticsEffect::}{actuator()}, which is the device that
    performs the effect. You can query if other actuators are
    available, see the QFeedbackActuator::actuators() function
    documentation for details.

    Errors occurring during playback is notified through the
    error() signal.

    \sa QFeedbackActuator
*/

/*!
    \fn virtual void QFeedbackEffect::setState(State state) = 0

    Requests the effect's State to change to the specified \a state.

    Subclasses reimplement this function to handle state changes
    for the effect.
*/

/*!
    Constructs the QFeedbackHatpicsEffect class, and passes \a parent to
    QObject's constructor.
*/
QFeedbackHapticsEffect::QFeedbackHapticsEffect(QObject *parent) : QFeedbackEffect(parent), priv(new QFeedbackHapticsEffectPrivate)
{
    QList<QFeedbackActuator*> list = QFeedbackActuator::actuators();
    if  (!list.isEmpty()) {
        priv->actuator = list.first();
    } else {
        priv->actuator = new QFeedbackActuator(this);
    }
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
    The value can be between 0 and 1.
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
QFeedbackActuator* QFeedbackHapticsEffect::actuator() const
{
    return priv->actuator;
}
void QFeedbackHapticsEffect::setActuator(QFeedbackActuator *actuator)
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

    It has a default value of -1, which means that it is not a periodic effect.
    It is not supported to change the period while an effect is running.
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

/*!
    \internal
*/
void QFeedbackHapticsEffect::setState(State state)
{
    State oldState = this->state();
    if (oldState != state) {
        QFeedbackHapticsInterface::instance()->setEffectState(this, state);
        emit stateChanged();
    }
}

/*!
    \internal
*/
QFeedbackEffect::State QFeedbackHapticsEffect::state() const
{
    return QFeedbackHapticsInterface::instance()->effectState(this);
}

/*!
    \class QFeedbackFileEffect
    \ingroup feedback
    \inmodule QtFeedback
    \brief The QFeedbackFileEffect class allows to play haptics feedback from a file.

    The files containing haptics data are usually suffixed \c .ifr.
    The feedback is usually varying in
    \l{QFeedbackHapticsEffect::}{intensity()}, and is for that reason
    often referred to as a "haptic tune". They are created, for
    instance, from music files where the feedback is based on a
    specific feature in the audio data. For example, you could have a
    phone vibrating along with the bass of a rock song.

    Although Qt Mobility does not let you record \c .ifr files, it lets
    you play them back using the QFeedbackFileEffect class. Setting
    up a QFeedbackFileEffect and starting it is done as follows:

    \code
        QFeedbackFileEffect hapticTune;
        hapticTune.setSource(QUrl::fromLocalFile("mySavedRumble.ifr"));
        hapticTune.load();
        hapticTune.start();
    \endcode

    As with other \l{QFeedbackEffect}s, QFeedbackFileEffect is at any
    given time in one of four states: \l{QFeedbackEffect::}{Loading},
    \l{QFeedbackEffect::}{Running}, \l{QFeedbackEffect::}{Paused}, or
    \l{QFeedbackEffect::}{Stopped}. You request state changes with
    start(), pause(), and stop().

    You can load() and unload() the file at will to free resources or
    be as fast as possible. You must load the file before it can be
    started, and it cannot be unloaded while playing. After the file is
    loaded, you can query its duration().

    QFeedbackFileEffect reports errors through the error() signal.

    \sa QFeedbackHapticsEffect
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

/*!
    \reimp
*/
int QFeedbackFileEffect::duration() const
{
    return QFeedbackFileInterface::instance()->effectDuration(this);
}

/*!
    \property QFeedbackFileEffect::source
    \brief the url of the file that is loaded.

    Setting that property will automatically unload the previous file and load the new one.
    Some backends may not support all URL schemes.
*/
QUrl QFeedbackFileEffect::source() const
{
    return priv->url;
}
void QFeedbackFileEffect::setSource(const QUrl &source)
{
    if (state() != QFeedbackEffect::Stopped) {
        qWarning("QFeedbackFileEffect::setSource: can't set the file while the feedback is running");
        return;
    }
    if (source != priv->url) {
        setLoaded(false);
        priv->url = source;
        setLoaded(true);
    }
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

    Makes sure that the file associated with the feedback object is loaded.
    It will be automatically loaded when setSource or start functions
    are called.
*/
void QFeedbackFileEffect::load()
{
    setLoaded(true);
}

/*!
    \fn void QFeedbackFileEffect::unload()

    makes sure that the file associated with the feedback object is unloaded.
    It will be automatically loaded when the setSource function is called with
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


/*!
\reimp
*/
void QFeedbackFileEffect::setState(State newState)
{
    State oldState = state();
    if (oldState != newState) {
        if (newState != Stopped && state() == Stopped)
            load(); // makes sure the file is loaded
        QFeedbackFileInterface::instance()->setEffectState(this, newState);
        emit stateChanged();
    }
}

/*!
\reimp
*/
QFeedbackEffect::State QFeedbackFileEffect::state() const
{
    return QFeedbackFileInterface::instance()->effectState(this);
}

#include "moc_qfeedbackeffect.cpp"

QTM_END_NAMESPACE
