/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgraphicsgeomap.h"
#include "qgraphicsgeomap_p.h"

#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"
#include "qgeomapobject.h"
#include "qgeomapoverlay.h"

#include "qgeoserviceprovider.h"
#include "qgeomappingmanager.h"
#include "qgeomapdata.h"

#include <QGraphicsSceneResizeEvent>
#include <QTimer>

#include <QNetworkProxyFactory>

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QGraphicsGeoMap
    \brief The QGraphicsGeoMap class is used to display a map and manager the
    interactions between the user and the map.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps-mapping

    Most of the functionality is provided by QGeoMappingManager, which
    handles most aspects of the display.

    The map viewport can be panned, the zoom level can be changed and the
    center coordinate of the viewport can be set explicitly.

    The screenPositionToCoordinate() and coordinateToScreenPoisition()
    functions can be used to convert between positions on the screen and
    global coordinates.

    Mouse and keyboard events should be handled by subclassing QGraphicsGeoMap
    and providing implementations of the event handling functions present in
    QGraphicsWidget.
*/

/*!
\enum QGraphicsGeoMap::MapType

Describes a type of map data.

\value NoMap
Indicates a lack of map valid data.

\value StreetMap
The map data is a graphical representation of streets and building boundaries.

\value SatelliteMapDay
The map data is composed of images collected by satellites during the daytime.

\value SatelliteMapNight
The map data is composed of images collected by satellites during the nighttime.
For the online nokia plugin the SatelliteMapDay data is being used.

\value TerrainMap
The map data is a graphical representation of terrain features.  This may also
include some of the information provided by QGraphicsGeoMap::StreetMap.

\value HybridMap
The map data is composed of images collected by satellites during the daytime,
on top of which there is a graphical representation of streets superimposed.

\value TransitMap
The map data is a graphical representation of streets and building boundaries,
on top of which there is a public transport scheme superimposed.

\value GrayStreetMap
The map data is a color-reduced version of the QGraphicsGeoMap::StreetMap map data.

\value MobileStreetMap
The map data is a mobile scheme (increased font and line size) of
the QGraphicsGeoMap::StreetMap map data.

\value MobileTerrainMap
The map data is a mobile scheme (increased font and line size) of
the QGraphicsGeoMap::TerrainMap map data.

\value MobileHybridMap
The map data is a mobile scheme (increased font and line size) of
the QGraphicsGeoMap::HybridMap map data.

\value MobileTransitMap
The map data is a mobile scheme (increased font and line size) of
the QGraphicsGeoMap::TransitMap map data.

\value MobileGrayStreetMap
The map data is a mobile scheme (increased font and line size) of
the QGraphicsGeoMap::GrayStreetMap map data.

*/

/*!
\enum QGraphicsGeoMap::ConnectivityMode

Describes the method of obtaining the mapping data.

\value NoConnectivity
There is no map data.

\value OfflineMode
The map data will come from an offline source.

\value OnlineMode
The map data will come from an online source.

\value HybridMode
The map data will come from a combination of offline and online sources.
*/

/*!
    Creates a new mapping widget, with the mapping operations managed by
    \a manager, and the specified \a parent.

    Note that the \a manager will typically be accessed from an instance of
    QGeoServiceProvider:
    \code
        QGeoServiceProvider serviceProvider("nokia");
        QGeoMappingManager *manager = serviceProvider.mappingManager();
        QGraphicsGeoMap *widget = new QGraphicsGeoMap(manager);
    \endcode
*/
QGraphicsGeoMap::QGraphicsGeoMap(QGeoMappingManager *manager, QGraphicsItem *parent)
    : QGraphicsWidget(parent),
      d_ptr(new QGraphicsGeoMapPrivate())
{
    Q_ASSERT(manager != 0);
    d_ptr->manager = manager;

    d_ptr->mapData = d_ptr->manager->createMapData();
    d_ptr->mapData->init();

    connect(d_ptr->mapData,
            SIGNAL(updateMapDisplay(QRectF)),
            this,
            SLOT(updateMapDisplay(QRectF)));

    setMapType(QGraphicsGeoMap::StreetMap);
    d_ptr->mapData->setWindowSize(size());

    connect(d_ptr->mapData,
            SIGNAL(zoomLevelChanged(qreal)),
            this,
            SIGNAL(zoomLevelChanged(qreal)));
    connect(d_ptr->mapData,
            SIGNAL(bearingChanged(qreal)),
            this,
            SIGNAL(bearingChanged(qreal)));
    connect(d_ptr->mapData,
            SIGNAL(tiltChanged(qreal)),
            this,
            SIGNAL(tiltChanged(qreal)));
    connect(d_ptr->mapData,
            SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)),
            this,
            SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)));
    connect(d_ptr->mapData,
            SIGNAL(centerChanged(QGeoCoordinate)),
            this,
            SIGNAL(centerChanged(QGeoCoordinate)));
    connect(d_ptr->mapData,
            SIGNAL(connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)),
            this,
            SIGNAL(connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)));

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    setMinimumSize(QSizeF(0, 0));
    setPreferredSize(QSizeF(500, 500));
}

