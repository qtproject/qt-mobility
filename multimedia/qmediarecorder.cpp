
#include "qmediarecorder.h"

#include "qabstractmediaobject_p.h"
#include "qmediasink.h"

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaRecorderPrivate : public QAbstractMediaObjectPrivate
{
public:
    QMediaRecorderService*  service;
    QMediaRecorderControl* control;
};

QMediaRecorder::QMediaRecorder(QMediaRecorderService *service, QObject *parent)
    : QAbstractMediaObject(*new QMediaRecorderPrivate, parent)
{
    Q_D(QMediaRecorder);

    d->service = service;
    d->control = qobject_cast<QMediaRecorderControl *>(service->control());
}

QMediaRecorder::~QMediaRecorder()
{
    Q_D(QMediaRecorder);

    delete d_func()->service;
}

void QMediaRecorder::setRecordingSource(QAbstractMediaObject* source)
{
}

void QMediaRecorder::setRecordingSink(QAbstractMediaObject* sink)
{
}

QMediaRecorder::State QMediaRecorder::state() const
{
    return QMediaRecorder::State(d_func()->control->state());
}

QMediaSink *QMediaRecorder::sink() const
{
    return d_func()->control->sink();
}

QAbstractMediaService* QMediaRecorder::service() const
{
    return d_func()->service;
}

//public Q_SLOTS:
void QMediaRecorder::record()
{
}

void QMediaRecorder::pause()
{
}

void QMediaRecorder::stop()
{
}

QMediaRecorderService* createMediaRecorderService(QMediaServiceProvider *provider)
{
    QObject *object = provider->createObject("com.nokia.Qt.RecorderService/1.0");

    if (object) {
        QMediaRecorderService *service = qobject_cast<QMediaRecorderService *>(object);

        if (service)
            return service;

        delete object;
    }
    return 0;
}

