/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
class Q_LOCATION_EXPORT QGeoCoordinateMaps
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
