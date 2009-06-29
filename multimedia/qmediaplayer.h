#ifndef QMEDIAPLAYER_H
#define QMEDIAPLAYER_H

#include <QtCore/qobject.h>

class QMedidPlayerSession;
class QMediaPlayerPrivate;

class QMediaPlayer : public QAbstractMediaObject
{
    Q_OBJECT

    Q_PROPERTY(QMediaPlaylist* mediaPlaylist READ mediaPlaylist WRITE setMediaPlaylist NOTIFY mediaPlaylistChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERYY(bool muted READ isMuted WRITE setMuted NOTIFY mutingChanged)
    Q_PROPERTY(bool buffering READ isBuffering NOTIFY bufferingChanged)
    Q_PROPERYY(int bufferStatus READ bufferStatus NOTIFY bufferStatusChanged)
    Q_PROPERTY(bool video READ isVideoAvailable NOTIFY videoAvailablityChanged)
    Q_PROPERTY(int playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChange)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    Q_ENUMS(State)

public:
    enum State { LoadingState, PlayingState, PausedState, StoppedState, SeekingState, EndOfStreamState };

    QMediaPlayer(QMediaPlayerService *service = createMediaPlayerService(), QObjectl *parent = 0);
    ~QMediaPlayer();

    State state() const;
    QMediaPlaylist* mediaPlaylist() const;

    qint64 duration() const;
    qint64 position() const;

    int volume() const;
    bool isMuted() const;

    bool isBuffering() const;
    int bufferStatus() const;

    bool isVideoAvailable() const;

    QMediaPlayerService* service() const;

public Q_SLOTS:
    void setMediaPlaylist(QMediaPlaylist *mediaPlaylist);

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
    void bufferStatusChanged(int percentFilled);

private:
    Q_DISABLE_COPY(QMediaPlayer)
    QMediaPlayerPrivate* d;
};

extern QMediaPlayerService* createMediaPlayerService(QMediaServiceProvider *provider = defaultServiceProvider("mediaplayer"));

#endif  // QMEDIAPLAYER_H
