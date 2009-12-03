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

#include <QtCore/qdebug.h>

#include <QWidget>
#include <QDir>
#include <QVariant>
#include <QTimer>

S60MediaPlayerSession::S60MediaPlayerSession(QObject *parent)
    : QObject(parent),
      m_state(QMediaPlayer::StoppedState),
      m_mediaStatus(QMediaPlayer::NoMedia),
      m_volume(100),
      m_playbackRate(1.0),
      m_muted(false),
      m_timer(new QTimer(this))
{    
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

S60MediaPlayerSession::~S60MediaPlayerSession()
{
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
    return m_metaDataMap.value("seekable").toBool();
}

void S60MediaPlayerSession::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (m_mediaStatus != status) {
        m_mediaStatus = status;
        emit mediaStatusChanged(status);
    }
}

void S60MediaPlayerSession::setState(QMediaPlayer::State state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged(state);
    }
}

void S60MediaPlayerSession::setVideoRenderer(QObject *renderer)
{
    Q_UNUSED(renderer);   
}

QPair<qint64, qint64> S60MediaPlayerSession::seekRange() const
{
    return QPair<qint64, qint64>();
}

bool S60MediaPlayerSession::isMetadataAvailable() const
{
    return (!m_metaDataMap.isEmpty());
}

QVariant S60MediaPlayerSession::metaData(const QString &key) const
{
    return m_metaDataMap.value(key);
}

void S60MediaPlayerSession::tick()
{
    if (m_timer->isActive()) {
        emit positionChanged(position());
    }
}

bool S60MediaPlayerSession::startTimer()
{
    if (!m_timer->isActive()) {
        m_timer->start(1000);
        return true;
    }
    return false;
}

void S60MediaPlayerSession::stopTimer()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}
qreal S60MediaPlayerSession::playbackRate() const
{
    return m_playbackRate;
}

void S60MediaPlayerSession::setMuted(bool muted)
{
    if (m_muted != muted) {   
        m_muted = muted;
        emit mutingChanged(m_muted);
    }
}

void S60MediaPlayerSession::setVolume(int volume)
{
    if (m_volume != volume && volume >= 0 && volume <= 100) {
        m_volume = volume;
        emit volumeChanged(m_volume);
    }
}

void S60MediaPlayerSession::setPlaybackRate(qreal rate)
{
    if (m_playbackRate != rate) {
        m_playbackRate = rate;
        emit playbackRateChanged(m_playbackRate);
    }
}

const QString& S60MediaPlayerSession::filePath() const
{
    return m_filePath;
}

void S60MediaPlayerSession::setFilePath(const QString &path)
{
    m_filePath = path;
}

QMap<QString, QVariant>& S60MediaPlayerSession::metaDataEntries()
{
    return m_metaDataMap;
}

int S60MediaPlayerSession::milliSecondsToMicroSeconds(int milliSeconds) const
{
    return milliSeconds * 1000;
}

int S60MediaPlayerSession::microSecondsToMilliSeconds(int microSeconds) const
{
    return microSeconds / 1000;
}
