#ifndef QMEDIARECORDER_H
#define QMEDIARECORDER_H

#include "qabstractmediacontrol.h"
#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"

#include "qmediaserviceprovider.h"

class QAbstractMediaObject;
class QMediaRecorderService;
class QMediaSink;

class QMediaRecorderPrivate;


extern QMediaRecorderService *createMediaRecorderService(QMediaServiceProvider *provider = defaultServiceProvider("mediarecorder"));

class QMediaRecorder : public QAbstractMediaObject
{
    Q_OBJECT

    // Q_PROPERTY(int position READ position NOTIFY positionChanged)

    Q_ENUMS(State)

public:
    enum State { LoadingState, RecordingState, PausedState, StoppedState };

    QMediaRecorder(QMediaRecorderService *service = createMediaRecorderService(), QObject *parent = 0);
    ~QMediaRecorder();

    void setRecordingSource(QAbstractMediaObject* source);
    void setRecordingSink(QAbstractMediaObject* sink);

    State state() const;
    QMediaSink *sink() const;

    QAbstractMediaService* service() const;

public Q_SLOTS:
    void record();
    void pause();
    void stop();

Q_SIGNALS:
    void stateChanged(QMediaRecorder::State state);

private:
    Q_DISABLE_COPY(QMediaRecorder)
    Q_DECLARE_PRIVATE(QMediaRecorder)
};

class QMediaRecorderService : public QAbstractMediaService
{
    Q_OBJECT
};

class QMediaRecorderControl : public QAbstractMediaControl
{
    Q_OBJECT
public:
    int state() const;
    QMediaSink *sink() const;
};

#endif  // QMEDIARECORDER_H
