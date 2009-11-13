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

#include "s60mediaplayersession.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>

#include <QWidget>
#include <QDir>
#include <QVariant>

S60MediaPlayerSession::S60MediaPlayerSession(QObject *parent)
    : QObject(parent),
      m_state(QMediaPlayer::StoppedState),
      m_mediaStatus(QMediaPlayer::UnknownMediaStatus),
      m_volume(100),
      m_playbackRate(1.0),
      m_muted(false),
      m_videoAvailable(false),
      m_seekable(false),
      m_lastPosition(0),
      m_duration(-1)
{    
}

S60MediaPlayerSession::~S60MediaPlayerSession()
{
}

QUrl S60MediaPlayerSession::url() const
{
    return m_url;
}

qint64 S60MediaPlayerSession::duration() const
{
    return -1;
}

qint64 S60MediaPlayerSession::position() const
{
    return 0;
}

qreal S60MediaPlayerSession::playbackRate() const
{
    return -1;
}

void S60MediaPlayerSession::setPlaybackRate(qreal rate)
{
    m_playbackRate = rate;
}

bool S60MediaPlayerSession::isBuffering() const
{
    return false;
}

int S60MediaPlayerSession::bufferingProgress() const
{
    return 0;
}

int S60MediaPlayerSession::volume() const
{
    return m_volume;
}

bool S60MediaPlayerSession::isMuted() const
{
    return m_muted;
}

bool S60MediaPlayerSession::isVideoAvailable() const
{
    return false;
}

bool S60MediaPlayerSession::isSeekable() const
{
    return m_seekable;
}

void S60MediaPlayerSession::play()
{
}

void S60MediaPlayerSession::pause()
{
    m_state = QMediaPlayer::PausedState;
    emit stateChanged(QMediaPlayer::PausedState);
}

void S60MediaPlayerSession::stop()
{
    // TODO: Stop()
    m_state = QMediaPlayer::StoppedState;
    emit stateChanged(QMediaPlayer::StoppedState);
}

void S60MediaPlayerSession::seek(qint64 ms)
{
    // TODO:
}

void S60MediaPlayerSession::setVolume(int volume)
{
    // TODO:
}

void S60MediaPlayerSession::setMuted(bool muted)
{
    // TODO:
}

void S60MediaPlayerSession::setSeekable(bool seekable)
{
    if (seekable != m_seekable) {
        m_seekable = seekable;
        emit seekableChanged(m_seekable);
    }
}

void S60MediaPlayerSession::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (m_mediaStatus != status) {
        m_mediaStatus = status;
        emit mediaStatusChanged(status);
    }
}

void S60MediaPlayerSession::setVideoRenderer(QObject *renderer)
{
    
}

void S60MediaPlayerSession::setMedia(const QMediaContent&, QIODevice *)
{
    
}

QPair<qint64, qint64> S60MediaPlayerSession::seekRange() const
{
    
}

bool S60MediaPlayerSession::isMetadataAvailable()
{
    return false;
}

QVariant S60MediaPlayerSession::metaData(QtMedia::MetaData key)
{
    Q_UNUSED(key);
    return QVariant();
}
