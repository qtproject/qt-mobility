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
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

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
    for (int i = 0 ; i < deviceHandles.size(); ++i)
        ImmVibeCloseDevice(deviceHandles.at(i));

    ImmVibeTerminate();
}

QList<QFeedbackDevice> QFeedbackImmersion::devices()
{
    QList<QFeedbackDevice> ret;
    const int nbDev = ImmVibeGetDeviceCount();
    for (int i = 0; i < nbDev; ++i) {
        ret << createFeedbackDevice(i);
    }

    return ret;
}

QString QFeedbackImmersion::deviceName(const QFeedbackDevice &dev)
{
    char szDeviceName[VIBE_MAX_DEVICE_NAME_LENGTH] = { 0 };
    if (VIBE_FAILED(ImmVibeGetDeviceCapabilityString(dev.id(),VIBE_DEVCAPTYPE_DEVICE_NAME,
                        VIBE_MAX_CAPABILITY_STRING_LENGTH, szDeviceName)))
        return QString();

    return QString::fromLocal8Bit(szDeviceName);
}

QFeedbackDevice::State QFeedbackImmersion::deviceState(const QFeedbackDevice &dev)
{
    QFeedbackDevice::State ret = QFeedbackDevice::Unknown;
    VibeInt32 s = 0;
    if (dev.isValid() && VIBE_SUCCEEDED(ImmVibeGetDeviceState(dev.id(), &s))) {
        if (s == VIBE_DEVICESTATE_ATTACHED)
            ret = QFeedbackDevice::Ready;
        else if (s == VIBE_DEVICESTATE_BUSY)
            ret = QFeedbackDevice:: Busy;
    }

    return ret;

}

QFeedbackDevice::Capabilities QFeedbackImmersion::supportedCapabilities(const QFeedbackDevice &)
{
  return QFeedbackDevice::Capabilities(QFeedbackDevice::Envelope | QFeedbackDevice::Period);
}

bool QFeedbackImmersion::isEnabled(const QFeedbackDevice &dev)
{
    VibeBool disabled = true;
   if (VIBE_FAILED(ImmVibeGetDevicePropertyBool(handleForDevice(dev), VIBE_DEVPROPTYPE_DISABLE_EFFECTS, &disabled)))
        return false;
    return !disabled;
}

void QFeedbackImmersion::setEnabled(const QFeedbackDevice &dev, bool enabled)
{
    ImmVibeSetDevicePropertyBool(handleForDevice(dev), VIBE_DEVPROPTYPE_DISABLE_EFFECTS, !enabled);
}


VibeInt32 QFeedbackImmersion::convertedDuration(int duration)
{
    //this allows to manage the infinite durations
    if (duration == -1)
        return VIBE_TIME_INFINITE;
    return duration;
}


VibeInt32 QFeedbackImmersion::handleForDevice(const QFeedbackDevice &device)
{
    return handleForDevice(device.id());
}

VibeInt32 QFeedbackImmersion::handleForDevice(int devId)
{
    if (devId < 0)
        return VIBE_INVALID_DEVICE_HANDLE_VALUE;

    //we avoid locking too much (it will only lock if the device is not yet open
    if (deviceHandles.size() <= devId) {
        QMutexLocker locker(&mutex);
        while (deviceHandles.size() <= devId)
            deviceHandles.append(VIBE_INVALID_DEVICE_HANDLE_VALUE);
    }

    if (VIBE_IS_INVALID_DEVICE_HANDLE(deviceHandles.at(devId))) {
        QMutexLocker locker(&mutex);
        if (VIBE_IS_INVALID_DEVICE_HANDLE(deviceHandles.at(devId))) {
            ImmVibeOpenDevice(devId, &deviceHandles[devId] );

            //temporary solution: provide a proto dev licence key
            ImmVibeSetDevicePropertyString(deviceHandles.at(devId), VIBE_DEVPROPTYPE_LICENSE_KEY, "IMWPROTOSJZF4EH6KWVUK8HAP5WACT6Q");
        }
    }
    return deviceHandles.at(devId);
}

QFeedbackEffect::ErrorType QFeedbackImmersion::updateEffectProperty(const QFeedbackEffect *effect, EffectProperty)
{
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);
    if (VIBE_IS_INVALID_EFFECT_HANDLE(effectHandle))
        return QFeedbackEffect::NoError; // the effect is simply not running

    VibeStatus status = VIBE_S_SUCCESS;
    if (effect->period() > 0) {
        status = ImmVibeModifyPlayingPeriodicEffect(handleForDevice(effect->device()), effectHandle,
                                           convertedDuration(effect->duration()),
                                           effect->intensity() / qreal(VIBE_MAX_MAGNITUDE), effect->period(),
                                           VIBE_DEFAULT_STYLE,
                                           effect->attackTime(), effect->attackIntensity(),
                                           effect->fadeTime(), effect->fadeIntensity());
    } else {
        status = ImmVibeModifyPlayingMagSweepEffect(handleForDevice(effect->device()), effectHandle,
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

QFeedbackEffect::ErrorType QFeedbackImmersion::updateEffectState(const QFeedbackEffect *effect)
{
    VibeStatus status = VIBE_S_SUCCESS;
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);

    switch (effect->state())
    {
    case QAbstractAnimation::Stopped:
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeStopPlayingEffect(handleForDevice(effect->device()), effectHandle);
            effectHandles.remove(effect);
        }
        break;
    case QAbstractAnimation::Paused:
        Q_ASSERT(VIBE_IS_VALID_EFFECT_HANDLE(effectHandle));
        status = ImmVibePausePlayingEffect(handleForDevice(effect->device()), effectHandle);
        break;
    case QAbstractAnimation::Running:
        //if the effect handle exists, the feedback must be paused 
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeResumePausedEffect(handleForDevice(effect->device()), effectHandle);
        } else {
            //we need to start the effect and create the handle
            VibeInt32 effectHandle = VIBE_INVALID_EFFECT_HANDLE_VALUE;
            if (effect->period() > 0) {
                status = ImmVibePlayPeriodicEffect(handleForDevice(effect->device()), convertedDuration(effect->duration()),
                    effect->intensity() / qreal(VIBE_MAX_MAGNITUDE), effect->period(),
                    VIBE_DEFAULT_STYLE, effect->attackTime(), effect->attackIntensity(),
                    effect->fadeTime(), effect->fadeIntensity(), &effectHandle);
            } else {
                status = ImmVibePlayMagSweepEffect(handleForDevice(effect->device()), convertedDuration(effect->duration()),
                    effect->intensity() / qreal(VIBE_MAX_MAGNITUDE),
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

QAbstractAnimation::State QFeedbackImmersion::actualEffectState(const QFeedbackEffect *effect)
{
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);
    if (VIBE_IS_INVALID_EFFECT_HANDLE(effectHandle))
        return QAbstractAnimation::Stopped; // the effect is simply not running

    VibeInt32 effectState = VIBE_EFFECT_STATE_NOT_PLAYING;
    ImmVibeGetEffectState(handleForDevice(effect->device()), effectHandle, &effectState);

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

    VibeInt32 dev = handleForDevice(0); //we always use the default (first) device

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
    ImmVibeGetEffectState(handleForDevice(0), effectHandle, &effectState);

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
