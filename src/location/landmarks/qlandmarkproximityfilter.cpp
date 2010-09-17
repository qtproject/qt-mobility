/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkproximityfilter.h"
#include "qlandmarkproximityfilter_p.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkProximityFilter
    \brief The QLandmarkProximityFilter class is used to search for landmarks based on the radius
    around a given coordinate.

    \inmodule QtLocation
    
    \ingroup landmarks-filter

    In order to be a valid filter, the region covered by the proximity filter must not cover one of the poles.
    The landmarks are returned in ascending order of distance.  A proximity filter may
    be used inside an intersection filter to sort landmarks by distance in combination with other
    criteria such as categories.
*/

/*!
    \enum QLandmarkProximityFilter::Selection
    Defines the selection behavior of the proximity filter.
    \value SelectAll Selects all landmarks in the given proximity
    \value SelectNearestOnly Selects only the closest landmark
*/

Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkProximityFilter)

/*!
    Creates a filter that will select landmarks within a given \a radius around
    a central \a coordinate.
*/
QLandmarkProximityFilter::QLandmarkProximityFilter(const QGeoCoordinate &coordinate, double radius)
        : QLandmarkFilter(new QLandmarkProximityFilterPrivate(coordinate, radius))
{
}

/*!
    \fn QLandmarkProximityFilter::QLandmarkProximityFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new proximity filter.
*/

/*!
    Destroys the filter.
*/
QLandmarkProximityFilter::~QLandmarkProximityFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the central coordinate of the filter.
*/
QGeoCoordinate QLandmarkProximityFilter::coordinate() const
{
    Q_D(const QLandmarkProximityFilter);
    return d->coordinate;
}

/*!
    Sets the central \a coordinate of the filter.
*/
void QLandmarkProximityFilter::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QLandmarkProximityFilter);
    d->coordinate = coordinate;
}

/*!
    Returns the radius of the filter.  the unit of the radius is meters.
*/
double QLandmarkProximityFilter::radius() const
{
    Q_D(const QLandmarkProximityFilter);
    return d->radius;
}

/*!
    Sets the \a radius of the filter.  The unit of the radius is meters.
*/
void QLandmarkProximityFilter::setRadius(double radius)
{
    Q_D(QLandmarkProximityFilter);
    d->radius = radius;
}

/*!
    Returns the selection type of the proximity filter.
    By default the proximity filter will be \c SelectAll to select all landmarks
    with a given proximity.
*/
QLandmarkProximityFilter::Selection QLandmarkProximityFilter::selection() const
{
    Q_D(const QLandmarkProximityFilter);
    return d->selection;
}

/*!
    Sets the \a selection type of the proximity filter.
*/
void QLandmarkProximityFilter::setSelection(QLandmarkProximityFilter::Selection selection)
{
    Q_D(QLandmarkProximityFilter);
    d->selection = selection;
}

/*******************************************************************************
*******************************************************************************/

QLandmarkProximityFilterPrivate::QLandmarkProximityFilterPrivate(const QGeoCoordinate &coordinate, double radius)
        : QLandmarkFilterPrivate(),
        coordinate(coordinate),
        radius(radius)
{
    type = QLandmarkFilter::ProximityFilter;
    selection = QLandmarkProximityFilter::SelectAll;
}

QLandmarkProximityFilterPrivate::QLandmarkProximityFilterPrivate(const QLandmarkProximityFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        coordinate(other.coordinate),
        radius(other.radius),
        selection(other.selection)
        {}

QLandmarkProximityFilterPrivate::~QLandmarkProximityFilterPrivate() {}

QTM_END_NAMESPACE
