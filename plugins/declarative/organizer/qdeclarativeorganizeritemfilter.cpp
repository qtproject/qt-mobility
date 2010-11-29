/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qdeclarativeorganizeritemfilter_p.h"
#include "qdeclarativeorganizermodel_p.h"
#include "qorganizeritemid.h"
#include "qorganizercollectionid.h"

/*!
   \qmlclass Filter QDeclarativeOrganizerItemFilter
   \brief The Filter element is used as a property of OrganizerModel, to allow
   selection of organizer items which have certain details or properties.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa QOrganizerItemFilter
 */


/*!
  \qmlproperty enumeration Filter::type

  This property holds the type value of this filter. It can be one of:

  \list
  \o Filter.DefaultFilter - A filter which matches everything (default).
  \o Filter.InvalidFilter - An invalid filter which matches nothing.
  \o Filter.DetailFilter - A filter which matches organizer items containing one or more details of a particular definition with a particular value.
  \o Filter.DetailRangeFilter - A filter which matches organizer items containing one or more details of a particular definition whose values are within a particular range.
  \o Filter.ChangeLogFilter - A filter which matches organizer items whose timestamps have been updated since some particular date and time.
  \o Filter.ActionFilter - A filter which matches organizer items for which a particular action is available, or which contain a detail with a particular value for which a particular action is available.
  \o Filter.CollectionFilter - A filter which matches any organizer item that is matched by collection.
  \o Filter.IntersectionFilter - A filter which matches all organizer items that are matched by all filters it includes.
  \o Filter.UnionFilter - A filter which matches any organizer item that is matched by any of the filters it includes.
  \o Filter.IdFilter - A filter which matches any organizer item whose local id is contained in a particular list of organizer item local ids.
  \endlist
  */

/*!
   \qmlclass ChangeLogFilter QDeclarativeOrganizerItemChangeLogFilter
   \brief The ChangeLogFilter element provides a filter based around a organizer item timestamp criterion.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa QOrganizerItemChangeLogFilter
 */

/*!
  \qmlproperty date ChangeLogFilter::since

  This property holds the date and time lower-bound criterion of the filter.
  */

/*!
  \qmlproperty enumeration ChangeLogFilter::eventType

  This property holds the type of change that this filter will match against. The value can be one of:
  \list
  \o ChangeLogFilter.EventAdded
  \o ChangeLogFilter.EventChanged
  \o ChangeLogFilter.EventRemoved
  \endlist
  */

/*!
   \qmlclass DetailFilter QDeclarativeOrganizerItemDetailFilter
   \brief The \l DetailFilter element provides a filter based around a detail value criterion.
   \qmlmodule organizer
   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa QOrganizerItemDetailFilter
 */

/*!
  \qmlproperty enumeration DetailFilter::detail

  This property holds the detail type of which details will be matched to.

  \sa ItemDetail::type
  */

/*!
  \qmlproperty int DetailFilter::field

  This property holds the detail field type of which detail fields will be matched to.
  Detail field types are enumeration values defined in each detail elements.

  \sa EventTime
  \sa JournalTime
  \sa TodoTime
  \sa TodoProgress
  \sa Reminder
  \sa AudibleReminder
  \sa VisualReminder
  \sa EmailReminder
  \sa Comment
  \sa Description
  \sa DisplayLabel
  \sa Guid
  \sa Location
  \sa Parent
  \sa Priority
  \sa Recurrence
  \sa Timestamp
  \sa Type
  \sa Tag
  */

/*!
  \qmlproperty variant DetailFilter::value

  This property holds the value criterion of the detail filter.
  */

/*!
  \qmlproperty enumeration DetailFilter::matchFlags

  This property holds the semantics of the value matching criterion. The valid match flags include:
  \list
  \o MatchExactly - Performs QVariant-based matching (default).
  \o MatchContains - The search term is contained in the item.
  \o MatchStartsWith - The search term matches the start of the item.
  \o MatchEndsWith - The search term matches the end of the item.
  \o MatchFixedString - Performs string-based matching. String-based comparisons are case-insensitive unless the \c MatchCaseSensitive flag is also specified.
  \o MatchCaseSensitive - The search is case sensitive.
  \endlist
  */

/*!
   \qmlclass DetailRangeFilter QDeclarativeOrganizerItemDetailRangeFilter
   \brief The DetailRangeFilter element provides a filter based around a detail value range criterion.


   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa QOrganizerItemDetailRangeFilter
 */

