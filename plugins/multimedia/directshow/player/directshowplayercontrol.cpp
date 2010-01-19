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

#include "directshowplayercontrol.h"

#include "directshowplayerservice.h"

#include <QtCore/qmath.h>

static int volumeToDecibels(int volume)
{
    if (volume == 0) {
        return -10000;
    } else if (volume == 100) {
        return 0;
#ifdef QT_USE_MATH_H_FLOATS
    } else if (sizeof(qreal) == sizeof(float)) {
        return qRound(::log10f(float(volume) / 100) * 5000);
#endif
    } else {
        return qRound(::log10(qreal(volume) / 100) * 5000);
    }
}

static int decibelsToVolume(int dB)
{
    if (dB == -10000) {
        return 0;
    } else if (dB == 0) {
        return 100;
    } else {
        return qRound(100 * qPow(10, qreal(dB) / 5000));
    }
}

DirectShowPlayerControl::DirectShowPlayerControl(DirectShowPlayerService *service, QObject *parent)
    : QMediaPlayerControl(parent)
    , m_service(service)
    , m_state(QMediaPlayer::StoppedState)
    , m_status(QMediaPlayer::UnknownMediaStatus)
    , m_muteVolume(-1)
    , m_stream(0)
    , m_loadStatus(0)
    , m_buffering(false)
{
}

DirectShowPlayerControl::~DirectShowPlayerControl()
{
}

QMediaPlayer::State DirectShowPlayerControl::state() const
{
    return m_state;
}

QMediaPlayer::MediaStatus DirectShowPlayerControl::mediaStatus() const
{
    return m_status;
}

qint64 DirectShowPlayerControl::duration() const
{
    LONGLONG duration = 0;
    switch (m_status) {
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::StalledMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
    case QMediaPlayer::EndOfMedia:
        if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_service->graph())) {
            seeking->GetDuration(&duration);
            seeking->Release();
        }
        break;
    default:
        break;
    }

    return duration / 10;
}

qint64 DirectShowPlayerControl::position() const
{
    LONGLONG position = 0;

    switch (m_status) {
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::StalledMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
    case QMediaPlayer::EndOfMedia:
        if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_service->graph())) {
            seeking->GetCurrentPosition(&position);
            seeking->Release();
        }
        break;
    default:
        break;
    }

    return position / 10;
}

void DirectShowPlayerControl::setPosition(qint64 position)
{
    m_service->seek(position);
}

int DirectShowPlayerControl::volume() const
{
    if (m_muteVolume >= 0) {
        return m_muteVolume;
    } else if (IBasicAudio *audio = com_cast<IBasicAudio>(m_service->graph())) {
        long dB = 0;

        audio->get_Volume(&dB);
        audio->Release();

        return decibelsToVolume(dB);
    } else {
        return 0;
    }
}

void DirectShowPlayerControl::setVolume(int volume)
{
    int boundedVolume = qBound(0, volume, 100);

    if (m_muteVolume >= 0) {
        m_muteVolume = boundedVolume;

        emit volumeChanged(m_muteVolume);
    } else if (IBasicAudio *audio = com_cast<IBasicAudio>(m_service->graph())) {
        audio->put_Volume(volumeToDecibels(volume));
        audio->Release();

        emit volumeChanged(boundedVolume);
    }
}

bool DirectShowPlayerControl::isMuted() const
{
    return m_muteVolume >= 0;
}

void DirectShowPlayerControl::setMuted(bool muted)
{
    if (muted && m_muteVolume < 0) {
        if (IBasicAudio *audio = com_cast<IBasicAudio>(m_service->graph())) {
            long dB = 0;

            audio->get_Volume(&dB);
            audio->Release();

            m_muteVolume = decibelsToVolume(dB);

            audio->put_Volume(-10000);
        } else {
            m_muteVolume = 0;
        }

        emit mutedChanged(muted);
    } else if (!muted && m_muteVolume >= 0) {
        if (IBasicAudio *audio = com_cast<IBasicAudio>(m_service->graph())) {
            audio->put_Volume(volumeToDecibels(m_muteVolume));
            audio->Release();
        }
        m_muteVolume = -1;

        emit mutedChanged(muted);
    }
}

