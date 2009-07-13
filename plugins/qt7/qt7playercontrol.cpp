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

#include <qmediaplaylist.h>

#include "qt7playercontrol.h"
#include "qt7movie.h"


Qt7PlayerControl::Qt7PlayerControl(QObject *parent):
    QMediaPlayerControl(parent)
{
    playlist = new QMediaPlaylist(0, this);
    _state = StoppedState;
    playlistPos = 0;

    movie = new Qt7Movie(this);
}

Qt7PlayerControl::~Qt7PlayerControl()
{
}

int Qt7PlayerControl::state() const
{
    return _state;
}

QMediaPlaylist* Qt7PlayerControl::mediaPlaylist() const
{
    return playlist;
}

bool Qt7PlayerControl::setMediaPlaylist(QMediaPlaylist *mediaPlaylist)
{
    if (mediaPlaylist == 0 || playlist == mediaPlaylist)
        return false;

    if (playlist->parent() == this)
        delete playlist;
    playlist = mediaPlaylist;
    return true;
}

qint64 Qt7PlayerControl::duration() const
{
    return movie->duration();
}

qint64 Qt7PlayerControl::position() const
{
    return movie->position();
}

void Qt7PlayerControl::setPosition(qint64 position)
{
    movie->setPosition(position);
}

int Qt7PlayerControl::playlistPosition() const
{
    return playlistPos;
}

void Qt7PlayerControl::setPlaylistPosition(int position)
{
    if (position > playlist->size())
        return;

    stop();
    movie->setSource(playlist->itemAt(position));
    play();

    playlistPos = position;
}

int Qt7PlayerControl::volume() const
{
    return movie->volume();
}

void Qt7PlayerControl::setVolume(int volume)
{
    movie->setVolume(volume);
}

bool Qt7PlayerControl::isMuted() const
{
    return movie->isMuted();
}

void Qt7PlayerControl::setMuted(bool muted)
{
    movie->setMuted(muted);
}

bool Qt7PlayerControl::isBuffering() const
{
    return false;
}

int Qt7PlayerControl::bufferStatus() const
{
    return 0;
}

bool Qt7PlayerControl::isVideoAvailable() const
{
    return movie->isVideoAvailable();
}

bool Qt7PlayerControl::isSeekable() const
{
    return movie->isSeekable();
}

float Qt7PlayerControl::playbackRate() const
{
    return 1;
}

void Qt7PlayerControl::setPlaybackRate(float rate)
{
}

void Qt7PlayerControl::play()
{
    if (_state == StoppedState) {
        movie->setSource(playlist->itemAt(playlistPos));
        movie->play();
    }
}

void Qt7PlayerControl::pause()
{
    if (_state == PlayingState)
        movie->pause();
}

void Qt7PlayerControl::stop()
{
    if (_state == PlayingState || _state == PausedState)
        movie->stop();
}

void Qt7PlayerControl::advance()
{
    stop();
    movie->setSource(playlist->itemAt(playlistPos++));
    play();
}

void Qt7PlayerControl::back()
{
    stop();
    movie->setSource(playlist->itemAt(playlistPos--));
    play();
}

void Qt7PlayerControl::setVideoOutput(Qt7Widget *output)
{
    movie->setVideoOutput(output);
}
