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

#include <QtCore/qurl.h>

QWmpPlayerControl::QWmpPlayerControl(IWMPCore3 *player, QObject *parent)
        : QMediaPlayerControl(parent)
        , m_player(player)
        , m_controls(0)
        , m_settings(0)
{
    m_player->get_controls(&m_controls);
    m_player->get_settings(&m_settings);
    m_player->get_network(&m_network);

}

QWmpPlayerControl::~QWmpPlayerControl()
{
    if (m_controls) m_controls->Release();
    if (m_settings) m_settings->Release();
    if (m_network) m_network->Release();
}

int QWmpPlayerControl::state() const
{
    return m_state;
}

void QWmpPlayerControl::setState(int state)
{
    emit stateChanged(m_state = state);
}

QMediaPlaylist *QWmpPlayerControl::mediaPlaylist() const
{
    return m_playlist;
}

bool QWmpPlayerControl::setMediaPlaylist(QMediaPlaylist *playlist)
{
    m_playlist = playlist;
}

qint64 QWmpPlayerControl::duration() const
{
    return m_duration;
}

void QWmpPlayerControl::setDuration(qint64 duration)
{
    emit durationChanged(m_duration = duration);
}

qint64 QWmpPlayerControl::position() const
{
    double position;

    if (m_controls)
        m_controls->get_currentPosition(&position);

    return position;
}

void QWmpPlayerControl::setPosition(qint64 position)
{
    if (m_controls)
        m_controls->put_currentPosition(position);
}

int QWmpPlayerControl::playlistPosition() const
{
    return 0;
}

void QWmpPlayerControl::setPlaylistPosition(int position)
{
    Q_UNUSED(position);
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
    if (m_settings)
        m_settings->put_volume(volume);
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
    if (m_settings)
        m_settings->put_mute(muted ? TRUE : FALSE);
}

bool QWmpPlayerControl::isBuffering() const
{
    return m_buffering;
}

void QWmpPlayerControl::setBuffering(bool buffering)
{
    emit bu
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
        emit videoAvailablilityChanged(m_videoAvailable = available);
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
}

void QWmpPlayerControl::back()
{
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


