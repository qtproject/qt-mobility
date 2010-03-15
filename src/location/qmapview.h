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
#include "qgeoengine.h"
#include "qroute.h"

QTM_BEGIN_NAMESPACE

class QMapRoute;
class QMapRect;
class QMapLine;
class QMapPixmap;
class QMapPolygon;
class QMapEllipse;
class QMapObject;
class QMapMarker;
/*!
* This class displays a map. The map is a 2D Mercator projection of geo coordinates.
* The map allows seamless continual panning along the x-axis.
* The map is logically 2^zoomLevel * width(map tile) pixels wide, and
* 2^zoomLevel * height(map tile) high, with the top left corner being (0,0).
* This logical 2D map space is spit equally into 2^zoomLevel map tiles along each axis.
* The QMapView provides a view port onto that logical map space that is always
* QMapView::width() wide and QMapView::height() high.
* In order not to pollute the internal Qt::QGraphicsScene, the QMapView takes care
* of dynamically adding the required graphics items to the graphics scene that are
* currently covered by the view port, and of releasing no longer visible items.
* This is necessary because a \ref QRoute e.g. can consists of 10,000+ route segments
* that would, if all added to the graphics scene, make the underlying Qt::QGraphicsView
* essentially grind to a halt.
* Instead of using a full-blown quad tree, QMapView exploits the fact that the map is made
* up of map tiles. Internally, it uses efficient hash maps (\ref tileToObjectsMap,
* \ref itemToObjectMap) to map internal \ref QMapObject<i></i>s to their corresponding tiles. These
* maps are then used to determine which map objects need to be shown in the current
* view port.
*/
class Q_LOCATION_EXPORT QMapView : public QGraphicsWidget
{
    Q_OBJECT

public:
    /*!
    * This class can be used to iterate through all map tiles that are
    * covered by a specified view port. The iteration goes row by row
    * (top-down), with each row being walked from left to right.
    */
    class TileIterator
    {
    public:
        /*!
        * Constructor.
        * @param mapView The associated QMapView object
        * @param viewPort The view port.
        */
        TileIterator(const QMapView& mapView, const QRectF& viewPort);
        /*!
        * @return True (at least one more tile is available), False (last tile has been reached)
        */
        inline bool hasNext() const {
            return hNext;
        }
        /*!
        * @return Indicates whether current tile is valid,
        *         invalid tile occur beyond north and south pole.
        */
        inline bool isValid() const {
            return valid;
        }
        /*!
        * Moves iterator to next tile.
        */
        void next();
        /*!
        * @return The column index of the current map tile.
        */
        inline quint32 col() const {
            return cl;
        }
        /*!
        * @return The row index of the current map tile.
        */
        inline quint32 row() const {
            return rw;
        }
        /*!
        * @return The bounding box of the map tile (in map pixel coordinates).
        */
        inline QRectF tileRect() const {
            return rect;
        }

    private:
        quint32 cl;
        quint32 rw;
        bool hNext;
        QRectF viewPort;
        quint64 numColRow;
        MapResolution mapRes;
        qint64 currX;
        qint64 currY;
        QRectF rect;
        bool valid;
    };

public:
    QMapView(QGraphicsItem* parent = 0, Qt::WindowFlags wFlags = 0);

    void init(QGeoEngine* geoEngine, const QGeoCoordinate& center = QGeoCoordinate(0, 0));

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    QGeoCoordinate center() const;
    QPointF mapCenter() const;
    void centerOn(const QPointF& pos);
    void centerOn(qreal x, qreal y);
    void centerOn(const QGeoCoordinate& geoPos);
    void moveViewPort(int deltaX, int deltaY);

    /*!
    * @return The width of the logical map space in pixels.
              I.e. the pixel range from 180W to 180E.
    */
    inline quint64 mapWidth() const {
        return numColRow * mapRes.size.width();
    }
    /*!
    * @return The height of the logical map space in pixels.
              I.e. the pixel range from 90N to 90S.
    */
    inline quint64 mapHeight() const {
        return numColRow * mapRes.size.height();
    }

    /*!
    * This method sets the horizontal range beyond the immediate limits of
    * the view port, for which map tiles should also be loaded. This can make
    * for a smoother panning experience.
    * @param range The horizontal range (in pixels) beyond the view port limits.
    */
    void setHorizontalPadding(quint32 range);
    /*!
    * @return The horizontal range beyond the immediate limits of
    *         the view port, for which map tiles should also be loaded.
    */
    inline quint32 horizontalPadding() const {
        return horPadding;
    }
    /*!
    * This method sets the vertical range beyond the immediate limits of
    * the view port, for which map tiles should also be loaded. This can make
    * for a smoother panning experience.
    * @param range The horizontal range (in pixels) beyond the view port limits.
    */
    void setVerticalPadding(quint32 range);
    /*!
    * @return The vertical range beyond the immediate limits of
    *         the view port, for which map tiles should also be loaded.
    */
    inline quint32 verticalPadding() const {
        return verPadding;
    }

