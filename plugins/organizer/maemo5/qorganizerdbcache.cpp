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

#include "qorganizerdbcache.h"
#include "qtorganizer.h"

QTM_USE_NAMESPACE

OrganizerDbCache::OrganizerDbCache()
{
    m_guidEventCache.setMaxCost(1000); // TODO: Adjust the costs
    m_guidTodoCache.setMaxCost(1000); // TODO: Adjust the costs
    m_guidJournalCache.setMaxCost(1000); // TODO: Adjust the costs
    m_idEventCache.setMaxCost(1000); // TODO: Adjust the costs
    m_calIdCache.setMaxCost(1000); // TODO: Adjust the costs
    m_typeCache.setMaxCost(1000); // TODO: Adjust the costs
    m_idsCache.setMaxCost(1000); // TODO: Adjust the costs
}

OrganizerDbCache::~OrganizerDbCache()
{
    invalidate();
}

void OrganizerDbCache::invalidate()
{
    //qDebug() << "invalidate";
    m_guidEventCache.clear();
    m_guidTodoCache.clear();
    m_guidJournalCache.clear();
    m_idEventCache.clear();
    m_calIdCache.clear();
    m_typeCache.clear();
    m_idsCache.clear();
}

void OrganizerDbCache::insertEvent(const OrganizerIdCacheKey &key, const CEvent* event)
{
    if (event) {
        OrganizerCacheEvent* cacheEvent = new OrganizerCacheEvent(event);
        m_idEventCache.insert(key, cacheEvent);
    }
}

void OrganizerDbCache::insertEventVector(const OrganizerGuidCacheKey &key, const std::vector<CEvent *>& eventVector)
{
    QList<OrganizerCacheEvent>* cacheList = new QList<OrganizerCacheEvent>();
    std::vector<CEvent *>::const_iterator event;
    for(event = eventVector.begin(); event != eventVector.end(); ++event) {
        CEvent *eventPtr = *event;
        cacheList->append(OrganizerCacheEvent(eventPtr));
    }
    m_guidEventCache.insert(key, cacheList);
}

void OrganizerDbCache::insertTodoVector(const OrganizerGuidCacheKey &key, const std::vector<CTodo *>& todoVector)
{
    QList<OrganizerCacheTodo>* cacheList = new QList<OrganizerCacheTodo>();
    std::vector<CTodo *>::const_iterator todo;
    for(todo = todoVector.begin(); todo != todoVector.end(); ++todo) {
        CTodo *todoPtr = *todo;
        cacheList->append(OrganizerCacheTodo(todoPtr));
    }
    m_guidTodoCache.insert(key, cacheList);
}

void OrganizerDbCache::insertJournalVector(const OrganizerGuidCacheKey &key, const std::vector<CJournal *>& journalVector)
{
    QList<OrganizerCacheJournal>* cacheList = new QList<OrganizerCacheJournal>();
    std::vector<CJournal *>::const_iterator journal;
    for(journal = journalVector.begin(); journal != journalVector.end(); ++journal) {
        CJournal *journalPtr = *journal;
        cacheList->append(OrganizerCacheJournal(journalPtr));
    }
    m_guidJournalCache.insert(key, cacheList);
}

void OrganizerDbCache::insertCalId(QOrganizerItemLocalId key, int value)
{
    int *cacheInt = new int(value);
    m_calIdCache.insert(key, cacheInt);
}

void OrganizerDbCache::insertTypeId(QOrganizerItemLocalId key, int value)
{
    int *cacheInt = new int(value);
    m_typeCache.insert(key, cacheInt);
}

void OrganizerDbCache::insertIds(const OrganizerCalIdTypeIdCacheKey &key, const std::vector<std::string> &idsVector)
{
    std::vector<std::string>* cacheVector = new std::vector<std::string>(idsVector);
    m_idsCache.insert(key, cacheVector);
}

bool OrganizerDbCache::containsEvent(const OrganizerIdCacheKey &key) const
{
    return m_idEventCache.contains(key);
}

bool OrganizerDbCache::containsEventVector(const OrganizerGuidCacheKey &key) const
{
    return m_guidEventCache.contains(key);
}

bool OrganizerDbCache::containsTodoVector(const OrganizerGuidCacheKey &key) const
{
    return m_guidTodoCache.contains(key);
}

bool OrganizerDbCache::containsJournalVector(const OrganizerGuidCacheKey &key) const
{
    return m_guidJournalCache.contains(key);
}

bool OrganizerDbCache::containsCalId(QOrganizerItemLocalId key) const
{
    return m_calIdCache.contains(key);
}

bool OrganizerDbCache::containsTypeId(QOrganizerItemLocalId key) const
{
    return m_typeCache.contains(key);
}

bool OrganizerDbCache::containsIds(const OrganizerCalIdTypeIdCacheKey &key) const
{
    return m_idsCache.contains(key);
}

CEvent* OrganizerDbCache::takeEvent(const OrganizerIdCacheKey &key) const
{
    OrganizerCacheEvent* cacheEvent = m_idEventCache.object(key);
    return cacheEvent->event();
}

void OrganizerDbCache::takeEventVector(const OrganizerGuidCacheKey &key, std::vector<CEvent *>& result) const
{
    QList<OrganizerCacheEvent>* cacheList = m_guidEventCache.object(key);
    if (!cacheList)
        return;

    result.clear();
    foreach (OrganizerCacheEvent event, *cacheList)
        result.push_back(event.event());
}

void OrganizerDbCache::takeTodoVector(const OrganizerGuidCacheKey &key, std::vector<CTodo *>& result) const
{
    QList<OrganizerCacheTodo>* cacheList = m_guidTodoCache.object(key);
    if (!cacheList)
        return;

    result.clear();
    foreach (OrganizerCacheTodo todo, *cacheList)
        result.push_back(todo.todo());
}

void OrganizerDbCache::takeJournalVector(const OrganizerGuidCacheKey &key, std::vector<CJournal *>& result) const
{
    QList<OrganizerCacheJournal>* cacheList = m_guidJournalCache.object(key);
    if (!cacheList)
        return;

    result.clear();
    foreach (OrganizerCacheJournal journal, *cacheList)
        result.push_back(journal.journal());
}

int OrganizerDbCache::takeCalId(QOrganizerItemLocalId key) const
{
    int *cacheInt = m_calIdCache.object(key);
    return *cacheInt;
}

int OrganizerDbCache::takeTypeId(QOrganizerItemLocalId key) const
{
    int *cacheInt = m_typeCache.object(key);
    return *cacheInt;
}

void OrganizerDbCache::takeIdsVector(const OrganizerCalIdTypeIdCacheKey &key, std::vector<std::string>& result) const
{
    std::vector<std::string>* cacheVector = m_idsCache.object(key);
    result = *cacheVector;
}
