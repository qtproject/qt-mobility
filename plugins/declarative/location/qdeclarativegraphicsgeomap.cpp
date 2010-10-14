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

#include "qdeclarativegraphicsgeomap_p.h"

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoserviceprovider_p.h"

#include <qgeoserviceprovider.h>
#include <qgeomappingmanager.h>
#include <qgeomapdata.h>
#include <qgeomapobject.h>

QTM_BEGIN_NAMESPACE

QDeclarativeGraphicsGeoMap::QDeclarativeGraphicsGeoMap(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
    serviceProvider_(0),
    mapData_(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    center_ = new QDeclarativeCoordinate(this);
    center_->setCoordinate(QGeoCoordinate(-27.0, 153.0));
    mapType_ = QDeclarativeGraphicsGeoMap::StreetMap;
    zoomLevel_ = 8;
    size_ = QSizeF(100.0, 100.0);
    
}

QDeclarativeGraphicsGeoMap::~QDeclarativeGraphicsGeoMap()
{
    qDeleteAll(objects_);

    if (mapData_)
        delete mapData_;

    if (serviceProvider_)
        delete serviceProvider_;
}

void QDeclarativeGraphicsGeoMap::paint(QPainter *painter,
                                       const QStyleOptionGraphicsItem *option,
                                       QWidget * /*widget*/)
{
    if (mapData_) {
        mapData_->paint(painter, option);
    }
}

void QDeclarativeGraphicsGeoMap::geometryChanged(const QRectF &newGeometry,
                                                 const QRectF & /*oldGeometry*/)
{
    setSize(newGeometry.size());
}

// this is write once
void QDeclarativeGraphicsGeoMap::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (plugin_)
        return;

    plugin_ = plugin;

    emit pluginChanged(plugin_);

    serviceProvider_ = new QGeoServiceProvider(plugin_->name(),
                                               plugin_->parameterMap());

    // check for error

    mappingManager_ = serviceProvider_->mappingManager();

    // check for error

    mapData_ = mappingManager_->createMapData();
    mapData_->init();
    //mapData_->setParentItem(this);

    // setters
    mapData_->setWindowSize(size_);
    mapData_->setZoomLevel(zoomLevel_);
    mapData_->setCenter(center_->coordinate());
    mapData_->setMapType(QGraphicsGeoMap::MapType(mapType_));
    mapData_->setConnectivityMode(QGraphicsGeoMap::ConnectivityMode(connectivityMode_));

    for (int i = 0; i < objects_.size(); ++i)
        mapData_->addMapObject(objects_.at(i));
    objects_.clear();


    // setup signals

    connect(mapData_,
            SIGNAL(updateMapDisplay(QRectF)),
            this,
            SLOT(updateMapDisplay(QRectF)));

    connect(mapData_,
            SIGNAL(centerChanged(QGeoCoordinate)),
            this,
            SLOT(internalCenterChanged(QGeoCoordinate)));

    connect(mapData_,
            SIGNAL(mapTypeChanged(QGraphicsGeoMap::MapType)),
            this,
            SLOT(internalMapTypeChanged(QGraphicsGeoMap::MapType)));

    connect(mapData_,
            SIGNAL(connectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)),
            this,
            SLOT(internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode)));

    connect(mapData_,
            SIGNAL(windowSizeChanged(QSizeF)),
            this,
            SIGNAL(sizeChanged(QSizeF)));

    connect(mapData_,
            SIGNAL(zoomLevelChanged(qreal)),
            this,
            SIGNAL(zoomLevelChanged(qreal)));
}

void QDeclarativeGraphicsGeoMap::updateMapDisplay(const QRectF &target)
{
    update(target);
}

QDeclarativeGeoServiceProvider* QDeclarativeGraphicsGeoMap::plugin() const
{
    return plugin_;
}

qreal QDeclarativeGraphicsGeoMap::minimumZoomLevel() const
{
    if (mappingManager_)
        return mappingManager_->minimumZoomLevel();
    else
        return -1.0;
}

qreal QDeclarativeGraphicsGeoMap::maximumZoomLevel() const
{
    if (mappingManager_)
        return mappingManager_->maximumZoomLevel();
    else
        return -1.0;
}

// TODO make these more QML like
//QList<MapType> QDeclarativeGraphicsGeoMap::supportedMapTypes() const;
//QList<ConnectivityMode> QDeclarativeGraphicsGeoMap::supportedConnectivityModes() const;

void QDeclarativeGraphicsGeoMap::setSize(const QSizeF &size)
{
    if (mapData_) {
        setWidth(size.width());
        setHeight(size.height());
        mapData_->setWindowSize(size);
    } else {
        if (size_ == size)
            return;

        size_ = size;

        emit sizeChanged(size_);
    }
        
}

QSizeF QDeclarativeGraphicsGeoMap::size() const
{
    if (mapData_)
        return mapData_->windowSize();
    else
        return size_;
}

void QDeclarativeGraphicsGeoMap::setZoomLevel(qreal zoomLevel)
{
    if (mapData_) {
        mapData_->setZoomLevel(zoomLevel);
    } else {
        if (zoomLevel_ == zoomLevel)
            return;

        zoomLevel_ = zoomLevel;

        emit zoomLevelChanged(zoomLevel_);
    }
}

qreal QDeclarativeGraphicsGeoMap::zoomLevel() const
{
    if (mapData_) {
        return mapData_->zoomLevel();
    } else {
        return zoomLevel_;
    }
}

