
/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativefeedbackeffect_p.h"


/*!
    \qmlclass FeedbackEffect QDeclarativeFeedbackEffect
    \brief The FeedbackEffect element is the base class for all feedback effects.
    \ingroup qml-feedback-api
    \since Mobility 1.1

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    You can't create one of these elements directly, but several other elements
    inherit the methods and properties of these elements.

    There are several predefined enumerations and constants provided in this class:

    1. Duration
    This enum describes the possible predefined duration types. Generally a specific
    value in milliseconds can be supplied instead of one of these values.
    \list
    \o Feedback.Infinite - Infinite effect duration
    \endlist

    2. State
    This enum describes the state of the effect. An effect will be in one of these states.
    \list
    \o Feedback.stopped - The effect is not running. This is the initial state.
    \o Feedback.paused  - The effect is paused.
    \o Feedback.running - The effect is running.
    \o Feedback.loading - The effect is loading.
    \endlist

    3. ErrorType
    This enum describes the possible errors happening on the effect.
    \list
    \o Feedback.UnknownError - An unknown error occurred.
    \o Feedback.DeviceBusy - The feedback could not start because the device is busy,
       the device could be busy if a higher-priority client is using the haptics/actuator device.
    \endlist


    \sa FileEffect, ThemeEffect, HapticsEffect, {QFeedbackEffect}
*/


QDeclarativeFeedbackEffect::QDeclarativeFeedbackEffect(QObject *parent)
    : QObject(parent), m_running(false), m_paused(false), m_error(UnknownError)
{
}

void QDeclarativeFeedbackEffect::setFeedbackEffect(QFeedbackEffect* effect)
{
    m_effect = effect;
    QObject::connect(m_effect, SIGNAL(stateChanged()), this, SLOT(updateState()));
    QObject::connect(m_effect, SIGNAL(error(QFeedbackEffect::ErrorType)), this, SLOT(_error(QFeedbackEffect::ErrorType)));
}
QFeedbackEffect* QDeclarativeFeedbackEffect::feedbackEffect()
{
    return m_effect;
}

/*!
  \qmlproperty bool FeedbackEffect::running

  This property is true if this feedback effect is running.
  \since Mobility 1.1
  */
bool QDeclarativeFeedbackEffect::isRunning() const
{
    return m_running;
}
void QDeclarativeFeedbackEffect::setRunning(bool running)
{
    QDeclarativeFeedbackEffect::State currentState = static_cast<QDeclarativeFeedbackEffect::State>(m_effect->state());
    if (currentState != QDeclarativeFeedbackEffect::Running && running) {
        m_running = true;
        m_effect->start();
        emit runningChanged();
    } else if (currentState != QDeclarativeFeedbackEffect::Stopped && !running) {
        m_running = false;
        m_effect->stop();
        emit runningChanged();
    }
}

/*!
  \qmlproperty bool FeedbackEffect::paused

  This property is true if this feedback effect is paused.
  \since Mobility 1.1
  */
bool QDeclarativeFeedbackEffect::isPaused() const
{
    return m_paused;
}
void QDeclarativeFeedbackEffect::setPaused(bool paused)
{
    QDeclarativeFeedbackEffect::State currentState = static_cast<QDeclarativeFeedbackEffect::State>(m_effect->state());
    if (currentState == QDeclarativeFeedbackEffect::Paused && !paused) {
        m_paused = true;
        m_effect->start();
        emit pausedChanged();
    } else if (currentState == QDeclarativeFeedbackEffect::Running && paused) {
        paused = false;
        m_effect->pause();
        emit pausedChanged();
    }
}


/*!
  \qmlproperty int FeedbackEffect::duration

    \since Mobility 1.1
  The duration of the effect, in milliseconds.  This is 0 for effects of unknown
  duration, or Feedback.Infinite for effects that don't stop.
  \sa Feedback
  */
int QDeclarativeFeedbackEffect::duration() const
{
    return m_effect->duration();
}
void QDeclarativeFeedbackEffect::setDuration(int newDuration)
{
    Q_UNUSED(newDuration)
    //default do nothing
}

/*!
  \qmlproperty FeedbackEffect::State FeedbackEffect::state

    \since Mobility 1.1
  This is the current state of the effect.  It is one of:
  \list
  \o Feedback.Stopped - the effect is not playing
  \o Feedback.Loading - the effect is being loaded
  \o Feedback.Running - the effect is playing
  \o Feedback.Paused - the effect was being played, but is now paused.
  \endlist
  \sa Feedback
  */
QDeclarativeFeedbackEffect::State QDeclarativeFeedbackEffect::state() const
{
    return static_cast<QDeclarativeFeedbackEffect::State>(m_effect->state());
}

void QDeclarativeFeedbackEffect::setState(QDeclarativeFeedbackEffect::State newState)
{
    Q_UNUSED(newState)
    //default do nothing
}
/*!
  \qmlproperty Feedback::ErrorType FeedbackEffect::error

    \since Mobility 1.1
  This property holds the error status of the FeedbackEffect.
  The error is one of the following values:
  \list
  \o Feedback.UnknownError - An unknown error occurred
  \o Feedback.DeviceBusy - The device resource is already being used.
  \endlist

  \sa Feedback QFeedbackEffect::ErrorType
  */
QDeclarativeFeedbackEffect::ErrorType QDeclarativeFeedbackEffect::error() const
{
    return m_error;
}

/*!
    \qmlmethod  Feedback::updateState()

     updates the state of the effect.
     \since Mobility 1.1
*/
void QDeclarativeFeedbackEffect::updateState() {
    bool running = m_effect->state() == QFeedbackEffect::Running;
    bool paused = m_effect->state() == QFeedbackEffect::Paused;
    if (running != m_running) {
        m_running = running;
        emit runningChanged();
    }
    if (paused != m_paused) {
        m_paused = paused;
        emit pausedChanged();
    }
}

/*!
    \qmlmethod  Feedback::start()

    makes sure that the effect associated with the feedback object is started.
    \since Mobility 1.2
    \sa QFeedbackEffect::start()
*/
void QDeclarativeFeedbackEffect::start() {
    m_effect->start();
}

/*!
    \qmlmethod  Feedback::stop()

    makes sure that the effect associated with the feedback object is stoped.
    \since Mobility 1.2
    \sa QFeedbackEffect::stop()
*/
void QDeclarativeFeedbackEffect::stop() {
    m_effect->stop();
}

/*!
    \qmlmethod  Feedback::pause()

    makes sure that the effect associated with the feedback object is paused.
    \since Mobility 1.2
    \sa QFeedbackEffect::pause()
*/
void QDeclarativeFeedbackEffect::pause() {
    m_effect->pause();
}

void QDeclarativeFeedbackEffect::_error(QFeedbackEffect::ErrorType err)
{
    if (static_cast<ErrorType>(err) != m_error) {
        m_error = static_cast<ErrorType>(err);
        emit errorChanged();
    }
}
