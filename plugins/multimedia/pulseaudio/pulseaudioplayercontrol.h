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
