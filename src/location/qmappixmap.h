/*
* ==============================================================================
*  Name        : QMapPixmap.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPPIXMAP_H
#define QLOCATION_MAPPIXMAP_H

#include <QPixmap>
#include <QPointF>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

namespace QLocation
{

/*!
* Internal representation of a pixmap that has been added to the map.
*/
class QMapPixmap : public QMapObject
{
    friend class QMapView;

public:
    /*!
    * The contructor.
    * @param topLeft The top left corner (as geo coordinate) where this pixmap should be added to the map.
    * @param pixmap The pixmap.
    * @param layerIndex The map layer index of the pixmap.
    */
    QMapPixmap(const QGeoCoordinateMaps& topLeft, const QPixmap& pixmap, quint16 layerIndex = 0);

    /*!
    * @return The pixmap.
    */
    QPixmap pixmap() const {
        return pic;
    }
    /*!
    * @return The top left corner (as geo coordinate) of this pixmap.
    */
    QGeoCoordinateMaps topLeft() const {
        return geoTopLeft;
    }

private:
    QPixmap pic; //!< The pixmap.
    QGeoCoordinateMaps geoTopLeft; //!< The top left coordinate of this pixmap.
    QPointF mapTopLeft; //!< The top left coordinate, for internal use only
};

}

#endif
