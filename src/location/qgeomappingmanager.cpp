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

#include "qgeomappingmanager.h"
#include "qgeomappingmanager_p.h"

#include <QNetworkProxy>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMappingManager
    \brief The QGeoMappingManager class manages requests to and replies from
    a geographical map image service.
    \ingroup maps

    The request functions return a QGeoMapReply instance, which is responsible
    for delivering the status and results of the request.

    The rest of the class consists of functions providing metadata about the
    service provider, primarily dealing with the capabilities of the service
    and any limitations that may apply to the request inputs.

    The QGeoMapReply objects and the QGeoMappingManager instance will both
    emit signals to indicate that a request has completed successfully or
    has resulted in an error.

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoMapReply object at an appropriate time. Do not
    directly delete it inside the slot connected to replyFinished() or
    replyError() - the deleteLater() function should be used instead.

    \sa QGeoMapReply
*/


/*!
    \enum QGeoMappingManager::ErrorCode

    Describes the type of error which occurred when a QGeoMappingManager instance
    attempted to process a request.

    \value NoError
        No error occurred.
*/

/*!
    Constructs a QGeoMappingManager object.
*/
QGeoMappingManager::QGeoMappingManager()
        : d_ptr(new QGeoMappingManagerPrivate) {}

/*!
    Destroys this QGeoMappingManager object.
*/
QGeoMappingManager::~QGeoMappingManager()
{
    delete d_ptr;
}

/*!
    \fn QGeoMapReply* QGeoMappingManager::requestMap(const QGeoCoordinate &center,
                                                     int zoomLevel,
                                                     const QSize &size,
                                                     const QGeoMapRequestOptions &requestOptions)

Requests a map image centered at the coordinate \a center with a zoom level
\a zoomLevel and an image size \a size.

The return value is a QGeoMapReply object, which manages the result of the
request.  If the request completes successfully then the QGeoMapReply object
will emit the QGeoMapReply::finished() signal and this QGeoMappingManager
instance will emit the QGeoMappingManager::finished() signal.

The results can then be retreived with QGeoMapReply::mapImage().

If an error occurs then the QGeoMapReply object will emit QGeoMapReply::error()
and this QGeoMappingManager instance will emit QGeoMappingManager::error().

Additional options can also be set with \a requestOptions.  The
supportedMapTypes() and supportedImageFormats() methods provide information
on the acceptable values that can be used inside a QGeoMapRequestOptions object.

The service may impose restrictions on the allowable zoom levels and image
sizes, which can be queried with other methods in this class.

\sa QGeoMapRequestOptions
*/

/*!
    Returns a list of the map types supported by this QGeoMappingManager
    instance.

    \sa QGeoMapWidget::MapType
    \sa QGeoMappingManager::setSupportedMapTypes()
*/
QList<QGeoMapWidget::MapType> QGeoMappingManager::supportedMapTypes() const
{
    return d_ptr->supportedMapTypes;
}

/*!
    Returns a list of the image formats supported by this QGeoMappingManager
    instance.

    \sa QGeoMappingManager::setSupportedImageFormats()
*/
QList<QString> QGeoMappingManager::supportedImageFormats() const
{
    return d_ptr->supportedImageFormats;
}

/*!
    Returns the minimum zoom level supported by this QGeoMappingManager
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    \sa QGeoMappingManager::setMinimumZoomLevel()
    \sa QGeoMappingManager::maximumZoomLevel()
*/
qreal QGeoMappingManager::minimumZoomLevel() const
{
    return d_ptr->minimumZoomLevel;
}

/*!
    Returns the maximum zoom level supported by this QGeoMappingManager
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    \sa QGeoMappingManager::setMaximumZoomLevel()
    \sa QGeoMappingManager::minimumZoomLevel()
*/
qreal QGeoMappingManager::maximumZoomLevel() const

{
    return d_ptr->maximumZoomLevel;
}

/*!
    Returns the size of the smallest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the minimum size of the map image.

    \sa QGeoMappingManager::setMinimumImageSize()
    \sa QGeoMappingManager::maximumImageSize()
*/
QSize QGeoMappingManager::minimumImageSize() const
{
    return d_ptr->minimumImageSize;
}

/*!
    Returns the size of the largest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the maximum size of the map image.
*/
QSize QGeoMappingManager::maximumImageSize() const
{
    return d_ptr->maximumImageSize;
}

/*!
    Sets the list of map types supported by this QGeoMappingManager instance to
    \a mapTypes.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedMapTypes() provides accurate information.

    \sa QGeoMapWidget::MapType
    \sa QGeoMappingManager::supportedMapTypes()
*/
void QGeoMappingManager::setSupportedMapTypes(const QList<QGeoMapWidget::MapType> &mapTypes)
{
    d_ptr->supportedMapTypes = mapTypes;
}

/*!
    Sets the list of image formats supported by this QGeoMappingManager
    instance to \a imageFormats.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedImageFormats() provides accurate information.

    \sa QGeoMappingManager::supportedImageFormats()
*/
void QGeoMappingManager::setSupportedImageFormats(const QList<QString> &imageFormats)
{
    d_ptr->supportedImageFormats = imageFormats;
}

/*!
    Sets the minimum zoom level supported by this QGeoMappingManager
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoCodingService should use this function to ensure that
    minimumZoomLevel() provides accurate information.

    \sa QGeoMappingManager::minimumZoomLevel()
    \sa QGeoMappingManager::setMaximumZoomLevel()
*/
void QGeoMappingManager::setMinimumZoomLevel(qreal minimumZoom)
{
    d_ptr->minimumZoomLevel = minimumZoom;
}

