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

#include "qgeoplace.h"
#include "qgeoplace_p.h"

#include "qlandmark.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoPlace
    \brief The QGeoPlace class represents a geolocation.
    \ingroup location

    This class represent a geo location as returned by QSearchResponse::places().
*/

/*!
    Default constructor.
*/
QGeoPlace::QGeoPlace()
        : d_ptr(new QGeoPlacePrivate())
{
}

QGeoPlace::QGeoPlace(QGeoPlacePrivate *dd)
    : d_ptr(dd) {}

/*!
    Constructs a copy of \a other.
*/
QGeoPlace::QGeoPlace(const QGeoPlace &other)
        : d_ptr(other.d_ptr)
{
}

/*!
    Destroys the location
*/
QGeoPlace::~QGeoPlace()
{
}

/*!
    Assigns \a other to this location and returns a reference
    to this location.
*/
QGeoPlace &QGeoPlace::operator= (const QGeoPlace & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

inline QGeoPlacePrivate* QGeoPlace::d_func()
{
    return reinterpret_cast<QGeoPlacePrivate*>(d_ptr.data());
}

inline const QGeoPlacePrivate* QGeoPlace::d_func() const
{
    return reinterpret_cast<const QGeoPlacePrivate*>(d_ptr.constData());
}

/*!
    Returns true if \a other is equal to this location,
    otherwise returns false.
*/
bool QGeoPlace::operator== (const QGeoPlace &other) const
{
    return (d_ptr.constData() == other.d_ptr.constData());
}

/*!
    Returns true if \a other is not equal to this location,
    otherwise returns false.
*/
bool QGeoPlace::operator!= (const QGeoPlace &other) const
{
    return (!this->operator ==(other));
}

/*!
*/
QGeoPlace::PlaceType QGeoPlace::type() const
{
    return d_ptr->type;
}

/*!
    Returns the bounding box that completely encloses the location.

    The x coordinates of the corner points represent longitudes,
    the y coordinates represent latitudes.
*/
QGeoBoundingBox QGeoPlace::boundingBox() const
{
    Q_D(const QGeoPlace);
    return d->boundingBox;
}

/*!
    Sets the \a boundingBox of the location.
*/
void QGeoPlace::setBoundingBox(const QGeoBoundingBox &boundingBox)
{
    Q_D(QGeoPlace);
    d->boundingBox = boundingBox;
}

/*!
    Returns the geocoordinate of this location.
*/
QGeoCoordinate QGeoPlace::coordinate() const
{
    Q_D(const QGeoPlace);
    return d->coordinate;
}

/*!
    Sets the \a coordinate of this location.
*/
void QGeoPlace::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QGeoPlace);
    d->coordinate = coordinate;
}

/*!
    Returns the address found.
*/
QGeoAddress QGeoPlace::address() const
{
    Q_D(const QGeoPlace);
    return d->address;
}

/*!
    Sets the \a address of this location.
*/
void QGeoPlace::setAddress(const QGeoAddress &address)
{
    Q_D(QGeoPlace);
    d->address = address;
}

/*******************************************************************************
*******************************************************************************/

QGeoPlacePrivate::QGeoPlacePrivate()
    : QSharedData(),
    type(QGeoPlace::GeoPlaceType){}

QGeoPlacePrivate::QGeoPlacePrivate(const QGeoPlacePrivate &other)
    : QSharedData(other),
    type(other.type),
    boundingBox(other.boundingBox),
    coordinate(other.coordinate),
    address(other.address) {}

QGeoPlacePrivate::~QGeoPlacePrivate(){}

QGeoPlacePrivate& QGeoPlacePrivate::operator= (const QGeoPlacePrivate &other)
{
    type = other.type;
    boundingBox = other.boundingBox;
    coordinate = other.coordinate;
    address = other.address;

    return *this;
}

bool QGeoPlacePrivate::operator== (const QGeoPlacePrivate &other) const
{
    return ((type == other.type)
            && (boundingBox == other.boundingBox)
            && (coordinate == other.coordinate)
            && (address == other.address));
}

QTM_END_NAMESPACE

