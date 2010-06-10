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
    Q_Q(QGalleryAbstractRequest);

    if (result == QGalleryAbstractRequest::NoResult) {
        result = response->result();

        if (result == QGalleryAbstractRequest::NoResult)
            return;

        state = response->isIdle()
                ? QGalleryAbstractRequest::Idle
                : QGalleryAbstractRequest::Inactive;

        switch (result) {
        case QGalleryAbstractRequest::Succeeded:
            emit q->succeeded();
            break;
        case QGalleryAbstractRequest::Cancelled:
            emit q->cancelled();
            break;
        default:
            emit q->failed(result);
            break;
        }
        emit q->finished(result);
        emit q->resultChanged();
        emit q->stateChanged(state);
    } else if (state == QGalleryAbstractRequest::Idle && !response->isIdle()) {
        state = QGalleryAbstractRequest::Inactive;

        emit q->stateChanged(state);
    }
}

void QGalleryAbstractRequestPrivate::_q_galleryDestroyed()
{
    Q_Q(QGalleryAbstractRequest);

    gallery = 0;

    q->clear();

    emit q->supportedChanged();
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

    \brief The QGalleryAbstractRequest class provides a base class for gallery
    requests.
*/

/*!
    \enum QGalleryAbstractRequest::State

    Identifies the state of a gallery request.

    \value Inactive The request has not been executed, or has finished.
    \value Active The request is currently executing.
    \value Cancelling The request was cancelled, but hasn't yet returned to the
    Inactive state.
    \value Idle The request has finished, and is monitoring its return values
    for changes.
*/

/*!
    \enum QGalleryAbstractRequest::Result

    Identifies the result of a gallery request.

    \value NoResult The request is still active and no result is available yet.
    \value Succeeded The request succeeded sucessfully.
    \value Cancelled The request was cancelled before it could finish.
    \value NoGallery The request cannot be executed because no \l gallery is
    set.
    \value NotSupported The request isn't supported by the \l gallery.
    \value ConnectionError The request could not be executed due to a problem
    connecting to the gallery.
    \value InvalidItemError The request could not be executed due to an invalid
    item ID.
    \value ItemTypeError The request could not be executed due to an invalid
    item type.
    \value InvalidPropertyError The request could not be executed because a
    filter referenced an invalid property.
    \value PropertyTypeError The request could not be executed because a filter
    compared a property to an invalid variant type.
    \value UnsupportedFilterTypeError The request could not be executed because
    a filter is not supported by the gallery.
    \value UnsupportedFilterOptionError The request could not be executed
    because an option on a filter is not supported by the gallery.
    \value PermissionsError The request could not be executed because the
    user has insufficient permissions.
    \value InvalidDestinationError  The request could not be executed because
    the given destination ID is invalid.
    \value InvalidUrlError The request could not be executed due to an invalid
    URL.
    \value RequestError A minimum value for request specific error results.
*/

/*!
    \enum QGalleryAbstractRequest::Type

    Identifies the type of a request.

    \value Item The request is a QGalleryItemRequest.
    \value Url The request is a QGalleryUrlRequest.
    \value Query The request is a QGalleryQueryRequest.
    \value Count The request is a QGalleryCountRequest.
    \value Remove The request is a QGalleryRemoveRequest.
*/

/*!
    Constructs a new gallery \a type request.

    The \a parent is passed to QObject.
*/

QGalleryAbstractRequest::QGalleryAbstractRequest(Type type, QObject *parent)
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
        QAbstractGallery *gallery, Type type, QObject *parent)
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
    return d_ptr->gallery;
}

void QGalleryAbstractRequest::setGallery(QAbstractGallery *gallery)
{
    if (d_ptr->gallery != gallery) {
        if (d_ptr->gallery)
            disconnect(d_ptr->gallery, SIGNAL(destroyed()), this, SLOT(_q_galleryDestroyed()));

        d_ptr->gallery = gallery;

        if (d_ptr->gallery)
            connect(d_ptr->gallery, SIGNAL(destroyed()), this, SLOT(_q_galleryDestroyed()));

        clear();

        emit supportedChanged();
    }
}

/*!
    \property QGalleryAbstractRequest::supported

    \brief Whether a request is supported by its current \a gallery.
*/

bool QGalleryAbstractRequest::isSupported() const
{
    return d_ptr->gallery && d_ptr->gallery->isRequestSupported(d_ptr->type);
}

/*!
    \fn QGalleryAbstractRequest::supportedChanged()

    Signals the \l supported property has changed.
*/


/*!
    Returns the type of a request.
*/
QGalleryAbstractRequest::Type QGalleryAbstractRequest::type() const
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
    \fn QGalleryAbstractRequest::stateChanged(QGalleryAbstractRequest::State state)

    Signals that the \a state of a request has changed.
