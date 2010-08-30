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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotiledmapreply.h"
#include "qgeotiledmapreply_p.h"

#include "qgeotiledmaprequest.h"

#include <qdebug.h>

QTM_BEGIN_NAMESPACE
/*!
    \class QGeoTiledMapReply
    \brief The QGeoTiledMapReply class represents the response from a geocoding service.

    This class represents the response from a geocoding service.
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
    Constructs a QGeoTiledMapReply with parent \a parent.
*/
QGeoTiledMapReply::QGeoTiledMapReply(const QGeoTiledMapRequest &request, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoTiledMapReplyPrivate(request))
{
}

QGeoTiledMapReply::QGeoTiledMapReply(Error error, const QString &errorString, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoTiledMapReplyPrivate(error, errorString)) {}

/*!
    Destructor.
*/
QGeoTiledMapReply::~QGeoTiledMapReply()
{
    delete d_ptr;
}

void QGeoTiledMapReply::setFinished(bool finished)
{
    d_ptr->isFinished = finished;
    if (d_ptr->isFinished)
        emit this->finished();
}

bool QGeoTiledMapReply::isFinished() const
{
    return d_ptr->isFinished;
}

void QGeoTiledMapReply::setError(QGeoTiledMapReply::Error error, const QString &errorString)
{
    d_ptr->error = error;
    d_ptr->errorString = errorString;
    emit this->error(error, errorString);
    setFinished(true);
}

QGeoTiledMapReply::Error QGeoTiledMapReply::error() const
{
    return d_ptr->error;
}

QString QGeoTiledMapReply::errorString() const
{
    return d_ptr->errorString;
}

bool QGeoTiledMapReply::isCached() const
{
    return d_ptr->isCached;
}

void QGeoTiledMapReply::setCached(bool cached)
{
    d_ptr->isCached = cached;
}

QGeoTiledMapRequest QGeoTiledMapReply::request() const
{
    return d_ptr->request;
}

QByteArray QGeoTiledMapReply::mapImageData() const
{
    return d_ptr->mapImageData;
}

void QGeoTiledMapReply::setMapImageData(const QByteArray &data)
{
    d_ptr->mapImageData = data;
}

QString QGeoTiledMapReply::mapImageFormat() const
{
    return d_ptr->mapImageFormat;
}

void QGeoTiledMapReply::setMapImageFormat(const QString &format)
{
    d_ptr->mapImageFormat = format;
}

void QGeoTiledMapReply::abort() {}

/*!
    \fn void QGeoTiledMapReply::finished()

    Indicates that the reply has been received and processed without error, and is ready to be used.
*/
/*!
    \fn void QGeoTiledMapReply::error(QGeoTiledMapReply::Error error, const QString &errorString)

    Indicates that an error occurred during the receiving or processing of the reply.
*/

/*******************************************************************************
*******************************************************************************/

QGeoTiledMapReplyPrivate::QGeoTiledMapReplyPrivate(const QGeoTiledMapRequest &request)
        : error(QGeoTiledMapReply::NoError),
        errorString(""),
        isFinished(false),
        isCached(false),
        request(request) {}

QGeoTiledMapReplyPrivate::QGeoTiledMapReplyPrivate(QGeoTiledMapReply::Error error, const QString &errorString)
        : error(error),
        errorString(errorString),
        isFinished(true),
        isCached(false) {}

QGeoTiledMapReplyPrivate::~QGeoTiledMapReplyPrivate() {}

#include "moc_qgeotiledmapreply.cpp"

QTM_END_NAMESPACE
