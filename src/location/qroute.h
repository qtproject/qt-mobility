/*
* ==============================================================================
*  Name        : QRoute.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_ROUTE_H
#define QLOCATION_ROUTE_H

#include <QDateTime>
#include <QList>
#include <QRectF>

#include "qmaneuver.h"

QTM_BEGIN_NAMESPACE

/*!
* This class represents a route as contained in a \ref QRouteReply.
* A CRoute contains a collection of \ref CManeuver objects.
*/
class QRoute
{
    friend class QRouteXmlHandler;

public:
    //! The default constructor.
    QRoute() {};
    //! The copy constructor.
    QRoute(const QRoute& route);
    //! The assignment operator.
    QRoute& operator=(const QRoute& route);

    /*!
    * @return The departure time of this route.
    */
    QDateTime departure() const {
        return tod;
    }
    /*!
    * @return The arrival time of this route.
    */
    QDateTime arrival() const {
        return toa;
    }
    /*!
    * @return The distance covered by this route in meters.
    */
    quint32 distance() const {
        return dist;
    }

    /*!
    * @return The bounding box that completely encloses the route.
    *         The x coordinates of the corner points represent longitudes,
              the y coordinates represent latitudes.
    */
    const QRectF& boundingBox() const {
        return box;
    }
    /*!
    * @return The list of all maneuvers comprising the route.
    */
    QList<QManeuver> maneuvers() const {
        return man;
    }

protected:
    quint32 dist;
    QDateTime tod;
    QDateTime toa;

    QRectF box;
    QList<QManeuver> man;
};

QTM_END_NAMESPACE

#endif
