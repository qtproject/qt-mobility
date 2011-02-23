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
#ifndef QFEEDBACK_SIMULATOR_H
#define QFEEDBACK_SIMULATOR_H

#include <qmobilityglobal.h>
#include "qfeedbackdata_simulator_p.h"
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QMutex>

#include <qfeedbackplugininterfaces.h>

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE
namespace Simulator { class Connection; }
QTM_END_NAMESPACE

class QFeedbackSimulator : public QObject, public QFeedbackHapticsInterface, public QFeedbackFileInterface, public QFeedbackThemeInterface
{
    Q_OBJECT
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackHapticsInterface)
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackFileInterface)
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackThemeInterface)
public:
    QFeedbackSimulator();
    virtual ~QFeedbackSimulator();

    // all interfaces
    virtual PluginPriority pluginPriority();

    // HapticsInterface
    virtual QList<QFeedbackActuator *> actuators();

    virtual void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &);
    virtual QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty);
    virtual bool isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability);

    virtual void updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty);
    virtual void setEffectState(const QFeedbackHapticsEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackHapticsEffect *);

    // FileInterface
    virtual void setLoaded(QFeedbackFileEffect*, bool);
    virtual void setEffectState(QFeedbackFileEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackFileEffect *);
    virtual int effectDuration(const QFeedbackFileEffect *);
    virtual QStringList supportedMimeTypes();

    // ThemeInterface
    virtual bool play(QFeedbackEffect::ThemeEffect);

signals:
    // used to call stateChanged on an effect
    void stateChangedProxySignal();

private:
    void setAnyEffectState(const QFeedbackEffect *effect, QFeedbackEffect::State state,
                           int actuatorId, int duration, const QString &info);
    QFeedbackEffect::State anyEffectState(const QFeedbackEffect *);
    void forceEffectState(int effectId, QFeedbackEffect::State state);
    void emitStateChanged(int effectId);

    void setActuator(const ActuatorData &data);
    void removeActuator(int id);
    void rebuildActuatorList();

    int mDefaultActuator;
    QList<QFeedbackActuator *> mActuatorList;
    QHash<int, QFeedbackActuator *> mActuators;
    QHash<int, ActuatorData> mActuatorData;
    QHash<const QFeedbackEffect *, QFeedbackEffect::State> mEffectStates;
    QHash<int, const QFeedbackEffect *> mIdToEffect;
    QHash<const QFeedbackEffect *, int> mEffectToId;

    // convenience
    Simulator::Connection *mConnection;
    friend class Simulator::Connection;
};


QT_END_HEADER

#endif // QFEEDBACK_SIMULATOR_H
