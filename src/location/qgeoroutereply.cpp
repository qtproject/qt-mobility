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

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRouteReply
    \brief The QGeoRouteReply class represents the response from a routing service.
    \ingroup location

    This class represents the response from a routing service.
    It also takes responsibility for any errors that happen while the request is
    in submitted to and processed by the service.
*/

/*!
    \enum ErrorCode

    \value NoError
        No error has occurred.
    \value NetworkError
        A networking error occurred.
    \value NoContentError
        The reply contained no content.
    \value UnknownError
        An error occurred which does not fit into any of the other categories.
*/

/*!
    Constructs a route reply object based on \a request, with parent \a parent.
*/
QGeoRouteReply::QGeoRouteReply(const QGeoRouteRequest &request, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoRouteReplyPrivate(request))
{
}

/*!
    Destroys the route reply object.
*/
QGeoRouteReply::~QGeoRouteReply()
{
    delete d_ptr;
}

/*!
    Returns the route request which specified the route.
*/
QGeoRouteRequest QGeoRouteReply::request() const
{
    return d_ptr->request;
}

/*!
    Returns the list of routes which were requested.
*/
QList<QGeoRoute> QGeoRouteReply::routes() const
{
    return d_ptr->routes;
}

/*!
    Sets the list of routes in the reply to \a routes.
*/
void QGeoRouteReply::setRoutes(const QList<QGeoRoute> &routes)
{
    d_ptr->routes = routes;
}

/*!
    \fn void QGeoRouteReply::abort()

    Cancels the route calculation operation if the reply hasn't been received already.
*/

/*!
    \fn void QGeoRouteReply::finished()

    Indicates that the reply has been received and processed without error, and is ready to be used.
*/
/*!
    \fn void QGeoRouteReply::error(QGeoRouteReply::Error error, const QString &errorString)

    Indicates that an error occurred during the receiving or processing of the reply.
*/

/*******************************************************************************
*******************************************************************************/

QGeoRouteReplyPrivate::QGeoRouteReplyPrivate(const QGeoRouteRequest &request)
    : request(request) {}

QGeoRouteReplyPrivate::QGeoRouteReplyPrivate(const QGeoRouteReplyPrivate &other)
    : request(other.request),
    routes(other.routes) {}

QGeoRouteReplyPrivate::~QGeoRouteReplyPrivate() {}

QGeoRouteReplyPrivate& QGeoRouteReplyPrivate::operator= (const QGeoRouteReplyPrivate &other)
{
    request = other.request;
    routes = other.routes;

    return *this;
}

#include "moc_qgeoroutereply.cpp"

QTM_END_NAMESPACE
