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
#include <QMultiMap>
#include <QString>
#include <QPainterPath>
#include <QPair>
#include <QList>
#include <QLineF>

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

struct QGeoTiledMapObjectInfo
{
    QRectF boundingBox;
};

struct QGeoTiledMapPolylineInfo : public QGeoTiledMapObjectInfo
{
    QPainterPath path;
};

struct QGeoTiledMapRouteInfo : public QGeoTiledMapObjectInfo
{
    QHash<qulonglong, QList< QPair<int, QLineF> > > intersectedTiles;
};

class QGeoTiledMapRectangleObjectInfo : public QGeoMapObjectInfo
{
public:
    QGeoTiledMapRectangleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapRectangleObjectInfo();

    void paint(QPainter *painter, const QRectF &viewPort, bool hitDetection);
    void update();

    const QGeoMapRectangleObjectPrivate* rectangle;
    QSharedPointer<QGeoTiledMapDataPrivate> mapData;
};

class QGeoTiledMapCircleObjectInfo : public QGeoMapObjectInfo
{
public:
    QGeoTiledMapCircleObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapCircleObjectInfo();

    void paint(QPainter *painter, const QRectF &viewPort, bool hitDetection);
    void update();

    const QGeoMapCircleObjectPrivate* circle;
    QSharedPointer<QGeoTiledMapDataPrivate> mapData;
};

class QGeoTiledMapPolylineObjectInfo : public QGeoMapObjectInfo
{
public:
    QGeoTiledMapPolylineObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapPolylineObjectInfo();

    void paint(QPainter *painter, const QRectF &viewPort, bool hitDetection);
    void update();

    const QGeoMapPolylineObjectPrivate* polyline;
    QSharedPointer<QGeoTiledMapDataPrivate> mapData;

    QList<QPoint> points;
};

class QGeoTiledMapPolygonObjectInfo : public QGeoMapObjectInfo
{
public:
    QGeoTiledMapPolygonObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapPolygonObjectInfo();

    void paint(QPainter *painter, const QRectF &viewPort, bool hitDetection);
    void update();

    const QGeoMapPolygonObjectPrivate* polygon;
    QSharedPointer<QGeoTiledMapDataPrivate> mapData;

    QList<QPoint> points;
};

class QGeoTiledMapMarkerObjectInfo : public QGeoMapObjectInfo
{
public:
    QGeoTiledMapMarkerObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapMarkerObjectInfo();

    void paint(QPainter *painter, const QRectF &viewPort, bool hitDetection);
    void update();

    const QGeoMapMarkerObjectPrivate* marker;
    QSharedPointer<QGeoTiledMapDataPrivate> mapData;
};

class QGeoTiledMapRouteObjectInfo : public QGeoMapObjectInfo
{
public:
    QGeoTiledMapRouteObjectInfo(const QGeoMapObjectPrivate *mapObjectPrivate);
    ~QGeoTiledMapRouteObjectInfo();

    void paint(QPainter *painter, const QRectF &viewPort, bool hitDetection);
    void update();

    qulonglong tileKey(int row, int col, int zoomLevel) const;
    QLineF connectShortest(const QGeoCoordinate &point1, const QGeoCoordinate &point2) const;
    void addRouteSegmentInfo(const QLineF &line, int index);

    const QGeoMapRouteObjectPrivate* route;
    QSharedPointer<QGeoTiledMapDataPrivate> mapData;

    QHash<qulonglong, QList< QPair<int, QLineF> > > intersectedTiles;
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

    QPoint maxZoomCenter;
    QSize maxZoomSize;

    QRectF screenRect;

    QSet<QRectF> requestRects;
    QSet<QRectF> replyRects;

    QList<QGeoTiledMapRequest> requests;
    QSet<QGeoTiledMapReply*> replies;

    QCache<QGeoTiledMapRequest, QPixmap> cache;
    QCache<QGeoTiledMapRequest, QPixmap> zoomCache;
    QPixmap emptyTile;

    QHash<QGeoMapObject*, QGeoTiledMapObjectInfo*> objInfo;

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
    QGeoTileIterator(const QRectF &screenRect, const QSize &tileSize, int zoomLevel);

    bool hasNext();
    void next();
    int row() const;
    int col() const;
    int zoomLevel() const;
    QRectF tileRect() const;

private:
    bool aHasNext;
    int aRow;
    int aCol;
    QRectF aScreenRect;
    QSize aTileSize;
    int aZoomLevel;
    QPointF currTopLeft;
    QRectF aTileRect;
};

QTM_END_NAMESPACE

#endif
