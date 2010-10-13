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
#include "qorganizermanager_p.h"
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
bool QOrganizerCollectionLocalId::operator==(const QOrganizerCollectionLocalId& other) const
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
bool QOrganizerCollectionLocalId::operator!=(const QOrganizerCollectionLocalId& other) const
{
    return !(*this == other);
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
        if (d->engineLocalIdType() == other.d->engineLocalIdType()) {
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
 * Constructs a new, null collection id
 */
QOrganizerCollectionId::QOrganizerCollectionId()
        : d(new QOrganizerCollectionIdPrivate)
{
}

/*!
 * Cleans up the memory in use by the collection id
 */
QOrganizerCollectionId::~QOrganizerCollectionId()
{
}

/*! Constructs a new collection id as a copy of \a other */
QOrganizerCollectionId::QOrganizerCollectionId(const QOrganizerCollectionId& other)
        : d(other.d)
{
}

/*! Assigns the collection id to be equal to \a other */
QOrganizerCollectionId& QOrganizerCollectionId::operator=(const QOrganizerCollectionId& other)
{
    d = other.d;
    return *this;
}

/*! Returns true if the collection id has the same manager URI and local id as \a other */
bool QOrganizerCollectionId::operator==(const QOrganizerCollectionId& other) const
{
    if (d->m_managerUri != other.d->m_managerUri)
        return false;
    if (d->m_localId != other.d->m_localId)
        return false;
    return true;
}

/*! Returns true if either the manager URI or local id of the collection id is different to that of \a other */
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
  Returns true if the local id part of this id is a null (default constructed) local id; otherwise, returns false.
 */
bool QOrganizerCollectionId::isNull() const
{
    return d->m_localId.isNull();
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
    if (id.d) {
        return id.d->debugStreamOut(dbg);
    }
    dbg << QString(QLatin1String("(null)"));
    return dbg;
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
 * Writes \a collectionId to the stream \a out.
 * Note that if the manager uri of \a collectionId is empty or invalid, operator>>() will
 * not be able to reconstruct the id from the data.
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerCollectionId& collectionId)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QOrganizerCollectionId
    out << formatVersion
        << collectionId.managerUri();

    // if the manager uri is null, there'd be no way to deserialize
    // the local id.  So, we don't serialize out the local id.
    if (collectionId.managerUri().isEmpty())
        return out;

    // the manager uri is not null.  we can serialize out the local id.
    out << collectionId.localId();
    return out;
}
/*!
 * Writes the manager-local collection id \a id to the stream \a out.
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerCollectionLocalId& id)
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
 * Reads an organizer collection id from stream \a in into \a collectionId.
 * Note that if the manager uri of the id which was streamed out was empty,
 * this function will produce a null \a collectionId.
 */
QDataStream& operator>>(QDataStream& in, QOrganizerCollectionId& collectionId)
{
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        QString managerUri;
        in >> managerUri;
        if (managerUri.isEmpty()) {
            // if no manager uri was set in the id which was serialized, then nothing can be deserialized.
            // in this case, return the null id.
            collectionId = QOrganizerCollectionId();
            return in;
        }

        quint8 localIdMarker = static_cast<quint8>(false);
        in >> localIdMarker;
        QOrganizerCollectionLocalId localId(QOrganizerManagerData::createEngineCollectionLocalId(managerUri));
        if (localIdMarker == static_cast<quint8>(true)) {
            if (localId.d) {
                // only stream in the local id data if it exists.
                localId.d->dataStreamIn(in);
            } else {
                // the local id should be the null local id.
                localId = QOrganizerCollectionLocalId();
            }
        } else {
            // the local id should be the null local id.
            localId = QOrganizerCollectionLocalId();
        }

        collectionId.setManagerUri(managerUri);
        collectionId.setLocalId(localId);
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}

#endif


/*!
 * Returns the URI of the manager which contains the collection identified by this id
 */
QString QOrganizerCollectionId::managerUri() const
{
    return d->m_managerUri;
}

/*!
 * Returns the manager-local id of the collection identified by this collection id
 */
QOrganizerCollectionLocalId QOrganizerCollectionId::localId() const
{
    return d->m_localId;
}

/*!
 * Sets the URI of the manager which contains the collection item identified by this id to \a uri.
 * If the old manager URI was different to \a uri, the local id will be set to the null local id.
 * \sa localId()
 */
void QOrganizerCollectionId::setManagerUri(const QString& uri)
{
    if (!d->m_managerUri.isEmpty() && d->m_managerUri != uri)
        d->m_localId = QOrganizerCollectionLocalId();
    d->m_managerUri = uri;
}

/*!
 * Sets the manager-local id of the collection identified by this collection id to \a id
 */
void QOrganizerCollectionId::setLocalId(const QOrganizerCollectionLocalId& id)
{
    d->m_localId = id;
}

QTM_END_NAMESPACE
