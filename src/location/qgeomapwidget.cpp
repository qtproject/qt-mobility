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
    \brief The QGeoMapWidget class is used to display a map and manager the
    interactions between the user and the map.

    \inmodule QtLocation
    
    \ingroup maps-mapping

    Most of the functionality is provided by QGeoMappingManager, which
    handles most aspects of the display.

    The map viewport can be panned, the zoom level can be changed and the
    center coordinate of the viewport can be set explicitly.

    The screenPositionToCoordinate() and coordinateToScreenPoisition()
    functions can be used to convert between positions on the screen and
    global coordinates.

    Mouse and keyboard events should be handled by subclassing QGeoMapWidget
    and providing implementations of the event handling functions present in
    QGraphicsWidget.
*/

// DESIGN TODO do we need a signal for when the user pans the map?

/*!
\enum QGeoMapWidget::MapType

Describes a type of map data.

\value NoMap
Indicates a lack of map valid data.

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
    Creates a new mapping widget, with the mapping operations managed by
    \a manager, and the specified \a parent.

    Note that the \a manager will typically be accessed from an instance of
    QGeoServiceProvider:
    \code
        QGeoServiceProvider serviceProvider("nokia");
        QGeoMappingManager *manager = serviceProvider.mappingManager();
        QGeoMapWidget *widget = new QGeoMapWidget(manager);
    \endcode
*/
QGeoMapWidget::QGeoMapWidget(QGeoMappingManager *manager, QGraphicsItem *parent)
        : QGraphicsWidget(parent),
        d_ptr(new QGeoMapWidgetPrivate(manager))
{
    d_ptr->mapData = d_ptr->manager->createMapData(this);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    setMinimumSize(QSizeF(0, 0));
    setPreferredSize(QSizeF(500, 500));
    d_ptr->mapData->setViewportSize(QSizeF(300, 300));
}

/*!
    Destroys this map widget.
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
    painter->drawPixmap(d_ptr->mapData->mapImage().rect(), d_ptr->mapData->mapImage(), d_ptr->mapData->mapImage().rect());
    QPixmap objOverlay = d_ptr->mapData->mapObjectsOverlay();

    if (!objOverlay.isNull())
        painter->drawPixmap(objOverlay.rect(), objOverlay, objOverlay.rect());
}

/*!
    Returns the minimum zoom level supported by the QGeoMappingManager
    associated with this widget.

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
    Returns the maximum zoom level supported by the QGeoMappingManager
    associated with this widget.

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

void QGeoMapWidget::startPanning()
{
    if (d_ptr->mapData)
        d_ptr->mapData->startPanning();
}

void QGeoMapWidget::stopPanning()
{
    if (d_ptr->mapData)
        d_ptr->mapData->stopPanning();
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
        update();
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
    Returns the map types supported by the QGeoMappingManager associated with
    this widget.
*/
QList<QGeoMapWidget::MapType> QGeoMapWidget::supportedMapTypes() const
{
    if (d_ptr->manager)
        return d_ptr->manager->supportedMapTypes();

    return QList<QGeoMapWidget::MapType>();
}

/*!
    Changes the type of map data to display to \a mapType.

    This will do nothing if \a mapType is not present in supportedMapTypes().
*/
void QGeoMapWidget::setMapType(QGeoMapWidget::MapType mapType)
{
    if (d_ptr->mapData && d_ptr->manager) {
        if (!d_ptr->manager->supportedMapTypes().contains(mapType))
            return;

        d_ptr->mapData->setMapType(mapType);
        d_ptr->manager->updateMapImage(d_ptr->mapData);
    }
}

/*!
    Returns the type of map data which is being displayed.
*/
QGeoMapWidget::MapType QGeoMapWidget::mapType() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapType();

    return QGeoMapWidget::NoMap;
}

/*!
    Adds \a mapObject to the list of map objects managed by this widget.

    If \a mapObject is within the viewport of the map and
    QGeoMapObject::isVisible() returns true then the map will display the map
    object immediately.

    The widget will take ownership of the \a mapObject.
*/
void QGeoMapWidget::addMapObject(QGeoMapObject *mapObject)
{
    if (!mapObject)
        return;

    if (d_ptr->mapData)
        d_ptr->mapData->addMapObject(mapObject);

    this->update();
}

/*!
    Removes \a mapObject from the list of map objects managed by this widget.

    If \a mapObject is within the viewport of the map and
    QGeoMapObject::isVisible() returns true then the map will stop displaying
    the map object immediately.

    The widget will release ownership of the \a mapObject.
*/
void QGeoMapWidget::removeMapObject(QGeoMapObject *mapObject)
{
    // TODO update display if visible?
    if (d_ptr->mapData)
        d_ptr->mapData->removeMapObject(mapObject);
}

/*!
    Returns the list of map objects managed by this widget.
*/
QList<QGeoMapObject*> QGeoMapWidget::mapObjects()
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjects();

    return QList<QGeoMapObject*>();
}

/*!
    Returns the list of map objects managed by this widget which are currently
    visible and at least partially within the viewport of the map.
*/
QList<QGeoMapObject*> QGeoMapWidget::visibleMapObjects()
{
    if (d_ptr->mapData)
        return d_ptr->mapData->visibleMapObjects();

    return QList<QGeoMapObject*>();
}

/*!
    Returns the list of map objects managed by this widget which are visible
    and contain the point \a screenPosition within their boundaries.
*/
QList<QGeoMapObject*> QGeoMapWidget::mapObjectsAtScreenPosition(const QPointF &screenPosition)
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjectsAtScreenPosition(screenPosition);

    return QList<QGeoMapObject*>();
}

/*!
    Returns the list of map objects managed by this widget which are visible
    and which are displayed at least partially within the on screen rectangle
    \a screenRect.
*/
QList<QGeoMapObject*> QGeoMapWidget::mapObjectsInScreenRect(const QRectF &screenRect)
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjectsInScreenRect(screenRect);

    return QList<QGeoMapObject*>();
}

/*!
    Returns the position on the screen at which \a coordinate is displayed.

    An invalid QPointF will be returned if \a coordinate is invalid or is not
    within the current viewport.
*/
QPointF QGeoMapWidget::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->coordinateToScreenPosition(coordinate);

    return QPointF();
}

/*!
    Returns the coordinate corresponding to the point in the viewport at \a
    screenPosition.

    An invalid QGeoCoordinate will be returned if \a screenPosition is invalid
    or is not within the current viewport.
*/
QGeoCoordinate QGeoMapWidget::screenPositionToCoordinate(QPointF screenPosition) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->screenPositionToCoordinate(screenPosition);

    return QGeoCoordinate();
}

/*!
\fn void QGeoMapWidget::zoomLevelChanged(qreal zoomLevel)

Indicates that the zoom level has changed to \a zoomLevel.
*/

/*!
\fn void QGeoMapWidget::centered(const QGeoCoordinate &coordinate)

Indicates that the map has been centered on \a coordinate.

This signal will not be emitted when the user pans the map.
*/

/*!
\fn void QGeoMapWidget::mapTypeChanged(MapType mapType)

Indicates that the type of the map has been changed.
*/

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