int DirectShowPlayerControl::bufferStatus() const
{
    return 0;
}

bool DirectShowPlayerControl::isVideoAvailable() const
{
    return false;
}

bool DirectShowPlayerControl::isSeekable() const
{
    DWORD capabilities = 0;

    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_service->graph())) {
        seeking->GetCapabilities(&capabilities);
        seeking->Release();
    }

    return capabilities & AM_SEEKING_CanSeekAbsolute;
}

QPair<qint64, qint64> DirectShowPlayerControl::seekRange() const
{
    LONGLONG minimum = 0;
    LONGLONG maximum = 0;

    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_service->graph())) {
        seeking->GetAvailable(&minimum, &maximum);
        seeking->Release();
    }

    return QPair<qint64, qint64>(minimum, maximum);
}

qreal DirectShowPlayerControl::playbackRate() const
{
    double rate = 0.0;

    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_service->graph())) {
        seeking->GetRate(&rate);
        seeking->Release();
    }

    return rate;
}

void DirectShowPlayerControl::setPlaybackRate(qreal rate)
{
    m_service->setRate(rate);
}

QMediaContent DirectShowPlayerControl::media() const
{
    return m_media;
}

const QIODevice *DirectShowPlayerControl::mediaStream() const
{
    return m_stream;
}

void DirectShowPlayerControl::setMedia(const QMediaContent &media, QIODevice *stream)
{
    m_media = media;
    m_stream = stream;

    m_state = QMediaPlayer::StoppedState;
    m_status = QMediaPlayer::LoadingMedia;

    m_service->load(media, stream);

    updateStatus();
}

void DirectShowPlayerControl::play()
{
    m_service->play();
    emit stateChanged(m_state = QMediaPlayer::PlayingState);
}

void DirectShowPlayerControl::pause()
{
    m_service->pause();
    emit stateChanged(m_state = QMediaPlayer::PausedState);
}

void DirectShowPlayerControl::stop()
{
    m_service->stop();
    emit stateChanged(m_state = QMediaPlayer::StoppedState);
}

void DirectShowPlayerControl::bufferingData(bool buffering)
{
    m_buffering = buffering;

    updateStatus();
}

void DirectShowPlayerControl::complete(HRESULT)
{
    m_state = QMediaPlayer::StoppedState;
    m_status = QMediaPlayer::EndOfMedia;
    m_buffering = false;
    emit mediaStatusChanged(m_status);
    emit stateChanged(m_state);
}

void DirectShowPlayerControl::loadStatus(long status)
{
    m_loadStatus = status;

    updateStatus();
}

void DirectShowPlayerControl::stateChange(long state)
{
    switch (state) {
    case State_Stopped:
        m_state = QMediaPlayer::StoppedState;
        break;
    case State_Running:
        m_state = QMediaPlayer::PlayingState;
        break;
    case State_Paused:
        m_state = QMediaPlayer::PausedState;
        break;
    default:
        break;
    }

    updateStatus();
    emit stateChanged(m_state);
}

void DirectShowPlayerControl::customEvent(QEvent *event)
{
    if (event->type() == QEvent::Type(GraphStatusChanged)) {
        updateStatus();

        event->accept();
    } else {
        QMediaPlayerControl::customEvent(event);
    }
}

void DirectShowPlayerControl::updateStatus()
{
    QMediaPlayer::MediaStatus status = m_status;

    switch (m_service->graphStatus()) {
    case DirectShowPlayerService::NoMedia:
        status = QMediaPlayer::NoMedia;
        break;
    case DirectShowPlayerService::Loading:
        status = QMediaPlayer::LoadingMedia;
        break;
    case DirectShowPlayerService::Loaded:
        if (m_state == QMediaPlayer::StoppedState)
            status = QMediaPlayer::LoadedMedia;
        else if (m_buffering)
            status = QMediaPlayer::BufferingMedia;
        else
            status = QMediaPlayer::BufferedMedia;
        break;
    case DirectShowPlayerService::InvalidMedia:
        status = QMediaPlayer::InvalidMedia;
        break;
    default:
        status = QMediaPlayer::UnknownMediaStatus;
        break;
    }

    if (status != m_status) {
        emit mediaStatusChanged(m_status = status);
        emit durationChanged(duration());
    }
}
