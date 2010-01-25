/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qphononplayercontrol.h"

#include <qmediaplaylistnavigator.h>

#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>

QPhononPlayerControl::QPhononPlayerControl(Phonon::MediaObject *session, QObject *parent)
   : QMediaPlayerControl(parent)
   , m_session(session)
   , m_state(QMediaPlayer::StoppedState)
   , m_mediaStatus(QMediaPlayer::NoMedia)
   , m_mediaStream(0)
{
    m_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::createPath(m_session, m_audioOutput);

    m_session->setTickInterval(1000);

    connect(m_session, SIGNAL(tick(qint64)),
            this, SIGNAL(positionChanged(qint64)));
    connect(m_session, SIGNAL(totalTimeChanged(qint64)),
            this, SIGNAL(durationChanged(qint64)));

    connect(m_audioOutput, SIGNAL(mutedChanged(bool)),
            this, SIGNAL(mutedChanged(bool)));
    connect(m_audioOutput, SIGNAL(volumeChanged(qreal)),
            this, SLOT(updateVolume()));

    connect(m_session, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(updateState(Phonon::State,Phonon::State)));
    connect(m_session, SIGNAL(hasVideoChanged(bool)),
            this, SIGNAL(videoAvailableChanged(bool)));
    connect(m_session, SIGNAL(seekableChanged(bool)),
            this, SIGNAL(seekableChanged(bool)));
    connect(m_session, SIGNAL(finished()),
            this, SLOT(processEOS()));
}

QPhononPlayerControl::~QPhononPlayerControl()
{
}

qint64 QPhononPlayerControl::position() const
{
    return m_session->currentTime();
}

qint64 QPhononPlayerControl::duration() const
{
    return m_session->totalTime();
}

QMediaPlayer::State QPhononPlayerControl::state() const
{
    return m_state;
}

QMediaPlayer::MediaStatus QPhononPlayerControl::mediaStatus() const
{
    return m_mediaStatus;
}

int QPhononPlayerControl::bufferStatus() const
{
    return 100;
}

int QPhononPlayerControl::volume() const
{
    return int(m_audioOutput->volume()*100);
}

bool QPhononPlayerControl::isMuted() const
{
    return m_audioOutput->isMuted();
}

bool QPhononPlayerControl::isSeekable() const
{
    return m_session->isSeekable();
}

QMediaTimeRange QPhononPlayerControl::availablePlaybackRanges() const
{
    QMediaTimeRange ranges;

    if(m_session->isSeekable())
        ranges.addInterval(0, m_session->totalTime());

    return ranges;
}

qreal QPhononPlayerControl::playbackRate() const
{
    return 1;
}

void QPhononPlayerControl::setPlaybackRate(qreal rate)
{
    Q_UNUSED(rate);
}

void QPhononPlayerControl::setPosition(qint64 pos)
{
    m_session->seek(pos);
}

void QPhononPlayerControl::play()
{
    m_state = QMediaPlayer::PlayingState;

    m_session->play();

    if (m_state == QMediaPlayer::PlayingState)
        emit stateChanged(m_state);

}

void QPhononPlayerControl::pause()
{
    m_state = QMediaPlayer::PausedState;

    m_session->pause();

    if (m_state == QMediaPlayer::PausedState)
        emit stateChanged(m_state);
}

void QPhononPlayerControl::stop()
{
    m_state = QMediaPlayer::StoppedState;

    m_session->stop();

    if (m_state == QMediaPlayer::StoppedState)
        emit stateChanged(m_state);
}

void QPhononPlayerControl::setVolume(int volume)
{
    m_audioOutput->setVolume(volume/100.0);
}

void QPhononPlayerControl::setMuted(bool muted)
{
    m_audioOutput->setMuted(muted);
}

QMediaContent QPhononPlayerControl::media() const
{
    return m_resources;
}

const QIODevice *QPhononPlayerControl::mediaStream() const
{
    return m_mediaStream;
}

void QPhononPlayerControl::setMedia(const QMediaContent &content, QIODevice *stream)
{
    m_resources = content;
    m_mediaStream = stream;

    QUrl url;

    if (!content.isNull())
        url = content.canonicalUrl();

    m_session->stop();
    if (m_mediaStream)
        m_session->setCurrentSource(Phonon::MediaSource(m_mediaStream));
    else
        m_session->setCurrentSource(Phonon::MediaSource(url));
}

bool QPhononPlayerControl::isVideoAvailable() const
{
    return m_session->hasVideo();
}

void QPhononPlayerControl::updateState(Phonon::State newState, Phonon::State oldState)
{
    switch (newState) {
    case Phonon::LoadingState:
        break;
    case Phonon::StoppedState:
        m_mediaStatus = m_session->currentSource().type() != Phonon::MediaSource::Invalid
                ? QMediaPlayer::LoadedMedia
                : QMediaPlayer::NoMedia;
        emit stateChanged(m_state = QMediaPlayer::StoppedState);
        emit mediaStatusChanged(m_mediaStatus);
        break;
    case Phonon::PlayingState:
        m_mediaStatus = QMediaPlayer::BufferedMedia;
        emit stateChanged(m_state = QMediaPlayer::PlayingState);
        emit mediaStatusChanged(m_mediaStatus);
        break;
    case Phonon::PausedState:
        m_mediaStatus = QMediaPlayer::BufferedMedia;
        emit stateChanged(m_state = QMediaPlayer::PausedState);
        emit mediaStatusChanged(m_mediaStatus);
        break;
    case Phonon::BufferingState:
        m_mediaStatus = QMediaPlayer::BufferingMedia;
        if (oldState == Phonon::StoppedState)
            emit stateChanged(m_state = QMediaPlayer::PlayingState);
        emit mediaStatusChanged(m_mediaStatus);
        break;
    case Phonon::ErrorState:
        if (m_session->errorType() == Phonon::FatalError && m_state != QMediaPlayer::StoppedState)
            emit stateChanged(m_state = QMediaPlayer::StoppedState);
        break;
    }
}

void QPhononPlayerControl::processEOS()
{
    m_mediaStatus = QMediaPlayer::EndOfMedia;
    emit stateChanged(m_state = QMediaPlayer::StoppedState);
    emit mediaStatusChanged(m_mediaStatus);
}

void QPhononPlayerControl::updateVolume()
{
    emit volumeChanged(volume());
}
