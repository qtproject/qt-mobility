#ifndef QMEDIARECORDER_H
#define QMEDIARECORDER_H

#include <QObject>

class QMediaRecorderSession;
class QMediaRecorderPrivate;



class QMediaRecorder : public QAbstractMediaObject
{
    Q_OBJECT

    Q_PROPERTY(int position READ positioin NOTIFY positionChanged)

    Q_ENUMS(State)

public:
    enum State { LoadingState, RecordingState, PausedState, StoppedState };

    QMediaRecorder(QMediaRecorderService *service = createMediaRecorderService(), QObject *parent = 0);
    ~QMediaRecorder();

    void setRecordingSource(QAbstractMediaObject* source);
    void setRecordingSink(QAbstractMediaObject* sink);

    State state() const;
    MediaSink sink() const;

    QMediaRecorderService* service() const;

public Q_SLOTS:
    void record();
    void pause();
    void stop();

Q_SIGNALS:
    void stateChangeed(QMediaRecorder::State state);

private:
    Q_DECLARE_PRIVATE(QMediaRecorder)
    Q_DISABLE_COPY(QMediaRecorder)
};

extern QMediaRecorderService* createMediaRecorderService(QMediaServiceProvider *provider = defaultProvider("mediarecorder"));

#endif  // QMEDIARECORDER_H
