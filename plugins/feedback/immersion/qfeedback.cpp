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

#include <qfeedbackactuator.h>
#include "qfeedback.h"
#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <QDebug>

#define MAX_FILE_SIZE (1 << 14) //16KB

Q_EXPORT_PLUGIN2(feedback_immersion, QFeedbackImmersion)

QFeedbackImmersion::QFeedbackImmersion() : QObject(qApp)
{
    if (VIBE_FAILED(ImmVibeInitialize(VIBE_CURRENT_VERSION_NUMBER))) {
        //that should be done once
        //error management
        qWarning() << "the Immersion library could not be initialized";
    }
}

QFeedbackImmersion::~QFeedbackImmersion()
{
    //cleanup the devices when terminating
    for (int i = 0 ; i < actuatorHandles.size(); ++i)
        ImmVibeCloseDevice(actuatorHandles.at(i));

    ImmVibeTerminate();
}

QList<QFeedbackActuator> QFeedbackImmersion::actuators()
{
    QList<QFeedbackActuator> ret;
    const int nbDev = ImmVibeGetDeviceCount();
    for (int i = 0; i < nbDev; ++i) {
        ret << createFeedbackActuator(i);
    }

    return ret;
}

void QFeedbackImmersion::setActuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop, const QVariant &value)
{
    switch (prop)
    {
    case Enabled:
        ImmVibeSetDevicePropertyBool(handleForActuator(actuator), VIBE_DEVPROPTYPE_DISABLE_EFFECTS, !value.toBool());
        break;
    default:
        break;
    }
}

QVariant QFeedbackImmersion::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    switch (prop)
    {
    case Name:
        {
            char szDeviceName[VIBE_MAX_DEVICE_NAME_LENGTH] = { 0 };
            if (VIBE_FAILED(ImmVibeGetDeviceCapabilityString(actuator.id(),VIBE_DEVCAPTYPE_DEVICE_NAME,
                VIBE_MAX_CAPABILITY_STRING_LENGTH, szDeviceName)))
                return QString();

            return QString::fromLocal8Bit(szDeviceName);
        }

    case State:
        {
            QFeedbackActuator::State ret = QFeedbackActuator::Unknown;
            VibeInt32 s = 0;
            if (actuator.isValid() && VIBE_SUCCEEDED(ImmVibeGetDeviceState(actuator.id(), &s))) {
                if (s == VIBE_DEVICESTATE_ATTACHED)
                    ret = QFeedbackActuator::Ready;
                else if (s == VIBE_DEVICESTATE_BUSY)
                    ret = QFeedbackActuator:: Busy;
            }

            return ret;
        }
    case SupportedCapabilities:
        return qVariantFromValue<int>(QFeedbackActuator::Envelope | QFeedbackActuator::Period);
    case Enabled:
        {
            VibeBool disabled = true;
            if (VIBE_FAILED(ImmVibeGetDevicePropertyBool(handleForActuator(actuator), VIBE_DEVPROPTYPE_DISABLE_EFFECTS, &disabled)))
                return false;
            return !disabled;
        }
    default:
        return QVariant();
    }
}

VibeInt32 QFeedbackImmersion::convertedDuration(int duration)
{
    //this allows to manage the infinite durations
    if (duration == -1)
        return VIBE_TIME_INFINITE;
    return duration;
}


VibeInt32 QFeedbackImmersion::handleForActuator(const QFeedbackActuator &actuator)
{
    return handleForActuator(actuator.id());
}

VibeInt32 QFeedbackImmersion::handleForActuator(int actId)
{
    if (actId < 0)
        return VIBE_INVALID_DEVICE_HANDLE_VALUE;

    //we avoid locking too much (it will only lock if the device is not yet open
    if (actuatorHandles.size() <= actId) {
        QMutexLocker locker(&mutex);
        while (actuatorHandles.size() <= actId)
            actuatorHandles.append(VIBE_INVALID_DEVICE_HANDLE_VALUE);
    }

    if (VIBE_IS_INVALID_DEVICE_HANDLE(actuatorHandles.at(actId))) {
        QMutexLocker locker(&mutex);
        if (VIBE_IS_INVALID_DEVICE_HANDLE(actuatorHandles.at(actId))) {
            ImmVibeOpenDevice(actId, &actuatorHandles[actId] );

            //temporary solution: provide a proto dev licence key
            ImmVibeSetDevicePropertyString(actuatorHandles.at(actId), VIBE_DEVPROPTYPE_LICENSE_KEY, "IMWPROTOSJZF4EH6KWVUK8HAP5WACT6Q");
        }
    }
    return actuatorHandles.at(actId);
}

