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
#include <QtCore/qtimer.h>

#include <qmediaplaylist.h>
#include <qmediaplaylistnavigator.h>

#include "qt7playercontrol.h"
#include "qt7movie.h"

class Qt7PlayerControlPrivate
{
public:
    qint64 duration;
    int volume;
    bool muted;
    Qt7PlayerControl::State state;
};

Qt7PlayerControl::Qt7PlayerControl(QObject *parent):
    QMediaPlayerControl(parent),
    d(new Qt7PlayerControlPrivate)
{
    playlist = new QMediaPlaylist(0, this);

    navigator = new QMediaPlaylistNavigator(playlist, this);
    navigator->setPlaybackMode(QMediaPlaylistNavigator::Linear);
    connect(navigator, SIGNAL(activated(QMediaSource)), SLOT(setSource(QMediaSource)));
    connect(navigator, SIGNAL(currentPositionChanged(int)), SIGNAL(playlistPositionChanged(int)));

    movie = new Qt7Movie(this);

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), SLOT(update()));

    d->duration = 0;
    d->volume = 100;
    d->muted = false;
    d->state = StoppedState;
}

Qt7PlayerControl::~Qt7PlayerControl()
{
}

int Qt7PlayerControl::state() const
{
    return d->state;
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
    return navigator->currentPosition();
}

void Qt7PlayerControl::setPlaylistPosition(int position)
{
    navigator->jump(position);
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
    return movie->rate();
}

void Qt7PlayerControl::setPlaybackRate(float rate)
{
    movie->setRate(rate);
}

void Qt7PlayerControl::play()
{
    switch (d->state) {
    case StoppedState: {
        movie->setSource(navigator->currentItem());
        movie->play();
        break;
    }
    case PausedState:
        movie->play();
        break;
    default:
        return;
    }

    updateTimer->start(500);
    emit stateChanged(d->state = PlayingState);
}

void Qt7PlayerControl::pause()
{
    if (d->state == PlayingState) {
        movie->pause();
        emit stateChanged(d->state = PausedState);
    }
}

void Qt7PlayerControl::stop()
{
    if (d->state == PlayingState || d->state == PausedState) {
        movie->stop();
        updateTimer->stop();
        emit stateChanged(d->state = StoppedState);
    }
}

void Qt7PlayerControl::advance()
{
    navigator->advance();
}

void Qt7PlayerControl::back()
{
    navigator->back();
}

void Qt7PlayerControl::setVideoOutput(Qt7Widget *output)
{
    movie->setVideoOutput(output);
}

void Qt7PlayerControl::setSource(QMediaSource const &source)
{
    movie->setSource(source);
    if (d->state == PlayingState)
        movie->play();
}

void Qt7PlayerControl::update()
{
    const State state = Qt7PlayerControl::State(movie->state());
    if (d->state != state) {
        d->state = state;
        if (d->state == StoppedState || d->state == PausedState)
            updateTimer->stop();

        emit stateChanged(d->state);
    }

    if (d->duration != movie->duration()) {
        d->duration = movie->duration();

        emit durationChanged(d->duration);
    }

    if (d->volume != movie->volume()) {
        d->volume = movie->volume();

        emit volumeChanged(d->volume);
    }

    if (d->muted != movie->isMuted()) {
        d->muted = movie->isMuted();

        emit mutingChanged(d->muted);
    }
}

