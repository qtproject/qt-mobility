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

    \brief The QGeoPlacesReply class manages an operation started by an
    instance of QGeoPlacesManager.

    \ingroup maps

    Instances of QGeoPlacesReply manage the state and results of these
    operations.

    The isFinished(), error() and errorString() methods provide information
    on whether the operation has completed and if it completed succesfully.

    If the operation completes successfully the results will be able to be
    accessed with places().
*/

/*!
    \enum QGeoPlacesReply::Error

    Describes an error which prevented the completion of the operation.

    \value NoError
        No error has occurred.
    \value EngineNotSetError
        The places manager that was used did not have a QGeoPlacesManagerEngine instance associated with it.
    \value CommunicationError
        An error occurred while communicating with the service provider.
    \value ParseError
        The response from the service provider was in an unrecognizable format.
    \value UnsupportedOptionError
        The requested operation or one of the options for the operation are not
        supported by the service provider.
    \value UnknownError
        An error occurred which does not fit into any of the other categories.
*/

/*!
    Constructs a places reply with the specified \a parent.
*/
QGeoPlacesReply::QGeoPlacesReply(QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoPlacesReplyPrivate()) {}

/*!
    Constructs a places reply with a given \a error and \a errorString and the specified \a parent.
*/
QGeoPlacesReply::QGeoPlacesReply(Error error, const QString &errorString, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoPlacesReplyPrivate(error, errorString)) {}

/*!
    Destroys this places reply object.
*/
QGeoPlacesReply::~QGeoPlacesReply()
{
    delete d_ptr;
}

/*!
    Sets whether or not this reply has finished to \a finished.

    If \a finished is true, this will cause the finished() signal to be
    emitted.

    If the operation completed successfully, QGeoPlacesReply::setPlaces()
    should be called before this function. If an error occured,
    QGeoPlacesReply::setError() should be used instead.
*/
void QGeoPlacesReply::setFinished(bool finished)
{
    d_ptr->isFinished = finished;
    if (d_ptr->isFinished)
        emit this->finished();
}

/*!
    Return true if the operation completed successfully or encountered an
    error which cause the operation to come to a halt.
*/
bool QGeoPlacesReply::isFinished() const
{
    return d_ptr->isFinished;
}

/*!
    Sets the error state of this reply to \a error and the textual
    representation of the error to \a errorString.

    This wil also cause error() and finished() signals to be emitted, in that
    order.
*/
void QGeoPlacesReply::setError(QGeoPlacesReply::Error error, const QString &errorString)
{
    d_ptr->error = error;
    d_ptr->errorString = errorString;
    emit this->error(error, errorString);
    setFinished(true);
}

/*!
    Returns the error state of this reply.

    If the result is QGeoPlacesReply::NoError then no error has occurred.
*/
QGeoPlacesReply::Error QGeoPlacesReply::error() const
{
    return d_ptr->error;
}

/*!
    Returns the textual representation of the error state of this reply.

    If no error has occurred this will return an empty string.  It is possible
    that an error occurred which has no associated textual representation, in
    which case this will also return an empty string.

    To determine whether an error has occurred, check to see if
    QGeoPlacesReply::error() is equal to QGeoPlacesReply::NoError.
*/
QString QGeoPlacesReply::errorString() const
{
    return d_ptr->errorString;
}

/*!
    Sets the viewport which contains the results to \a bounds.
*/
void QGeoPlacesReply::setBounds(const QGeoBoundingBox &bounds)
{
    d_ptr->bounds = bounds;
}

/*!
    Returns the viewport which contains the results.

    This function will return an invalid QGeoBoundingBox if no viewport bias
    was specified in the QGeoPlacesManager function which created this reply.
*/
QGeoBoundingBox QGeoPlacesReply::bounds() const
{
    return d_ptr->bounds;
}

/*!
    Returns a list of places.

    The places are the results of the operation corresponding to the
    QGeoPlacesManager function which created this reply.
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
    Sets the list of \a places in the reply.
*/
void QGeoPlacesReply::setPlaces(const QList<QGeoPlace> &places)
{
    d_ptr->places = places;
}

/*!
    Cancels the operation.

    This will do nothing if the reply is finished.
*/
void QGeoPlacesReply::abort()
{
    if (!isFinished())
        setFinished(true);
}

/*!
    \fn void QGeoPlacesReply::finished()

    This signal is emitted when this reply has finished processing.

    If error() equals QGeoPlacesReply::NoError then the processing
    finished successfully.

    This signal and QGeoPlacesManager::finished() will be
    emitted at the same time.

    \note Do no delete this reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/
/*!
    \fn void QGeoPlacesReply::error(QGeoPlacesReply::Error error, const QString &errorString)

    This signal is emitted when an error has been detected in the processing of
    this reply. The finished() signal will probably follow.

    The error will be described by the error code \error. If \a errorString is
    not empty it will contain a textual description of the error.

    This signal and QGeoPlacesManager::error() will be emitted at the same time.

    \note Do no delete this reply object in the slot connected to this
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

QGeoPlacesReplyPrivate& QGeoPlacesReplyPrivate::operator= (const QGeoPlacesReplyPrivate & other)
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
