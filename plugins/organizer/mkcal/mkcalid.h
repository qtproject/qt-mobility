/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MKCALID_H
#define MKCALID_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qorganizeritemengineid.h"

QTM_USE_NAMESPACE

class MKCalEngineFactory; // forward decl. for friend decl.

class MKCalItemId : public QOrganizerItemEngineId
{
public:
    MKCalItemId() {}
    MKCalItemId(const QString& id, const KDateTime& rid) : m_id(id), m_rid(rid), m_hash(0) {}

    bool isEqualTo(const QOrganizerItemEngineId* other) const
    {
        const MKCalItemId* otherid = static_cast<const MKCalItemId*>(other);
        if (hash() != otherid->hash())
            return false;
        if (m_id != otherid->m_id)
            return false;
        if (m_rid != otherid->m_rid)
            return false;
        return true;
    }
    bool isLessThan(const QOrganizerItemEngineId* other) const
    {
        const MKCalItemId* otherid = static_cast<const MKCalItemId*>(other);
        if (m_id < otherid->m_id)
            return true;
        if (m_rid < otherid->m_rid)
            return true;
        return false;
    }
    QOrganizerItemEngineId* clone() const
    {
        return new MKCalItemId(m_id, m_rid, m_hash);
    }
    QString managerUri() const
    {
        static QString uri(QLatin1String("qtorganizer:mkcal:"));
        return uri;
    }
#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        return dbg << m_id << m_rid.dateTime() << managerUri();
    }
#endif
    uint hash() const
    {
        if (!m_hash)
            m_hash = (qHash(m_id) << 4) + m_rid.toTime_t();
        return m_hash;
    }

    QString toString() const;

    QString id() const
    {
        return m_id;
    }
    KDateTime recurrenceID() const
    {
        return m_rid;
    }

    static const MKCalItemId* id_cast(const QOrganizerItemId& itemId)
    {
        static MKCalItemId empty;
        if (itemId.isNull() || itemId.managerUri() != empty.managerUri())
            return &empty;
        return static_cast<const MKCalItemId *>(QOrganizerManagerEngine::engineItemId(itemId));
    }

private:
    QString m_id;
    KDateTime m_rid;
    mutable int m_hash;

    MKCalItemId(const QString& id, const KDateTime& rid, int hash) : m_id(id), m_rid(rid), m_hash(hash) {}
};


class MKCalCollectionId : public QOrganizerCollectionEngineId
{
public:
    MKCalCollectionId() {}
    MKCalCollectionId(const QString& uid) : m_uid(uid), m_hash(0) {}
    bool isEqualTo(const QOrganizerCollectionEngineId* other) const
    {
        const MKCalCollectionId* otherid = static_cast<const MKCalCollectionId*>(other);
        if (hash() != otherid->hash())
            return false;
        if (m_uid != otherid->m_uid)
            return false;
        return true;
    }
    bool isLessThan(const QOrganizerCollectionEngineId* other) const
    {
        const MKCalCollectionId* otherid = static_cast<const MKCalCollectionId*>(other);
        if (managerUri() < otherid->managerUri())
            return true;
        if (m_uid < otherid->m_uid)
            return true;
        return false;
    }
    QOrganizerCollectionEngineId* clone() const
    {
        return new MKCalCollectionId(m_uid, m_hash);
    }
    QString managerUri() const
    {
        static QString uri(QLatin1String("qtorganizer:mkcal:"));
        return uri;
    }
#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const
    {
        return dbg << m_uid << ":" << managerUri();
    }
#endif
    uint hash() const
    {
        if (!m_hash)
            m_hash = qHash(m_uid);
        return m_hash;
    }

    QString toString() const;

    static const MKCalCollectionId* id_cast(const QOrganizerCollectionId& collId)
    {
        static MKCalCollectionId empty;
        if (collId.isNull() || collId.managerUri() != empty.managerUri())
            return &empty;
        return static_cast<const MKCalCollectionId *>(QOrganizerManagerEngine::engineCollectionId(collId));
    }

    QString uid() const
    {
        return m_uid;
    }

private:
    QString m_uid;
    mutable int m_hash;

    MKCalCollectionId(const QString& uid, int hash) : m_uid(uid), m_hash(hash) {}
};

#endif
