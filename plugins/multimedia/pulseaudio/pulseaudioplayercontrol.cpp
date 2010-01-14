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

#include <QtCore/qobject.h>
#include <QtCore/qdebug.h>
#include <QtNetwork>

#include "wavedecoder.h"
#include "pulseaudioplayercontrol.h"

// Less than ideal
#define PA_SCACHE_ENTRY_SIZE_MAX (1024*1024*16)

namespace
{

inline pa_sample_spec audioFormatToSampleSpec(const QAudioFormat &format)
{
    pa_sample_spec  spec;

    spec.rate = format.frequency();
    spec.channels = format.channels();

    if (format.sampleSize() == 8)
        spec.format = PA_SAMPLE_U8;
    else if (format.sampleSize() == 16) {
        switch (format.byteOrder()) {
        case QAudioFormat::BigEndian: spec.format = PA_SAMPLE_S16BE; break;
        case QAudioFormat::LittleEndian: spec.format = PA_SAMPLE_S16LE; break;
        }
    }
    else if (format.sampleSize() == 32) {
        switch (format.byteOrder()) {
        case QAudioFormat::BigEndian: spec.format = PA_SAMPLE_S32BE; break;
        case QAudioFormat::LittleEndian: spec.format = PA_SAMPLE_S32LE; break;
        }
    }

    return spec;
}

class PulseDaemon
{

public:
     PulseDaemon():m_prepared(false)
    {
        prepare();
    }

    ~PulseDaemon()
    {
        if (m_prepared)
            release();
    }

    inline void lock()
    {
        pa_threaded_mainloop_lock(m_mainLoop);
    }

    inline void unlock()
    {
        pa_threaded_mainloop_unlock(m_mainLoop);
    }

    inline pa_context *context() const
    {
        return m_context;
    }

private:
    void prepare()
    {
        m_mainLoop = pa_threaded_mainloop_new();
        if (m_mainLoop == 0) {
            qWarning("PulseAudioService: unable to create pulseaudio mainloop");
            return;
        }

        if (pa_threaded_mainloop_start(m_mainLoop) != 0) {
            qWarning("PulseAudioService: unable to start pulseaudio mainloop");
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }

        m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

        lock();
        m_context = pa_context_new(m_mainLoopApi, QString("QtPulseAudio:%1").arg(::getpid()).toAscii().constData());
        if (m_context == 0) {
            qWarning("PulseAudioService: Unable to create new pulseaudio context");
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }

        if (pa_context_connect(m_context, NULL, (pa_context_flags_t)0, NULL) < 0) {
            qWarning("PulseAudioService: pa_context_connect() failed");
            pa_context_unref(m_context);
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }
        unlock();

        m_prepared = true;
    }

    void release()
    {
        if (!m_prepared) return;
        pa_threaded_mainloop_stop(m_mainLoop);
        pa_threaded_mainloop_free(m_mainLoop);
        m_prepared = false;
    }

    bool m_prepared;
    pa_context *m_context;
    pa_threaded_mainloop *m_mainLoop;
    pa_mainloop_api *m_mainLoopApi;
};

}

Q_GLOBAL_STATIC(PulseDaemon, daemon)



PulseAudioPlayerControl::PulseAudioPlayerControl(QObject *parent) :
    QMediaPlayerControl(parent),
    m_muted(false),
    m_playQueued(false),
    m_volume(100),
    m_duration(0),
    m_dataUploaded(0),
    m_state(QMediaPlayer::StoppedState),
    m_status(QMediaPlayer::NoMedia),
    m_reply(0),
    m_stream(0),
    m_networkAccessManager(0)
{
}

PulseAudioPlayerControl::~PulseAudioPlayerControl()
{
    delete m_reply;

    unloadSample();
}

QMediaPlayer::State PulseAudioPlayerControl::state() const
{
    return m_state;
}

QMediaPlayer::MediaStatus PulseAudioPlayerControl::mediaStatus() const
{
    return m_status;
}

qint64 PulseAudioPlayerControl::duration() const
{
    return m_duration;
}

