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
#include "qfeedbackplugin.h"

#include <QtCore>


QTM_BEGIN_NAMESPACE


/*!
    \class QFeedbackEffect
    \brief This is the base class for the feedback framework.
*/

QFeedbackEffect::QFeedbackEffect(QObject *parent) : QAbstractAnimation(parent)
{
}

/*!
    \fn void QFeedbackEffect::playThemeEffect(InstantEffect effect)

    plays instant feedback.

    That feedback is defined by the theme of the system.
*/
void QFeedbackEffect::playThemeEffect(InstantEffect effect)
{
    if (QThemeFeedbackInterface *iface = QThemeFeedbackInterface::instance())
        iface->play(effect);
    else
        qWarning("QFeedbackEffect::playThemeEffect: playing themed effects not supported");
}

bool QFeedbackEffect::supportsThemeEffect()
{
    return QThemeFeedbackInterface::instance() != 0;
}


/*!
    \class QHapticsFeedbackEffect
    \brief The QHapticsFeedbackEffect allows to play a haptics feedback on an actuator.

    It is possible to set the duration, intenisy and envelope of the effect.
    It is a subclass of QFeedbackEffect (subclass of QAbstractAnimation) which makes it 
    inherit properties from it like the possibility to loop or be integrated in an 
    animation group. It can also be started, stopped or paused.

    You can set the duration to INFINITE. It is then up to the program to stop the effect.

    A feedback effect always works on a feedback actuator.

    it can report errors through the error signal.

    \sa QAbstractAnimation, QFeedbackDevice
*/

QHapticsFeedbackEffect::QHapticsFeedbackEffect(QObject *parent) : QFeedbackEffect(parent), priv(new QHapticsFeedbackEffectPrivate)
{
}


QHapticsFeedbackEffect::~QHapticsFeedbackEffect()
{
}

/*!
    \property QHapticsFeedbackEffect::duration
    \brief the expected duration of the effect.

    This property defines the duration of the feedback effect.
*/
int QHapticsFeedbackEffect::duration() const
{
    return priv->duration;
}
void QHapticsFeedbackEffect::setDuration(int msecs)
{
    if (priv->duration == msecs)
        return;
    priv->duration = msecs;
    QHapticsFeedbackInterface::instance()->updateEffectProperty(this, QHapticsFeedbackInterface::Duration);
}

/*!
    \property QHapticsFeedbackEffect::intensity
    \brief the intensity of the effect.

    This property defines the intensity of the feedback effect.
    The value can be between 0. and 1.
*/
qreal QHapticsFeedbackEffect::intensity() const
{
    return priv->intensity;
}
void QHapticsFeedbackEffect::setIntensity(qreal intensity)
{
    if (priv->intensity == intensity)
        return;
    priv->intensity = intensity;
    QHapticsFeedbackInterface::instance()->updateEffectProperty(this, QHapticsFeedbackInterface::Intensity);
}

/*!
    \property QHapticsFeedbackEffect::attackTime
    \brief the duration of the fade-in effect.

    This property defines the duration of the fade-in effect in msecs.
*/
int QHapticsFeedbackEffect::attackTime() const
{
    return priv->attackTime;
}
void QHapticsFeedbackEffect::setAttackTime(int msecs)
{
    if (priv->attackTime == msecs)
        return;
    priv->attackTime = msecs;
    QHapticsFeedbackInterface::instance()->updateEffectProperty(this, QHapticsFeedbackInterface::AttackTime);
}

/*!
    \property QHapticsFeedbackEffect::attackIntensity
    \brief the initial intensity of the effect.

    This property defines the initial intensity of the effect, before it fades in.
    It is usually lower than intensity.
*/
qreal QHapticsFeedbackEffect::attackIntensity() const
{
    return priv->attackIntensity;
}
void QHapticsFeedbackEffect::setAttackIntensity(qreal intensity)
{
    if (priv->attackIntensity == intensity)
        return;
    priv->attackIntensity = intensity;
    QHapticsFeedbackInterface::instance()->updateEffectProperty(this, QHapticsFeedbackInterface::AttackIntensity);
}

/*!
    \property QHapticsFeedbackEffect::fadeTime
    \brief the duration of the fade-out effect.

    This property defines the duration of the fade-out effect in msecs.
*/
int QHapticsFeedbackEffect::fadeTime() const
{
    return priv->fadeTime;
}
void QHapticsFeedbackEffect::setFadeTime(int msecs)
{
    if (priv->fadeTime == msecs)
        return;
    priv->fadeTime = msecs;
    QHapticsFeedbackInterface::instance()->updateEffectProperty(this, QHapticsFeedbackInterface::FadeTime);
}

