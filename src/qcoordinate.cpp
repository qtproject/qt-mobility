/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#include "qcoordinate.h"

#include <QDateTime>
#include <QHash>
#include <QDataStream>
#include <QDebug>
#include <qnumeric.h>
#include <qmath.h>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double qcoordinate_EARTH_MEAN_RADIUS = 6371.0072;

inline static double qcoordinate_degToRad(double deg) { return deg * M_PI / 180; }
inline static double qcoordinate_radToDeg(double rad) { return rad * 180 / M_PI; }


class QCoordinatePrivate
{
public:
    double lat;
    double lng;
    double alt;
};


/*!
    \class QCoordinate
    \brief The QCoordinate class defines a geographical position on the surface of the Earth.

    A QCoordinate is defined by latitude, longitude, and optionally, altitude.

    Use type() to determine whether a coordinate is a 2D coordinate (has
    latitude and longitude only) or 3D coordinate (has latitude, longitude
    and altitude). Use distanceTo() and azimuthTo() to calculate the distance
    and bearing between coordinates.

    The coordinate values should be specified using the WGS84 datum.
*/

/*!
    \enum QCoordinate::CoordinateType
    Defines the types of a coordinate.

    \value InvalidCoordinate An invalid coordinate. A coordinate is invalid if its latitude or longitude values are invalid.
    \value Coordinate2D A coordinate with valid latitude and longitude values.
    \value Coordinate3D A coordinate with valid latitude and longitude values, and also an altitude value.
*/

/*!
    \enum QCoordinate::CoordinateFormat
    Defines the possible formatting options for toString().

    \value DecimalDegrees Returns a string representation of the coordinates in decimal degrees format.
    \value DecimalDegreesWithHemisphere Returns a string representation of the coordinates in decimal degrees format, using 'N', 'S', 'E' or 'W' to indicate the hemispheres of the coordinates.
    \value DegreesMinutes Returns a string representation of the coordinates in degrees-minutes format.
    \value DegreesMinutesWithHemisphere Returns a string representation of the coordinates in degrees-minutes format, using 'N', 'S', 'E' or 'W' to indicate the hemispheres of the coordinates.
    \value DegreesMinutesSeconds Returns a string representation of the coordinates in degrees-minutes-seconds format.
    \value DegreesMinutesSecondsWithHemisphere Returns a string representation of the coordinates in degrees-minutes-seconds format, using 'N', 'S', 'E' or 'W' to indicate the hemispheres of the coordinates.

    \sa toString()
*/


/*!
    Constructs a coordinate. The coordinate will be invalid until
    setLatitude() and setLongitude() have been called.
*/
QCoordinate::QCoordinate()
    : d(new QCoordinatePrivate)
{
    d->lat = qQNaN();
    d->lng = qQNaN();
    d->alt = qQNaN();
}

/*!
    Constructs a coordinate with the given \a latitude and \a longitude.
*/
QCoordinate::QCoordinate(double latitude, double longitude)
    : d(new QCoordinatePrivate)
{
    d->lat = latitude;
    d->lng = longitude;
    d->alt = qQNaN();
}

/*!
    Constructs a coordinate with the given \a latitude, \a longitude
    and \a altitude.
*/
QCoordinate::QCoordinate(double latitude, double longitude, double altitude)
    : d(new QCoordinatePrivate)
{
    d->lat = latitude;
    d->lng = longitude;
    d->alt = altitude;
}

/*!
    Constructs a coordinate from the contents of \a other.
*/
QCoordinate::QCoordinate(const QCoordinate &other)
    : d(new QCoordinatePrivate)
{
    operator=(other);
}

/*!
    Destroys the coordinate object.
*/
QCoordinate::~QCoordinate()
{
    delete d;
}

/*!
    Assigns \a other to this coordinate and returns a reference to this
    coordinate.
*/
QCoordinate &QCoordinate::operator=(const QCoordinate &other)
{
    if (this == &other)
        return *this;

    d->lat = other.d->lat;
    d->lng = other.d->lng;
    d->alt = other.d->alt;

    return *this;
}

/*!
    Returns true if the latitude, longitude and altitude of this
    coordinate are the same as those of \a other.
*/
bool QCoordinate::operator==(const QCoordinate &other) const
{
    return ( (qIsNaN(d->lat) && qIsNaN(other.d->lat)) || qFuzzyCompare(d->lat, other.d->lat) )
            && ( (qIsNaN(d->lng) && qIsNaN(other.d->lng)) || qFuzzyCompare(d->lng, other.d->lng) )
            && ( (qIsNaN(d->alt) && qIsNaN(other.d->alt)) || qFuzzyCompare(d->alt, other.d->alt) );
}

/*!
    Returns true if the type() is Coordinate2D or Coordinate3D.
*/
bool QCoordinate::isValid() const
{
    CoordinateType t = type();
    return t == Coordinate2D || t == Coordinate3D;
}

