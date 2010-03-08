/*
* ==============================================================================
*  Name        : QMapPolygon.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPPOLYGON_H
#define QLOCATION_MAPPOLYGON_H

#include <QPen>
#include <QBrush>
#include <QPainterPath>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

namespace QLocation
{
/*!
* Internal representation of a polygon that has been added to the map.
*/
class QMapPolygon : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * Constructor
    * @param polygon The list of geo coordinates that define the polygon.
    * @param pen Then pen used for drawing the rectangle.
    * @param brush The brush used for drawing the rectangle.
    * @param layerIndex The layer index of the polygon. Higher layers are stacked on top of lower layers.
    */
    QMapPolygon(const QMapView& mapView, const QList<QGeoCoordinateMaps>& polygon,
                const QPen& pen, const QBrush& brush, quint16 layerIndex);

    /*!
    * @return The polygon.
    */
    QList<QGeoCoordinateMaps> polygon() const {
        return poly;
    }
    /*!
    * @return pen Then pen used for drawing the polygon.
    */
    QPen pen() const {
        return p;
    }
    /*!
    * @return brush The brush used for drawing the polygon.
    */
    QBrush brush() const {
        return br;
    }

protected:
    virtual bool intersects(const QRectF& rect) const;
    virtual void compMapCoords();
    virtual void paint(QPainter* painter, const QRectF& viewPort);

private:
    QList<QGeoCoordinateMaps> poly; //!< The polygon.
    QPen p; //!< The pen used for drawing the polygon.
    QBrush br; //!< The brush used for drawing the polygon.
    QPainterPath path;
};

}

#endif
