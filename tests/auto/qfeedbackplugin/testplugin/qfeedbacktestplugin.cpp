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
#include "qfeedbacktestplugin.h"
#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <QDebug>

Q_EXPORT_PLUGIN2(feedback_testplugin, QFeedbackTestPlugin)

QFeedbackTestPlugin::QFeedbackTestPlugin() : QObject(qApp)
{
}

QFeedbackTestPlugin::~QFeedbackTestPlugin()
{
}

QFeedbackInterface::PluginPriority QFeedbackTestPlugin::pluginPriority()
{
    return PluginLowPriority;
}

QList<QFeedbackActuator> QFeedbackTestPlugin::actuators()
{
    QList<QFeedbackActuator> ret;
    QFeedbackActuator testActuator = createFeedbackActuator(7357); // test actuator
    ret << testActuator;
    return ret;
}

void QFeedbackTestPlugin::setActuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop, const QVariant &value)
{
    Q_UNUSED(actuator)
    Q_UNUSED(prop)
    Q_UNUSED(value)
}

QVariant QFeedbackTestPlugin::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    Q_UNUSED(actuator)

    switch (prop) {
        case Name:
            return QString(QLatin1String("test plugin"));

        case State:
            return static_cast<int>(QFeedbackActuator::Unknown);

        case Enabled:
            return false;
        default:
            break;
    }

    return QVariant();
}

bool QFeedbackTestPlugin::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability cap)
{
    switch (cap) {
        case QFeedbackActuator::Envelope:
        case QFeedbackActuator::Period:
            return true;
        default:
            break;
    }

    return false;
}


void QFeedbackTestPlugin::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty ep)
{
    Q_UNUSED(effect)
    Q_UNUSED(ep)
}

void QFeedbackTestPlugin::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State state)
{
    Q_UNUSED(effect)
    Q_UNUSED(state)
}

QFeedbackEffect::State QFeedbackTestPlugin::effectState(const QFeedbackHapticsEffect *effect)
{
    Q_UNUSED(effect)
    return QFeedbackEffect::Stopped;
}

void QFeedbackTestPlugin::setLoaded(QFeedbackFileEffect *effect, bool load)
{
    Q_UNUSED(effect)
    Q_UNUSED(load)
}

void QFeedbackTestPlugin::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
{
    Q_UNUSED(effect)
    Q_UNUSED(state)
}

QFeedbackEffect::State QFeedbackTestPlugin::effectState(const QFeedbackFileEffect *effect)
{
    Q_UNUSED(effect)
    return QFeedbackEffect::Stopped;
}

int QFeedbackTestPlugin::effectDuration(const QFeedbackFileEffect *effect)
{
    Q_UNUSED(effect)
    return -1;
}

QStringList QFeedbackTestPlugin::supportedMimeTypes()
{
    return QStringList();
}
