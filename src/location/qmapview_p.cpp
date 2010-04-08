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

#include <QPainter>

#include "qmapview_p.h"
#include "qmapobject.h"
#include "qmapobject_p.h"
#include "qgeomapservice.h"

QTM_BEGIN_NAMESPACE

QMapViewPrivate::QMapViewPrivate(QtMobility::QMapView *q)
        : q_ptr(q), numColRow(1), mapService(0), currZoomLevel(0),
        horizontalPadding(0), verticalPadding(0), routeDetails(0),
        panActive(false), pannable(true), releaseTimer(q),
        mapVersion(MapVersion(MapVersion::Newest)),
        mapSchmeme(MapScheme(MapScheme::Normal_Day)),
        mapResolution(MapResolution(MapResolution::Res_256_256)),
        mapFormat(MapFormat(MapFormat::Png))
{
}

/*!
    When the zoom level is increased, this method is invoked to scale and add the
    map tiles in the view port of the old map scene to cover (as much as possible)
    of the view port of the new zoom level map scene while the new map tiles
    are being requested.
    It returns the scaled map tiles.
*/
QHash<quint64, QPair<QPixmap, bool> > QMapViewPrivate::preZoomIn(qreal scale)
{
    Q_Q(QMapView);
    QHash<quint64, QPair<QPixmap, bool> > scaledTiles;
    QMapView::TileIterator it(*q, viewPort);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint64 index = q->getTileIndex(it.col(), it.row());

        if (mapTiles.contains(index)) {
            QPixmap& pixmap = mapTiles[index].first;
            QPixmap scaledPixmap = pixmap.scaled(mapResolution.size * scale);

            for (int i = 0; i < scale; i++)

                for (int j = 0; j < scale; j++) {
                    QPixmap scaledTile = scaledPixmap.copy(i * mapResolution.size.width(),
                                                           j * mapResolution.size.height(),
                                                           mapResolution.size.width(),
                                                           mapResolution.size.height()
                                                          );
                    quint32 scaledCol = it.col() * scale + i;
                    quint32 scaledRow = it.row() * scale + j;
                    quint64 scaledIndex = ((quint64) scaledRow) * numColRow * scale + scaledCol;
                    scaledTiles[scaledIndex] = qMakePair(scaledTile, false);
                }
        }
    }

    return scaledTiles;
}

/*!
    When the zoom level is decreased, this method is invoked to scale and add the
    map tiles in the view port of the old map scene to cover (as much as possible)
    of the view port of the new zoom level map scene while the new map tiles
    are being requested.
    It returns the scaled map tiles.
*/
QHash<quint64, QPair<QPixmap, bool> > QMapViewPrivate::preZoomOut(qreal scale)
{
    Q_Q(QMapView);
    QHash<quint64, QImage> combinedTiles;
    quint32 reverseScale = (quint32)(1.0 / scale);
    QMapView::TileIterator it(*q, viewPort);

    while (it.hasNext()) {
        it.next();

        if (!it.isValid())
            continue;

        quint32 scaledRow = it.row() / reverseScale;
        quint32 scaledCol = it.col() / reverseScale;
        quint64 scaledIndex = ((quint64) scaledRow) * (numColRow / reverseScale) + scaledCol;

        if (!combinedTiles.contains(scaledIndex)) {
            combinedTiles[scaledIndex] = QImage(mapResolution.size, QImage::Format_ARGB32_Premultiplied);
        }

        QImage& combinedTile = combinedTiles[scaledIndex];
        QPainter painter(&combinedTile);
        quint64 index = q->getTileIndex(it.col(), it.row());

        if (mapTiles.contains(index)) {
            QPixmap& pixmap = mapTiles[index].first;
            QPixmap scaledPixmap = pixmap.scaled(mapResolution.size * scale);
            painter.drawPixmap(mapResolution.size.width() * scale * (it.col() % reverseScale),
                               mapResolution.size.height() * scale * (it.row() % reverseScale),
                               scaledPixmap);
        }
    }

    QHash<quint64, QPair<QPixmap, bool> > scaledTiles;
    QHashIterator<quint64, QImage> combinedIt(combinedTiles);

    while (combinedIt.hasNext()) {
        combinedIt.next();
        scaledTiles[combinedIt.key()] = qMakePair(QPixmap::fromImage(combinedIt.value()), false);
    }

    return scaledTiles;
}

