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

    \brief The QGalleryAbstractResponse class provides a base class for
    responses to gallery requests.
*/

/*!
    Constructs a new gallery response.

    The \a parent is passed to QObject.
*/

QGalleryAbstractResponse::QGalleryAbstractResponse(QObject *parent)
    : QGalleryItemList(*new QGalleryAbstractResponsePrivate, parent)
{
}

/*!
    \internal
*/

QGalleryAbstractResponse::QGalleryAbstractResponse(
        QGalleryAbstractResponsePrivate &dd, QObject *parent)
    : QGalleryItemList(dd, parent)
{
}

/*!
    Destroys a gallery response.
*/

QGalleryAbstractResponse::~QGalleryAbstractResponse()
{
}

/*!
    Identifies if the items returned by a response are being monitored for
    changes.

    Returns true if a response is in an idle state, and false otherwise.
*/
bool QGalleryAbstractResponse::isIdle() const
{
    return d_func()->idle;
}

/*!
    Returns the result of a gallery request.
*/

int QGalleryAbstractResponse::result() const
{
    return d_func()->result;
}

/*!
    \fn QGalleryAbstractResponse::waitForFinished(int msecs)

    Waits for \a msecs for the a response to finish.

    Returns true if the response has finished on return, and returns false if
    the wait time expires or the request is inactive or idle.
*/

/*!
    Cancels an active or idle gallery response.

    The default implementation finishes the an active response with the
    \l QGalleryAbstractRequest::Cancelled result.  If the reponse is idle the
    \l finished() signal will be re-emitted with idle
*/

void QGalleryAbstractResponse::cancel()
{
    Q_D(QGalleryAbstractResponse);

    if (d->result == QGalleryAbstractRequest::NoResult) {
        d->result = QGalleryAbstractRequest::Cancelled;
        d->idle = false;

        emit finished();
    } else if (d->idle) {
        d->idle = false;

        emit finished();
    }
}

/*!
    \fn QGalleryAbstractResponse::progressChanged(int current, int maximum)

    Signals that the \a current or \a maximum progress of a request has
    changed.
*/

/*!
    Finalizes a gallery response, and sets the \a result.

    If \a idle is true the items returned by a response will be monitored
    for changes and updated as appropriate.
*/

void QGalleryAbstractResponse::finish(int result, bool idle)
{
    Q_D(QGalleryAbstractResponse);

    if (d->result == QGalleryAbstractRequest::NoResult
            && result != QGalleryAbstractRequest::NoResult) {
        d->result = result;
        d->idle = idle;

        emit finished();
    }
}

/*!
    \fn QGalleryAbstractResponse::finished()

    Signals that a response has finished.
*/

#include "moc_qgalleryabstractresponse.cpp"

QTM_END_NAMESPACE

