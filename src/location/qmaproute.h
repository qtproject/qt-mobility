/*
* ==============================================================================
*  Name        : QMapRoute.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPROUTE_H
#define QLOCATION_MAPROUTE_H

#include <QPen>
#include <QMap>
#include <QList>
#include <QPixmap>
#include <QLineF>

#include "qroute.h"
#include "qmapobject.h"

QTM_BEGIN_NAMESPACE

/*!
* Internal representation of a route that has been added to the map.
* It keeps track of all individual \ref QRouteSegment<i></i>s.
*/
class QMapRoute : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * Constructor.
    * @param route The actual \ref QRoute that this map route represents.
    */
    QMapRoute(const QMapView& mapView,
              const QRoute& route,
              const QPen& pen = QPen(),
              const QPixmap& endpointMarker = QPixmap(),
              quint16 layerIndex = 0);

    /*!
    * @param The pen used for drawing this route.
    */
    QPen pen() const {
        return rPen;
    }
    /*!
    * @return The z index of this route on the map, as used for layering.
    *         Objects with higher z indices are stacked on top of objects
    *         with lower z indices.
    */
    quint16 zValue() const {
        return z;
    }

protected:
    virtual void compMapCoords();
    virtual void compIntersectingTiles(const QRectF& box);
    virtual bool intersects(const QRectF& rect) const;
    virtual void paint(QPainter* painter, const QRectF& viewPort);

private:
    void addSegment(const QLineF& line);

private:
    QRoute rt;
    QMap<quint64, QList<QLineF> > segments; //!< All individual route segments.
    QPen rPen; //!< The pen used for drawing this route.
    quint16 z; //!< The z index, used for layering.
};


QTM_END_NAMESPACE

#endif
