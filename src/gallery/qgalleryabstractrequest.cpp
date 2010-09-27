/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgalleryabstractrequest_p.h"

#include "qabstractgallery.h"

QTM_BEGIN_NAMESPACE

void QGalleryAbstractRequestPrivate::_q_finished()
{
    if (status == QGalleryAbstractRequest::Active
            || status == QGalleryAbstractRequest::Cancelling
            || status == QGalleryAbstractRequest::Idle) {
        if (response->error() != QGalleryAbstractRequest::NoError) {
            error = response->error();
            errorString = response->errorString();

            status = QGalleryAbstractRequest::Error;

            emit q_func()->error(error, errorString);
            emit q_func()->errorChanged();
            emit q_func()->statusChanged(status);
        } else if (status == QGalleryAbstractRequest::Idle) {
            if (!response->isIdle()) {
                status = QGalleryAbstractRequest::Finished;

                emit q_func()->statusChanged(status);
            }
        } else if (response->isIdle()) {
            status = QGalleryAbstractRequest::Idle;

            if (!wasIdle) {
                wasIdle = true;

                emit q_func()->finished();
            }

            emit q_func()->statusChanged(status);
        } else if (!response->isActive()) {
            status = QGalleryAbstractRequest::Finished;

            if (!wasIdle)
                emit q_func()->finished();

            emit q_func()->statusChanged(status);
        }
    }
}

void QGalleryAbstractRequestPrivate::_q_cancelled()
{
    if (status == QGalleryAbstractRequest::Cancelling) {
        if (!wasIdle) {
            status = QGalleryAbstractRequest::Cancelled;

            emit q_func()->cancelled();
        } else {
            status = QGalleryAbstractRequest::Finished;
        }
        emit q_func()->statusChanged(status);
    }
}

void QGalleryAbstractRequestPrivate::_q_resumed()
{
    if (status == QGalleryAbstractRequest::Idle && !response->isIdle()) {
        status = QGalleryAbstractRequest::Active;

        emit q_func()->statusChanged(status);
    }
}

void QGalleryAbstractRequestPrivate::_q_progressChanged(int current, int maximum)
{
    currentProgress = current;
    maximumProgress = maximum;

    emit q_func()->progressChanged(current, maximum);
}

/*!
    \class QGalleryAbstractRequest

    \ingroup gallery
    \ingroup gallery-requests

    \inmodule QtGallery

    \brief The QGalleryAbstractRequest class provides a base class for gallery
    requests.
*/

/*!
    \enum QGalleryAbstractRequest::Status

    Identifies the status of a gallery request.

    \value Inactive The request has not been executed, or has finished.
    \value Active The request is currently executing.
    \value Cancelling The request was cancelled, but hasn't yet returned to the
    Inactive status.
    \value Cancelled The request was cancelled.
    \value Idle The request has finished, and is monitoring its return values
    for changes.
    \value Finished The request is finished.
    \value Error The request runs into an error.
*/

/*!
    \enum QGalleryAbstractRequest::RequestError

    \value NoError No error.
    \value NoGallery No gallery found.
    \value NotSupported Request is not supported.
    \value GalleryError The gallery is wrong or corrupt.
*/

/*!
    \enum QGalleryAbstractRequest::RequestType

    Identifies the type of a request.

    \value QueryRequest The request is a QGalleryQueryRequest.
    \value ItemRequest The request is a QGalleryItemRequest.
    \value TypeRequest The request is a QGalleryTypeRequest
*/

/*!
    Constructs a new gallery \a type request.

    The \a parent is passed to QObject.
*/

QGalleryAbstractRequest::QGalleryAbstractRequest(RequestType type, QObject *parent)
    : QObject(parent)
    , d_ptr(new QGalleryAbstractRequestPrivate(0, type))
{
    d_ptr->q_ptr = this;
}

/*!
    Constructs a new \a type request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryAbstractRequest::QGalleryAbstractRequest(
        QAbstractGallery *gallery, RequestType type, QObject *parent)
    : QObject(parent)
    , d_ptr(new QGalleryAbstractRequestPrivate(gallery, type))
{
    d_ptr->q_ptr = this;
}

/*!
    \internal
*/

QGalleryAbstractRequest::QGalleryAbstractRequest(
        QGalleryAbstractRequestPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&(dd))
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a gallery request.
*/