*/

/*!
    \property QGalleryAbstractRequest::result

    \brief The result of a request.

    This will be a value of \l Result, or a request specific error value.
*/

int QGalleryAbstractRequest::result() const
{
    return d_ptr->result;
}

/*!
    \fn QGalleryAbstractRequest::resultChanged()

    Signals that the result of a request has changed.
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
    if (!d_ptr->gallery) {
        d_ptr->result = NoGallery;

        emit failed(d_ptr->result);
        emit finished(d_ptr->result);
        emit resultChanged();
    } else {
        QGalleryAbstractResponse *oldResponse = d_ptr->response;
        int oldResult = d_ptr->result;
        State oldState = d_ptr->state;

        d_ptr->response = d_ptr->gallery->createResponse(this);

        if (d_ptr->response) {
            d_ptr->result = d_ptr->response->result();

            if (d_ptr->result > Succeeded) {
                d_ptr->state = Inactive;

                delete d_ptr->response;
                d_ptr->response = 0;

                if (oldResponse)
                    setResponse(0);

            } else {
                if (d_ptr->result == NoResult)
                    d_ptr->state = Active;
                else if (d_ptr->response->isIdle())
                    d_ptr->state = Idle;
                else
                    d_ptr->state = Inactive;

                connect(d_ptr->response, SIGNAL(finished()), this, SLOT(_q_finished()));
                connect(d_ptr->response, SIGNAL(progressChanged(int,int)),
                        this, SLOT(_q_progressChanged(int,int)));

                setResponse(d_ptr->response);
            }
        } else {
            d_ptr->result = NotSupported;
            d_ptr->state = Inactive;

            if (oldResponse)
                setResponse(0);
        }

        delete oldResponse;

        if (d_ptr->currentProgress != 0 || d_ptr->maximumProgress != 0) {
            d_ptr->currentProgress = 0;
            d_ptr->maximumProgress = 0;

            emit progressChanged(0, 0);
        }

        switch (d_ptr->result) {
        case QGalleryAbstractRequest::NoResult:
            break;
        case QGalleryAbstractRequest::Cancelled:
            break;
        case QGalleryAbstractRequest::Succeeded:
            emit succeeded();
            emit finished(d_ptr->result);
            break;
        default:
            emit failed(d_ptr->result);
            emit finished(d_ptr->result);
            break;
        }

        if (d_ptr->result != oldResult)
            emit resultChanged();

        if (d_ptr->state != oldState)
            emit stateChanged(d_ptr->state);
    }
}

/*!
    Cancels the execution of a request.  If the request is idle this will stop
    any future updates.
*/

void QGalleryAbstractRequest::cancel()
{
    switch (d_ptr->state) {
    case Active:
        d_ptr->state = Cancelling;
        d_ptr->response->cancel();

        if (d_ptr->state == Cancelling)
            emit stateChanged(d_ptr->state);
        break;
    case Idle:
        d_ptr->response->cancel();
        break;
    default:
        break;
    }
}

/*!
    Clears the results of a request.

    If the request is active or idle it will be cancelled.
*/

void QGalleryAbstractRequest::clear()
{
    if (d_ptr->response) {
        bool wasFinished = d_ptr->result != NoResult;
        bool wasActive = d_ptr->state != Inactive;

        QGalleryAbstractResponse *oldResponse = d_ptr->response;

        d_ptr->response = 0;
        d_ptr->result = NoResult;
        d_ptr->state = Inactive;

        setResponse(0);

        delete oldResponse;

        if (d_ptr->currentProgress != 0 || d_ptr->maximumProgress != 0) {
            d_ptr->currentProgress = 0;
            d_ptr->maximumProgress = 0;

            emit progressChanged(0, 0);
        }

        if (wasFinished && d_ptr->result == NoResult)
            emit resultChanged();

        if (wasActive && d_ptr->state == Inactive)
            emit stateChanged(d_ptr->state);
    }
}

/*!
    \fn QGalleryAbstractRequest::succeeded()

    Signals that a request has completed successfully.
*/

/*!
    \fn QGalleryAbstractRequest::cancelled()

    Signals that a request was cancelled before it could finish.
*/

/*!
    \fn QGalleryAbstractRequest::failed(int result)

    Signals that a request failed with the given \a result.
*/

/*!
    \fn QGalleryAbstractRequest::finished(int result)

    Signals that a request has finished with the given \a result.
*/

/*!
    \fn QGalleryAbstractRequest::setResponse(QGalleryAbstractResponse *response)

    Sets the \a response to an executed request.
*/

#include "moc_qgalleryabstractrequest.cpp"

QTM_END_NAMESPACE