/*!
    Sets the maximum zoom level supported by this QGeoMappingManager
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoCodingService should use this function to ensure that
    maximumZoomLevel() provides accurate information.

    \sa QGeoMappingManager::maximumZoomLevel()
    \sa QGeoMappingManager::setMinimumZoomLevel()
*/
void QGeoMappingManager::setMaximumZoomLevel(qreal maximumZoom)
{
    d_ptr->maximumZoomLevel = maximumZoom;
}

/*!
    Sets the size of the smallest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the minimum size of the map image.

    Subclasses of QGeoCodingService should use this function to ensure that
    minimumImageSize() provides accurate information.

    \sa QGeoMappingManager::minimumImageSize()
    \sa QGeoMappingManager::setMaximumImageSize()
*/
void QGeoMappingManager::setMinimumImageSize(const QSize &minimumImageSize)
{
    d_ptr->minimumImageSize = minimumImageSize;
}

/*!
    Sets the size of the largest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the maximum size of the map image.

    Subclasses of QGeoCodingService should use this function to ensure that
    maximumImageSize() provides accurate information.

    \sa QGeoMappingManager::maximumImageSize()
    \sa QGeoMappingManager::setMinimumImageSize()
*/
void QGeoMappingManager::setMaximumImageSize(const QSize &maximumImageSize)
{
    d_ptr->maximumImageSize = maximumImageSize;
}

//QGeoMapViewport* QGeoMappingManager::createViewport(QGeoMapWidget *widget)
//{
//    return new QGeoMapViewport(widget);
//}

//void QGeoMappingManager::updateMapImage(const QGeoMapViewport &viewport)
//{
//}

//QPointF QGeoMappingManager::coordinateToScreenPosition(const QGeoMapViewport *viewport, const QGeoCoordinate &coordinate) const
//{
//    return QPointF();
//}

//QGeoCoordinate QGeoMappingManager::screenPositionToCoordinate(const QGeoMapViewport *viewport, QPointF screenPosition) const
//{
//    return QGeoCoordinate();
//}

//void QGeoMappingManager::setTileSize(const QSize &tileSize);
//QSize QGeoMappingManager::tileSize() const;

//// TODO better way of returning these results
//void QGeoMappingManager::coordinateToTileIndices(const QGeoCoordinate &coordinate, qint32 zoomLevel, qint32 *rowIndex, qint32 *columnIndex);
//QGeoCoordinate QGeoMappingManager::tileIndicesToUpperLeftCoordinate(qint32 zoomLevel, qint32 rowIndex, qint32 columIndex);
//QGeoBoundingBox QGeoMappingManager::getTileBounds(qint32 zoomLevel, qint32 rowIndex, qint32 columnIndex);

//// TODO create a QGeoMapTileRequest object to make this saner?
//QGeoMapReply* QGeoMappingManager::getTileImage(qint32 zoomLevel, qint32 rowIndex, qint32 columnIndex, QGeoMapWidget::MapType mapType, const QString &imageFormat);

/*!
    \fn void QGeoMappingManager::replyFinished(QGeoMapReply *reply)

    Indicates that a request handled by this QGeoMappingManager object has
    finished successfully.  The result of the request will be in \a reply.

    Note that \a reply will be the same object returned by this
    QGeoMappingManager instance when the request was issued, and that the
    QGeoMapReply::finished() signal can be used instead of this signal if it
    is more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::finished()
*/

/*!
    \fn void QGeoMappingManager::replyError(QGeoMapReply *reply,
                                            QGeoMappingManager::ErrorCode errorCode,
                                            QString errorString);

    Indicates that a request handled by this QGeoMappingManager object has
    failed.  The error is described by \a errorCode and \a errorString, and \a
    reply is the QGeoMapReply object which was managing the result of the
    corresponding service request.

    Note that \a reply will be the same object returned by this
    QGeoMappingManager instance when the request was issued, and that the
    QGeoMapReply::error() signal can be used instead of this signal if it is
    more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::error()
*/

/*******************************************************************************
*******************************************************************************/

QGeoMappingManagerPrivate::QGeoMappingManagerPrivate() {}

QGeoMappingManagerPrivate::QGeoMappingManagerPrivate(const QGeoMappingManagerPrivate &other)
        : supportedMapTypes(other.supportedMapTypes),
        supportedImageFormats(other.supportedImageFormats),
        minimumZoomLevel(other.minimumZoomLevel),
        maximumZoomLevel(other.maximumZoomLevel),
        minimumImageSize(other.minimumImageSize),
        maximumImageSize(other.maximumImageSize) {}

QGeoMappingManagerPrivate::~QGeoMappingManagerPrivate() {}

QGeoMappingManagerPrivate& QGeoMappingManagerPrivate::operator= (const QGeoMappingManagerPrivate & other)
{
    supportedMapTypes = other.supportedMapTypes;
    supportedImageFormats = other.supportedImageFormats;
    minimumZoomLevel = other.minimumZoomLevel;
    maximumZoomLevel = other.maximumZoomLevel;
    minimumImageSize = other.minimumImageSize;
    maximumImageSize = other.maximumImageSize;

    return *this;
}

#include "moc_qgeomappingmanager.cpp"

QTM_END_NAMESPACE