/*!
  \qmlproperty enumeration DetailRangeFilter::detail

  This property holds the detail type of which details will be matched to.

  \sa ItemDetail::type
  \sa DetailFilter::detail

  */

/*!
  \qmlproperty int DetailRangeFilter::field

  This property holds the detail field type of which detail fields will be matched to.
  Detail field types are enumeration values defined in each detail elements.

  \sa EventTime
  \sa JournalTime
  \sa TodoTime
  \sa TodoProgress
  \sa Reminder
  \sa AudibleReminder
  \sa VisualReminder
  \sa EmailReminder
  \sa Comment
  \sa Description
  \sa DisplayLabel
  \sa Guid
  \sa Location
  \sa Parent
  \sa Priority
  \sa Recurrence
  \sa Timestamp
  \sa Type
  \sa Tag

  \sa DetailFilter::field
  */
/*!
  \qmlproperty variant DetailRangeFilter::min

  This property holds the lower bound of the value range criterion. By default, there is no lower bound.
  */
/*!
  \qmlproperty variant DetailRangeFilter::max

  This property holds the upper bound of the value range criterion. By default, there is no upper bound.
  */

/*!
  \qmlproperty enumeration DetailRangeFilter::matchFlags

  This property holds the match flags of the criterion, which define semantics such as case sensitivity, and exact matching.
  \sa DetailFilter::matchFlags
  */

/*!
  \qmlproperty enumeration DetailRangeFilter::rangeFlags

  This property holds a set of flags which defines the boundary condition semantics of the value range criterion. The valid range flags include:
    \list
    \o DetailRangeFilter.IncludeLower
    \o DetailRangeFilter.IncludeUpper
    \o DetailRangeFilter.ExcludeLower
    \o DetailRangeFilter.ExcludeUpper
    \endlist
  */

/*!
   \qmlclass IntersectionFilter QDeclarativeOrganizerItemIntersectionFilter
   \brief The IntersectionFilter element provides a filter which intersects the results of other filters.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa QOrganizerItemIntersectionFilter
 */

/*!
  \qmlproperty QDeclarativeListProperty IntersectionFilter::filters

  This property holds the list of filters which form the intersection filter.
  */


/*!
   \qmlclass UnionFilter QDeclarativeOrganizerItemUnionFilter
   \brief The UnionFilter element provides a filter which unions the results of other filters.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa QOrganizerItemUnionFilter
 */

/*!
  \qmlproperty QDeclarativeListProperty UnionFilter::filters

  This property holds the list of filters which form the union filter.
  */

/*!
   \qmlclass IdFilter QDeclarativeOrganizerItemIdFilter
   \brief The IdFilter element provides a filter based around a list of organizer item ids.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa {QOrganizerItemIdFilter}
 */

/*!
  \qmlproperty list<string> IdFilter::ids

  This property holds the list of ids of organizer items which match this filter.
  */


/*!
   \qmlclass CollectionFilter QDeclarativeOrganizerItemCollectionFilter
   \brief The CollectionFilter element provides a filter based around a list of organizer item ids.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.
   \sa Collection
 */


/*!
  \qmlproperty list<variant> CollectionFilter::ids

  This property holds the list of ids of organizer items which match this filter.

 */

/*!
   \qmlclass InvalidFilter QDeclarativeOrganizerItemInvalidFilter
   \brief the InvalidFilter element provides a filter which will never match any organizer items.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa QOrganizerItemInvalidFilter
 */


QOrganizerItemFilter QDeclarativeOrganizerItemIdFilter::filter() const
{
    QOrganizerItemIdFilter f;
    QList<QOrganizerItemId> ids;

    foreach(const QVariant& id, m_ids) {
        QOrganizerItemId itemId = QDeclarativeOrganizerModel::itemIdFromHash(id.value<uint>());
        if (!itemId.isNull())
            ids << itemId;
    }

    f.setIds(ids);
    return f;
}
QOrganizerItemFilter QDeclarativeOrganizerItemCollectionFilter::filter() const
{
    QOrganizerItemCollectionFilter f;
    QSet<QOrganizerCollectionId> ids;

    foreach(const QVariant& id, m_ids) {
        QOrganizerCollectionId cId = QDeclarativeOrganizerModel::collectionIdFromHash(id.value<uint>());
        if (!cId.isNull())
            ids << cId;
    }

    f.setCollectionIds(ids);
    return f;
}
