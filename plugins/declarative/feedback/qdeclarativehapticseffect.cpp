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

#include "qdeclarativehapticseffect_p.h"
/*!
    \qmlclass HapticsEffect
    \brief The HapticsEffect element represents a custom haptic feedback effect.
    \ingroup qml-feedback-api
    \inherits FeedbackEffect

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    This class closely corresponds to the C++ \l QFeedbackHapticsEffect class.

    \snippet doc/src/snippets/declarative/declarative-feedback.qml Haptics Effect

    \sa Actuator, {QFeedbackHapticsEffect}
*/
QDeclarativeHapticsEffect::QDeclarativeHapticsEffect(QObject *parent)
    : QDeclarativeFeedbackEffect(parent), m_actuator(0)
{
    d = new QFeedbackHapticsEffect(this);
    setFeedbackEffect(d);

    QFeedbackActuator* fa = d->actuator();

    QList<QFeedbackActuator*> actuators = QFeedbackActuator::actuators();
    foreach (QFeedbackActuator* actuator, actuators) {
        QDeclarativeFeedbackActuator* dfa;
        dfa = new QDeclarativeFeedbackActuator(this, actuator);
        if (fa && *fa == *actuator) {
            m_actuator = dfa;
        }
        m_actuators.push_back(dfa);
    }
}

void QDeclarativeHapticsEffect::setDuration(int msecs)
{
    if (msecs != d->duration()) {
        d->setDuration(msecs);
        emit durationChanged();
    }
}

int QDeclarativeHapticsEffect::duration() const
{
    return d->duration();
}
/*!
    \qmlproperty double HapticsEffect::intensity

    The intensity of the main part of the haptics effect, from 0.0 to 1.0.
*/
void QDeclarativeHapticsEffect::setIntensity(qreal intensity)
{
    if (qFuzzyCompare(intensity, d->intensity())) {
        d->setIntensity(intensity);
        emit intensityChanged();
    }
}

qreal QDeclarativeHapticsEffect::intensity() const
{
    return d->intensity();
}

/*!
    \qmlproperty int HapticsEffect::attackTime

    The duration of the attack (fade-in) part of the haptics effect.
*/

void QDeclarativeHapticsEffect::setAttackTime(int msecs)
{
    if (msecs != d->attackTime()) {
        d->setAttackTime(msecs);
        emit attackTimeChanged();
    }
}

int QDeclarativeHapticsEffect::attackTime() const
{
    return d->attackTime();
}
/*!
    \qmlproperty double HapticsEffect::attackIntensity

    The intensity of the attack (fade-in) part of the haptics effect, from 0.0 to 1.0.
*/
void QDeclarativeHapticsEffect::setAttackIntensity(qreal intensity)
{
    if (qFuzzyCompare(intensity, d->attackIntensity())) {
        d->setAttackIntensity(intensity);
        emit intensityChanged();
    }
}

qreal QDeclarativeHapticsEffect::attackIntensity() const
{
    return d->attackIntensity();
}
/*!
    \qmlproperty int HapticsEffect::fadeTime

    The duration of the fade-out part of the haptics effect.
*/

void QDeclarativeHapticsEffect::setFadeTime(int msecs)
{
    if (msecs != d->fadeTime()) {
        d->setFadeTime(msecs);
        emit fadeTimeChanged();
    }
}

int QDeclarativeHapticsEffect::fadeTime() const
{
    return d->fadeTime();
}

void QDeclarativeHapticsEffect::setFadeIntensity(qreal intensity)
{
    if (qFuzzyCompare(intensity, d->fadeIntensity())) {
        d->setFadeIntensity(intensity);
        emit fadeIntensityChanged();
    }
}
/*!
    \qmlproperty double HapticsEffect::fadeIntensity

    The intensity of the fade-out part of the haptics effect, from 0.0 to 1.0.
*/
qreal QDeclarativeHapticsEffect::fadeIntensity() const
{
    return d->fadeIntensity();
}
/*!
    \qmlproperty int HapticsEffect::period

    The period of the haptics effect.  If the period is zero, the effect will
    not repeat.  If it is non-zero, the effect will repeat every period milliseconds.
*/

void QDeclarativeHapticsEffect::setPeriod(int msecs)
{
    if (msecs != d->period()) {
        d->setPeriod(msecs);
        emit periodChanged();
    }
}

int QDeclarativeHapticsEffect::period() const
{
    return d->period();
}

/*!
    \qmlproperty Actuator HapticsEffect::actuator

    The actuator that is used for playing this effect.
    \sa Actuator
*/
void QDeclarativeHapticsEffect::setActuator(QDeclarativeFeedbackActuator *actuator)
{
    if (actuator != m_actuator) {
        if (!actuator
         || !m_actuator
         || !(*(actuator->feedbackActuator()) == *(m_actuator->feedbackActuator()))) {
            m_actuator = actuator;
            d->setActuator(m_actuator->feedbackActuator());
            emit actuatorChanged();
        }
    }
}

QDeclarativeFeedbackActuator* QDeclarativeHapticsEffect::actuator() const
{
    return m_actuator;
}
/*!
    \qmlproperty list<Actuator> HapticsEffect::availableActuators

    This property holds a list of available actuators.
    This property is read only.
    \sa Actuator
*/
QDeclarativeListProperty<QDeclarativeFeedbackActuator> QDeclarativeHapticsEffect::availableActuators() {
    return QDeclarativeListProperty<QDeclarativeFeedbackActuator>(this,
                                                                  0,
                                                                  0 /*appending actuators are not allowed*/,
                                                                  actuator_count,
                                                                  actuator_at,
                                                                  0 /*removing actuators are not allowed*/);
}

int QDeclarativeHapticsEffect::actuator_count(QDeclarativeListProperty<QDeclarativeFeedbackActuator> *prop)
{
    return static_cast<QDeclarativeHapticsEffect*>(prop->object)->m_actuators.size();
}
QDeclarativeFeedbackActuator* QDeclarativeHapticsEffect::actuator_at(QDeclarativeListProperty<QDeclarativeFeedbackActuator> *prop, int index)
{
    return static_cast<QDeclarativeHapticsEffect*>(prop->object)->m_actuators.at(index);
}
