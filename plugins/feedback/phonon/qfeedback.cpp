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

#include "qfeedback.h"
#include <QtCore/QtPlugin>
#include <QtCore/QCoreApplication>
#include <phonon/backendcapabilities.h>
#include <phonon/audiooutput.h>

Q_EXPORT_PLUGIN2(feedback_phonon, QFeedbackPhonon)

static QFeedbackEffect::State convertToEffectState(Phonon::State state)
{
    switch(state)
    {
    case Phonon::BufferingState:
    case Phonon::LoadingState:
        return QFeedbackEffect::Loading;
    case Phonon::PausedState:
        return QFeedbackEffect::Paused;
    case Phonon::PlayingState:
        return QFeedbackEffect::Running;
    case Phonon::ErrorState:
    case Phonon::StoppedState:
    default:
        return QFeedbackEffect::Stopped;
    }
}

QFeedbackPhonon::QFeedbackPhonon() : QObject(qApp)
{
}

QFeedbackPhonon::~QFeedbackPhonon()
{
}

void QFeedbackPhonon::mediaObjectStateChanged()
{
    QFeedbackMediaObject *mediaObject = qobject_cast<QFeedbackMediaObject*>(sender());
    Q_ASSERT(mediaObject);
    QFeedbackFileEffect *effect = mediaObject->effect;

    switch(mediaObject->state())
    {
    case Phonon::BufferingState:
    case Phonon::LoadingState:
        //nothing to do, wait for the next state change
        break;
    case Phonon::StoppedState:
    case Phonon::PausedState:
    case Phonon::PlayingState:
        reportLoadFinished(effect, true);
        break;
    case Phonon::ErrorState:
    default:
        reportError(effect, QFeedbackEffect::UnknownError);
        //we need to delete the mediaobject
        mediaObject->deleteLater();
        audioPlayers.remove(effect);
        reportLoadFinished(effect, false);
    }
}

void QFeedbackPhonon::setLoaded(QFeedbackFileEffect *effect, bool load)
{
    Q_ASSERT(audioPlayers.contains(effect) != load);

    if (load) {
        //let's create the possibility to laod a file
        QFeedbackMediaObject *mediaObject = new QFeedbackMediaObject(this, effect);
        Phonon::AudioOutput *audio = new Phonon::AudioOutput(mediaObject);
        if (!Phonon::createPath(mediaObject, audio).isValid()) {
            mediaObject->deleteLater();
            reportLoadFinished(effect, false); //an error occurred
        }

        audioPlayers[effect] = mediaObject;
        mediaObject->setCurrentSource(Phonon::MediaSource(effect->fileName()));


    } else {
        //unload
        delete audioPlayers[effect];
        audioPlayers.remove(effect);
    }

}

void QFeedbackPhonon::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
{
    //the file should be loaded
    Q_ASSERT(audioPlayers.contains(effect));

    QFeedbackMediaObject *mediaObject = audioPlayers[effect];
    switch(state) 
    {
    case QFeedbackEffect::Stopped:
        mediaObject->stop();
        break;
    case QFeedbackEffect::Paused:
        mediaObject->pause();
        break;
    case QFeedbackEffect::Running:
        mediaObject->play();
        break;
    }
}

QFeedbackEffect::State QFeedbackPhonon::effectState(const QFeedbackFileEffect *effect)
{
    QFeedbackMediaObject *mediaObject = audioPlayers.value(effect, 0);
    if (!mediaObject)
        return QFeedbackEffect::Stopped;

    return convertToEffectState(mediaObject->state());
}

int QFeedbackPhonon::effectDuration(const QFeedbackFileEffect *effect)
{
    Phonon::MediaObject *mediaObject = audioPlayers.value(effect, 0);
    if (!mediaObject)
        return 0;
    return mediaObject->totalTime();
}

QStringList QFeedbackPhonon::supportedMimeTypes()
{
    return Phonon::BackendCapabilities::availableMimeTypes();
}