QFeedbackEffect::ErrorType QFeedbackImmersion::updateEffectProperty(const QHapticsFeedbackEffect *effect, EffectProperty)
{
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);
    if (VIBE_IS_INVALID_EFFECT_HANDLE(effectHandle))
        return QFeedbackEffect::NoError; // the effect is simply not running

    VibeStatus status = VIBE_S_SUCCESS;
    if (effect->period() > 0) {
        status = ImmVibeModifyPlayingPeriodicEffect(handleForActuator(effect->actuator()), effectHandle,
                                           convertedDuration(effect->duration()),
                                           effect->intensity() / qreal(VIBE_MAX_MAGNITUDE), effect->period(),
                                           VIBE_DEFAULT_STYLE,
                                           effect->attackTime(), effect->attackIntensity(),
                                           effect->fadeTime(), effect->fadeIntensity());
    } else {
        status = ImmVibeModifyPlayingMagSweepEffect(handleForActuator(effect->actuator()), effectHandle,
                                           convertedDuration(effect->duration()),
                                           effect->intensity() / qreal(VIBE_MAX_MAGNITUDE),
                                           VIBE_DEFAULT_STYLE,
                                           effect->attackTime(), effect->attackIntensity(),
                                           effect->fadeTime(), effect->fadeIntensity());
    }

    if (VIBE_FAILED(status))
        return QFeedbackEffect::UnknownError;

    return QFeedbackEffect::NoError;
}

QFeedbackEffect::ErrorType QFeedbackImmersion::updateEffectState(const QHapticsFeedbackEffect *effect)
{
    VibeStatus status = VIBE_S_SUCCESS;
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);

    switch (effect->state())
    {
    case QAbstractAnimation::Stopped:
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeStopPlayingEffect(handleForActuator(effect->actuator()), effectHandle);
            effectHandles.remove(effect);
        }
        break;
    case QAbstractAnimation::Paused:
        Q_ASSERT(VIBE_IS_VALID_EFFECT_HANDLE(effectHandle));
        status = ImmVibePausePlayingEffect(handleForActuator(effect->actuator()), effectHandle);
        break;
    case QAbstractAnimation::Running:
        //if the effect handle exists, the feedback must be paused 
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeResumePausedEffect(handleForActuator(effect->actuator()), effectHandle);
        } else {
            //we need to start the effect and create the handle
            VibeInt32 effectHandle = VIBE_INVALID_EFFECT_HANDLE_VALUE;
            if (effect->period() > 0) {
                status = ImmVibePlayPeriodicEffect(handleForActuator(effect->actuator()), convertedDuration(effect->duration()),
                    qRound(effect->intensity() * qreal(VIBE_MAX_MAGNITUDE)), effect->period(),
                    VIBE_DEFAULT_STYLE, effect->attackTime(), effect->attackIntensity(),
                    effect->fadeTime(), effect->fadeIntensity(), &effectHandle);
            } else {
                status = ImmVibePlayMagSweepEffect(handleForActuator(effect->actuator()), convertedDuration(effect->duration()),
                    qRound(effect->intensity() * qreal(VIBE_MAX_MAGNITUDE)),
                    VIBE_DEFAULT_STYLE, effect->attackTime(), effect->attackIntensity(),
                    effect->fadeTime(), effect->fadeIntensity(), &effectHandle);
            }
            if (VIBE_SUCCEEDED(status))
                effectHandles.insert(effect, effectHandle);
        }
        break;
    }

    if (VIBE_FAILED(status))
        return QFeedbackEffect::UnknownError;

    return QFeedbackEffect::NoError;
}

