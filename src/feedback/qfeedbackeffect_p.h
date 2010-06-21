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

#ifndef QFEEDBACKEFFECT_P_H
#define QFEEDBACKEFFECT_P_H

#include "qfeedbackeffect.h"
#include "qfeedbackactuator.h"

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


QTM_BEGIN_NAMESPACE

class QFeedbackHapticsEffectPrivate
{
public:
    QFeedbackHapticsEffectPrivate() :
                    duration(250), attackTime(0), fadeTime(0), period(-1),
                    intensity(1), attackIntensity(0), fadeIntensity(0)
    {

    }

    // Try to avoid holes (mostly where qreal == double)
    int duration;
    int attackTime;
    int fadeTime;
    int period;
    QFeedbackActuator actuator;
    qreal intensity;
    qreal attackIntensity;
    qreal fadeIntensity;
};

class QFeedbackFileEffectPrivate
{
public:
    QFeedbackFileEffectPrivate(QFeedbackFileEffect *effect) : effect(effect), loaded(false), isLoading(false), backendUsed(-1)
    {
    }

    static QFeedbackFileEffectPrivate *get(QFeedbackFileEffect *e) { return e->priv.data(); }
    static const QFeedbackFileEffectPrivate *get(const QFeedbackFileEffect *e) { return e->priv.data(); }

    void loadFinished(bool success);

    QFeedbackFileEffect *effect;

    QString fileName;
    bool loaded;

    //used for loading the file
    bool isLoading;
    int backendUsed;
};

QTM_END_NAMESPACE


#endif
