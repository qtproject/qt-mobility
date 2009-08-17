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

#include "qwmpplayercontrol.h"

#include "qwmpevents.h"
#include "qwmpglobal.h"
#include "qwmpmetadata.h"
#include "qwmpplaylist.h"

#include "qmediaplayer.h"
#include "qmediaplaylist.h"

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QWmpPlayerControl::QWmpPlayerControl(IWMPCore3 *player, QWmpEvents *events, QObject *parent)
        : QMediaPlayerControl(parent)
        , m_player(player)
        , m_controls(0)
        , m_settings(0)
        , m_playlistSource()
        , m_playlist(0)
        , m_proxiedPlaylist(0)
        , m_proxyPlaylist(0)
        , m_state(QMediaPlayer::StoppedState)
        , m_duration(0)
        , m_buffering(false)
        , m_videoAvailable(false)
{
    m_player->get_controls(&m_controls);
    m_player->get_settings(&m_settings);
    m_player->get_network(&m_network);

    m_playlistSource = new QWmpPlaylist(player, events);
    m_playlist = new QMediaPlaylist(m_playlistSource);

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
    delete m_playlist;

    if (m_controls) m_controls->Release();
    if (m_settings) m_settings->Release();
    if (m_network) m_network->Release();
}

QMediaPlayer::State QWmpPlayerControl::state() const
{
    return m_state;
}

void QWmpPlayerControl::setState(QMediaPlayer::State state)
{
    emit stateChanged(m_state = state);
}

QMediaPlaylist *QWmpPlayerControl::mediaPlaylist() const
{
    return m_playlist;
}

