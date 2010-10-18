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

#ifndef QORGANIZERITEMID_H
#define QORGANIZERITEMID_H

#include <QString>
#include <QSharedDataPointer>

#include "qtorganizerglobal.h"

class QDataStream;

QTM_BEGIN_NAMESPACE

// MSVC needs the function declared before the friend declaration
class QOrganizerItemLocalId;
class QOrganizerItemId;
Q_ORGANIZER_EXPORT uint qHash(const QOrganizerItemLocalId& key);
Q_ORGANIZER_EXPORT uint qHash(const QOrganizerItemId& key);
#ifndef QT_NO_DEBUG_STREAM
Q_ORGANIZER_EXPORT QDebug operator<<(QDebug dbg, const QOrganizerItemLocalId& id);
Q_ORGANIZER_EXPORT QDebug operator<<(QDebug dbg, const QOrganizerItemId& id);
#endif
#ifndef QT_NO_DATASTREAM
Q_ORGANIZER_EXPORT QDataStream& operator<<(QDataStream& out, const QOrganizerItemLocalId& id);
Q_ORGANIZER_EXPORT QDataStream& operator<<(QDataStream& out, const QOrganizerItemId& id);
Q_ORGANIZER_EXPORT QDataStream& operator>>(QDataStream& in, QOrganizerItemId& id);
#endif

class QOrganizerItemManagerEngine;
class QOrganizerItemEngineLocalId;

class QOrganizerItemId;
class Q_ORGANIZER_EXPORT QOrganizerItemLocalId
{
public:
    QOrganizerItemLocalId();
    explicit QOrganizerItemLocalId(QOrganizerItemEngineLocalId* engineId);

    ~QOrganizerItemLocalId();

    QOrganizerItemLocalId(const QOrganizerItemLocalId& other);
    QOrganizerItemLocalId& operator=(const QOrganizerItemLocalId& other);

    bool operator==(const QOrganizerItemLocalId& other) const;
    bool operator!=(const QOrganizerItemLocalId& other) const;
    bool operator<(const QOrganizerItemLocalId& other) const;

    bool isNull() const;

private:
    QOrganizerItemEngineLocalId* d;

#ifndef QT_NO_DEBUG_STREAM
    Q_ORGANIZER_EXPORT friend QDebug operator<<(QDebug dbg, const QOrganizerItemLocalId& id);
#endif
#ifndef QT_NO_DATASTREAM
    Q_ORGANIZER_EXPORT friend QDataStream& operator<<(QDataStream& out, const QOrganizerItemLocalId& id);
    Q_ORGANIZER_EXPORT friend QDataStream& operator>>(QDataStream& in, QOrganizerItemId& id);
#endif
    Q_ORGANIZER_EXPORT friend uint qHash(const QOrganizerItemLocalId& key);
    friend class QOrganizerItemManagerEngine;
};

class QOrganizerItemIdPrivate;
class Q_ORGANIZER_EXPORT QOrganizerItemId
{
public:
    QOrganizerItemId();
    ~QOrganizerItemId();

    QOrganizerItemId(const QOrganizerItemId& other);
    QOrganizerItemId& operator=(const QOrganizerItemId& other);

    bool operator==(const QOrganizerItemId& other) const;
    bool operator!=(const QOrganizerItemId& other) const;
    bool operator<(const QOrganizerItemId& other) const;

    bool isNull() const;

    QString managerUri() const;
    QOrganizerItemLocalId localId() const;

    void setManagerUri(const QString& uri);
    void setLocalId(const QOrganizerItemLocalId& id);

private:
    QSharedDataPointer<QOrganizerItemIdPrivate> d;
};

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemLocalId), Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemId), Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QOrganizerItemLocalId));
Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QOrganizerItemId));


#endif

