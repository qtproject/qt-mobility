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
    return d_func()->status == QGalleryAbstractRequest::Active;
}

/*!
    Identifies if the items returned by a response are being monitored for
    changes.

    Returns true if a response is in an idle state, and false otherwise.
*/

bool QGalleryAbstractResponse::isIdle() const
{
    return d_func()->status == QGalleryAbstractRequest::Idle;
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
    \l QGalleryAbstractRequest::Cancelled result.  If the reponse is idle the
    \l finished() signal will be re-emitted with idle
*/

void QGalleryAbstractResponse::cancel()
{
    Q_D(QGalleryAbstractResponse);

    if (d->status == QGalleryAbstractRequest::Active
            || d->status == QGalleryAbstractRequest::Idle) {
        d->status = QGalleryAbstractRequest::Cancelled;

        emit cancelled();
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

    if (d->status == QGalleryAbstractRequest::Active
            || (d->status == QGalleryAbstractRequest::Idle && !idle)) {
        d->status = idle
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

    if (d->status == QGalleryAbstractRequest::Idle) {
        d->status = QGalleryAbstractRequest::Active;

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

    if (d->status == QGalleryAbstractRequest::Active
            || d->status == QGalleryAbstractRequest::Idle) {
        d->status = QGalleryAbstractRequest::Finished;

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
    \fn QGalleryAbstractResponse::cancelled()

    Signals that a response was cancelled.
*/

#include "moc_qgalleryabstractresponse.cpp"

QTM_END_NAMESPACE

