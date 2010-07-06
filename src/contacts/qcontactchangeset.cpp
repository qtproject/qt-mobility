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


#include "qcontactchangeset.h"
#include "qcontactchangeset_p.h"
#include "qcontactmanagerengine.h"

QTM_BEGIN_NAMESPACE

/*!
   \class QContactChangeSet
  
  \inmodule QtContacts
  
   \brief The QContactChangeSet class provides a simple API to
   simplify the emission of state-change signals from
   QContactManagerEngine implementations.
  
   This class can be utilised by backend implementations to ensure
   correct emission of the \l QContactManagerEngine::dataChanged(), \l
   QContactManagerEngine::contactsAdded(), \l
   QContactManagerEngine::contactsChanged() and \l
   QContactManagerEngine::contactsRemoved().
  
   \sa QContactManagerEngine
 */

/*!
   Constructs a new change set
 */
QContactChangeSet::QContactChangeSet()
    : d(new QContactChangeSetData)
{
}

/*!
   Constructs a copy of the \a other change set
 */
QContactChangeSet::QContactChangeSet(const QContactChangeSet& other)
    : d(other.d)
{
}

/*!
   Frees the memory used by this change set
 */
QContactChangeSet::~QContactChangeSet()
{
}

/*!
   Assigns this change set to be equal to \a other
 */
QContactChangeSet& QContactChangeSet::operator=(const QContactChangeSet& other)
{
    d = other.d;
    return *this;
}

/*!
   Sets the data changed flag to \a dataChanged.  If this is set to true prior to calling \l emitSignals(),
   only the \l QContactManagerEngine::dataChanged() signal will be emitted; otherwise, the appropriate
   finer-grained signals will be emitted.
 */
void QContactChangeSet::setDataChanged(bool dataChanged)
{
    d->m_dataChanged = dataChanged;
}

/*!
   Returns the value of the data changed flag
 */
bool QContactChangeSet::dataChanged()
{
    return d->m_dataChanged;
}

/*!
   Returns the set of ids of contacts which have been added to
   the database.
 */
QSet<QContactLocalId> QContactChangeSet::addedContacts() const
{
    return d->m_addedContacts;
}

/*!
  Inserts the given contact id \a addedContactId into the set of ids of contacts
  which have been added to the database.
 */
void QContactChangeSet::insertAddedContact(QContactLocalId addedContactId)
{
    d->m_addedContacts.insert(addedContactId);
}

/*!
  Inserts each of the given contact ids \a addedContactIds into the set of ids of contacts
  which have been added to the database.
 */
void QContactChangeSet::insertAddedContacts(const QList<QContactLocalId>& addedContactIds)
{
    foreach (const QContactLocalId& id, addedContactIds)
        d->m_addedContacts.insert(id);
}

/*!
  Clears the set of ids of contacts which have been added to the database
 */
void QContactChangeSet::clearAddedContacts()
{
    d->m_addedContacts.clear();
}

/*!
   Returns the set of ids of contacts which have been changed in
   the database.
 */
QSet<QContactLocalId> QContactChangeSet::changedContacts() const
{
    return d->m_changedContacts;
}

/*!
  Inserts the given contact id \a changedContactId into the set of ids of contacts
  which have been changed to the database.
 */
void QContactChangeSet::insertChangedContact(QContactLocalId changedContactId)
{
    d->m_changedContacts.insert(changedContactId);
}

/*!
  Inserts each of the given contact ids \a changedContactIds into the set of ids of contacts
  which have been changed to the database.
 */
void QContactChangeSet::insertChangedContacts(const QList<QContactLocalId>& changedContactIds)
{
    foreach (const QContactLocalId& id, changedContactIds)
        d->m_changedContacts.insert(id);
}

/*!
  Clears the set of ids of contacts which have been changed to the database
 */
void QContactChangeSet::clearChangedContacts()
{
    d->m_changedContacts.clear();
}

/*!
   Returns the set of ids of contacts which have been removed from
   the database.
 */
QSet<QContactLocalId> QContactChangeSet::removedContacts() const
{
    return d->m_removedContacts;
}

