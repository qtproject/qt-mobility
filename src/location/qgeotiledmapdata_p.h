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

#ifndef QGEOTILEDMAPDATA_P_H
#define QGEOTILEDMAPDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgeomapdata_p.h"
#include "qgeomapobject.h"
#include "qgeomapobject_p.h"

#include <QRectF>
#include <QHash>
#include <QCache>
#include <QVector>
#include <QString>
#include <QPainterPath>
#include <QPair>
#include <QList>
#include <QLineF>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsItem;
class QGraphicsRectItem;
class QGraphicsPolygonItem;
class QGraphicsPathItem;
class QGraphicsPixmapItem;
class QGraphicsLineItem;
class QGraphicsItemGroup;

QTM_BEGIN_NAMESPACE

class QGeoMapRectangleObject;
class QGeoMapMarkerObject;
class QGeoMapPolylineObject;
class QGeoMapRouteObject;

class QGeoMapRectangleObjectPrivate;
class QGeoMapCircleObjectPrivate;
class QGeoMapMarkerObjectPrivate;
class QGeoMapPolylineObjectPrivate;
class QGeoMapPolygonObjectPrivate;
class QGeoMapRouteObjectPrivate;

class QGeoTiledMapData;
class QGeoTiledMapRequest;
class QGeoTiledMapReply;

class QGeoTiledMapObjectInfo : public QGeoMapObjectInfo
{
public:
    QGeoTiledMapObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapObjectInfo();

    void addToParent();
    void removeFromParent();

    QGeoBoundingBox boundingBox() const;
    bool contains(const QGeoCoordinate &coord) const;

    QRectF bounds;

    QGraphicsItem *graphicsItem;
    QGeoTiledMapDataPrivate *mapData;
};

class QGeoTiledMapRectangleObjectInfo : public QGeoTiledMapObjectInfo
{
public:
    QGeoTiledMapRectangleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapRectangleObjectInfo();

    void objectUpdate();
    void mapUpdate();

    const QGeoMapRectangleObjectPrivate* rectangle;
    QGraphicsRectItem *rectangleItem;
};

class QGeoTiledMapCircleObjectInfo : public QGeoTiledMapObjectInfo
{
public:
    QGeoTiledMapCircleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapCircleObjectInfo();

    void objectUpdate();
    void mapUpdate();

    const QGeoMapCircleObjectPrivate* circle;
};

class QGeoTiledMapPolylineObjectInfo : public QGeoTiledMapObjectInfo
{
public:
    QGeoTiledMapPolylineObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapPolylineObjectInfo();

    void objectUpdate();
    void mapUpdate();

    const QGeoMapPolylineObjectPrivate *polyline;
    QGraphicsPathItem *pathItem;

    QList<QPointF> points;
};

class QGeoTiledMapPolygonObjectInfo : public QGeoTiledMapObjectInfo
{
public:
    QGeoTiledMapPolygonObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapPolygonObjectInfo();

    void objectUpdate();
    void mapUpdate();

    const QGeoMapPolygonObjectPrivate *polygon;
    QGraphicsPolygonItem *polygonItem;

    QPolygonF points;
};

class QGeoTiledMapMarkerObjectInfo : public QGeoTiledMapObjectInfo
{
public:
    QGeoTiledMapMarkerObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapMarkerObjectInfo();

    void objectUpdate();
    void mapUpdate();

    const QGeoMapMarkerObjectPrivate* marker;
    QGraphicsPixmapItem *pixmapItem;
};

class QGeoTiledMapRouteObjectInfo : public QGeoTiledMapObjectInfo
{
public:
    QGeoTiledMapRouteObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapRouteObjectInfo();

    void objectUpdate();
    void mapUpdate();

    //QLineF connectShortest(const QGeoCoordinate &point1, const QGeoCoordinate &point2) const;

    const QGeoMapRouteObjectPrivate *route;
    QGraphicsPathItem *pathItem;
    //QGraphicsItemGroup *groupItem;

    QList<QPointF> points;
    QList<QPointF> distanceFilteredPoints;
    qreal oldZoom;
};

class QGeoTiledMapDataPrivate : public QGeoMapDataPrivate
{
public:
    QGeoTiledMapDataPrivate(QGeoMappingManagerEngine *engine, QGeoMapWidget *widget, QGeoTiledMapData *q);
    QGeoTiledMapDataPrivate(const QGeoTiledMapDataPrivate &other);
    ~QGeoTiledMapDataPrivate();
    QGeoTiledMapDataPrivate& operator= (const QGeoTiledMapDataPrivate &other);

    bool intersects(QGeoMapObject *mapObject, const QRectF &rect);

    void updateScreenRect();

    int zoomFactor;

    QPoint maxZoomCenter;
    QSize maxZoomSize;
    QRect maxZoomScreenRect;

    QSet<QRect> requestRects;
    QSet<QRect> replyRects;

    QList<QGeoTiledMapRequest> requests;
    QSet<QGeoTiledMapReply*> replies;

    QCache<QGeoTiledMapRequest, QPixmap> cache;
    QCache<QGeoTiledMapRequest, QPixmap> zoomCache;
    QPixmap emptyTile;

    QGraphicsScene *scene;

    QHash<QGraphicsItem*, QGeoMapObject*> itemMap;

    QGeoTiledMapData* q_ptr;
    Q_DECLARE_PUBLIC(QGeoTiledMapData)

protected:
    QGeoMapObjectInfo* createRectangleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const;
    QGeoMapObjectInfo* createCircleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const;
    QGeoMapObjectInfo* createPolylineObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const;
    QGeoMapObjectInfo* createPolygonObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const;
    QGeoMapObjectInfo* createMarkerObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const;
    QGeoMapObjectInfo* createRouteObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate) const;
};

class QGeoTileIterator
{
public:
    QGeoTileIterator(const QGeoTiledMapDataPrivate *mapData);
    QGeoTileIterator(QGeoTiledMapData *mapData, const QRect &screenRect, const QSize &tileSize, int zoomLevel);

    bool hasNext();
    QGeoTiledMapRequest next();

private:
    QGeoTiledMapData *mapData;
    bool atEnd;
    int row;
    int col;
    QRect screenRect;
    QSize tileSize;
    int zoomLevel;
    QPoint currTopLeft;
    QRect tileRect;
};

QTM_END_NAMESPACE

#endif
