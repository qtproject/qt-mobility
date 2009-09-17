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

#include "qgstreamerplayercontrol.h"
#include "qgstreamerplayersession.h"

#include "qmediaplaylistnavigator.h"

#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>


QGstreamerPlayerControl::QGstreamerPlayerControl(QGstreamerPlayerSession *session, QObject *parent)
   :QMediaPlayerControl(parent), m_session(session)
{
    connect(m_session, SIGNAL(positionChanged(qint64)),
            this, SIGNAL(positionChanged(qint64)));
    connect(m_session, SIGNAL(durationChanged(qint64)),
            this, SIGNAL(durationChanged(qint64)));
    connect(m_session, SIGNAL(mutedStateChaned(bool)),
            this, SIGNAL(mutingChanged(bool)));
    connect(m_session, SIGNAL(volumeChanged(int)),
            this, SIGNAL(volumeChanged(int)));
    connect(m_session, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SIGNAL(stateChanged(QMediaPlayer::State)));
    connect(m_session, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_session,SIGNAL(bufferingProgressChanged(int)),
            this, SIGNAL(bufferStatusChanged(int)));
    connect(m_session, SIGNAL(videoAvailabilityChanged(bool)),
            this, SIGNAL(videoAvailabilityChanged(bool)));
    connect(m_session, SIGNAL(seekableChanged(bool)),
            this, SIGNAL(seekableChanged(bool)));
}

QGstreamerPlayerControl::~QGstreamerPlayerControl()
{
}

qint64 QGstreamerPlayerControl::position() const
{
    return m_session->position();
}

qint64 QGstreamerPlayerControl::duration() const
{
    return m_session->duration();
}

QMediaPlayer::State QGstreamerPlayerControl::state() const
{
    return m_session->state();
}

QMediaPlayer::MediaStatus QGstreamerPlayerControl::mediaStatus() const
{
    return m_session->mediaStatus();
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

bool QGstreamerPlayerControl::isSeekable() const
{
    return m_session->isSeekable();
}

float QGstreamerPlayerControl::playbackRate() const
{
    return m_session->playbackRate();
}

void QGstreamerPlayerControl::setPlaybackRate(float rate)
{
    m_session->setPlaybackRate(rate);
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

QMediaSource QGstreamerPlayerControl::media() const
{
    return m_currentResource;
}

const QIODevice *QGstreamerPlayerControl::mediaStream() const
{
    return 0;
}

void QGstreamerPlayerControl::setMedia(const QMediaSource &source, QIODevice *)
{
    m_currentResource = source;

    QUrl url;

    if (!source.isNull())
        url = source.contentUri();

    m_session->stop();
    m_session->load(url);

    emit currentSourceChanged(m_currentResource);
}

void QGstreamerPlayerControl::setVideoOutput(QObject *output)
{
    m_session->setVideoRenderer(output);
}

bool QGstreamerPlayerControl::isVideoAvailable() const
{
    return m_session->isVideoAvailable();
}
