
/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60mediasettings.h"

S60MediaSettings::S60MediaSettings(QObject *parent)
    : QObject(parent)
    , m_volume(30)
    , m_muted(false)
    , m_playbackRate(0)
    , m_mediaStatus(QMediaPlayer::NoMedia)
    , m_audioEndpoint(QString("Default"))
    , m_videoOutput(0)
{

}

void S60MediaSettings::setVolume(int volume)
{
    m_volume = volume;
}

void S60MediaSettings::setMuted(bool muted)
{
    m_muted = muted;
}

void S60MediaSettings::setPlaybackRate(qreal rate)
{
    m_playbackRate = rate;
}

void S60MediaSettings::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    m_mediaStatus = status;
}

void S60MediaSettings::setAudioEndpoint(const QString &audioEndpoint)
{
    m_audioEndpoint = audioEndpoint;
}

void S60MediaSettings::setMediaType(MediaType type)
{
    m_mediaType = type;
}

void S60MediaSettings::setVideoOutput(QObject *value)
{
    m_videoOutput = value;
}

int S60MediaSettings::volume() const
{
    return m_volume;
}

bool S60MediaSettings::isMuted() const
{
    return m_muted;
}

qreal S60MediaSettings::playbackRate() const
{
    return m_playbackRate;
}

QMediaPlayer::MediaStatus S60MediaSettings::mediaStatus() const
{
    return m_mediaStatus;
}

QString S60MediaSettings::audioEndpoint() const
{
    return m_audioEndpoint;
}

S60MediaSettings::MediaType S60MediaSettings::mediaType() const
{
    return m_mediaType;
}

QObject *S60MediaSettings::videoOutput() const
{
    return m_videoOutput;
}
