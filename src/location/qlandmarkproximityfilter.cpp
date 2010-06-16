/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qlandmarkproximityfilter.h"
#include "qlandmarkproximityfilter_p.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkProximityFilter
    \brief The QLandmarkProximityFilter class is used to search for landmarks based on the radius
    around a given coordinate.
    \ingroup landmarks-filter
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
    Returns the radius of the filter.
*/
double QLandmarkProximityFilter::radius() const
{
    Q_D(const QLandmarkProximityFilter);
    return d->radius;
}

/*!
    Sets the \a radius of the filter.
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
