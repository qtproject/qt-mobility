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

#include "qgeomapviewport.h"
#include "qgeomapviewport_p.h"
#include "qgeomapwidget.h"

QTM_BEGIN_NAMESPACE

//!
//    \class QGeoMapViewport
//    \brief The QGeoMapviewport class manages requests to and replies from
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
//    Constructs a QGeoMapViewport object.
//*/
//QGeoMapViewport::QGeoMapViewport(QGeoMappingManager *manager)
//        : d_ptr(new QGeoMapViewportPrivate)
//{
//    d->engine = manager;
//}

///*!
//    Destroys this QGeoMapViewport object.
//*/
//QGeoMapViewport::~QGeoMapViewport()
//{
//    Q_D(QGeoMapViewport);
//    delete d;
//}

///*!
//    Returns the current internal zoom level
//*/
//int QGeoMapViewport::zoomLevel() const
//{
//    Q_D(const QGeoMapViewport);
//    return d->zoomLevel;
//}

///*!
//    Sets the \a size of the internal view port. This will usually be
//    set by the associated QGeoMapWidget to the size of its drawable
//    rectangle.
//*/
//void QGeoMapViewport::setViewportSize(const QSize &size)
//{
//    Q_D(QGeoMapViewport);
//    d->viewportSize = size;
//}

///*!
//    Returns the size of the internal view port.
//*/
//QSize QGeoMapViewport::viewportSize() const
//{
//    Q_D(const QGeoMapViewport);
//    return d->viewportSize;
//}

///*!
//    Sets the map \a widget associated with this mapping service.
//    Whenever the internal map representation changes,
//    \a widget->update() will be called.
//*/
////void QGeoMapViewport::setMapWidget(QGeoMapWidget *widget)
////{
////    Q_D(QGeoMapViewport);
////    d->mapWidget = widget;
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


QGeoMapViewport::QGeoMapViewport(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget)
    : d_ptr(new QGeoMapViewportPrivate())
{
    Q_D(QGeoMapViewport);
    d->widget = widget;
    d->engine = engine;
}

QGeoMapViewport::QGeoMapViewport(QGeoMapViewportPrivate *dd, QGeoMappingManagerEngine *engine, QGeoMapWidget *widget)
    : d_ptr(dd)
{
    Q_D(QGeoMapViewport);
    d->widget = widget;
    d->engine = engine;
}

QGeoMapViewport::~QGeoMapViewport()
{
    Q_D(QGeoMapViewport);

    if (d->engine)
        d->engine->removeViewport(this);

    delete d;
}

QGeoMapWidget* QGeoMapViewport::widget() const
{
    Q_D(const QGeoMapViewport);
    return d->widget;
}

QGeoMappingManagerEngine* QGeoMapViewport::engine() const
{
    Q_D(const QGeoMapViewport);
    return d->engine;
}

void QGeoMapViewport::setZoomLevel(qreal zoomLevel)
{
    Q_D(QGeoMapViewport);
    d->zoomLevel = zoomLevel;
}

qreal QGeoMapViewport::zoomLevel() const
{
    Q_D(const QGeoMapViewport);
    return d->zoomLevel;
}

void QGeoMapViewport::pan(int dx, int dy)
{
    QPointF pos = coordinateToScreenPosition(center());
    setCenter(screenPositionToCoordinate(QPointF(pos.x() + dx, pos.y() + dy)));
}

void QGeoMapViewport::setCenter(const QGeoCoordinate &center)
{
    Q_D(QGeoMapViewport);
    d->center = center;
}

QGeoCoordinate QGeoMapViewport::center() const
{
    Q_D(const QGeoMapViewport);
    return d->center;
}

void QGeoMapViewport::setViewportSize(const QSizeF &size)
{
    Q_D(QGeoMapViewport);
    d->viewportSize = size;
}

QSizeF QGeoMapViewport::viewportSize() const
{
    Q_D(const QGeoMapViewport);
    return d->viewportSize;
}

void QGeoMapViewport::setMapType(QGeoMapWidget::MapType mapType)
{
    Q_D(QGeoMapViewport);
    d->mapType = mapType;
}

QGeoMapWidget::MapType QGeoMapViewport::mapType() const
{
    Q_D(const QGeoMapViewport);
    return d->mapType;
}

void QGeoMapViewport::setImageChangesTriggerUpdates(bool trigger)
{
    Q_D(QGeoMapViewport);
    d->imageChangesTriggerUpdates = trigger;
}

bool QGeoMapViewport::imageChangesTriggerUpdates() const
{
    Q_D(const QGeoMapViewport);
    return d->imageChangesTriggerUpdates;
}

void QGeoMapViewport::setMapImage(const QPixmap &mapImage)
{
    Q_D(QGeoMapViewport);
    d->mapImage = mapImage;
    if (d->imageChangesTriggerUpdates)
        d->widget->mapImageUpdated();
}

QPixmap QGeoMapViewport::mapImage()
{
    Q_D(QGeoMapViewport);
    return d->mapImage;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapViewportPrivate::QGeoMapViewportPrivate() {}

QGeoMapViewportPrivate::QGeoMapViewportPrivate(const QGeoMapViewportPrivate &other)
    : widget(other.widget),
    engine(other.engine),
    zoomLevel(other.zoomLevel),
    center(other.center),
    viewportSize(other.viewportSize),
    mapType(other.mapType),
    imageChangesTriggerUpdates(other.imageChangesTriggerUpdates),
    mapImage(other.mapImage) {}

QGeoMapViewportPrivate::~QGeoMapViewportPrivate() {}

QGeoMapViewportPrivate& QGeoMapViewportPrivate::operator= (const QGeoMapViewportPrivate &other)
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
