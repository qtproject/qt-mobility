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

#include "qgeomapreply.h"
#include "qgeomapreply_p.h"

#include <QPixmap>

QTM_BEGIN_NAMESPACE

/*!
  \class QGeoMapReply
  \brief The QGeoMapReply class represents the result of a request for mapping
  images.
  \ingroup maps

    The QGeoMapReply class is responsible for signalling whether a mapping
    request completed, whether the request was completed successfully, and for
    delivering the result of the request.

    Unless the cancel() method is used, a QGeoMapReply object will emit the
    finished() or error() signals to indicate that the request has completed.

    After the finished() signal has been emitted the result can be retrieved
    with mapImage().

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoMapReply object at an appropriate time. Do not
    directly delete it inside the slot connected to finished() or error() -
    the deleteLater() function should be used instead.

    \sa QGeoMappingService
*/

/*!
    Constructs a QGeoMapReply object with parent \a parent.
*/
QGeoMapReply::QGeoMapReply(QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoMapReplyPrivate()) {}

/*!
    Destroys this QGeoMapReply object.
*/
QGeoMapReply::~QGeoMapReply()
{
    Q_D(QGeoMapReply);
    delete d;
}

/*!
    Sets the map image in the reply to \a image.

    \sa QGeoMapReply::mapImage()
*/
void QGeoMapReply::setMapImage(const QPixmap &image)
{
    Q_D(QGeoMapReply);
    d->mapImage = image;
}

/*!
    Returns the requested map image.

    The result of calling this function prior to receiving the finished()
    signal is undefined.

    \sa QGeoMapReply::setMapImage()
*/
QPixmap QGeoMapReply::mapImage() const
{
    Q_D(const QGeoMapReply);
    return d->mapImage;
}

/*!
    \fn void QGeoMapReply::cancel()

    Cancels this QGeoMapReply if it has not yet completed.

    The finished() signal will not be emitted.

    This function has no effect if the geocoding request has completed and this
    QGeoMapReply object has emitted the finished() or error() signals.

    The user is still responsible for deleting this QGeoMapReply object.
*/

/*!
    \fn void QGeoMapReply::finished()

    This QGeoMapReply object represents the outcome of a request against a
    QGeoMappingService instance.  If this signal is emitted it indicates that
    the requested service was completed successfully.

    Note that the QGeoMappingService::replyFinished() signal can be used instead
    of this signal if it is more convinient to do so.

    Do not delete this QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMappingService::replyFinished()
*/

/*!
    \fn void QGeoMapReply::error(QGeoMappingService::ErrorCode errorCode,
                                 QString errorString)

    This QGeoMapReply object represents the outcome of a request against a
    QGeoMappingService instance.  If this signal is emitted it indicates that
    the requested service did not finish successfully.  The error that
    prevented the fulfullment of the request is described by \a errorCode
    and \a errorString.

    Note that the QGeoMappingService::replyError() signal can be used instead of
    this signal if it is more convinient to do so.

    Do not delete this QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMappingService::replyError()
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapReplyPrivate::QGeoMapReplyPrivate() {}

QGeoMapReplyPrivate::QGeoMapReplyPrivate(const QGeoMapReplyPrivate &other)
    : mapImage(other.mapImage) {}

QGeoMapReplyPrivate::~QGeoMapReplyPrivate() {}

QGeoMapReplyPrivate& QGeoMapReplyPrivate::operator= (const QGeoMapReplyPrivate &other)
{
    mapImage = other.mapImage;

    return *this;
}

#include "moc_qgeomapreply.cpp"

QTM_END_NAMESPACE
