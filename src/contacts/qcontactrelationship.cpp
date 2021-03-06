/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactrelationship.h"
#include "qcontactrelationship_p.h"
#include "qtcontactsglobal.h"
#include "qcontactid.h"

#include <QList>
#include <QPair>
#include <QString>
#include <QHash>
#include <QDebug>
#include <QDataStream>

QTM_BEGIN_NAMESPACE

/*!
  \class QContactRelationship
  \brief The QContactRelationship class describes a one-to-one relationship
  between a locally-stored contact and another (possibly remote) contact.

  \inmodule QtContacts
   \since 1.0

  \ingroup contacts-main

  Each relationship is uniquely identified by the combination of the first
  contact id, second contact id, and the relationship type.

  A relationship should not contain a second contact which is the same as
  the first contact.
  Any local contacts which are referenced in the relationship (that is, any
  source contact, or any second contact whose manager URI is left empty or
  whose manager URI references the manager that stores the source contact,
  and in which the relationship will be saved) should exist.

  If any of these requirements are not met, validation of the relationship
  may fail when attempting to save the relationship in a QContactManager.

  \sa QContactRelationshipFilter
 */

/*!
  \enum QContactRelationship::Role
  Describes the roles that a contact may take in a relationship.
  \value First The contact is the first contact in the relationship
  \value Second The contact is the second contact in the relationship
  \value Either The contact is either the first or second contact in the relationship
 */

/*!
 * \variable QContactRelationship::HasMember
 * The relationship type which identifies the first contact as being a group which includes the second contact
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRelationship::HasMember, "HasMember");

/*!
 * \variable QContactRelationship::Aggregates
 * The relationship type which identifies the first contact as aggregating the second contact into a metacontact
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRelationship::Aggregates, "Aggregates");

/*!
 * \variable QContactRelationship::IsSameAs
 * The relationship type which identifies the first contact as being the same contact as the second contact
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRelationship::IsSameAs, "IsSameAs");

/*!
 * \variable QContactRelationship::HasAssistant
 * The relationship type which identifies the second contact as being the assistant of the first contact
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRelationship::HasAssistant, "HasAssistant");

/*!
 * \variable QContactRelationship::HasManager
 * The relationship type which identifies the second contact as being the manager of the first contact
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRelationship::HasManager, "HasManager");

/*!
 * \variable QContactRelationship::HasSpouse
 * The relationship type which identifies the second contact as being the spouse of the first contact
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRelationship::HasSpouse, "HasSpouse");

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
 * \since 1.0
 */
QContactRelationship::QContactRelationship(const QContactRelationship& other)
        : d(other.d)
{
}

/*!
 * Assigns this relationship to be equal to \a other
 * \since 1.0
 */
QContactRelationship& QContactRelationship::operator=(const QContactRelationship& other)
{
    d = other.d;
    return *this;
}

/*!
 * Returns true if this relationship is equal to the \a other relationship, otherwise returns false.
 * \since 1.0
 */
bool QContactRelationship::operator==(const QContactRelationship &other) const
{
    if (d->m_first != other.d->m_first)
        return false;
    if (d->m_second != other.d->m_second)
        return false;
    if (d->m_relationshipType != other.d->m_relationshipType)
        return false;
    return true;
}

/*!
 * Returns the hash value for \a key.
 * \since 1.0
 */
uint qHash(const QContactRelationship &key)
{
    return qHash(key.first()) + qHash(key.second())
        + QT_PREPEND_NAMESPACE(qHash)(key.relationshipType());
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QContactRelationship& rel)
{
    dbg.nospace() << "QContactRelationship(" << rel.first() << ' ' << rel.relationshipType()
            << ' ' << rel.second() << ')';
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
 * Writes \a rel to the stream \a out.
 * \since 1.0
 */
QDataStream& operator<<(QDataStream& out, const QContactRelationship& rel)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QContactRelationship
    return out << formatVersion << rel.first() << rel.relationshipType() << rel.second();
}

/*!
 * Reads a contact relationship from stream \a in into \a rel.
 * \since 1.0
 */
QDataStream& operator>>(QDataStream& in, QContactRelationship& rel)
{
    rel = QContactRelationship();
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        QContactId first;
        QString relationshipType;
        QContactId second;
        in >> first >> relationshipType >> second;
        rel.setFirst(first);
        rel.setRelationshipType(relationshipType);
        rel.setSecond(second);
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}
#endif

/*!
 * \fn QContactRelationship::operator!=(const QContactRelationship& other) const
 * Returns true if this relationship is not equal to \a other, otherwise returns false.
 * \since 1.0
 */

/*!
 * Returns the id of the locally-stored contact which has a relationship of the given type with the second contact
 * \sa relationshipType(), second(), setFirst()
 * \since 1.0
 */
QContactId QContactRelationship::first() const
{
    return d->m_first;
}

/*!
 * Returns the id of the contact with which the first contact has a relationship of the given type
 * \sa relationshipType(), first()
 * \since 1.0
 */
QContactId QContactRelationship::second() const
{
    return d->m_second;
}

/*!
 * Returns the type of relationship which the source contact has with the destination contacts
 * \sa setRelationshipType()
 * \since 1.0
 */
QString QContactRelationship::relationshipType() const
{
    return d->m_relationshipType;
}

/*!
 * Sets the id of the first contact in the relationship to \a firstId.  This contact
 * must be stored in the manager in which the relationship is stored, and has
 * a relationship of the specified type with the second contact.
 * \since 1.0
 * \sa first()
 */
void QContactRelationship::setFirst(const QContactId& firstId)
{
    d->m_first = firstId;
}

/*!
 * Sets the second contact in the relationship to \a secondId.  The first contact
 * has a relationship of the specified type with this contact.
 * \since 1.0
 * \sa second()
 */
void QContactRelationship::setSecond(const QContactId& secondId)
{
    d->m_second = secondId;
}

/*!
 * Sets the type of relationship that the source contact has with the destination contacts
 * to \a relationshipType.
 * \since 1.0
 * \sa relationshipType()
 */
void QContactRelationship::setRelationshipType(const QString& relationshipType)
{
    d->m_relationshipType = relationshipType;
}

QTM_END_NAMESPACE
