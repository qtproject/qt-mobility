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
#include "qlandmarkfilter_p.h"

#include "qlandmarkintersectionfilter.h"
#include "qlandmarkunionfilter.h"

#include "qlandmarkcategoryid.h"
#include "qlandmarkid.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

QLandmarkFilterPrivate::QLandmarkFilterPrivate()
    : QSharedData(),
      type(QLandmarkFilter::DefaultFilter),
      maxMatches(-1)
{
}

QLandmarkFilterPrivate::QLandmarkFilterPrivate(const QLandmarkFilterPrivate &other)
    : QSharedData(other),
      type(other.type),
      maxMatches(other.maxMatches)
{
}

QLandmarkFilterPrivate::~QLandmarkFilterPrivate()
{
}

/*!
    \class QLandmarkFilter
    \brief The QLandmarkFilter class is used to select landmarks.
    \ingroup location

    This class is used as a parameter to search for landmarks using
    QLandmarkManager::landmarkIds() or a QLandmarkIdFetchRequest.  It allows
    selection of landmarks which meet certain criteria.


    The QLandmarkFilter is primarily intended to serve as the base class
    for all filter types.  However it also has the type of a QLandmark::DefaultFilter,
    and may thus be used as a filter to match all landmarks.

    If the existing collection of filter types is not sufficient then
    custom filter should be created by inheriting from QLandmarkCustomFilter.
*/

/*!
    \enum QLandmarkFilter::FilterType
    Describes the type of the filter
    \value LandmarkInvalidFilter, An invalid filter which matches nothing
    \value LandmarkNameFilter A filter which matches landmarks based on name
    \value LandmarkProximityFilter A filter which matches landmarks within a certain range of a given
           coordinate.
    \value LandmarkNearestFilter A filter which matches a landmark closest to a given coordinate.
    \value LandmarkCategoryFilter A filter which matches landmarks that belong to a given category
    \value LandmarkBoxFilter A filter which matches landmarks within a given bounding box.
    \value LandmarkIntersectionFilter A filter which performs an AND operation with its
                                      constituent filters
    \value LandmarkUnionFilter A filter which performs an OR operation with its constiuent
                               filters
    \value LandmarkDefaultFilter A filter which matches all landmarks.
    \value LandmarkCustomFilter A filter which matches landmarks based on a custom made algorithm.
*/

/*!
    Constructs an default landmark filter.
*/
QLandmarkFilter::QLandmarkFilter()
{
    //TODO: implement
}

QLandmarkFilter::QLandmarkFilter(const QLandmarkFilter &other)
    :d(other.d)
{
}

/*!
    \internal
*/
QLandmarkFilter::QLandmarkFilter(QLandmarkFilterPrivate &dd)
    :d(&dd)
{
    //TODO: implement
}

/*!
    Destroys the filter.
*/
QLandmarkFilter::~QLandmarkFilter()
{
    //TODO: implement
}

/*!
    Returns the type of the filter.
*/
QLandmarkFilter::FilterType QLandmarkFilter::type() const
{
    return QLandmarkFilter::InvalidFilter;
}

/*!
    Returns the maximum number of matches this filter will make.
*/
int QLandmarkFilter::maximumMatches() const
{
    return 0;
}

/*!
    Set the maximum number of matches this filter will make
    to \a maxMatches.
*/
void QLandmarkFilter::setMaximumMatches(int maxMatches)
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
    Returns true if the filter has the same type and criteria as \a other.
*/
bool QLandmarkFilter::operator==(const QLandmarkFilter &other) const
{
    return false;
}

/*!
    Assigns \a other to this filter and returns a reference to this filter.
*/
QLandmarkFilter &QLandmarkFilter::operator=(const QLandmarkFilter &other)
{
    return *this;
}

/*!
 \relates QLandmarkFilter
 Returns a filter which is the intersection of the \a left and \a right filters
 \sa QLandmarkIntersectionFilter
 */
const QLandmarkFilter operator&(const QLandmarkFilter& left, const QLandmarkFilter& right)
{
    /* TODO implement better handling when left or right is an intersection filter */

    /* usual fallback case */
    QLandmarkIntersectionFilter nif;
    nif << left << right;
    return nif;
}

/*!
 \relates QLandmarkFilter
 Returns a filter which is the union of the \a left and \a right filters
 \sa QLandmarkUnionFilter
 */
const QLandmarkFilter operator|(const QLandmarkFilter& left, const QLandmarkFilter& right)
{
    /* TODO implement better handling when left or right is a union filter */
    /* usual fallback case */
    QLandmarkUnionFilter nif;
    nif << left << right;
    return nif;
}

QTM_END_NAMESPACE
