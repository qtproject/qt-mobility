/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "pixelindexengine.h"
#include <QBuffer>
#include <QByteArray>

TilePixelValue::TilePixelValue()
    : data_(0) {}

TilePixelValue::TilePixelValue(QRgb rgb)
    : data_(0)
{
    setRgb(rgb);
}

TilePixelValue::TilePixelValue(unsigned int zoom, unsigned int px, unsigned int py)
    : data_(0)
{
    setZoom(zoom);
    setPx(px);
    setPy(py);
}

TilePixelValue::~TilePixelValue() {}

void TilePixelValue::setRgb(QRgb rgb)
{
    data_ = static_cast<unsigned int>(rgb);
}

QRgb TilePixelValue::rgb() const
{
    return static_cast<QRgb>(data_);
}

void TilePixelValue::setZoom(unsigned int zoom)
{
    data_ = (data_ & 0xFFFFFFF0) + (zoom & 0x0000000F);
}

unsigned int TilePixelValue::zoom() const
{
    return (data_ & 0x0000000F);
}

void TilePixelValue::setPx(unsigned int px)
{
    data_ = (data_ & 0xFFFFC00F) + ((px & 0x000003FF) << 4);
}

unsigned int TilePixelValue::px() const
{
    return (data_ & 0x00003FF0) >> 4;
}

void TilePixelValue::setPy(unsigned int py)
{
    data_ = (data_ & 0xFF003FFF) + ((py & 0x000003FF) << 14);
}

unsigned int TilePixelValue::py() const
{
    return (data_ & 0x00FFC000) >> 14;
}

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

WhiteTileEngine::WhiteTileEngine(const QMap<QString, QVariant> &parameters, QObject *parent) :
    QGeoTiledMappingManagerEngine(parameters, parent)
{
    setTileSize(QSize(128,128));
    setMinimumZoomLevel(0.0);
    setMaximumZoomLevel(3.0);

    QList<QGraphicsGeoMap::MapType> types;
    types << QGraphicsGeoMap::StreetMap;
    types << QGraphicsGeoMap::TerrainMap;
    setSupportedMapTypes(types);

    QList<QGraphicsGeoMap::ConnectivityMode> modes;
    modes << QGraphicsGeoMap::OfflineMode;
    setSupportedConnectivityModes(modes);
}

QPixmap indexedPixmap(int width, int height)
{
    QImage im(width, height, QImage::Format_RGB888);
    im.fill(0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            TilePixelValue tpv;
            tpv.setPy(y);
            tpv.setPx(x);
            tpv.setZoom(1);
            im.setPixel(x, y, tpv.rgb());
        }
    }
    return QPixmap::fromImage(im);
}

QGeoTiledMapReply *PixelIndexEngine::getTileImage(const QGeoTiledMapRequest &request)
{
    QImage im(128, 128, QImage::Format_RGB888);
    im.fill(0);

    for (int y = 0; y < im.height(); y++) {
        for (int x = 0; x < im.width(); x++) {
            TilePixelValue tpv;
            tpv.setPy(request.row()*128 + y);
            tpv.setPx(request.column()*128 + x);
            tpv.setZoom(request.zoomLevel());
            im.setPixel(x, y, tpv.rgb());
        }
    }

    QPixmap pm = QPixmap::fromImage(im);
    return new PixmapTiledMapReply(pm, request);
}

QGeoTiledMapReply *WhiteTileEngine::getTileImage(const QGeoTiledMapRequest &request)
{
    QImage im(128, 128, QImage::Format_RGB888);
    im.fill(0xffffffff);

    QPixmap pm = QPixmap::fromImage(im);
    return new PixmapTiledMapReply(pm, request);
}
