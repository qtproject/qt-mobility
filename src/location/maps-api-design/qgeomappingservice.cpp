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

#include "qgeomappingservice.h"
#include "qgeomappingservice_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMappingService
    \brief The QGeoMappingService class manages requests to and replies from
    a geographical map image service.
    \ingroup maps

    The request functions return a QGeoMapReply instance, which is responsible
    for delivering the status and results of the request.

    The rest of the class consists of functions providing metadata about the
    service provider, primarily dealing with the capabilities of the service
    and any limitations that may apply to the request inputs.

    The QGeoMapReply objects and the QGeoMappingService instance will both
    emit signals to indicate that a request has completed successfully or
    has resulted in an error.

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoMapReply object at an appropriate time. Do not
    directly delete it inside the slot connected to replyFinished() or
    replyError() - the deleteLater() function should be used instead.

    \sa QGeoMapReply
*/

/*!
    \enum QGeoMappingService::ErrorCode

    Describes the type of error which occurred when a QGeoMappingService instance
    attempted to process a request.

    \value NoError
        No error occurred.
*/

/*!
    Constructs a QGeoMappingService object.
*/
QGeoMappingService::QGeoMappingService()
        : d_ptr(new QGeoMappingServicePrivate()) {}

/*!
    Destroys this QGeoMappingService object.
*/
QGeoMappingService::~QGeoMappingService()
{
    Q_D(QGeoMappingService);
    delete d;
}

/*!
    \fn QGeoMapReply* QGeoMappingService::requestMap(const QGeoCoordinate &center,
                                                     int zoomLevel,
                                                     const QSize &size,
                                                     const QGeoMapRequestOptions &requestOptions)

Requests a map image centered at the coordinate \a center with a zoom level
\a zoomLevel and an image size \a size.

The return value is a QGeoMapReply object, which manages the result of the
request.  If the request completes successfully then the QGeoMapReply object
will emit the QGeoMapReply::finished() signal and this QGeoMappingService
instance will emit the QGeoMappingService::finished() signal.

The results can then be retreived with QGeoMapReply::mapImage().

If an error occurs then the QGeoMapReply object will emit QGeoMapReply::error()
and this QGeoMappingService instance will emit QGeoMappingService::error().

Additional options can also be set with \a requestOptions.  The
supportedMapTypes() and supportedImageFormats() methods provide information
on the acceptable values that can be used inside a QGeoMapRequestOptions object.

The service may impose restrictions on the allowable zoom levels and image
sizes, which can be queried with other methods in this class.

\sa QGeoMapRequestOptions
*/

/*!
    Returns a list of the map types supported by this QGeoMappingService
    instance.

    \sa QGeoMapWidget::MapType
    \sa QGeoMappingService::setSupportedMapTypes()
*/
QList<QGeoMapWidget::MapType> QGeoMappingService::supportedMapTypes() const
{
    Q_D(const QGeoMappingService);
    return d->supportedMapTypes;
}

/*!
    Returns a list of the image formats supported by this QGeoMappingService
    instance.

    \sa QGeoMappingService::setSupportedImageFormats()
*/
QList<QString> QGeoMappingService::supportedImageFormats() const
{
    Q_D(const QGeoMappingService);
    return d->supportedImageFormats;
}

/*!
    Returns the minimum zoom level supported by this QGeoMappingService
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    \sa QGeoMappingService::setMinimumZoomLevel()
    \sa QGeoMappingService::maximumZoomLevel()
*/
int QGeoMappingService::minimumZoomLevel() const
{
    Q_D(const QGeoMappingService);
    return d->minimumZoomLevel;
}

/*!
    Returns the maximum zoom level supported by this QGeoMappingService
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    \sa QGeoMappingService::setMaximumZoomLevel()
    \sa QGeoMappingService::minimumZoomLevel()
*/
int QGeoMappingService::maximumZoomLevel() const

{
    Q_D(const QGeoMappingService);
    return d->maximumZoomLevel;
}

/*!
    Returns the size of the smallest map image which is supported by this
    QGeoMappingService instance.

    An invalid size indicates that this QGeoMappingService instance places
    no restrictions on the minimum size of the map image.

    \sa QGeoMappingService::setMinimumSize()
    \sa QGeoMappingService::maximumImageSize()
*/
QSize QGeoMappingService::minimumImageSize() const
{
    Q_D(const QGeoMappingService);
    return d->minimumImageSize;
}

/*!
    Returns the size of the largest map image which is supported by this
    QGeoMappingService instance.

    An invalid size indicates that this QGeoMappingService instance places
    no restrictions on the maximum size of the map image.
*/
QSize QGeoMappingService::maximumImageSize() const
{
    Q_D(const QGeoMappingService);
    return d->maximumImageSize;
}

/*!
    Sets the list of map types supported by this QGeoMappingService instance to
    \a mapTypes.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedMapTypes() provides accurate information.

    \sa QGeoMapWidget::MapType
    \sa QGeoMappingService::supportedMapTypes()
*/
void QGeoMappingService::setSupportedMapTypes(const QList<QGeoMapWidget::MapType> &mapTypes)
{
    Q_D(QGeoMappingService);
    d->supportedMapTypes = mapTypes;
}

