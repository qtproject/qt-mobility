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
QGeoMappingManager::QGeoMappingManager(QGeoMappingManagerPrivate &dd)
    : d_ptr(&dd) {}

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
    Q_D(QGeoMappingManager);
    delete d;
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

    \sa QGeoMappingManager::MapType
    \sa QGeoMappingManager::setSupportedMapTypes()
*/
QList<QGeoMappingManager::MapType> QGeoMappingManager::supportedMapTypes() const
{
    Q_D(const QGeoMappingManager);
    return d->supportedMapTypes;
}

/*!
    Returns a list of the image formats supported by this QGeoMappingManager
    instance.

    \sa QGeoMappingManager::setSupportedImageFormats()
*/
QList<QString> QGeoMappingManager::supportedImageFormats() const
{
    Q_D(const QGeoMappingManager);
    return d->supportedImageFormats;
}

/*!
    Returns the minimum zoom level supported by this QGeoMappingManager
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    \sa QGeoMappingManager::setMinimumZoomLevel()
    \sa QGeoMappingManager::maximumZoomLevel()
*/
int QGeoMappingManager::minimumZoomLevel() const
{
    Q_D(const QGeoMappingManager);
    return d->minimumZoomLevel;
}

/*!
    Returns the maximum zoom level supported by this QGeoMappingManager
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    \sa QGeoMappingManager::setMaximumZoomLevel()
    \sa QGeoMappingManager::minimumZoomLevel()
*/
int QGeoMappingManager::maximumZoomLevel() const

{
    Q_D(const QGeoMappingManager);
    return d->maximumZoomLevel;
}

/*!
    Returns the size of the smallest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the minimum size of the map image.

    \sa QGeoMappingManager::setMinimumSize()
    \sa QGeoMappingManager::maximumImageSize()
*/
QSize QGeoMappingManager::minimumImageSize() const
{
    Q_D(const QGeoMappingManager);
    return d->minimumSize;
}

/*!
    Returns the size of the largest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the maximum size of the map image.
*/
QSize QGeoMappingManager::maximumImageSize() const
{
    Q_D(const QGeoMappingManager);
    return d->maximumSize;
}

/*!
    Sets the list of map types supported by this QGeoMappingManager instance to
    \a mapTypes.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedMapTypes() provides accurate information.

    \sa QGeoMappingManager::MapType
    \sa QGeoMappingManager::supportedMapTypes()
*/
void QGeoMappingManager::setSupportedMapTypes(const QList<MapType> &mapTypes)
{
    Q_D(QGeoMappingManager);
    d->supportedMapTypes = mapTypes;
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
    Q_D(QGeoMappingManager);
    d->supportedImageFormats = imageFormats;
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
void QGeoMappingManager::setMinimumZoomLevel(int minimumZoom)
{
    Q_D(QGeoMappingManager);
    d->minimumZoomLevel = minimumZoom;
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
void QGeoMappingManager::setMaximumZoomLevel(int maximumZoom)
{
    Q_D(QGeoMappingManager);
    d->maximumZoomLevel = maximumZoom;
}

/*!
    Sets the size of the smallest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the minimum size of the map image.

    Subclasses of QGeoCodingService should use this function to ensure that
    minimumImageSize() provides accurate information.

    \sa QGeoMappingManager::minimumImageSize()
    \sa QGeoMappingManager::setMaximumSize()
*/
void QGeoMappingManager::setMinimumSize(const QSize &minimumSize)
{
    Q_D(QGeoMappingManager);
    d->minimumSize = minimumSize;
}

/*!
    Sets the size of the largest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the maximum size of the map image.

    Subclasses of QGeoCodingService should use this function to ensure that
    maximumImageSize() provides accurate information.

    \sa QGeoMappingManager::maximumImageSize()
    \sa QGeoMappingManager::setMinimumSize()
*/
void QGeoMappingManager::setMaximumSize(const QSize &maximumSize)
{
    Q_D(QGeoMappingManager);
    d->maximumSize = maximumSize;
}

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

/*!
    Returns the current internal zoom level
*/
int QGeoMappingManager::zoomLevel() const
{
    Q_D(const QGeoMappingManager);
    return d->zoomLevel;
}

/*!
    Sets the \a size of the internal view port. This will usually be 
    set by the associated QGeoMapWidget to the size of its drawable
    rectangle.
*/
void QGeoMappingManager::setViewPortSize(const QSize &size)
{
    Q_D(QGeoMappingManager);
    d->viewPortSize = size;
}

/*!
    Returns the size of the internal view port.
*/
QSize QGeoMappingManager::viewPortSize() const
{
    Q_D(const QGeoMappingManager);
    return d->viewPortSize;
}

/*!
    Sets the map \a widget associated with this mapping service.
    Whenever the internal map representation changes,
    \a widget->update() will be called.
*/
//void QGeoMappingManager::setMapWidget(QGeoMapWidget *widget)
//{
//    Q_D(QGeoMappingManager);
//    d->mapWidget = widget;
//}

/*!
    \fn virtual void pan(int startX, int startY, int endX, int endY) = 0

    Pans the internal map representation by the pixel delta
    defined by \a start and \a end point.
*/

/*!
    \fn virtual void paint(QPainter *painter) = 0

    Paints the internal map representation into the
    context of \a painter.
    The internal view port is aligned with the top-left
    corner of the \a painter.
*/

/*******************************************************************************
*******************************************************************************/

QGeoMappingManagerPrivate::QGeoMappingManagerPrivate() 
//    : mapWidget(0)
{}

QGeoMappingManagerPrivate::QGeoMappingManagerPrivate(const QGeoMappingManagerPrivate &other)
    : supportedMapTypes(other.supportedMapTypes),
    supportedImageFormats(other.supportedImageFormats),
    minimumZoomLevel(other.minimumZoomLevel),
    maximumZoomLevel(other.maximumZoomLevel),
    minimumSize(other.minimumSize),
    maximumSize(other.maximumSize),
    viewPortSize(other.viewPortSize),
    zoomLevel(other.zoomLevel)
//    mapWidget(other.mapWidget)
{}

QGeoMappingManagerPrivate::~QGeoMappingManagerPrivate() {}

QGeoMappingManagerPrivate& QGeoMappingManagerPrivate::operator= (const QGeoMappingManagerPrivate &other)
{
    supportedMapTypes = other.supportedMapTypes;
    supportedImageFormats = other.supportedImageFormats;
    minimumZoomLevel = other.minimumZoomLevel;
    maximumZoomLevel = other.maximumZoomLevel;
    minimumSize = other.minimumSize;
    maximumSize = other.maximumSize;
    viewPortSize = other.viewPortSize;
    zoomLevel = other.zoomLevel;
//    mapWidget = other.mapWidget;
    return *this;
}


#include "moc_qgeomappingmanager.cpp"

QTM_END_NAMESPACE
