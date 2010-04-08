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

#include "qgeomaptilereply.h"
#include "qgeomaptilereply_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapTileReply
    \brief The QGeoMapTileReply class represents the response from a mapping service.
    \ingroup location

    This class represents the response from a mapping service.
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
    Constructs a QGeoMapTileReply from a QGeoMapTileRequest \a request, and with parent \a parent.
*/
QGeoMapTileReply::QGeoMapTileReply(const QGeoMapTileRequest &request, QObject *parent)
        : QObject(parent), d_ptr(new QGeoMapTileReplyPrivate())
{
    Q_D(QGeoMapTileReply);
    d->request = request;
}

/*!
    Destructor.
*/
QGeoMapTileReply::~QGeoMapTileReply()
{
    Q_D(QGeoMapTileReply);
    delete d;
}

/*!
    Returns the request which prompted this reply.
*/
QGeoMapTileRequest QGeoMapTileReply::request() const
{
    Q_D(const QGeoMapTileReply);
    return d->request;
}

/*!
    Returns the requested map data.
*/
QByteArray QGeoMapTileReply::data() const
{
    Q_D(const QGeoMapTileReply);
    return d->data;
}

/*!
    Sets the requested map data to \a data.
*/
void QGeoMapTileReply::setData(const QByteArray &data)
{
    Q_D(QGeoMapTileReply);
    d->data = data;
}

/*!
    Causes this QGeoMapTileReply to emit finished() if it has been configured properly
    or error() if there is a problem with the configuration.
*/
void QGeoMapTileReply::done()
{
    Q_D(QGeoMapTileReply);
    if (d->data.size() > 0) {
        emit finished();
    } else {
        emit error(QGeoMapTileReply::NoContentError, "The reply to the routing request was empty.");
    }
}

/*!
    \fn void QGeoMapTileReply::cancel()

    Cancels the receiving of this reply if the reply hasn't been received already.
*/

/*!
    \fn void QGeoMapTileReply::finished()

    Indicates that the reply has been received and processed without error, and is ready to be used.
*/
/*!
    \fn void QGeoMapTileReply::error(QGeoMapTileReply::ErrorCode errorCode, const QString &errorString = QString())

    Indicates that an error occurred during the receiving or processing of the reply.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapTileReplyPrivate::QGeoMapTileReplyPrivate() {}

#include "moc_qgeomaptilereply.cpp"

QTM_END_NAMESPACE
