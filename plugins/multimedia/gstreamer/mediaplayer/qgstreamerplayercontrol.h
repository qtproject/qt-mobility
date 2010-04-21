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

#ifndef QGSTREAMERPLAYERCONTROL_H
#define QGSTREAMERPLAYERCONTROL_H

#include <QtCore/qobject.h>

#include <qmediaplayercontrol.h>
#include <qmediaplayer.h>

#include <limits.h>

QT_BEGIN_NAMESPACE
class QMediaPlaylist;
class QMediaPlaylistNavigator;
class QSocketNotifier;
QT_END_NAMESPACE

QT_USE_NAMESPACE

class QGstreamerPlayerSession;
class QGstreamerPlayerService;

class QGstreamerPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT

public:
    QGstreamerPlayerControl(QGstreamerPlayerSession *session, QObject *parent = 0);
    ~QGstreamerPlayerControl();

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    qint64 position() const;
    qint64 duration() const;

    int bufferStatus() const;

    int volume() const;
    bool isMuted() const;

    bool isAudioAvailable() const;
    bool isVideoAvailable() const;
    void setVideoOutput(QObject *output);

    bool isSeekable() const;
    QMediaTimeRange availablePlaybackRanges() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    QMediaContent media() const;
    const QIODevice *mediaStream() const;
    void setMedia(const QMediaContent&, QIODevice *);

public Q_SLOTS:
    void setPosition(qint64 pos);

    void play();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

private Q_SLOTS:
    void writeFifo();
    void fifoReadyWrite(int socket);

    void updateState(QMediaPlayer::State);
    void processEOS();
    void setBufferProgress(int progress);

private:
    bool openFifo();
    void closeFifo();

    QGstreamerPlayerSession *m_session;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_mediaStatus;
    int m_bufferProgress;
    QMediaContent m_currentResource;
    QIODevice *m_stream;
    QSocketNotifier *m_fifoNotifier;
    int m_fifoFd[2];
    bool m_fifoCanWrite;
    int m_bufferSize;
    int m_bufferOffset;
    char m_buffer[PIPE_BUF];
};

#endif