    void setPannable(bool isPannable);
    /*!
     * Converts a geo coordinate to a map coordinate (in pixels).
     * @param geoCoord A geo coordinate.
     * @return The corresponding map coordinate (in pixels).
     */
    QPointF geoToMap(const QGeoCoordinate& geoCoord) const;
    /*!
     * Converts a normalized Mercator coordinate to a map coordinate.
     * @param mercatorCoord A normalized Mercator coordinate.
     * @return The corresponding map coordinate (in pixels).
     */
    QPointF mercatorToMap(const QPointF& mercatorCoord) const;
    /*!
    * Converts a map coordinate (in pixels) into its corresponding geo coordinate.
    * @param mapCoord The map coordinate (in pixels).
    */
    QGeoCoordinate mapToGeo(const QPointF& mapCoord) const;
    /*!
    * To compute the normalized mercator coordinate for the given pixel coordinate
    * according to the map.
    * @param coord A pixel coordinate inside the map.
    * @return The computed normalized Mercator coordinate.
    */
    QPointF mapToMercator(const QPointF&mapCoord) const;
    /*!
    * Determines the col and row index of the tile that the given map coordinate
    * (in pixels) lies on.
    * @param mapCoord The map coordinate in pixels.
    * @param col A pointer to the buffer that will take the calculated col index.
    * @param row A pointer to the buffer that will take the calculated row index.
    */
    void mapToTile(const QPointF& mapCoord, quint32* col, quint32* row) const;
    /*!
    * @param col The col index of a map tile.
    * @param row The row index of a map tile.
    * @return The bounding box of the map tile given by its col and row index
    *         for the current zoom level and map configuration.
    */
    QRectF getTileRect(quint32 col, quint32 row) const;

    /*!
    * A route can consist of 10,000+ individual legs. Drawing them all into the
    * view port (e.g. when the map is zoomed to a continent scale) can be prhobitively
    * expensive. Therefore, the map view will suppress some route legs according to the
    * current zoom level.<br>
    * This method sets the minimum manhattan distance in pixels between two consecutive visible route way points.
    * In other words, specifying <i>0</i> here, will force the map view to always draw all individual
    * route legs that are in the current view port. Higher values will allow for much faster rendering,
    * especially at far out zoom levels, but the route may appear less smooth.<br>
    * The default value is 20 pixels. At QGeoEngine::maxZoomLevel(), all route legs are always shown.
    * @param pixels The minum manhattan distance between two consecutive visible route way points.
    */
    void setRouteDetailLevel(quint32 pixels);
    /*!
    * \see setRouteDetailLevel()
    * @return The minum manhattan distance between two consecutive visible route way points.
    */
    inline quint32 routeDetailLevel() const {
        return routeDetails;
    }

    void addMapObject(QMapObject* mapObject);
    void removeMapObject(QMapObject* mapObject);

    /*!
    * @return The maximum zoom level.
    */
    inline quint16 maxZoomLevel() const {
        if (geoEngine)
            return geoEngine->maxZoomLevel();
        return 0;
    }
    /*!
    * @return The current zoom level.
    */
    inline quint16 zoomLevel() const {
        return currZoomLevel;
    }
    /*!
    * Map a two-dimensional map tile index onto a one-dimensional one.
    * @param col The map tile col index.
    * @param row The map tile row index.
    */

    inline quint64 getTileIndex(quint32 col, quint32 row) const {
        return ((quint64) row) * numColRow + col;
    }
    QLineF connectShortest(const QGeoCoordinate& point1, const QGeoCoordinate& point2) const;

    /*!
    * @return The current map version.
    */
    inline MapVersion version() const {
        return mapVer;
    }
    /*!
    * Sets the map version.
    * @param mapVersion The new map version.
    */
    void setVersion(const MapVersion& mapVersion);
    /*!
    * @return The current map resolution.
    */
    inline MapResolution resolution() const {
        return mapRes;
    }
    /*!
    * Sets the map resolution.
    * @param mapResolution The new map resolution.
    */
    void setResolution(const MapResolution& mapResolution);
    /*!
    * @return The current map format.
    */
    inline MapFormat format() const {
        return mapFrmt;
    }
    /*!
    * Sets the map format.
    * @param mapVersion The new map format.
    */
    void setFormat(const MapFormat& mapFormat);
    /*!
    * @return The current map scheme.
    */
    inline MapScheme scheme() const {
        return mapSchm;
    }
    /*!
    * Sets the map scheme.
    * @param mapVersion The new map scheme.
    */
    void setScheme(const MapScheme& mapScheme);

private:
    Q_DISABLE_COPY(QMapView)

protected:

    /*!
    * Requests a map tile from the underlying geo engine for the given
    * col and row index using the current zoom level and map configuration.
    * @param col The col index of the requested map tile.
    * @param row The row index of the requested map tile.
    */
    void requestTile(quint32 col, quint32 row);
    /*!
    * Cancels all pending tile replies.
    */
    void cancelPendingTiles();
    /*!
    * When the zoom level is increased, this method is invoked to scale and add the
    * map tiles in the view port of the old map scene to cover (as much as possible)
    * of the view port of the new zoom level map scene
    * while the new map tiles are being requested.
    * @return The scaled map tiles.
    */
    QHash<quint64, QPair<QPixmap, bool> > preZoomIn(qreal scale);
    /*!
    * When the zoom level is decreased, this method is invoked to scale and add the
    * map tiles in the view port of the old map scene to cover (as much as possible)
    * of the view port of the new zoom level map scene
    * while the new map tiles are being requested.
    * @return The scaled map tiles.
    */
    QHash<quint64, QPair<QPixmap, bool> > preZoomOut(qreal scale);

    /*!
    * @param center The map coordinate (in pixels) of a view port.
    * @return The map coordinate (in pixels) of the top left corner of a view port
    *         as specified by its <i>center</i>.
    */
    QPointF getTopLeftFromCenter(const QPointF& center);

    void paintLayers(QPainter* painter);


    /*!
    * Determines all map tiles that intersect or completely cover
    * the given map object.
    * @param mapObject The map object.
    */
    //void getIntersectingTiles(QMapObject* mapObject);


    /*!
    * Adds an entry to the \ref tileToObjectsMap for each tile that
    * intersects or completely covers he given map object.
    * @param mapObject The map object.
    */
    void addMapObjectToTiles(QMapObject* mapObject);

public slots:
    /*!
    * This slot is connected to \ref releaseTimer timeout().
    * It releases all map tiles that are not currently covered by the view port.
    */
    void releaseRemoteTiles();
    /*!
    * This slot is connected to \ref QGeoEngine::finished(QMapTileReply*).
    * @param reply The map tile reply.
    */
    void tileFetched(QMapTileReply* reply);
    /*!
    * Sets the map's current zoom level.
    * @param zoomLevel The new zoom level.
    */
    void setZoomLevel(int zoomLevel);

signals:
    /*!
    * This signal is emitted when the map receieves a mouse click event.
    * @param geoCoord The geo coordinate where the click occured.
    * @param mouseEvent The associated mouse event.
    */
    void mapClicked(QGeoCoordinate geoCoord, QGraphicsSceneMouseEvent* mouseEvent);
    /*!
    * This signal is emitted after the map has changed its zoom level.
    * @param oldZoomLevel The previous zoom level.
    * @param newZoomLevel The new zoom level.
    */
    void zoomLevelChanged(quint16 oldZoomLevel, quint16 newZoomLevel);
    /*!
    * This signal is emitted when the center of the viewport onto the map has changed.
    * Call \ref center() or \ref mapCenter() to get the current center coordinates.
    */
    void centerChanged();
    /*!
    * This signal is emitted when a map object was selected by left-clicking on it.
    * @param mapObject The select map object.
    */
    void mapObjectSelected(const QMapObject* mapObject);

protected:
    /*!
    * Reimplemented from Qt::QWidget::mousePressEvent()
    */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    /*!
    * Reimplemented from Qt::QWidget::mouseReleaseEvent()
    */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    /*!
    * Reimplemented from Qt::QWidget::mouseMoveEvent()
    */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    /*!
    * Reimplemented from Qt::QWidget::wheelEvent()
    */
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    /*!
    * Reimplemented from Qt::QGraphicsWidget::resizeEvent()
    */
    virtual void resizeEvent(QGraphicsSceneResizeEvent* event);

protected:
    QRectF viewPort; //!< The logical view port.
    quint32 numColRow; //!< The number of tiles along both the x- and y-axis
    QGeoEngine* geoEngine; //!< the underlying geo engine
    quint32 horPadding; //!< horizontal preload padding
    quint32 verPadding; //!< vertical preload padding
    quint32 routeDetails; //!< Minimum manhattan distance betwee two consecutive visible route way points.

    /*! The period timer that triggers the removal
    * of scene items that are not within the current
    * map view port.
    */
    QTimer releaseTimer; //!< Periodic release timer

    quint16 currZoomLevel; //!< The current zoom level
    MapVersion mapVer; //!< The current map version
    MapScheme mapSchm; //!< The current map scheme
    MapResolution mapRes; //!< The current map resolution
    MapFormat mapFrmt; //!< The current map format

    QSet<QMapObject*> mapObjects; //!< Keeps track of all map objects.
    /*!
    * Stores for each map tile (as specified by its one-dimensional index)
    * a list of all map objects that intersect or are completely covered by
    * the map tile.
    */
    QHash<quint64, QList<QMapObject*> > tileToObjectsMap; //!< Map tile to map object hash map.

    /*!
    * Stores for each requested map tile (as given by its
    * one-dimensional tile index) the corresonding QMapTileReply.
    */
    QHash<quint64, QMapTileReply*> pendingTiles; //!< Pending requested map tiles

    QHash<quint64, QPair<QPixmap, bool> > mapTiles;

    bool panActive;
};

QTM_END_NAMESPACE

#endif
