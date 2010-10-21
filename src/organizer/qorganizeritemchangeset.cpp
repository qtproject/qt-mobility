/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizeritemchangeset.h"
#include "qorganizeritemchangeset_p.h"
#include "qorganizermanagerengine.h"

QTM_BEGIN_NAMESPACE

/*!
   \class QOrganizerItemChangeSet

   \brief The QOrganizerItemChangeSet class provides a simple API to
   simplify the emission of state-change signals from
   QOrganizerManagerEngine implementations.

  \inmodule QtOrganizer

   This class can be utilised by backend implementations to ensure
   correct emission of the \l QOrganizerManagerEngine::dataChanged(), \l
   QOrganizerManagerEngine::itemsAdded(), \l
   QOrganizerManagerEngine::itemsChanged() and \l
   QOrganizerManagerEngine::itemsRemoved().

   \sa QOrganizerManagerEngine
 */

/*!
   Constructs a new change set
 */
QOrganizerItemChangeSet::QOrganizerItemChangeSet()
    : d(new QOrganizerItemChangeSetData)
{
}

/*!
   Constructs a copy of the \a other change set
 */
QOrganizerItemChangeSet::QOrganizerItemChangeSet(const QOrganizerItemChangeSet& other)
    : d(other.d)
{
}

/*!
   Frees the memory used by this change set
 */
QOrganizerItemChangeSet::~QOrganizerItemChangeSet()
{
}

/*!
   Assigns this change set to be equal to \a other
 */
QOrganizerItemChangeSet& QOrganizerItemChangeSet::operator=(const QOrganizerItemChangeSet& other)
{
    d = other.d;
    return *this;
}

/*!
   Sets the data changed flag to \a dataChanged.  If this is set to true prior to calling \l emitSignals(),
   only the \l QOrganizerManagerEngine::dataChanged() signal will be emitted; otherwise, the appropriate
   finer-grained signals will be emitted.
 */
void QOrganizerItemChangeSet::setDataChanged(bool dataChanged)
{
    d->m_dataChanged = dataChanged;
}

/*!
   Returns the value of the data changed flag
 */
bool QOrganizerItemChangeSet::dataChanged()
{
    return d->m_dataChanged;
}

/*!
   Returns the set of ids of organizer items which have been added to
   the database.
 */
QSet<QOrganizerItemId> QOrganizerItemChangeSet::addedItems() const
{
    return d->m_addedItems;
}

/*!
  Inserts the given organizer item id \a addedOrganizerItemId into the set of ids of organizer items
  which have been added to the database.
 */
void QOrganizerItemChangeSet::insertAddedItem(const QOrganizerItemId& addedOrganizerItemId)
{
    d->m_addedItems.insert(addedOrganizerItemId);
}

/*!
  Inserts each of the given organizer item ids \a addedOrganizerItemIds into the set of ids of organizer items
  which have been added to the database.
 */
void QOrganizerItemChangeSet::insertAddedItems(const QList<QOrganizerItemId>& addedOrganizerItemIds)
{
    foreach (const QOrganizerItemId& id, addedOrganizerItemIds)
        d->m_addedItems.insert(id);
}

/*!
  Clears the set of ids of organizer items which have been added to the database
 */
void QOrganizerItemChangeSet::clearAddedItems()
{
    d->m_addedItems.clear();
}

/*!
   Returns the set of ids of organizer items which have been changed in
   the database.
 */
QSet<QOrganizerItemId> QOrganizerItemChangeSet::changedItems() const
{
    return d->m_changedItems;
}

/*!
  Inserts the given organizer item id \a changedOrganizerItemId into the set of ids of organizer items
  which have been changed to the database.
 */
void QOrganizerItemChangeSet::insertChangedItem(const QOrganizerItemId& changedOrganizerItemId)
{
    d->m_changedItems.insert(changedOrganizerItemId);
}

/*!
  Inserts each of the given organizer item ids \a changedOrganizerItemIds into the set of ids of organizer items
  which have been changed to the database.
 */
void QOrganizerItemChangeSet::insertChangedItems(const QList<QOrganizerItemId>& changedOrganizerItemIds)
{
    foreach (const QOrganizerItemId& id, changedOrganizerItemIds)
        d->m_changedItems.insert(id);
}

/*!
  Clears the set of ids of organizer items which have been changed to the database
 */
void QOrganizerItemChangeSet::clearChangedItems()
{
    d->m_changedItems.clear();
}

/*!
   Returns the set of ids of organizer items which have been removed from
   the database.
 */
QSet<QOrganizerItemId> QOrganizerItemChangeSet::removedItems() const
{
    return d->m_removedItems;
}

/*!
  Inserts the given organizer item id \a removedOrganizerItemId into the set of ids of organizer items
  which have been removed to the database.
 */
void QOrganizerItemChangeSet::insertRemovedItem(const QOrganizerItemId& removedOrganizerItemId)
{
    d->m_removedItems.insert(removedOrganizerItemId);
}

/*!
  Inserts each of the given organizer item ids \a removedOrganizerItemIds into the set of ids of organizer items
  which have been removed to the database.
 */
void QOrganizerItemChangeSet::insertRemovedItems(const QList<QOrganizerItemId>& removedOrganizerItemIds)
{
    foreach (const QOrganizerItemId& id, removedOrganizerItemIds)
        d->m_removedItems.insert(id);
}

/*!
  Clears the set of ids of organizer items which have been removed to the database
 */
void QOrganizerItemChangeSet::clearRemovedItems()
{
    d->m_removedItems.clear();
}


/*!
   Clears all flags and sets of ids in this change set
 */
void QOrganizerItemChangeSet::clearAll()
{
    d->m_dataChanged = false;
    d->m_addedItems.clear();
    d->m_changedItems.clear();
    d->m_removedItems.clear();
}

/*!
   Emits the appropriate signals from the given \a engine given the state of the change set
 */
void QOrganizerItemChangeSet::emitSignals(QOrganizerManagerEngine *engine)
{
    if (!engine)
        return;

    if (d->m_dataChanged) {
        emit engine->dataChanged();
    } else {
        if (!d->m_addedItems.isEmpty())
            emit engine->itemsAdded(d->m_addedItems.toList());
        if (!d->m_changedItems.isEmpty())
            emit engine->itemsChanged(d->m_changedItems.toList());
        if (!d->m_removedItems.isEmpty())
            emit engine->itemsRemoved(d->m_removedItems.toList());
    }
}

QTM_END_NAMESPACE
