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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizercollectionid.h"
#include "qorganizercollectionid_p.h"
#include <QHash>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerCollectionId
  \brief The QOrganizerCollectionId class provides information that uniquely identifies
  a collection in a particular manager.

  It consists of a manager URI which identifies the manager which manages the collection,
  and the local id of the collection in that manager.

  A "null" QOrganizerCollectionId has an empty manager URI, and an invalid QOrganizerCollectionLocalId (0).

  \sa QOrganizerCollectionLocalId
 */

/*!
  \typedef QOrganizerCollectionLocalId
  \relates QOrganizerCollectionId
  \brief The QOrganizerCollectionLocalId type represents the unique id of a collection within its manager.

  Most collection-related operations within a \l QOrganizerItemManager accept a QOrganizerCollectionLocalId.  Some operations
  (involving links to collections outside a particular manager) also accept a manager URI - this
  combination is stored in a \l QOrganizerCollectionId.

  An invalid QOrganizerCollectionLocalId is represented by a zero (0) value.

  \sa QOrganizerCollectionId
*/

/*!
 * Constructs a new organizeritem id
 */
QOrganizerCollectionId::QOrganizerCollectionId()
        : d(new QOrganizerCollectionIdPrivate)
{
}

/*!
 * Cleans up the memory in use by the organizeritem id
 */
QOrganizerCollectionId::~QOrganizerCollectionId()
{
}

/*! Constructs a new organizeritem id as a copy of \a other */
QOrganizerCollectionId::QOrganizerCollectionId(const QOrganizerCollectionId& other)
        : d(other.d)
{
}

/*! Assigns the organizeritem id to be equal to \a other */
QOrganizerCollectionId& QOrganizerCollectionId::operator=(const QOrganizerCollectionId& other)
{
    d = other.d;
    return *this;
}

/*! Returns true if the organizeritem id has the same manager URI and local id as \a other */
bool QOrganizerCollectionId::operator==(const QOrganizerCollectionId& other) const
{
    if (d->m_managerUri != other.d->m_managerUri)
        return false;
    if (d->m_localId != other.d->m_localId)
        return false;
    return true;
}

/*! Returns true if either the manager URI or local id of the organizeritem id is different to that of \a other */
bool QOrganizerCollectionId::operator!=(const QOrganizerCollectionId& other) const
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

    This operator is provided primarily to allow use of a QOrganizerCollectionId
    as a key in a QMap.
 */
bool QOrganizerCollectionId::operator<(const QOrganizerCollectionId& other) const
{
    const int comp = this->managerUri().compare(other.managerUri());
    if (comp != 0)
        return comp < 0;

    return this->localId() < other.localId();
}

/*!
 * Returns the hash value for \a key.
 */
uint qHash(const QOrganizerCollectionId &key)
{
    return QT_PREPEND_NAMESPACE(qHash)(key.managerUri())
            + QT_PREPEND_NAMESPACE(qHash)(key.localId());
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QOrganizerCollectionId& id)
{
    dbg.nospace() << "QOrganizerCollectionId(" << id.managerUri() << ", " << id.localId() << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
 * Writes \a collectionId to the stream \a out.
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerCollectionId& collectionId)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QOrganizerCollectionId
    return out << formatVersion
               << collectionId.managerUri()
               << collectionId.localId();
}

/*!
 * Reads an organizer collection id from stream \a in into \a collectionId.
 */
QDataStream& operator>>(QDataStream& in, QOrganizerCollectionId& collectionId)
{
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        QString managerUri;
        QOrganizerCollectionLocalId lid;
        in >> managerUri >> lid;
        collectionId = QOrganizerCollectionId();
        collectionId.setManagerUri(managerUri);
        collectionId.setLocalId(lid);
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}
#endif

/*!
 * Returns the URI of the manager which contains the organizeritem identified by this id
 */
QString QOrganizerCollectionId::managerUri() const
{
    return d->m_managerUri;
}

/*!
 * Returns the manager-local id of the organizeritem identified by this organizeritem id
 */
QOrganizerCollectionLocalId QOrganizerCollectionId::localId() const
{
    return d->m_localId;
}

/*!
 * Sets the URI of the manager which contains the organizeritem identified by this id to \a uri
 */
void QOrganizerCollectionId::setManagerUri(const QString& uri)
{
    d->m_managerUri = uri;
}

/*!
 * Sets the manager-local id of the organizeritem identified by this organizeritem id to \a id
 */
void QOrganizerCollectionId::setLocalId(const QOrganizerCollectionLocalId& id)
{
    d->m_localId = id;
}

QTM_END_NAMESPACE