/*!
    Destroys this map widget.
*/
QGraphicsGeoMap::~QGraphicsGeoMap()
{
    delete d_ptr;
}

/*!
  \reimp
  \since 1.1
*/
void QGraphicsGeoMap::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    if (d_ptr->mapData) {
        d_ptr->mapData->setWindowSize(event->newSize());
    }
}

/*!
  \reimp
  \since 1.1
*/
QPainterPath QGraphicsGeoMap::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

/*!
  \reimp
  \since 1.1
*/
void QGraphicsGeoMap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (d_ptr->mapData) {
        painter->save();
        painter->setViewport(pos().x(), pos().y(), size().width(), size().height());
        painter->setWindow(pos().x(), pos().y(), size().width(), size().height());
        d_ptr->mapData->paint(painter, option);
        painter->restore();
    }
}

void QGraphicsGeoMap::updateMapDisplay(const QRectF &target)
{
    update(target);
}

/*!
    \property QGraphicsGeoMap::minimumZoomLevel
    \brief the minimum zoom level supported by the
    QGeoMappingManager associated with this widget.

    Larger values of the zoom level correspond to more detailed views of the
    map.
    \since 1.1
*/
qreal QGraphicsGeoMap::minimumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->minimumZoomLevel();

    return -1;
}

/*!
    \property QGraphicsGeoMap::maximumZoomLevel
    \brief the maximum zoom level supported by the
    QGeoMappingManager associated with this widget.

    Larger values of the zoom level correspond to more detailed views of the
    map.
    \since 1.1
*/
qreal QGraphicsGeoMap::maximumZoomLevel() const
{
    if (d_ptr->manager)
        return d_ptr->manager->maximumZoomLevel();

    return -1;
}

/*!
    \property QGraphicsGeoMap::zoomLevel
    \brief the zoom level of the map.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    If zoomLevel is less than minimumZoomLevel then minimumZoomLevel
    will be used, and if zoomLevel is  larger than
    maximumZoomLevel then maximumZoomLevel will be used.
    \since 1.1
*/
void QGraphicsGeoMap::setZoomLevel(qreal zoomLevel)
{
    if (d_ptr->mapData)
        d_ptr->mapData->setZoomLevel(zoomLevel);
}

qreal QGraphicsGeoMap::zoomLevel() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->zoomLevel();

    return -1;
}
/*!
    \property QGraphicsGeoMap::supportsBearing
    \brief  whether bearing is supported by the
    QGeoMappingManager associated with this widget.
    \since 1.2
*/
bool QGraphicsGeoMap::supportsBearing() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->supportsBearing();

    return false;
}

/*!
    \property QGraphicsGeoMap::bearing
    \brief the bearing of the map.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.

    Changes to this property will be ignored if supportsBearing()
    returns false.
    \since 1.2
*/
void QGraphicsGeoMap::setBearing(qreal bearing)
{
    if (d_ptr->mapData)
        d_ptr->mapData->setBearing(bearing);
}

qreal QGraphicsGeoMap::bearing() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->bearing();

    return 0;
}

/*!
    \property QGraphicsGeoMap::supportsTilting
    \brief whether tilting is supported by the
    QGeoMappingManager associated with this widget.
    \since 1.2
*/
bool QGraphicsGeoMap::supportsTilting() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->supportsTilting();

    return false;
}

