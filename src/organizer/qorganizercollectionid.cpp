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

#include "qorganizercollectionid.h"
#include "qorganizercollectionid_p.h"
#include "qorganizercollectionenginelocalid.h"
#include "qorganizeritemmanager_p.h"
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
olle
  Most collection-related operations within a \l QOrganizerItemManager accept a QOrganizerCollectionLocalId.  Some operations
  (involving links to collections outside a particular manager) also accept a manager URI - this
  combination is stored in a \l QOrganizerCollectionId.

  A default-constructed QOrganizerCollectionLocalId is a null QOrganizerCollectionLocalId, which does not identify
  any item in a manager.

  \sa QOrganizerCollectionId
*/

/*!
  Constructs a new, null QOrganizerCollectionLocalId.
 */
QOrganizerCollectionLocalId::QOrganizerCollectionLocalId()
    : d(0)
{
}

/*!
  Cleans up any memory in use by this local id.
 */
QOrganizerCollectionLocalId::~QOrganizerCollectionLocalId()
{
    delete d;
}

/*!
  Constructs a manager-unique local id which wraps the given engine-unique item id
  \a engineItemId.  This local id takes ownership of the engine-unique item id and
  will delete it when the local id goes out of scope.  Engine implementors must not
  delete the \a engineItemId or undefined behaviour will occur.
 */
QOrganizerCollectionLocalId::QOrganizerCollectionLocalId(QOrganizerCollectionEngineLocalId* engineItemId)
    : d(engineItemId)
{
}

/*!
  Constructs a new copy of the \a other id.
 */
QOrganizerCollectionLocalId::QOrganizerCollectionLocalId(const QOrganizerCollectionLocalId& other)
{
    d = 0;
    if (other.d)
        d = other.d->clone();
}

/*!
  Assigns the \a other id to this id.
 */
QOrganizerCollectionLocalId& QOrganizerCollectionLocalId::operator=(const QOrganizerCollectionLocalId& other)
{
    // clean up our "old" engine id.
    if (d)
        delete d;

    // default case: other.d is a null ptr.
    d = 0;

    // if it isn't, clone the "new" engine id.
    if (other.d)
        d = other.d->clone();

    return *this;
}

/*!
  Returns true if this id is equal to the \a other id; otherwise returns false.
 */
bool QOrganizerCollectionLocalId::operator==(const QOrganizerCollectionLocalId& other) const
{
    // if both ids are null then they are equal.
    if (d == 0 && other.d == 0)
        return true;

    if (d && other.d) {
        // ensure they're of the same type (and therefore comparable)
        if (d->engineLocalIdType() && other.d->engineLocalIdType()) {
            return d->isEqualTo(other.d);
        }
    }
    return false;
}

/*!
  Returns true if this id is not equal to the \a other id; otherwise, returns false.
 */
bool QOrganizerCollectionLocalId::operator!=(const QOrganizerCollectionLocalId& other) const
{
    if (d && other.d) {
        // ensure they're of the same type (and therefore comparable)
        if (d->engineLocalIdType() && other.d->engineLocalIdType()) {
            return !(d->isEqualTo(other.d));
        }
    }
    return false;
}

/*!
  Returns true if this id is less than the \a other id; otherwise, returns false.
 */
bool QOrganizerCollectionLocalId::operator<(const QOrganizerCollectionLocalId& other) const
{
    // a null id is always less than a non-null id.
    if (d == 0 && other.d != 0)
        return true;

    if (d && other.d) {
        // ensure they're of the same type (and therefore comparable)
        if (d->engineLocalIdType() && other.d->engineLocalIdType()) {
            return d->isLessThan(other.d);
        }
    }

    return false;
}

/*!
  Returns true if this id is a null or default constructed id; otherwise, returns false.
 */
bool QOrganizerCollectionLocalId::isNull() const
{
    return (d == 0);
}

/*!
  Streams this id out to the debug stream \a dbg
 */
QDebug QOrganizerCollectionLocalId::debugStreamOut(QDebug dbg)
{
    if (d)
        return d->debugStreamOut(dbg);
    return dbg;
}

/*!
  Streams this id out to the data stream \a out
 */
QDataStream& QOrganizerCollectionLocalId::dataStreamOut(QDataStream& out)
{
    if (d)
        return d->dataStreamOut(out);
    return out;
}

/*!
  Streams this id in from the data stream \a in
 */
QDataStream& QOrganizerCollectionLocalId::dataStreamIn(QDataStream& in)
{
    if (d)
        return d->dataStreamIn(in);
    return in;
}

/*!
  Returns the hash value for this id.  Note that this hash value
  is only unique per manager (that is, you should not store a hash
  of local id to item, where the items come from multiple managers,
  since it is likely that collisions will occur).
 */
uint QOrganizerCollectionLocalId::hash() const
{
    if (d)
        return d->hash();
    return 0;
}

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
uint qHash(const QOrganizerCollectionLocalId &key)
{
    if (key.d)
        return QT_PREPEND_NAMESPACE(qHash)(key.d->hash());
    return 0;
}

/*!
 * Returns the hash value for \a key.
 */
uint qHash(const QOrganizerCollectionId &key)
{
    return QT_PREPEND_NAMESPACE(qHash)(key.managerUri())
            + qHash(key.localId());
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QOrganizerCollectionId& id)
{
    dbg.nospace() << "QOrganizerCollectionId(" << id.managerUri() << ", " << id.localId() << ")";
    return dbg.maybeSpace();
}

QDebug operator<<(QDebug dbg, const QOrganizerCollectionLocalId& id)
{
    return id.d->debugStreamOut(dbg);
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

QDataStream& operator<<(QDataStream& out, const QOrganizerCollectionLocalId& id)
{
    return id.d->dataStreamOut(out);
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
        in >> managerUri;
        QOrganizerCollectionLocalId localId(QOrganizerItemManagerData::createEngineCollectionLocalId(managerUri));
        collectionId = QOrganizerCollectionId();
        if (localId.d) {
            collectionId.setManagerUri(managerUri);
            localId.d->dataStreamIn(in);
        }
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
