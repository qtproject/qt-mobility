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

    void _q_stateChanged(int state);
    void _q_error(int error, const QString &errorString);
};

QMediaRecorderPrivate::QMediaRecorderPrivate(QAbstractMediaService *service)
    :QAbstractMediaObjectPrivate(),
     service(service),
     error(QMediaRecorder::NoError)
{
    control = qobject_cast<QMediaRecorderControl *>(service->control(QMediaRecorderControl_iid));
}

void QMediaRecorderPrivate::_q_stateChanged(int state)
{
    Q_Q(QMediaRecorder);

    const QMediaRecorder::State ps = QMediaRecorder::State(state);

    if (ps == QMediaRecorder::RecordingState)
        q->addPropertyWatch("position");
    else
        q->removePropertyWatch("position");

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


QMediaRecorder::QMediaRecorder(QAbstractMediaService *service, QObject *parent)
    :QAbstractMediaObject(*new QMediaRecorderPrivate(service), parent)
{
    Q_D(QMediaRecorder);

    if (d->control) {
        connect(d->control, SIGNAL(stateChanged(int)), SLOT(_q_stateChanged(int)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}

QMediaRecorder::QMediaRecorder(QAbstractMediaObject *mediaObject, QObject *parent)
    :QAbstractMediaObject(*new QMediaRecorderPrivate(mediaObject->service()), parent)
{
    Q_D(QMediaRecorder);

    if (d->control) {
        connect(d->control, SIGNAL(stateChanged(int)), SLOT(_q_stateChanged(int)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}

QMediaRecorder::~QMediaRecorder()
{
    stop();
}

bool QMediaRecorder::isValid() const
{
    return d_func()->control != 0;
}

QAbstractMediaService* QMediaRecorder::service() const
{
    return d_func()->service;
}

QMediaSink QMediaRecorder::sink() const
{
    return d_func()->control ? d_func()->control->sink() : QMediaSink();
}

bool QMediaRecorder::setSink(const QMediaSink &sink)
{
    Q_D(QMediaRecorder);
    return d->control ? d->control->setSink(sink) : false;
}

QMediaRecorder::State QMediaRecorder::state() const
{
    return d_func()->control ? QMediaRecorder::State(d_func()->control->state()) : StoppedState;
}

QMediaRecorder::Error QMediaRecorder::error() const
{
    return d_func()->error;
}

QString QMediaRecorder::errorString() const
{
    return d_func()->errorString;
}

void QMediaRecorder::unsetError()
{
    Q_D(QMediaRecorder);

    d->error = NoError;
    d->errorString = QString();
}

qint64 QMediaRecorder::position() const
{
    return d_func()->control ? d_func()->control->position() : 0;
}

void QMediaRecorder::setPositionUpdatePeriod(int ms)
{
    setNotifyInterval(ms);
}


void QMediaRecorder::record()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->record();
}

void QMediaRecorder::pause()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->pause();
}

void QMediaRecorder::stop()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->stop();
}

#include "moc_qmediarecorder.cpp"
