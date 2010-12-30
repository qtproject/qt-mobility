/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
        if (d->managerUri() == other.d->managerUri()) {
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
        if (d->managerUri() == other.d->managerUri()) {
            return d->isLessThan(other.d);
        }

        // not the same type?  just compare the manager uri.
        return d->managerUri() < other.d->managerUri();
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
        return key.d->hash();
    return 0;
}

#ifndef QT_NO_DEBUG_STREAM
/*!
  Outputs \a id to the debug stream \a dbg
 */
QDebug operator<<(QDebug dbg, const QOrganizerCollectionId& id)
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
  Streams \a collectionId to the data stream \a out
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerCollectionId& collectionId)
{
    out << (collectionId.toString());
    return out;
}

/*!
  Streams \a collectionId in from the data stream \a in
 */
QDataStream& operator>>(QDataStream& in, QOrganizerCollectionId& collectionId)
{
    QString idString;
    in >> idString;
    collectionId = QOrganizerCollectionId::fromString(idString);
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

/*!
  Builds a string from the given \a managerName, \a params and \a engineIdString
 */
inline QString buildIdString(const QString& managerName, const QMap<QString, QString>& params, const QString& engineIdString)
{
    // the constructed id string will be of the form: "qtorganizer:managerName:param1=value1&param2=value2:
    QString ret(QLatin1String("qtorganizer:%1:%2:%3"));

    // we have to escape each param
    QStringList escapedParams;
    QStringList keys = params.keys();
    for (int i=0; i < keys.size(); i++) {
        QString key = keys.at(i);
        QString arg = params.value(key);
        arg = arg.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        arg = arg.replace(QLatin1Char('='), QLatin1String("&equ;"));
        arg = arg.replace(QLatin1Char(':'), QLatin1String("&#58;"));
        key = key.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        key = key.replace(QLatin1Char('='), QLatin1String("&equ;"));
        key = key.replace(QLatin1Char(':'), QLatin1String("&#58;"));
        key = key + QLatin1Char('=') + arg;
        escapedParams.append(key);
    }

    // and we escape the engine id string.
    QString escapedEngineId = engineIdString;
    escapedEngineId.replace(QLatin1Char('&'), QLatin1String("&amp;"));
    escapedEngineId.replace(QLatin1Char(':'), QLatin1String("&#58;"));

    return ret.arg(managerName, escapedParams.join(QLatin1String("&")), escapedEngineId);
}

/*!
  Parses the individual components of the given \a idString and fills the \a managerName, \a params and \a engineIdString.
  Returns true if the parts could be parsed successfully, false otherwise.
 */
inline bool parseIdString(const QString& idString, QString* managerName, QMap<QString, QString>* params, QString* engineIdString)
{
    QStringList colonSplit = idString.split(QLatin1Char(':'));

    QString prefix = colonSplit.value(0);
    if (prefix != QLatin1String("qtorganizer") || colonSplit.size() != 4)
        return false; // invalid serialized string.  we cannot continue.

    QString mgrName = colonSplit.value(1);
    QString paramString = colonSplit.value(2);
    QString engIdString = colonSplit.value(3);

    // Now we have to decode each parameter
    QMap<QString, QString> outParams;
    if (!paramString.isEmpty()) {
        QStringList params = paramString.split(QRegExp(QLatin1String("&(?!(amp;|equ;))")), QString::KeepEmptyParts);
        // If we have an empty string for paramstring, we get one entry in params,
        // so skip that case.
        for(int i = 0; i < params.count(); i++) {
            /* This should be something like "foo&amp;bar&equ;=grob&amp;" */
            QStringList paramChunk = params.value(i).split(QLatin1String("="), QString::KeepEmptyParts);

            if (paramChunk.count() != 2)
                return false;

            QString arg = paramChunk.value(0);
            QString param = paramChunk.value(1);

            arg.replace(QLatin1String("&#58;"), QLatin1String(":"));
            arg.replace(QLatin1String("&equ;"), QLatin1String("="));
            arg.replace(QLatin1String("&amp;"), QLatin1String("&"));
            param.replace(QLatin1String("&#58;"), QLatin1String(":"));
            param.replace(QLatin1String("&equ;"), QLatin1String("="));
            param.replace(QLatin1String("&amp;"), QLatin1String("&"));
            if (arg.isEmpty())
                return false;
            outParams.insert(arg, param);
        }
    }

    // and unescape the engine id string.
    engIdString.replace(QLatin1String("&#58;"), QLatin1String(":"));
    engIdString.replace(QLatin1String("&amp;"), QLatin1String("&"));

    // now fill the return values.
    if (managerName)
        *managerName = mgrName;
    if (params)
        *params = outParams;
    if (engineIdString)
        *engineIdString = engIdString;

    // and return.
    return true;
}

/*!
  Serializes the id to a string.  The format of the string will be:
  "qtorganizer:managerName:constructionParams:serializedEngineLocalItemId"
 */
QString QOrganizerCollectionId::toString() const
{
    QString mgrName;
    QMap<QString, QString> params;
    QString engineId;

    if (d) {
        QOrganizerManager::parseUri(d->managerUri(), &mgrName, &params);
        engineId = d->toString();
    }

    // having extracted the params the name, we now need to build a new string.
    return buildIdString(mgrName, params, engineId);
}

/*!
  Deserializes the given \a idString.  Returns a default-constructed (null)
  item id if the given \a idString is not a valid, serialized item id, or
  if the manager engine from which the id came could not be found.
 */
QOrganizerCollectionId QOrganizerCollectionId::fromString(const QString& idString)
{
    QString managerName;
    QMap<QString, QString> params;
    QString engineIdString;

    if (!parseIdString(idString, &managerName, &params, &engineIdString))
        return QOrganizerCollectionId(); // invalid idString given.

    QOrganizerCollectionEngineId* engineId = QOrganizerManagerData::createEngineCollectionId(managerName, params, engineIdString);
    return QOrganizerCollectionId(engineId);
}

QTM_END_NAMESPACE
