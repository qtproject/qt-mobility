/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QUrl>
#include "s60mediaplayercontrol.h"
#include "s60mediaplayersession.h"
#include "s60mmtrace.h"

S60MediaPlayerControl::S60MediaPlayerControl(S60MediaPlayerService *service)
    : QMediaPlayerControl(service)
    , m_service(service)
    , m_session(0)
    , m_stream(0)
    , m_mediaSettings(new S60MediaSettings(this))
{
    TRACE("S60MediaPlayerControl::S60MediaPlayerControl" << qtThisPtr());
}

S60MediaPlayerControl::~S60MediaPlayerControl()
{
    TRACE("S60MediaPlayerControl::~S60MediaPlayerControl" << qtThisPtr());
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
    return m_mediaSettings->mediaStatus();
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
    return m_mediaSettings->volume();
}

bool S60MediaPlayerControl::isMuted() const
{
   if (m_session)
       return m_session->isMuted();
   return m_mediaSettings->isMuted();
}

bool S60MediaPlayerControl::isSeekable() const
{
    if (m_session)
       return m_session->isSeekable();
    return false;
}

QMediaTimeRange S60MediaPlayerControl::availablePlaybackRanges() const
{
    QMediaTimeRange ranges;
    if (m_session && m_session->isSeekable())
        ranges.addInterval(0, m_session->duration());
    return ranges;
}

qreal S60MediaPlayerControl::playbackRate() const
{
    return m_mediaSettings->playbackRate();
}

void S60MediaPlayerControl::setPlaybackRate(qreal rate)
{
    TRACE("S60MediaPlayerControl::setPlaybackRate" << qtThisPtr() << "rate" << rate);
    qreal currentRate = m_mediaSettings->playbackRate();
    if (!qFuzzyCompare(currentRate, rate))
    {
        if (m_session)
            m_session->setPlaybackRate(rate);
        m_mediaSettings->setPlaybackRate(rate);
    }
}

void S60MediaPlayerControl::setPosition(qint64 pos)
{
    TRACE("S60MediaPlayerControl::setPosition" << qtThisPtr() << "pos" << pos);
    if (m_session)
        m_session->setPosition(pos);
}

void S60MediaPlayerControl::play()
{
    TRACE("S60MediaPlayerControl::play" << qtThisPtr());
    if (m_session)
        m_session->play();
}

void S60MediaPlayerControl::pause()
{
    TRACE("S60MediaPlayerControl::pause" << qtThisPtr());
    if (m_session)
        m_session->pause();
}

void S60MediaPlayerControl::stop()
{
    TRACE("S60MediaPlayerControl::stop" << qtThisPtr());
    if (m_session)
        m_session->stop();
}

void S60MediaPlayerControl::setVolume(int volume)
{
    int boundVolume = qBound(0, volume, 100);
    if (boundVolume == m_mediaSettings->volume())
        return;
    TRACE("S60MediaPlayerControl::setVolume" << qtThisPtr()
          << "volume" << volume << "boundVolume" << boundVolume);
    m_mediaSettings->setVolume(boundVolume);
    if (m_session)
        m_session->setVolume(boundVolume);
}

void S60MediaPlayerControl::setMuted(bool muted)
{
    if (m_mediaSettings->isMuted() == muted)
        return;
    TRACE("S60MediaPlayerControl::setMuted" << qtThisPtr() << "muted" << muted);
    m_mediaSettings->setMuted(muted);
    if (m_session)
        m_session->setMuted(muted);
}

QMediaContent S60MediaPlayerControl::media() const
{
    return m_media;
}

const QIODevice *S60MediaPlayerControl::mediaStream() const
{
    return m_stream;
}

void S60MediaPlayerControl::setMedia(const QMediaContent &source, QIODevice *stream)
{
    TRACE("S60MediaPlayerControl::setMedia" << qtThisPtr()
          << "source" << source.canonicalUrl().toString());
    Q_UNUSED(stream)
    if (m_session && (m_media == source)&& !source.canonicalUrl().isEmpty()) {
        if (m_session->isStreaming())
            m_session->load(source);
    } else {
        if (!source.canonicalUrl().isEmpty()) {
            m_session->disconnect();
            m_session->deleteLater();
            m_session = 0;
            m_session = m_service->createPlayerSession(source);
        }
        QMediaPlayer::MediaStatus mediaStatus = QMediaPlayer::NoMedia;
        if (m_session && !source.canonicalUrl().isEmpty()) {
            m_session->load(source);
            mediaStatus = m_session->mediaStatus();
        }
        m_mediaSettings->setMediaStatus(mediaStatus);
    }
    emit stateChanged(QMediaPlayer::StoppedState);
    if (m_media != source) {
        m_media = source;
        emit mediaChanged(m_media);
    }
}

S60MediaPlayerSession* S60MediaPlayerControl::session()
{
    return m_session;
}

void S60MediaPlayerControl::setVideoOutput(QObject *output)
{
    TRACE("S60MediaPlayerControl::setVideoOutput" << qtThisPtr()
          << "output" << output);
    m_mediaSettings->setVideoOutput(output);
    if (m_session)
        m_session->setVideoRenderer(output);
}

QObject *S60MediaPlayerControl::videoOutput() const
{
    return m_mediaSettings->videoOutput();
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

const S60MediaSettings &S60MediaPlayerControl::mediaControlSettings() const
{
    return *m_mediaSettings;
}

void S60MediaPlayerControl::setAudioEndpoint(const QString &name)
{
    TRACE("S60MediaPlayerControl::setAudioEndpoint" << qtThisPtr()
          << "name" << name);
    m_mediaSettings->setAudioEndpoint(name);
}

void S60MediaPlayerControl::setMediaType(S60MediaSettings::MediaType type)
{
    TRACE("S60MediaPlayerControl::setMediaType" << qtThisPtr()
          << "type" << type);
    m_mediaSettings->setMediaType(type);
}

