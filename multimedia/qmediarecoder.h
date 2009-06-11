class QMediaRecorder : public QObject
{
public:
    QMediaRecorder(QMediaRecorderSession* = createMediaSession());

    State state() const;
    MediaSink sink() const
    MediaSession* session() const;

slots:
    void setMediaSink(MediaSink);

    void record();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void stateChangeed(oldState, newState);
};

