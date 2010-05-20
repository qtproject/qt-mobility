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

#include "qgeomappingreply.h"
#include "qgeomappingreply_p.h"

QTM_BEGIN_NAMESPACE
/*!
    \class QGeoMappingReply
    \brief The QGeoMappingReply class represents the response from a geocoding service.
    \ingroup location

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
    Constructs a QGeoMappingReply with parent \a parent.
*/
QGeoMappingReply::QGeoMappingReply(QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoMappingReplyPrivate())
{
}

QGeoMappingReply::QGeoMappingReply(Error error, const QString &errorString, QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoMappingReplyPrivate(error, errorString)) {}

/*!
    Destructor.
*/
QGeoMappingReply::~QGeoMappingReply()
{
    delete d_ptr;
}

void QGeoMappingReply::setFinished(bool finished)
{
    d_ptr->isFinished = finished;
    if (d_ptr->isFinished)
        emit this->finished();
}

bool QGeoMappingReply::isFinished() const
{
    return d_ptr->isFinished;
}

void QGeoMappingReply::setError(QGeoMappingReply::Error error, const QString &errorString)
{
    d_ptr->error = error;
    d_ptr->errorString = errorString;
    emit this->error(error, errorString);
    setFinished(true);
}

QGeoMappingReply::Error QGeoMappingReply::error() const
{
    return d_ptr->error;
}

QString QGeoMappingReply::errorString() const
{
    return d_ptr->errorString;
}


/*!
    Returns a list of places corresponding to the request.
*/
QPixmap QGeoMappingReply::mapImage() const
{
    return d_ptr->mapImage;
}


/*!
    Sets the list of places in the reply to \a places.
*/
void QGeoMappingReply::setMapImage(const QPixmap &image)
{
    d_ptr->mapImage = image;
}

/*!
    Cancels the receiving of this reply if the reply hasn't been received already.
*/
void QGeoMappingReply::abort() {}

/*!
    \fn void QGeoMappingReply::finished()

    Indicates that the reply has been received and processed without error, and is ready to be used.
*/
/*!
    \fn void QGeoMappingReply::error(QGeoMappingReply::Error error, const QString &errorString)

    Indicates that an error occurred during the receiving or processing of the reply.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMappingReplyPrivate::QGeoMappingReplyPrivate()
    : error(QGeoMappingReply::NoError),
    errorString(""),
    isFinished(false) {}

QGeoMappingReplyPrivate::QGeoMappingReplyPrivate(QGeoMappingReply::Error error, const QString &errorString)
    : error(error),
    errorString(errorString),
    isFinished(true) {}

QGeoMappingReplyPrivate::QGeoMappingReplyPrivate(const QGeoMappingReplyPrivate &other)
    : error(error),
    errorString(errorString),
    isFinished(isFinished),
    mapImage(other.mapImage) {}

QGeoMappingReplyPrivate::~QGeoMappingReplyPrivate() {}

QGeoMappingReplyPrivate& QGeoMappingReplyPrivate::operator= (const QGeoMappingReplyPrivate &other)
{
    error = other.error;
    errorString = other.errorString;
    isFinished = other.isFinished;
    mapImage = other.mapImage;

    return *this;
}

#include "moc_qgeomappingreply.cpp"

QTM_END_NAMESPACE
