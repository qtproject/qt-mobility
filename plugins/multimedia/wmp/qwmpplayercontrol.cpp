/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qwmpplayercontrol.h"

#include "qwmpevents.h"
#include "qwmpglobal.h"
#include "qwmpmetadata.h"
#include "qwmpplaylist.h"

#include <qmediaplayer.h>
#include <qmediaplaylist.h>

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QWmpPlayerControl::QWmpPlayerControl(IWMPCore3 *player, QWmpEvents *events, QObject *parent)
    : QMediaPlayerControl(parent)
    , m_player(player)
    , m_controls(0)
    , m_settings(0)
    , m_state(QMediaPlayer::StoppedState)
    , m_duration(0)
    , m_buffering(false)
    , m_audioAvailable(false)
    , m_videoAvailable(false)
{
    m_player->get_controls(&m_controls);
    m_player->get_settings(&m_settings);
    m_player->get_network(&m_network);

    if (m_settings)
        m_settings->put_autoStart(FALSE);

    WMPPlayState state = wmppsUndefined;
    if (m_player->get_playState(&state) == S_OK)
        playStateChangeEvent(state);

    connect(events, SIGNAL(Buffering(VARIANT_BOOL)), this, SLOT(bufferingEvent(VARIANT_BOOL)));
    connect(events, SIGNAL(PositionChange(double,double)),
            this, SLOT(positionChangeEvent(double,double)));
    connect(events, SIGNAL(PlayStateChange(long)), this, SLOT(playStateChangeEvent(long)));
    connect(events, SIGNAL(CurrentItemChange(IDispatch*)),
            this, SLOT(currentItemChangeEvent(IDispatch*)));
    connect(events, SIGNAL(MediaChange(IDispatch*)), this, SLOT(mediaChangeEvent(IDispatch*)));
}

QWmpPlayerControl::~QWmpPlayerControl()
{
    if (m_controls) m_controls->Release();
    if (m_settings) m_settings->Release();
    if (m_network) m_network->Release();
}

QMediaPlayer::State QWmpPlayerControl::state() const
{
    return m_state;
}

QMediaPlayer::MediaStatus QWmpPlayerControl::mediaStatus() const
{
    return m_status;
}

qint64 QWmpPlayerControl::duration() const
{
    double duration = 0.;

    IWMPMedia *media = 0;
    if (m_controls && m_controls->get_currentItem(&media) == S_OK) {
        media->get_duration(&duration);

        media->Release();
    }

    return m_duration * 1000;
}

qint64 QWmpPlayerControl::position() const
{
    double position;

    if (m_controls)
        m_controls->get_currentPosition(&position);

    return position * 1000;
}

void QWmpPlayerControl::setPosition(qint64 position)
{
    if (m_controls)
        m_controls->put_currentPosition(double(position) / 1000.);
}

int QWmpPlayerControl::volume() const
{
    long volume = 0;

    if (m_settings)
        m_settings->get_volume(&volume);

    return volume;
}

void QWmpPlayerControl::setVolume(int volume)
{
    if (m_settings && m_settings->put_volume(volume) == S_OK)
        emit volumeChanged(volume);
}

bool QWmpPlayerControl::isMuted() const
{
    VARIANT_BOOL mute = FALSE;

    if (m_settings)
        m_settings->get_mute(&mute);

    return mute;
}

void QWmpPlayerControl::setMuted(bool muted)
{
    if (m_settings && m_settings->put_mute(muted ? TRUE : FALSE) == S_OK)
        emit mutedChanged(muted);

}

int QWmpPlayerControl::bufferStatus() const
{
    long progress = 0;

    if (m_network)
        m_network->get_bufferingProgress(&progress);

    return progress;
}

bool QWmpPlayerControl::isVideoAvailable() const
{
    return m_videoAvailable;
}

bool QWmpPlayerControl::isAudioAvailable() const
{
    return m_audioAvailable;
}

