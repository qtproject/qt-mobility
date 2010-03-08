/* ==============================================================================
*  Name        : QGeoCoordinateMaps.cpp
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#include <math.h>

#include "qgeocoordinatemaps.h"

#define PI 3.14159265358979323846
#define MIN(a,b) ( (a) < (b) ? (a) : (b) )
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )

#define SET_LONGITUDE 0x01
#define SET_LATITUDE  0x02

namespace QLocation
{

QGeoCoordinateMaps::QGeoCoordinateMaps()
{
    lng = 0;
    lat = 0;
    amNull = 0;
}

QGeoCoordinateMaps::QGeoCoordinateMaps(const QPointF& coordinate)
{
    amNull = 0;
    setLongitude(coordinate.x());
    setLatitude(coordinate.y());
}

QGeoCoordinateMaps::QGeoCoordinateMaps(qreal longitude, qreal latitude)
{
    amNull = 0;
    setLongitude(longitude);
    setLatitude(latitude);
}

QGeoCoordinateMaps::QGeoCoordinateMaps(const QGeoCoordinateMaps& coordinate)
{
    lng = coordinate.lng;
    lat = coordinate.lat;
    amNull = coordinate.amNull;
}

QGeoCoordinateMaps& QGeoCoordinateMaps::operator=(const QGeoCoordinateMaps& coordinate)
{
    lng = coordinate.lng;
    lat = coordinate.lat;
    amNull = coordinate.amNull;
    return *this;
}

bool QGeoCoordinateMaps::operator==(const QGeoCoordinateMaps& coordinate) const
{
    return lng == coordinate.lng && lat == coordinate.lat;
}

bool QGeoCoordinateMaps::operator!=(const QGeoCoordinateMaps& coordinate) const
{
    return !(lng == coordinate.lng && lat == coordinate.lat);
}

void QGeoCoordinateMaps::setLatitude(qreal latitude)
{
    amNull |= SET_LATITUDE;
    lat = latitude;

    if (lat > 90)
        lat = 90;
    else if (lat < -90)
        lat = -90;
}

void QGeoCoordinateMaps::setY(qreal y)
{
    setLatitude(y);
}

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

void QGeoCoordinateMaps::setX(qreal x)
{
    setLongitude(x);
}

QPointF QGeoCoordinateMaps::toMercator() const
{
    return QPointF(lng / 360.0f + 0.5f,
                   MIN(1, MAX(0, 0.5f - (log(tan((PI / 4.0f) + (PI / 2.0f) * lat / 180.0f)) / PI) / 2.0f)));

}

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

bool QGeoCoordinateMaps::isNull() const
{
    return !((amNull & SET_LONGITUDE) &&
             (amNull & SET_LATITUDE)
            );
}

}

