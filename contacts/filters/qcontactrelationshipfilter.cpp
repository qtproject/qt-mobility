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

#include "qcontactrelationshipfilter.h"
#include "qcontactrelationshipfilter_p.h"
#include "qcontactmanager.h"

/*!
 * \class QContactRelationshipFilter
 * \brief The QContactRelationshipFilter class provides a filter based around relationship criteria
 *
 * It may be used to select contacts which are involved in relationships which are of a certain type, or which involve certain contacts.
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactRelationshipFilter)

/*!
 * \enum QContactRelationshipFilter::Role
 * Describes the roles that a contact may take in a relationship
 * \value First The contact is the first contact in the relationship
 * \value Second The contact is the second contact in the relationship
 * \value Either The contact is either the first or second contact in the relationship
 */

/*!
 * \fn QContactRelationshipFilter::QContactRelationshipFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, else constructs a new QContactRelationshipFilter.
 */

/*!
 * Constructs a new relationship filter
 */
QContactRelationshipFilter::QContactRelationshipFilter()
    : QContactFilter(new QContactRelationshipFilterPrivate)
{
}

/*!
 * Sets the role in the relationship that a contact must be in order to match this filter to \a roleInRelationship
 */
void QContactRelationshipFilter::setRole(QContactRelationshipFilter::Role roleInRelationship)
{
    Q_D(QContactRelationshipFilter);
    d->m_roleInRelationship = roleInRelationship;
}

/*!
 * Sets the type of relationship which a contact must have in order to match this filter to \a relationshipType
 */
void QContactRelationshipFilter::setRelationshipType(const QString& relationshipType)
{
    Q_D(QContactRelationshipFilter);
    d->m_relationshipType = relationshipType;
}

/*!
 * Sets the contact id of the other participant which must be present in the relationship with the contact
 * in order for the contact to match the filter to be \a id
 */
void QContactRelationshipFilter::setOtherParticipantId(const QContactId& id)
{
    Q_D(QContactRelationshipFilter);
    d->m_otherParticipantId = id;
}

/*!
 * Returns the role that a contact must have in a relationship in order to match the filter
 */
QContactRelationshipFilter::Role QContactRelationshipFilter::role() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_roleInRelationship;
}

/*!
 * Returns the type of relationship that a contact must have in order to match the filter
 */
QString QContactRelationshipFilter::relationshipType() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_relationshipType;
}

/*!
 * Returns the id of another contact with whom a contact must have a relationship in order to match the filter
 */
QContactId QContactRelationshipFilter::otherParticipantId() const
{
    Q_D(const QContactRelationshipFilter);
    return d->m_otherParticipantId;
}
