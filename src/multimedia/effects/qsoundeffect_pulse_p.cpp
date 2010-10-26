/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// INTERNAL USE ONLY: Do NOT use for any other purpose.
//

#include <QtCore/qcoreapplication.h>
#include <qaudioformat.h>
#include <QtNetwork>
#include <QTime>

#include "qwavedecoder_p.h"

#include "qsoundeffect_pulse_p.h"

#if defined(Q_WS_MAEMO_5)
#include <pulse/ext-stream-restore.h>
#endif

#include <unistd.h>

// Less than ideal
#define PA_SCACHE_ENTRY_SIZE_MAX (1024*1024*16)

QT_BEGIN_NAMESPACE

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

class PulseDaemon : public QObject
{
    Q_OBJECT
public:
    PulseDaemon(): m_prepared(false)
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

    int volume()
    {
        return m_vol;
    }

Q_SIGNALS:
    void contextReady();

private:
    void prepare()
    {
        m_vol = 100;

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
        m_context = pa_context_new(m_mainLoopApi, QString(QLatin1String("QtPulseAudio:%1")).arg(::getpid()).toAscii().constData());

        pa_context_set_state_callback(m_context, context_state_callback, this);

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

        //wait for the context to be ready, otherwise we will fail on stream operations later
        while(pa_context_get_state(m_context) != PA_CONTEXT_READY)
            pa_threaded_mainloop_wait(m_mainLoop);

        unlock();

        //shall we wait untill context is ready

        m_prepared = true;
    }

    void release()
    {
        if (!m_prepared) return;
        pa_threaded_mainloop_stop(m_mainLoop);
        pa_threaded_mainloop_free(m_mainLoop);
        m_prepared = false;
    }

    static void context_state_callback(pa_context *c, void *userdata)
    {
        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);
        switch (pa_context_get_state(c)) {
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
                break;
            case PA_CONTEXT_READY:
    #if defined(Q_WS_MAEMO_5)
                pa_ext_stream_restore_set_subscribe_cb(c, &stream_restore_monitor_callback, self);
                pa_ext_stream_restore_subscribe(c, 1, NULL, self);
    #endif
                pa_threaded_mainloop_signal(self->m_mainLoop, 0);
                QMetaObject::invokeMethod(self, "contextReady", Qt::QueuedConnection);
                break;
            default:
                break;
        }
    }

#if defined(Q_WS_MAEMO_5)

    void stream_restore_monitor_callback(pa_context *c, void *userdata)
    {
        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);
        pa_ext_stream_restore2_read(c, &stream_restore_info_callback, self);
    }

    void stream_restore_info_callback(pa_context *c, const pa_ext_stream_restore2_info *info,
            int eol, void *userdata)
    {
        Q_UNUSED(c)

        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);

        if (!eol) {
            if (QString(info->name).startsWith(QLatin1String("sink-input-by-media-role:x-maemo"))) {
                const unsigned str_length = 256;
                char str[str_length];
                pa_cvolume_snprint(str, str_length, &info->volume);
                self->m_vol = QString(str).replace(" ","").replace("%","").mid(2).toInt();
            }
        }
    }
#endif

    int  m_vol;
    bool m_prepared;
    pa_context *m_context;
    pa_threaded_mainloop *m_mainLoop;
    pa_mainloop_api *m_mainLoopApi;
};

}

Q_GLOBAL_STATIC(PulseDaemon, daemon)


QSoundEffectPrivate::QSoundEffectPrivate(QObject* parent):
    QObject(parent),
    m_retry(false),
    m_muted(false),
    m_playQueued(false),
    m_sampleLoaded(false),
    m_volume(100),
    m_duration(0),
    m_dataUploaded(0),
    m_loopCount(1),
    m_runningCount(0),
    m_playing(false),
    m_status(QSoundEffect::Null),
    m_reply(0),
    m_stream(0),
    m_pulseStream(0),
    m_timerID(0),
    m_waveDecoder(0),
    m_writeCallbackPulseStream(0),
    m_networkAccessManager(0)
{
}

QSoundEffectPrivate::~QSoundEffectPrivate()
{
    if (m_timerID != 0)
        killTimer(m_timerID);
    m_timerID = 0;
    if (m_reply)
        m_reply->deleteLater();

    unloadSample();

    clearTasks();
}

QStringList QSoundEffectPrivate::supportedMimeTypes()
{
    QStringList supportedTypes;
    supportedTypes << QLatin1String("audio/x-wav") << QLatin1String("audio/vnd.wave") ;
    return supportedTypes;
}

QUrl QSoundEffectPrivate::source() const
{
    return m_source;
}

void QSoundEffectPrivate::setSource(const QUrl &url)
{
    clearTasks();

    if (url.isEmpty()) {
        m_source = QUrl();
        unloadSample();
        setStatus(QSoundEffect::Null);
        return;
    }

    m_source = url;

    if (m_networkAccessManager == 0)
        m_networkAccessManager = new QNetworkAccessManager(this);

    m_stream = m_networkAccessManager->get(QNetworkRequest(m_source));

    unloadSample();
    loadSample();
}

