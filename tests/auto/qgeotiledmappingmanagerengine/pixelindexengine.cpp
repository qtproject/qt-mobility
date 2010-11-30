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
