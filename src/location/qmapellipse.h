/*
* ==============================================================================
*  Name        : QMapEllipse.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPELLIPSE_H
#define QLOCATION_MAPELLIPSE_H

#include <QPen>
#include <QBrush>
#include <QPointF>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* Internal representation of an ellipse that has been added to the map.
*/
class QMapEllipse : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * Constructor.
    * @param topLeft The top left geo coordinate of the bounding box of the ellipse.
    * @param bottomRight The bottom right geo coordinate of the bounding box of the ellipse.
    * @param pen Then pen used for drawing the ellipse.
    * @param brush The brush used for drawing the ellipse.
    */
    QMapEllipse(const QGeoCoordinateMaps& topLeft, const QGeoCoordinateMaps& bottomRight,
                const QPen& pen = QPen(), const QBrush& brush = QBrush(),
                quint16 layerIndex = 0);

    /*!
    * @return The top left geo coordinate of the bounding box of the ellipse.
    */
    QGeoCoordinateMaps topLeft() const {
        return geoTopLeft;
    }
    /*!
    * @return The bottom right geo coordinate of the bounding box of the ellipse.
    */
    QGeoCoordinateMaps bottomRight() const {
        return geoBottomRight;
    }
    /*!
    * @return pen Then pen used for drawing the ellipse.
    */
    QPen pen() const {
        return p;
    }
    /*!
    * @return brush The brush used for drawing the ellipse.
    */
    QBrush brush() const {
        return b;
    }

private:
    QGeoCoordinateMaps geoTopLeft; //!< The top left geo coordinate of the bounding box of the ellipse.
    QPointF mapTopLeft; //!< The top left map coordinate of the bounding box of the ellipse, for internal use only
    QGeoCoordinateMaps geoBottomRight; //!< The bottom right geo coordinate of the bounding box of the ellipse.
    QPointF mapBottomRight; //!< The bottom right map coordinate of the bounding box of the ellipse, for internal use only
    QPen p; //!< The pen used for drawing the ellipse.
    QBrush b; //!< The brush used for drawing the ellipse.
};

QTM_END_NAMESPACE

#endif
