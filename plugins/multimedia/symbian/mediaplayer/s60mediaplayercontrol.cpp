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

#include "s60mediaplayercontrol.h"
#include "s60mediaplayersession.h"

#include <QMediaPlaylistNavigator>

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
    return QMediaPlayer::NoMedia;
}

int S60MediaPlayerControl::bufferStatus() const
{
    return -1;
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

QPair<qint64, qint64> S60MediaPlayerControl::seekRange() const
{
    if (m_session) {
        return q_check_ptr(m_session)->isSeekable()
            ? qMakePair<qint64, qint64>(0, m_session->duration())
            : qMakePair<qint64, qint64>(0, 0);
    }
    return QPair<qint64, qint64>();
}

qreal S60MediaPlayerControl::playbackRate() const
{
    if (m_session)
        return  m_session->playbackRate();
    return m_mediaSettings.playbackRate();
}

void S60MediaPlayerControl::setPlaybackRate(qreal rate)
{
    if (m_session)
        m_session->setPlaybackRate(rate);
    m_mediaSettings.setPlaybackRate(rate);
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
    if (m_mediaSettings.volume() != volume && volume >= 0 && volume <= 100) {
        m_mediaSettings.setVolume(volume);

        if (m_session)
            m_session->setVolume(volume);

        emit volumeChanged(volume);
    }
}

void S60MediaPlayerControl::setMuted(bool muted)
{
    if (m_session)
        m_session->setMuted(muted);
    else if (m_mediaSettings.isMuted() != muted) 
        emit mutedChanged(muted);
    m_mediaSettings.setMuted(muted);
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
    if (m_session && (m_currentResource == source) && m_session->state() == QMediaPlayer::LoadedMedia) {
        return;
    }

    // store to variable as session is created based on the content type.
    m_currentResource = source;
    if (m_session) {
        m_session->stop();
    }
    m_session = currentPlayerSession();

    QUrl url;
    if (m_session && !source.isNull()) {
        url = source.canonicalUrl();
		if (m_session->isUrl() == false) {
			m_session->load(url);	
		} else {
			m_session->loadUrl(url);
		}
		emit mediaChanged(m_currentResource);
    }
    else {
        emit mediaStatusChanged(QMediaPlayer::InvalidMedia);
    }

}

void S60MediaPlayerControl::setVideoOutput(QObject *output)
{
    if (!m_session)
        m_session = m_mediaPlayerResolver.VideoPlayerSession();
    m_session->setVideoRenderer(output);
}

bool S60MediaPlayerControl::isVideoAvailable() const
{
    if (m_session)
        return m_session->isVideoAvailable();
    return false;
}

S60MediaPlayerSession* S60MediaPlayerControl::currentPlayerSession() 
{
    return m_mediaPlayerResolver.PlayerSession();   
}

const S60MediaSettings& S60MediaPlayerControl::mediaControlSettings() const
{
    return m_mediaSettings;
}
