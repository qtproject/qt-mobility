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
#include "qorganizeritemenginelocalid.h"
#include "qorganizermanager_p.h"
#include <QHash>
#include <QDebug>
#include <QDataStream>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemId
  \brief The QOrganizerItemId class provides information that uniquely identifies
  an organizer item in a particular manager.

  \inmodule QtOrganizer

  It consists of a manager URI which identifies the manager which contains the organizer item,
  and the local id of the organizer item in that manager.

  A "null" QOrganizerItemId has an empty manager URI, and an invalid QOrganizerItemLocalId (0).

  \sa QOrganizerItemLocalId
 */

/*!
  \class QOrganizerItemLocalId
  \relates QOrganizerItemId
  \brief The QOrganizerItemLocalId class represents the unique id of an organizer item within its manager.

  Most operations within a \l QOrganizerManager accept a QOrganizerItemLocalId.  Some operations
  (involving links to organizer items outside a particular manager) also accept a manager URI - this
  combination is stored in a \l QOrganizerItemId.

  A default-constructed QOrganizerItemLocalId is a null QOrganizerItemLocalId, which does not identify
  any item in a manager.  Clients may wish to set the local id of an item to the default-constructed id
  if they wish to copy that item from one manager to another (by calling QOrganizerManager::saveItem()),
  whereupon that manager will replace the id with one that identifies the newly created item.

  \sa QOrganizerItemId
*/

/*!
  Constructs a new, null QOrganizerItemLocalId.
 */
QOrganizerItemLocalId::QOrganizerItemLocalId()
    : d(0)
{
}

/*!
  Cleans up any memory in use by this local id.
 */
QOrganizerItemLocalId::~QOrganizerItemLocalId()
{
    delete d;
}

/*!
  Constructs a manager-unique local id which wraps the given engine-unique item id
  \a engineItemId.  This local id takes ownership of the engine-unique item id and
  will delete it when the local id goes out of scope.  Engine implementors must not
  delete the \a engineItemId or undefined behaviour will occur.
 */
QOrganizerItemLocalId::QOrganizerItemLocalId(QOrganizerItemEngineLocalId* engineItemId)
    : d(engineItemId)
{
}

/*!
  Constructs a new copy of the \a other id.
 */
QOrganizerItemLocalId::QOrganizerItemLocalId(const QOrganizerItemLocalId& other)
{
    if (other.d)
        d = other.d->clone();
    else
        d = 0;
}

/*!
  Assigns the \a other id to this id.
 */
QOrganizerItemLocalId& QOrganizerItemLocalId::operator=(const QOrganizerItemLocalId& other)
{
    if (d == other.d)
        return *this;

    // clean up our "old" engine id.
    if (d)
        delete d;

    if (other.d)
        d = other.d->clone();
    else
        d = 0;

    return *this;
}

/*!
  Returns true if this id is equal to the \a other id; otherwise returns false.
 */
bool QOrganizerItemLocalId::operator==(const QOrganizerItemLocalId& other) const
{
    // if both ids are null then they are equal.
    if (d == 0 && other.d == 0)
        return true;

    if (d && other.d) {
        // ensure they're of the same type (and therefore comparable)
        if (d->engineLocalIdType() == other.d->engineLocalIdType()) {
            return d->isEqualTo(other.d);
        }
    }
    return false;
}

/*!
  Returns true if this id is not equal to the \a other id; otherwise, returns false.
 */
bool QOrganizerItemLocalId::operator!=(const QOrganizerItemLocalId& other) const
{
    return !(*this == other);
}

/*!
  Returns true if this id is less than the \a other id; otherwise, returns false.
 */
bool QOrganizerItemLocalId::operator<(const QOrganizerItemLocalId& other) const
{
    // a null id is always less than a non-null id.
    if (d == 0 && other.d != 0)
        return true;

    if (d && other.d) {
        // ensure they're of the same type (and therefore comparable)
        if (d->engineLocalIdType() == other.d->engineLocalIdType()) {
            return d->isLessThan(other.d);
        }
    }

    return false;
}

/*!
  Returns true if this id is a null or default constructed id; otherwise, returns false.
 */
bool QOrganizerItemLocalId::isNull() const
{
    return (d == 0);
}

/*!
 * Constructs a new organizer item id
 */
QOrganizerItemId::QOrganizerItemId()
        : d(new QOrganizerItemIdPrivate)
{
}

/*!
 * Cleans up the memory in use by the organizer item id
 */
QOrganizerItemId::~QOrganizerItemId()
{
}

/*! Constructs a new organizer item id as a copy of \a other */
QOrganizerItemId::QOrganizerItemId(const QOrganizerItemId& other)
        : d(other.d)
{
}

/*! Assigns the organizer item id to be equal to \a other */
QOrganizerItemId& QOrganizerItemId::operator=(const QOrganizerItemId& other)
{
    d = other.d;
    return *this;
}