int QSoundEffectPrivate::loopCount() const
{
    return m_loopCount;
}

void QSoundEffectPrivate::setLoopCount(int loopCount)
{
    if (loopCount == 0)
        loopCount = 1;
    m_loopCount = loopCount;
}

int QSoundEffectPrivate::volume() const
{
    return m_volume;
}

void QSoundEffectPrivate::setVolume(int volume)
{
    m_volume = volume;
}

bool QSoundEffectPrivate::isMuted() const
{
    return m_muted;
}

void QSoundEffectPrivate::setMuted(bool muted)
{
    m_muted = muted;
}

bool QSoundEffectPrivate::isLoaded() const
{
    return m_status == QSoundEffect::Ready;
}

bool QSoundEffectPrivate::isPlaying() const
{
    return m_playing;
}

QSoundEffect::Status QSoundEffectPrivate::status() const
{
    return m_status;
}

void QSoundEffectPrivate::setPlaying(bool playing)
{
    if (m_playing == playing)
        return;
    m_playing = playing;
    emit playingChanged();
}

void QSoundEffectPrivate::setStatus(QSoundEffect::Status status)
{
    if (m_status == status)
        return;
    bool oldLoaded = isLoaded();
    m_status = status;
    emit statusChanged();
    if (oldLoaded != isLoaded())
        emit loadedChanged();
}

void QSoundEffectPrivate::play()
{
    if (m_retry) {
        m_retry = false;
        setSource(m_source);
        return;
    }

    if (!m_sampleLoaded) {
        m_playQueued = true;
    } else if (m_runningCount >= 0) {
        if (m_runningCount > 0) {
            if (m_timerID != 0)
                killTimer(m_timerID);
            m_timerID = 0;
        }
        m_runningCount = m_loopCount;
        playSample();
    }

    setPlaying(true);
}

void QSoundEffectPrivate::decoderReady()
{
    if (m_waveDecoder->size() >= PA_SCACHE_ENTRY_SIZE_MAX) {
        m_waveDecoder->deleteLater();
        m_waveDecoder = 0;
        qWarning("QSoundEffect(pulseaudio): Attempting to load to large a sample");
        return;
    }

    if (m_name.isNull())
        m_name = QString(QLatin1String("QtPulseSample-%1-%2")).arg(::getpid()).arg(quintptr(this)).toUtf8();

    daemon()->lock();

    if (pa_context_get_state(daemon()->context()) != PA_CONTEXT_READY) {
        connect(daemon(), SIGNAL(contextReady()), SLOT(contextReady()));
        daemon()->unlock();
        return;
    }

    createPulseStream();

    daemon()->unlock();
}

void QSoundEffectPrivate::decoderError()
{
    qWarning("QSoundEffect(pulseaudio): Error decoding source");
    setStatus(QSoundEffect::Error);
    setPlaying(false);
}

void QSoundEffectPrivate::checkPlayTime()
{
    int elapsed = m_playbackTime.elapsed();

    if (elapsed < m_duration)
        m_timerID = startTimer(m_duration - elapsed);
}

void QSoundEffectPrivate::loadSample()
{
    m_sampleLoaded = false;
    m_dataUploaded = 0;
    m_waveDecoder = new QWaveDecoder(m_stream);
    connect(m_waveDecoder, SIGNAL(formatKnown()), SLOT(decoderReady()));
    connect(m_waveDecoder, SIGNAL(invalidFormat()), SLOT(decoderError()));
    setStatus(QSoundEffect::Loading);
}

void QSoundEffectPrivate::unloadSample()
{
    if (!m_sampleLoaded)
        return;

    daemon()->lock();
    pa_context_remove_sample(daemon()->context(), m_name.constData(), NULL, NULL);
    daemon()->unlock();

    m_duration = 0;
    m_dataUploaded = 0;
    m_sampleLoaded = false;
}

void QSoundEffectPrivate::uploadSample()
{
    daemon()->lock();

    if (m_writeCallbackPulseStream != m_pulseStream) {
        daemon()->unlock();
        return;
    }

    size_t bufferSize = qMin(pa_stream_writable_size(m_pulseStream),
            size_t(m_waveDecoder->bytesAvailable()));
    char buffer[bufferSize];

    size_t len = 0;
    while (len < (m_waveDecoder->size())) {
        qint64 read = m_waveDecoder->read(buffer, qMin((int)bufferSize,
                      (int)(m_waveDecoder->size()-len)));
        if (read > 0) {
            if (pa_stream_write(m_pulseStream, buffer, size_t(read), 0, 0, PA_SEEK_RELATIVE) == 0)
                len += size_t(read);
            else
                break;
        }
    }

    m_dataUploaded += len;
    pa_stream_set_write_callback(m_pulseStream, NULL, NULL);

    if (m_waveDecoder->size() == m_dataUploaded) {
        int err = pa_stream_finish_upload(m_pulseStream);
        if(err != 0) {
            qWarning("pa_stream_finish_upload() err=%d",err);
            pa_stream_disconnect(m_pulseStream);
            m_retry = true;
            m_playQueued = false;
            //QMetaObject::invokeMethod(this, "play");
            this->play();
            daemon()->unlock();
            return;
        }
        pa_stream_disconnect(m_pulseStream);
        m_duration = m_waveDecoder->duration();
        m_waveDecoder->deleteLater();
        m_waveDecoder = 0;
        m_stream->deleteLater();
        m_stream =0;
        m_sampleLoaded = true;
        if (m_playQueued) {
            m_playQueued = false;
            //QMetaObject::invokeMethod(this, "play");
            this->play();
        }
    }
    daemon()->unlock();
    setStatus(QSoundEffect::Ready);
}

