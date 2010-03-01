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

#include "qlandmarkfilter.h"

//TODO: move this when category filter is moved.
#include "qlandmarkcategoryid.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkFilter
    \brief The QLandmarkFilter class is used to select landmarks made available
    through QLandmarkManager.
    \ingroup location

    This class is used as a parameter to various functions offered by
    QLandmarkManager, to allow selection of landmarks which have certain
    details or properties.
*/

/*!
    \enum QLandmarkFilter::FilterType
    Describes the type of the filter
    \value InvalidFilter, An invalid filter which matches nothing
    \value LandmarkNameFilter A filter which matches landmarks based on name
    \value LandmarkProximityFilter A filter which matches landmarks within a certain range of a given
           coordinate.
    \value LandmarkNearestFilter A filter which matches a landmark closest to a given coordinate.
    \value LandmarkCategoryFilter A filter which matches landmarks that belong to a given category
    \value LandmarkBoxFilter A filter which matches landmarks within a given bounding box.
    \value LandmarkCustomFilter A filter which matches landmarks based on a custom made algorithm.
*/

/*!
    Constructs an empty filter
*/
QLandmarkFilter::QLandmarkFilter()
{
    //TODO: implement
}

/*!
    Destroys the filter
*/
QLandmarkFilter::~QLandmarkFilter()
{
    //TODO: implement
}

/*!
    Returns the type of the filter
*/
QLandmarkFilter::FilterType QLandmarkFilter::type() const
{
    return QLandmarkFilter::InvalidFilter;
}

/*!
    Returns the maximum number of matches this filter will make.
*/
int QLandmarkFilter::maxMatches() const
{
    return 0;
}

/*!
    Set the maximum number of matches this filter will make
    to \a maxMatches.
*/
void QLandmarkFilter::setMaxMatches(int maxMatches)
{
    //TODO: implement
}


/*!
    Returns true if this filter is not identical to \a other.
*/
bool QLandmarkFilter::operator!=(const QLandmarkFilter &other) const
{
    return true;
}

/*!
    Returns true if the filter has the same type and criteria as \a other
*/
bool QLandmarkFilter::operator==(const QLandmarkFilter &other) const
{
    return false;
}

/*!
    Assigns \a other to this filter and returns a reference to this filter
*/
QLandmarkFilter &QLandmarkFilter::operator=(const QLandmarkFilter &other)
{
    return *this;
}

/*!
    \class QLandmarkNameFilter
    \brief The QLandmarkNameFilter class is used to search for landmarks by name.
*/

/*!
    Creates a filter that selects landmarks by \a name.
*/
QLandmarkNameFilter::QLandmarkNameFilter(const QString &name)
{
    //TODO: implement
}

/*!
    Destroys the filter
*/
QLandmarkNameFilter::~QLandmarkNameFilter()
{
    //TODO: implement
}

/*!
    Returns the name that the filter will use to determine matches.
*/
QString QLandmarkNameFilter::name()
{
    return QString();
}

/*!
    Sets the \a name that the filter will use to determine matches.
*/
void QLandmarkNameFilter::setName(const QString &name)
{
}


/*!
    \class QLandmarkProximityFilter
    \brief The QLandmarkProximityFilter class is used to search for landmarks based on proximity
    around a given coordinate.
    \ingroup location

    This filter will make matches for landmarks which are found within a given radius
    around a central coordinate.
*/

/*!
    Creates a filter that will select landmarks within a given \a radius around
    a central \a coordinate.
*/
QLandmarkProximityFilter::QLandmarkProximityFilter(const QGeoCoordinate &coordinate, double radius)
{
    //TODO: implement
}

/*!
    Destroys the filter.
*/
QLandmarkProximityFilter::~QLandmarkProximityFilter()
{
    //TODO: implement
}

/*!
    Returns the central coordinate of the filter.
*/
QGeoCoordinate QLandmarkProximityFilter::coordinate()
{
    return QGeoCoordinate();
}

/*!
    Sets the central \a coordinate of the filter.
*/
void QLandmarkProximityFilter::setCoordinate(const QGeoCoordinate &coordinate)
{
}

/*!
    Returns the radius of the filter.
*/
double QLandmarkProximityFilter::radius()
{
    return 0.0;
}

/*!
    Sets the \a radius of the filter.
*/
void QLandmarkProximityFilter::setRadius(double radius)
{
}

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
{
    //TODO: implement
}

/*!
    Destroys the filter.
*/
QLandmarkNearestFilter::~QLandmarkNearestFilter()
{
    //TODO: implement
}

/*!
    Returns the central coordinate of the filter.
*/
QGeoCoordinate QLandmarkNearestFilter::coordinate() const
{
    return QGeoCoordinate();
}

/*!
    Sets the central \a coordinate of the filter.
*/
void QLandmarkNearestFilter::setCoordinate(const QGeoCoordinate &coordinate)
{
}

/*!
    \class QLandmarkCategoryFilter
    \brief The QLandmarkCategoryFilter class is used to search for landmarks that
    belong to a certain category.
    \ingroup location
*/

/*!
    Creates a filter that searches for landmarks that belong to the category
    identified by \a categoryId.
*/
QLandmarkCategoryFilter::QLandmarkCategoryFilter(const QLandmarkCategoryId &categoryId)
{
    //TODO: implement
}

/*!
    Destroys the filter.
*/
QLandmarkCategoryFilter::~QLandmarkCategoryFilter()
{
    //TODO: implement
}