void QDeclarativeGraphicsGeoMap::setCenter(const QDeclarativeCoordinate *center)
{
    if (mapData_) {
        mapData_->setCenter(center->coordinate());
    } else {
        if (center_->coordinate() == center->coordinate())
            return;

        center_->setCoordinate(center->coordinate());

        emit declarativeCenterChanged(center_);
    }
}

QDeclarativeCoordinate* QDeclarativeGraphicsGeoMap::center() const
{
    if (mapData_)
        center_->setCoordinate(mapData_->center());
    return center_;
}

void QDeclarativeGraphicsGeoMap::setMapType(QDeclarativeGraphicsGeoMap::MapType mapType)
{
    if (mapData_) {
        mapData_->setMapType(QGraphicsGeoMap::MapType(mapType));
    } else {
        if (mapType_ == mapType)
            return;

        mapType_ = mapType;

        emit mapTypeChanged(mapType_);
    }
}

QDeclarativeGraphicsGeoMap::MapType QDeclarativeGraphicsGeoMap::mapType() const
{
    if (mapData_) {
        return QDeclarativeGraphicsGeoMap::MapType(mapData_->mapType());
    } else {
        return mapType_;
    }
}

void QDeclarativeGraphicsGeoMap::setConnectivityMode(QDeclarativeGraphicsGeoMap::ConnectivityMode connectivityMode)
{
    if (mapData_) {
        mapData_->setConnectivityMode(QGraphicsGeoMap::ConnectivityMode(connectivityMode));
    } else {
        if (connectivityMode_ == connectivityMode)
            return;

        connectivityMode_ = connectivityMode;

        emit connectivityModeChanged(connectivityMode_);
    }
}

QDeclarativeGraphicsGeoMap::ConnectivityMode QDeclarativeGraphicsGeoMap::connectivityMode() const
{
    if (mapData_)
        return QDeclarativeGraphicsGeoMap::ConnectivityMode(mapData_->connectivityMode());
    else
        return connectivityMode_;
}

QDeclarativeListProperty<QGeoMapObject> QDeclarativeGraphicsGeoMap::objects()
{
    return QDeclarativeListProperty<QGeoMapObject>(this,
            0,
            object_append,
            object_count,
            object_at,
            object_clear);
}

void QDeclarativeGraphicsGeoMap::object_append(QDeclarativeListProperty<QGeoMapObject> *prop, QGeoMapObject *mapObject)
{
    QDeclarativeGraphicsGeoMap *map = static_cast<QDeclarativeGraphicsGeoMap*>(prop->object);

    if (map->mapData_)
        map->mapData_->addMapObject(mapObject);
    else
        map->objects_.append(mapObject);
}

int QDeclarativeGraphicsGeoMap::object_count(QDeclarativeListProperty<QGeoMapObject> *prop)
{
    QDeclarativeGraphicsGeoMap *map = static_cast<QDeclarativeGraphicsGeoMap*>(prop->object);

    if (map->mapData_)
        return map->mapData_->mapObjects().size();
    else
        return map->objects_.size();
}

QGeoMapObject *QDeclarativeGraphicsGeoMap::object_at(QDeclarativeListProperty<QGeoMapObject> *prop, int index)
{
    QDeclarativeGraphicsGeoMap *map = static_cast<QDeclarativeGraphicsGeoMap*>(prop->object);

    if (map->mapData_)
        return map->mapData_->mapObjects().at(index);
    else
        return map->objects_.at(index);
}

void QDeclarativeGraphicsGeoMap::object_clear(QDeclarativeListProperty<QGeoMapObject> *prop)
{
    QDeclarativeGraphicsGeoMap *map = static_cast<QDeclarativeGraphicsGeoMap*>(prop->object);

    if (map->mapData_)
        map->mapData_->clearMapObjects();
    else
        map->objects_.clear();
}

QDeclarativeCoordinate* QDeclarativeGraphicsGeoMap::toCoordinate(QPointF screenPosition) const
{
    QGeoCoordinate coordinate;

    if (mapData_)
        coordinate = mapData_->screenPositionToCoordinate(screenPosition);

    return new QDeclarativeCoordinate(coordinate,
                                      const_cast<QDeclarativeGraphicsGeoMap *>(this));
}

QPointF QDeclarativeGraphicsGeoMap::toScreenPosition(QDeclarativeCoordinate* coordinate) const
{
    QPointF point;

    if (mapData_)
        point = mapData_->coordinateToScreenPosition(coordinate->coordinate());

    return point;
}

void QDeclarativeGraphicsGeoMap::pan(int dx, int dy)
{
    if (mapData_) {
        mapData_->pan(dx, dy);
        update();
    }
}

void QDeclarativeGraphicsGeoMap::internalCenterChanged(const QGeoCoordinate &coordinate)
{
    emit declarativeCenterChanged(new QDeclarativeCoordinate(coordinate, this));
}

void QDeclarativeGraphicsGeoMap::internalMapTypeChanged(QGraphicsGeoMap::MapType mapType)
{
    emit mapTypeChanged(QDeclarativeGraphicsGeoMap::MapType(mapType));
}

void QDeclarativeGraphicsGeoMap::internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode)
{
    emit connectivityModeChanged(QDeclarativeGraphicsGeoMap::ConnectivityMode(connectivityMode));
}

#include "moc_qdeclarativegraphicsgeomap_p.cpp"

QTM_END_NAMESPACE

