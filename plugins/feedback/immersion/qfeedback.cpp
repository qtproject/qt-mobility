/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    } else {
        const int nbDev = ImmVibeGetDeviceCount();
        for (int i = 0; i < nbDev; ++i) {
            actuatorList << createFeedbackActuator(this, i);
        }
    }
}

QFeedbackImmersion::~QFeedbackImmersion()
{
    //cleanup the devices when terminating
    for (int i = 0 ; i < actuatorHandles.size(); ++i)
        ImmVibeCloseDevice(actuatorHandles.at(i));

    ImmVibeTerminate();
}

QFeedbackInterface::PluginPriority QFeedbackImmersion::pluginPriority()
{
    return PluginNormalPriority;
}

QList<QFeedbackActuator*> QFeedbackImmersion::actuators()
{
    return actuatorList;
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
            if (VIBE_FAILED(ImmVibeGetDeviceCapabilityString(actuator.id(), VIBE_DEVCAPTYPE_DEVICE_NAME,
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

bool QFeedbackImmersion::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability cap)
{
    switch(cap)
    {
    case QFeedbackActuator::Envelope:
    case QFeedbackActuator::Period:
        return true;
    default:
        return false;
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

void QFeedbackImmersion::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty)
{
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);
    if (VIBE_IS_INVALID_EFFECT_HANDLE(effectHandle))
        return; // the effect is simply not running

    VibeStatus status = VIBE_S_SUCCESS;
    if (effect->period() > 0) {
        status = ImmVibeModifyPlayingPeriodicEffect(handleForActuator(effect->actuator()), effectHandle,
                                           convertedDuration(effect->duration()),
                                           effect->intensity() * qreal(VIBE_MAX_MAGNITUDE), effect->period(),
                                           VIBE_DEFAULT_STYLE,
                                           effect->attackTime(), effect->attackIntensity() * qreal(VIBE_MAX_MAGNITUDE),
                                           effect->fadeTime(), effect->fadeIntensity() * qreal(VIBE_MAX_MAGNITUDE));
    } else {
        status = ImmVibeModifyPlayingMagSweepEffect(handleForActuator(effect->actuator()), effectHandle,
                                           convertedDuration(effect->duration()),
                                           effect->intensity() * qreal(VIBE_MAX_MAGNITUDE),
                                           VIBE_DEFAULT_STYLE,
                                           effect->attackTime(), effect->attackIntensity() * qreal(VIBE_MAX_MAGNITUDE),
                                           effect->fadeTime(), effect->fadeIntensity() * qreal(VIBE_MAX_MAGNITUDE));
    }

    if (VIBE_FAILED(status))
        reportError(effect, QFeedbackEffect::UnknownError);

}

void QFeedbackImmersion::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State state)
{
    VibeStatus status = VIBE_S_SUCCESS;
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);

    switch (state)
    {
    case QFeedbackEffect::Stopped:
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeStopPlayingEffect(handleForActuator(effect->actuator()), effectHandle);
            effectHandles.remove(effect);
        }
        break;
    case QFeedbackEffect::Paused:
        Q_ASSERT(VIBE_IS_VALID_EFFECT_HANDLE(effectHandle));
        status = ImmVibePausePlayingEffect(handleForActuator(effect->actuator()), effectHandle);
        break;
    case QFeedbackEffect::Running:
        //if the effect handle exists, the feedback must be paused 
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeResumePausedEffect(handleForActuator(effect->actuator()), effectHandle);
        } else {
            //we need to start the effect and create the handle
            VibeInt32 effectHandle = VIBE_INVALID_EFFECT_HANDLE_VALUE;
            if (effect->period() > 0) {
                status = ImmVibePlayPeriodicEffect(handleForActuator(effect->actuator()), convertedDuration(effect->duration()),
                    qRound(effect->intensity() * qreal(VIBE_MAX_MAGNITUDE)), effect->period(),
                    VIBE_DEFAULT_STYLE, effect->attackTime(), effect->attackIntensity() * qreal(VIBE_MAX_MAGNITUDE),
                    effect->fadeTime(), effect->fadeIntensity() * qreal(VIBE_MAX_MAGNITUDE), &effectHandle);
            } else {
                status = ImmVibePlayMagSweepEffect(handleForActuator(effect->actuator()), convertedDuration(effect->duration()),
                    qRound(effect->intensity() * qreal(VIBE_MAX_MAGNITUDE)),
                    VIBE_DEFAULT_STYLE, effect->attackTime(), effect->attackIntensity() * qreal(VIBE_MAX_MAGNITUDE),
                    effect->fadeTime(), effect->fadeIntensity() * qreal(VIBE_MAX_MAGNITUDE), &effectHandle);
            }
            if (VIBE_SUCCEEDED(status))
                effectHandles.insert(effect, effectHandle);
        }
        break;
    default:
        break;
    }
}

