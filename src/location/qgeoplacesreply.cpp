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

#include "qgeoplacesreply.h"
#include "qgeoplacesreply_p.h"

QTM_BEGIN_NAMESPACE
/*!
    \class QGeoPlacesReply
    \brief The QGeoPlacesReply class represents the response from a geocoding service.
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
    Constructs a QGeoPlacesReply with parent \a parent.
*/
QGeoPlacesReply::QGeoPlacesReply(QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoPlacesReplyPrivate()) {}

/*!
*/
/*QGeoPlacesReply::QGeoPlacesReply(const QGeoBoundingBox &bounds, QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoPlacesReplyPrivate())
{
    d_ptr->bounds = bounds;
}*/

/*!
*/
QGeoPlacesReply::QGeoPlacesReply(Error error, const QString &errorString, QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoPlacesReplyPrivate(error, errorString)) {}

/*!
    Destructor.
*/
QGeoPlacesReply::~QGeoPlacesReply()
{
    delete d_ptr;
}

/*!
*/
void QGeoPlacesReply::setFinished(bool finished)
{
    d_ptr->isFinished = finished;
    if (d_ptr->isFinished)
        emit this->finished();
}

/*!
*/
bool QGeoPlacesReply::isFinished() const
{
    return d_ptr->isFinished;
}

/*!
*/
void QGeoPlacesReply::setError(QGeoPlacesReply::Error error, const QString &errorString)
{
    d_ptr->error = error;
    d_ptr->errorString = errorString;
    emit this->error(error, errorString);
    setFinished(true);
}

/*!
*/
QGeoPlacesReply::Error QGeoPlacesReply::error() const
{
    return d_ptr->error;
}

/*!
*/
QString QGeoPlacesReply::errorString() const
{
    return d_ptr->errorString;
}

/*!
*/
void QGeoPlacesReply::setBounds(const QGeoBoundingBox &bounds)
{
    d_ptr->bounds = bounds;
}

/*!
*/
QGeoBoundingBox QGeoPlacesReply::bounds() const
{
    return d_ptr->bounds;
}

/*!
    Returns a list of places corresponding to the request.
*/
QList<QGeoPlace> QGeoPlacesReply::places() const
{
    return d_ptr->places;
}

/*!
    Adds \a place to the list of places in this reply.
*/
void QGeoPlacesReply::addPlace(const QGeoPlace &place)
{
    d_ptr->places.append(place);
}

/*!
    Sets the list of places in the reply to \a places.
*/
void QGeoPlacesReply::setPlaces(const QList<QGeoPlace> &places)
{
    d_ptr->places = places;
}

/*!
    Cancels the operation.

    This will do nothing if the reply is finished.
*/
void QGeoPlacesReply::abort() {}

/*!
    \fn void QGeoPlacesReply::finished()

    This signal is emitted when \a reply has finished processing.

    If error() equals QGeoPlacesReply::NoError then the processing
    finished successfully.

    This signal and QGeoPlacesManager::finished(QGeoRouteReply *reply) will be
    emitted at the same time.

    \note Do no delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/
/*!
    \fn void QGeoPlacesReply::error(QGeoPlacesReply::Error error, const QString &errorString)

    This signal is emitted when an error has been detected in the processing of
    \a reply. The finished() signal will probably follow.

    The error will be described by the error code \error. If \a errorString is
    not empty it will contain a textual description of the error.

    This signal and QGeoPlacesReply::error() will be emitted at the same time.

    \note Do no delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeoPlacesReplyPrivate::QGeoPlacesReplyPrivate()
    : error(QGeoPlacesReply::NoError),
    errorString(""),
    isFinished(false) {}

QGeoPlacesReplyPrivate::QGeoPlacesReplyPrivate(QGeoPlacesReply::Error error, const QString &errorString)
    : error(error),
    errorString(errorString),
    isFinished(true) {}

QGeoPlacesReplyPrivate::QGeoPlacesReplyPrivate(const QGeoPlacesReplyPrivate &other)
    : error(error),
    errorString(errorString),
    isFinished(isFinished),
    bounds(other.bounds),
    places(other.places) {}

QGeoPlacesReplyPrivate::~QGeoPlacesReplyPrivate() {}

QGeoPlacesReplyPrivate& QGeoPlacesReplyPrivate::operator= (const QGeoPlacesReplyPrivate &other)
{
    error = other.error;
    errorString = other.errorString;
    isFinished = other.isFinished;
    bounds = other.bounds;
    places = other.places;

    return *this;
}

#include "moc_qgeoplacesreply.cpp"

QTM_END_NAMESPACE
