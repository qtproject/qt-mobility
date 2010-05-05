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

#include "qgeocodereply.h"
#include "qgeocodereply_p.h"

#include "qgeolocation.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoCodeReply
    \brief The QGeoCodeReply class represents the result of a request for
    geocoding information.
    \ingroup maps

    The QGeoCodeReply class is responsible for signalling whether a
    geocoding request completed, whether the request was completed successfully,
     and for delivering the result of the request.

    Unless the cancel() method is used, a QGeoCodeReply object will emit the
    finished() or error() signals to indicate that the request has completed.

    After the finished() signal has been emitted the results can be retrieved
    with locations().

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoCodeReply object at an appropriate time. Do not
    directly delete it inside the slot connected to finished() or error() -
    the deleteLater() function should be used instead.

    \sa QGeoCodingService
*/

/*!
    Constructs a QGeoCodeReply object with parent \a parent.
*/
QGeoCodeReply::QGeoCodeReply(QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoCodeReplyPrivate()) {}

/*!
    Destroys this QGeoCodeReply object.
*/
QGeoCodeReply::~QGeoCodeReply()
{
    Q_D(QGeoCodeReply);
    delete d;
}

/*!
    Sets the list of locations in the reply to \a locations.

    The QGeoLocation class contains address and coordinate information and
    may also contain a bounding box which contains the results.

    The geocoding services are responsible for converting address to
    coordinates and coordinates to addresses.  The locations returned should
    contain the address and coordinate data relevant to the conversion.

    The locations should be ordered from the most specific to least specific
    repsonses to the request.

    \sa QGeoLocation
    \sa QGeoCodeReply::locations()
*/
void QGeoCodeReply::setLocations(const QList<QGeoLocation> &locations)
{
    Q_D(QGeoCodeReply);
    d->locations = locations;
}

/*!
    Returns the result of the geocoding request as a list of locations.

    The QGeoLocation class contains address and coordinate information and
    may also contain a bounding box which contains the results.

    The geocoding services are responsible for converting address to
    coordinates and coordinates to addresses.  The locations returned should
    contain the address and coordinate data relevant to the conversion.

    The locations will be ordered from the most specific to least specific
    repsonses to the request.

    The result of calling this function prior to receiving the finished()
    signal is undefined.

    \sa QGeoLocation
    \sa QGeoCodeReply::setLocations()
*/
QList<QGeoLocation> QGeoCodeReply::locations() const
{
    Q_D(const QGeoCodeReply);
    return d->locations;
}

/*!
    \fn void QGeoCodeReply::cancel()

    Cancels this QGeoCodeReply if it has not yet completed.

    The finished() signal will not be emitted.

    This function has no effect if the geocoding request has completed and this
    QGeoCodeReply object has emitted the finished() or error() signals.

    The user is still responsible for deleting this QGeoCodeReply object.
*/

/*!
    \fn void QGeoCodeReply::finished()

    This QGeoCodeReply object represents the outcome of a request against a
    QGeoCodingService instance.  If this signal is emitted it indicates that
    the requested service was completed successfully.

    Note that the QGeoCodingService::replyFinished() signal can be used instead
    of this signal if it is more convinient to do so.

    Do not delete this QGeoCodeReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoCodingService::replyFinished()
*/

/*!
    \fn void QGeoCodeReply::error(QGeoCodingService::ErrorCode errorCode,
                                  QString errorString)


    This QGeoCodeReply object represents the outcome of a request against a
    QGeoCodingService instance.  If this signal is emitted it indicates that
    the requested service did not finish successfully.  The error that
    prevented the fulfullment of the request is described by \a errorCode
    and \a errorString.

    Note that the QGeoCodingService::replyError() signal can be used instead of
    this signal if it is more convinient to do so.

    Do not delete this QGeoCodeReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoCodingService::replyError()
*/

/*******************************************************************************
*******************************************************************************/

QGeoCodeReplyPrivate::QGeoCodeReplyPrivate() {}

QGeoCodeReplyPrivate::QGeoCodeReplyPrivate(const QGeoCodeReplyPrivate &other)
    : locations(other.locations) {}

QGeoCodeReplyPrivate::~QGeoCodeReplyPrivate() {}

QGeoCodeReplyPrivate& QGeoCodeReplyPrivate::operator= (const QGeoCodeReplyPrivate &other)
{
    locations = other.locations;
    return *this;
}

#include "moc_qgeocodereply.cpp"

QTM_END_NAMESPACE