QGalleryAbstractRequest::~QGalleryAbstractRequest()
{
}

/*!
    \property QGalleryAbstractRequest::gallery

    \brief The gallery service a request acts on.
*/

QAbstractGallery *QGalleryAbstractRequest::gallery() const
{
    return d_ptr->gallery.data();
}

void QGalleryAbstractRequest::setGallery(QAbstractGallery *gallery)
{
    if (d_ptr->gallery.data() != gallery) {
        d_ptr->gallery = gallery;

        emit supportedChanged();
    }
}

/*!
    \property QGalleryAbstractRequest::supported

    \brief Whether a request is supported by its current \a gallery.
*/

bool QGalleryAbstractRequest::isSupported() const
{
    return d_ptr->gallery && d_ptr->gallery.data()->isRequestSupported(d_ptr->type);
}

/*!
    \fn QGalleryAbstractRequest::supportedChanged()

    Signals the \l supported property has changed.
*/


/*!
    Returns the type of a request.
*/
QGalleryAbstractRequest::RequestType QGalleryAbstractRequest::type() const
{
    return d_ptr->type;
}

/*!
    \property QGalleryAbstractRequest::status

    \brief The status of a request.
*/

QGalleryAbstractRequest::Status QGalleryAbstractRequest::status() const
{
    return d_ptr->status;
}

/*!
    \property QGalleryAbstractRequest::error

    \brief The error encountered by an unsuccessful request.

    Common errors are defined in \l Error, more specific errors are defined by
    the gallery implementations such as in QDocumentGallery::Error.
*/

int QGalleryAbstractRequest::error() const
{
    return d_ptr->error;
}

/*!
    \property QGalleryAbstractRequest::errorString

    \brief A string describing the cause of an \l error in more detail.

    This may be an empty string if more information is not known.
*/

QString QGalleryAbstractRequest::errorString() const
{
    return d_ptr->errorString;
}

/*!
    \fn QGalleryAbstractRequest::errorChanged()

    Signals that the \l error and \l errorString properties have changed.
*/

/*!
    \property QGalleryAbstractRequest::currentProgress

    \brief the current progress of a request.
*/

int QGalleryAbstractRequest::currentProgress() const
{
    return d_func()->currentProgress;
}

/*!
    \property QGalleryAbstractRequest::maximumProgress

    \brief the maximum value of \l currentProgress
*/

int QGalleryAbstractRequest::maximumProgress() const
{
    return d_func()->maximumProgress;
}

/*!
    \fn QGalleryAbstractRequest::progressChanged(int current, int maximum)

    Signals that the \a current or \a maximum progress value has changed.
*/

/*!
    Waits for \a msecs for the a request to finish.

    Returns true if the request has finished on return, and returns false if the
    wait time expires or the request is inactive or idle.
*/
bool QGalleryAbstractRequest::waitForFinished(int msecs)
{
    return d_ptr->response && d_ptr->status == Active
            ? d_ptr->response->waitForFinished(msecs)
            : true;
}

/*!
    Executes a request.

    \bold {Note}: A valid \l gallery must be set before a request can be
    executed.
*/

