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

#include "qgeomapdata.h"
#include "qgeomapdata_p.h"
#include "qgeomapwidget.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QGeoMapData
  \brief The QGeoMapData class is.
  \ingroup maps-mapping
 */

//!
//    \class QGeoMapData
//    \brief The QGeoMapData class manages requests to and replies from
//    a geographical map image service.
//    \ingroup maps

//    The request functions return a QGeoMapReply instance, which is responsible
//    for delivering the status and results of the request.

//    The rest of the class consists of functions providing metadata about the
//    service provider, primarily dealing with the capabilities of the service
//    and any limitations that may apply to the request inputs.

//    The QGeoMapReply objects and the QGeoMappingManager instance will both
//    emit signals to indicate that a request has completed successfully or
//    has resulted in an error.

//    \note After the request has finished, it is the responsibility of the user
//    to delete the QGeoMapReply object at an appropriate time. Do not
//    directly delete it inside the slot connected to replyFinished() or
//    replyError() - the deleteLater() function should be used instead.

//    \sa QGeoMapReply
//*/


///*!
//    Constructs a QGeoMapData object.
//*/
//QGeoMapData::QGeoMapData(QGeoMappingManager *manager)
//        : d_ptr(new QGeoMapDataPrivate)
//{
//    d_ptr->engine = manager;
//}

///*!
//    Destroys this QGeoMapData object.
//*/
//QGeoMapData::~QGeoMapData()
//{
//    Q_D(QGeoMapData);
//    delete d;
//}

///*!
//    Returns the current internal zoom level
//*/
//int QGeoMapData::zoomLevel() const
//{
//    Q_D(const QGeoMapData);
//    return d_ptr->zoomLevel;
//}

///*!
//    Sets the \a size of the internal view port. This will usually be
//    set by the associated QGeoMapWidget to the size of its drawable
//    rectangle.
//*/
//void QGeoMapData::setViewportSize(const QSize &size)
//{
//    Q_D(QGeoMapData);
//    d_ptr->viewportSize = size;
//}

///*!
//    Returns the size of the internal view port.
//*/
//QSize QGeoMapData::viewportSize() const
//{
//    Q_D(const QGeoMapData);
//    return d_ptr->viewportSize;
//}

///*!
//    Sets the map \a widget associated with this mapping service.
//    Whenever the internal map representation changes,
//    \a widget->update() will be called.
//*/
////void QGeoMapData::setMapWidget(QGeoMapWidget *widget)
////{
////    Q_D(QGeoMapData);
////    d_ptr->mapWidget = widget;
////}

///*!
//    \fn virtual void pan(int startX, int startY, int endX, int endY) = 0

//    Pans the internal map representation by the pixel delta
//    defined by \a start and \a end point.
//*/

///*!
//    \fn virtual void paint(QPainter *painter) = 0

//    Paints the internal map representation into the
//    context of \a painter.
//    The internal view port is aligned with the top-left
//    corner of the \a painter.


QGeoMapData::QGeoMapData(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget)
        : d_ptr(new QGeoMapDataPrivate())
{
    d_ptr->widget = widget;
    d_ptr->engine = engine;
}

//QGeoMapData::QGeoMapData(QGeoMapDataPrivate *dd, QGeoMappingManagerEngine *engine, QGeoMapWidget *widget)
//        : d_ptr(dd)
//{
//    d_ptr->widget = widget;
//    d_ptr->engine = engine;
//}

QGeoMapData::~QGeoMapData()
{
    if (d_ptr->engine)
        d_ptr->engine->removeMapData(this);

    delete d_ptr;
}

QGeoMapWidget* QGeoMapData::widget() const
{
    return d_ptr->widget;
}

QGeoMappingManagerEngine* QGeoMapData::engine() const
{
    return d_ptr->engine;
}

void QGeoMapData::setZoomLevel(qreal zoomLevel)
{
    d_ptr->zoomLevel = zoomLevel;
}

qreal QGeoMapData::zoomLevel() const
{
    return d_ptr->zoomLevel;
}

void QGeoMapData::pan(int dx, int dy)
{
    QPointF pos = coordinateToScreenPosition(center());
    setCenter(screenPositionToCoordinate(QPointF(pos.x() + dx, pos.y() + dy)));
}

void QGeoMapData::setCenter(const QGeoCoordinate &center)
{
    d_ptr->center = center;
}

QGeoCoordinate QGeoMapData::center() const
{
    return d_ptr->center;
}

void QGeoMapData::setViewportSize(const QSizeF &size)
{
    d_ptr->viewportSize = size;
}

QSizeF QGeoMapData::viewportSize() const
{
    return d_ptr->viewportSize;
}

void QGeoMapData::setMapType(QGeoMapWidget::MapType mapType)
{
    d_ptr->mapType = mapType;
}

QGeoMapWidget::MapType QGeoMapData::mapType() const
{
    return d_ptr->mapType;
}

void QGeoMapData::setImageChangesTriggerUpdates(bool trigger)
{
    d_ptr->imageChangesTriggerUpdates = trigger;
}

bool QGeoMapData::imageChangesTriggerUpdates() const
{
    return d_ptr->imageChangesTriggerUpdates;
}

void QGeoMapData::setMapImage(const QPixmap &mapImage)
{
    d_ptr->mapImage = mapImage;
    if (d_ptr->imageChangesTriggerUpdates)
        d_ptr->widget->mapImageUpdated();
}

QPixmap QGeoMapData::mapImage()
{
    return d_ptr->mapImage;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapDataPrivate::QGeoMapDataPrivate() {}

QGeoMapDataPrivate::QGeoMapDataPrivate(const QGeoMapDataPrivate &other)
        : widget(other.widget),
        engine(other.engine),
        zoomLevel(other.zoomLevel),
        center(other.center),
        viewportSize(other.viewportSize),
        mapType(other.mapType),
        imageChangesTriggerUpdates(other.imageChangesTriggerUpdates),
        mapImage(other.mapImage) {}

QGeoMapDataPrivate::~QGeoMapDataPrivate() {}

QGeoMapDataPrivate& QGeoMapDataPrivate::operator= (const QGeoMapDataPrivate & other)
{
    widget = other.widget;
    engine = other.engine;
    zoomLevel = other.zoomLevel;
    center = other.center;
    viewportSize = other.viewportSize;
    mapType = other.mapType;
    imageChangesTriggerUpdates = other.imageChangesTriggerUpdates;
    mapImage = other.mapImage;

    return *this;
}

QTM_END_NAMESPACE