/*!
    Returns the type of this coordinate.
*/
QCoordinate::CoordinateType QCoordinate::type() const
{
    if ( (d->lat >= -90 && d->lat <= 90)
          && (d->lng >= -180 && d->lng <= 180) ) {
        if (qIsNaN(d->alt))
            return Coordinate2D;
        return Coordinate3D;
    }
    return InvalidCoordinate;
}


/*!
    Returns the latitude, in decimal degrees. The return value is undefined
    if the latitude has not been set.

    A positive latitude indicates the Northern Hemisphere, and a negative
    latitude indicates the Southern Hemisphere.

    \sa setLatitude(), type()
*/
double QCoordinate::latitude() const
{
    return d->lat;
}

/*!
    Sets the latitude (in decimal degrees) to \a latitude. The value should
    be in the WGS84 datum.

    To be valid, the latitude must be between -90 to 90 inclusive.

    \sa latitude()
*/
void QCoordinate::setLatitude(double latitude)
{
    d->lat = latitude;
}

/*!
    Returns the longitude, in decimal degrees. The return value is undefined
    if the longitude has not been set.

    A positive longitude indicates the Eastern Hemisphere, and a negative
    longitude indicates the Western Hemisphere.

    \sa setLongitude(), type()
*/
double QCoordinate::longitude() const
{
    return d->lng;
}

/*!
    Sets the longitude (in decimal degrees) to \a longitude. The value should
    be in the WGS84 datum.

    To be valid, the longitude must be between -180 to 180 inclusive.

    \sa longitude()
*/
void QCoordinate::setLongitude(double longitude)
{
    d->lng = longitude;
}

/*!
    Returns the altitude (meters above sea level).

    The return value is undefined if the altitude has not been set.

    \sa setAltitude(), type()
*/
double QCoordinate::altitude() const
{
    return d->alt;
}

/*!
    Sets the altitude (meters above sea level) to \a altitude.

    \sa altitude()
*/
void QCoordinate::setAltitude(double altitude)
{
    d->alt = altitude;
}

/*!
    Returns the distance (in meters) from this coordinate to the coordinate
    specified by \a other. Altitude is not used in the calculation.

    This calculation returns the great-circle distance between the two
    coordinates, with an assumption that the Earth is spherical for the
    purpose of this calculation.

    Returns 0 if the type of this coordinate or the type of \a other is
    QCoordinate::InvalidCoordinate.
*/
qreal QCoordinate::distanceTo(const QCoordinate &other) const
{
    if (type() == QCoordinate::InvalidCoordinate
            || other.type() == QCoordinate::InvalidCoordinate) {
        return 0;
    }

    // Haversine formula
    double dlat = qcoordinate_degToRad(other.d->lat - d->lat);
    double dlon = qcoordinate_degToRad(other.d->lng - d->lng);
    double y = qSin(dlat/2) * qSin(dlat/2)
            + qCos(qcoordinate_degToRad(d->lat))
            * qCos(qcoordinate_degToRad(other.d->lat))
            * qSin(dlon/2) * qSin(dlon/2);
    double x = 2 * atan2(qSqrt(y), qSqrt(1-y));
    return qreal(x * qcoordinate_EARTH_MEAN_RADIUS * 1000);
}

/*!
    Returns the azimuth (or bearing) in degrees from this coordinate to the
    coordinate specified by \a other. Altitude is not used in the calculation.

    There is an assumption that the Earth is spherical for the purpose of
    this calculation.

    Returns 0 if the type of this coordinate or the type of \a other is
    QCoordinate::InvalidCoordinate.
*/
qreal QCoordinate::azimuthTo(const QCoordinate &other) const
{
    if (type() == QCoordinate::InvalidCoordinate
            || other.type() == QCoordinate::InvalidCoordinate) {
        return 0;
    }

    double dlon = qcoordinate_degToRad(other.d->lng - d->lng);
    double lat1Rad = qcoordinate_degToRad(d->lat);
    double lat2Rad = qcoordinate_degToRad(other.d->lat);

    double y = qSin(dlon) * qCos(lat2Rad);
    double x = qCos(lat1Rad) * qSin(lat2Rad) - qSin(lat1Rad) * qCos(lat2Rad) * qCos(dlon);

    double whole;
    double fraction = modf(qcoordinate_radToDeg(atan2(y, x)), &whole);
    return qreal((int(whole + 360) % 360) + fraction);
}

