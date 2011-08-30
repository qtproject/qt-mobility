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
#ifndef QFEEDBACK_SYMBIAN_H
#define QFEEDBACK_SYMBIA_H

#include <qmobilityglobal.h>

#include <qfeedbackplugininterfaces.h>

#include "../../../src/multimedia/effects/qsoundeffect_p.h"

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QFeedbackMediaObject;

class QFeedbackMMK : public QObject, public QFeedbackFileInterface
{
    Q_OBJECT
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackFileInterface)
public:
    QFeedbackMMK();
    virtual ~QFeedbackMMK();

    virtual void setLoaded(QFeedbackFileEffect*, bool);
    virtual void setEffectState(QFeedbackFileEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackFileEffect *);
    virtual int effectDuration(const QFeedbackFileEffect*);
    virtual QStringList supportedMimeTypes();
private Q_SLOTS:
    void soundEffectStatusChanged();
    void soundEffectPlayingChanged();

private:
    struct FeedbackInfo {
        FeedbackInfo() : soundEffect(0), loaded(false), playing(false) {}
        QSoundEffect* soundEffect;
        bool loaded;
        bool playing;
    };

    QHash<const QFeedbackFileEffect*, FeedbackInfo> mEffects;
    QHash<const QSoundEffect*, QFeedbackFileEffect*> mEffectMap;
};

QT_END_HEADER

#endif
