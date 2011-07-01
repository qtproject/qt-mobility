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

#include <QtCore/QtPlugin>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include <QDebug>
#include "qfeedback.h"

Q_EXPORT_PLUGIN2(feedback_mmk, QFeedbackMMK)

QFeedbackMMK::QFeedbackMMK() : QObject(qApp)
{
}

QFeedbackMMK::~QFeedbackMMK()
{
    foreach(FeedbackInfo fi, mEffects.values()) {
        delete fi.soundEffect;
    }
}

void QFeedbackMMK::setLoaded(QFeedbackFileEffect *effect, bool load)
{
    if (!effect)
        return;

    // See if we have seen this effect before
    FeedbackInfo fi = mEffects.value(effect);

    if (load) {
        // Well.. we might already have an effect, since we don't create them until
        // we load...
        if (fi.loaded) {
            // We've already loaded?
            return;
        } else {
            if (fi.soundEffect) {
                // We've started a load, they must just be impatient
                // Pushing this elevator button does nothing..
                return;
            } else {
                // New sound effect!
                QUrl url = effect->source();
                if (QFile::exists(url.toLocalFile())) {
                    fi.soundEffect = new QSoundEffect(this);
                    mEffects.insert(effect, fi);
                    mEffectMap.insert(fi.soundEffect, effect);

                    connect(fi.soundEffect, SIGNAL(statusChanged()), this, SLOT(soundEffectStatusChanged()));
                    connect(fi.soundEffect, SIGNAL(playingChanged()), this, SLOT(soundEffectPlayingChanged()));
                    fi.soundEffect->setSource(url);

                    // conceptually we're now loading, so we have to do this manually??
                    QMetaObject::invokeMethod(effect, "stateChanged");
                } else {
                    reportLoadFinished(effect, false);
                }
            }
        }
    } else {
        // Time to unload.
        if (fi.soundEffect) {
            mEffectMap.remove(fi.soundEffect);
            fi.soundEffect->deleteLater();
        }
        mEffects.remove(effect);
    }
}

void QFeedbackMMK::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
{
    FeedbackInfo fi = mEffects.value(effect);
    switch (state)
    {
        case QFeedbackEffect::Stopped:
            if (fi.playing) {
                Q_ASSERT(fi.soundEffect);
                fi.playing = false;
                mEffects.insert(effect, fi); // overwrite previous version
                fi.soundEffect->stop();
            }
            break;

        case QFeedbackEffect::Paused:
            // Well, we can't pause, really
            reportError(effect, QFeedbackEffect::UnknownError);
            break;

        case QFeedbackEffect::Running:
            if (fi.playing) {
                // We're already playing.
            } else if (fi.soundEffect) {
                fi.playing = true;
                mEffects.insert(effect, fi); // overwrite previous version
                fi.soundEffect->play();
            }
            break;
        default:
            break;
    }
}

QFeedbackEffect::State QFeedbackMMK::effectState(const QFeedbackFileEffect *effect)
{
    FeedbackInfo fi = mEffects.value(effect);
    if (fi.soundEffect) {
        if (fi.playing) // we might not be loaded, however
            return QFeedbackEffect::Running;
        if (fi.loaded)
            return QFeedbackEffect::Stopped; // No idle?
        return QFeedbackEffect::Loading;
    }
    return QFeedbackEffect::Stopped;
}

int QFeedbackMMK::effectDuration(const QFeedbackFileEffect *effect)
{
    Q_UNUSED(effect);
    // XXX This isn't supported by MMK currently
    return 0;
}

QStringList QFeedbackMMK::supportedMimeTypes()
{
    return QSoundEffect::supportedMimeTypes();
}

void QFeedbackMMK::soundEffectStatusChanged()
{
    QSoundEffect* se = qobject_cast<QSoundEffect*>(sender());
    if (se) {
        // Hmm, now look up the right sound effect
        QFeedbackFileEffect* fe = mEffectMap.value(se);
        if (!fe)
            return;

        FeedbackInfo fi = mEffects.value(fe);

        switch(se->status()) {
            case QSoundEffect::Error:
                if (!fi.soundEffect || !fi.loaded) {
                    // Error before we got loaded, so fail the load
                    mEffectMap.remove(se);
                    mEffects.remove(fe);
                    se->deleteLater();
                    reportLoadFinished(fe, false); // this ends our involvement
                } else {
                    reportError(fe, QFeedbackEffect::UnknownError); // this doesn't do much
                }
                break;

            case QSoundEffect::Ready:
                if (fe->state() == QFeedbackEffect::Loading) {
                    reportLoadFinished(fe, true);

                    FeedbackInfo fi = mEffects.value(fe);
                    fi.loaded = true;
                    mEffects.insert(fe, fi);

                    QMetaObject::invokeMethod(fe, "stateChanged");
                }
                break;

            default:
                // Nothing to handle here?
                break;
        }
    }
}

void QFeedbackMMK::soundEffectPlayingChanged()
{
    QSoundEffect* se = qobject_cast<QSoundEffect*>(sender());
    if (se) {
        QFeedbackFileEffect* fileEffect = mEffectMap.value(se);
        FeedbackInfo fi = mEffects.value(fileEffect);

        if (fi.soundEffect == se) {
            if (fi.playing != se->isPlaying()) {
                fi.playing = se->isPlaying();
                mEffects.insert(fileEffect, fi); // overwrite previous version

                QFeedbackFileEffect* fe = mEffectMap.value(se);
                // Emit the stateChanged() signal
                if (fe) {
                    QMetaObject::invokeMethod(fe, "stateChanged");
                }
            } else {
                // Do nothing, internal state is already ok
            }
        }
    }
}