void QSoundEffectPrivate::playSample()
{
    pa_volume_t volume;
    daemon()->lock();
#ifdef Q_WS_MAEMO_5
    volume = PA_VOLUME_NORM / 100 * ((daemon()->volume() + m_volume) / 2);
#else
    volume = pa_sw_volume_from_linear(qreal(m_volume) * 0.01);
#endif
    pa_operation_unref(
            pa_context_play_sample(daemon()->context(),
                m_name.constData(),
                0,
                volume,
                play_callback,
                this)
            );
    daemon()->unlock();

    m_playbackTime.start();
}

void QSoundEffectPrivate::stop()
{
    m_runningCount = 0;
    m_playQueued = false;
    if (m_timerID != 0)
        killTimer(m_timerID);
    m_timerID = 0;
    setPlaying(false);
}

void QSoundEffectPrivate::createPulseStream()
{
    pa_sample_spec spec = audioFormatToSampleSpec(m_waveDecoder->audioFormat());
    pa_stream *stream = pa_stream_new(daemon()->context(), m_name.constData(), &spec, 0);
    if (stream == 0) {
        qWarning("QSoundEffect(pulseaudio): Failed to create stream");
    }
    else {
        pa_stream_set_state_callback(stream, stream_state_callback, this);
        pa_stream_set_write_callback(stream, stream_write_callback, this);
        if (pa_stream_connect_upload(stream, m_waveDecoder->size()) != 0)
            qWarning("QSoundEffect(pulseaudio): Failed to connect stream, error = %s", pa_strerror(pa_context_errno(daemon()->context())));
    }
    m_pulseStream = stream;
}

void QSoundEffectPrivate::timerEvent(QTimerEvent *event)
{
    Q_ASSERT(m_runningCount != 0);
    if (m_runningCount < 0)
        playSample();
    else if (--m_runningCount > 0)
        playSample();
    else
        setPlaying(false);
    m_timerID = 0;
    killTimer(event->timerId());
}

void QSoundEffectPrivate::contextReady()
{
    qWarning("context ready callback");
    daemon()->lock();
    createPulseStream();
    daemon()->unlock();
}

void QSoundEffectPrivate::clearTasks()
{
    if (m_waveDecoder) {
        disconnect(m_waveDecoder, SIGNAL(formatKnown()), this, SLOT(decoderReady()));
        disconnect(m_waveDecoder, SIGNAL(invalidFormat()), this, SLOT(decoderError()));
        m_waveDecoder->deleteLater();
    }
    m_waveDecoder = 0;

    if (m_stream)
        m_stream->deleteLater();
    m_stream = 0;

    disconnect(daemon(), SIGNAL(contextReady()), this, SLOT(contextReady()));

    if (m_pulseStream) {
        daemon()->lock();
        pa_stream_disconnect(m_pulseStream);
        pa_stream_unref(m_pulseStream);
        m_pulseStream = 0;
        daemon()->unlock();
    }
}

void QSoundEffectPrivate::stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
    Q_UNUSED(length);

    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);
    self->m_writeCallbackPulseStream = s;
    QMetaObject::invokeMethod(self, "uploadSample", Qt::QueuedConnection);
}

void QSoundEffectPrivate::stream_state_callback(pa_stream *s, void *userdata)
{
    switch (pa_stream_get_state(s)) {
        case PA_STREAM_CREATING:
        case PA_STREAM_READY:
        case PA_STREAM_TERMINATED:
            break;

        case PA_STREAM_FAILED:
        default:
            qWarning("QSoundEffect(pulseaudio): Error in pulse audio stream");
            break;
    }
}

void QSoundEffectPrivate::play_callback(pa_context *c, int success, void *userdata)
{
    Q_UNUSED(c);

    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);

    if (success == 1) {
        QMetaObject::invokeMethod(self, "checkPlayTime", Qt::QueuedConnection);
    }
}

QT_END_NAMESPACE

#include "moc_qsoundeffect_pulse_p.cpp"
#include "qsoundeffect_pulse_p.moc"
