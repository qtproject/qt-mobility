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

#include <QtCore/qstringlist.h>

#include "mpdplayercontrol.h"
#include "mpdplaylistsource.h"
#include "mpddaemon.h"


MpdPlayerControl::MpdPlayerControl(MpdDaemon *mpd, QObject *parent):
    QMediaPlayerControl(parent),
    daemon(mpd)
{
    playlist = new QMediaPlaylist(new MpdPlaylistSource(daemon, this));

    connect(daemon, SIGNAL(notify()), SLOT(notify()));
    connect(daemon, SIGNAL(playerStateChanged(int)), SIGNAL(stateChanged(int)));
    connect(daemon, SIGNAL(mixerChanged()), SLOT(mixerChanged()));
    connect(daemon, SIGNAL(playlistItemChanged(int)), SLOT(playlistItemChanged(int)));
    connect(daemon, SIGNAL(positionChanged(qint64)), SIGNAL(positionChanged(qint64)));
    connect(daemon, SIGNAL(durationChanged(qint64)), SIGNAL(durationChanged(qint64)));
    connect(daemon, SIGNAL(volumeChanged(int)), SLOT(handleVolumeChanged(int)));
    connect(daemon, SIGNAL(mutingChanged(bool)), SIGNAL(mutingChanged(bool)));

    savedVolume = 100;
    m_muted = false;
}

MpdPlayerControl::~MpdPlayerControl()
{
}

int MpdPlayerControl::state() const
{
    return 0;
}

QMediaPlaylist* MpdPlayerControl::mediaPlaylist() const
{
    return playlist;
}

bool MpdPlayerControl::setMediaPlaylist(QMediaPlaylist *mediaPlaylist)
{
    Q_UNUSED(mediaPlaylist);
    return false;
}


qint64 MpdPlayerControl::duration() const
{
    return 0;
}

qint64 MpdPlayerControl::position() const
{
    return 0;
}

void MpdPlayerControl::setPosition(qint64 position)
{
    daemon->send(QString("seek %1 %2").arg(daemon->currentSongPos()).arg(int(position / 1000)));
}

int MpdPlayerControl::playlistPosition() const
{
    return playlistPos;
}

void MpdPlayerControl::setPlaylistPosition(int position)
{
}

int MpdPlayerControl::volume() const
{
    return daemon->volume();
}

static inline int clamp_volume(int volume)
{
    return qMin(100, qMax(0, volume));
}

void MpdPlayerControl::setVolume(int volume)
{
    daemon->send(QString("setvol %1").arg(clamp_volume(volume)));
}

bool MpdPlayerControl::isMuted() const
{
    return m_muted;
}

void MpdPlayerControl::setMuted(bool muted)
{
    if (m_muted == muted)
        return;
    m_muted = muted;
    if (m_muted)
        savedVolume = daemon->volume();

    daemon->send(QString("setvol %1").arg(muted ? 0 : savedVolume));
}

bool MpdPlayerControl::isBuffering() const
{
    return false;
}

int MpdPlayerControl::bufferStatus() const
{
    return 0;
}

bool MpdPlayerControl::isVideoAvailable() const
{
    return false;
}

bool MpdPlayerControl::isSeekable() const
{
    return true;
}

void MpdPlayerControl::play()
{
    daemon->send("play");
}

void MpdPlayerControl::pause()
{
    daemon->send("pause");
}

void MpdPlayerControl::stop()
{
    daemon->send("stop");
}

void MpdPlayerControl::advance()
{
    const int currentSongPos = daemon->currentSongPos();

    if (currentSongPos + 1 == playlist->size())
        return;

    daemon->send(QString("play %1").arg(currentSongPos + 1));
}

void MpdPlayerControl::back()
{
    const int currentSongPos = daemon->currentSongPos();

    if (currentSongPos == 0)
        return;

    daemon->send(QString("play %1").arg(currentSongPos - 1));
}

void MpdPlayerControl::notify()
{
    emit durationChanged(daemon->duration());
    emit positionChanged(daemon->position());
}

void MpdPlayerControl::mixerChanged()
{
    emit volumeChanged(daemon->volume());
    emit mutingChanged(daemon->volume() == 0);
}

void MpdPlayerControl::playlistItemChanged(int position)
{
    emit playlistPositionChanged(playlistPos = position);
}

void MpdPlayerControl::handleVolumeChanged(int volume)
{
    if (!m_muted)
        emit volumeChanged(volume);
}

