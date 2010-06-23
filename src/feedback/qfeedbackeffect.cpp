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
    \brief This is the base class for the feedback framework.
*/

QFeedbackEffect::QFeedbackEffect(QObject *parent) : QObject(parent)
{
}

void QFeedbackEffect::start()
{
    setState(Running);
}

void QFeedbackEffect::stop()
{
    setState(Stopped);
}

void QFeedbackEffect::pause()
{
    setState(Paused);
}

/*!
    \fn void QFeedbackEffect::playThemeEffect(InstantEffect effect)

    plays instant feedback and return true if the effect could be played.

    That feedback is defined by the theme of the system.
*/
bool QFeedbackEffect::playThemeEffect(ThemeEffect effect)
{
    if (QFeedbackThemeInterface *iface = QFeedbackThemeInterface::instance())
        return iface->play(effect);
    return false;
}

bool QFeedbackEffect::supportsThemeEffect()
{
    return QFeedbackThemeInterface::instance() != 0;
}


/*!
    \class QFeedbackHapticsEffect
    \brief The QFeedbackHapticsEffect allows to play a haptics feedback on an actuator.

    It is possible to set the duration, intenisy and envelope of the effect.
    It is a subclass of QFeedbackEffect (subclass of QAbstractAnimation) which makes it 
    inherit properties from it like the possibility to loop or be integrated in an 
    animation group. It can also be started, stopped or paused.

    You can set the duration to INFINITE. It is then up to the program to stop the effect.

    A feedback effect always works on a feedback actuator.

    it can report errors through the error signal.

    \sa QAbstractAnimation, QFeedbackDevice
*/

QFeedbackHapticsEffect::QFeedbackHapticsEffect(QObject *parent) : QFeedbackEffect(parent), priv(new QFeedbackHapticsEffectPrivate)
{
}


QFeedbackHapticsEffect::~QFeedbackHapticsEffect()
{
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

QFeedbackEffect::State QFeedbackHapticsEffect::state() const
{
    return QFeedbackHapticsInterface::instance()->effectState(this);
}

/*!
    \internal
*/
void QFeedbackFileEffectPrivate::loadFinished(bool success)
{
    loaded = success;
    if( !success)
        backendUsed = -1;
}


QFeedbackFileEffect::QFeedbackFileEffect(QObject *parent) : QFeedbackEffect(parent), priv(new QFeedbackFileEffectPrivate(this))
{
}

QFeedbackFileEffect::~QFeedbackFileEffect()
{
    setLoaded(false); //ensures we unload the file and frees resources
}

int QFeedbackFileEffect::duration() const
{
    return QFeedbackFileInterface::instance()->effectDuration(this);
}

/*!
    \property QFeedbackFileEffect::filename
    \brief the name of the file that is loaded.
*/
QString QFeedbackFileEffect::fileName() const
{
    return priv->fileName;
}
void QFeedbackFileEffect::setFileName(const QString &fileName)
{
    if (state() != QAbstractAnimation::Stopped) {
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

    if (state() != QAbstractAnimation::Stopped) {
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

QFeedbackEffect::State QFeedbackFileEffect::state() const
{
    return QFeedbackFileInterface::instance()->effectState(this);
}

#include "moc_qfeedbackeffect.cpp"

QTM_END_NAMESPACE
