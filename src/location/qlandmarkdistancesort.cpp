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

#include "qlandmarkdistancesort.h"
#include "qlandmarkdistancesort_p.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkDistanceSort
    \brief The QLandmarkDistanceSort class is used to sort landmarks by distance
    from a given coordinate.
    \ingroup landmarks-sort
*/

Q_IMPLEMENT_LANDMARKSORTORDER_PRIVATE(QLandmarkDistanceSort)

/*!
    Creates a sort order that sorts landmarks by distance from a given \a coordinate.
    If the \a direction is Qt::Ascending, it means that landmarks are listed in
    increasing order of distance.
*/
QLandmarkDistanceSort::QLandmarkDistanceSort(const QGeoCoordinate &coordinate, Qt::SortOrder direction)
        : QLandmarkSortOrder(new QLandmarkDistanceSortPrivate())
{
    Q_D(QLandmarkDistanceSort);
    d->origin = coordinate;
    d->order = direction;
}

/*!
    Destroys the sort order.
*/
QLandmarkDistanceSort::~QLandmarkDistanceSort()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the coordinate to sort distances from.
*/
QGeoCoordinate QLandmarkDistanceSort::coordinate() const
{
    Q_D(const QLandmarkDistanceSort);
    return d->origin;
}

/*!
    Sets the \a coordinate to sort distances from.
*/
void QLandmarkDistanceSort::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QLandmarkDistanceSort);
    d->origin = coordinate;
}

/*******************************************************************************
*******************************************************************************/

QLandmarkDistanceSortPrivate::QLandmarkDistanceSortPrivate()
        : QLandmarkSortOrderPrivate(),
         origin(QGeoCoordinate())
{
    type = QLandmarkSortOrder::DistanceSort;
}

QLandmarkDistanceSortPrivate::QLandmarkDistanceSortPrivate(const QLandmarkDistanceSortPrivate &other)
        : QLandmarkSortOrderPrivate(other),
        origin(other.origin)
{
}

QLandmarkDistanceSortPrivate::~QLandmarkDistanceSortPrivate() {}

QTM_END_NAMESPACE
