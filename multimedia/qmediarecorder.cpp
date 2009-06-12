
#include <private/qobject_p.h>

#include "qmediarecorder.h"

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaRecorderPrivate : public QObjectPrivate
{
public:
    QMediaRecorderSession*  session;
}:

QMediaRecorder::QMediaRecorder(QMediaRecorderSession* session, QObject* parent):
    QObject(*new QMediaRecorderPrivate, parent)
{
    d_func()->session = session;
}

QMediaRecorder::~QMediaRecorder()
{
}

QMediaRecorder::State state() const
{
}

QMediaSink QMediaRecorder::sink() const
{
}

QMediaRecorderSession* QMediaRecorder::session() const
{
}

//public Q_SLOTS:
void QMediaRecorder::setMediaSink(QMediaSink sink)
{
}

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

