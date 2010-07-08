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

#include "qmobilityglobal.h"
#include "qgeomapobject.h"

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

class QGeoTiledMapDataPrivate
{
public:
    QGeoTiledMapDataPrivate(QGeoTiledMapData *q);
    QGeoTiledMapDataPrivate(const QGeoTiledMapDataPrivate &other);
    ~QGeoTiledMapDataPrivate();
    QGeoTiledMapDataPrivate& operator= (const QGeoTiledMapDataPrivate &other);

    static qulonglong tileKey(int row, int col, int zoomLevel);

    void calculateInfo(QGeoMapObject *mapObject);
    void calculateMapRectangleInfo(QGeoMapRectangleObject *rectangle);
    void calculateMapMarkerInfo(QGeoMapMarkerObject *marker);
    void calculateMapPolylineInfo(QGeoMapPolylineObject *polyline);
    void calculateMapRouteInfo(QGeoMapRouteObject *route);
    void addRouteSegmentInfo(QGeoTiledMapRouteInfo *info, const QLineF &line, int index) const;

    bool intersects(QGeoMapObject *mapObject, const QRectF &rect);

    void paintMapObject(QPainter &painter, const QRectF &viewPort, QGeoMapObject *mapObject, bool hitDetection = false);
    void paintMapRectangle(QPainter &painter, const QRectF &viewPort, QGeoMapRectangleObject *rectangle, bool hitDetection);
    void paintMapMarker(QPainter &painter, const QRectF &viewPort, QGeoMapMarkerObject *marker, bool hitDetection);
    void paintMapPolyline(QPainter &painter, const QRectF &viewPort, QGeoMapPolylineObject *polyline, bool hitDetection);
    void paintMapRoute(QPainter &painter, const QRectF &viewPort, QGeoMapRouteObject *route, bool hitDetection);

    void clearObjInfo();

    QLineF connectShortest(const QGeoCoordinate& point1, const QGeoCoordinate& point2) const;

    qulonglong width;
    qulonglong height;

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
