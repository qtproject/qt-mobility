
#include "qmediarecorder.h"

#include <private/qobject_p.h>


#include "qmediasink.h"

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaRecorderPrivate
{
public:
    QMediaRecorderService*  service;
    QMediaRecorderControl* control;
};

QMediaRecorder::QMediaRecorder(QMediaRecorderService *service, QObject *parent)
    : QAbstractMediaObject(parent)
    , d(new QMediaRecorderPrivate)
{
    d->service = service;
    d->control = qobject_cast<QMediaRecorderControl *>(service->control());
}

QMediaRecorder::~QMediaRecorder()
{
    delete d->service;
    delete d;
}

void QMediaRecorder::setRecordingSource(QAbstractMediaObject* source)
{
}

void QMediaRecorder::setRecordingSink(QAbstractMediaObject* sink)
{
}

QMediaRecorder::State QMediaRecorder::state() const
{
    return QMediaRecorder::State(d->control->state());
}

QMediaSink *QMediaRecorder::sink() const
{
    return d->control->sink();
}

QAbstractMediaService* QMediaRecorder::service() const
{
    return d->service;
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

