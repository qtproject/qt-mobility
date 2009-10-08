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

#include <QList>
#include <QPair>
#include <QString>

/*!
 * \class QContactRelationship
 * \brief Describes a one-to-many relationship between a locally-stored contact and a list of other (possibly remote) contacts.
 *
 * Each relationship is uniquely identified by the combination of the source contact id and the relationship type.
 *
 * A relationship should not contain a destination contact which is the same as the source contact, nor should it contain
 * duplicate destination contacts.  Finally, any local contacts which are referenced in the relationship (that is, any
 * source contact, or any destination contact whose manager URI is left empty or whose manager URI references the manager
 * that stores the source contact, and in which the relationship will be saved) should exist.
 * If any of these requirements are not met, validation of the relationship may fail when attempting to save the relationship
 * in a QContactManager.
 */

/*!
 * \variable QContactRelationship::RelationshipTypeHasMember
 * The relationship type which identifies the source contact as being a group which includes each of the destination contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeHasMember, "HasMember");

/*!
 * \variable QContactRelationship::RelationshipTypeAggregates
 * The relationship type which identifies the source contact as aggregating the destination contacts into a metacontact
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeAggregates, "Aggregates");

/*!
 * \variable QContactRelationship::RelationshipTypeIs
 * The relationship type which identifies the source contact as being the same contact as the destination contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIs, "Is");

/*!
 * \variable QContactRelationship::RelationshipTypeIsAssistantOf
 * The relationship type which identifies the source contact as being the assistant of the destination contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIsAssistantOf, "IsAssistantOf");

/*!
 * \variable QContactRelationship::RelationshipTypeIsManagerOf
 * The relationship type which identifies the source contact as being the manager of the destination contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIsManagerOf, "IsManagerOf");

/*!
 * \variable QContactRelationship::RelationshipTypeIsSpouseOf
 * The relationship type which identifies the source contact as being the spouse of the destination contacts
 */
Q_DEFINE_LATIN1_LITERAL(QContactRelationship::RelationshipTypeIsSpouseOf, "IsSpouseOf");

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
    if (d->m_sourceContact != other.d->m_sourceContact)
        return false;
    if (d->m_destinationContacts != other.d->m_destinationContacts)
        return false;
    if (d->m_relationshipType != other.d->m_relationshipType)
        return false;
    return true;
}

/*!
 * Returns the id of the locally-stored contact which has a relationship of the given type with the destination contacts
 * \sa relationshipType(), destination(), setSourceId()
 */
QUniqueId QContactRelationship::sourceContact() const
{
    return d->m_sourceContact;
}

/*!
 * Returns the id of the contact with which the left contact has a relationship of the given type
 * \sa relationshipType(), sourceId(), setInvolved()
 */
QList<QPair<QString, QUniqueId> > QContactRelationship::destinationContacts() const
{
    return d->m_destinationContacts;
}

/*!
 * Returns the type of relationship which the source contact has with the destination contacts
 * \sa setRelationshipType()
 */
QString QContactRelationship::relationshipType() const
{
    return d->m_relationshipType;
}

/*!
 * Sets the id of the source contact in the relationship to \a id.  This contact
 * must be stored in the manager in which the relationship is stored, and has
 * a relationship of the specified type with the destination contacts.
 * \sa leftId()
 */
void QContactRelationship::setSourceContact(const QUniqueId& id)
{
    d->m_sourceContact = id;
}

/*!
 * Sets the destination contacts in the relationship to \a destinationContacts.  The source contact
 * has a relationship of the specified type with these contacts.
 * \sa destination()
 */
void QContactRelationship::setDestinationContacts(const QList<QPair<QString, QUniqueId> >& contacts)
{
    d->m_destinationContacts = contacts;
}

/*!
 * Sets the type of relationship that the source contact has with the destination contacts
 * to \a relationshipType.
 * \sa relationshipType()
 */
void QContactRelationship::setRelationshipType(const QString& relationshipType)
{
    d->m_relationshipType = relationshipType;
}

/*!
 * Removes the destination contact found at the given \a position in the list of destination contacts if it exists.
 * Returns true if the destination contact was removed from the list successfully, false if it did not exist or
 * \a position is not a valid index position in the list.
 */
bool QContactRelationship::removeDestinationContact(int position)
{
    if (position < 0 || position >= d->m_destinationContacts.size())
        return false;
    d->m_destinationContacts.removeAt(position);
    return true;
}

/*!
 * Removes the given destination \a contact from the list of destination contacts if it exists.
 * Returns true if the destination contact was removed from the list successfully, false if it did not exist.
 */
bool QContactRelationship::removeDestinationContact(const QPair<QString, QUniqueId>& contact)
{
    return d->m_destinationContacts.removeOne(contact);
}

/*!
 * Inserts the destination \a contact into the list of destination contacts at the specified \a position.
 * If \a position is 0, the \a contact will be inserted at the head of the list.  If \a position is greater
 * than or equal to the size of the list of destination contacts, it will be append to the end of the list.
 */
void QContactRelationship::insertDestinationContact(int position, const QPair<QString, QUniqueId>& contact)
{
    d->m_destinationContacts.insert(position, contact);
}

/*!
 * Appends the given \a contact onto the list of destination contacts.
 */
void QContactRelationship::appendDestinationContact(const QPair<QString, QUniqueId>& contact)
{
    d->m_destinationContacts.append(contact);
}

/*!
 * Appends a contact in the relationship's manager identified by the given \a contactId onto the list of destination contacts.
 */
void QContactRelationship::appendDestinationContact(const QUniqueId& contactId)
{
    QPair<QString, QUniqueId> contact = QPair<QString, QUniqueId>(QString(), contactId);
    appendDestinationContact(contact);
}
