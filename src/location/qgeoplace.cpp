/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeoplace.h"
#include "qgeoplace_p.h"
#include "qlandmark.h"

#ifdef QGEOPLACE_DEBUG
#include <QDebug>
#endif

#if !defined(Q_CC_MWERKS)
template<> QTM_PREPEND_NAMESPACE(QGeoPlacePrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QGeoPlacePrivate)>::clone()
{
    return d->clone();
}
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoPlace
    \brief The QGeoPlace class represents basic information about a place.

    \inmodule QtLocation
    \since 1.1

    \ingroup location

    A QGeoPlace contains a coordinate and the corresponding address, along
    with an optional bounding box describing the minimum viewport necessary
    to display the entirety of the place.

    A QGeoPlace may contain an QLandmark instance.  The isLandmark() function
    can be used to determine if this is the case, and the
    QLandmark(const QGeoPlace &place) constructor can be used to restore
    access to the landmark data.

    For example:
    \code
    QGeoPlace p;
    QLandmark l;
    ...
    if (p.isLandmark())
        l = QLandmark(p)
    \endcode
*/

/*!
    Constructs an empty place object.
*/
QGeoPlace::QGeoPlace()
        : d_ptr(new QGeoPlacePrivate())
{
}

/*!
    \internal
*/
QGeoPlace::QGeoPlace(QGeoPlacePrivate *dd)
        : d_ptr(dd) {}

/*!
    Constructs a copy of \a other.
    \since 1.1
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
    \since 1.1
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
    \since 1.1
*/
bool QGeoPlace::operator== (const QGeoPlace &other) const
{
    return ( *(d_ptr.constData()) == *(other.d_ptr.constData()));
}

/*!
    Returns true if \a other is not equal to this place,
    otherwise returns false.
    \since 1.1
*/
bool QGeoPlace::operator!= (const QGeoPlace &other) const
{
    return (!this->operator ==(other));
}

/*!
    This function returns whether this QGeoPlace instance contain all of the
    information required to construct a QLandmark instance.

    If so, the QLandmark(const QGeoPlace &place) constructor can be used to
    restore access to the landmark data.
    \since 1.1
*/
bool QGeoPlace::isLandmark() const
{
    return (d_ptr->type == QGeoPlacePrivate::LandmarkType);
}

/*!
    Returns the viewport associated with this place.

    The viewport is a suggestion for a size and position of a map window
    which aims to view this palce.
    \since 1.1
*/
QGeoBoundingBox QGeoPlace::viewport() const
{
    Q_D(const QGeoPlace);
    return d->viewport;
}

/*!
    Sets the viewport associated with this place to \a viewport.

    The viewport is a suggestion for a size and position of a map window
    which aims to view this palce.
    \since 1.1
*/
void QGeoPlace::setViewport(const QGeoBoundingBox &viewport)
{
    Q_D(QGeoPlace);
    d->viewport = viewport;
}

/*!
    Returns the coordinate that this place is located at.
    \since 1.1
*/
QGeoCoordinate QGeoPlace::coordinate() const
{
    Q_D(const QGeoPlace);
    return d->coordinate;
}

/*!
    Sets the \a coordinate that this place is located at.
    \since 1.1
*/
void QGeoPlace::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QGeoPlace);
    d->coordinate = coordinate;
}

/*!
    Returns the address of this place.
    \since 1.1
*/
QGeoAddress QGeoPlace::address() const
{
    Q_D(const QGeoPlace);
    return d->address;
}

/*!
    Sets the \a address of this place.
    \since 1.1
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
        type(QGeoPlacePrivate::GeoPlaceType) {}

QGeoPlacePrivate::QGeoPlacePrivate(const QGeoPlacePrivate &other)
        : QSharedData(other),
        type(other.type),
        viewport(other.viewport),
        coordinate(other.coordinate),
        address(other.address) {}

QGeoPlacePrivate::~QGeoPlacePrivate() {}

QGeoPlacePrivate& QGeoPlacePrivate::operator= (const QGeoPlacePrivate & other)
{
    type = other.type;
    viewport = other.viewport;
    coordinate = other.coordinate;
    address = other.address;

    return *this;
}

bool QGeoPlacePrivate::operator== (const QGeoPlacePrivate &other) const
{

#ifdef QGEOPLACE_DEBUG
    qDebug() << "type" << (type == other.type);
    qDebug() << "viewport" << (viewport == other.viewport);
    qDebug() << "coordinate:" <<  (coordinate == other.coordinate);
    qDebug() << "address:" << (address == other.address);
#endif

    return ((type == other.type)
            && (viewport == other.viewport)
            && (coordinate == other.coordinate)
            && (address == other.address));
}

QTM_END_NAMESPACE

