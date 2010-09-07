
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

#include "s60mediaplayercontrol.h"
#include "s60mediaplayersession.h"

#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>

S60MediaPlayerControl::S60MediaPlayerControl(MS60MediaPlayerResolver& mediaPlayerResolver, QObject *parent)
    : QMediaPlayerControl(parent),
      m_mediaPlayerResolver(mediaPlayerResolver),
      m_session(NULL),
      m_stream(NULL)
{
}

S60MediaPlayerControl::~S60MediaPlayerControl()
{
}

qint64 S60MediaPlayerControl::position() const
{
    if (m_session)
        return m_session->position();
    return 0;
}

qint64 S60MediaPlayerControl::duration() const
{
    if (m_session)
        return m_session->duration();
    return -1;
}

QMediaPlayer::State S60MediaPlayerControl::state() const
{
    if (m_session)
        return m_session->state();
    return QMediaPlayer::StoppedState;
}

QMediaPlayer::MediaStatus S60MediaPlayerControl::mediaStatus() const
{   
    if (m_session)
        return m_session->mediaStatus();
    return m_mediaSettings.mediaStatus();
}

int S60MediaPlayerControl::bufferStatus() const
{
    if (m_session)
        return m_session->bufferStatus();
    return 0;
}

int S60MediaPlayerControl::volume() const
{
    if (m_session)
        return m_session->volume();
    return m_mediaSettings.volume();
}

bool S60MediaPlayerControl::isMuted() const
{
   if (m_session)
       return  m_session->isMuted();
   return m_mediaSettings.isMuted();
}

bool S60MediaPlayerControl::isSeekable() const
{
    if (m_session)
       return  m_session->isSeekable();
    return false;
}

QMediaTimeRange S60MediaPlayerControl::availablePlaybackRanges() const
{
    QMediaTimeRange ranges;

    if(m_session && m_session->isSeekable())
        ranges.addInterval(0, m_session->duration());

    return ranges;
}

qreal S60MediaPlayerControl::playbackRate() const
{
    // TODO: Add Symbian^3 support
    return m_mediaSettings.playbackRate();
}

void S60MediaPlayerControl::setPlaybackRate(qreal rate)
{
    // TODO: Add Symbian^3 support
    m_mediaSettings.setPlaybackRate(rate);
    emit playbackRateChanged(playbackRate());
    
}

void S60MediaPlayerControl::setPosition(qint64 pos)
{
    if (m_session)
        m_session->setPosition(pos);
}

void S60MediaPlayerControl::play()
{
    if (m_session)
        m_session->play();
}

void S60MediaPlayerControl::pause()
{
    if (m_session)
        m_session->pause();
}

void S60MediaPlayerControl::stop()
{
    if (m_session)
        m_session->stop();
}

void S60MediaPlayerControl::setVolume(int volume)
{
    int boundVolume = qBound(0, volume, 100);
    if (boundVolume == m_mediaSettings.volume())
        return;
    
    m_mediaSettings.setVolume(boundVolume);
    if (m_session)
        m_session->setVolume(boundVolume);

    emit volumeChanged(boundVolume);
}

void S60MediaPlayerControl::setMuted(bool muted)
{
    if (m_mediaSettings.isMuted() == muted)
        return;
    
    m_mediaSettings.setMuted(muted);
    if (m_session)
        m_session->setMuted(muted);
    
    emit mutedChanged(muted);
}

QMediaContent S60MediaPlayerControl::media() const
{
    return m_currentResource;
}

const QIODevice *S60MediaPlayerControl::mediaStream() const
{
    return m_stream;
}

void S60MediaPlayerControl::setMedia(const QMediaContent &source, QIODevice *stream)
{
    Q_UNUSED(stream)
    // we don't want to set & load media again when it is already loaded    
    if (m_session && m_currentResource == source)
        return;
    
    // store to variable as session is created based on the content type.
    m_currentResource = source;
    S60MediaPlayerSession *newSession = m_mediaPlayerResolver.PlayerSession();
    m_mediaSettings.setMediaStatus(QMediaPlayer::UnknownMediaStatus);
    
    if (m_session)
        m_session->reset();
    else {
        emit mediaStatusChanged(QMediaPlayer::UnknownMediaStatus);
        emit error(QMediaPlayer::NoError, QString());
    }
    
    m_session = newSession;
    
    if (m_session)
        m_session->load(source.canonicalUrl());
    else {
        QMediaPlayer::MediaStatus status = (source.isNull()) ? QMediaPlayer::NoMedia : QMediaPlayer::InvalidMedia;
        m_mediaSettings.setMediaStatus(status);
        emit stateChanged(QMediaPlayer::StoppedState);
        emit error((source.isNull()) ? QMediaPlayer::NoError : QMediaPlayer::ResourceError, 
                   (source.isNull()) ? "" : tr("Media couldn't be resolved"));
        emit mediaStatusChanged(status);
    }
    emit mediaChanged(m_currentResource);
  }

S60MediaPlayerSession* S60MediaPlayerControl::session()
{
    return m_session;
}

void S60MediaPlayerControl::setVideoOutput(QObject *output)
{
    m_mediaPlayerResolver.VideoPlayerSession()->setVideoRenderer(output);
}

bool S60MediaPlayerControl::isAudioAvailable() const
{
    if (m_session)
        return m_session->isAudioAvailable(); 
    return false;
}

bool S60MediaPlayerControl::isVideoAvailable() const
{
    if (m_session)
        return m_session->isVideoAvailable();
    return false;
}

const S60MediaSettings& S60MediaPlayerControl::mediaControlSettings() const
{
    return m_mediaSettings;
}

void S60MediaPlayerControl::setAudioEndpoint(const QString& name)
{
    m_mediaSettings.setAudioEndpoint(name);
}