/*!
    \property QHapticsFeedbackEffect::fadeIntensity
    \brief the final intensity of the effect.

    This property defines the final intensity of the effect, after it fades out.
    It is usually lower than intensity.
*/
qreal QHapticsFeedbackEffect::fadeIntensity() const
{
    return priv->fadeIntensity;
}
void QHapticsFeedbackEffect::setFadeIntensity(qreal intensity)
{
    if (priv->fadeIntensity == intensity)
        return;
    priv->fadeIntensity = intensity;
    QHapticsFeedbackInterface::instance()->updateEffectProperty(this, QHapticsFeedbackInterface::FadeIntensity);
}

/*!
    \property QHapticsFeedbackEffect::actuator
    \brief the actuator on which the effect operates.

    This property defines the actuator on which the effect operates.
*/
QFeedbackActuator QHapticsFeedbackEffect::actuator() const
{
    return priv->actuator;
}
void QHapticsFeedbackEffect::setActuator(const QFeedbackActuator &actuator)
{
    if (state() != Stopped) {
        qWarning("QHapticsFeedbackEffect::setActuator: The effect is not stopped");
        return;
    }

    priv->actuator = actuator;
}

/*!
    \property QHapticsFeedbackEffect::period
    \brief set the period for the effect.

    It has a default value of -1, which mean that it is not a periodic effect.
    Note: not all actuators support periodic effects
*/
int QHapticsFeedbackEffect::period() const
{
    return priv->period;
}
void QHapticsFeedbackEffect::setPeriod(int msecs)
{
    if (state() != Stopped) {
        qWarning("QHapticsFeedbackEffect::setPeriod: the period can only  be changed if the effect is stopped");
        return;
    }
    priv->period = msecs;
}

/*
    \reimp
*/
void QHapticsFeedbackEffect::updateCurrentTime(int /*currentTime*/)
{
    switch(QHapticsFeedbackInterface::instance()->actualEffectState(this))
    {
    case QAbstractAnimation::Running:
        start();
        break;
    case QAbstractAnimation::Paused:
        pause();
        break;
    case QAbstractAnimation::Stopped:
        stop();
        break;
    }
}

/*
    \reimp
*/
void QHapticsFeedbackEffect::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    ErrorType e = QHapticsFeedbackInterface::instance()->updateEffectState(this);
    QAbstractAnimation::updateState(newState, oldState);
    if (e != NoError)
        emit error(e);
}


void QFileFeedbackEffectPrivate::loadFinished(bool success)
{
    isLoading = false;
    loaded = success;
    if( !success)
        backendUsed = -1;
    emit effect->loadFinished(success);
}


QFileFeedbackEffect::QFileFeedbackEffect(QObject *parent) : QFeedbackEffect(parent), priv(new QFileFeedbackEffectPrivate(this))
{
}

QFileFeedbackEffect::~QFileFeedbackEffect()
{
    setLoaded(false); //ensures we unload the file and frees resources
}

int QFileFeedbackEffect::duration() const
{
    return QFileFeedbackInterface::instance()->effectDuration(this);
}

QString QFileFeedbackEffect::fileName() const
{
    return priv->fileName;
}
void QFileFeedbackEffect::setFileName(const QString &fileName)
{
    if (state() != QAbstractAnimation::Stopped) {
        qWarning("QFileFeedbackEffect::setFileName: can't set the file while the feedback is running");
        return;
    }
    setLoaded(false);
    priv->fileName = fileName;
    setLoaded(true);
}

bool QFileFeedbackEffect::isLoading() const
{
    return priv->isLoading;
}
bool QFileFeedbackEffect::isLoaded() const
{
    return priv->loaded;
}
void QFileFeedbackEffect::setLoaded(bool load)
{
    if (state() != QAbstractAnimation::Stopped) {
        qWarning() << "QFileFeedbackEffect::setLoaded: can't load/unload a file while the effect is not stopped";
        return;
    }

    if (priv->loaded == load && !(isLoading() && !load) )
        return;

    QFileFeedbackInterface::instance()->setLoaded(this, load);
}

QStringList QFileFeedbackEffect::mimeTypes()
{
    return QFileFeedbackInterface::instance()->mimeTypes();
}

void QFileFeedbackEffect::updateCurrentTime(int /*currentTime*/)
{
    switch(QFileFeedbackInterface::instance()->actualEffectState(this))
    {
    case QAbstractAnimation::Running:
        start();
        break;
    case QAbstractAnimation::Paused:
        pause();
        break;
    case QAbstractAnimation::Stopped:
        stop();
        break;
    }
}

void QFileFeedbackEffect::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    ErrorType e = QFileFeedbackInterface::instance()->updateEffectState(this);
    QAbstractAnimation::updateState(newState, oldState);
    if (e != NoError)
        emit error(e);
}

#include "moc_qfeedbackeffect.cpp"

QTM_END_NAMESPACE
