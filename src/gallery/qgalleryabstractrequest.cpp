/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgalleryabstractrequest_p.h"

#include "qabstractgallery.h"

QTM_BEGIN_NAMESPACE

void QGalleryAbstractRequestPrivate::_q_finished()
{
    if (state == QGalleryAbstractRequest::Active
            || state == QGalleryAbstractRequest::Canceling
            || state == QGalleryAbstractRequest::Idle) {
        if (response->error() != QGalleryAbstractRequest::NoError) {
            error = response->error();
            errorString = response->errorString();

            state = QGalleryAbstractRequest::Error;

            emit q_func()->error(error, errorString);
            emit q_func()->errorChanged();
            emit q_func()->stateChanged(state);
        } else if (state == QGalleryAbstractRequest::Idle) {
            if (!response->isIdle()) {
                state = QGalleryAbstractRequest::Finished;

                emit q_func()->stateChanged(state);
            }
        } else if (response->isIdle()) {
            state = QGalleryAbstractRequest::Idle;

            if (!wasIdle) {
                wasIdle = true;

                emit q_func()->finished();
            }

            emit q_func()->stateChanged(state);
        } else if (!response->isActive()) {
            state = QGalleryAbstractRequest::Finished;

            if (!wasIdle)
                emit q_func()->finished();

            emit q_func()->stateChanged(state);
        }
    }
}

void QGalleryAbstractRequestPrivate::_q_canceled()
{
    if (state == QGalleryAbstractRequest::Canceling) {
        if (!wasIdle) {
            state = QGalleryAbstractRequest::Canceled;

            emit q_func()->canceled();
        } else {
            state = QGalleryAbstractRequest::Finished;
        }
        emit q_func()->stateChanged(state);
    }
}

void QGalleryAbstractRequestPrivate::_q_resumed()
{
    if (state == QGalleryAbstractRequest::Idle && !response->isIdle()) {
        state = QGalleryAbstractRequest::Active;

        emit q_func()->stateChanged(state);
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

    Gallery requests are the interface through which clients can interact with
    a gallery to execute queries or perform actions.  QAbstractGalleryRequest
    manages executing requests against a gallery and reporting the state of
    responses.

    In order to execute a request it is first necessary to assign the \l gallery
    to the request.  If the request is supported by the gallery the \l supported
    property will be true, otherwise if the request is not supported or no
    gallery is assigned to the request \l supported will be false.

    Once a request has been assigned a gallery and its own arguments it can be
    \l {execute()}{executed}.  As requests are asynchronous results will not
    be available immediately, instead the request will go into an \l Active
    \l state until it is finished.  If the request is successful the finished()
    signal will be emitted and if not the error() signal will be emitted, in
    both cases the stateChanged() signal will be emitted with the new state of
    the request.

    Active requests can be canceled by calling the cancel() function.
    Canceling a request can also be an asynchronous action and does not always
    prevent a request from finishing successfully such as when the earliest
    possible point to interrupt a request is on completion.  If a canceled
    request isn't aborted immediately it will go into the \l Canceling state
    until it is aborted and enters the \l Canceled state or succeeds and enters
    the \l Finished state.

    Some requests support automatically updating their results in response to
    changes in their source data or some other event.  A request that will
    provide these sort of updates will enter the Idle state upon finishing
    its initial request.  From the \l Idle state a request may return to the
    \l Active state itself to refresh its results or update it results
    immeditately if a a new query is not required.  Cancelling an idle request
    will put it into the \l Finished state and prevent future updates.

    While requests are asynchronous it is possible to use them in a synchronous
    manner by using the waitForFinished() function to block until the request
    has finished.
*/

/*!
    \enum QGalleryAbstractRequest::State

    Identifies the state of a gallery request.

    \value Inactive The request has not been executed.
    \value Active The request is currently executing.
    \value Canceling The request was canceled, but hasn't yet reached the
    Canceled state.
    \value Canceled The request was canceled.
    \value Idle The request has finished, but may return to the active state
    in response to an internal event.
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

        emit galleryChanged();
        emit supportedChanged();
    }
}

/*!
    \fn QGalleryAbstractRequest::galleryChanged()

    Signals that the value of \l gallery has changed.
*/

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
    \property QGalleryAbstractRequest::state

    \brief The state of a request.
*/

QGalleryAbstractRequest::State QGalleryAbstractRequest::state() const
{
    return d_ptr->state;
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
    return d_ptr->response && d_ptr->state == Active
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
        d_ptr->state = Error;
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
                d_ptr->state = Error;

                d_ptr->response.reset();

                if (oldResponse)
                    setResponse(0);

                emit error(d_ptr->error, d_ptr->errorString);
                emit errorChanged();
            } else {
                if (d_ptr->response->isActive()) {
                    d_ptr->state = Active;
                    d_ptr->wasIdle = false;
                } else if (d_ptr->response->isIdle()) {
                    d_ptr->state = Idle;
                    d_ptr->wasIdle = true;
                } else {
                    d_ptr->state = Finished;
                }

                connect(d_ptr->response.data(), SIGNAL(finished()), this, SLOT(_q_finished()));
                connect(d_ptr->response.data(), SIGNAL(resumed()), this, SLOT(_q_resumed()));
                connect(d_ptr->response.data(), SIGNAL(canceled()), this, SLOT(_q_canceled()));
                connect(d_ptr->response.data(), SIGNAL(progressChanged(int,int)),
                        this, SLOT(_q_progressChanged(int,int)));

                setResponse(d_ptr->response.data());
            }

            oldResponse.reset();
        } else {
            d_ptr->state = Error;
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

        if (d_ptr->state == Finished || d_ptr->state == Idle)
            emit finished();
    }

    if (oldError != NoError && d_ptr->error == NoError)
        emit errorChanged();

    emit stateChanged(d_ptr->state);
}

/*!
    Cancels the execution of a request.  If the request is idle this will stop
    any future updates.
*/

void QGalleryAbstractRequest::cancel()
{
    if (d_ptr->state == Active || d_ptr->state == Idle) {
        d_ptr->state = Canceling;
        d_ptr->response->cancel();

        if (d_ptr->state == Canceling)
            emit stateChanged(d_ptr->state);
    }
}

/*!
    Clears the results of a request.

    If the request is active or idle it will be canceled.
*/

void QGalleryAbstractRequest::clear()
{
    const int oldError = d_ptr->error;

    d_ptr->error = NoError;
    d_ptr->errorString = QString();

    if (d_ptr->response) {
        QScopedPointer<QGalleryAbstractResponse> oldResponse(d_ptr->response.take());

        d_ptr->state = Inactive;

        setResponse(0);

        oldResponse.reset();

        if (d_ptr->currentProgress != 0 || d_ptr->maximumProgress != 0) {
            d_ptr->currentProgress = 0;
            d_ptr->maximumProgress = 0;

            emit progressChanged(0, 0);
        }

        emit stateChanged(d_ptr->state);
    } else if (d_ptr->state == Error) {
        d_ptr->state = Inactive;

        emit stateChanged(d_ptr->state);
    }

    if (oldError != NoError)
        emit errorChanged();
}

/*!
    \fn QGalleryAbstractRequest::finished()

    Signals that a request has finished.
*/

/*!
    \fn QGalleryAbstractRequest::canceled()

    Signals that a request was canceled before it could finish.
*/

/*!
    \fn QGalleryAbstractRequest::stateChanged(QGalleryAbstractRequest::State state)

    Signals that the \a state of a request has changed.
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
