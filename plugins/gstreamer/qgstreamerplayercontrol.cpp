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

#include "qgstreamerplayercontrol.h"
#include "qgstreamerplayersession.h"

#include "qmediaplaylistnavigator.h"
#include "qmediasource.h"

#include <QtCore/qurl.h>


QMediaPlayerControl::QMediaPlayerControl(QObject *parent)
    :QAbstractMediaControl(parent)
{
}


QGstreamerPlayerControl::QGstreamerPlayerControl(QGstreamerPlayerService *service, QObject *parent)
   :QMediaPlayerControl(parent), m_service(service)
{
    QMediaPlaylist *playlist = new QMediaPlaylist(0,this);
    m_navigator = new QMediaPlaylistNavigator(playlist,this);
    m_session = new QGstreamerPlayerSession(this);
    m_navigator->setPlaybackMode(QMediaPlaylistNavigator::Linear);

    connect(m_navigator, SIGNAL(activated(QMediaSource)),
            this, SLOT(play(QMediaSource)));
    connect(m_navigator, SIGNAL(currentPositionChanged(int)),
            this, SIGNAL(playlistPositionChanged(int)));

    connect(m_session, SIGNAL(playbackFinished()),
            m_navigator, SLOT(advance()));

    connect(m_session, SIGNAL(positionChanged(qint64)),
            this, SIGNAL(positionChanged(qint64)));
    connect(m_session, SIGNAL(durationChanged(qint64)),
            this, SIGNAL(durationChanged(qint64)));
    connect(m_session, SIGNAL(mutedStateChanged(bool)),
            this, SIGNAL(mutedStateChanged(bool)));
    connect(m_session, SIGNAL(volumeChaned(int)),
            this, SIGNAL(volumeChaned(int)));
    connect(m_session, SIGNAL(stateChanged(QMediaPlayerControl::State)),
            this, SIGNAL(stateChanged(QMediaPlayerControl::State)));
    connect(m_session,SIGNAL(bufferingChanged(bool)),
            this, SIGNAL(bufferingChanged(bool)));
    connect(m_session,SIGNAL(bufferingProgressChanged(int)),
            this, SIGNAL(bufferingProgressChanged(int)));
}

QGstreamerPlayerControl::~QGstreamerPlayerControl()
{
}

int QGstreamerPlayerControl::playlistPosition() const
{
    return m_navigator->currentPosition();
}

QMediaSource QGstreamerPlayerControl::currentMediaSource() const
{
    return m_navigator->currentItem();
}

qint64 QGstreamerPlayerControl::position() const
{
    return m_session->position();
}

qint64 QGstreamerPlayerControl::duration() const
{
    return m_session->duration();
}

int QGstreamerPlayerControl::state() const
{
    return int(m_session->state());
}

bool QGstreamerPlayerControl::isBuffering() const
{
    return false;
}


int QGstreamerPlayerControl::bufferStatus() const
{
    return 100;
}

int QGstreamerPlayerControl::volume() const
{
    return m_session->volume();
}

bool QGstreamerPlayerControl::isMuted() const
{
    return m_session->isMuted();
}

void QGstreamerPlayerControl::setPlaylistPosition(int playlistPosition)
{
    m_navigator->jump(playlistPosition);
}

void QGstreamerPlayerControl::advance()
{
    m_navigator->advance();
}

void QGstreamerPlayerControl::back()
{
    m_navigator->back();
}

void QGstreamerPlayerControl::setPosition(qint64 pos)
{
    m_session->seek(pos);
}

void QGstreamerPlayerControl::play()
{
    m_session->play();
}

void QGstreamerPlayerControl::pause()
{
    m_session->pause();
}

void QGstreamerPlayerControl::stop()
{
    m_session->stop();
}

void QGstreamerPlayerControl::setVolume(int volume)
{
    m_session->setVolume(volume);
}

void QGstreamerPlayerControl::setMuted(bool muted)
{
    m_session->setMuted(muted);    
}

void QGstreamerPlayerControl::play(const QMediaSource &src)
{
    QVariant location = src.dataLocation();
    QUrl url;

    if (location.convert(QVariant::Url)) {
        url = location.toUrl();
    } else if (location.convert(QVariant::String)) {
        url = QUrl(location.toString());
    }

    emit currentMediaChanged(src);

    if (url.isValid()) {
        m_session->stop();
        m_session->load(url);
        m_session->play();        
    } else {
        m_navigator->advance();
    }
}

QMediaPlaylist *QGstreamerPlayerControl::mediaPlaylist() const
{
    return m_navigator->playlist();
}

bool QGstreamerPlayerControl::setMediaPlaylist(QMediaPlaylist *playlist)
{
    m_navigator->setPlaylist(playlist);
    return true;
}

void QGstreamerPlayerControl::setVideoOutput(QObject *output)
{
    m_session->setVideoRenderer(output);
}

bool QGstreamerPlayerControl::isVideoAvailable() const
{
    return m_session->isVideoAvailable();
}
