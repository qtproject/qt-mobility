#ifndef PIXELINDEXENGINE_H
#define PIXELINDEXENGINE_H

#include <QtGui/QApplication>
#include <QDebug>
#include <QtGui>

#include <qgeocoordinate.h>
#include <qgraphicsgeomap.h>
#include <qgeomapdata.h>
#include <qgeotiledmapdata.h>
#include <qgeotiledmaprequest.h>
#include <qgeotiledmapreply.h>
#include <qgeotiledmappingmanagerengine.h>

QTM_USE_NAMESPACE

typedef union {
    QRgb rgb;
    struct {
        unsigned zoom:4;
        unsigned px:10;
        unsigned py:10;
    };
} TilePixelValue;

class PixmapTiledMapReply : public QGeoTiledMapReply
{
    Q_OBJECT
public:
    PixmapTiledMapReply(QPixmap &pixmap, const QGeoTiledMapRequest &request, QObject *parent=0);

private:
    QPixmap m_pixmap;
};

class PixelIndexEngine : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT
public:
    PixelIndexEngine(const QMap<QString, QVariant> &parameters, QObject *parent=0);
    QGeoTiledMapReply *getTileImage(const QGeoTiledMapRequest &request);

signals:

public slots:

};

#endif // PIXELINDEXENGINE_H
