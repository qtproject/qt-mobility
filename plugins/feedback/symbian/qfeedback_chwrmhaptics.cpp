/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qfeedbackactuator.h>
#include "qfeedback_symbian.h"
#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>

#include <hwrmhaptics.h>
#include <hwrmlogicalactuators.h>

#define MAX_FILE_SIZE 16535 // XXX TODO: what should this be?
#define VIBE_MAX_MAGNITUDE 10000 // XXX TODO: what should this be?

/* ============================== Common Functions ============================== */
CHWRMHaptics *QFeedbackSymbian::haptics(int actuator)
{
    THWRMLogicalActuators logicalActuator = static_cast<THWRMLogicalActuators>(actuator);
    CHWRMHaptics* haptics = m_haptics.value(logicalActuator);
    if (!haptics) {
        QT_TRAP_THROWING(haptics = CHWRMHaptics::NewL(this, 0));
        if (haptics) {
            // no recovery is possible if this fails.
            QT_TRAP_THROWING(haptics->OpenActuatorL(logicalActuator));

            // Add our new haptics handle to our internal hash.
            m_haptics.insert(logicalActuator, haptics);
        }
    }

    return haptics;
}

/* ============================== Haptics Interface Functions ============================== */

void QFeedbackSymbian::setActuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop, const QVariant &value)
{
    // set the device property for the actuator.
    switch(prop)
    {
    case Enabled:
        TRAP_IGNORE(haptics(actuator.id())->SetDeviceProperty(CHWRMHaptics::EHWRMHapticsDisableEffects, (!(value.toBool()))));
        // fall through.
    default:
        break; // we don't allow setting the name or the state.
    }
}

QVariant QFeedbackSymbian::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    // return the device property for the actuator.
    TInt value(0);
    switch(prop)
    {
        case Name:
        {
            return QString("chwrmhaptics:actuator:%1").arg(actuator.id());
        }
        case State:
        {
            TInt status = 0;
            TRAP_IGNORE(status = haptics(actuator.id())->HapticsStatus());
            switch (status)
            {
            case MHWRMHapticsObserver::EHWRMHapticsStatusAvailable:
                return QFeedbackActuator::Ready;
            case MHWRMHapticsObserver::EHWRMHapticsStatusReserved:
                return QFeedbackActuator::Busy;
            case MHWRMHapticsObserver::EHWRMHapticsStatusSuspended:
                return QFeedbackActuator::Busy; // a higher priority client is using the actuator.
            default:
                break;
            }
            return QFeedbackActuator::Unknown;
        }
        case Enabled:
        {
            TRAP_IGNORE(haptics(actuator.id())->GetDeviceProperty(CHWRMHaptics::EHWRMHapticsDisableEffects, value));
            return (!value);
        }
        default:
        {
            break; // unknown property type.  Just return 0 (false).
        }
    }

    return QVariant::fromValue(value);
}

bool QFeedbackSymbian::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability cap)
{
    switch(cap)
    {
    case QFeedbackActuator::Envelope:
    case QFeedbackActuator::Period:
        return true;
    default:
        break;
    }
    return false;
}

void QFeedbackSymbian::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty prop)
{
    TInt errorCode(KErrNone);
    TInt effectHandle = effectHandles.value(effect);

    if (prop == Period) {
        // We cannot change the period of a playing effect.
        reportError(effect, QFeedbackEffect::DeviceBusy);
        return;
    }

    // If we get here, it was some property other than Period which needed modification.
    // In this case, we can modify the effect directly, without stopping and then recreating it.
    if (effect->period() == -1) { // magnitude sweep (non-periodic) effect
        // reconstruct the effect with the updated parameters
        CHWRMHaptics::THWRMHapticsMagSweepEffect magnitudeSweepEffect;
        magnitudeSweepEffect.iDuration = effect->duration(); // the ModifyPlayingXxxEffect() function is smart enough to modify the end time without resetting the play time.
        magnitudeSweepEffect.iMagnitude = qRound(effect->intensity() * qreal(VIBE_MAX_MAGNITUDE));
        magnitudeSweepEffect.iStyle = CHWRMHaptics::EHWRMHapticsStyleSmooth; // XXX TODO: support this?
        magnitudeSweepEffect.iAttackTime = effect->attackTime();
        magnitudeSweepEffect.iAttackLevel = qRound(effect->attackIntensity() * qreal(VIBE_MAX_MAGNITUDE));
        magnitudeSweepEffect.iFadeTime = effect->fadeTime();
        magnitudeSweepEffect.iFadeLevel = qRound(effect->fadeIntensity() * qreal(VIBE_MAX_MAGNITUDE));

        // update the effect
        TRAP(errorCode, haptics(effect->actuator()->id())->ModifyPlayingMagSweepEffect(effectHandle, magnitudeSweepEffect));
    } else { // periodic effect
        CHWRMHaptics::THWRMHapticsPeriodicEffect periodicEffect;
        periodicEffect.iDuration = effect->duration(); // the ModifyPlayingXxxEffect() function is smart enough to modify the end time without resetting the play time.
        periodicEffect.iMagnitude = qRound(effect->intensity() * qreal(VIBE_MAX_MAGNITUDE));
        periodicEffect.iPeriod = effect->period();
        periodicEffect.iStyle = CHWRMHaptics::EHWRMHapticsStyleSmooth; // XXX TODO: support this?
        periodicEffect.iAttackTime = effect->attackTime();
        periodicEffect.iAttackLevel = qRound(effect->attackIntensity() * qreal(VIBE_MAX_MAGNITUDE));
        periodicEffect.iFadeTime = effect->fadeTime();
        periodicEffect.iFadeLevel = qRound(effect->fadeIntensity() * qreal(VIBE_MAX_MAGNITUDE));

        // update the effect
        TRAP(errorCode, haptics(effect->actuator()->id())->ModifyPlayingPeriodicEffect(effectHandle, periodicEffect));
    }

    if (errorCode != KErrNone) {
        reportError(effect, QFeedbackEffect::UnknownError);
    }
}

