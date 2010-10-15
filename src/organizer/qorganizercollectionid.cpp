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
#include "qorganizercollectionengineid.h"
#include "qorganizermanager_p.h"
#include <QHash>
#include <QDebug>

#if !defined(Q_CC_MWERKS)
template<> QTM_PREPEND_NAMESPACE(QOrganizerCollectionEngineId) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QOrganizerCollectionEngineId)>::clone()
{
    return d ? d->clone() : 0;
}
#endif

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerCollectionId
  \brief The QOrganizerCollectionId class provides information that uniquely identifies
  a collection in a particular manager.

  It consists of a manager URI which identifies the manager which manages the collection,
  and the id of the collection in that manager.

  A "null" QOrganizerCollectionId has an empty manager URI, and an invalid QOrganizerCollectionId (0).
 */

/*!
 * Constructs a new, null collection id
 */
QOrganizerCollectionId::QOrganizerCollectionId()
        : d(0)
{
}

/*!
 * Cleans up the memory in use by the collection id
 */
QOrganizerCollectionId::~QOrganizerCollectionId()
{
}

/*!
  Constructs a manager-unique id which wraps the given engine-unique item id
  \a engineItemId.  This id takes ownership of the engine-unique item id and
  will delete it when the id goes out of scope.  Engine implementors must not
  delete the \a engineItemId or undefined behaviour will occur.
 */
QOrganizerCollectionId::QOrganizerCollectionId(QOrganizerCollectionEngineId* engineItemId)
    : d(engineItemId)
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

/*! Returns true if the collection id has the same manager URI and id as \a other */
bool QOrganizerCollectionId::operator==(const QOrganizerCollectionId& other) const
{
    // if both ids are null then they are equal.
    if (d == 0 && other.d == 0)
        return true;

    if (d && other.d) {
        // ensure they're of the same type (and therefore comparable)
        if (d->engineIdType() == other.d->engineIdType()) {
            return d->isEqualTo(other.d);
        }
    }
    return false;
}

/*! Returns true if either the manager URI or id of the collection id is different to that of \a other */
bool QOrganizerCollectionId::operator!=(const QOrganizerCollectionId& other) const
{
    return !(*this == other);
}

/*! Returns true if this id is less than the \a other id.
    This id will be considered less than the \a other id if the
    manager URI of this id is alphabetically less than the manager
    URI of the \a other id.  If both ids have the same manager URI,
    this id will be considered less than the \a other id if the
    id of this id is less than the id of the \a other id.

    The invalid, empty id consists of an empty manager URI and the
    invalid, zero id, and hence will be less than any non-invalid
    id.

    This operator is provided primarily to allow use of a QOrganizerCollectionId
    as a key in a QMap.
 */
bool QOrganizerCollectionId::operator<(const QOrganizerCollectionId& other) const
{
    // a null id is always less than a non-null id.
    if (d == 0 && other.d != 0)
        return true;

    if (d && other.d) {
        // ensure they're of the same type (and therefore comparable)
        if (d->engineIdType() == other.d->engineIdType()) {
            return d->isLessThan(other.d);
        }
    }

    return false;
}

/*!
  Returns true if the id part of this id is a null (default constructed) id; otherwise, returns false.
 */
bool QOrganizerCollectionId::isNull() const
{
    return (d == 0);
}

/*!
 * Returns the hash value for \a key.
 */
uint qHash(const QOrganizerCollectionId &key)
{
    if (key.d)
        return QT_PREPEND_NAMESPACE(qHash)(key.d->hash());
    return 0;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug& operator<<(QDebug& dbg, const QOrganizerCollectionId& id)
{
    dbg.nospace() << "QOrganizerCollectionId(";
    if (id.isNull())
        dbg.nospace() << "(null))";
    else
        id.d->debugStreamOut(dbg)  << ")";
    return dbg.maybeSpace();
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

    // the id is not null. we can serialize out
    if (collectionId.d)
        collectionId.d->dataStreamOut(out);

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

        collectionId = QOrganizerCollectionId(QOrganizerManagerData::createEngineCollectionId(managerUri));
        if (collectionId.d) {
            // only stream in the id data if it exists.
            collectionId.d->dataStreamIn(in);
        } else {
            // the id should be the null id.
            collectionId = QOrganizerCollectionId();
        }
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
    return d ? d->managerUri() : QString();
}

QTM_END_NAMESPACE
