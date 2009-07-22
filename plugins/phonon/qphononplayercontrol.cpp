/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qphononplayercontrol.h"

#include "qmediaplaylistnavigator.h"

#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>

QPhononPlayerControl::QPhononPlayerControl(Phonon::MediaObject *session, QObject *parent)
   :QMediaPlayerControl(parent), m_session(session), m_state(QMediaPlayer::StoppedState)
{
    QMediaPlaylist *playlist = new QMediaPlaylist(0,this);
    m_navigator = new QMediaPlaylistNavigator(playlist,this);    
    m_navigator->setPlaybackMode(QMediaPlaylistNavigator::Linear);

    m_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::createPath(m_session, m_audioOutput);

    m_session->setTickInterval(1000);

    connect(m_navigator, SIGNAL(activated(QMediaResourceList)),
            this, SLOT(play(QMediaResourceList)));
    connect(m_navigator, SIGNAL(currentPositionChanged(int)),
            this, SIGNAL(playlistPositionChanged(int)));

    connect(m_session, SIGNAL(finished()),
            m_navigator, SLOT(advance()));

    connect(m_session, SIGNAL(tick(qint64)),
            this, SIGNAL(positionChanged(qint64)));
    connect(m_session, SIGNAL(totalTimeChanged(qint64)),
            this, SIGNAL(durationChanged(qint64)));

    connect(m_audioOutput, SIGNAL(mutedChanged(bool)),
            this, SIGNAL(mutingChanged(bool)));
    connect(m_audioOutput, SIGNAL(volumeChanged(qreal)),
            this, SLOT(updateVolume()));

    connect(m_session, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(updateState()));
    connect(m_session, SIGNAL(hasVideoChanged(bool)),
            this, SIGNAL(videoAvailabilityChanged(bool)));
    connect(m_session, SIGNAL(seekableChanged(bool)),
            this, SIGNAL(seekableChanged(bool)));
}

QPhononPlayerControl::~QPhononPlayerControl()
{
}

int QPhononPlayerControl::playlistPosition() const
{
    return m_navigator->currentPosition();
}

qint64 QPhononPlayerControl::position() const
{
    return m_session->currentTime();
}

qint64 QPhononPlayerControl::duration() const
{
    return m_session->totalTime();
}

int QPhononPlayerControl::state() const
{
    return m_state;
}

bool QPhononPlayerControl::isBuffering() const
{
    return m_session->state() == Phonon::BufferingState;
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

float QPhononPlayerControl::playbackRate() const
{
    return 1;
}

void QPhononPlayerControl::setPlaybackRate(float rate)
{
    Q_UNUSED(rate);
}

void QPhononPlayerControl::setPlaylistPosition(int playlistPosition)
{
    m_navigator->jump(playlistPosition);
}

void QPhononPlayerControl::advance()
{
    m_navigator->advance();
}

void QPhononPlayerControl::back()
{
    m_navigator->back();
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

void QPhononPlayerControl::play(const QMediaResourceList &resources)
{
    QUrl url;

    if (!resources.isEmpty())
        url = resources.first().uri();

    if (url.isValid()) {
        m_session->stop();
        m_session->setCurrentSource(url);
        m_session->play();        
    } else {
        m_navigator->advance();
    }
}

QMediaPlaylist *QPhononPlayerControl::mediaPlaylist() const
{
    return m_navigator->playlist();
}

bool QPhononPlayerControl::setMediaPlaylist(QMediaPlaylist *playlist)
{
    m_navigator->setPlaylist(playlist);
    return true;
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
        if (m_state != QMediaPlayer::StoppedState)
            emit stateChanged(m_state = QMediaPlayer::StoppedState);
        break;
    case Phonon::PlayingState:
        if (m_state != QMediaPlayer::PlayingState)
            emit stateChanged(m_state = QMediaPlayer::PlayingState);
        break;
    case Phonon::PausedState:
        if (m_state != QMediaPlayer::PausedState)
            emit stateChanged(m_state = QMediaPlayer::PausedState);
        break;
    case Phonon::BufferingState:
        emit bufferingChanged(true);
        break;
    case Phonon::ErrorState:
        if (m_session->errorType() == Phonon::FatalError && m_state != QMediaPlayer::StoppedState)
            emit stateChanged(m_state = QMediaPlayer::StoppedState);
        break;

    }

    if (oldState == Phonon::BufferingState)
        emit bufferingChanged(false);
}

void QPhononPlayerControl::updateVolume()
{
    emit volumeChanged(volume());
}
