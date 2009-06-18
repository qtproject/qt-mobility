
#include <private/qobject_p.h>

#include "qmediarecorder.h"

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaRecorderPrivate : public QAbstractNediaObject
{
public:
    QMediaRecorderSession*  session;
    QRecordControl* control;
}:

QMediaRecorder::QMediaRecorder(QMediaRecorderSession *session, QObject *parent):
    QAbstractMediaSession(*new QMediaRecorderPrivate, parent)
{
    d_func()->session = session;
}

QMediaRecorder::~QMediaRecorder()
{
    Q_D(QMediaRecorder)

    delete d->control;
    delete d->service;
}

void QMediaRecorder::setRecordingSource(QAbstractMediaObject* source)
{
}

void QMediaRecorder::setRecordingSink(QAbstractMediaObject* sink)
{
}

QMediaRecorder::State state() const
{
    return d_func()->control->state();
}

QMediaSink QMediaRecorder::sink() const
{
    return d_func()->control->sink();
}

QMediaRecorderSession* QMediaRecorder::session() const
{
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

void QMediaRecorder::setVolume(int volume)
{
}

void QMediaRecorder::setMuted(bool muted)
{
}

QMediaRecorderService* createMediaRecorderService(QMediaServiceProvider *provider)
{
    return qobject_cast<QMediaRecorderService*>(provider->createObject("com.nokia.Qt.RecorderService/1.0"));
}