/*!
    Requests a map tile from the underlying geo engine for the given
    \a col and \a row index using the current zoom level and map configuration.
*/
void QMapViewPrivate::requestTile(quint32 col, quint32 row)
{
    if (!mapService)
        return;

    Q_Q(QMapView);
    quint64 tileIndex = q->getTileIndex(col, row);

    if (pendingTiles.contains(tileIndex)) {
        return;
    }

    QGeoMapTileRequest request;
    request.setVersion(mapVersion);
    request.setScheme(mapSchmeme);
    request.setResolution(mapResolution);
    request.setFormat(mapFormat);
    request.setCol(col);
    request.setRow(row);
    request.setZoomLevel(currZoomLevel);

    QGeoMapTileReply* reply = mapService->getMapTile(request);
    pendingTiles[tileIndex] = reply;
}

/*!
    Cancels all pending tile replies.
*/
void QMapViewPrivate::cancelPendingTiles()
{
    QHashIterator<quint64, QGeoMapTileReply*> it(pendingTiles);

    while (it.hasNext()) {
        it.next();
        it.value()->cancel();
        it.value()->deleteLater();
    }

    pendingTiles.clear();
}

/*!
    Paints all map objects that are covered by the current view port.
*/
void QMapViewPrivate::paintLayers(QPainter* painter)
{
    Q_Q(QMapView);
    QMapView::TileIterator it(*q, viewPort);
    QMap<quint16, QSet<QMapObject*> > stackedObj;

    while (it.hasNext()) {
        it.next();
        quint64 tileIndex = q->getTileIndex(it.col(), it.row());

        if (tileToObjects.contains(tileIndex)) {
            QList<QMapObject*>& mapObjects = tileToObjects[tileIndex];

            for (int i = 0; i < mapObjects.size(); i++) {
                QMapObject* obj = mapObjects[i];

                if (!stackedObj.contains(obj->zValue()))
                    stackedObj[obj->zValue()] = QSet<QMapObject*>();

                if (!stackedObj[obj->zValue()].contains(obj))
                    stackedObj[obj->zValue()].insert(obj);
            }
        }
    }

    QMapIterator<quint16, QSet<QMapObject*> > mit(stackedObj);

    while (mit.hasNext()) {
        mit.next();
        QSetIterator<QMapObject*> sit(mit.value());

        while (sit.hasNext()) {
            QMapObject *obj = sit.next();
            obj->paint(painter, viewPort);

            qint64 mapWidth = q->mapWidth();
            //Is view port wrapping around date line?
            qreal right = viewPort.right();
            for(int i=1; right >= mapWidth;++i, right -= mapWidth) {
                painter->save();
                painter->translate(mapWidth *i, 0);
                obj->paint(painter, viewPort);
                painter->restore();

                //TODO: this logic is incorrect
                // we need to handle the case where an object crosses the dateline
                // and our viewport does not but we should still see the wrapped around
                // object
                if (i == 1) {
                    painter->save();
                    painter->translate(-mapWidth, 0);
                    obj->paint(painter,viewPort);
                    painter->restore();
                }
            }
        }
    }
}

/*!
    Adds an entry to tileToObjects for each tile that
    intersects or completely covers he given \a mapObject.
*/
void QMapViewPrivate::addMapObjectToTiles(QMapObject* mapObject)
{
    for (int i = 0; i < mapObject->d_ptr->intersectingTiles.count(); i++) {
        if (!tileToObjects.contains(mapObject->d_ptr->intersectingTiles[i]))
            tileToObjects[mapObject->d_ptr->intersectingTiles[i]] = QList<QMapObject*>();

        tileToObjects[mapObject->d_ptr->intersectingTiles[i]].append(mapObject);
    }
}

QTM_END_NAMESPACE