void QFeedbackSymbian::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State newState)
{
    // retrieve the handle for this effect.
    TInt effectHandle = effectHandles.value(effect);

    TInt err(KErrNone);
    switch(newState)
    {
    case QFeedbackEffect::Stopped:
        TRAP(err, haptics(effect->actuator()->id())->StopPlayingEffect(effectHandle));
        if (err == KErrNone) {
            // we remove the effect from our hashes if it's stopped.
            effectHandles.remove(effect);
            handleEffects.remove(effectHandle);
        }
        break;
    case QFeedbackEffect::Paused:
        TRAP(err, haptics(effect->actuator()->id())->PausePlayingEffect(effectHandle));
        break;
    case QFeedbackEffect::Running:
        if (effect->state() == QFeedbackEffect::Paused) {
            // if it was paused, we can resume it by specifying the handle
            TRAP(err, haptics(effect->actuator()->id())->ResumePausedEffect(effectHandle));
        } else {
            // otherwise, we will receive a new handle from the PlayXxxEffect() functions
            if (effect->period() == -1) {
                // reconstruct the effect with the updated parameters
                CHWRMHaptics::THWRMHapticsMagSweepEffect magnitudeSweepEffect;
                magnitudeSweepEffect.iDuration = effect->duration();
                magnitudeSweepEffect.iMagnitude = qRound(effect->intensity() * qreal(VIBE_MAX_MAGNITUDE));
                magnitudeSweepEffect.iStyle = CHWRMHaptics::EHWRMHapticsStyleSmooth; // XXX TODO: support this?
                magnitudeSweepEffect.iAttackTime = effect->attackTime();
                magnitudeSweepEffect.iAttackLevel = qRound(effect->attackIntensity() * qreal(VIBE_MAX_MAGNITUDE));
                magnitudeSweepEffect.iFadeTime = effect->fadeTime();
                magnitudeSweepEffect.iFadeLevel = qRound(effect->fadeIntensity() * qreal(VIBE_MAX_MAGNITUDE));
                TRAP(err, haptics(effect->actuator()->id())->PlayMagSweepEffect(magnitudeSweepEffect, effectHandle));
            } else {
                // find the handle for the effect
                CHWRMHaptics::THWRMHapticsPeriodicEffect periodicEffect;
                periodicEffect.iDuration = effect->duration();
                periodicEffect.iMagnitude = qRound(effect->intensity() * qreal(VIBE_MAX_MAGNITUDE));
                periodicEffect.iPeriod = effect->period();
                periodicEffect.iStyle = CHWRMHaptics::EHWRMHapticsStyleSmooth; // XXX TODO: support this?
                periodicEffect.iAttackTime = effect->attackTime();
                periodicEffect.iAttackLevel = qRound(effect->attackIntensity() * qreal(VIBE_MAX_MAGNITUDE));
                periodicEffect.iFadeTime = effect->fadeTime();
                periodicEffect.iFadeLevel = qRound(effect->fadeIntensity() * qreal(VIBE_MAX_MAGNITUDE));
                TRAP(err, haptics(effect->actuator()->id())->PlayPeriodicEffect(periodicEffect, effectHandle));
            }
            if (err == KErrNone) {
                // replacement insert operation.
                effectHandles.insert(effect, effectHandle);
                handleEffects.insert(effectHandle, effect);
            }
	}
        // fall through
    default:
        break;
    }

    // ugly hack: after starting the effect, we must update the intensity, or it doesn't work.  Bug?
    updateEffectProperty(effect, Intensity);

    if (err != KErrNone)
        reportError(effect, QFeedbackEffect::UnknownError);
}

QFeedbackEffect::State QFeedbackSymbian::effectState(const QFeedbackHapticsEffect *effect)
{
    TInt handle = effectHandles.value(effect);
    if (handle) {
        TInt state = CHWRMHaptics::EHWRMHapticsEffectNotPlaying;
        if (haptics(effect->actuator()->id())->GetEffectState(handle, state) == KErrNone) {
            switch (state) {
                case CHWRMHaptics::EHWRMHapticsEffectNotPlaying:
                default:
                    return QFeedbackEffect::Stopped;

                case CHWRMHaptics::EHWRMHapticsEffectPlaying:
                    return QFeedbackEffect::Running;

                case CHWRMHaptics::EHWRMHapticsEffectPaused:
                    return QFeedbackEffect::Paused;
            }
        }
    }
    return QFeedbackEffect::Stopped;
}

