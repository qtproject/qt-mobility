/*
* ==============================================================================
*  Name        : QGeoLocation.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_GEOPLACE_H
#define QLOCATION_GEOPLACE_H

#include <QRectF>

#include "qaddress.h"
#include "qalternativeaddress.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* This class represent a geo location as returned by
* \ref QGeocodingReply::places().
*/
class QGeoLocation
{
    friend class QGeocodingXmlHandler;

public:
    /*!
    * Default constructor.
    */
    QGeoLocation() {}

    /*!
    * @return The bounding box that completely encloses the location.
    *         The x coordinates of the corner points represent longitudes,
              the y coordinates represent latitudes.
    */
    QRectF boundingBox() const {
        return box;
    }
    /*!
    * @return The geo coordinate of this location.
    */
    QGeoCoordinateMaps position() const {
        return pos;
    }
    /*!
    * @return Description of the location.
    */
    QString title() const {
        return ttl;
    }
    /*!
    * @return The MARC language used in the description of this location.
    */
    QString language() const {
        return lang;
    }
    /*!
    * @return The address found.
    */
    QAddress address() const {
        return addr;
    }
    /*!
    * @return Alternatives to the address found.
    */
    QAlternativeAddress alternatives() const {
        return altern;
    }

private:
    QRectF          box;
    QGeoCoordinateMaps  pos;
    QString         ttl;
    QString         lang;
    QAddress        addr;
    QAlternativeAddress altern;
};

QTM_END_NAMESPACE

#endif
