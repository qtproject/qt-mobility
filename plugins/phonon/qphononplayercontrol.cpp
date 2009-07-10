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
#include <phonon>

#include "qmediaplaylistnavigator.h"
#include "qmediasource.h"

#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>


QMediaPlayerControl::QMediaPlayerControl(QObject *parent)
    :QAbstractMediaControl(parent)
{
}


QPhononPlayerControl::QPhononPlayerControl(Phonon::MediaObject *session, QObject *parent)
   :QMediaPlayerControl(parent), m_session(session)
{
    QMediaPlaylist *playlist = new QMediaPlaylist(0,this);
    m_navigator = new QMediaPlaylistNavigator(playlist,this);    
    m_navigator->setPlaybackMode(QMediaPlaylistNavigator::Linear);

    m_audioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::createPath(m_session, m_audioOutput);

    m_session->setTickInterval(1000);

    connect(m_navigator, SIGNAL(activated(QMediaSource)),
            this, SLOT(play(QMediaSource)));
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

QMediaSource QPhononPlayerControl::currentMediaSource() const
{
    return m_navigator->currentItem();
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
    switch (m_session->state()) {
        case Phonon::LoadingState: return int(QMediaPlayer::LoadingState);
        case Phonon::StoppedState: return int(QMediaPlayer::StoppedState);
        case Phonon::PlayingState: return int(QMediaPlayer::PlayingState);
        case Phonon::BufferingState: return int(QMediaPlayer::PausedState);
        case Phonon::PausedState: return int(QMediaPlayer::PausedState);
        case Phonon::ErrorState: return int(QMediaPlayer::ErrorState);
    }

    return int(QMediaPlayer::ErrorState);
}

bool QPhononPlayerControl::isBuffering() const
{
    return false;
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
    m_session->play();
}

void QPhononPlayerControl::pause()
{
    m_session->pause();
}

void QPhononPlayerControl::stop()
{
    m_session->stop();
}

void QPhononPlayerControl::setVolume(int volume)
{
    m_audioOutput->setVolume(volume/100.0);
}

void QPhononPlayerControl::setMuted(bool muted)
{
    m_audioOutput->setMuted(muted);
}

void QPhononPlayerControl::play(const QMediaSource &src)
{
    QVariant location = src.dataLocation();
    QUrl url;

    if (location.convert(QVariant::Url)) {
        url = location.toUrl();
    } else if (location.convert(QVariant::String)) {
        url = QUrl(location.toString());
    }

    if (url.isValid()) {
        m_session->stop();
        m_session->setCurrentSource(url);
        emit currentMediaChanged(src);
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

void QPhononPlayerControl::updateState()
{
    emit stateChanged(state());
}

void QPhononPlayerControl::updateVolume()
{
    emit volumeChanged(volume());
}
