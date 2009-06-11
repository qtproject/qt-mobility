#ifndef QMEDIARECORDER_H
#define QMEDIARECORDER_H

#include <QObject>

class QMediaRecorderSession;
class QMediaRecorderPrivate;

class QMediaRecorder : public QObject
{
    Q_OBJECT

    Q_ENUMS(State)

public:
    enum State { LoadingState, RecordingState, PausedState, StoppedState };

    QMediaRecorder(QMediaRecorderSession* session = createMediaSession(), QObject* parent = 0);
    ~QMediaRecorder();

    State state() const;
    MediaSink sink() const;

    QMediaRecorderSession* session() const;

public Q_SLOTS:
    void setMediaSink(MediaSink);

    void record();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

Q_SIGNALS:
    void stateChangeed(oldState, newState);

private:
    Q_DECLARE_PRIVATE(QMediaRecorder)
    Q_DISABLE_COPY(QMediaRecorder)
};

#endif  // QMEDIARECORDER_H
