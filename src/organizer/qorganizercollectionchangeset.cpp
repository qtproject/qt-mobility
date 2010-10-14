/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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

#include "qorganizercollectionchangeset.h"
#include "qorganizercollectionchangeset_p.h"
#include "qorganizermanagerengine.h"

QTM_BEGIN_NAMESPACE

/*!
   \class QOrganizerCollectionChangeSet

   \brief The QOrganizerCollectionChangeSet class provides a simple API to
   simplify the emission of state-change signals from QOrganizerManagerEngine
   implementations.

  \inmodule QtOrganizer

  

   This class can be utilized by backend implementations to ensure
   correct emission of the \l QOrganizerManagerEngine::dataChanged(),
   \l QOrganizerManagerEngine::itemsAdded(),
   \l QOrganizerManagerEngine::itemsChanged() and
   \l QOrganizerManagerEngine::itemsRemoved().

   \sa QOrganizerManagerEngine
 */

/*!
   Constructs a new change set
 */
QOrganizerCollectionChangeSet::QOrganizerCollectionChangeSet()
    : d(new QOrganizerCollectionChangeSetData)
{
}

/*!
   Constructs a copy of the \a other change set
 */
QOrganizerCollectionChangeSet::QOrganizerCollectionChangeSet(const QOrganizerCollectionChangeSet& other)
    : d(other.d)
{
}

/*!
   Frees the memory used by this change set
 */
QOrganizerCollectionChangeSet::~QOrganizerCollectionChangeSet()
{
}

/*!
   Assigns this change set to be equal to \a other
 */
QOrganizerCollectionChangeSet& QOrganizerCollectionChangeSet::operator=(const QOrganizerCollectionChangeSet& other)
{
    d = other.d;
    return *this;
}

/*!
   Sets the data changed flag to \a dataChanged.  If this is set to true prior to calling \l emitSignals(),
   only the \l QOrganizerManagerEngine::dataChanged() signal will be emitted; otherwise, the appropriate
   finer-grained signals will be emitted.
 */
void QOrganizerCollectionChangeSet::setDataChanged(bool dataChanged)
{
    d->m_dataChanged = dataChanged;
}

/*!
   Returns the value of the data changed flag
 */
bool QOrganizerCollectionChangeSet::dataChanged()
{
    return d->m_dataChanged;
}

/*!
   Returns the set of ids of collections which have been added to
   the database.
 */
QSet<QOrganizerCollectionId> QOrganizerCollectionChangeSet::addedCollections() const
{
    return d->m_addedCollections;
}

/*!
  Inserts the given collection id \a addedOrganizerCollectionId into the set of ids of collections
  which have been added to the database.
 */
void QOrganizerCollectionChangeSet::insertAddedCollection(const QOrganizerCollectionId& addedOrganizerCollectionId)
{
    d->m_addedCollections.insert(addedOrganizerCollectionId);
}

/*!
  Inserts each of the given collection ids \a addedOrganizerCollectionIds into
  the set of ids of collections which have been added to the database.
 */
void QOrganizerCollectionChangeSet::insertAddedCollections(const QList<QOrganizerCollectionId>& addedOrganizerCollectionIds)
{
    foreach (const QOrganizerCollectionId& id, addedOrganizerCollectionIds)
        d->m_addedCollections.insert(id);
}

/*!
  Clears the set of ids of collections which have been added to the database
 */
void QOrganizerCollectionChangeSet::clearAddedCollections()
{
    d->m_addedCollections.clear();
}

/*!
   Returns the set of ids of collections which have been changed in
   the database.
 */
QSet<QOrganizerCollectionId> QOrganizerCollectionChangeSet::changedCollections() const
{
    return d->m_changedCollections;
}

/*!
  Inserts the given collection id \a changedOrganizerCollectionId into the set of ids of collections
  which have been changed to the database.
 */
void QOrganizerCollectionChangeSet::insertChangedCollection(const QOrganizerCollectionId& changedOrganizerCollectionId)
{
    d->m_changedCollections.insert(changedOrganizerCollectionId);
}

/*!
  Inserts each of the given collection ids \a changedOrganizerCollectionIds into the set of ids of collections
  which have been changed to the database.
 */
void QOrganizerCollectionChangeSet::insertChangedCollections(const QList<QOrganizerCollectionId>& changedOrganizerCollectionIds)
{
    foreach (const QOrganizerCollectionId& id, changedOrganizerCollectionIds)
        d->m_changedCollections.insert(id);
}

/*!
  Clears the set of ids of collections which have been changed to the database
 */
void QOrganizerCollectionChangeSet::clearChangedCollections()
{
    d->m_changedCollections.clear();
}

/*!
   Returns the set of ids of collections which have been removed from
   the database.
 */
QSet<QOrganizerCollectionId> QOrganizerCollectionChangeSet::removedCollections() const
{
    return d->m_removedCollections;
}

/*!
  Inserts the given collection id \a removedOrganizerCollectionId into the set of ids of collections
  which have been removed to the database.
 */
void QOrganizerCollectionChangeSet::insertRemovedCollection(const QOrganizerCollectionId& removedOrganizerCollectionId)
{
    d->m_removedCollections.insert(removedOrganizerCollectionId);
}

/*!
  Inserts each of the given collection ids \a removedOrganizerCollectionIds into the set of ids of collections
  which have been removed to the database.
 */
void QOrganizerCollectionChangeSet::insertRemovedCollections(const QList<QOrganizerCollectionId>& removedOrganizerCollectionIds)
{
    foreach (const QOrganizerCollectionId& id, removedOrganizerCollectionIds)
        d->m_removedCollections.insert(id);
}

/*!
  Clears the set of ids of collections which have been removed to the database
 */
void QOrganizerCollectionChangeSet::clearRemovedCollections()
{
    d->m_removedCollections.clear();
}


/*!
   Clears all flags and sets of ids in this change set
 */
void QOrganizerCollectionChangeSet::clearAll()
{
    d->m_dataChanged = false;
    d->m_addedCollections.clear();
    d->m_changedCollections.clear();
    d->m_removedCollections.clear();
}

/*!
   Emits the appropriate signals from the given \a engine given the state of the change set
 */
void QOrganizerCollectionChangeSet::emitSignals(QOrganizerManagerEngine *engine)
{
    if (!engine)
        return;

    if (d->m_dataChanged) {
        emit engine->dataChanged();
    } else {
        if (!d->m_addedCollections.isEmpty())
            emit engine->collectionsAdded(d->m_addedCollections.toList());
        if (!d->m_changedCollections.isEmpty())
            emit engine->collectionsChanged(d->m_changedCollections.toList());
        if (!d->m_removedCollections.isEmpty())
            emit engine->collectionsRemoved(d->m_removedCollections.toList());
    }
}

QTM_END_NAMESPACE
