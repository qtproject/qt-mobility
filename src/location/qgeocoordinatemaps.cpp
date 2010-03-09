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

#include <math.h>

#include "qgeocoordinatemaps.h"

#define PI 3.14159265358979323846
#define MIN(a,b) ( (a) < (b) ? (a) : (b) )
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )

#define SET_LONGITUDE 0x01
#define SET_LATITUDE  0x02

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoCoordinateMaps
    \brief The QGeoCoordinateMaps class represents a geocoordinate.
    \ingroup location

    Longitude and latitude are expressed as real values of degrees, 
    with e.g. (13.377, 52.5161) representing Berlin, (-74.0071, 40.7145) 
    representing New York, (-43.1951, -22.9767) representing Rio de Janeiro, 
    and (145.103, -37.868) representing Melbourne. 
    
    In terms of a QPointF, the longitude represents the x coordinate, 
    and the latitude represents the y coordinate.
*/

/*!
    Default constructor.
*/
QGeoCoordinateMaps::QGeoCoordinateMaps()
{
    lng = 0;
    lat = 0;
    amNull = 0;
}

/*!
    Constructs a QGeoCoordinateMaps instance from \a coordinate. 
    
    The longitude comes from coordinate.x() and the latitude comes from coordinate.y().
*/
QGeoCoordinateMaps::QGeoCoordinateMaps(const QPointF& coordinate)
{
    amNull = 0;
    setLongitude(coordinate.x());
    setLatitude(coordinate.y());
}

/*!
    Constructs a QGeoCoordinateMaps instance from \a latitude and \a longitude.
*/
QGeoCoordinateMaps::QGeoCoordinateMaps(qreal longitude, qreal latitude)
{
    amNull = 0;
    setLongitude(longitude);
    setLatitude(latitude);
}

/*!
    Constructors a QGeoCoordinateMaps from \a coordinate.
*/
QGeoCoordinateMaps::QGeoCoordinateMaps(const QGeoCoordinateMaps& coordinate)
{
    lng = coordinate.lng;
    lat = coordinate.lat;
    amNull = coordinate.amNull;
}

/*!
    Assigns the value of \a coordinate to this QGeoCoordinateMaps
*/
QGeoCoordinateMaps& QGeoCoordinateMaps::operator=(const QGeoCoordinateMaps& coordinate)
{
    lng = coordinate.lng;
    lat = coordinate.lat;
    amNull = coordinate.amNull;
    return *this;
}

/*!
    Returns whether this QGeoCoordinateMaps is equal to \a coordinate

    Two QGeoCoordinateMaps are deemed equal if both their latitudes and longitudes are identical.
*/
bool QGeoCoordinateMaps::operator==(const QGeoCoordinateMaps& coordinate) const
{
    return lng == coordinate.lng && lat == coordinate.lat;
}

/*!
    Returns whether this QGeoCoordinateMaps is unequal to \a coordinate
    \sa QGeoCoordinateMaps::operator==()
*/
bool QGeoCoordinateMaps::operator!=(const QGeoCoordinateMaps& coordinate) const
{
    return !(lng == coordinate.lng && lat == coordinate.lat);
}


/*!
    Returns the latitude of this geocoordinate.
*/
qreal QGeoCoordinateMaps::latitude() const {
    return lat;
}
/*!
    Same as latitude().
*/
qreal QGeoCoordinateMaps::y() const {
    return lat;
}

/*!
    Sets the latitude of this geocoordinate to \a latitude.
*/
void QGeoCoordinateMaps::setLatitude(qreal latitude)
{
    amNull |= SET_LATITUDE;
    lat = latitude;

    if (lat > 90)
        lat = 90;
    else if (lat < -90)
        lat = -90;
}

/*!
    Same as \l {QGeoCoordinateMaps::setLatitude()} {setLatitude(y)}.
*/
void QGeoCoordinateMaps::setY(qreal y)
{
    setLatitude(y);
}

/*!
    Returns the longitude of this geocoordinate.
*/
qreal QGeoCoordinateMaps::longitude() const {
    return lng;
}
/*!
    Same as longitude().
*/
qreal QGeoCoordinateMaps::x() const {
    return lng;
}

/*!
    Sets the longitude of this geocoordinate to \a longitude.
*/
void QGeoCoordinateMaps::setLongitude(qreal longitude)
{
    //In C++, the result of modulo with negative operands
    //is implementation-defined, so we need to tread carefully
    amNull |= SET_LONGITUDE;
    lng = longitude;

    if (lng == -180)
        lng = 180;

    if (lng < -180 || lng > 180) {
        if (lng != 180) {
            bool isWest = lng < 0;

            if (isWest)
                lng *= -1;

            lng = -180 + rmod(lng + 180.0, 360.0);

            if (isWest)
                lng *= -1;
        }
    }
}

/*!
    Same as \l {QGeoCoordinateMaps::setLongitude()} {setLongitude(x)}.
*/
void QGeoCoordinateMaps::setX(qreal x)
{
    setLongitude(x);
}

/*!
    Returns a point corresponding to the normailzed Mercator projection of this coordinate.
*/
QPointF QGeoCoordinateMaps::toMercator() const
{
    return QPointF(lng / 360.0f + 0.5f,
                   MIN(1, MAX(0, 0.5f - (log(tan((PI / 4.0f) + (PI / 2.0f) * lat / 180.0f)) / PI) / 2.0f)));

}

/*!
    Returns a geocoordinate corresponding to the normalized Mercator coordinate \a mercatorCoord.
*/
QGeoCoordinateMaps QGeoCoordinateMaps::fromMercator(const QPointF& mercatorCoord)
{
    qreal y = mercatorCoord.y();
    //make sure 0 <= y <= 1
    if (y < 0)
        y = 0;
    else if (y > 1.0f)
        y = 1.0f;

    qreal lat;

    if (y == 0)
        lat = 90.0f;
    else if (y == 1)
        lat = -90.0f;
    else
        lat = (180.0f / PI) * (2.0f * atan(exp(PI * (1.0f - 2.0f * y))) - (PI / 2.0f));

    double lng = modulorize(mercatorCoord.x(), 1.0f) * 360.0f - 180.0f;

    return QGeoCoordinateMaps(lng, lat);
}

qreal QGeoCoordinateMaps::rmod(const qreal a, const qreal b)
{
    quint64 div = static_cast<quint64>(a / b);
    return a - static_cast<qreal>(div) * b;
}

/**
* To modulorize a number to fit in range from 0 to max (exclude). Examples:<br/>
* modulorize( -3, 10); // 7<br/>
* modulorize(-13, 10); // 7<br/>
* modulorize(  7, 10); // 7<br/>
* modulorize( 17, 10); // 7<br/>
* modulorize( 10, 10); // 0<br/>
*
* @param {qreal} value The value to normalize.
* @param {qreal} max The exclusive upper bound
* @return {qreal} the normalized value.
*/
qreal QGeoCoordinateMaps::modulorize(qreal value, qreal max)
{
    //In C++, the result of modulo with negative operands
    //is implementation-defined, so we need to tread carefully
    if (value >= 0)
        return rmod(value, max);
    else
        return rmod(max - rmod(-1 * value, max), max);
}

/*!
    Returns whether this geocoordinate is null. 
    
    A geocoordinate is considered null until both 
    the latitude and longitude have been set.
*/
bool QGeoCoordinateMaps::isNull() const
{
    return !((amNull & SET_LONGITUDE) &&
             (amNull & SET_LATITUDE)
            );
}

QTM_END_NAMESPACE