void QWmpPlayerControl::setAudioAvailable(bool available)
{
    if (m_audioAvailable != available)
        emit audioAvailableChanged(m_audioAvailable = available);
}

void QWmpPlayerControl::setVideoAvailable(bool available)
{
    if (m_videoAvailable != available)
        emit videoAvailableChanged(m_videoAvailable = available);
}

bool QWmpPlayerControl::isSeekable() const
{
    return true;
}

QMediaTimeRange QWmpPlayerControl::availablePlaybackRanges() const
{
    QMediaTimeRange ranges;

    IWMPMedia *media = 0;
    if (m_controls && m_controls->get_currentItem(&media) == S_OK) {
        double duration = 0;
        media->get_duration(&duration);
        media->Release();

        if(duration > 0)
            ranges.addInterval(0, duration * 1000);
    }

    return ranges;
}

qreal QWmpPlayerControl::playbackRate() const
{
    double rate = 0.;

    if (m_settings)
        m_settings->get_rate(&rate);

    return rate;
}

void QWmpPlayerControl::setPlaybackRate(qreal rate)
{
    if (m_settings)
        m_settings->put_rate(rate);
}

void QWmpPlayerControl::play()
{
    if (m_controls)
        m_controls->play();
}

void QWmpPlayerControl::pause()
{
    if (m_controls)
        m_controls->pause();
}

void QWmpPlayerControl::stop()
{
    if (m_controls)
        m_controls->stop();
}

QMediaContent QWmpPlayerControl::media() const
{
    QMediaResourceList resources;

    QUrl tmpUrl = url();

    if (!tmpUrl.isEmpty())
        resources << QMediaResource(tmpUrl);

    return resources;
}

const QIODevice *QWmpPlayerControl::mediaStream() const
{
    return 0;
}

void QWmpPlayerControl::setMedia(const QMediaContent &content, QIODevice *stream)
{
    if (!content.isNull() && !stream)
        setUrl(content.canonicalUrl());
    else
        setUrl(QUrl());
}

void QWmpPlayerControl::bufferingEvent(VARIANT_BOOL buffering)
{
    if (m_state != QMediaPlayer::StoppedState) {
        emit mediaStatusChanged(m_status = buffering
                ? QMediaPlayer::BufferingMedia
                : QMediaPlayer::BufferedMedia);
    }
}

void QWmpPlayerControl::currentItemChangeEvent(IDispatch *dispatch)
{
    IWMPMedia *media = 0;
    if (dispatch && dispatch->QueryInterface(
            __uuidof(IWMPMedia), reinterpret_cast<void **>(&media)) == S_OK) {
        double duration = 0;

        if (media->get_duration(&duration) == S_OK)
            emit durationChanged(duration * 1000);
    }
}

void QWmpPlayerControl::mediaChangeEvent(IDispatch *dispatch)
{
    IWMPMedia *media = 0;
    if (dispatch &&  dispatch->QueryInterface(
            __uuidof(IWMPMedia), reinterpret_cast<void **>(&media)) == S_OK) {
        IWMPMedia *currentMedia = 0;
        if (m_controls && m_controls->get_currentItem(&currentMedia) == S_OK) {
            VARIANT_BOOL isEqual = VARIANT_FALSE;
            if (media->get_isIdentical(currentMedia, &isEqual) == S_OK && isEqual) {
                double duration = 0;

                if (media->get_duration(&duration) == S_OK)
                    emit durationChanged(duration * 1000);
            }
            currentMedia->Release();
        }
        media->Release();
    }
}

void QWmpPlayerControl::positionChangeEvent(double from, double to)
{
    Q_UNUSED(from);

    emit positionChanged(to * 1000);
}

