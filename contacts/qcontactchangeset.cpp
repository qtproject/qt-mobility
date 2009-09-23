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


#include "qcontactchangeset.h"
#include "qcontactchangeset_p.h"
#include "qcontactmanagerengine.h"

/*!
 * \class QContactChangeSet
 *
 * \brief Provides a simple API to simplify the emission of state-change signals from QContactManagerEngine implementations.
 *
 * This class can be utilised by backend implementations to ensure correct emission of the \l QContactManagerEngine::dataChanged(),
 * \l QContactManagerEngine::contactsAdded(), \l QContactManagerEngine::contactsChanged(), \l QContactManagerEngine::contactsRemoved(),
 * \l QContactManagerEngine::groupsAdded(), \l QContactManagerEngine::groupsChanged(), and \l QContactManagerEngine::groupsRemoved() signals.
 *
 * \sa QContactManagerEngine
 */

/*!
 * Constructs a new change set
 */
QContactChangeSet::QContactChangeSet()
    : d(new QContactChangeSetData)
{
}

/*!
 * Constructs a copy of the \a other change set
 */
QContactChangeSet::QContactChangeSet(const QContactChangeSet& other)
    : d(other.d)
{
}

/*!
 * Frees the memory used by this change set
 */
QContactChangeSet::~QContactChangeSet()
{
}

/*!
 * Assigns this change set to be equal to \a other
 */
QContactChangeSet& QContactChangeSet::operator=(const QContactChangeSet& other)
{
    d = other.d;
    return *this;
}

/*!
 * Sets the data changed flag to \a dataChanged.  If this is set to true prior to calling \l emitSignals(),
 * only the \l QContactManagerEngine::dataChanged() signal will be emitted.
 */
void QContactChangeSet::setDataChanged(bool dataChanged)
{
    d->m_dataChanged = dataChanged;
}

/*!
 * Returns the value of the data changed flag
 */
bool QContactChangeSet::dataChanged()
{
    return d->m_dataChanged;
}

/*!
 * Sets the set of ids of contacts which have been added to the managed store to \a ids.
 * If this is done prior to calling \l emitSignals(), and the data changed flag is not set to true
 * when \l emitSignals() is called, the \l QContactManagerEngine::contactsAdded() signal will be emitted
 * (if the set is non-empty).
 */
void QContactChangeSet::setAddedContacts(const QSet<QUniqueId>& ids)
{
    d->m_addedContacts = ids;
}

/*!
 * Returns the set of ids of contacts which have been added, which has previously been set in this change set
 */
QSet<QUniqueId> QContactChangeSet::addedContacts()
{
    return d->m_addedContacts;
}

/*!
 * Sets the set of ids of contacts which have been changed in the managed store to \a ids.
 * If this is done prior to calling \l emitSignals(), and the data changed flag is not set to true
 * when \l emitSignals() is called, the \l QContactManagerEngine::contactsChanged() signal will be emitted
 * (if the set is non-empty).
 */
void QContactChangeSet::setChangedContacts(const QSet<QUniqueId>& ids)
{
    d->m_changedContacts = ids;
}

/*!
 * Returns the set of ids of contacts which have been changed, which has previously been set in this change set
 */
QSet<QUniqueId> QContactChangeSet::changedContacts()
{
    return d->m_changedContacts;
}

/*!
 * Sets the set of ids of contacts which have been removed from the managed store to \a ids.
 * If this is done prior to calling \l emitSignals(), and the data changed flag is not set to true
 * when \l emitSignals() is called, the \l QContactManagerEngine::contactsRemoved() signal will be emitted
 * (if the set is non-empty).
 */
void QContactChangeSet::setRemovedContacts(const QSet<QUniqueId>& ids)
{
    d->m_removedContacts = ids;
}

/*!
 * Returns the set of ids of contacts which have been removed, which has previously been set in this change set
 */
QSet<QUniqueId> QContactChangeSet::removedContacts()
{
    return d->m_removedContacts;
}

/*!
 * Sets the set of ids of groups which have been added to the managed store to \a ids.
 * If this is done prior to calling \l emitSignals(), and the data changed flag is not set to true
 * when \l emitSignals() is called, the \l QContactManagerEngine::groupsAdded() signal will be emitted
 * (if the set is non-empty).
 */
void QContactChangeSet::setAddedGroups(const QSet<QUniqueId>& ids)
{
    d->m_addedGroups = ids;
}

/*!
 * Returns the set of ids of groups which have been added, which has previously been set in this change set
 */
QSet<QUniqueId> QContactChangeSet::addedGroups()
{
    return d->m_addedGroups;
}

/*!
 * Sets the set of ids of groups which have been changed in the managed store to \a ids.
 * If this is done prior to calling \l emitSignals(), and the data changed flag is not set to true
 * when \l emitSignals() is called, the \l QContactManagerEngine::groupsChanged() signal will be emitted
 * (if the set is non-empty).
 */
void QContactChangeSet::setChangedGroups(const QSet<QUniqueId>& ids)
{
    d->m_changedGroups = ids;
}

/*!
 * Returns the set of ids of groups which have been changed, which has previously been set in this change set
 */
QSet<QUniqueId> QContactChangeSet::changedGroups()
{
    return d->m_changedGroups;
}

/*!
 * Sets the set of ids of groups which have been removed from the managed store to \a ids.
 * If this is done prior to calling \l emitSignals(), and the data changed flag is not set to true
 * when \l emitSignals() is called, the \l QContactManagerEngine::groupsRemoved() signal will be emitted
 * (if the set is non-empty).
 */
void QContactChangeSet::setRemovedGroups(const QSet<QUniqueId>& ids)
{
    d->m_removedGroups = ids;
}

/*!
 * Returns the set of ids of groups which have been removed, which has previously been set in this change set
 */
QSet<QUniqueId> QContactChangeSet::removedGroups()
{
    return d->m_removedGroups;
}

/*!
 * Clears all flags and sets of ids in this change set
 */
void QContactChangeSet::clear()
{
    d->m_dataChanged = false;
    d->m_addedContacts.clear();
    d->m_changedContacts.clear();
    d->m_removedContacts.clear();
    d->m_addedGroups.clear();
    d->m_changedGroups.clear();
    d->m_removedGroups.clear();
}

/*!
 * Emits the appropriate signals from the given \a engine given the state of the change set
 */
void QContactChangeSet::emitSignals(QContactManagerEngine *engine)
{
    if (d->m_dataChanged) {
        emit engine->dataChanged();
    } else {
        if (!d->m_addedContacts.isEmpty())
            emit engine->contactsAdded(d->m_addedContacts.toList());
        if (!d->m_changedContacts.isEmpty())
            emit engine->contactsChanged(d->m_changedContacts.toList());
        if (!d->m_removedContacts.isEmpty())
            emit engine->contactsRemoved(d->m_removedContacts.toList());
        if (!d->m_addedGroups.isEmpty())
            emit engine->groupsAdded(d->m_addedGroups.toList());
        if (!d->m_changedGroups.isEmpty())
            emit engine->groupsChanged(d->m_changedGroups.toList());
        if (!d->m_removedGroups.isEmpty())
            emit engine->groupsRemoved(d->m_removedGroups.toList());
    }
}