qint64 PulseAudioPlayerControl::position() const
{
    return 0;
}

void PulseAudioPlayerControl::setPosition(qint64 position)
{
    Q_UNUSED(position);
}

int PulseAudioPlayerControl::volume() const
{
    return m_volume;
}

void PulseAudioPlayerControl::setVolume(int volume)
{
    if (m_volume == volume)
        return;

    emit volumeChanged(m_volume = volume);
}

bool PulseAudioPlayerControl::isMuted() const
{
    return m_muted;
}

void PulseAudioPlayerControl::setMuted(bool muted)
{
    if (m_muted == muted)
        return;

    emit mutedChanged(m_muted = muted);
}

int PulseAudioPlayerControl::bufferStatus() const
{
    return 0;
}

bool PulseAudioPlayerControl::isVideoAvailable() const
{
    return false;
}

bool PulseAudioPlayerControl::isSeekable() const
{
    return false;
}

QPair<qint64,qint64> PulseAudioPlayerControl::seekRange() const
{
    return qMakePair(qint64(), qint64());
}

qreal PulseAudioPlayerControl::playbackRate() const
{
    return 1.0;
}

void PulseAudioPlayerControl::setPlaybackRate(qreal rate)
{
    Q_UNUSED(rate);
}

QMediaContent PulseAudioPlayerControl::media() const
{
    return m_media;
}

const QIODevice * PulseAudioPlayerControl::mediaStream() const
{
    return m_media.isNull() ? m_stream : 0;
}

void PulseAudioPlayerControl::setMedia(const QMediaContent &media, QIODevice *stream)
{
    if (media.isNull() && stream == 0) {
        m_media = QMediaContent();
        m_stream = 0;
        unloadSample();
        return;
    }

    if (stream != 0) {
        if (m_stream == stream)
            return;

        m_stream = stream;
        m_media = QMediaContent();
    }
    else {
        if (m_media == media)
            return;
        m_media = media;

        if (m_networkAccessManager == 0)
            m_networkAccessManager = new QNetworkAccessManager(this);

        m_stream = m_networkAccessManager->get(QNetworkRequest(m_media.canonicalUrl()));
    }

    unloadSample();
    loadSample();

    emit mediaChanged(m_media);
}

void PulseAudioPlayerControl::play()
{
    if (m_status == QMediaPlayer::LoadingMedia) {
        m_playQueued = true;
        return;
    }

    if (m_status != QMediaPlayer::BufferedMedia ||
        m_state == QMediaPlayer::PlayingState)
        return;

    daemon()->lock();
    pa_operation_unref(
            pa_context_play_sample(daemon()->context(),
                                   m_name.constData(),
                                   0,
                                   PA_VOLUME_NORM,
                                   play_callback,
                                   this)
            );
    daemon()->unlock();

    m_playbackTime.start();

    emit stateChanged(m_state = QMediaPlayer::PlayingState);
}

void PulseAudioPlayerControl::pause()
{
    emit stateChanged(m_state = QMediaPlayer::StoppedState);
}

void PulseAudioPlayerControl::stop()
{
    emit stateChanged(m_state = QMediaPlayer::StoppedState);
}

void PulseAudioPlayerControl::loadSample()
{
    m_waveDecoder = new WaveDecoder(m_stream);
    connect(m_waveDecoder, SIGNAL(formatKnown()), SLOT(decoderReady()));
    connect(m_waveDecoder, SIGNAL(invalidFormat()), SLOT(decoderError()));

    m_status = QMediaPlayer::LoadingMedia;
    emit mediaStatusChanged(m_status);
}

void PulseAudioPlayerControl::unloadSample()
{
    if (m_status != QMediaPlayer::BufferedMedia)
        return;

    m_status = QMediaPlayer::NoMedia;

    daemon()->lock();
    pa_context_remove_sample(daemon()->context(), m_name.constData(), NULL, NULL);
    daemon()->unlock();

    m_duration = 0;
    m_dataUploaded = 0;
}

