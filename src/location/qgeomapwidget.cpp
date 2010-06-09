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

#include "qgeomapwidget.h"
#include "qgeomapwidget_p.h"

#include "qgeocoordinate.h"
#include "qgeomapobject.h"

#include <QGraphicsSceneResizeEvent>
#include <QTimer>

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QGeoMapWidget
  \brief The QGeoMapWidget class is.
  \ingroup maps-mapping
*/

/*!
\enum QGeoMapWidget::MapType

Describes a type of map data.

\value StreetMap
The map data is a graphical representation of streets and building boundaries.

\value SatelliteMapDay
The map data is composed of images collected by satellites during the daytime.

\value SatelliteMapNight
The map data is composed of images collected by satellites during the nighttime.

\value TerrainMap
The map data is a graphical representation of terrain features.  This may also
include some of the information provided by QGeoMapWidget::StreetMap.
*/

/*!
*/
QGeoMapWidget::QGeoMapWidget(QGeoMappingManager *manager)
        : d_ptr(new QGeoMapWidgetPrivate(manager))
{
    d_ptr->mapData = d_ptr->manager->createMapData(this);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    setMinimumSize(QSizeF(0, 0));
    setPreferredSize(QSizeF(500, 500));
    d_ptr->mapData->setViewportSize(QSizeF(300, 300));
}

/*!
*/
QGeoMapWidget::~QGeoMapWidget()
{
    delete d_ptr;
}

/*!
  \reimp
*/
void QGeoMapWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsWidget::resizeEvent(event);
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->setViewportSize(event->newSize());
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }

    event->accept();
}

/*!
  \reimp
*/
QPainterPath QGeoMapWidget::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

/*!
  \reimp
*/
void QGeoMapWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    d_ptr->mapData->mapImage().rect();
    painter->drawPixmap(d_ptr->mapData->mapImage().rect(), d_ptr->mapData->mapImage(), d_ptr->mapData->mapImage().rect());
}

/*!
    Returns the minimum zoom level supported by this manager.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMapWidget::minimumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->minimumZoomLevel();

    return -1;
}

/*!
    Returns the maximum zoom level supported by this manager.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMapWidget::maximumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->maximumZoomLevel();

    return -1;
}

/*!
    Sets the zoom level of the map to \a zoomLevel.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    If \a zoomLevel is less than minimumZoomLevel() then minimumZoomLevel()
    will be used, and if \a zoomLevel is  larger than
    maximumZoomLevel() then maximumZoomLevel() will be used.
*/
void QGeoMapWidget::setZoomLevel(qreal zoomLevel)
{
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->setZoomLevel(zoomLevel);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
    Returns the zoom level of the map.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMapWidget::zoomLevel() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->zoomLevel();

    return -1;
}

/*!
    Pans the map view \a dx pixels in the x direction and \a dy pixels
    in they y direction.

    The x and y axes are specified in Graphics View Framework coordinates.
    By default this will mean that positive values of \a dx move the
    viewed area to the right and that positive values of \a dy move the
    viewed area down.
*/
void QGeoMapWidget::pan(int dx, int dy)
{
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->pan(dx, dy);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
    Centers the map viewport on the coordinate \a center.
*/
void QGeoMapWidget::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->setCenter(center);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
    Returns the coordinate of the point in the center of the map viewport.
*/
QGeoCoordinate QGeoMapWidget::center() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->center();

    return QGeoCoordinate();
}

/*!
    Changes the type of map data to display to \a mapType.
*/
void QGeoMapWidget::setMapType(QGeoMapWidget::MapType mapType)
{
    if (d_ptr->mapData && d_ptr->manager) {
        d_ptr->mapData->setMapType(mapType);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
    Returns the type of map data which is being displayed.
*/
QGeoMapWidget::MapType QGeoMapWidget::mapType() const
{
    // TODO null check
    return d_ptr->mapData->mapType();
}

/*!
*/
void QGeoMapWidget::addMapObject(QGeoMapObject *mapObject)
{
    // TODO null check
    d_ptr->mapData->addMapObject(mapObject);
}

/*!
*/
void QGeoMapWidget::removeMapObject(QGeoMapObject *mapObject)
{
    // TODO null check
    d_ptr->mapData->removeMapObject(mapObject);
}

/*!
*/
QList<QGeoMapObject*> QGeoMapWidget::mapObjects()
{
    // TODO null check
    return d_ptr->mapData->mapObjects();
}

/*!
*/
QList<QGeoMapObject*> QGeoMapWidget::visibleMapObjects()
{
    // TODO null check
    return d_ptr->mapData->visibleMapObjects();
}

/*!
*/
QList<QGeoMapObject*> QGeoMapWidget::mapObjectsAtScreenPosition(const QPointF &screenPosition, int radius)
{
    // TODO null check
    return d_ptr->mapData->mapObjectsAtScreenPosition(screenPosition, radius);
}

/*!
*/
QList<QGeoMapObject*> QGeoMapWidget::mapObjectsInScreenRect(const QRectF &screenRect)
{
    // TODO null check
    return d_ptr->mapData->mapObjectsInScreenRect(screenRect);
}

/*!
*/
QPointF QGeoMapWidget::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->coordinateToScreenPosition(coordinate);

    return QPointF();
}

/*!
*/
QGeoCoordinate QGeoMapWidget::screenPositionToCoordinate(QPointF screenPosition) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->screenPositionToCoordinate(screenPosition);

    return QGeoCoordinate();
}

/*******************************************************************************
*******************************************************************************/

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(QGeoMappingManager *manager)
        : manager(manager),
        mapData(0),
        panActive(false) {}

QGeoMapWidgetPrivate::QGeoMapWidgetPrivate(const QGeoMapWidgetPrivate &other)
        : manager(other.manager),
        mapData(other.mapData),
        panActive(other.panActive) {}

QGeoMapWidgetPrivate::~QGeoMapWidgetPrivate() {}

QGeoMapWidgetPrivate& QGeoMapWidgetPrivate::operator= (const QGeoMapWidgetPrivate & other)
{
    manager = other.manager;
    mapData = other.mapData;
    panActive = other.panActive;

    return *this;
}

#include "moc_qgeomapwidget.cpp"

QTM_END_NAMESPACE