/*!
  Inserts the given contact id \a removedContactId into the set of ids of contacts
  which have been removed to the database.
 */
void QContactChangeSet::insertRemovedContact(QContactLocalId removedContactId)
{
    d->m_removedContacts.insert(removedContactId);
}

/*!
  Inserts each of the given contact ids \a removedContactIds into the set of ids of contacts
  which have been removed to the database.
 */
void QContactChangeSet::insertRemovedContacts(const QList<QContactLocalId>& removedContactIds)
{
    foreach (const QContactLocalId& id, removedContactIds)
        d->m_removedContacts.insert(id);
}

/*!
  Clears the set of ids of contacts which have been removed to the database
 */
void QContactChangeSet::clearRemovedContacts()
{
    d->m_removedContacts.clear();
}

/*!
   Returns the set of ids of contacts which have been affected
   by the addition of relationships to the database.
 */
QSet<QContactLocalId> QContactChangeSet::addedRelationshipsContacts() const
{
    return d->m_addedRelationships;
}

/*!
  Inserts the given contact id \a affectedContactId into the set of ids of contacts
  which have been affected by the addition of a relationship to the database.
 */
void QContactChangeSet::insertAddedRelationshipsContact(QContactLocalId affectedContactId)
{
    d->m_addedRelationships.insert(affectedContactId);
}

/*!
  Inserts each of the given contact ids \a affectedContactIds into the set of ids of contacts
  which have been affected by the addition of a relationship to the database.
 */
void QContactChangeSet::insertAddedRelationshipsContacts(const QList<QContactLocalId>& affectedContactIds)
{
    foreach (const QContactLocalId& id, affectedContactIds)
        d->m_addedRelationships.insert(id);
}

/*!
  Clears the set of ids of contacts which have been affected by the addition of a relationship to the database.
 */
void QContactChangeSet::clearAddedRelationshipsContacts()
{
    d->m_addedRelationships.clear();
}

/*!
   Returns the set of ids of contacts which have been affected
   by the removal of relationships from the database.
 */
QSet<QContactLocalId> QContactChangeSet::removedRelationshipsContacts() const
{
    return d->m_removedRelationships;
}

/*!
  Inserts the given contact id \a affectedContactId into the set of ids of contacts
  which have been affected by the removal of a relationship to the database.
 */
void QContactChangeSet::insertRemovedRelationshipsContact(QContactLocalId affectedContactId)
{
    d->m_removedRelationships.insert(affectedContactId);
}

/*!
  Inserts each of the given contact ids \a affectedContactIds into the set of ids of contacts
  which have been affected by the removal of a relationship to the database.
 */
void QContactChangeSet::insertRemovedRelationshipsContacts(const QList<QContactLocalId>& affectedContactIds)
{
    foreach (const QContactLocalId& id, affectedContactIds)
        d->m_removedRelationships.insert(id);
}

/*!
  Clears the set of ids of contacts which have been affected by the removal of a relationship to the database.
 */
void QContactChangeSet::clearRemovedRelationshipsContacts()
{
    d->m_removedRelationships.clear();
}

/*!
  Sets the pair of ids which represent the old and new self contact ids
  to the given pair of ids \a oldAndNewContactId.
  The first id in the pair is the old self contact id, while the second
  id in the pair is the new self contact id.  If the new id is different
  to the old id at the point in time when emitSignals() is called,
  the QContactManagerEngine::selfContactIdChanged signal will be emitted.
 */
void QContactChangeSet::setOldAndNewSelfContactId(const QPair<QContactLocalId, QContactLocalId> &oldAndNewContactId)
{
    d->m_oldAndNewSelfContactId = oldAndNewContactId;
}

/*!
   Returns the pair of ids which represents the
   old and new self contact ids.  The first id in the pair is the
   old self contact id, while the second id in the pair is the
   new self contact id.  If the new id is different to the old id
   at the point in time when emitSignals() is called,
   the QContactManagerEngine::selfContactIdChanged() signal will be emitted.
 */
QPair<QContactLocalId, QContactLocalId> QContactChangeSet::oldAndNewSelfContactId() const
{
    return d->m_oldAndNewSelfContactId;
}