/*!
    \property QGraphicsGeoMap::minimumTilt
    \brief the minimum tilt supported by the
    QGeoMappingManager associated with this widget.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
    \since 1.2
*/
qreal QGraphicsGeoMap::minimumTilt() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->minimumTilt();

    return 0;
}

/*!
    \property QGraphicsGeoMap::maximumTilt
    \brief the maximum tilt supported by the
    QGeoMappingManager associated with this widget.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.
    \since 1.2
*/
qreal QGraphicsGeoMap::maximumTilt() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->maximumTilt();

    return 0;
}

/*!
    \property QGraphicsGeoMap::tilt
    \brief the tilt of the map.

    Value in degrees where 0 is equivalent to 90 degrees between view and earth's
    surface i.e. looking straight down to earth.

    If \a tilt is less than minimumTilt() then minimumTilt()
    will be used, and if \a tilt is  larger than
    maximumTilt() then maximumTilt() will be used.

    Changes to this property will be ignored if supportsTilting()
    returns false.
    \since 1.2
*/
void QGraphicsGeoMap::setTilt(qreal tilt)
{
    if (d_ptr->mapData)
        d_ptr->mapData->setTilt(tilt);
}

qreal QGraphicsGeoMap::tilt() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->tilt();

    return 0;
}

/*!
    Pans the map view \a dx pixels in the x direction and \a dy pixels
    in the y direction.

    The x and y axes are specified in Graphics View Framework coordinates.
    By default this will mean that positive values of \a dx move the
    viewed area to the right and that positive values of \a dy move the
    viewed area down.

    After the panning has occurred the centerChanged() signal will be emitted.
    \since 1.1
*/
void QGraphicsGeoMap::pan(int dx, int dy)
{
    if (d_ptr->mapData) {
        d_ptr->mapData->pan(dx, dy);
        update();
    }
}

/*!
    \property QGraphicsGeoMap::center
    \brief the coordinate at the center of the map viewport.

    Panning the map can be more efficient than changing the center by small
    increments.
    \since 1.1
*/
void QGraphicsGeoMap::setCenter(const QGeoCoordinate &center)
{
    if (d_ptr->mapData)
        d_ptr->mapData->setCenter(center);
}

QGeoCoordinate QGraphicsGeoMap::center() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->center();

    return QGeoCoordinate();
}

/*!
    Returns the map types supported by the QGeoMappingManager associated with
    this widget.
    \since 1.1
*/
QList<QGraphicsGeoMap::MapType> QGraphicsGeoMap::supportedMapTypes() const
{
    if (d_ptr->manager)
        return d_ptr->manager->supportedMapTypes();

    return QList<QGraphicsGeoMap::MapType>();
}

/*!
  \property QGraphicsGeoMap::mapType
  \brief the type of map data displayed by the map.

  Setting mapType to a type not present in supportedMapTypes() will do
  nothing.
    \since 1.1
*/
void QGraphicsGeoMap::setMapType(QGraphicsGeoMap::MapType mapType)
{
    if (d_ptr->mapData && d_ptr->manager) {
        if (!d_ptr->manager->supportedMapTypes().contains(mapType))
            return;

        d_ptr->mapData->setMapType(mapType);
    }
}

QGraphicsGeoMap::MapType QGraphicsGeoMap::mapType() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapType();

    return QGraphicsGeoMap::NoMap;
}

/*!
    Returns the connectivity modes supported by the QGeoMappingManager associated with
    this widget.
    \since 1.1
*/
QList<QGraphicsGeoMap::ConnectivityMode> QGraphicsGeoMap::supportedConnectivityModes() const
{
    if (d_ptr->manager)
        return d_ptr->manager->supportedConnectivityModes();

    return QList<QGraphicsGeoMap::ConnectivityMode>();
}

/*!
  \property QGraphicsGeoMap::connectivityMode
  \brief the connectivity mode used to obtain the map data.

  Setting connectivityMode to a mode not present in supportedConnectivityModes() will do
  nothing.
  \since 1.1
*/
void QGraphicsGeoMap::setConnectivityMode(QGraphicsGeoMap::ConnectivityMode connectivityMode)
{
    if (d_ptr->mapData && d_ptr->manager) {
        if (!d_ptr->manager->supportedConnectivityModes().contains(connectivityMode))
            return;

        d_ptr->mapData->setConnectivityMode(connectivityMode);
    }
}

