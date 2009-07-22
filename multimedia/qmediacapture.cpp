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

#include "qmediacapture.h"
#include "qmediacapturecontrol.h"
#include "qabstractmediaobject_p.h"
#include "qabstractmediaservice.h"
#include "qmediaserviceprovider.h"

#include <QtCore/qdebug.h>


Q_MEDIA_EXPORT QAbstractMediaService *createMediaCaptureService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.qt.AudioCapture/1.0") : 0;

    if (object != 0) {
        QAbstractMediaService *service = qobject_cast<QAbstractMediaService*>(object);

        if (service != 0)
            return service;

        delete service;
    }

    return 0;
}

/*!
    \class QMediaCapture
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaCapturePrivate : public QAbstractMediaObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaCapture)

public:
    QMediaCapturePrivate(QAbstractMediaService *service);

    QAbstractMediaService* service;
    QMediaCaptureControl* control;
    QMediaCapture::Error error;
    QString errorString;

    void _q_stateChanged(int state);    
    void _q_error(int error, const QString &errorString);
};

QMediaCapturePrivate::QMediaCapturePrivate(QAbstractMediaService *service)
    :QAbstractMediaObjectPrivate(),
     service(service),
     error(QMediaCapture::NoError)
{
    control = qobject_cast<QMediaCaptureControl *>(service->control("com.nokia.qt.MediaCaptureControl"));    
}

void QMediaCapturePrivate::_q_stateChanged(int state)
{
    Q_Q(QMediaCapture);

    const QMediaCapture::State ps = QMediaCapture::State(state);

    if (ps == QMediaCapture::RecordingState)
        q->addPropertyWatch("position");
    else
        q->removePropertyWatch("position");

    emit q->stateChanged(ps);
}


void QMediaCapturePrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QMediaCapture);

    this->error = QMediaCapture::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
    emit q->errorStringChanged(this->errorString);
}


QMediaCapture::QMediaCapture(QAbstractMediaService *service, QObject *parent)
    :QAbstractMediaObject(*new QMediaCapturePrivate(service), parent)
{
    Q_D(QMediaCapture);

    if (d->control) {        
        connect(d->control, SIGNAL(stateChanged(int)), SLOT(_q_stateChanged(int)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}

QMediaCapture::QMediaCapture(QAbstractMediaObject *mediaObject, QObject *parent)
    :QAbstractMediaObject(*new QMediaCapturePrivate(mediaObject->service()), parent)
{
    Q_D(QMediaCapture);

    if (d->control) {
        connect(d->control, SIGNAL(stateChanged(int)), SLOT(_q_stateChanged(int)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}

QMediaCapture::~QMediaCapture()
{
    stop();
}

bool QMediaCapture::isValid() const
{
    return d_func()->control != 0;
}

QAbstractMediaService* QMediaCapture::service() const
{
    return d_func()->service;
}

QMediaSink QMediaCapture::sink() const
{
    return d_func()->control ? d_func()->control->sink() : QMediaSink();
}

bool QMediaCapture::setSink(const QMediaSink &sink)
{
    Q_D(QMediaCapture);
    return d->control ? d->control->setSink(sink) : false;
}

QMediaCapture::State QMediaCapture::state() const
{    
    return d_func()->control ? QMediaCapture::State(d_func()->control->state()) : StoppedState;
}

QMediaCapture::Error QMediaCapture::error() const
{    
    return d_func()->error;
}

QString QMediaCapture::errorString() const
{
    return d_func()->errorString;
}

void QMediaCapture::unsetError()
{
    Q_D(QMediaCapture);

    d->error = NoError;
    d->errorString = QString();
}

qint64 QMediaCapture::position() const
{
    return d_func()->control ? d_func()->control->position() : 0;
}

void QMediaCapture::setPositionUpdatePeriod(int ms)
{
    Q_D(QMediaCapture);
    setNotifyInterval(ms);
}


void QMediaCapture::record()
{
    Q_D(QMediaCapture);
    if (d->control)
        d->control->record();
}

void QMediaCapture::pause()
{
    Q_D(QMediaCapture);
    if (d->control)
        d->control->pause();
}

void QMediaCapture::stop()
{
    Q_D(QMediaCapture);
    if (d->control)
        d->control->stop();
}

#include "moc_qmediacapture.cpp"
