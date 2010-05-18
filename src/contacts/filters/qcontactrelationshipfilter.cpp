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

#include "qcontactrelationshipfilter.h"
#include "qcontactrelationshipfilter_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactRelationshipFilter
  \brief The QContactRelationshipFilter class provides a filter based
  around relationship criteria.
  
  \ingroup contacts-filters
 
  It may be used to select contacts which are involved in relationships
  which are of a certain type, or which involve certain contacts.

  One common use-case might be to select the contacts which are a member of a particular group.
  This use-case may be met with the following filter:

  \code
  QContactRelationshipFilter groupFilter;                               // select all contacts which are involved
  groupFilter.setRelationshipType(QContactRelationship::HasMember);     // in a group relationship
  groupFilter.setRelatedContactId(groupContact.id());                   // with the group contact
  groupFilter.setRelatedContactRole(QContactRelationshipFilter::First); // where the group contact is the first participant
  \endcode

  Another common use-case might be to select the groups which a particular contact is a member of.
  This use-case may be met with the following filter:

  \code
  QContactRelationshipFilter whichGroupsFilter;                                 // select all contacts which are involved
  whichGroupsFilter.setRelationshipType(QContactRelationshipFilter::HasMember); // in a group relationship
  whichGroupsFilter.setRelatedContactId(particularContact.id());                // with the particular contact
  whichGroupsFilter.setRelatedContactRole(QContactRelationshipFilter::Second);  // where the particular contact is the second participant
  \endcode

 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactRelationshipFilter)

/*!
  \deprecated
  \enum QContactRelationshipFilter::Role
  Describes the roles that a contact may take in a relationship.  DEPRECATED: use QContactRelationship::Role instead!
  \value First The contact is the first contact in the relationship
  \value Second The contact is the second contact in the relationship
  \value Either The contact is either the first or second contact in the relationship
 */

/*!
  \fn QContactRelationshipFilter::QContactRelationshipFilter(const QContactFilter& other)
  Constructs a copy of \a other if possible, else constructs a new QContactRelationshipFilter.
 */

/*!
  Constructs a new relationship filter
 */
QContactRelationshipFilter::QContactRelationshipFilter()
    : QContactFilter(new QContactRelationshipFilterPrivate)
{
}

/*!
  Sets the type of relationship which a contact must have in order to match this filter to \a relationshipType
 */
void QContactRelationshipFilter::setRelationshipType(const QString& relationshipType)
{
    Q_D(QContactRelationshipFilter);
    d->m_relationshipType = relationshipType;
}

/*!
  Returns the type of relationship that a contact must have in order to match the filter
 */
QString QContactRelationshipFilter::relationshipType() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_relationshipType;
}

/*!
  Sets the id of the contact with whom the tested contact must have a relationship in order for the tested contact to match this filter to be \a relatedContactId
 */
void QContactRelationshipFilter::setRelatedContactId(const QContactId &relatedContactId)
{
    Q_D(QContactRelationshipFilter);
    d->m_relatedContactId = relatedContactId;
}

/*!
  Returns the id of the contact with whom the tested contact must have a relationship in order for the tested contact to match this filter
 */
QContactId QContactRelationshipFilter::relatedContactId() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_relatedContactId;
}

/*!
  \deprecated
  Sets the role in the relationship with the tested contact that the related contact must play in order for the tested contact to match this filter to be \a relatedContactRole.
  This function is deprecated and will be removed after the transition period has elapsed.  Use setRelatedContactRole(QContactRelationship::Role role) instead!
 */
void QContactRelationshipFilter::setRelatedContactRole(QContactRelationshipFilter::Role relatedContactRole)
{
    Q_D(QContactRelationshipFilter);
    d->m_relatedContactRole = static_cast<QContactRelationship::Role>(relatedContactRole);
}

///*!
//  \deprecated
//  Returns the role in the relationship with the tested contact that the related contact must play in order for the tested contact to match this filter.
//  This function is deprecated and will be removed after the transition period has elapsed.  Use relatedContactRole() which returns a QContactRelationship::Role instead!
//  Note: since signature is the same, this function cannot be deprecated properly.  This causes a source break!
// */
//QContactRelationshipFilter::Role QContactRelationshipFilter::relatedContactRole() const
//{
//    Q_D(const QContactRelationshipFilter);
//    return d->m_relatedContactRole;
//}

/*!
  Sets the role in the relationship with the tested contact that the related contact must play in order for the tested contact to match this filter to be \a relatedContactRole
 */
void QContactRelationshipFilter::setRelatedContactRole(QContactRelationship::Role relatedContactRole)
{
    Q_D(QContactRelationshipFilter);
    d->m_relatedContactRole = relatedContactRole;
}

/*!
  Returns the role in the relationship with the tested contact that the related contact must play in order for the tested contact to match this filter
 */
QContactRelationship::Role QContactRelationshipFilter::relatedContactRole() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_relatedContactRole;
}

/*!
  \deprecated
  Sets the role in the relationship that a contact must be in order to match this filter to \a roleInRelationship

  This function has been deprecated - you should pass the opposite value (e.g. First instead of Second, Second
  instead of First) to \c setRelatedContactRole().
  \sa setRelatedContactRole()
 */
void QContactRelationshipFilter::setRole(QContactRelationshipFilter::Role roleInRelationship)
{
    Q_D(QContactRelationshipFilter);
    switch(roleInRelationship) {
        case QContactRelationshipFilter::Either:
            d->m_relatedContactRole = QContactRelationship::Either;
            break;
        case QContactRelationshipFilter::First:
            d->m_relatedContactRole = QContactRelationship::Second;
            break;
        case QContactRelationshipFilter::Second:
            d->m_relatedContactRole = QContactRelationship::First;
            break;
    }
}

/*!
  \deprecated
  Sets the contact id of the other participant which must be present in the relationship with the contact
  in order for the contact to match the filter to be \a id
 */
void QContactRelationshipFilter::setOtherParticipantId(const QContactId& id)
{
    Q_D(QContactRelationshipFilter);
    d->m_relatedContactId = id;
}

/*!
  \deprecated
  Returns the role that a contact must have in a relationship in order to match the filter
 */
QContactRelationshipFilter::Role QContactRelationshipFilter::role() const
{
    Q_D(const QContactRelationshipFilter);
    switch(d->m_relatedContactRole) {
        case QContactRelationshipFilter::First:
            return QContactRelationshipFilter::Second;
        case QContactRelationshipFilter::Second:
            return QContactRelationshipFilter::First;
        case QContactRelationshipFilter::Either:
        default:
            return QContactRelationshipFilter::Either;
    }
}

/*!
  \deprecated
  Returns the id of another contact with whom a contact must have a relationship in order to match the filter
 */
QContactId QContactRelationshipFilter::otherParticipantId() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_relatedContactId;
}

QTM_END_NAMESPACE