/*!
   Clears all flags and sets of ids in this change set
 */
void QContactChangeSet::clearAll()
{
    d->m_dataChanged = false;
    d->m_addedContacts.clear();
    d->m_changedContacts.clear();
    d->m_removedContacts.clear();
    d->m_addedRelationships.clear();
    d->m_removedRelationships.clear();
    d->m_oldAndNewSelfContactId = QPair<QContactLocalId, QContactLocalId>();
}

/*!
   Emits the appropriate signals from the given \a engine given the state of the change set
 */
void QContactChangeSet::emitSignals(QContactManagerEngine *engine)
{
    if (!engine)
        return;

    if (d->m_dataChanged) {
        emit engine->dataChanged();
    } else {
        if (!d->m_addedContacts.isEmpty())
            emit engine->contactsAdded(d->m_addedContacts.toList());
        if (!d->m_changedContacts.isEmpty())
            emit engine->contactsChanged(d->m_changedContacts.toList());
        if (!d->m_removedContacts.isEmpty())
            emit engine->contactsRemoved(d->m_removedContacts.toList());
        if (!d->m_addedRelationships.isEmpty())
            emit engine->relationshipsAdded(d->m_addedRelationships.toList());
        if (!d->m_removedRelationships.isEmpty())
            emit engine->relationshipsRemoved(d->m_removedRelationships.toList());
        if (d->m_oldAndNewSelfContactId.first != d->m_oldAndNewSelfContactId.second)
            emit engine->selfContactIdChanged(d->m_oldAndNewSelfContactId.first, d->m_oldAndNewSelfContactId.second);
    }
}


/*!
  \deprecated
   Returns a reference to the set of ids of contacts which have been added to
   the database.

   Don't use this.. use the const version instead.
 */
QSet<QContactLocalId>& QContactChangeSet::addedContacts()
{
    return d->m_addedContacts;
}

/*!
  \deprecated
   Returns a reference to the set of ids of contacts which have been changed in
   the database.

   Don't use this.. use the const version instead.
 */
QSet<QContactLocalId>& QContactChangeSet::changedContacts()
{
    return d->m_changedContacts;
}

/*!
  \deprecated
   Returns a reference to the set of ids of contacts which have been removed from
   the database.

   Don't use this.. use the const version instead.
 */
QSet<QContactLocalId>& QContactChangeSet::removedContacts()
{
    return d->m_removedContacts;
}

/*!
  \deprecated
   Returns a reference to the set of ids of contacts which have been affected
   by the addition of relationships to the database.

   Don't use this.. use the const version instead.
 */
QSet<QContactLocalId>& QContactChangeSet::addedRelationshipsContacts()
{
    return d->m_addedRelationships;
}

/*!
  \deprecated
   Returns a reference to the set of ids of contacts which have been affected
   by the removal of relationships from the database.

   Don't use this.. use the const version instead.
 */
QSet<QContactLocalId>& QContactChangeSet::removedRelationshipsContacts()
{
    return d->m_removedRelationships;
}

/*!
  \deprecated
   Returns a reference to the pair of ids which represents the
   old and new self contact ids.  The first id in the pair is the
   old self contact id, while the second id in the pair is the
   new self contact id.  If the new id is different to the old id
   at the point in time when emitSignals() is called,
   the QContactManagerEngine::selfContactIdChanged() signal will be emitted.

   Don't use this.. use the const version instead.
 */
QPair<QContactLocalId, QContactLocalId>& QContactChangeSet::oldAndNewSelfContactId()
{
    return d->m_oldAndNewSelfContactId;
}

/*!
  \deprecated
   Clears all flags and sets of ids in this change set

   Use clearAll() instead.
 */
void QContactChangeSet::clear()
{
    d->m_dataChanged = false;
    d->m_addedContacts.clear();
    d->m_changedContacts.clear();
    d->m_removedContacts.clear();
    d->m_addedRelationships.clear();
    d->m_removedRelationships.clear();
    d->m_oldAndNewSelfContactId = QPair<QContactLocalId, QContactLocalId>();
}


QTM_END_NAMESPACE
