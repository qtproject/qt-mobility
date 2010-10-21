/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QORGANIZERMAEMO5IDS_P_H
#define QORGANIZERMAEMO5IDS_P_H

#include "qtorganizer.h"
#include "qorganizeritemengineid.h"
#include "qorganizercollectionengineid.h"

QTM_USE_NAMESPACE

class OrganizerCalendarDatabaseAccess;
class OrganizerItemTransform;     // forward declare maemo5 transform.
class QOrganizerItemMaemo5Engine; // forward declare maemo5 engine.
class OrganizerAsynchProcess;     // forward declare maemo5 asynchronous process.

class QOrganizerCollectionMaemo5EngineId : public QOrganizerCollectionEngineId
{
public:
    QOrganizerCollectionMaemo5EngineId();
    QOrganizerCollectionMaemo5EngineId(quint32 collectionId);
    ~QOrganizerCollectionMaemo5EngineId();
    QOrganizerCollectionMaemo5EngineId(const QOrganizerCollectionMaemo5EngineId& other);

    bool isEqualTo(const QOrganizerCollectionEngineId* other) const;
    bool isLessThan(const QOrganizerCollectionEngineId* other) const;

    QString managerUri() const;
    QOrganizerCollectionEngineId* clone() const;

    QString toString() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const;
#endif
    uint hash() const;

public:
    quint32 m_localCollectionId;
    friend class OrganizerAsynchProcess;
};

class QOrganizerItemMaemo5EngineId : public QOrganizerItemEngineId
{
public:
    QOrganizerItemMaemo5EngineId();
    QOrganizerItemMaemo5EngineId(quint32 itemId);
    ~QOrganizerItemMaemo5EngineId();
    QOrganizerItemMaemo5EngineId(const QOrganizerItemMaemo5EngineId& other);

    bool isEqualTo(const QOrganizerItemEngineId* other) const;
    bool isLessThan(const QOrganizerItemEngineId* other) const;

    QString managerUri() const;
    QOrganizerItemEngineId* clone() const;

    QString toString() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const;
#endif
    uint hash() const;

public:
    quint32 m_localItemId; // the maemo5 backend can use a single quint32 to uniquely identify an item in it.
    friend class OrganizerAsynchProcess;
};

inline QOrganizerItemId makeItemLocalId(quint32 id) {
    return QOrganizerItemId(new QOrganizerItemMaemo5EngineId(id));
}

inline quint32 readItemLocalId(const QOrganizerItemId& id) {
    return static_cast<const QOrganizerItemMaemo5EngineId*>(
            QOrganizerManagerEngine::engineItemId(id))->m_localItemId;
}

inline QOrganizerCollectionId makeCollectionLocalId(quint32 id) {
    return QOrganizerCollectionId(new QOrganizerCollectionMaemo5EngineId(id));
}

inline quint32 readCollectionLocalId(const QOrganizerCollectionId& id) {
    return static_cast<const QOrganizerCollectionMaemo5EngineId*>(
            QOrganizerManagerEngine::engineCollectionId(id))->m_localCollectionId;
}

#endif

