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

#include "qlandmarknearestfilter.h"
#include "qlandmarknearestfilter_p.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkNearestFilter
    \brief The QLandmarkNearestFilter class is used to search for the landmark nearest
    to a given coordinate.
    \ingroup location
*/

/*!
    Creates a filter that will match the landmark nearest to a given central
    \a coordinate.
*/
QLandmarkNearestFilter::QLandmarkNearestFilter(const QGeoCoordinate &coordinate)
    : QLandmarkFilter(new QLandmarkNearestFilterPrivate(coordinate))
{
}

/*!
    Destroys the filter.
*/
QLandmarkNearestFilter::~QLandmarkNearestFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the central coordinate of the filter.
*/
QGeoCoordinate QLandmarkNearestFilter::coordinate() const
{
    Q_D(const QLandmarkNearestFilter);
    return d->coordinate;
}

/*!
    Sets the central \a coordinate of the filter.
*/
void QLandmarkNearestFilter::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QLandmarkNearestFilter);
    d->coordinate = coordinate;
}

/*******************************************************************************
*******************************************************************************/

QLandmarkNearestFilterPrivate::QLandmarkNearestFilterPrivate(const QGeoCoordinate &coordinate)
        : coordinate(coordinate)
{
    type = QLandmarkFilter::NearestFilter;
}

QLandmarkNearestFilterPrivate::QLandmarkNearestFilterPrivate(const QLandmarkNearestFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        coordinate(other.coordinate) {}

QLandmarkNearestFilterPrivate::~QLandmarkNearestFilterPrivate() {}

QTM_END_NAMESPACE
