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

#include "s60videoplayercontrol.h"
#include "s60videoplayersession.h"

#include <multimedia/qmediaplaylistnavigator.h>

#include <QtCore/qdir.h>
#include <QtCore/qsocketnotifier.h>
#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <unistd.h>

S60VideoPlayerControl::S60VideoPlayerControl(S60VideoPlayerSession *session, QObject *parent)
    : QMediaPlayerControl(parent)
    , m_session(session)
    //, m_stream(0)
    //, m_fifoNotifier(0)
    //, m_fifoCanWrite(false)
    //, m_bufferSize(0)
    //, m_bufferOffset(0)
{
   // m_fifoFd[0] = -1;
    //m_fifoFd[1] = -1;

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

S60VideoPlayerControl::~S60VideoPlayerControl()
{
    /*if (m_fifoFd[0] >= 0) {
        ::close(m_fifoFd[0]);
        ::close(m_fifoFd[1]);
        m_fifoFd[0] = -1;
        m_fifoFd[1] = -1;
    }*/
}

qint64 S60VideoPlayerControl::position() const
{
    return m_session->position();
}

qint64 S60VideoPlayerControl::duration() const
{
    return m_session->duration();
}

QMediaPlayer::State S60VideoPlayerControl::state() const
{
    return m_session->state();
}

QMediaPlayer::MediaStatus S60VideoPlayerControl::mediaStatus() const
{
    return m_session->mediaStatus();
}

int S60VideoPlayerControl::bufferStatus() const
{
    return 100;
}

int S60VideoPlayerControl::volume() const
{
    return m_session->volume();
}

bool S60VideoPlayerControl::isMuted() const
{
    return m_session->isMuted();
}

bool S60VideoPlayerControl::isSeekable() const
{
    return m_session->isSeekable();
}

qreal S60VideoPlayerControl::playbackRate() const
{
    return m_session->playbackRate();
}

void S60VideoPlayerControl::setPlaybackRate(qreal rate)
{
    m_session->setPlaybackRate(rate);
}

void S60VideoPlayerControl::setPosition(qint64 pos)
{
    m_session->seek(pos);
}

void S60VideoPlayerControl::play()
{
    m_session->play();

    /*if (m_fifoFd[1] >= 0) {
        m_fifoCanWrite = true;

        writeFifo();
    }*/
}

void S60VideoPlayerControl::pause()
{
    m_session->pause();
}

void S60VideoPlayerControl::stop()
{
    m_session->stop();
}

void S60VideoPlayerControl::setVolume(int volume)
{
    m_session->setVolume(volume);
}

void S60VideoPlayerControl::setMuted(bool muted)
{
    m_session->setMuted(muted);
}

QMediaSource S60VideoPlayerControl::media() const
{
    return m_currentResource;
}

const QIODevice *S60VideoPlayerControl::mediaStream() const
{
    return m_stream;
}

void S60VideoPlayerControl::setMedia(const QMediaSource &source, QIODevice *stream)
{
/*
    m_session->stop();

    if (m_stream) {
        closeFifo();

        disconnect(m_stream, SIGNAL(readyRead()), this, SLOT(writeFifo()));
        m_stream = 0;
    }

    m_currentResource = source;
    m_stream = stream;

    QUrl url;

    if (m_stream) {
        if (m_stream->isReadable() && openFifo()) {
            url = QUrl(QString(QLatin1String("fd://%1")).arg(m_fifoFd[0]));
        }
    } else if (!source.isNull()) {
        url = source.contentUri();
    }

    m_session->load(url);
*/
    emit currentSourceChanged(m_currentResource);
}

void S60VideoPlayerControl::setVideoOutput(QObject *output)
{
    m_session->setVideoRenderer(output);
}

bool S60VideoPlayerControl::isVideoAvailable() const
{
    return m_session->isVideoAvailable();
}

/*void S60VideoPlayerControl::writeFifo()
{

    if (m_fifoCanWrite) {
        qint64 bytesToRead = qMin<qint64>(
                m_stream->bytesAvailable(), PIPE_BUF - m_bufferSize);

        if (bytesToRead > 0) {
            int bytesRead = m_stream->read(&m_buffer[m_bufferOffset + m_bufferSize], bytesToRead);

            if (bytesRead > 0)
                m_bufferSize += bytesRead;
        }

        if (m_bufferSize > 0) {
            int bytesWritten = ::write(m_fifoFd[1], &m_buffer[m_bufferOffset], size_t(m_bufferSize));

            if (bytesWritten > 0) {
                m_bufferOffset += bytesWritten;
                m_bufferSize -= bytesWritten;

                if (m_bufferSize == 0)
                    m_bufferOffset = 0;
            } else if (errno == EAGAIN) {
                m_fifoCanWrite = false;
            } else {
                closeFifo();
            }
        }
    }

    m_fifoNotifier->setEnabled(m_stream->bytesAvailable() > 0);

}

void S60VideoPlayerControl::fifoReadyWrite(int socket)
{
    if (socket == m_fifoFd[1]) {
        m_fifoCanWrite = true;

        writeFifo();
    }
}

bool S60VideoPlayerControl::openFifo()
{
    Q_ASSERT(m_fifoFd[0] < 0);
    Q_ASSERT(m_fifoFd[1] < 0);

    if (::pipe(m_fifoFd) == 0) {
        int flags = ::fcntl(m_fifoFd[1], F_GETFD);

        if (::fcntl(m_fifoFd[1], F_SETFD, flags | O_NONBLOCK) >= 0) {
            m_fifoNotifier = new QSocketNotifier(m_fifoFd[1], QSocketNotifier::Write);

            connect(m_fifoNotifier, SIGNAL(activated(int)), this, SLOT(fifoReadyWrite(int)));

            return true;
        } else {
            qWarning("Failed to make pipe non blocking %d", errno);

            ::close(m_fifoFd[0]);
            ::close(m_fifoFd[1]);

            m_fifoFd[0] = -1;
            m_fifoFd[1] = -1;

            return false;
        }
    } else {
        qWarning("Failed to create pipe %d", errno);

        return false;
    }
}

void S60VideoPlayerControl::closeFifo()
{
    if (m_fifoFd[0] >= 0) {
        delete m_fifoNotifier;
        m_fifoNotifier = 0;

        ::close(m_fifoFd[0]);
        ::close(m_fifoFd[1]);
        m_fifoFd[0] = -1;
        m_fifoFd[1] = -1;

        m_fifoCanWrite = false;

        m_bufferSize = 0;
        m_bufferOffset = 0;
    }
}
*/

