/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60MEDIAPLAYERCONTROL_H
#define S60MEDIAPLAYERCONTROL_H

#include <qmediaplayercontrol.h>
#include "s60mediasettings.h"

QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QMediaTimeRange;
class QMediaContent;
QT_END_NAMESPACE

QT_USE_NAMESPACE

class S60MediaPlayerSession;
class S60MediaPlayerService;

class S60MediaPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT

public:
    S60MediaPlayerControl(S60MediaPlayerService *service);
    ~S60MediaPlayerControl();

    // QMediaPlayerControl
    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;
    qint64 duration() const;
    qint64 position() const;
    void setPosition(qint64 pos);
    int volume() const;
    void setVolume(int volume);
    bool isMuted() const;
    void setMuted(bool muted);
    int bufferStatus() const;
    bool isAudioAvailable() const;
    bool isVideoAvailable() const;
    bool isSeekable() const;
    QMediaTimeRange availablePlaybackRanges() const;
    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);
    QMediaContent media() const;
    const QIODevice *mediaStream() const;
    void setMedia(const QMediaContent &source, QIODevice *device);
    void play();
    void pause();
    void stop();

    // Own methods
    S60MediaPlayerSession* session();
    void setVideoOutput(QObject *output);
    QObject *videoOutput() const;
    const S60MediaSettings& mediaControlSettings() const;
    void setAudioEndpoint(const QString &name);
    void setMediaType(S60MediaSettings::MediaType type);

private:
    S60MediaPlayerService *m_service;
    S60MediaPlayerSession *m_session;
    QMediaContent m_media;
    QIODevice *m_stream;
    S60MediaSettings *m_mediaSettings;
};

#endif
