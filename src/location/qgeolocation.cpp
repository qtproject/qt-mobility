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

#include "qgeolocation.h"
#include "qgeolocation_p.h"

QTM_BEGIN_NAMESPACE

QGeoLocationPrivate::QGeoLocationPrivate()
        : QSharedData()
{
}

QGeoLocationPrivate::QGeoLocationPrivate(const QGeoLocationPrivate &other)
        : QSharedData(other),
        box(other.box),
        coord(other.coord),
        addr(other.addr)
{
}

QGeoLocationPrivate::~QGeoLocationPrivate()
{
}

/*!
    \class QGeoLocation
    \brief The QGeoLocation class represents a geolocation.
    \ingroup location

    This class represent a geo location as returned by QSearchResponse::places().
*/

/*!
    Default constructor.
*/
QGeoLocation::QGeoLocation()
        : d(new QGeoLocationPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QGeoLocation::QGeoLocation(const QGeoLocation &other)
        : d(other.d)
{
}

/*!
    Destroys the location
*/
QGeoLocation::~QGeoLocation()
{
}

/*!
    Assigns \a other to this location and returns a reference
    to this location.
*/
QGeoLocation &QGeoLocation::operator=(const QGeoLocation & other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if \a other is equal to this location,
    otherwise returns false.
*/
bool QGeoLocation::operator==(const QGeoLocation &other) const
{
    return (d->box == other.boundingBox())
           && (d->coord == other.coordinate())
           && (d->addr == other.address());
}

/*!
    \fn bool QGeoLocation::operator!=(const QGeoLocation &other) const

    Returns true if \a other is not equal to this location,
    otherwise returns false.
*/

/*!
    Returns the bounding box that completely encloses the location.

    The x coordinates of the corner points represent longitudes,
    the y coordinates represent latitudes.
*/
QRectF QGeoLocation::boundingBox() const
{
    return d->box;
}

/*!
    Sets the \a boundingBox of the location.
*/
void QGeoLocation::setBoundingBox(const QRectF &boundingBox)
{
    d->box = boundingBox;
}

/*!
    Returns the geocoordinate of this location.
*/
QGeoCoordinate QGeoLocation::coordinate() const
{
    return d->coord;
}

/*!
    Sets the \a coordinate of this location.
*/
void QGeoLocation::setCoordinate(const QGeoCoordinate &coordinate)
{
    d->coord = coordinate;
}

/*!
    Returns the address found.
*/
QGeoAddress QGeoLocation::address() const
{
    return d->addr;
}

/*!
    Sets the \a address of this location.
*/
void QGeoLocation::setAddress(const QGeoAddress &address)
{
    d->addr = address;
}

QTM_END_NAMESPACE

