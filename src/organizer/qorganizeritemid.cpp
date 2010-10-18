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

#include "qorganizeritemid.h"
#include "qorganizeritemengineid.h"
#include "qorganizermanager_p.h"
#include <QHash>
#include <QDebug>
#include <QDataStream>

#if !defined(Q_CC_MWERKS)
template<> QTM_PREPEND_NAMESPACE(QOrganizerItemEngineId) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QOrganizerItemEngineId)>::clone()
{
    return d ? d->clone() : 0;
}
#endif

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemId
  \brief The QOrganizerItemId class provides information that uniquely identifies
  an organizer item in a particular manager.

  \inmodule QtOrganizer

  It consists of a manager URI which identifies the manager which contains the organizer item,
  and the id of the organizer item in that manager.

  A "null" QOrganizerItemId has an empty manager URI
 */

/*!
 * Constructs a new organizer item id
 */
QOrganizerItemId::QOrganizerItemId()
        : d(0)
{
}

/*!
  Constructs a manager-unique id which wraps the given engine-unique item id
  \a engineItemId.  This id takes ownership of the engine-unique item id and
  will delete it when the id goes out of scope.  Engine implementors must not
  delete the \a engineItemId or undefined behaviour will occur.
 */
QOrganizerItemId::QOrganizerItemId(QOrganizerItemEngineId* engineItemId)
    : d(engineItemId)
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

/*! Returns true if the organizer item id has the same manager URI and id as \a other */
bool QOrganizerItemId::operator==(const QOrganizerItemId& other) const
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

/*! Returns true if either the manager URI or id of the organizer item id is different to that of \a other */
bool QOrganizerItemId::operator!=(const QOrganizerItemId& other) const
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

    This operator is provided primarily to allow use of a QOrganizerItemId
    as a key in a QMap.
 */
bool QOrganizerItemId::operator<(const QOrganizerItemId& other) const
{
    // a null id is always less than a non-null id.
    if (d == 0 && other.d != 0)
        return true;

    if (d && other.d) {
        // ensure they're of the same type (and therefore comparable)
        if (d->managerUri() == other.d->managerUri()) {
            return d->isLessThan(other.d);
        }
    }

    return false;
}

/*!
 * Returns the hash value for \a key.
 */
uint qHash(const QOrganizerItemId &key)
{
    if (key.d)
        return key.d->hash();
    return 0;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QOrganizerItemId& id)
{
    dbg.nospace() << "QOrganizerItemId(";
    if (id.isNull())
        dbg.nospace() << "(null))";
    else
        id.d->debugStreamOut(dbg)  << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
  Streams \a itemId to the data stream \a out
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerItemId& itemId)
{
    out << (itemId.toString());
    return out;
}

/*!
  Streams \a collectionId in from the data stream \a in
 */
QDataStream& operator>>(QDataStream& in, QOrganizerItemId& itemId)
{
    QString idString;
    in >> idString;
    itemId = QOrganizerItemId::fromString(idString);
    return in;
}
#endif


/*!
  Returns true if the local id part of the id is a null (default constructed) local id; otherwise, returns false.
 */
bool QOrganizerItemId::isNull() const
{
    return d == 0;
}

/*!
 * Returns the URI of the manager which contains the organizer item identified by this id
 */
QString QOrganizerItemId::managerUri() const
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
QString QOrganizerItemId::toString() const
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
QOrganizerItemId QOrganizerItemId::fromString(const QString& idString)
{
    QString managerName;
    QMap<QString, QString> params;
    QString engineIdString;

    if (!parseIdString(idString, &managerName, &params, &engineIdString))
        return QOrganizerItemId(); // invalid idString given.

    QOrganizerItemEngineId* engineId = QOrganizerManagerData::createEngineItemId(managerName, params, engineIdString);
    return QOrganizerItemId(engineId);
}

QTM_END_NAMESPACE
