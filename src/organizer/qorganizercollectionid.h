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

#ifndef QORGANIZERCOLLECTIONID_H
#define QORGANIZERCOLLECTIONID_H

#include <QMap>
#include <QString>
#include <QSharedDataPointer>

#include "qtorganizerglobal.h"

QTM_BEGIN_NAMESPACE

// MSVC needs the function declared before the friend declaration
class QOrganizerCollectionId;
Q_ORGANIZER_EXPORT uint qHash(const QOrganizerCollectionId& key);
#ifndef QT_NO_DATASTREAM
Q_ORGANIZER_EXPORT QDataStream& operator<<(QDataStream& out, const QOrganizerCollectionId& collectionId);
Q_ORGANIZER_EXPORT QDataStream& operator>>(QDataStream& in, QOrganizerCollectionId& collectionId);
#endif
#ifndef QT_NO_DEBUG_STREAM
Q_ORGANIZER_EXPORT QDebug operator<<(QDebug dbg, const QOrganizerCollectionId& id);
#endif

class QOrganizerManagerEngine;
class QOrganizerCollectionEngineId;

class Q_ORGANIZER_EXPORT QOrganizerCollectionId
{
public:
    QOrganizerCollectionId();
    ~QOrganizerCollectionId();

    explicit QOrganizerCollectionId(QOrganizerCollectionEngineId* engineId);
    QOrganizerCollectionId(const QOrganizerCollectionId& other);
    QOrganizerCollectionId& operator=(const QOrganizerCollectionId& other);

    bool operator==(const QOrganizerCollectionId& other) const;
    bool operator!=(const QOrganizerCollectionId& other) const;
    bool operator<(const QOrganizerCollectionId& other) const;

    bool isNull() const;

    QString managerUri() const;

    QString toString() const;
    static QOrganizerCollectionId fromString(const QString& idString);

private:
    QSharedDataPointer<QOrganizerCollectionEngineId> d;

#ifndef QT_NO_DEBUG_STREAM
    Q_ORGANIZER_EXPORT friend QDebug operator<<(QDebug dbg, const QOrganizerCollectionId& id);
#endif
    Q_ORGANIZER_EXPORT friend uint qHash(const QOrganizerCollectionId& key);
    friend class QOrganizerManagerEngine;
};

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerCollectionId), Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QOrganizerCollectionId))

#endif

