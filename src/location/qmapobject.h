/*
* ==============================================================================
*  Name        : QMapObject.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPOBJECT_H
#define QLOCATION_MAPOBJECT_H

#include <QGraphicsItem>
#include <QRectF>
#include <QLineF>
#include <QList>
#include <QObject>

#include "qmapview.h"

namespace QLocation
{
/*!
* This is the abstract base class of all internal map objects. Map objects store all information
* that is needed to generate concrete Qt:QGraphicsItem objects when they become covered by the
* view port.
*/
class QMapObject //: QObject
{
    //Q_OBJECT

    friend class QMapView;

public:
    enum MapObjectType {
        Marker,
        Line,
        Rect,
        Route,
        Ellipse,
        Polygon,
        Pixmap
    };

public:
    /*!
    * Constructor.
    */
    QMapObject(const QMapView& mapView, MapObjectType type, quint16 z = 0)
            : mapView(mapView), objType(type), z(z) {}

    /*!
    * @return The z index of this map object, as used for layering.
    *         Objects with higher z indices are stacked on top of objects
    *         with lower z indices.
    */
    inline quint16 zValue() const {
        return z;
    }
    inline MapObjectType type() const {
        return objType;
    }

    static QRectF boundingRect(const QLineF& line);
    static bool intersect(const QRectF& rect, const QLineF& line);

protected:
    /*!
    * Computes whether this map objects intersects with or completely lies in the given rectangle.
    * @param tileRect The rectangle in terms of map (pixel) coordinates.
    * @return true (intersects) / false (does not intersect)
    */
    virtual bool intersects(const QRectF& rect) const = 0;
    /*!
    * Computes the appropriate map coordinates (in pixels) according to the current QMapView setup.
    */
    virtual void compMapCoords() = 0;
    virtual void compIntersectingTiles(const QRectF& box);
    virtual void paint(QPainter* painter, const QRectF& viewPort) = 0;

protected:
    //! The one-dimensional indices of the tiles that this map object either intersects or completely lies on.
    QList<quint64> intersectingTiles;
    const QMapView& mapView;
    MapObjectType objType;
    quint16 z; //!< The z index, used for layering.
};

}

#endif