/*!
    Returns this coordinate as a string in the specified \a format.

    For example, if this coordinate latitude-longitude coordinates of
    (-27.46758\unicode{0xB0}, 153.027892\unicode{0xB0}), these are the
    strings that are returned depending on \a format:

    \table
    \header
        \o \a format value
        \o Returned string
    \row
        \o \l DecimalDegrees
        \o -27.46758\unicode{0xB0}, 153.02789\unicode{0xB0}
    \row
        \o \l DecimalDegreesWithHemisphere
        \o 27.46758\unicode{0xB0} S, 153.02789\unicode{0xB0} E
    \row
        \o \l DegreesMinutes
        \o -27\unicode{0xB0} 28.054', 153\unicode{0xB0} 1.673'
    \row
        \o \l DegreesMinutesWithHemisphere
        \o 27\unicode{0xB0} 28.054 S', 153\unicode{0xB0} 1.673' E
    \row
        \o \l DegreesMinutesSeconds
        \o -27\unicode{0xB0} 28' 3.2", 153\unicode{0xB0} 1' 40.4"
    \row
        \o \l DegreesMinutesSecondsWithHemisphere
        \o 27\unicode{0xB0} 28' 3.2" S, 153\unicode{0xB0} 1' 40.4" E
    \endtable
*/
QString QCoordinate::toString(CoordinateFormat format) const
{
    if (type() == QCoordinate::InvalidCoordinate)
        return QObject::tr("<Invalid coordinate>");

    QString latStr;
    QString longStr;

    double absLat = qAbs(d->lat);
    double absLng = qAbs(d->lng);
    QChar symbol(0x00B0);   // degrees symbol

    switch (format) {
        case DecimalDegrees:
        case DecimalDegreesWithHemisphere:
        {
            latStr = QString::number(absLat, 'f', 5) + symbol;
            longStr = QString::number(absLng, 'f', 5) + symbol;
            break;
        }
        case DegreesMinutes:
        case DegreesMinutesWithHemisphere:
        {
            double latMin = (absLat - int(absLat)) * 60;
            double lngMin = (absLng - int(absLng)) * 60;
            latStr = QString("%1%2 %3'")
                    .arg(QString::number(int(absLat)))
                    .arg(symbol)
                    .arg(QString::number(latMin, 'f', 3));
            longStr = QString("%1%2 %3'")
                    .arg(QString::number(int(absLng)))
                    .arg(symbol)
                    .arg(QString::number(lngMin, 'f', 3));
            break;
        }
        case DegreesMinutesSeconds:
        case DegreesMinutesSecondsWithHemisphere:
        {
            double latMin = (absLat - int(absLat)) * 60;
            double lngMin = (absLng - int(absLng)) * 60;
            double latSec = (latMin - int(latMin)) * 60;
            double lngSec = (lngMin - int(lngMin)) * 60;

            latStr = QString("%1%2 %3' %4\"")
                    .arg(QString::number(int(absLat)))
                    .arg(symbol)
                    .arg(QString::number(int(latMin)))
                    .arg(QString::number(latSec, 'f', 1));
            longStr = QString("%1%2 %3' %4\"")
                    .arg(QString::number(int(absLng)))
                    .arg(symbol)
                    .arg(QString::number(int(lngMin)))
                    .arg(QString::number(lngSec, 'f', 1));
            break;
        }
    }

    // now add the "-" to the start, or append the hemisphere char
    switch (format) {
        case DecimalDegrees:
        case DegreesMinutes:
        case DegreesMinutesSeconds:
        {
            if (d->lat < 0)
                latStr.insert(0, "-");
            if (d->lng < 0)
                longStr.insert(0, "-");
            break;
        }
        case DecimalDegreesWithHemisphere:
        case DegreesMinutesWithHemisphere:
        case DegreesMinutesSecondsWithHemisphere:
        {
            if (d->lat < 0)
                latStr.append(" S");
            else if (d->lat > 0)
                latStr.append(" N");
            if (d->lng < 0)
                longStr.append(" W");
            else if (d->lng > 0)
                longStr.append(" E");
            break;
        }
    }

    return QString("%1, %2").arg(latStr, longStr);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QCoordinate &coord)
{
    dbg.nospace() << "QCoordinate(" << coord.latitude();
    dbg.nospace() << ", ";
    dbg.nospace() << coord.longitude();
    dbg.nospace() << ", ";
    if (coord.type() == QCoordinate::Coordinate3D)
        dbg.nospace() << coord.altitude();
    else
        dbg.nospace() << '?';
    dbg.nospace() << ')';
    return dbg;
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
    Writes the given \a coordinate to the specified \a stream.

    \sa {Format of the QDataStream Operators}
*/

QDataStream &operator<<(QDataStream &stream, const QCoordinate &coordinate)
{
    stream << coordinate.latitude();
    stream << coordinate.longitude();
    stream << coordinate.altitude();
    return stream;
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
    Reads a coordinate from the specified \a stream into the given
    \a coordinate.

    \sa {Format of the QDataStream Operators}
*/

QDataStream &operator>>(QDataStream &stream, QCoordinate &coordinate)
{
    double value;
    stream >> value;
    coordinate.setLatitude(value);
    stream >> value;
    coordinate.setLongitude(value);
    stream >> value;
    coordinate.setAltitude(value);
    return stream;
}
#endif