void PulseAudioPlayerControl::timerEvent(QTimerEvent *event)
{
    if (m_state == QMediaPlayer::PlayingState) {
        m_state = QMediaPlayer::StoppedState;
        emit stateChanged(m_state);
    }

    killTimer(event->timerId());
}

void PulseAudioPlayerControl::decoderReady()
{
    if (m_waveDecoder->size() >= PA_SCACHE_ENTRY_SIZE_MAX) {
        m_status = QMediaPlayer::InvalidMedia;
        emit mediaStatusChanged(m_status);
        qWarning("QtPulseAudio: attempting to load to large a sample");
        return;
    }

    if (m_name.isNull())
        m_name = QString("QtPulseSample-%1-%2").arg(::getpid()).arg(int(this)).toUtf8();

    pa_sample_spec spec = audioFormatToSampleSpec(m_waveDecoder->audioFormat());

    daemon()->lock();
    pa_stream *stream = pa_stream_new(daemon()->context(), m_name.constData(), &spec, 0);
    pa_stream_set_state_callback(stream, stream_state_callback, this);
    pa_stream_set_write_callback(stream, stream_write_callback, this);
    pa_stream_connect_upload(stream, (size_t)m_waveDecoder->size());
    daemon()->unlock();
}

void PulseAudioPlayerControl::decoderError()
{
    emit mediaStatusChanged(m_status = QMediaPlayer::InvalidMedia);
}

void PulseAudioPlayerControl::checkPlayTime()
{
    int elapsed = m_playbackTime.elapsed();

    if (elapsed >= m_duration) {
        m_state = QMediaPlayer::StoppedState;
        emit stateChanged(m_state);
    }
    else
        startTimer(m_duration - elapsed);
}

void PulseAudioPlayerControl::stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
    Q_UNUSED(length);

    PulseAudioPlayerControl *self = reinterpret_cast<PulseAudioPlayerControl*>(userdata);

    size_t bufferSize = qMin(pa_stream_writable_size(s),
                             size_t(self->m_waveDecoder->bytesAvailable()));
    char buffer[bufferSize];

    size_t len = 0;
    while (len < length) {
        qint64 read = self->m_waveDecoder->read(buffer, qMin(bufferSize, length -len));
        if (read > 0) {
            if (pa_stream_write(s, buffer, size_t(read), 0, 0, PA_SEEK_RELATIVE) == 0)
                len += size_t(read);
            else
                break;
        }
    }
    self->m_dataUploaded += len;

    if (self->m_waveDecoder->size() == self->m_dataUploaded) {
        pa_stream_finish_upload(s);

        self->m_duration = self->m_waveDecoder->duration();
        emit self->durationChanged(self->m_duration);

        self->m_status = QMediaPlayer::BufferedMedia;
        emit self->mediaStatusChanged(self->m_status);

        self->m_waveDecoder->deleteLater();
        if (!self->m_media.isNull())
            self->m_stream->deleteLater();

        if (self->m_playQueued) {
            self->m_playQueued = false;
            QMetaObject::invokeMethod(self, "play");
        }
    }
}

void PulseAudioPlayerControl::stream_state_callback(pa_stream *s, void *userdata)
{
    PulseAudioPlayerControl *self = reinterpret_cast<PulseAudioPlayerControl*>(userdata);

    switch (pa_stream_get_state(s)) {
    case PA_STREAM_CREATING:
    case PA_STREAM_READY:
    case PA_STREAM_TERMINATED:
        break;

    case PA_STREAM_FAILED:
    default:
        self->m_status = QMediaPlayer::InvalidMedia;
        emit self->mediaStatusChanged(self->m_status);
        break;
    }
}

void PulseAudioPlayerControl::play_callback(pa_context *c, int success, void *userdata)
{
    Q_UNUSED(c);

    PulseAudioPlayerControl *self = reinterpret_cast<PulseAudioPlayerControl*>(userdata);

    if (success == 1)
        QMetaObject::invokeMethod(self, "checkPlayTime", Qt::QueuedConnection);
    else {
        self->m_state = QMediaPlayer::StoppedState;
        emit self->stateChanged(self->m_state);
    }
}

