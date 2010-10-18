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
#ifndef QORGANIZERDBCACHE_H
#define QORGANIZERDBCACHE_H

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

#include "qtorganizer.h"
#include "qorganizerdbcachewrappers.h"
#include <QCache>

QTM_USE_NAMESPACE

class OrganizerGuidCacheKey {
public:
    OrganizerGuidCacheKey(int calId, int compType, QString guid)
        : m_calId(calId), m_compType(compType), m_guid(guid) { }

    int m_calId;
    int m_compType;
    QString m_guid;
};

inline bool operator==(const OrganizerGuidCacheKey &k1, const OrganizerGuidCacheKey &k2)
{
    return (k1.m_calId == k2.m_calId && k1.m_compType == k2.m_compType
            && k1.m_guid == k2.m_guid);
}

inline uint qHash(const OrganizerGuidCacheKey &key)
{
    return qHash(key.m_guid); // TODO: Check this
}

class OrganizerIdCacheKey {
public:
    OrganizerIdCacheKey(int calId, QString id)
        : m_calId(calId), m_id(id) { }

    int m_calId;
    QString m_id;
};

inline bool operator==(const OrganizerIdCacheKey &k1, const OrganizerIdCacheKey &k2)
{
    return (k1.m_calId == k2.m_calId && k1.m_id == k2.m_id);
}

inline uint qHash(const OrganizerIdCacheKey &key)
{
    return qHash(key.m_id) ^ qHash(key.m_calId);
}

class OrganizerCalIdTypeIdCacheKey {
public:
    OrganizerCalIdTypeIdCacheKey(int calId, int compType)
        : m_calId(calId), m_compType(compType) { }

    int m_calId;
    int m_compType;
};

inline bool operator==(const OrganizerCalIdTypeIdCacheKey &k1, const OrganizerCalIdTypeIdCacheKey &k2)
{
    return (k1.m_calId == k2.m_calId && k1.m_compType == k2.m_compType);
}

inline uint qHash(const OrganizerCalIdTypeIdCacheKey &key)
{
    return qHash(key.m_calId) ^ qHash(key.m_compType);
}

class OrganizerDbCache : public QObject {
    Q_OBJECT

public:
    OrganizerDbCache();
    ~OrganizerDbCache();

    void insertEvent(const OrganizerIdCacheKey &key, const CEvent* event);
    void insertTodo(const OrganizerIdCacheKey &key, const CTodo* todo);
    void insertJournal(const OrganizerIdCacheKey &key, const CJournal* journal);

    void insertEventVector(const OrganizerGuidCacheKey &key, const std::vector<CEvent *>& eventVector);
    void insertTodoVector(const OrganizerGuidCacheKey &key, const std::vector<CTodo *>& todoVector);
    void insertJournalVector(const OrganizerGuidCacheKey &key, const std::vector<CJournal *>& journalVector);

    void insertCalId(quint32 key, int value);
    void insertTypeId(quint32 key, int value);
    void insertIds(const OrganizerCalIdTypeIdCacheKey &key, const std::vector<std::string>& idsVector);

    bool containsEvent(const OrganizerIdCacheKey &key) const;
    bool containsTodo(const OrganizerIdCacheKey &key) const;
    bool containsJournal(const OrganizerIdCacheKey &key) const;

    bool containsEventVector(const OrganizerGuidCacheKey &key) const;
    bool containsTodoVector(const OrganizerGuidCacheKey &key) const;
    bool containsJournalVector(const OrganizerGuidCacheKey &key) const;

    bool containsCalId(quint32 key) const;
    bool containsTypeId(quint32 key) const;
    bool containsIds(const OrganizerCalIdTypeIdCacheKey &key) const;

    CEvent* takeEvent(const OrganizerIdCacheKey &key) const;
    CTodo* takeTodo(const OrganizerIdCacheKey &key) const;
    CJournal* takeJournal(const OrganizerIdCacheKey &key) const;

    void takeEventVector(const OrganizerGuidCacheKey &key, std::vector<CEvent *>& result) const;
    void takeTodoVector(const OrganizerGuidCacheKey &key, std::vector<CTodo *>& result) const;
    void takeJournalVector(const OrganizerGuidCacheKey &key, std::vector<CJournal *>& result) const;

    int takeCalId(quint32 key) const;
    int takeTypeId(quint32 key) const;
    void takeIdsVector(const OrganizerCalIdTypeIdCacheKey &key, std::vector<std::string>& result) const;

public Q_SLOTS:
    void invalidate();

private:
    QCache<OrganizerIdCacheKey, OrganizerCacheEvent> m_idEventCache;
    QCache<OrganizerIdCacheKey, OrganizerCacheTodo> m_idTodoCache;
    QCache<OrganizerIdCacheKey, OrganizerCacheJournal> m_idJournalCache;

    QCache<OrganizerGuidCacheKey, QList<OrganizerCacheEvent> > m_guidEventCache;
    QCache<OrganizerGuidCacheKey, QList<OrganizerCacheTodo> > m_guidTodoCache;
    QCache<OrganizerGuidCacheKey, QList<OrganizerCacheJournal> > m_guidJournalCache;
    QCache<quint32, int> m_calIdCache;
    QCache<quint32, int> m_typeCache;
    QCache<OrganizerCalIdTypeIdCacheKey, std::vector<std::string> > m_idsCache;
};

#endif // QORGANIZERDBCACHE_H
