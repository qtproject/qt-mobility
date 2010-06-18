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

#include "qfeedback.h"
#include <QtCore/QtPlugin>
#include <phonon/backendcapabilities.h>
#include <phonon/audiooutput.h>

Q_EXPORT_PLUGIN2(feedback_phonon, QFeedbackPhonon)

QFeedbackPhonon::QFeedbackPhonon()
{
}

QFeedbackPhonon::~QFeedbackPhonon()
{
}

void QFeedbackPhonon::mediaObjectStateChanged()
{
    QFeedbackMediaObject *mediaObject = qobject_cast<QFeedbackMediaObject*>(sender());
    Q_ASSERT(mediaObject);
    mediaObject->overrideState = false;
    QFeedbackFileEffect *effect = mediaObject->effect;
    if (!effect->isLoading())
        return;
    switch(mediaObject->state())
    {
    case Phonon::LoadingState:
        //nothing to do, wait for the next state change
        break;
    case Phonon::StoppedState:
    case Phonon::PausedState:
    case Phonon::PlayingState:
        asyncLoadFinished(effect, true);
        break;
    case Phonon::ErrorState:
    default:
        //we need to delete the mediaobject
        mediaObject->deleteLater();
        audioPlayers.remove(effect);
        asyncLoadFinished(effect, false);
        break;
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
            asyncLoadFinished(effect, false); //an error occurred
        }

        audioPlayers[effect] = mediaObject;
        mediaObject->setCurrentSource(Phonon::MediaSource(effect->fileName()));


    } else {
        //unload
        delete audioPlayers[effect];
        audioPlayers.remove(effect);
    }

}

QFeedbackEffect::ErrorType QFeedbackPhonon::updateEffectState(QFeedbackFileEffect *effect)
{
    //the file should be loaded
    Q_ASSERT(audioPlayers.contains(effect));

    QFeedbackMediaObject *mediaObject = audioPlayers[effect];
    switch(effect->state()) 
    {
    case QAbstractAnimation::Stopped:
        mediaObject->stop();
        break;
    case QAbstractAnimation::Paused:
        mediaObject->pause();
        break;
    case QAbstractAnimation::Running:
        mediaObject->play();
        break;
    }

    //from now on until the state changed we need to override the state of the effect because
    //the change is not immediate
    mediaObject->overrideState = true;

    return QFeedbackFileEffect::NoError;
}

QAbstractAnimation::State QFeedbackPhonon::actualEffectState(const QFeedbackFileEffect *effect)
{
    QFeedbackMediaObject *mediaObject = audioPlayers.value(effect, 0);
    if (!mediaObject)
        return QAbstractAnimation::Stopped;

    if (mediaObject->overrideState)
        return effect->state();


    switch (mediaObject->state())
    {
    case Phonon::PlayingState:
        return QAbstractAnimation::Running;
    case Phonon::PausedState:
        return QAbstractAnimation::Paused;
    default:
        return QAbstractAnimation::Stopped;
    }
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