bool QWmpPlayerControl::setMediaPlaylist(QMediaPlaylist *playlist)
{
    if (playlist == m_proxiedPlaylist || playlist == m_playlist)
        return true;

    if (m_proxiedPlaylist) {
        disconnect(m_proxiedPlaylist, SIGNAL(itemsInserted(int,int)),
                   this, SLOT(proxiedItemsInserted(int,int)));
        disconnect(m_proxiedPlaylist, SIGNAL(itemsRemoved(int,int)),
                   this, SLOT(proxiedItemsRemoved(int,int)));
        disconnect(m_proxiedPlaylist, SIGNAL(itemsChanged(int,int)),
                   this, SLOT(proxiedItemsChanged(int,int)));
    }

    m_proxiedPlaylist = playlist;

    if (m_proxiedPlaylist) {
        if (m_proxyPlaylist && m_proxyPlaylist->clear() != S_OK) {
            m_proxyPlaylist->Release();
            m_proxyPlaylist = 0;
        }

        if (!m_proxyPlaylist && m_player->newPlaylist(0, 0, &m_proxyPlaylist) != S_OK) {
            m_proxiedPlaylist = 0;
        } else {
            proxiedItemsInserted(0, m_proxiedPlaylist->size() - 1);

            if (m_player->put_currentPlaylist(m_proxyPlaylist) != S_OK) {
                m_proxyPlaylist->Release();
                m_proxyPlaylist = 0;
                m_proxiedPlaylist = 0;
            } else {
                connect(m_proxiedPlaylist, SIGNAL(itemsInserted(int,int)),
                           this, SLOT(proxiedItemsInserted(int,int)));
                connect(m_proxiedPlaylist, SIGNAL(itemsRemoved(int,int)),
                           this, SLOT(proxiedItemsRemoved(int,int)));
                connect(m_proxiedPlaylist, SIGNAL(itemsChanged(int,int)),
                           this, SLOT(proxiedItemsChanged(int,int)));
            }
        }
    } else {

    }

    return m_proxiedPlaylist == playlist;
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

int QWmpPlayerControl::playlistPosition() const
{
    int position = 0;

    IWMPMedia *media = 0;
    if (m_controls && m_controls->get_currentItem(&media) == S_OK) {
        position = QWmpMetaData::value(media, QLatin1String("PlaylistIndex")).toInt();

        media->Release();
    }

    return position;
}

void QWmpPlayerControl::setPlaylistPosition(int position)
{

    IWMPPlaylist *playlist = 0;
    if (m_controls && m_player->get_currentPlaylist(&playlist) == S_OK) {
        IWMPMedia *media = 0;
        if (playlist->get_item(position, &media) == S_OK) {
            m_controls->put_currentItem(media);

            media->Release();
        }
        playlist->Release();
    }
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
        emit mutingChanged(muted);

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

void QWmpPlayerControl::setVideoAvailable(bool available)
{
    if (m_videoAvailable != available)
        emit this->videoAvailabilityChanged(m_videoAvailable = available);
}

bool QWmpPlayerControl::isSeekable() const
{
    return true;
}


float QWmpPlayerControl::playbackRate() const
{
    double rate = 0.;

    if (m_settings)
        m_settings->get_rate(&rate);

    return rate;
}

void QWmpPlayerControl::setPlaybackRate(float rate)
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

void QWmpPlayerControl::advance()
{
    if (m_controls)
        m_controls->next();
}

void QWmpPlayerControl::back()
{
    if (m_controls)
        m_controls->previous();
}

void QWmpPlayerControl::proxiedItemsInserted(int start, int end)
{
    for (int i = start; i <= end; ++i) {
        QMediaResourceList resources = m_proxiedPlaylist->resources(i);

        IWMPMedia *media = 0;
        if (!resources.isEmpty() && m_player->newMedia(
                QAutoBStr(resources.first().uri()), &media) == S_OK) {
            m_proxyPlaylist->appendItem(media);
        }
    }
}

void QWmpPlayerControl::proxiedItemsRemoved(int start, int end)
{
    for (int i = start; i <= end; ++i) {
        IWMPMedia *media = 0;
        if (m_proxyPlaylist->get_item(start, &media) == S_OK)
            m_proxyPlaylist->removeItem(media);
        else
           ++start;
    }
}

void QWmpPlayerControl::proxiedItemsChanged(int start, int end)
{
    proxiedItemsInserted(start, end);
    proxiedItemsRemoved(start, end);
}

void QWmpPlayerControl::bufferingEvent(VARIANT_BOOL buffering)
{
    emit bufferingChanged(m_buffering = buffering);
}

void QWmpPlayerControl::currentItemChangeEvent(IDispatch *dispatch)
{
    IWMPMedia *media = 0;
    if (dispatch && dispatch->QueryInterface(
            __uuidof(IWMPMedia), reinterpret_cast<void **>(&media)) == S_OK) {
        int index = QWmpMetaData::value(media, QLatin1String("PlaylistIndex")).toInt();

        emit playlistPositionChanged(index);

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

void QWmpPlayerControl::openPlaylistChangeEvent(IDispatch *dispatch)
{
    IWMPPlaylist *playlist = 0;
    if (dispatch)
        dispatch->QueryInterface(__uuidof(IWMPPlaylist), reinterpret_cast<void **>(&playlist));

    if (playlist != m_proxyPlaylist && m_proxyPlaylist) {
        disconnect(m_proxiedPlaylist, SIGNAL(itemsInserted(int,int)),
                   this, SLOT(proxiedItemsInserted(int,int)));
        disconnect(m_proxiedPlaylist, SIGNAL(itemsRemoved(int,int)),
                   this, SLOT(proxiedItemsRemoved(int,int)));
        disconnect(m_proxiedPlaylist, SIGNAL(itemsChanged(int,int)),
                   this, SLOT(proxiedItemsChanged(int,int)));

        m_proxyPlaylist->Release();
        m_proxyPlaylist = 0;
        m_proxiedPlaylist = 0;
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
    if (m_player) {
        BSTR string = SysAllocString(reinterpret_cast<const wchar_t *>(url.toString().unicode()));

        m_player->put_URL(string);

        SysFreeString(string);
    }
}