QGraphicsGeoMap::ConnectivityMode QGraphicsGeoMap::connectivityMode() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->connectivityMode();

    return QGraphicsGeoMap::NoConnectivity;
}

/*!
    Returns whether custom map objects are supported by this engine.

    Custom map objects are map objects based on QGraphicsItem instances, which
    are hard to support in cases where the map rendering is not being
    performed by the Qt Graphics View framwork.
    \since 1.2
*/
bool QGraphicsGeoMap::supportsCustomMapObjects() const
{
    if (d_ptr->manager)
        return d_ptr->manager->supportsCustomMapObjects();

    return false;
}

/*!
    Returns the map objects associated with this map.
    \since 1.1
*/
QList<QGeoMapObject*> QGraphicsGeoMap::mapObjects() const
{
    if (!d_ptr->mapData)
        return QList<QGeoMapObject*>();

    return d_ptr->mapData->mapObjects();
}

/*!
    Adds \a mapObject to the list of map objects managed by this widget.

    If \a mapObject is within the viewport of the map and
    QGeoMapObject::isVisible() returns true then the map will display the map
    object immediately.

    The map will take ownership of the \a mapObject.

    If supportsCustomMapObject() returns false and \a mapObject is a custom map
    object then \a mapObject will not be added to the map.
    \since 1.1
*/
void QGraphicsGeoMap::addMapObject(QGeoMapObject *mapObject)
{
    if (!mapObject || !d_ptr->mapData)
        return;

    if ((mapObject->type() == QGeoMapObject::CustomType) && !supportsCustomMapObjects())
        return;

    d_ptr->mapData->addMapObject(mapObject);

    this->update();
}

/*!
    Removes \a mapObject from the list of map objects managed by this widget.

    If \a mapObject is within the viewport of the map and
    QGeoMapObject::isVisible() returns true then the map will stop displaying
    the map object immediately.

    The map will release ownership of the \a mapObject.
    \since 1.1
*/
void QGraphicsGeoMap::removeMapObject(QGeoMapObject *mapObject)
{
    if (!mapObject || !d_ptr->mapData)
        return;

    d_ptr->mapData->removeMapObject(mapObject);

    this->update();
}

/*!
    Clears the map objects associated with this map.

    The map objects will be deleted.
    \since 1.1
*/
void QGraphicsGeoMap::clearMapObjects()
{
    if (!d_ptr->mapData)
        return;

    d_ptr->mapData->clearMapObjects();
}

/*!
    Returns the map overlays associated with this map.
    \since 1.1
*/
QList<QGeoMapOverlay*> QGraphicsGeoMap::mapOverlays() const
{
    if (!d_ptr->mapData)
        return QList<QGeoMapOverlay*>();

    return d_ptr->mapData->mapOverlays();
}

/*!
    Adds \a overlay to the list of map overlays associated with this map.

    The overlays will be drawn in the order in which they were added.

    The map will take ownership of \a overlay.
    \since 1.1
*/
void QGraphicsGeoMap::addMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay || !d_ptr->mapData)
        return;

    d_ptr->mapData->addMapOverlay(overlay);

    this->update();
}

/*!
    Removes \a overlay from the list of map overlays associated with this map.

    The map will release ownership of \a overlay.
    \since 1.1
*/
void QGraphicsGeoMap::removeMapOverlay(QGeoMapOverlay *overlay)
{
    if (!overlay || !d_ptr->mapData)
        return;

    d_ptr->mapData->removeMapOverlay(overlay);

    this->update();
}

/*!
    Clears the map overlays associated with this map.

    The map overlays will be deleted.
    \since 1.1
*/
void QGraphicsGeoMap::clearMapOverlays()
{
    if (!d_ptr->mapData)
        return;

    d_ptr->mapData->clearMapOverlays();
}

