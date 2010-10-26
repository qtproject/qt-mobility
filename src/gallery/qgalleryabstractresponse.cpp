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

#include "qgalleryabstractresponse_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGalleryAbstractResponse

    \ingroup gallery

    \inmodule QtGallery

    \brief The QGalleryAbstractResponse class provides a base class for
    responses to gallery requests.

    QGalleryAbstractResponse is the interface through which a QAbstractGallery
    returns a response to a QGalleryAbstractRequest.  The interface provides
    functions to communicate the state of a response to a request, additional
    classes derived from QGalleryAbstractRequest such as QGalleryResultSet are
    used to communicate actual results.

    New responses start in an active state indicating they have not yet
    finished.  When a response has finished; implementers should call the
    finish function which will put the response into an inactive state and
    emit the finished signal.  If the idle argument of finish is true then
    isIdle() will be true on return indicating that the response may return to
    an active state in order to refresh its results in the future.  The resume
    function will return the response to an active state and emit the resumed
    signal.

    If a response cannot finish successfully then implementers should call the
    error() function and supply an error code and description.  This will finish
    the request and set values for error() and errorString().

    If a request is cancelled it will call cancel() on its response.  The
    base implementation wsets isActive() and isIdle() to false and emits
    the canceled signal, so implementers will need to override the default
    implementation to actually cancel any ongoing task or suspend updates before
    calling the base implementation.  If the response can't be canceled
    synchronously then calling the base implementation should be deferred until
    the cancelation has completed.  Canceling a response should not discard any
    intermediate results already made available by the response.

    Unless a response finishes synchronously and calls finish() or error()
    from its constructor it must implement the waitForFinished() function
    to allow clients to block while waiting for the response to finish.
*/

/*!
    Constructs a new gallery response.

    The \a parent is passed to QObject.
*/

QGalleryAbstractResponse::QGalleryAbstractResponse(QObject *parent)
    : QObject(parent)
    , d_ptr(new QGalleryAbstractResponsePrivate)
{
    d_ptr->q_ptr = this;
}

/*!
    Constructs a new gallery response, for a request that has finished with an
    \a error.  The optional \a errorString will describe the error in greater
    detail.

    The \a parent is passed to QObject.
*/

QGalleryAbstractResponse::QGalleryAbstractResponse(
        int error, const QString &errorString, QObject *parent)
    : QObject(parent)
    , d_ptr(new QGalleryAbstractResponsePrivate)
{
    d_ptr->q_ptr = this;

    QGalleryAbstractResponse::error(error, errorString);
}

/*!
    \internal
*/

QGalleryAbstractResponse::QGalleryAbstractResponse(
        QGalleryAbstractResponsePrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a gallery response.
*/

QGalleryAbstractResponse::~QGalleryAbstractResponse()
{
}

/*!
    Identifies if a response is an an active state.

    Returns true if a response is active, and false otherwise.
*/

bool QGalleryAbstractResponse::isActive() const
{
    return d_func()->state == QGalleryAbstractRequest::Active;
}

/*!
    Identifies if the items returned by a response are being monitored for
    changes.

    Returns true if a response is in an idle state, and false otherwise.
*/

bool QGalleryAbstractResponse::isIdle() const
{
    return d_func()->state == QGalleryAbstractRequest::Idle;
}

/*!
    Returns an identifier describing an error condition encountered by a
    response.

    In the case of no error this will return QGalleryAbstractRequest::NoError.

    \sa QGalleryAbstractRequest::Error, QDocumentGallery::Error
*/

int QGalleryAbstractResponse::error() const
{
    return d_func()->error;
}

/*!
    Returns a string describing the cause of an \l error() in more detail.
*/

QString QGalleryAbstractResponse::errorString() const
{
    return d_func()->errorString;
}

/*!
    Waits for \a msecs for the a response to finish.

    Returns true if the response has finished on return, and returns false if
    the wait time expires or the request is inactive or idle.
*/

bool QGalleryAbstractResponse::waitForFinished(int msecs)
{
    Q_UNUSED(msecs);

    return true;
}

/*!
    Cancels an active or idle gallery response.

    The default implementation finishes the an active response with the
    \l QGalleryAbstractRequest::Canceled result.  If the reponse is idle the
    \l finished() signal will be re-emitted with idle
*/

void QGalleryAbstractResponse::cancel()
{
    Q_D(QGalleryAbstractResponse);

    if (d->state == QGalleryAbstractRequest::Active
            || d->state == QGalleryAbstractRequest::Idle) {
        d->state = QGalleryAbstractRequest::Canceled;

        emit canceled();
    }
}

/*!
    \fn QGalleryAbstractResponse::progressChanged(int current, int maximum)

    Signals that the \a current or \a maximum progress of a request has
    changed.
*/

/*!
    Finalizes a gallery response.

    If \a idle is true the items returned by a response will be monitored
    for changes and updated as appropriate.
*/

void QGalleryAbstractResponse::finish(bool idle)
{
    Q_D(QGalleryAbstractResponse);

    if (d->state == QGalleryAbstractRequest::Active
            || (d->state == QGalleryAbstractRequest::Idle && !idle)) {
        d->state = idle
                ? QGalleryAbstractRequest::Idle
                : QGalleryAbstractRequest::Finished;

        emit finished();
    }
}

/*!
    Returns a response to an active state.

    An idle response can call this to indicate it has begun refreshing its
    contents.
*/

void QGalleryAbstractResponse::resume()
{
    Q_D(QGalleryAbstractResponse);

    if (d->state == QGalleryAbstractRequest::Idle) {
        d->state = QGalleryAbstractRequest::Active;

        emit resumed();
    }
}

/*!
    Finalizes a response in response to an error condition.

    The \a error, and \a errorString are communicated to issuing request.
*/

void QGalleryAbstractResponse::error(int error, const QString &errorString)
{
    Q_D(QGalleryAbstractResponse);

    if (d->state == QGalleryAbstractRequest::Active
            || d->state == QGalleryAbstractRequest::Idle) {
        d->state = QGalleryAbstractRequest::Finished;

        d->error = error;
        d->errorString = errorString;

        emit finished();
    }
}

/*!
    \fn QGalleryAbstractResponse::finished()

    Signals that a response has finished.
*/

/*!
    \fn QGalleryAbstractResponse::resumed()

    Signals that an idle response has resumed communications.
*/

/*!
    \fn QGalleryAbstractResponse::canceled()

    Signals that a response was canceled.
*/

#include "moc_qgalleryabstractresponse.cpp"

QTM_END_NAMESPACE

