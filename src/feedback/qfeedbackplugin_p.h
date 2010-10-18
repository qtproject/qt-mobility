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

#ifndef QFEEDBACKPLUGIN_P_H
#define QFEEDBACKPLUGIN_P_H

#include <qmobilityglobal.h>
#include "qfeedbackactuator.h"
#include "qfeedbackeffect.h"
#include "qfeedbackplugininterfaces.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QVariant>


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Feedback framework.  This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//
//

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QDummyBackend : QObject, public QFeedbackHapticsInterface
{
public:
    QDummyBackend() : QObject(qApp) { }

    QList<QFeedbackActuator*> actuators() { return QList<QFeedbackActuator*>(); }

    void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &) { }
    QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty) { return QVariant(); }
    bool isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability) { return false; }

    void updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty) { }
    void setEffectState(const QFeedbackHapticsEffect *, QFeedbackEffect::State) { }
    QFeedbackEffect::State effectState(const QFeedbackHapticsEffect *) { return QFeedbackEffect::Stopped; }

    virtual bool play(QFeedbackEffect::ThemeEffect effect) { Q_UNUSED(effect); return false; }
    virtual PluginPriority pluginPriority() { return PluginLowPriority; }
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
