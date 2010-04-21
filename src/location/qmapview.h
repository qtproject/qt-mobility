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

#ifndef QLOCATION_MAPVIEW_H
#define QLOCATION_MAPVIEW_H

#include <QGraphicsWidget>
#include <QTimer>
#include <QPen>
#include <QSet>
#include <QHash>
#include <QLineF>
#include <QString>
#include <QFont>
#include <QColor>

#include "qgeocoordinate.h"
#include "qgeoroute.h"
#include "qmaptileservice_nokia_p.h"

QTM_BEGIN_NAMESPACE

class QMapRoute;
class QMapRect;
class QMapLine;
class QMapPixmap;
class QMapPolygon;
class QMapEllipse;
class QMapObject;
class QMapMarker;
class QMapViewPrivate;
class QMapTileService;
class QMapTileReply;

class Q_LOCATION_EXPORT QMapView : public QGraphicsWidget
{
    Q_OBJECT

public:
    class TileIteratorPrivate;
    class TileIterator
    {
    public:
        TileIterator(const QMapView& mapView, const QRectF& viewPort);
        ~TileIterator();

        bool hasNext() const;
        bool isValid() const;
        void next();
        quint32 col() const;
        quint32 row() const;
        quint64 index() const;
        QRectF tileRect() const;

    private:
        TileIteratorPrivate *d_ptr;

        Q_DECLARE_PRIVATE(TileIterator)
    };

    friend class TileIterator;

public:
    QMapView(QGraphicsItem* parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~QMapView();

    void init(QMapTileService* mapService, const QGeoCoordinate& center = QGeoCoordinate(0, 0));

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    void centerOn(const QPointF& pos);
    void centerOn(qreal x, qreal y);
    QPointF getViewportCenter() const;
    QGeoCoordinate getCenter() const;
    void setCenter(const QGeoCoordinate& coord);

    /*!
    * Returns the current heading.
    */
    quint16 getHeading() const;
    /*!
    * Sets heading.
    * @param heading The new heading.
    */
    void setHeading(quint16 heading);
    /*!
    * Returns the current tilt.
    */
    quint16 getTilt() const;
    /*!
    * Sets tilt.
    * @param tilt The new tilt.
    */
    void setTilt(quint16 tilt);
    /*!
    * Returns the map view bounds.
    */
    QRectF getViewBounds() const;
    
    quint16 maxZoomLevel() const;
    quint16 getZoomLevel() const;
    
    void pan(int deltaX, int deltaY);
    void pan(int dragX, int dragY, int dropX, int dropY);
    
    quint64 mapWidth() const;
    quint64 mapHeight() const;

    //setBaseMapType()
    
    /*!
    * Sets hotspot.
    * @param hotspot The new hotspot.
    */
    void setHotSpot(QPointF& hotspot);

    QPointF translateToViewport(const QGeoCoordinate& coord) const;
    QGeoCoordinate translateFromViewport(const QPointF& point) const;

    void setHorizontalPadding(quint32 range);
    quint32 horizontalPadding() const;
    void setVerticalPadding(quint32 range);
    quint32 verticalPadding() const;

    void setPannable(bool isPannable);
    bool isPannable() const;

    void setRouteDetailLevel(quint32 pixels);
    quint32 routeDetailLevel() const;

    void addMapObject(QMapObject* mapObject);
    void removeMapObject(QMapObject* mapObject);
    QMapObject* getTopmostMapObject(const QGeoCoordinate& geoCoordinate);
    QMapObject* getTopmostMapObject(const QPointF& mapCoordinate);

    /*!
    * Sets the map version.
    * @param mapVersion The new map version.
    */
    void setVersion(QMapTileServiceNokia::MapVersion mapVersion);
    /*!
    * Sets the map resolution.
    * @param mapResolution The new map resolution.
    */
    void setTileSize(QMapTileServiceNokia::TileSize tileSize);
    /*!
    * Sets the map format.
    * @param mapVersion The new map format.
    */
    void setFormat(QMapTileServiceNokia::MapFormat mapFormat);
    void setScheme(QMapTileServiceNokia::MapScheme scheme);

private:
    Q_DISABLE_COPY(QMapView)

    QPointF mercatorToMap(const QPointF& mercatorCoordinate) const;
    QPointF mapToMercator(const QPointF&mapCoordinate) const;

    void mapToTile(const QPointF& mapCoordinate, quint32* col, quint32* row) const;
    QRectF getTileRect(quint32 col, quint32 row) const;
    quint64 getTileIndex(quint32 col, quint32 row) const;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    virtual void resizeEvent(QGraphicsSceneResizeEvent* event);

public slots:
    void releaseRemoteTiles();
    void tileFetched(QMapTileReply* reply);
    void setZoomLevel(int level);

signals:
    void mapClicked(QGeoCoordinate geoCoord, QGraphicsSceneMouseEvent* mouseEvent);
    void zoomLevelChanged(quint16 oldZoomLevel, quint16 newZoomLevel);
    void centerChanged();
    void mapObjectSelected(QMapObject* mapObject);

protected:
    QMapViewPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QMapView)
};

QTM_END_NAMESPACE

#endif
