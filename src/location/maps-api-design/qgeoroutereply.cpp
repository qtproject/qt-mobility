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

#include "qgeoroutereply.h"
#include "qgeoroutereply_p.h"

#include "qgeoroute.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRouteReply
    \brief The QGeoRouteReply class represents the result of a request for
    routing information.
    \ingroup maps

    The QGeoRouteReply class is responsible for signalling whether a
    routing request completed, whether the request was completed successfully,
    and for delivering the result of the request.

    Unless the cancel() method is used, a QGeoRouteReply object will emit the
    finished() or error() signals to indicate that the request has completed.

    After the finished() signal has been emitted the results can be retrieved
    with routes().

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoRouteReply object at an appropriate time. Do not
    directly delete it inside the slot connected to finished() or error() -
    the deleteLater() function should be used instead.

    \sa QGeoRoutingService
*/

/*!
    Constructs a QGeoRouteReply object with parent \a parent.
*/
QGeoRouteReply::QGeoRouteReply(QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoRouteReplyPrivate()) {}

/*!
    Destroys this QGeoRouteReply object.
*/
QGeoRouteReply::~QGeoRouteReply()
{
    Q_D(QGeoRouteReply);
    delete d;
}

/*!
    Sets the routes in the reply to \a routes.

    \sa QGeoRouteReply::routes()
*/
void QGeoRouteReply::setRoutes(const QList<QGeoRoute> &routes)
{
    Q_D(QGeoRouteReply);
    d->routes = routes;
}

/*!
    Returns the requested routes.

    The result of calling this function prior to receiving the finished()
    signal is undefined.

    \sa QGeoRouteReply::setRoutes()
*/
QList<QGeoRoute> QGeoRouteReply::routes() const
{
    Q_D(const QGeoRouteReply);
    return d->routes;
}

/*!
    \fn void QGeoRouteReply::cancel()

    Cancels this QGeoRouteReply if it has not yet completed.

    The finished() signal will not be emitted.

    This function has no effect if the routing request has completed and this
    QGeoRouteReply object has emitted the finished() or error() signals.

    The user is still responsible for deleting this QGeoRouteReply object.
*/

 /*!
    \fn void QGeoRouteReply::finished()

    A QGeoRouteReply object represents the outcome of a request against a
    QGeoMappingService instance.  If this signal is emitted it indicates that
    the requested service was completed successfully.

    Note that the QGeoRoutingService::replyFinished() signal can be used instead
    of this signal if it is more convinient to do so.

    Do not delete this QGeoRouteReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoRoutingService::replyFinished()
*/

/*!
    \fn void QGeoRouteReply::error(QGeoRoutingService::ErrorCode errorCode,
                                   QString errorString)

    A QGeoRouteReply object represents the outcome of a request against a
    QGeoRoutingService instance.  If this signal is emitted it indicates that
    the requested service did not finish successfully.  The error that
    prevented the fulfullment of the request is described by \a errorCode
    and \a errorString.

    Note that the QGeoRoutingService::replyError() signal can be used instead of
    this signal if it is more convinient to do so.

    Do not delete this QGeoRouteReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoRoutingService::replyError()
*/

/*******************************************************************************
*******************************************************************************/

QGeoRouteReplyPrivate::QGeoRouteReplyPrivate() {}

QGeoRouteReplyPrivate::QGeoRouteReplyPrivate(const QGeoRouteReplyPrivate &other)
    : route(other.routes) {}

QGeoRouteReplyPrivate::~QGeoRouteReplyPrivate() {}

QGeoRouteReplyPrivate& QGeoRouteReplyPrivate::operator= (const QGeoRouteReplyPrivate &other)
{
    routes = other.routes;

    return *this;
}

#include "moc_qgeoroutereply.cpp"

QTM_END_NAMESPACE
