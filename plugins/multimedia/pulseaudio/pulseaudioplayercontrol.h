/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef PULSEAUDIOPLAYERCONTROL_H
#define PULSEAUDIOPLAYERCONTROL_H

#include <pulse/pulseaudio.h>

#include <QTime>

#include <qmediaplayercontrol.h>
#include <qmediacontent.h>

class QNetworkReply;
class QNetworkAccessManager;
class WaveDecoder;


QTM_USE_NAMESPACE

class PulseAudioPlayerControl : public QMediaPlayerControl
{
Q_OBJECT
public:
    explicit PulseAudioPlayerControl(QObject *parent = 0);
    ~PulseAudioPlayerControl();

    QMediaPlayer::State state() const;

    QMediaPlayer::MediaStatus mediaStatus() const;

    qint64 duration() const;

    qint64 position() const;
    void setPosition(qint64 position);

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    int bufferStatus() const;

    bool isVideoAvailable() const;

    bool isSeekable() const;
    QPair<qint64,qint64> seekRange() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    QMediaContent media() const;
    const QIODevice *mediaStream() const;
    void setMedia(const QMediaContent &media, QIODevice *stream);

public slots:
    void play();
    void pause();
    void stop();

private slots:
    void decoderReady();
    void decoderError();
    void checkPlayTime();

private:
    void loadSample();
    void unloadSample();
    void timerEvent(QTimerEvent *event);

    static void stream_write_callback(pa_stream *s, size_t length, void *userdata);
    static void stream_state_callback(pa_stream *s, void *userdata);
    static void play_callback(pa_context *c, int success, void *userdata);

    bool    m_muted;
    bool    m_playQueued;
    int     m_volume;
    int     m_duration;
    int     m_dataUploaded;
    QTime  m_playbackTime;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_status;
    QByteArray m_name;
    QMediaContent   m_media;
    QNetworkReply *m_reply;
    WaveDecoder *m_waveDecoder;
    QIODevice *m_stream;
    QNetworkAccessManager *m_networkAccessManager;
};

#endif // PULSEAUDIOPLAYERCONTROL_H
