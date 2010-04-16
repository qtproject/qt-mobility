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

#ifndef QLOCATION_MAPVIEW_P_H
#define QLOCATION_MAPVIEW_P_H

#include <QRectF>
#include <QTimer>
#include <QSet>
#include <QHash>

#include "qmapview.h"
#include "qmaptilereply.h"

QTM_BEGIN_NAMESPACE

class QMapTileService;

class QMapViewPrivate
{
public:
    QMapViewPrivate(QMapView* q);

    QMapView* q_ptr;
    Q_DECLARE_PUBLIC(QMapView)

    QHash<quint64, QPair<QPixmap, bool> > preZoomIn(qreal scale);
    QHash<quint64, QPair<QPixmap, bool> > preZoomOut(qreal scale);

    void requestTile(quint32 col, quint32 row);
    void cancelPendingTiles();

    void paintLayers(QPainter* painter);
    void addMapObjectToTiles(QMapObject* mapObject);
    void requestMissingMapTiles();

    QRectF viewPort; //!< The logical view port.
    quint32 numColRow; //!< The number of tiles along both the x- and y-axis
    QMapTileService* mapService; //!< the underlying map service
    quint32 horizontalPadding; //!< horizontal preload padding
    quint32 verticalPadding; //!< vertical preload padding
    quint32 routeDetails; //!< Minimum manhattan distance betwee two consecutive visible route way points.

    /*! The period timer that triggers the removal
    * of scene items that are not within the current
    * map view port.
    */
    QTimer releaseTimer; //!< Periodic release timer

    quint16 currZoomLevel; //!< The current zoom level
    QMapTileServiceNokia::MapVersion mapVersion; //!< The current map version
    QMapTileServiceNokia::MapScheme mapSchmeme; //!< The current map scheme
    QMapTileServiceNokia::TileSize tileSize; //!< The current map resolution
    QMapTileServiceNokia::MapFormat mapFormat; //!< The current map format

    QSet<QMapObject*> mapObjects; //!< Keeps track of all map objects.
    /*!
    * Stores for each map tile (as specified by its one-dimensional index)
    * a list of all map objects that intersect or are completely covered by
    * the map tile.
    */
    QHash<quint64, QList<QMapObject*> > tileToObjects; //!< Map tile to map object hash map.

    /*!
    * Stores for each requested map tile (as given by its
    * one-dimensional tile index) the corresonding QMapTileReply.
    */
    QHash<quint64, QMapTileReply*> pendingTiles; //!< Pending requested map tiles

    QHash<quint64, QPair<QPixmap, bool> > mapTiles;

    bool panActive;
    bool pannable;
};

QTM_END_NAMESPACE

#endif
