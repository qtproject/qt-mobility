/*
* ==============================================================================
*  Name        : QGeoCoordinateMaps.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_GEOCOORDINATE_H
#define QLOCATION_GEOCOORDINATE_H

#include "qmobilityglobal.h"

#include <QPointF>

QTM_BEGIN_NAMESPACE

/*!
* This class represents a geo coordinate. Longitude and latitude are expressed
* as real values of degrees, with e.g. (13.377, 52.5161) representing Berlin,
* (-74.0071, 40.7145) representing New York, (-43.1951, -22.9767) representing
* Rio de Janeiro, and (145.103, -37.868) representing Melbourne. In terms of a
* <i>QPointF</i>, the longitude represents the x coordinate, and the latitude represents
* the y coordinate.
*/
class QGeoCoordinateMaps
{
public:
    /*!
    * Default constructor.
    */
    QGeoCoordinateMaps();
    /*!
    * Constructs a geo coordinate from a QPointF object. QPointF::x() is assigned to
    * the longitude, QPointF::y() is assigned to the latitude.
    * @param coordinate A QPointF representing a geo coordinate.
    */
    QGeoCoordinateMaps(const QPointF& coordinate);
    /*!
    * Constructs a geo coordinate from a QPointF object. QPointF::x() is assigned to
    * the longitude, QPointF::y() is assigned to the latitude.
    * @param coordinate A QPointF representing a geo coordinate.
    */
    QGeoCoordinateMaps(qreal longitude, qreal latitude);
    /*!
    * Copy constructor.
    * @param coordinate The geo coordinate to be copied.
    */
    QGeoCoordinateMaps(const QGeoCoordinateMaps& coordinate);
    /*!
    * Assignment operator.
    * @param coordinate The geo coordinate to assign from.
    */
    QGeoCoordinateMaps& operator=(const QGeoCoordinateMaps& coordinate);

    /*!
    * Equality operator. Two geo coordinates are deemed equal if both
    * their latitudes and longitudes are identical.
    * @param coordinate The geo coordinate compared against.
    */
    bool operator==(const QGeoCoordinateMaps& coordinate) const;
    /*!
    * The inverse of \ref operator==().
    * @param coordinate The geo coordinate compared against.
    */
    bool operator!=(const QGeoCoordinateMaps& coordinate) const;

    /*!
    * @Return The latitude of this geo coordinate.
    */
    qreal latitude() const {
        return lat;
    }
    /*!
    * Same as \ref latitude().
    */
    qreal y() const {
        return lat;
    }
    /*! Sets the latitude of this geo coordinate.
    * @param latitude The latitude.
    */
    void setLatitude(const qreal latitude);
    /*!
    * Same as \ref setLatitude().
    */
    void setY(qreal y);

    /*!
    * @Return The longitude of this geo coordinate.
    */
    qreal longitude() const {
        return lng;
    }
    /*!
    * Same as \ref longitude().
    */
    qreal x() const {
        return lng;
    }
    /*! Sets the longitude of this geo coordinate.
    * @param longitude The longitude of this geo coordinate.
    */
    void setLongitude(qreal longitude);
    /*!
    * Same as \ref setLongitude().
    */
    void setX(qreal x);

    /*!
    * @return Whether this geo coordinate is null. A geo coordinate is considered null
    *         until both the latitude and longitude have been set.
    */
    bool isNull() const;
    /*!
    * Converts this geo coordinate into its normalized coordinate according
    * to a Mercator projection.
    * @return The corresponding Mercator coordinate.
    */
    QPointF toMercator() const;
    /*!
    * Constructs a geo coordinate from its corresponding normalized Mercator coordinate.
    * @return The corresponding geo coordinate.
    */
    static QGeoCoordinateMaps fromMercator(const QPointF& mercatorCoord);

private:
    static qreal rmod(const qreal a, const qreal b);
    static qreal modulorize(qreal value, qreal max);
private:
    qreal lat;
    qreal lng;
    int amNull;
};

QTM_END_NAMESPACE

#endif
