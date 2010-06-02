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

#ifndef QFEEDBACKPLUGIN_H
#define QFEEDBACKPLUGIN_H

#include <qmobilityglobal.h>
#include "qfeedbackdevice.h"
#include "qfeedbackeffect.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class Q_FEEDBACK_EXPORT QFeedbackInterface
{
public:
    enum EffectProperty
    {
        Duration,
        Intensity,
        AttackTime,
        AttackIntensity,
        FadeTime,
        FadeIntensity,
        Period,
        Device
    };
    virtual ~QFeedbackInterface() { }

    //static members for devices management
    virtual QFeedbackDevice defaultDevice(QFeedbackDevice::Type) = 0;
    virtual QList<QFeedbackDevice> devices() = 0;

    //for device handling
    virtual QString deviceName(const QFeedbackDevice &) = 0;
    virtual QFeedbackDevice::State deviceState(const QFeedbackDevice &) = 0;
    virtual QFeedbackDevice::Capabilities supportedCapabilities(const QFeedbackDevice &) = 0;
    virtual QFeedbackDevice::Type type(const QFeedbackDevice &) = 0;
    virtual bool isEnabled(const QFeedbackDevice &) = 0;
    virtual void setEnabled(const QFeedbackDevice &, bool) = 0;

    //effects
    virtual QFeedbackEffect::ErrorType updateEffectProperty(const QFeedbackEffect *, EffectProperty) = 0;
    virtual QFeedbackEffect::ErrorType updateEffectState(const QFeedbackEffect *) = 0;
    virtual QAbstractAnimation::State actualEffectState(const QFeedbackEffect *) = 0;

    static QFeedbackInterface *instance();

protected:
    //utility function for the backends
    QFeedbackDevice createFeedbackDevice(int id);
};

class QThemedFeedbackInterface
{
public:
    virtual void play(QFeedbackEffect::InstantEffect) = 0;
    static QThemedFeedbackInterface *instance();
};


QTM_END_NAMESPACE

Q_DECLARE_INTERFACE(QtMobility::QFeedbackInterface, "com.nokia.qt.QFeedbackInterface")
Q_DECLARE_INTERFACE(QtMobility::QThemedFeedbackInterface, "com.nokia.qt.QThemedFeedbackInterface")

QT_END_HEADER

#endif
