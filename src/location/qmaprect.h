/*
* ==============================================================================
*  Name        : QMapRect.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPRECT_H
#define QLOCATION_MAPRECT_H

#include <QPen>
#include <QBrush>
#include <QRectF>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* Internal representation of a rectangle that has been added to the map.
*/
class QMapRect : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * Constructor.
    * @param topLeft The top left geo coordinate of the rectangle.
    * @param bottomRight The bottom right geo coordinate of the rectangle.
    * @param pen Then pen used for drawing the rectangle.
    * @param brush The brush used for drawing the rectangle.
    * @param layerIndex The layer index of the rectangle. Higher layers are stacked on top of lower layers.
    */
    QMapRect(const QMapView& mapView, const QGeoCoordinateMaps& topLeft,
             const QGeoCoordinateMaps& bottomRight, const QPen& pen = QPen(),
             const QBrush& brush = QBrush(), quint16 layerIndex = 0);

    /*!
    * @return The top left geo coordinate of the rectangle.
    */
    QGeoCoordinateMaps topLeft() const {
        return geoTopLeft;
    }
    /*!
    * @return The bottom right geo coordinate of the rectangle.
    */
    QGeoCoordinateMaps bottomRight() const {
        return geoBottomRight;
    }
    /*!
    * @return pen Then pen used for drawing the rectangle.
    */
    QPen pen() const {
        return p;
    }
    /*!
    * @return brush The brush used for drawing the rectangle.
    */
    QBrush brush() const {
        return b;
    }

protected:
    virtual void compMapCoords();
    virtual bool intersects(const QRectF& rect) const;
    virtual void paint(QPainter* painter, const QRectF& viewPort);

private:
    QGeoCoordinateMaps geoTopLeft; //!< The top left geo coordinate of the rectangle.
    QGeoCoordinateMaps geoBottomRight; //!< The bottom right geo coordinate of the rectangle.
    QPen p; //!< The pen used for drawing the rectangle.
    QBrush b; //!< The brush used for drawing the rectangle.
    QRectF rect;
};

QTM_END_NAMESPACE

#endif