/*!
    Sets the list of image formats supported by this QGeoMappingService
    instance to \a imageFormats.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedImageFormats() provides accurate information.

    \sa QGeoMappingService::supportedImageFormats()
*/
void QGeoMappingService::setSupportedImageFormats(const QList<QString> &imageFormats)
{
    Q_D(QGeoMappingService);
    d->supportedImageFormats = imageFormats;
}

/*!
    Sets the minimum zoom level supported by this QGeoMappingService
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoCodingService should use this function to ensure that
    minimumZoomLevel() provides accurate information.

    \sa QGeoMappingService::minimumZoomLevel()
    \sa QGeoMappingService::setMaximumZoomLevel()
*/
void QGeoMappingService::setMinimumZoomLevel(int minimumZoom)
{
    Q_D(QGeoMappingService);
    d->minimumZoomLevel = minimumZoom;
}

/*!
    Sets the maximum zoom level supported by this QGeoMappingService
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoCodingService should use this function to ensure that
    maximumZoomLevel() provides accurate information.

    \sa QGeoMappingService::maximumZoomLevel()
    \sa QGeoMappingService::setMinimumZoomLevel()
*/
void QGeoMappingService::setMaximumZoomLevel(int maximumZoom)
{
    Q_D(QGeoMappingService);
    d->maximumZoomLevel = maximumZoom;
}

/*!
    Sets the size of the smallest map image which is supported by this
    QGeoMappingService instance.

    An invalid size indicates that this QGeoMappingService instance places
    no restrictions on the minimum size of the map image.

    Subclasses of QGeoCodingService should use this function to ensure that
    minimumImageSize() provides accurate information.

    \sa QGeoMappingService::minimumImageSize()
    \sa QGeoMappingService::setMaximumSize()
*/
void QGeoMappingService::setMinimumSize(const QSize &minimumImageSize)
{
    Q_D(QGeoMappingService);
    d->minimumImageSize = minimumImageSize;
}

/*!
    Sets the size of the largest map image which is supported by this
    QGeoMappingService instance.

    An invalid size indicates that this QGeoMappingService instance places
    no restrictions on the maximum size of the map image.

    Subclasses of QGeoCodingService should use this function to ensure that
    maximumImageSize() provides accurate information.

    \sa QGeoMappingService::maximumImageSize()
    \sa QGeoMappingService::setMinimumSize()
*/
void QGeoMappingService::setMaximumSize(const QSize &maximumImageSize)
{
    Q_D(QGeoMappingService);
    d->maximumImageSize = maximumImageSize;
}

/*!
    \fn void QGeoMappingService::replyFinished(QGeoMapReply *reply)

    Indicates that a request handled by this QGeoMappingService object has
    finished successfully.  The result of the request will be in \a reply.

    Note that \a reply will be the same object returned by this
    QGeoMappingService instance when the request was issued, and that the
    QGeoMapReply::finished() signal can be used instead of this signal if it
    is more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::finished()
*/

/*!
    \fn void QGeoMappingService::replyError(QGeoMapReply *reply,
                                            QGeoMappingService::ErrorCode errorCode,
                                            QString errorString);

    Indicates that a request handled by this QGeoMappingService object has
    failed.  The error is described by \a errorCode and \a errorString, and \a
    reply is the QGeoMapReply object which was managing the result of the
    corresponding service request.

    Note that \a reply will be the same object returned by this
    QGeoMappingService instance when the request was issued, and that the
    QGeoMapReply::error() signal can be used instead of this signal if it is
    more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::error()
*/

/*******************************************************************************
*******************************************************************************/

QGeoMappingServicePrivate::QGeoMappingServicePrivate() {}

QGeoMappingServicePrivate::QGeoMappingServicePrivate(const QGeoMappingServicePrivate &other)
        : supportedMapTypes(other.supportedMapTypes),
        supportedImageFormats(other.supportedImageFormats),
        minimumZoomLevel(other.minimumZoomLevel),
        maximumZoomLevel(other.maximumZoomLevel),
        minimumImageSize(other.minimumImageSize),
        maximumImageSize(other.maximumImageSize) {}

QGeoMappingServicePrivate::~QGeoMappingServicePrivate() {}

QGeoMappingServicePrivate& QGeoMappingServicePrivate::operator= (const QGeoMappingServicePrivate & other)
{
    supportedMapTypes = other.supportedMapTypes;
    supportedImageFormats = other.supportedImageFormats;
    minimumZoomLevel = other.minimumZoomLevel;
    maximumZoomLevel = other.maximumZoomLevel;
    minimumImageSize = other.minimumImageSize;
    maximumImageSize = other.maximumImageSize;

    return *this;
}

#include "moc_qgeomappingservice.cpp"

QTM_END_NAMESPACE
