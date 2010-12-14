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

/*
typedef union {
    QRgb rgb;
    struct {
        unsigned int zoom:4;
        unsigned int px:10;
        unsigned int py:10;
    };
} TilePixelValue;
*/
class TilePixelValue {

public:
    TilePixelValue();
    TilePixelValue(QRgb rgb);
    TilePixelValue(unsigned int zoom, unsigned int px, unsigned int py);
    ~TilePixelValue();

    void setRgb(QRgb rgb);
    QRgb rgb() const;

    void setZoom(unsigned int zoom);
    unsigned int zoom() const;

    void setPx(unsigned int px);
    unsigned int px() const;

    void setPy(unsigned int py);
    unsigned int py() const;

private:
    unsigned int data_;
};

QPixmap indexedPixmap(int width, int height);

/*!
  A tiled map reply subclass that is constructed already finished, with
  a given QPixmap as its content (saved in PNG format).
  */
class PixmapTiledMapReply : public QGeoTiledMapReply
{
    Q_OBJECT
public:
    PixmapTiledMapReply(QPixmap &pixmap, const QGeoTiledMapRequest &request,
                        QObject *parent=0);

private:
    QPixmap m_pixmap;
};

/*!
  Tiled map engine that returns tiles made up of pixels with their color set
  to a TilePixelValue for that pixel's location and zoom level. Used for testing
  the map rendering code.
  */
class PixelIndexEngine : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT
public:
    PixelIndexEngine(const QMap<QString, QVariant> &parameters,
                     QObject *parent=0);

    QGeoTiledMapReply *getTileImage(const QGeoTiledMapRequest &request);

};

/*!
  Returns pure white tiles.
  */
class WhiteTileEngine : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT
public:
    WhiteTileEngine(const QMap<QString, QVariant> &parameters,
                    QObject *parent=0);

    QGeoTiledMapReply *getTileImage(const QGeoTiledMapRequest &request);
};

#endif // PIXELINDEXENGINE_H
