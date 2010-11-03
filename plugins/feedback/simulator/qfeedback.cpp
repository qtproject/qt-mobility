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

#include <qfeedbackactuator.h>
#include "qfeedback.h"
#include "connection_p.h"
#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <QtCore/QMetaEnum>
#include <QDebug>

Q_EXPORT_PLUGIN2(feedback_simulator, QFeedbackSimulator)

QFeedbackSimulator::QFeedbackSimulator()
    : QObject(qApp)
    , mDefaultActuator(-1)
    , mConnection(Simulator::Connection::instance())
{
    mConnection->connect(this);
}

QFeedbackSimulator::~QFeedbackSimulator()
{
}

QFeedbackInterface::PluginPriority QFeedbackSimulator::pluginPriority()
{
    return PluginNormalPriority;
}

QList<QFeedbackActuator *> QFeedbackSimulator::actuators()
{
    return mActuatorList;
}

void QFeedbackSimulator::setActuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop, const QVariant &value)
{
    if (!mActuatorData.contains(actuator.id()))
        return;
    ActuatorData &data = mActuatorData[actuator.id()];

    switch (prop)
    {
    case Enabled:
        data.enabled = value.toBool();
        mConnection->setActuatorEnabled(actuator.id(), data.enabled);
        break;
    default:
        break;
    }
}

QVariant QFeedbackSimulator::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    if (!mActuatorData.contains(actuator.id()))
        return QVariant();
    const ActuatorData &data = mActuatorData.value(actuator.id());

    switch (prop)
    {
    case Name:
        return data.name;
    case State:
        return data.state;
    case Enabled:
        return data.enabled;
    default:
        break;
    }

    return QVariant();
}

bool QFeedbackSimulator::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability)
{
    return true;
}

static int totalEffectDuration(const QFeedbackHapticsEffect *effect)
{
    if (effect->period() != -1)
        return QFeedbackEffect::Infinite;
    return effect->attackTime() + effect->duration() + effect->fadeTime();
}

void QFeedbackSimulator::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty property)
{
    if (anyEffectState(effect) == QFeedbackEffect::Stopped
            || (property != AttackTime && property != Duration
                && property != FadeTime && property != Period)) {
        return; // nothing to do
    }

    int effectId = mEffectToId.value(effect, -1);
    if (effectId == -1) {
        reportError(effect, QFeedbackEffect::UnknownError);
        return;
    }

    mConnection->setEffectDuration(effectId, totalEffectDuration(effect));
}

void QFeedbackSimulator::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State state)
{
    const int totalDuration = totalEffectDuration(effect);
    setAnyEffectState(effect, state, effect->actuator()->id(), totalDuration,
                      tr("Custom haptics effect"));
}

QFeedbackEffect::State QFeedbackSimulator::effectState(const QFeedbackHapticsEffect *effect)
{
    return anyEffectState(effect);
}


void QFeedbackSimulator::setLoaded(QFeedbackFileEffect *, bool)
{
}

void QFeedbackSimulator::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
{
    setAnyEffectState(effect, state, mDefaultActuator, effect->duration(),
                      tr("File: %1").arg(effect->source().toString()));
}

QFeedbackEffect::State QFeedbackSimulator::effectState(const QFeedbackFileEffect *effect)
{
    return anyEffectState(effect);
}

int QFeedbackSimulator::effectDuration(const QFeedbackFileEffect *)
{
    // ### Make configurable.
    return 2000;
}

QStringList QFeedbackSimulator::supportedMimeTypes()
{
    return QStringList();
}

bool QFeedbackSimulator::play(QFeedbackEffect::ThemeEffect theme)
{
    if (mDefaultActuator == -1)
        return false;

    const QMetaEnum themeEnum = QFeedbackEffect::staticMetaObject.enumerator(
                QFeedbackEffect::staticMetaObject.indexOfEnumerator("ThemeEffect"));
    QString themeString = themeEnum.valueToKey(theme);

    int effectId = mConnection->startEffect(mDefaultActuator,
                                            themeString, 2000);
    if (effectId != -1) {
        mActuatorData[mDefaultActuator].state = QFeedbackActuator::Busy;
        return true;
    }
    return false;
}

