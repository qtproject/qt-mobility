/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactrelationship.h"
#include "qcontactrelationship_p.h"
#include "qtcontactsglobal.h"

/*!
 * \class QContactRelationship
 * \brief Describes a relationship between two contacts.
 *
 * Each relationship is uniquely identified by the combination of the
 * left contact id, the left contact manager uri, the relationship type, the right contact id and the right contact manager uri.
 * That is, the priority of a relationship is not a defining datum.
 */

/*!
 * \variable QContactRelationship::RelationshipTypeIsMemberOf
 * The relationship type which identifies the left contact as a member of the group described by the right contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIsMemberOf, "IsMemberOf");

/*!
 * \variable QContactRelationship::RelationshipTypeIsAggregatedBy
 * The relationship type which identifies the left contact as being aggregated by the metacontact described by the right contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIsAggregatedBy, "IsAggregatedBy");

/*!
 * \variable QContactRelationship::RelationshipTypeIs
 * The relationship type which identifies the left contact as being the same contact as the right contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIs, "Is");

/*!
 * \variable QContactRelationship::RelationshipTypeAssistant
 * The relationship type which identifies the left contact as being the assistant of the right contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeAssistant, "Assistant");

/*!
 * \variable QContactRelationship::RelationshipTypeManager
 * The relationship type which identifies the left contact as being the manager of the right contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeManager, "Manager");

/*!
 * \variable QContactRelationship::RelationshipTypeSpouse
 * The relationship type which identifies the left contact as being the spouse of the right contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeSpouse, "Spouse");

/*!
 * Constructs a new relationship
 */
QContactRelationship::QContactRelationship()
        : d(new QContactRelationshipPrivate)
{
}

/*!
 * Frees the memory in use by the relationship
 */
QContactRelationship::~QContactRelationship()
{
}

/*!
 * Creates a copy of the \a other relationship
 */
QContactRelationship::QContactRelationship(const QContactRelationship& other)
        : d(other.d)
{
}

/*!
 * Assigns this relationship to be equal to \a other
 */
QContactRelationship& QContactRelationship::operator=(const QContactRelationship& other)
{
    d = other.d;
    return *this;
}

/*!
 * Returns true if this relationship is equal to the \a other relationship, otherwise returns false.
 * Ignores the priority of the relationship when calculating equality.
 */
bool QContactRelationship::operator==(const QContactRelationship &other) const
{
    if (d->m_leftId != other.d->m_leftId)
        return false;
    if (d->m_rightId != other.d->m_rightId)
        return false;
    if (d->m_leftManagerUri != other.d->m_leftManagerUri)
        return false;
    if (d->m_rightManagerUri != other.d->m_rightManagerUri)
        return false;
    if (d->m_relationshipType != other.d->m_relationshipType)
        return false;
    return true;
}

/*!
 * Returns the id of the contact which has a relationship of the given type with the right contact
 * \sa relationshipType(), rightId(), setLeftId()
 */
QUniqueId QContactRelationship::leftId() const
{
    return d->m_leftId;
}

/*!
 * Returns the id of the contact with which the left contact has a relationship of the given type
 * \sa relationshipType(), leftId(), setRightId()
 */
QUniqueId QContactRelationship::rightId() const
{
    return d->m_rightId;
}

/*!
 * Returns the URI of the manager in which the left contact is stored
 * \sa setLeftManagerUri()
 */
QString QContactRelationship::leftManagerUri() const
{
    return d->m_leftManagerUri;
}

/*!
 * Returns the URI of the manager in which the right contact is stored
 * \sa setRightManagerUri()
 */
QString QContactRelationship::rightManagerUri() const
{
    return d->m_rightManagerUri;
}

/*!
 * Returns the type of relationship which the left contact has with the right contact
 * \sa setRelationshipType()
 */
QString QContactRelationship::relationshipType() const
{
    return d->m_relationshipType;
}

/*!
 * Returns the priority of the relationship.  The priority of the relationship determines how important
 * the relationship is in comparison with other relationships.  A relationship with higher priority
 * is more important than a relationship with a lower priority.
 * \sa setPriority()
 */
int QContactRelationship::priority() const
{
    return d->m_priority;
}

/*!
 * Sets the id of the left contact in the relationship to \a id.  This contact has
 * a relationship of the specified type with the right contact.
 * \sa leftId()
 */
void QContactRelationship::setLeftId(const QUniqueId& id)
{
    d->m_leftId = id;
}

/*!
 * Sets the id of the right contact in the relationship to \a id.  The left contact
 * has a relationship of the specified type with this contact.
 * \sa rightId()
 */
void QContactRelationship::setRightId(const QUniqueId& id)
{
    d->m_rightId = id;
}

/*!
 * Sets the URI of the manager in which the left contact is stored to \a uri.
 * If the \a uri is empty, or if this function is not called prior to saving
 * the relationship in a manager, the manager will assume that the left contact
 * is stored by it.
 * \sa leftManagerUri()
 */
void QContactRelationship::setLeftManagerUri(const QString& uri)
{
    d->m_leftManagerUri = uri;
}

/*!
 * Sets the URI of the manager in which the right contact is stored to \a uri.
 * If the \a uri is empty, or if this function is not called prior to saving
 * the relationship in a manager, the manager will assume that the right contact
 * is stored by it.
 * \sa rightManagerUri()
 */
void QContactRelationship::setRightManagerUri(const QString& uri)
{
    d->m_rightManagerUri = uri;
}

/*!
 * Sets the type of relationship that the left contact has with the right relationship,
 * to \a relationshipType.
 * \sa relationshipType()
 */
void QContactRelationship::setRelationshipType(const QString& relationshipType)
{
    d->m_relationshipType = relationshipType;
}

/*!
 * Sets the priority of the relationship to \a priority.
 * \sa priority()
 */
void QContactRelationship::setPriority(int priority)
{
    d->m_priority = priority;
}
