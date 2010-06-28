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

#include "qgeodistance.h"
#include "qgeodistance_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoDistance
    \brief The QGeoDistance class represents a geographical distance.

    \inmodule QtLocation
    
    \ingroup maps

    The QGeoDistance class represents geographical distance in either metres,
    kilometres or miles, and is able to convert distances between those units.

    The units of measurement are defined in QGeoDistance::DistanceUnits.

    \sa QGeoDistance::DistanceUnits
*/

/*!
\enum QGeoDistance::DistanceUnits

Represents a unit of measurement.

\value Metres
    metres
\value Kilometres
    1000 metres
\value Miles
    miles
*/

/*!
    Constructs a QGeoDistance object with a magnitude \a magnitude, and units
    \a units.
*/
QGeoDistance::QGeoDistance(double magnitude, DistanceUnits units)
        : d_ptr(new QGeoDistancePrivate(magnitude, units)) {}

/*!
    Constructs a QGeoDistance object from the contents of \a other.
*/
QGeoDistance::QGeoDistance(const QGeoDistance &other)
        : d_ptr(other.d_ptr) {}

/*!
    Destroys this QGeoDistance object.
*/
QGeoDistance::~QGeoDistance()
{
}

/*!
    Assigns \a other to the current QGeoDistance object, then returns a reference
    to the current QGeoDistance object.
*/
QGeoDistance& QGeoDistance::operator= (const QGeoDistance & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Sets the magnitude of the distance to \a magnitude.

    \sa QGeoDistance::magnitude()
*/
void QGeoDistance::setMagnitude(double magnitude)
{
    d_ptr->magnitude = magnitude;
}

/*!
    Returns the magnitude of the distance.

    \sa QGeoDistance::setMagnitude()
*/
double QGeoDistance::magnitude() const
{
    return d_ptr->magnitude;
}

/*!
    Sets the units of the distance to \a units.

    If \a convert is true then the value of the distance will be converted from
    a distance with the previous units to a distance with the new units.
*/
void QGeoDistance::setUnits(QGeoDistance::DistanceUnits units, bool convert)
{
    if (convert)
        d_ptr->magnitude = this->convert(units);
    d_ptr->units = units;
}

/*!
    Returns the units of the distance.
*/
QGeoDistance::DistanceUnits QGeoDistance::units() const
{
    return d_ptr->units;
}

/*!
    Returns the magnitude of the distance when converted to \a units.
*/
double QGeoDistance::convert(QGeoDistance::DistanceUnits units) const
{
    if (units == d_ptr->units)
        return d_ptr->magnitude;

    // TODO implement

    return -1.0;
}

/*******************************************************************************
*******************************************************************************/

QGeoDistancePrivate::QGeoDistancePrivate(double magnitude, QGeoDistance::DistanceUnits units)
        : QSharedData(),
        magnitude(magnitude),
        units(units) {}

QGeoDistancePrivate::QGeoDistancePrivate(const QGeoDistancePrivate &other)
        : QSharedData(other),
        magnitude(other.magnitude),
        units(other.units) {}

QGeoDistancePrivate::~QGeoDistancePrivate() {}

QGeoDistancePrivate& QGeoDistancePrivate::operator= (const QGeoDistancePrivate & other)
{
    magnitude = other.magnitude;
    units = other.units;
    return *this;
}

QTM_END_NAMESPACE