void QFeedbackSimulator::setAnyEffectState(const QtMobility::QFeedbackEffect *effect,
                                           QFeedbackEffect::State state, int actuatorId,
                                           int duration, const QString &info)
{
    if (actuatorId == -1) {
        reportError(effect, QFeedbackEffect::UnknownError);
        return;
    }

    const int effectId = mEffectToId.value(effect, -1);

    QFeedbackEffect::State old = anyEffectState(effect);
    switch (state) {
    case QFeedbackEffect::Running: {
        switch (old) {
        case QFeedbackEffect::Running:
            return;
        case QFeedbackEffect::Stopped: {
            int effectId = mConnection->startEffect(actuatorId, info, duration);
            if (effectId != -1) {
                mIdToEffect.insert(effectId, effect);
                mEffectToId.insert(effect, effectId);
                forceEffectState(effectId, state);
                mActuatorData[actuatorId].state = QFeedbackActuator::Busy;
            } else {
                reportError(effect, QFeedbackEffect::DeviceBusy);
            }
            return;
        }
        case QFeedbackEffect::Paused: {
            if (effectId != -1 && mConnection->resumeEffect(effectId)) {
                forceEffectState(effectId, state);
            } else {
                reportError(effect, QFeedbackEffect::UnknownError);
            }
            return;
        }
        case QFeedbackEffect::Loading:
            reportError(effect, QFeedbackEffect::UnknownError);
            return;
        }
        break;
    }
    case QFeedbackEffect::Paused: {
        switch (old) {
        case QFeedbackEffect::Paused:
            return;
        case QFeedbackEffect::Running:
            if (effectId != -1 && mConnection->pauseEffect(effectId)) {
                forceEffectState(effectId, state);
            } else {
                reportError(effect, QFeedbackEffect::UnknownError);
            }
            return;
        case QFeedbackEffect::Stopped:
        case QFeedbackEffect::Loading:
            reportError(effect, QFeedbackEffect::UnknownError);
            return;
        }
        break;
    }
    case QFeedbackEffect::Stopped: {
        switch (old) {
        case QFeedbackEffect::Stopped:
            return;
        case QFeedbackEffect::Running:
        case QFeedbackEffect::Paused:
            if (effectId != -1 && mConnection->stopEffect(effectId)) {
                forceEffectState(effectId, state);
                mActuatorData[actuatorId].state = QFeedbackActuator::Ready;
            } else {
                reportError(effect, QFeedbackEffect::UnknownError);
            }
            return;
        case QFeedbackEffect::Loading:
            reportError(effect, QFeedbackEffect::UnknownError);
            return;
        }
        break;
    }
    case QFeedbackEffect::Loading: {
        switch (old) {
        case QFeedbackEffect::Loading:
            return;
        case QFeedbackEffect::Stopped:
            forceEffectState(effectId, state);
            return;
        case QFeedbackEffect::Paused:
        case QFeedbackEffect::Running:
            reportError(effect, QFeedbackEffect::UnknownError);
            return;
        }
        break;
    }
    }
}

QFeedbackEffect::State QFeedbackSimulator::anyEffectState(const QtMobility::QFeedbackEffect *effect)
{
    return mEffectStates.value(effect, QFeedbackEffect::Stopped);
}

void QFeedbackSimulator::forceEffectState(int effectId, QFeedbackEffect::State state)
{
    const QFeedbackEffect *effect = mIdToEffect.value(effectId);
    if (!effect)
        return;
    if (state != QFeedbackEffect::Stopped)
        mEffectStates.insert(effect, state);
    else {
        mEffectStates.remove(effect);
        mEffectToId.remove(effect);
        mIdToEffect.remove(effectId);
    }
}

void QFeedbackSimulator::setActuator(const QtMobility::ActuatorData &data)
{
    mActuatorData.insert(data.id, data);
    if (!mActuators.contains(data.id)) {
        mActuators.insert(data.id, createFeedbackActuator(this, data.id));
        rebuildActuatorList();
    }
}

void QFeedbackSimulator::removeActuator(int id)
{
    mActuatorData.remove(id);
    mActuators.remove(id);
    if (mDefaultActuator == id)
        mDefaultActuator = -1;
    rebuildActuatorList();
}

void QFeedbackSimulator::rebuildActuatorList()
{
    mActuatorList.clear();

    if (mActuators.isEmpty())
        return;

    // the first entry is the default actuator
    QFeedbackActuator *defaultActuator = 0;
    if (mDefaultActuator != -1) {
        defaultActuator = mActuators.value(mDefaultActuator);
        mActuatorList += defaultActuator;
    }

    foreach (QFeedbackActuator *actuator, mActuators) {
        if (actuator != defaultActuator)
            mActuatorList += actuator;
    }
}