QFeedbackEffect::State QFeedbackImmersion::effectState(const QFeedbackHapticsEffect *effect)
{
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);
    if (VIBE_IS_INVALID_EFFECT_HANDLE(effectHandle))
        return QFeedbackEffect::Stopped; // the effect is simply not running

    VibeInt32 effectState = VIBE_EFFECT_STATE_NOT_PLAYING;
    ImmVibeGetEffectState(handleForActuator(effect->actuator()), effectHandle, &effectState);

    //here we detect changes in the state of the effect
    switch(effectState)
    {
    case VIBE_EFFECT_STATE_PAUSED:
        return QFeedbackEffect::Paused;
    case VIBE_EFFECT_STATE_PLAYING:
        return QFeedbackEffect::Running;
    case VIBE_EFFECT_STATE_NOT_PLAYING:
    default:
        effectHandles.remove(effect);
        return QFeedbackEffect::Stopped;
    }
}

void QFeedbackImmersion::setLoaded(QFeedbackFileEffect *effect, bool load)
{
    const QUrl url = effect->source();

    // This doesn't handle qrc urls..
    const QString fileName = url.toLocalFile();
    if (fileName.isEmpty())
        return;

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
        reportLoadFinished(effect, success);
    } else {
        //unload
        fc.refCount--;
        if (fc.refCount == 0)
            fileData.remove(fileName);
    }
}

void QFeedbackImmersion::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
{
    VibeStatus status = VIBE_S_SUCCESS;
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);

    VibeInt32 dev = handleForActuator(0); //we always use the default (first) device

    switch (state)
    {
    case QFeedbackEffect::Stopped:
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeStopPlayingEffect(dev, effectHandle);
            effectHandles.remove(effect);
        }
        break;
    case QFeedbackEffect::Paused:
        Q_ASSERT(VIBE_IS_VALID_EFFECT_HANDLE(effectHandle));
        status = ImmVibePausePlayingEffect(dev, effectHandle);
        break;
    case QFeedbackEffect::Running:
        //if the effect handle exists, the feedback must be paused 
        if (VIBE_IS_VALID_EFFECT_HANDLE(effectHandle)) {
            status = ImmVibeResumePausedEffect(dev, effectHandle);
        } else {
            //we need to start the effect and create the handle
            QString fileName = effect->source().toLocalFile();
            Q_ASSERT(fileData.contains(fileName));
            status = ImmVibePlayIVTEffect(dev, fileData[fileName].constData(), 0, &effectHandle);
            if (VIBE_SUCCEEDED(status))
                effectHandles.insert(effect, effectHandle);
        }
        break;
    default:
        break;
    }

    if (VIBE_FAILED(status))
        reportError(effect, QFeedbackEffect::UnknownError);
}

QFeedbackEffect::State QFeedbackImmersion::effectState(const QFeedbackFileEffect *effect)
{
    VibeInt32 effectHandle = effectHandles.value(effect, VIBE_INVALID_EFFECT_HANDLE_VALUE);
    if (VIBE_IS_INVALID_EFFECT_HANDLE(effectHandle))
        return QFeedbackEffect::Stopped; // the effect is simply not running

    VibeInt32 effectState = VIBE_EFFECT_STATE_NOT_PLAYING;
    ImmVibeGetEffectState(handleForActuator(0), effectHandle, &effectState);

    //here we detect changes in the state of the effect
    switch(effectState)
    {
    case VIBE_EFFECT_STATE_PAUSED:
        return QFeedbackEffect::Paused;
    case VIBE_EFFECT_STATE_PLAYING:
        return QFeedbackEffect::Running;
    case VIBE_EFFECT_STATE_NOT_PLAYING:
    default:
        return QFeedbackEffect::Stopped;
    }
}

int QFeedbackImmersion::effectDuration(const QFeedbackFileEffect *effect)
{
    VibeInt32 ret = 0;
    QString fileName = effect->source().toLocalFile();
    if (fileData.contains(fileName))
        ImmVibeGetIVTEffectDuration(fileData[fileName].constData(), 0, &ret);

    return ret;
}

QStringList QFeedbackImmersion::supportedMimeTypes()
{
    return QStringList() << QLatin1String("vibra/ivt");
}
