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


#include "qroutereply.h"
#include "qroutereply_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QRouteReply
    \brief The QRouteReply class represents a response to a request for routing information.
    \ingroup location

    This class represents a routing response.
*/

/*!
    \enum QRouteReply::ResultCode

    Possible result codes as returned by the geo engine

    \value OK
        request succeeded
    \value Failed
        request failed
    \value FailedWithAlternative
        request failed, but a close alternative was found
*/

QRouteReply::QRouteReply()
    : d_ptr(new QRouteReplyPrivate())
{
}

/*!
    Returns the result code as reported by the geo engine.
*/
QRouteReply::ResultCode QRouteReply::resultCode() const
{
    Q_D(const QRouteReply);
    return d->rCode;
}
/*!
    Sets the result code
*/
void QRouteReply::setResultCode(QRouteReply::ResultCode result)
{
    Q_D(QRouteReply);
    d->rCode = result;
}

/*!
    Returns a textual description of the result.
*/
QString QRouteReply::resultDescription() const
{
    Q_D(const QRouteReply);
    return d->descr;
}

/*!
    Sets a textual description of the result.
*/
void QRouteReply::setResultDescription(QString description)
{
    Q_D(QRouteReply);
    d->descr = description;
}

/*!
    Returns the RFC 3066 language code of the reply.
*/
QString QRouteReply::language() const
{
    Q_D(const QRouteReply);
    return d->lang;
}

/*!
    Sets the RFC 3066 language code of the reply.
*/
void QRouteReply::setLanguage(QString language)
{
    Q_D(QRouteReply);
    d->lang = language;
}

/*!
    Returns the number of routes contained in this reply.
*/
int QRouteReply::count() const
{
    Q_D(const QRouteReply);
    return d->rt.size();
}

/*!
    Returns a list containing all found routes.

    \sa QRoute
*/
const QList<QRoute>& QRouteReply::routes() const
{
    Q_D(const QRouteReply);
    return d->rt;
}
/*!
    Adds a route to the list of found routes.
*/
void QRouteReply::addRoute(QRoute route)
{
    Q_D(QRouteReply);
    d->rt.append(route);
}

#include "moc_qroutereply.cpp"

QTM_END_NAMESPACE