void QGalleryAbstractRequest::execute()
{
    const int oldError = d_ptr->error;

    d_ptr->error = NoError;
    d_ptr->errorString = QString();

    if (!d_ptr->gallery) {
        d_ptr->status = Error;
        d_ptr->error = NoGallery;
        d_ptr->errorString = tr("No gallery has been set on the %1.", "%1 = class name")
                .arg(QString::fromLatin1(metaObject()->className()));

        if (d_ptr->response) {
            QScopedPointer<QGalleryAbstractResponse> oldResponse(d_ptr->response.take());

            Q_UNUSED(oldResponse);

            setResponse(0);
        }

        emit error(d_ptr->error, d_ptr->errorString);
        emit errorChanged();
    } else {
        QScopedPointer<QGalleryAbstractResponse> oldResponse(
                d_ptr->gallery.data()->createResponse(this));
        d_ptr->response.swap(oldResponse);

        if (d_ptr->response) {
            d_ptr->error = d_ptr->response->error();

            if (d_ptr->error != NoError) {
                d_ptr->errorString = d_ptr->response->errorString();
                d_ptr->status = Error;

                d_ptr->response.reset();

                if (oldResponse)
                    setResponse(0);

                emit error(d_ptr->error, d_ptr->errorString);
                emit errorChanged();
            } else {
                if (d_ptr->response->isActive()) {
                    d_ptr->status = Active;
                    d_ptr->wasIdle = false;
                } else if (d_ptr->response->isIdle()) {
                    d_ptr->status = Idle;
                    d_ptr->wasIdle = true;
                } else {
                    d_ptr->status = Finished;
                }

                connect(d_ptr->response.data(), SIGNAL(finished()), this, SLOT(_q_finished()));
                connect(d_ptr->response.data(), SIGNAL(resumed()), this, SLOT(_q_resumed()));
                connect(d_ptr->response.data(), SIGNAL(cancelled()), this, SLOT(_q_cancelled()));
                connect(d_ptr->response.data(), SIGNAL(progressChanged(int,int)),
                        this, SLOT(_q_progressChanged(int,int)));

                setResponse(d_ptr->response.data());
            }

            oldResponse.reset();
        } else {
            d_ptr->status = Error;
            d_ptr->error = NotSupported;
            d_ptr->errorString = tr(
                    "%1 is not supported by %2.", "%1 = interface, %2 = gallery implementation")
                    .arg(QString::fromLatin1(metaObject()->className()))
                    .arg(QString::fromLatin1(d_ptr->gallery.data()->metaObject()->className()));

            if (oldResponse)
                setResponse(0);

            oldResponse.reset();

            emit error(d_ptr->error, d_ptr->errorString);
            emit errorChanged();
        }

        if (d_ptr->currentProgress != 0 || d_ptr->maximumProgress != 0) {
            d_ptr->currentProgress = 0;
            d_ptr->maximumProgress = 0;

            emit progressChanged(0, 0);
        }

        if (d_ptr->status == Finished || d_ptr->status == Idle)
            emit finished();
    }

    if (oldError != NoError && d_ptr->error == NoError)
        emit errorChanged();

    emit statusChanged(d_ptr->status);
}

/*!
    Cancels the execution of a request.  If the request is idle this will stop
    any future updates.
*/

void QGalleryAbstractRequest::cancel()
{
    if (d_ptr->status == Active || d_ptr->status == Idle) {
        d_ptr->status = Cancelling;
        d_ptr->response->cancel();

        if (d_ptr->status == Cancelling)
            emit statusChanged(d_ptr->status);
    }
}

/*!
    Clears the results of a request.

    If the request is active or idle it will be cancelled.
*/

void QGalleryAbstractRequest::clear()
{
    const int oldError = d_ptr->error;

    d_ptr->error = NoError;
    d_ptr->errorString = QString();

    if (d_ptr->response) {
        QScopedPointer<QGalleryAbstractResponse> oldResponse(d_ptr->response.take());

        d_ptr->status = Inactive;

        setResponse(0);

        oldResponse.reset();

        if (d_ptr->currentProgress != 0 || d_ptr->maximumProgress != 0) {
            d_ptr->currentProgress = 0;
            d_ptr->maximumProgress = 0;

            emit progressChanged(0, 0);
        }

        emit statusChanged(d_ptr->status);
    } else if (d_ptr->status == Error) {
        d_ptr->status = Inactive;

        emit statusChanged(d_ptr->status);
    }

    if (oldError != NoError)
        emit errorChanged();
}

/*!
    \fn QGalleryAbstractRequest::finished()

    Signals that a request has finished.
*/

/*!
    \fn QGalleryAbstractRequest::cancelled()

    Signals that a request was cancelled before it could finish.
*/

/*!
    \fn QGalleryAbstractRequest::statusChanged(QGalleryAbstractRequest::Status status)

    Signals that the \a status of a request has changed.
*/

/*!
    \fn QGalleryAbstractRequest::error(int error, const QString &errorString)

    Signals that a request failed with the given \a error.  The \a errorString
    may provide more detail.
*/

/*!
    \fn QGalleryAbstractRequest::setResponse(QGalleryAbstractResponse *response)

    Sets the \a response to an executed request.
*/

#include "moc_qgalleryabstractrequest.cpp"

QTM_END_NAMESPACE
