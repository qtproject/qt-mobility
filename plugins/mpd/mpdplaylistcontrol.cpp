/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstringlist.h>

#include "mpdplaylistcontrol.h"
#include "mpdplaylistsource.h"
#include "mpddaemon.h"

#include "qmediaplaylist.h"

MpdPlaylistControl::MpdPlaylistControl(MpdDaemon *mpd, QObject *parent):
    QMediaPlaylistControl(parent),
    daemon(mpd)
{
    playlist = new MpdPlaylistSource(daemon, this);

    connect(daemon, SIGNAL(playlistItemChanged(int)), SLOT(playlistItemChanged(int)));
}

MpdPlaylistControl::~MpdPlaylistControl()
{
}

QMediaPlaylistProvider* MpdPlaylistControl::playlistProvider() const
{
    return playlist;
}

bool MpdPlaylistControl::setPlaylistProvider(QMediaPlaylistProvider *mediaPlaylist)
{
    Q_UNUSED(mediaPlaylist);
    return false;
}

QMediaPlaylist::PlaybackMode MpdPlaylistControl::playbackMode() const
{
    return QMediaPlaylist::Linear;
}

void MpdPlaylistControl::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    Q_UNUSED(mode);
    return;
}

int MpdPlaylistControl::currentPosition() const
{
    return playlistPos;
}

void MpdPlaylistControl::setCurrentPosition(int position)
{
    daemon->send(QString("play %1").arg(position));
}

int MpdPlaylistControl::nextPosition(int steps) const
{
    const int currentSongPos = daemon->currentSongPos();

    if (currentSongPos + steps >= playlist->size())
        return -1;
    else
        return currentSongPos + steps;
}

int MpdPlaylistControl::previousPosition(int steps) const
{
    const int currentSongPos = daemon->currentSongPos();

    if (currentSongPos - steps < 0)
        return -1;
    else
        return currentSongPos - steps;
}

void MpdPlaylistControl::advance()
{
    int pos = nextPosition(1);
    if (pos != -1)
        daemon->send(QString("play %1").arg(pos));
}

void MpdPlaylistControl::back()
{
    int pos = previousPosition(1);
    if (pos != -1)
        daemon->send(QString("play %1").arg(pos));
}

void MpdPlaylistControl::playlistItemChanged(int position)
{
    emit playlistPositionChanged(playlistPos = position);
}

