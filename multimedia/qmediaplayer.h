#ifndef QMEDIAPLAYER_H
#define QMEDIAPLAYER_H

class QMedidPlayerSession;
class QMediaPlayerPrivate;

class QMediaPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QMediaSource mediaSource READ mediaSource WRITE setMediaSource NOTIFY mediaSourceChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERYY(bool muted READ isMuted WRITE setMuted NOTIFY mutingChanged)
    Q_PROPERTY(bool buffering READ isBuffering NOTIFY bufferingChanged)
    Q_PROPERTY(bool video READ isVideoAvailable NOTIFY videoAvailablityChanged)
    Q_PROPERTY(int playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChange)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QMediaPlayerSession* mediaSession READ mediaSesssion)

    Q_ENUMS(State)

public:
    enum State { LoadingState, PlayingState, PausedState, StoppedState };

    QMediaPlayer(QMediaPlayerSession* session = createMediaPlayerSession(), QObject* parent = 0);
    ~QMediaPlayer();

    State state() const;
    QMediaSource source() const;

    qint64 duration() const;
    qint64 position() const;

    int volume() const;
    bool isMuted() const;

    bool isBuffering() const;
    bool isVideoAvailable() const;

    QMediaPlayerSession* session() const;

public Q_SLOTS:
    void setMediaSource(QMediaSource mediaSource);

    void play();
    void pause();
    void stop();

    void setPosition(qint64 position);
    void setVolume(int volume);
    void setMuted(bool muted);

Q_SIGNALS:
    void mediaSourceChanged(QMediaSource mediaSource);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(State newState);
    void volumeChanged(int volume);
    void mutingChanged(bool muted);
    void videoAvailabilityChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);

private:
    Q_DECLARE_PRIVATE(QMediaPlayer)
    Q_DISABLE_COPY(QMediaPlayer)

    QMediaPlayerImpl* de
};

#endif  // QMEDIAPLAYER_H