/*!
    Returns the category identifier that this filter matches by.
*/
QLandmarkCategoryId QLandmarkCategoryFilter::category() const
{
    return QLandmarkCategoryId();
}

/*!
    Sets the category identifier that this filter matches by
    to \a categoryId.
*/
void QLandmarkCategoryFilter::setCategoryId(const QLandmarkCategoryId &categoryId)
{
}

/*!
    \class QLandmarkCustomFilter
    \brief The QLandmarkCustomFilter class provides the interface which
    all custom filters should inherit.
    \ingroup location

    All custom landmark filters should inherit off the QLandmarkCustomFilter
    and must implement the isMatch() function.
*/

/*!
    Creates a custom landmark filter.
*/
QLandmarkCustomFilter::QLandmarkCustomFilter()
{
}

/*!
    Destroys the filter.
*/
QLandmarkCustomFilter::~QLandmarkCustomFilter()
{
}

/*!
    Returns true if the landmark identified by \a landmarkId
    should be matched by the filter.  All custom landmark filters
    need to implement this function.
*/
bool QLandmarkCustomFilter::isMatch(const QLandmarkId &landmarkId)
{
    return false;
}


/*!
    \class QLandmarkSortOrder
    \brief The QLandmarkSortOrder class defines how a list of landmarks
    should be ordered according to some criteria.
    \ingroup location
*/

/*!
    \enum QLandmarkSortOrder::SortType
    Defines the type of sort order.
    \value LandmarkNameFilter   Sorts landmarks by name.
    \value LandmarkDistanceFilter   Sorts landmarks by distance from a particular
           coordinate.
    \value LandmarkCustomFilter Is a custom sorting filter.
*/

/*!
    Constructs a landmark sort order.
*/
QLandmarkSortOrder::QLandmarkSortOrder()
{
}


/*!
    Returns the type of sort.
*/
QLandmarkSortOrder::SortType QLandmarkSortOrder::type() const
{
    return QLandmarkSortOrder::LandmarkCustomFilter;
}

/*!
    Returns the direction of the sort order.
*/
Qt::SortOrder QLandmarkSortOrder::direction() const
{
    return Qt::AscendingOrder;
}

/*!
    Sets the \a direction of the sort order.
*/
void QLandmarkSortOrder::setDirection(Qt::SortOrder direction)
{
}

/*!
    \fn int QLandmarkSortOrder::compare(const QLandmark &l1, const QLandmark &l2) const

    Compares \a l1 and \a l2 and returns an integer less than, equal to, or greater than
    zero if \a l1 is less than, equal to, or greater than \a l2.
*/

/*!
    \fn QList<QLandmarkId> QLandmarkSortOrder::sort(const QList<QLandmarkId> &landmarkIds) const

    Returns a sorted list of \a landmarkIds.
*/

/*!
    \class QLandmarkNameSort
    \brief The QLandmarkNameSort class is used to sort landmarks by name.
    \ingroup location
*/

/*!
    Creates a sort order that sorts by name in the given \a direction.
*/
QLandmarkNameSort::QLandmarkNameSort(Qt::SortOrder direction)
{
    //TODO: implement
}

/*!
    Destroys the sort order.
*/
QLandmarkNameSort::~QLandmarkNameSort()
{
    //TODO: implement
}

/*!
  \reimp
*/
int QLandmarkNameSort::compare(const QLandmark &l1, const QLandmark &l2) const
{
    return 0; //TODO: implement
}

/*!
    \reimp
*/
QList<QLandmarkId> QLandmarkNameSort::sort(const QList<QLandmarkId> &landmarkIds) const
{
    return QList<QLandmarkId>();
}

/*!
    \class QLandmarkDistanceSort
    \brief The QLandmarkDistanceSort class is used to sort landmarks by distance
    from a central coordinate.
    \ingroup location
*/
// ----- QLandmarkDistanceSort -----

/*!
    Creates a sort order that sorts landmarks by distance from a central \a coordinate.
    If the \a direction is Qt::Ascending, it means that landmarks are listed in
    increasing order of distance.
*/
QLandmarkDistanceSort::QLandmarkDistanceSort(QGeoCoordinate coordinate, Qt::SortOrder direction)
{
    //TODO: implement
}

/*!
    Destroys the sort order.
*/
QLandmarkDistanceSort::~QLandmarkDistanceSort()
{
    //TODO: implement
}

/*!
    Returns the central coordinate of the sort order
*/
QGeoCoordinate QLandmarkDistanceSort::coordinate() const
{
    return QGeoCoordinate();
}

/*!
    Sets the central \a coordinate of the sort order.
*/
void QLandmarkDistanceSort::setCoordinate(const QGeoCoordinate &coordinate)
{
}
/*!
    \reimp
*/
int QLandmarkDistanceSort::compare(const QLandmark &l1, const QLandmark &l2) const
{
    return 0;  //TODO: implement
}

/*!
    \reimp
*/

QList<QLandmarkId> QLandmarkDistanceSort::sort(const QList<QLandmarkId> &landmarkIds) const
{
    return QList<QLandmarkId>();
}

/*!
    \class QLandmarkCustomSort
    \brief The QLandmarkCustomSort class is the interface which all
    custom sort orders should inherit.

    All custom sort orders should inherit this class and implement the
    compare() and sort() functions.
*/

/*!
    Constructs a custom landmark sort order.
*/
QLandmarkCustomSort::QLandmarkCustomSort()
{
}

/*!
    Destroys the sort order.
*/
QLandmarkCustomSort::~QLandmarkCustomSort()
{
}

QTM_END_NAMESPACE
