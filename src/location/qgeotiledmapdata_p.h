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
#include <QMultiMap>
#include <QString>
#include <QPainterPath>

QTM_BEGIN_NAMESPACE

class QGeoMapRectangleObject;
class QGeoMapMarkerObject;
class QGeoMapPolylineObject;
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

    bool intersects(QGeoMapObject *mapObject, const QRectF &rect) const;

    void paintMapObject(QPainter &painter, QGeoMapObject *mapObject);
    void paintMapRectangle(QPainter &painter, QGeoMapRectangleObject *rectangle);
    void paintMapMarker(QPainter &painter, QGeoMapMarkerObject *marker);
    void paintMapPolyline(QPainter &painter, QGeoMapPolylineObject *polyline);

    void clearObjInfo();

    qulonglong width;
    qulonglong height;

    QRectF protectRegion;
    QRectF screenRect;

    QSet<QRectF> requestRects;
    QSet<QRectF> replyRects;

    QList<QGeoTiledMapRequest> requests;
    QSet<QGeoTiledMapReply*> replies;

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
