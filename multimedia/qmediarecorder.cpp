/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmediarecorder.h"

#include "qmediarecordercontrol.h"
#include "qabstractmediaobject_p.h"
#include "qabstractmediaservice.h"
#include "qmediaserviceprovider.h"

#include <QtCore/qdebug.h>


Q_MEDIA_EXPORT QAbstractMediaService *createMediaCaptureService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.qt.AudioRecorder/1.0") : 0;

    if (object != 0) {
        QAbstractMediaService *service = qobject_cast<QAbstractMediaService*>(object);

        if (service != 0)
            return service;

        delete object;
    }

    return 0;
}

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaRecorderPrivate : public QAbstractMediaObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaRecorder)

public:
    QMediaRecorderPrivate(QAbstractMediaService *service);

    QAbstractMediaService* service;
    QMediaRecorderControl* control;
    QMediaRecorder::Error error;
    QString errorString;

    void _q_stateChanged(QMediaRecorder::State state);
    void _q_error(int error, const QString &errorString);
};

QMediaRecorderPrivate::QMediaRecorderPrivate(QAbstractMediaService *service)
    :QAbstractMediaObjectPrivate(),
     service(service),
     error(QMediaRecorder::NoError)
{
    if(service)
        control = qobject_cast<QMediaRecorderControl *>(service->control(QMediaRecorderControl_iid));
    else
        control = 0;
}

void QMediaRecorderPrivate::_q_stateChanged(QMediaRecorder::State ps)
{
    Q_Q(QMediaRecorder);

    if (ps == QMediaRecorder::RecordingState)
        q->addPropertyWatch("duration");
    else
        q->removePropertyWatch("duration");

    emit q->stateChanged(ps);
}


void QMediaRecorderPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QMediaRecorder);

    this->error = QMediaRecorder::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
    emit q->errorStringChanged(this->errorString);
}

/*!
    Construct a media recorder object with \a service and \a parent.
*/

QMediaRecorder::QMediaRecorder(QAbstractMediaService *service, QObject *parent)
    :QAbstractMediaObject(*new QMediaRecorderPrivate(service), parent)
{
    Q_D(QMediaRecorder);

    if (d->control) {
        connect(d->control, SIGNAL(stateChanged(QMediaRecorder::State)), SLOT(_q_stateChanged(QMediaRecorder::State)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}

/*!
    Construct a media recorder object with \a mediaObject and \a parent.
*/

QMediaRecorder::QMediaRecorder(QAbstractMediaObject *mediaObject, QObject *parent)
    :QAbstractMediaObject(*new QMediaRecorderPrivate(mediaObject->service()), parent)
{
    Q_D(QMediaRecorder);

    if (d->control) {
        connect(d->control, SIGNAL(stateChanged(QMediaRecorder::State)), SLOT(_q_stateChanged(QMediaRecorder::State)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}

/*!
    Destruct the media recorder object.
*/

QMediaRecorder::~QMediaRecorder()
{
    stop();
}

/*!
    Returns true if a valid recording device is available.
*/

bool QMediaRecorder::isValid() const
{
    return d_func()->control != 0;
}

/*!
    Returns the media service being used.
*/

QAbstractMediaService* QMediaRecorder::service() const
{
    return d_func()->service;
}

/*!
    Returns the sink being used.
*/

QMediaSink QMediaRecorder::sink() const
{
    return d_func()->control ? d_func()->control->sink() : QMediaSink();
}

/*!
    Returns true if set of sink being used to \a sink is successful.
*/

bool QMediaRecorder::setSink(const QMediaSink &sink)
{
    Q_D(QMediaRecorder);
    return d->control ? d->control->setSink(sink) : false;
}

/*!
    Return the current state.
*/

QMediaRecorder::State QMediaRecorder::state() const
{
    return d_func()->control ? QMediaRecorder::State(d_func()->control->state()) : StoppedState;
}

/*!
    Return the current error state.
*/

QMediaRecorder::Error QMediaRecorder::error() const
{
    return d_func()->error;
}

/*!
    Return the current error string.
*/

QString QMediaRecorder::errorString() const
{
    return d_func()->errorString;
}

/*!
    Clear the current error.
*/

void QMediaRecorder::unsetError()
{
    Q_D(QMediaRecorder);

    d->error = NoError;
    d->errorString = QString();
}

/*!
    \property QMediaRecorder::duration
    \brief Recorded media duration in milliseconds.
*/

/*!
    Return the current duration in milliseconds.
*/

qint64 QMediaRecorder::duration() const
{
    return d_func()->control ? d_func()->control->duration() : 0;
}

/*!
    Set the duration update period to \a ms in milliseconds.
*/

void QMediaRecorder::setPositionUpdatePeriod(int ms)
{
    setNotifyInterval(ms);
}

/*!
    Start recording.
*/

void QMediaRecorder::record()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->record();
}

/*!
    Pause recording.
*/

void QMediaRecorder::pause()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->pause();
}

/*!
    Stop recording.
*/

void QMediaRecorder::stop()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->stop();
}

/*!
    \enum QMediaRecorder::State

    \value StoppedState    The recorder is not active.
    \value RecordingState  The recorder is currently active and producing data.
    \value PausedState     The recorder is paused.
*/

/*!
    \enum QMediaRecorder::Error

    \value NoError         No Errors.
    \value ResourceError   Device is not ready or not available.
    \value FormatError     Current format is not supported.
    \value NetworkError    ?
*/

/*!
    \fn QMediaRecorder::stateChanged(State state)

    Signal emitted when \a state changed.
*/

/*!
    \fn QMediaRecorder::durationChanged(qint64 duration)

    Signal emitted when \a duration changed.
*/

/*!
    \fn QMediaRecorder::error(QMediaRecorder::Error error)

    Signal emitted when \a error changed.
*/

/*!
    \fn QMediaRecorder::errorStringChanged(const QString &error)

    Signal emitted when \a error changed.
*/

#include "moc_qmediarecorder.cpp"
