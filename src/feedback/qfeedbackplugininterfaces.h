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

#ifndef QFEEDBACKPLUGIN_H
#define QFEEDBACKPLUGIN_H

#include <qmobilityglobal.h>
#include "qfeedbackactuator.h"
#include "qfeedbackeffect.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class Q_FEEDBACK_EXPORT QFeedbackInterface
{
public:
    enum PluginPriority
    {
        PluginLowPriority,
        PluginNormalPriority,
        PluginHighPriority
    };

protected:
    static void reportError(const QFeedbackEffect *, QFeedbackEffect::ErrorType);
};

class Q_FEEDBACK_EXPORT QFeedbackHapticsInterface : public QFeedbackInterface
{
public:
    //going with enums allow more flexibility without breaking BC
    enum EffectProperty
    {
        Duration,
        Intensity,
        AttackTime,
        AttackIntensity,
        FadeTime,
        FadeIntensity,
        Period //optional
    };

    enum ActuatorProperty
    {
        Name,
        State,
        Enabled
    };

    //static members for actuators management
    virtual QList<QFeedbackActuator> actuators() = 0;
    virtual ~QFeedbackHapticsInterface() {}

    virtual PluginPriority pluginPriority() = 0;

    //for actuator handling
    virtual void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &) = 0;
    virtual QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty) = 0;
    virtual bool isActuatorCapabilitySupported(QFeedbackActuator::Capability) = 0;
    
    //effects
    virtual void updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty) = 0;
    virtual void setEffectState(const QFeedbackHapticsEffect *, QFeedbackEffect::State) = 0;
    virtual QFeedbackEffect::State effectState(const QFeedbackHapticsEffect *) = 0;

    static QFeedbackHapticsInterface *instance();

protected:
    //utility function for the backends
    QFeedbackActuator createFeedbackActuator(int id);
};

class QFeedbackThemeInterface : public QFeedbackInterface
{
public:
    virtual ~QFeedbackThemeInterface() {}
    virtual PluginPriority pluginPriority() = 0;
    virtual bool play(QFeedbackEffect::ThemeEffect) = 0;
    static QFeedbackThemeInterface *instance();
};

class Q_FEEDBACK_EXPORT QFeedbackFileInterface : public QFeedbackInterface
{
public:
    virtual ~QFeedbackFileInterface() {}
    virtual void setLoaded(QFeedbackFileEffect*, bool) = 0;
    virtual void setEffectState(QFeedbackFileEffect *, QFeedbackEffect::State) = 0;
    virtual QFeedbackEffect::State effectState(const QFeedbackFileEffect *) = 0;
    virtual int effectDuration(const QFeedbackFileEffect*) = 0;
    virtual QStringList supportedMimeTypes() = 0;

    static QFeedbackFileInterface *instance();

protected:
    //function called from the plugin when it has finished loading a file
    static void asyncLoadFinished(QFeedbackFileEffect*, bool success);

};


QTM_END_NAMESPACE

Q_DECLARE_INTERFACE( QTM_NAMESPACE::QFeedbackHapticsInterface, "com.nokia.qt.QFeedbackHapticsInterface/1.0")
Q_DECLARE_INTERFACE( QTM_NAMESPACE::QFeedbackThemeInterface, "com.nokia.qt.QFeedbackThemeInterface/1.0")
Q_DECLARE_INTERFACE( QTM_NAMESPACE::QFeedbackFileInterface, "com.nokia.qt.QFeedbackFileInterface/1.0")

QT_END_HEADER

#endif
