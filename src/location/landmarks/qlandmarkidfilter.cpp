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

#include "qlandmarkidfilter.h"
#include "qlandmarkidfilter_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkIdFilter
    \brief The QLandmarkIdFilter class is used to search for landmarks based
           on a list of landmark identifiers.

    \inmodule QtLocation
    
    \ingroup landmarks-filter
*/

/*!
    \enum QLandmarkIdFilter::MatchingScheme
    Defines the matching behavior of the id filter.
    \value MatchAll A landmark must be returned for every id assigned to the filter.
                    Failure to match every id results in a QLandmarkManager::DoesNotExistError.
    \value MatchSubset Every id does not have to be matched to a landmark.  Matching
                       only a subset of the assigned ids is acceptable.
*/

Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkIdFilter)

/*!
    Creates a filter that selects landmarks using a list of landmark \a ids and a
    \a matchingScheme.
*/
QLandmarkIdFilter::QLandmarkIdFilter(const QList<QLandmarkId> &ids, MatchingScheme matchingScheme)
        : QLandmarkFilter(new QLandmarkIdFilterPrivate(ids, matchingScheme)) {}

/*!
    \fn QLandmarkIdFilter::QLandmarkIdFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new id filter.
*/

/*!
    Destroys the filter.
*/
QLandmarkIdFilter::~QLandmarkIdFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the list of landmark identifiers that the filter will use to determine matches.
*/
QList<QLandmarkId> QLandmarkIdFilter::landmarkIds() const
{
    Q_D(const QLandmarkIdFilter);
    return d->landmarkIds;
}

/*!
    Sets the \a ids that the filter will use to determine matches.
*/
void QLandmarkIdFilter::setLandmarkIds(const QList<QLandmarkId> &ids)
{
    Q_D(QLandmarkIdFilter);
    d->landmarkIds = ids;
}

/*!
    Adds \a id to the list of landmark ids this filter searches for.
*/
void QLandmarkIdFilter::append(const QLandmarkId &id)
{
    Q_D(QLandmarkIdFilter);
    d->landmarkIds.append(id);
}

/*!
    Removes \a id to the list of landmark ids this filter searches for.
*/
void QLandmarkIdFilter::remove(const QLandmarkId &id)
{
    Q_D(QLandmarkIdFilter);
    d->landmarkIds.removeAll(id);
}

/*!
    Removes all identifiers from the filter.
*/
void QLandmarkIdFilter::clear()
{
    Q_D(QLandmarkIdFilter);
    d->landmarkIds.clear();
}

/*!
    Appends the given \a id to the list of landmark identifiers
    this filter searches for.
*/
QLandmarkIdFilter &QLandmarkIdFilter::operator<<(const QLandmarkId &id)
{
    Q_D(QLandmarkIdFilter);
    d->landmarkIds.append(id);
    return *this;
}

/*!
    Returns the matching scheme of the filter.

    The matching scheme specifies whether the filter needs to match all
    landmark ids or only a subset.  The default scheme is \c MatchSubset.
*/
QLandmarkIdFilter::MatchingScheme QLandmarkIdFilter::matchingScheme() const
{
    Q_D(const QLandmarkIdFilter);
    return d->matchingScheme;
}

/*!
    Sets the \a matchingScheme of the filter.
*/
void QLandmarkIdFilter::setMatchingScheme(QLandmarkIdFilter::MatchingScheme matchingScheme)
{
    Q_D(QLandmarkIdFilter);
    d->matchingScheme = matchingScheme;
}

/*******************************************************************************
*******************************************************************************/

QLandmarkIdFilterPrivate::QLandmarkIdFilterPrivate(const QList<QLandmarkId> &ids, QLandmarkIdFilter::MatchingScheme scheme)
        : landmarkIds(ids), matchingScheme(scheme)
{
    type = QLandmarkFilter::LandmarkIdFilter;
}

QLandmarkIdFilterPrivate::QLandmarkIdFilterPrivate(const QLandmarkIdFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        landmarkIds(other.landmarkIds),
        matchingScheme(other.matchingScheme)
{
}

QLandmarkIdFilterPrivate::~QLandmarkIdFilterPrivate() {}

QTM_END_NAMESPACE