QAbstractAnimation::State QFeedbackImmersion::actualEffectState(const QHapticsFeedbackEffect *effect)
{
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);
    if (VIBE_IS_INVALID_EFFECT_HANDLE(effectHandle))
        return QAbstractAnimation::Stopped; // the effect is simply not running

    VibeInt32 effectState = VIBE_EFFECT_STATE_NOT_PLAYING;
    ImmVibeGetEffectState(handleForActuator(effect->actuator()), effectHandle, &effectState);

    //here we detect changes in the state of the effect
    switch(effectState)
    {
    case VIBE_EFFECT_STATE_PAUSED:
        return QAbstractAnimation::Paused;
    case VIBE_EFFECT_STATE_PLAYING:
        return QAbstractAnimation::Running;
    case VIBE_EFFECT_STATE_NOT_PLAYING:
    default:
        effectHandles.remove(effect);
        return QAbstractAnimation::Stopped;
    }
}

void QFeedbackImmersion::setLoaded(QFileFeedbackEffect *effect, bool load)
{
    const QString fileName = effect->fileName();
    if (!load && !fileData.contains(fileName))
        return;

    FileContent &fc = fileData[fileName];
    if (load) {
        bool success = true;
        fc.refCount++;
        if (fc.refCount == 1) {
            //we need to load the file
            QFile file(fileName);
            success = false;
            if (file.size() < MAX_FILE_SIZE && file.open(QIODevice::ReadOnly)) {
                fc.ba = file.readAll();
                //now let's try to check the file content with immersion
                if (VIBE_FAILED(ImmVibeGetIVTEffectCount(fc.constData()))) {
                    fileData.remove(fileName); 
                } else {
                    success = true;
                }
            }
        }
        asyncLoadFinished(effect, success);
    } else {
        //unload
        fc.refCount--;
        if (fc.refCount == 0)
            fileData.remove(fileName);
    }
}

QFileFeedbackEffect::ErrorType QFeedbackImmersion::updateEffectState(QFileFeedbackEffect *effect)
{
    VibeStatus status = VIBE_S_SUCCESS;
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);

    VibeInt32 dev = handleForActuator(0); //we always use the default (first) device

    switch (effect->state())
    {
    case QAbstractAnimation::Stopped:
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeStopPlayingEffect(dev, effectHandle);
            effectHandles.remove(effect);
        }
        break;
    case QAbstractAnimation::Paused:
        Q_ASSERT(VIBE_IS_VALID_EFFECT_HANDLE(effectHandle));
        status = ImmVibePausePlayingEffect(dev, effectHandle);
        break;
    case QAbstractAnimation::Running:
        //if the effect handle exists, the feedback must be paused 
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeResumePausedEffect(dev, effectHandle);
        } else {
            //we need to start the effect and create the handle
            Q_ASSERT(fileData.contains(effect->fileName()));
            status = ImmVibePlayIVTEffect(dev, fileData[effect->fileName()].constData(), 0, &effectHandle);
            if (VIBE_SUCCEEDED(status))
                effectHandles.insert(effect, effectHandle);
        }
        break;
    }

    if (VIBE_FAILED(status))
        return QFileFeedbackEffect::UnknownError;

    return QFileFeedbackEffect::NoError;
}

QAbstractAnimation::State QFeedbackImmersion::actualEffectState(const QFileFeedbackEffect *effect)
{
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);
    if (VIBE_IS_INVALID_EFFECT_HANDLE(effectHandle))
        return QAbstractAnimation::Stopped; // the effect is simply not running

    VibeInt32 effectState = VIBE_EFFECT_STATE_NOT_PLAYING;
    ImmVibeGetEffectState(handleForActuator(0), effectHandle, &effectState);

    //here we detect changes in the state of the effect
    switch(effectState)
    {
    case VIBE_EFFECT_STATE_PAUSED:
        return QAbstractAnimation::Paused;
    case VIBE_EFFECT_STATE_PLAYING:
        return QAbstractAnimation::Running;
    case VIBE_EFFECT_STATE_NOT_PLAYING:
    default:
        return QAbstractAnimation::Stopped;
    }
}

int QFeedbackImmersion::effectDuration(const QFileFeedbackEffect *effect)
{
    VibeInt32 ret = 0;
    if (fileData.contains(effect->fileName()))
        ImmVibeGetIVTEffectDuration(fileData[effect->fileName()].constData(), 0, &ret);

    return ret;
}

QStringList QFeedbackImmersion::mimeTypes()
{
    return QStringList() << QLatin1String("vibra/ivt");
}
