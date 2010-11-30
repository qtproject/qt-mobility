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

#include "pixelindexengine.h"
#include <QBuffer>
#include <QByteArray>

PixmapTiledMapReply::PixmapTiledMapReply(QPixmap &pixmap, const QGeoTiledMapRequest &request, QObject *parent) :
    QGeoTiledMapReply(request, parent),
    m_pixmap(pixmap)
{
    QByteArray ba;
    QBuffer buff(&ba);
    buff.open(QIODevice::WriteOnly);
    pixmap.save(&buff, "PNG");

    setMapImageData(ba);
    setMapImageFormat("PNG");
    setFinished(true);
}

PixelIndexEngine::PixelIndexEngine(const QMap<QString, QVariant> &parameters, QObject *parent) :
    QGeoTiledMappingManagerEngine(parameters, parent)
{
    // might want to parametrise these for future use
    setTileSize(QSize(128,128));
    setMinimumZoomLevel(0.00);
    setMaximumZoomLevel(3.0);

    QList<QGraphicsGeoMap::MapType> types;
    types << QGraphicsGeoMap::StreetMap;
    types << QGraphicsGeoMap::TerrainMap;
    setSupportedMapTypes(types);

    QList<QGraphicsGeoMap::ConnectivityMode> modes;
    modes << QGraphicsGeoMap::OfflineMode;
    setSupportedConnectivityModes(modes);
}

QGeoTiledMapReply *PixelIndexEngine::getTileImage(const QGeoTiledMapRequest &request)
{
    QImage im(128, 128, QImage::Format_RGB888);
    im.fill(0);

    for (int y = 0; y < im.height(); y++) {
        for (int x = 0; x < im.width(); x++) {
            TilePixelValue tpv;
            tpv.py = request.row()*128 + y;
            tpv.px = request.column()*128 + x;
            tpv.zoom = request.zoomLevel();

            im.setPixel(x, y, tpv.rgb);
        }
    }

    QPixmap pm = QPixmap::fromImage(im);
    return new PixmapTiledMapReply(pm, request);
}