/*!
    Returns a bounding box corresponding to the physical area displayed
    in the viewport of the map.

    The bounding box which is returned is defined by the upper left and
    lower right corners of the visible area of the map.
    \since 1.1
*/
QGeoBoundingBox QGraphicsGeoMap::viewport() const
{
    if (!d_ptr->mapData)
        return QGeoBoundingBox();

    return d_ptr->mapData->viewport();
}

/*!
    Attempts to fit the bounding box \a bounds into the viewport of the map.

    This method will change the zoom level to the maximum zoom level such
    that all of \a bounds is visible within the resulting viewport.

    If \a preserveViewportCenter is false the map will be centered on the
    bounding box \a bounds before the zoom level is changed, otherwise the
    center of the map will not be changed.
    \since 1.1
*/
void QGraphicsGeoMap::fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter)
{
    if (!d_ptr->mapData)
        return;

    d_ptr->mapData->fitInViewport(bounds, preserveViewportCenter);
}

/*!
    Returns the list of visible map objects managed by this widget which
    contain the point \a screenPosition within their boundaries.
    \since 1.1
*/
QList<QGeoMapObject*> QGraphicsGeoMap::mapObjectsAtScreenPosition(const QPointF &screenPosition) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjectsAtScreenPosition(screenPosition);

    return QList<QGeoMapObject*>();
}

/*!
    Returns the list of visible map objects managed by this widget which are
    displayed at least partially within the on screen rectangle
    \a screenRect.
    \since 1.1
*/
QList<QGeoMapObject*> QGraphicsGeoMap::mapObjectsInScreenRect(const QRectF &screenRect) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjectsInScreenRect(screenRect);

    return QList<QGeoMapObject*>();
}

/*!
    Returns the list of visible map objects manager by this widget which
    are displayed at least partially within the viewport of the map.
    \since 1.1
*/
QList<QGeoMapObject*> QGraphicsGeoMap::mapObjectsInViewport() const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->mapObjectsInViewport();

    return QList<QGeoMapObject*>();
}

/*!
    Returns the position on the screen at which \a coordinate is displayed.

    An invalid QPointF will be returned if \a coordinate is invalid or is not
    within the current viewport.
    \since 1.1
*/
QPointF QGraphicsGeoMap::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
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
    \since 1.1
*/
QGeoCoordinate QGraphicsGeoMap::screenPositionToCoordinate(QPointF screenPosition) const
{
    if (d_ptr->mapData)
        return d_ptr->mapData->screenPositionToCoordinate(screenPosition);

    return QGeoCoordinate();
}

/*!
\fn void QGraphicsGeoMap::zoomLevelChanged(qreal zoomLevel)

    This signal is emitted when the zoom level of the map changes.

    The new value is \a zoomLevel.
    \since 1.1
*/

/*!
\fn void QGraphicsGeoMap::bearingChanged(qreal bearing)

    This signal is emitted when the bearing of the map changes.

    The new value is \a bearing.

    This signal will not be emitted if supportsBearing()
    returns false.
    \since 1.2
*/

/*!
\fn void QGraphicsGeoMap::tiltChanged(qreal tilt)

    This signal is emitted when the tilt of the map changes.

    The new value is \a tilt.

    This signal will not be emitted if supportsTilting()
    returns false.
    \since 1.2
*/

/*!
\fn void QGraphicsGeoMap::centerChanged(const QGeoCoordinate &coordinate)

    This signal is emitted when the center of the map changes.

    The new value is \a coordinate.
    \since 1.1
*/

/*!
\fn void QGraphicsGeoMap::mapTypeChanged(QGraphicsGeoMap::MapType mapType)

    This signal is emitted when the map type changes.

    The new value is \a mapType.
    \since 1.1
*/

/*!
\fn void QGraphicsGeoMap::connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode)

    This signal is emitted when the connectivity mode used to fetch the
    map data changes.

    The new value is \a connectivityMode.
    \since 1.1
*/

/*******************************************************************************
*******************************************************************************/

QGraphicsGeoMapPrivate::QGraphicsGeoMapPrivate()
      : manager(0),
      mapData(0),
      panActive(false) {}

QGraphicsGeoMapPrivate::~QGraphicsGeoMapPrivate()
{
    if (mapData)
        delete mapData;
}

#include "moc_qgraphicsgeomap.cpp"

QTM_END_NAMESPACE
