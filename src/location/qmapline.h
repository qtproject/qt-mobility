/*
* ==============================================================================
*  Name        : QMapLine.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPLINE_H
#define QLOCATION_MAPLINE_H

#include <QPen>
#include <QPointF>
#include <QPainter>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* Internal representation of a line that has been added to the map.
*/
class QMapLine : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * The contructor.
    * @param point1 The first end point (as geo coordinate) of the line.
    * @param point2 The second end point (as geo coordinate) of the line.
    * @param pen The pen used for drawing the line.
    * @param layerIndex The map layer index of the pixmap.
    */
    QMapLine(const QMapView& mapView, const QGeoCoordinateMaps& point1, const QGeoCoordinateMaps& point2,
             const QPen& pen = QPen(), quint16 layerIndex = 0);

    /*!
    * @return The pen used for drawing this line.
    */
    QPen pen() const {
        return p;
    }
    /*!
    * @return The first end point (as geo coordinate) of the line.
    */
    QGeoCoordinateMaps point1() const {
        return pt1;
    }
    /*!
    * @return The second end point (as geo coordinate) of the line.
    */
    QGeoCoordinateMaps point2() const {
        return pt2;
    }

protected:
    virtual bool intersects(const QRectF& tileRect) const;
    virtual void compMapCoords();
    virtual void paint(QPainter* painter, const QRectF& viewPort);

private:
    QGeoCoordinateMaps pt1; //!< The first end point of the line.
    QGeoCoordinateMaps pt2; //!< The second end point of the line.
    QPen p; //!< The pen used for drawing this line.
    QLineF line;
};

QTM_END_NAMESPACE

#endif
