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
 * \brief Describes a one-to-many relationship between a locally-stored contact and a list of other (possibly remote) contacts.
 *
 * Each relationship is uniquely identified by the combination of the source id and the relationship type.
 */

/*!
 * \variable QContactRelationship::RelationshipTypeHasMember
 * The relationship type which identifies the source contact as being a group which includes each of the involved contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeHasMember, "HasMember");

/*!
 * \variable QContactRelationship::RelationshipTypeAggregates
 * The relationship type which identifies the source contact as aggregating the involved contacts into a metacontact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIsAggregates, "Aggregates");

/*!
 * \variable QContactRelationship::RelationshipTypeIs
 * The relationship type which identifies the source contact as being the same contact as the involved contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIs, "Is");

/*!
 * \variable QContactRelationship::RelationshipTypeAssistant
 * The relationship type which identifies the source contact as being the assistant of the involved contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeAssistant, "Assistant");

/*!
 * \variable QContactRelationship::RelationshipTypeManager
 * The relationship type which identifies the source contact as being the manager of the involved contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeManager, "Manager");

/*!
 * \variable QContactRelationship::RelationshipTypeSpouse
 * The relationship type which identifies the source contact as being the spouse of the involved contacts
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
 */
bool QContactRelationship::operator==(const QContactRelationship &other) const
{
    if (d->m_sourceId != other.d->m_sourceId)
        return false;
    if (d->m_involved != other.d->m_involved)
        return false;
    if (d->m_relationshipType != other.d->m_relationshipType)
        return false;
    return true;
}

/*!
 * Returns the id of the locally-stored contact which has a relationship of the given type with the involved contacts
 * \sa relationshipType(), involved(), setSourceId()
 */
QUniqueId QContactRelationship::sourceId() const
{
    return d->m_sourceId;
}

/*!
 * Returns the id of the contact with which the left contact has a relationship of the given type
 * \sa relationshipType(), sourceId(), setInvolved()
 */
QList<QPair<QString, QUniqueId> > QContactRelationship::involved() const
{
    return d->m_involved;
}

/*!
 * Returns the type of relationship which the source contact has with the involved contacts
 * \sa setRelationshipType()
 */
QString QContactRelationship::relationshipType() const
{
    return d->m_relationshipType;
}

/*!
 * Sets the id of the source contact in the relationship to \a id.  This contact
 * must be stored in the manager in which the relationship is stored, and has
 * a relationship of the specified type with the involved contacts.
 * \sa leftId()
 */
void QContactRelationship::setSourceId(const QUniqueId& id)
{
    d->m_sourceId = id;
}

/*!
 * Sets the involved contacts in the relationship to \a involvedContacts.  The source contact
 * has a relationship of the specified type with these contacts.
 * \sa involved()
 */
void QContactRelationship::setInvolved(const QList<QPair<QString, QUniqueId> >& involvedContacts)
{
    d->m_involved = involvedContacts;
}

/*!
 * Sets the type of relationship that the source contact has with the involved contacts
 * to \a relationshipType.
 * \sa relationshipType()
 */
void QContactRelationship::setRelationshipType(const QString& relationshipType)
{
    d->m_relationshipType = relationshipType;
}

