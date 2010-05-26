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
    \brief The QGeoPlace class represents basic information about a place.
    \ingroup location

    A QGeoPlace contains a coordinate and the corresponding address, along
    with an optional bounding box describing the minimum viewport necessary
    to display the entirety of the place.

    The subclasses of QGeoPlace can provide further place information.  It is
    possible to convert instance of QGeoPlace subclasses to and from QGeoPlace
    instances.

    \sa type().
*/

// TODO
// Consider making the type enum and method protected
// Provide a public bool isLandmark() const method instead

/*!
    Constructs an empty place object.
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
    Destroys this place.
*/
QGeoPlace::~QGeoPlace()
{
}

/*!
    Assigns \a other to this place and returns a reference
    to this place.
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
    Returns true if \a other is equal to this place,
    otherwise returns false.
*/
bool QGeoPlace::operator== (const QGeoPlace &other) const
{
    return (d_ptr.constData() == other.d_ptr.constData());
}

/*!
    Returns true if \a other is not equal to this place,
    otherwise returns false.
*/
bool QGeoPlace::operator!= (const QGeoPlace &other) const
{
    return (!this->operator ==(other));
}

/*!
    Returns type information about this place instance.

    An instance of a subclass of QGeoPlace can be converted to a
    QGeoPlace instance and back without losing any data.  The
    QGeoPlace::PlaceType enum is used to indicate which subclass initially
    created this QGeoPlace instance.

    If the type corresponds to a subclass of QGeoPlace then this place
    object can be converted back by using the subclass constructor which takes
    a QGeoPlace object as an argument.  If the type is QGeoPlace::GeoPlaceType
    then using this place in the same subclass constructor will only initialize
    the coordinate, address and bounding box of the subclass.
*/
QGeoPlace::PlaceType QGeoPlace::type() const
{
    return d_ptr->type;
}

/*!
    Returns a bounding box that completely encloses this place.
*/
QGeoBoundingBox QGeoPlace::boundingBox() const
{
    Q_D(const QGeoPlace);
    return d->boundingBox;
}

/*!
    Sets the \a boundingBox of this place.

    The \a boundingBox should completely enclose this place.
*/
void QGeoPlace::setBoundingBox(const QGeoBoundingBox &boundingBox)
{
    Q_D(QGeoPlace);
    d->boundingBox = boundingBox;
}

/*!
    Returns the coordinate that this place is located at.
*/
QGeoCoordinate QGeoPlace::coordinate() const
{
    Q_D(const QGeoPlace);
    return d->coordinate;
}

/*!
    Sets the \a coordinate that this place is located at.
*/
void QGeoPlace::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QGeoPlace);
    d->coordinate = coordinate;
}

/*!
    Returns the address of this place.
*/
QGeoAddress QGeoPlace::address() const
{
    Q_D(const QGeoPlace);
    return d->address;
}

/*!
    Sets the \a address of this place.
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
        type(QGeoPlace::GeoPlaceType) {}

QGeoPlacePrivate::QGeoPlacePrivate(const QGeoPlacePrivate &other)
        : QSharedData(other),
        type(other.type),
        boundingBox(other.boundingBox),
        coordinate(other.coordinate),
        address(other.address) {}

QGeoPlacePrivate::~QGeoPlacePrivate() {}

QGeoPlacePrivate& QGeoPlacePrivate::operator= (const QGeoPlacePrivate & other)
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

