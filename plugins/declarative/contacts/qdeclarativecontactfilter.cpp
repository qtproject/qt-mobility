/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qdeclarativecontactfilter_p.h"

/*!
   \qmlclass Filter QDeclarativeContactFilter
   \brief The Filter element is used as a property of ContactModel, to allow
   selection of contacts which have certain details or properties.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactFilter
 */


/*!
  \qmlproperty enumeration Filter::type

  This property holds the type value of this filter. It can be one of:

  \list
  \o Filter.DefaultFilter - A filter which matches everything (default).
  \o Filter.InvalidFilter - An invalid filter which matches nothing.
  \o Filter.DetailFilter - A filter which matches contacts containing one or more details of a particular definition with a particular value.
  \o Filter.DetailRangeFilter - A filter which matches contacts containing one or more details of a particular definition whose values are within a particular range.
  \o Filter.ChangeLogFilter - A filter which matches contacts whose timestamps have been updated since some particular date and time.
  \o Filter.ActionFilter - A filter which matches contacts for which a particular action is available, or which contain a detail with a particular value for which a particular action is available.
  \o Filter.RelationshipFilter - A filter which matches contacts which participate in a particular type of relationship, or relationship with a specified contact.
  \o Filter.IntersectionFilter - A filter which matches all contacts that are matched by all filters it includes.
  \o Filter.UnionFilter - A filter which matches any contact that is matched by any of the filters it includes.
  \o Filter.IdFilter - A filter which matches any contact whose local id is contained in a particular list of contact local ids.
  \endlist
  */

/*!
   \qmlclass ActionFilter QDeclarativeContactActionFilter
   \brief The ActionFilter element provides a filter based around an action availability criterion.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactActionFilter
 */

/*!
  \qmlproperty string ActionFilter::actionName

  This property holds the action name criterion of the filter.
  */

/*!
   \qmlclass ChangeLogFilter QDeclarativeContactChangeLogFilter
   \brief The ChangeLogFilter element provides a filter based around a contact timestamp criterion.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactChangeLogFilter
 */

/*!
  \qmlproperty date ChangeLogFilter::since

  This property holds the date and time lower-bound criterion of the filter.The value can be one of:
  \list
  \o ChangeLogFilter.EventAdded
  \o ChangeLogFilter.EventChanged
  \o ChangeLogFilter.EventRemoved
  \endlist
  */

/*!
  \qmlproperty enumeration ChangeLogFilter::eventType

  This property holds the type of change that this filter will match against.
  */

/*!
   \qmlclass DetailFilter QDeclarativeContactDetailFilter
   \brief The DetailFilter element provides a filter based around a detail value criterion.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactDetailFilter
 */

/*!
  \qmlproperty enumeration DetailFilter::detail

  This property holds the detail type of which details will be matched to.

  \sa ContactDetail::type
  */

/*!
  \qmlproperty int DetailFilter::field

  This property holds the detail field type of which detail fields will be matched to.
  Detail field types are enumeration values defined in each detail elements.

  \sa Address
  \sa Anniversary
  \sa Avatar
  \sa Birthday
  \sa DisplayLabel
  \sa EmailAddress
  \sa Family
  \sa Favorite
  \sa Gender
  \sa GeoLocation
  \sa GlobalPresence
  \sa Guid
  \sa Name
  \sa Nickname
  \sa Note
  \sa OnlineAccount
  \sa Organization
  \sa PhoneNumber
  \sa Presence
  \sa Ringtone
  \sa SyncTarget
  \sa Tag
  \sa Timestamp
  \sa Url
  \sa Hobby
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
   \qmlclass DetailRangeFilter QDeclarativeContactDetailRangeFilter
   \brief The DetailRangeFilter element provides a filter based around a detail value range criterion.


   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactDetailRangeFilter
 */

/*!
  \qmlproperty enumeration DetailRangeFilter::detail

  This property holds the detail type of which details will be matched to.

  \sa ContactDetail::type
  \sa DetailFilter::detail
  */

/*!
  \qmlproperty int DetailRangeFilter::field

  This property holds the detail field type of which detail fields will be matched to.
  Detail field types are enumeration values defined in each detail elements.

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

  This property holds a set of flags which defines the boundary condition semantics of the value range criterion.The valid range flags include:
    \list
    \o DetailRangeFilter.IncludeLower
    \o DetailRangeFilter.IncludeUpper
    \o DetailRangeFilter.ExcludeLower
    \o DetailRangeFilter.ExcludeUpper
    \endlist
  */
/*!
   \qmlclass IntersectionFilter QDeclarativeContactIntersectionFilter
   \brief The IntersectionFilter element provides a filter which intersects the results of other filters.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactIntersectionFilter
 */

/*!
  \qmlproperty QDeclarativeListProperty IntersectionFilter::filters

  This property holds the list of filters which form the intersection filter.
  */


/*!
   \qmlclass UnionFilter QDeclarativeContactUnionFilter
   \brief The UnionFilter element provides a filter which unions the results of other filters.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactUnionFilter
 */

/*!
  \qmlproperty QDeclarativeListProperty UnionFilter::filters

  This property holds the list of filters which form the union filter.
  */

/*!
   \qmlclass IdFilter QDeclarativeContactLocalIdFilter
   \brief The IdFilter element provides a filter based around a list of contact ids.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactLocalIdFilter
 */

/*!
  \qmlproperty list<string> IdFilter::ids

  This property holds the list of ids of contacts which match this filter.
  */


/*!
   \qmlclass RelationshipFilter QDeclarativeContactRelationshipFilter
   \brief The RelationshipFilter element provides a filter based around relationship criteria.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa Relationship
   \sa RelationshipModel
   \sa QContactRelationshipFilter
   \sa QContactRelationship
 */


/*!
  \qmlproperty variant RelationshipFilter::relationshipType

  This property holds the type of relationship that a contact must have in order to match the filter.

  \sa Relationship::type
 */

/*!
  \qmlproperty int RelationshipFilter::relatedContactId

  This property holds the id of the contact with whom the tested contact must have a relationship in order for the tested contact to match this filter
  */
/*!
  \qmlproperty enumeration RelationshipFilter::relatedContactRole

  This property holds the role in the relationship with the tested contact that the related contact must play in order for the tested contact to match this filter.
  The role can be one of:
  \list
  \o Relationship.First - The contact is the first contact in the relationship.
  \o Relationship.Second - The contact is the second contact in the relationship.
  \o Relationship.Either - The contact is either the first or second contact in the relationship (default).
  \endlist
  */

/*!
   \qmlclass InvalidFilter QDeclarativeContactInvalidFilter
   \brief the InvalidFilter element provides a filter which will never match any contacts.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactInvalidFilter
 */
