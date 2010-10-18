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

#ifndef QORGANIZERMAEMO5IDS_P_H
#define QORGANIZERMAEMO5IDS_P_H

#include "qtorganizer.h"
#include "qorganizeritemenginelocalid.h"
#include "qorganizercollectionenginelocalid.h"

QTM_USE_NAMESPACE

class OrganizerCalendarDatabaseAccess;
class OrganizerItemTransform;     // forward declare maemo5 transform.
class QOrganizerItemMaemo5Engine; // forward declare maemo5 engine.
class OrganizerAsynchProcess;     // forward declare maemo5 asynchronous process.

class QOrganizerCollectionMaemo5EngineLocalId : public QOrganizerCollectionEngineLocalId
{
public:
    QOrganizerCollectionMaemo5EngineLocalId();
    QOrganizerCollectionMaemo5EngineLocalId(quint32 collectionId);
    ~QOrganizerCollectionMaemo5EngineLocalId();
    QOrganizerCollectionMaemo5EngineLocalId(const QOrganizerCollectionMaemo5EngineLocalId& other);

    bool isEqualTo(const QOrganizerCollectionEngineLocalId* other) const;
    bool isLessThan(const QOrganizerCollectionEngineLocalId* other) const;

    uint engineLocalIdType() const;
    QOrganizerCollectionEngineLocalId* clone() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug debugStreamOut(QDebug dbg);
#endif
#ifndef QT_NO_DATASTREAM
    QDataStream& dataStreamOut(QDataStream& out);
    QDataStream& dataStreamIn(QDataStream& in);
#endif
    uint hash() const;

public:
    quint32 m_localCollectionId;
    friend class OrganizerAsynchProcess;
};

class QOrganizerItemMaemo5EngineLocalId : public QOrganizerItemEngineLocalId
{
public:
    QOrganizerItemMaemo5EngineLocalId();
    QOrganizerItemMaemo5EngineLocalId(quint32 itemId);
    ~QOrganizerItemMaemo5EngineLocalId();
    QOrganizerItemMaemo5EngineLocalId(const QOrganizerItemMaemo5EngineLocalId& other);

    bool isEqualTo(const QOrganizerItemEngineLocalId* other) const;
    bool isLessThan(const QOrganizerItemEngineLocalId* other) const;

    uint engineLocalIdType() const;
    QOrganizerItemEngineLocalId* clone() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug debugStreamOut(QDebug dbg);
#endif
#ifndef QT_NO_DATASTREAM
    QDataStream& dataStreamOut(QDataStream& out);
    QDataStream& dataStreamIn(QDataStream& in);
#endif
    uint hash() const;

public:
    quint32 m_localItemId; // the maemo5 backend can use a single quint32 to uniquely identify an item in it.
    friend class OrganizerAsynchProcess;
};

inline QOrganizerItemLocalId makeItemLocalId(quint32 id) {
    return QOrganizerItemLocalId(new QOrganizerItemMaemo5EngineLocalId(id));
}

inline quint32 readItemLocalId(const QOrganizerItemLocalId& id) {
    return static_cast<QOrganizerItemMaemo5EngineLocalId*>(
            QOrganizerManagerEngine::engineLocalItemId(id))->m_localItemId;
}

inline QOrganizerCollectionLocalId makeCollectionLocalId(quint32 id) {
    return QOrganizerCollectionLocalId(new QOrganizerCollectionMaemo5EngineLocalId(id));
}

inline quint32 readCollectionLocalId(const QOrganizerCollectionLocalId& id) {
    return static_cast<QOrganizerCollectionMaemo5EngineLocalId*>(
            QOrganizerManagerEngine::engineLocalCollectionId(id))->m_localCollectionId;
}

#endif