void QWmpPlayerControl::playStateChangeEvent(long state)
{
    switch (state) {
    case wmppsUndefined:
        m_state = QMediaPlayer::StoppedState;
        m_status = QMediaPlayer::UnknownMediaStatus;

        emit stateChanged(m_state);
        emit mediaStatusChanged(m_status);
        break;
    case wmppsStopped:
        if (m_state != QMediaPlayer::StoppedState) {
            m_state = QMediaPlayer::StoppedState;
            m_status = QMediaPlayer::LoadedMedia;

            emit stateChanged(m_state);
            emit mediaStatusChanged(m_status);
        }
        break;
    case wmppsPaused:
        if (m_state != QMediaPlayer::PausedState && m_status != QMediaPlayer::BufferedMedia) {
            m_state = QMediaPlayer::PausedState;
            m_status = QMediaPlayer::BufferedMedia;

            emit stateChanged(m_state);
            emit mediaStatusChanged(m_status);
        } else if (m_state != QMediaPlayer::PausedState) {
            emit stateChanged(m_state = QMediaPlayer::PausedState);
        } else if (m_status != QMediaPlayer::BufferedMedia) {
            emit mediaStatusChanged(m_status = QMediaPlayer::BufferedMedia);
        }
        break;
    case wmppsPlaying:
    case wmppsScanForward:
    case wmppsScanReverse:
        if (m_state != QMediaPlayer::PlayingState && m_status != QMediaPlayer::BufferedMedia) {
            m_state = QMediaPlayer::PlayingState;
            m_status = QMediaPlayer::BufferedMedia;

            emit stateChanged(m_state);
            emit mediaStatusChanged(m_status);
        } else if (m_state != QMediaPlayer::PlayingState) {
            emit stateChanged(m_state = QMediaPlayer::PlayingState);
        } else if (m_status != QMediaPlayer::BufferedMedia) {
            emit mediaStatusChanged(m_status = QMediaPlayer::BufferedMedia);
        }

        if (m_state != QMediaPlayer::PlayingState)
            emit stateChanged(m_state = QMediaPlayer::PlayingState);
        if (m_status != QMediaPlayer::BufferedMedia)
            emit mediaStatusChanged(m_status = QMediaPlayer::BufferedMedia);
        break;
    case wmppsBuffering:
    case wmppsWaiting:
        if (m_status != QMediaPlayer::StalledMedia && m_state != QMediaPlayer::StoppedState)
            emit mediaStatusChanged(m_status = QMediaPlayer::StalledMedia);
        break;
    case wmppsMediaEnded:
        if (m_status != QMediaPlayer::EndOfMedia && m_state != QMediaPlayer::StoppedState) {
            m_state = QMediaPlayer::StoppedState;
            m_status = QMediaPlayer::StalledMedia;

            emit stateChanged(m_state);
            emit mediaStatusChanged(m_status);
        }
        break;
    case wmppsTransitioning:
        if (m_status != QMediaPlayer::LoadingMedia)
            emit mediaStatusChanged(m_status = QMediaPlayer::LoadingMedia);
        break;
    case wmppsReady:
        if (m_status != QMediaPlayer::LoadedMedia)
            m_status = m_status = QMediaPlayer::LoadedMedia;

            if (m_state != QMediaPlayer::StoppedState)
                emit stateChanged(QMediaPlayer::StoppedState);
            emit mediaStatusChanged(m_status);
        break;
    case wmppsReconnecting:
        if (m_status != QMediaPlayer::StalledMedia && m_state != QMediaPlayer::StoppedState)
            emit mediaStatusChanged(m_status = QMediaPlayer::StalledMedia);
        break;
    default:
        break;
    }
}

QUrl QWmpPlayerControl::url() const
{
    BSTR string;
    if (m_player && m_player->get_URL(&string) == S_OK) {
        QUrl url(QString::fromWCharArray(string, SysStringLen(string)), QUrl::StrictMode);

        SysFreeString(string);

        return url;
    } else {
        return QUrl();
    }
}

void QWmpPlayerControl::setUrl(const QUrl &url)
{
    if (url != QWmpPlayerControl::url() && m_player) {
        BSTR string = SysAllocString(reinterpret_cast<const wchar_t *>(url.toString().unicode()));

        m_player->put_URL(string);

        SysFreeString(string);
    }
}