/* ============================== File Interface Functions ============================== */

CHWRMHaptics* QFeedbackSymbian::fileHaptics()
{
    // XXX Which actuator for file effects?
    // XXX TODO: add actuator selection API to QFeedbackFileEffect.
    return haptics(EHWRMLogicalActuatorAny);
}

// MHWRMHapticsObserver functions
void QFeedbackSymbian::EffectCompletedL(TInt error, TInt handle)
{
    Q_UNUSED(error); // The observer feedback function isn't called currently, so ignoring the error should be safe for now.
    QFeedbackEffect* effect = const_cast<QFeedbackEffect*>(handleEffects.value(handle));
    if (effect) {
        QMetaObject::invokeMethod(effect, "stateChanged");
    }
}

void QFeedbackSymbian::HapticsStatusChangedL(THWRMHapticsStatus aStatus)
{
    Q_UNUSED(aStatus);
    // We don't currently care.  Perhaps an actuator property should change?

}

void QFeedbackSymbian::setLoaded(QFeedbackFileEffect* effect, bool load)
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
            // We need to load the data and put it into haptics
            QFile file(fileName);
            success = false;
            if (file.size() < MAX_FILE_SIZE && file.open(QIODevice::ReadOnly)) {
                fc.ba = file.readAll();
                // XXX can we clear fc.ba after the load?
                if (fileHaptics()->LoadEffectData(fc.constData(), fc.fileHandle) == KErrNone) {
                    success = true;
                } else {
                    fileData.remove(fileName);
                }
            }
        }
        reportLoadFinished(effect, success);
    } else {
        //unload
        fc.refCount--;
        if (fc.refCount == 0) {
            fileHaptics()->DeleteEffectData(fc.fileHandle);
            fileData.remove(fileName);
        }

        // Also get rid of effectHandles
        TInt effectHandle = effectHandles.value(effect);
        if (effectHandle) {
            handleEffects.remove(effectHandle);
            effectHandles.remove(effect);
        }
    }
}

void QFeedbackSymbian::setEffectState(QFeedbackFileEffect * effect, QFeedbackEffect::State state)
{
    TInt effectHandle = effectHandles.value(effect);
    TInt status = 0;

    switch (state)
    {
    case QFeedbackEffect::Stopped:
        if (effectHandle) {
            status = fileHaptics()->StopPlayingEffect(effectHandle);
            // The handle is invalid after calling stop() on it; remove it from our hashes.
            effectHandles.remove(effect);
            handleEffects.remove(effectHandle);
        }
        break;
    case QFeedbackEffect::Paused:
        if (effectHandle)
            status = fileHaptics()->PausePlayingEffect(effectHandle);
        break;
    case QFeedbackEffect::Running:
        if (effectHandle) {
            // If the effect handle exists, we're resuming
            status = fileHaptics()->ResumePausedEffect(effectHandle);
        } else {
            // Assuming we've loaded the effect, we just need to start it
            QString fileName = effect->source().toLocalFile();
            Q_ASSERT(fileData.contains(fileName)); // false if we haven't loaded it yet (prevented by upper levels?)

            status = fileHaptics()->PlayEffect(fileData[fileName].fileHandle, 0, effectHandle);
            if (status == KErrNone) {
                effectHandles.insert(effect, effectHandle);
                handleEffects.insert(effectHandle, effect);
            }
        }
        break;
    default:
        break;
    }
}

QFeedbackEffect::State QFeedbackSymbian::effectState(const QFeedbackFileEffect * effect)
{
    TInt handle = effectHandles.value(effect);
    if (handle) {
        TInt state = CHWRMHaptics::EHWRMHapticsEffectNotPlaying;
        if (fileHaptics()->GetEffectState(handle, state) == KErrNone) {
            switch (state) {
                case CHWRMHaptics::EHWRMHapticsEffectNotPlaying:
                default:
                    return QFeedbackEffect::Stopped;

                case CHWRMHaptics::EHWRMHapticsEffectPlaying:
                    return QFeedbackEffect::Running;

                case CHWRMHaptics::EHWRMHapticsEffectPaused:
                    return QFeedbackEffect::Paused;
            }
        }
    }
    return QFeedbackEffect::Stopped;
}

int QFeedbackSymbian::effectDuration(const QFeedbackFileEffect* effect)
{
    TInt handle = effectHandles.value(effect);
    if (handle) {
        TInt duration = 0;
        if (fileHaptics()->GetEffectDuration(handle, 0, duration) == KErrNone)
            return duration;
    }
    return 0; // unknown
}

QStringList QFeedbackSymbian::supportedMimeTypes()
{
    return QStringList() << QLatin1String("vibra/ivt");
}
