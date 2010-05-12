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

#include "qorganizeritemid.h"
#include "qorganizeritemid_p.h"
#include <QHash>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemId
  \brief The QOrganizerItemId class provides information that uniquely identifies
  a organizeritem in a particular manager.

  It consists of a manager URI which identifies the manager which contains the organizeritem,
  and the local id of the organizeritem in that manager.

  A "null" QOrganizerItemId has an empty manager URI, and an invalid QOrganizerItemLocalId (0).

  \sa QOrganizerItemLocalId
 */

/*!
  \typedef QOrganizerItemLocalId
  \relates QOrganizerItemId
  \brief The QOrganizerItemLocalId type represents the unique id of a organizeritem within its manager.

  Most operations within a \l QOrganizerItemManager accept a QOrganizerItemLocalId.  Some operations
  (involving links to organizeritems outside a particular manager) also accept a manager URI - this
  combination is stored in a \l QOrganizerItemId.

  An invalid QOrganizerItemLocalId is represented by a zero (0) value.

  \sa QOrganizerItemId
*/

/*!
 * Constructs a new organizeritem id
 */
QOrganizerItemId::QOrganizerItemId()
        : d(new QOrganizerItemIdPrivate)
{
}

/*!
 * Cleans up the memory in use by the organizeritem id
 */
QOrganizerItemId::~QOrganizerItemId()
{
}

/*! Constructs a new organizeritem id as a copy of \a other */
QOrganizerItemId::QOrganizerItemId(const QOrganizerItemId& other)
        : d(other.d)
{
}

/*! Assigns the organizeritem id to be equal to \a other */
QOrganizerItemId& QOrganizerItemId::operator=(const QOrganizerItemId& other)
{
    d = other.d;
    return *this;
}

/*! Returns true if the organizeritem id has the same manager URI and local id as \a other */
bool QOrganizerItemId::operator==(const QOrganizerItemId& other) const
{
    if (d->m_managerUri != other.d->m_managerUri)
        return false;
    if (d->m_localId != other.d->m_localId)
        return false;
    return true;
}

/*! Returns true if either the manager URI or local id of the organizeritem id is different to that of \a other */
bool QOrganizerItemId::operator!=(const QOrganizerItemId& other) const
{
    return !(*this == other);
}

/*! Returns true if this id is less than the \a other id.
    This id will be considered less than the \a other id if the
    manager URI of this id is alphabetically less than the manager
    URI of the \a other id.  If both ids have the same manager URI,
    this id will be considered less than the \a other id if the
    local id of this id is less than the local id of the \a other id.

    The invalid, empty id consists of an empty manager URI and the
    invalid, zero local id, and hence will be less than any non-invalid
    id.

    This operator is provided primarily to allow use of a QOrganizerItemId
    as a key in a QMap.
 */
bool QOrganizerItemId::operator<(const QOrganizerItemId& other) const
{
    const int comp = this->managerUri().compare(other.managerUri());
    if (comp != 0)
        return comp < 0;

    return this->localId() < other.localId();
}

/*!
 * Returns the hash value for \a key.
 */
uint qHash(const QOrganizerItemId &key)
{
    return QT_PREPEND_NAMESPACE(qHash)(key.managerUri())
            + QT_PREPEND_NAMESPACE(qHash)(key.localId());
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QOrganizerItemId& id)
{
    dbg.nospace() << "QOrganizerItemId(" << id.managerUri() << ", " << id.localId() << ")";
    return dbg.maybeSpace();
}
#endif

/*!
 * Returns the URI of the manager which contains the organizeritem identified by this id
 */
QString QOrganizerItemId::managerUri() const
{
    return d->m_managerUri;
}

/*!
 * Returns the manager-local id of the organizeritem identified by this organizeritem id
 */
QOrganizerItemLocalId QOrganizerItemId::localId() const
{
    return d->m_localId;
}

/*!
 * Sets the URI of the manager which contains the organizeritem identified by this id to \a uri
 */
void QOrganizerItemId::setManagerUri(const QString& uri)
{
    d->m_managerUri = uri;
}

/*!
 * Sets the manager-local id of the organizeritem identified by this organizeritem id to \a id
 */
void QOrganizerItemId::setLocalId(const QOrganizerItemLocalId& id)
{
    d->m_localId = id;
}

QTM_END_NAMESPACE