/*! Returns true if the organizer item id has the same manager URI and local id as \a other */
bool QOrganizerItemId::operator==(const QOrganizerItemId& other) const
{
    if (d && other.d)
        return (d->m_managerUri == other.d->m_managerUri) &&
               (d->m_localId == other.d->m_localId);

    return !d && !other.d;
}

/*! Returns true if either the manager URI or local id of the organizer item id is different to that of \a other */
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
uint qHash(const QOrganizerItemLocalId &key)
{
    if (key.d)
        return QT_PREPEND_NAMESPACE(qHash)(key.d->hash());
    return 0;
}

/*!
 * Returns the hash value for \a key.
 */
uint qHash(const QOrganizerItemId &key)
{
    return QT_PREPEND_NAMESPACE(qHash)(key.managerUri())
            + qHash(key.localId());
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QOrganizerItemLocalId& id)
{
    if (id.d) {
        return id.d->debugStreamOut(dbg);
    }

    dbg << QString(QLatin1String("(null)")); // no local id data.
    return dbg;
}

QDebug operator<<(QDebug dbg, const QOrganizerItemId& id)
{
    dbg.nospace() << "QOrganizerItemId(" << id.managerUri() << ", " << id.localId() << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& operator<<(QDataStream& out, const QOrganizerItemLocalId& id)
{
    if (id.d) {
        // we include a marker which contains "true" if there is local id data to be streamed.
        out << static_cast<quint8>(true);
        return id.d->dataStreamOut(out);
    }

    out << static_cast<quint8>(false);
    return out;
}

/*!
 * Writes \a id to the stream \a out.
 * Note that if the manager uri of \a id is empty or invalid, operator>>() would
 * not be able to reconstruct the id from the data, so the id is not serialized,
 * to allow operator>>() to reconstruct the rest of the data properly.
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerItemId& id)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QOrganizerItemId
    out << formatVersion
        << id.managerUri();

    // if the manager uri is null, there'd be no way to deserialize
    // the local id.  So, we don't serialize out the local id.
    if (id.managerUri().isEmpty())
        return out;

    // the manager uri is not null.  we can serialize out the local id.
    out << id.localId();
    return out;
}

/*!
 * Reads \a id in from the stream \a in.
 * Note that if the manager uri of \a id is empty or invalid, operator>>() would
 * not be able to reconstruct the id from the data, and hence it will return
 * a null id.
 */
QDataStream& operator>>(QDataStream& in, QOrganizerItemId& id)
{
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        QString managerUri;
        in >> managerUri;
        if (managerUri.isEmpty()) {
            // if no manager uri was set in the id which was serialized, then nothing can be deserialized.
            // in this case, return the null id.
            id = QOrganizerItemId();
            return in;
        }
        quint8 localIdMarker = static_cast<quint8>(false);
        in >> localIdMarker;
        QOrganizerItemLocalId localId(QOrganizerManagerData::createEngineItemLocalId(managerUri));
        if (localIdMarker == static_cast<quint8>(true)) {
            if (localId.d) {
                // only try to stream in data if it exists and the engine could create an engine
                // specific localId based on the managerUri. otherwise, skip it.
                localId.d->dataStreamIn(in);
            } else {
                // the local id should be the null local id
                localId = QOrganizerItemLocalId();
            }
        } else {
            // the local id should be the null local id
            localId = QOrganizerItemLocalId();
        }

        id.setManagerUri(managerUri);
        id.setLocalId(localId);
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}
#endif

/*!
  Returns true if the local id part of the id is a null (default constructed) local id; otherwise, returns false.
 */
bool QOrganizerItemId::isNull() const
{
    return d->m_localId.isNull();
}

/*!
 * Returns the URI of the manager which contains the organizer item identified by this id
 */
QString QOrganizerItemId::managerUri() const
{
    return d->m_managerUri;
}

/*!
 * Returns the manager-local id of the organizer item identified by this organizer item id
 */
QOrganizerItemLocalId QOrganizerItemId::localId() const
{
    return d->m_localId;
}

/*!
 * Sets the URI of the manager which contains the organizer item identified by this id to \a uri.
 * If the old manager URI was different to \a uri, the local id will be set to the null local id.
 * \sa localId()
 */
void QOrganizerItemId::setManagerUri(const QString& uri)
{
    if (!d->m_managerUri.isEmpty() && d->m_managerUri != uri)
        d->m_localId = QOrganizerItemLocalId();
    d->m_managerUri = uri;
}

/*!
 * Sets the manager-local id of the organizer item identified by this organizer item id to \a id
 */
void QOrganizerItemId::setLocalId(const QOrganizerItemLocalId& id)
{
    d->m_localId = id;
}

QTM_END_NAMESPACE
