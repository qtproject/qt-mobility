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

#include "qorganizerdbcachewrappers.h"
#include "qtorganizer.h"

QTM_USE_NAMESPACE

OrganizerCacheEvent::OrganizerCacheEvent(const CEvent* event)
{
    m_event = *event;
    CEvent* castEvent = const_cast<CEvent *>(event);
    m_event.setGUid(castEvent->getGUid());
    m_event.setCreatedTime(castEvent->getCreatedTime());
    m_event.setLastModified(castEvent->getLastModified());
}

OrganizerCacheEvent::OrganizerCacheEvent(const OrganizerCacheEvent& other)
{
    m_event = other.m_event;
    OrganizerCacheEvent *castOther = const_cast<OrganizerCacheEvent *>(&other);
    m_event.setGUid(castOther->m_event.getGUid());
    m_event.setCreatedTime(castOther->m_event.getCreatedTime());
    m_event.setLastModified(castOther->m_event.getLastModified());
}

OrganizerCacheEvent::~OrganizerCacheEvent()
{
}

OrganizerCacheEvent OrganizerCacheEvent::operator=(const OrganizerCacheEvent& other)
{
    if (&other != this) {
        m_event = other.m_event;
        OrganizerCacheEvent *castOther = const_cast<OrganizerCacheEvent *>(&other);
        m_event.setGUid(castOther->m_event.getGUid());
        m_event.setCreatedTime(castOther->m_event.getCreatedTime());
        m_event.setLastModified(castOther->m_event.getLastModified());
    }
    return *this;
}

CEvent* OrganizerCacheEvent::event()
{
    CEvent *retn = new CEvent(m_event);
    retn->setGUid(m_event.getGUid());
    retn->setCreatedTime(m_event.getCreatedTime());
    retn->setLastModified(m_event.getLastModified());
    return retn;
}

OrganizerCacheTodo::OrganizerCacheTodo(const CTodo* todo)
{
    CTodo *castTodo = const_cast<CTodo *>(todo);
    m_todo = *castTodo;
    m_todo.setGUid(castTodo->getGUid());
    m_todo.setCreatedTime(castTodo->getCreatedTime());
    m_todo.setLastModified(castTodo->getLastModified());
}

OrganizerCacheTodo::OrganizerCacheTodo(const OrganizerCacheTodo& other)
{
    m_todo = const_cast<OrganizerCacheTodo &>(other).m_todo;
    OrganizerCacheTodo *castOther = const_cast<OrganizerCacheTodo *>(&other);
    m_todo.setGUid(castOther->m_todo.getGUid());
    m_todo.setCreatedTime(castOther->m_todo.getCreatedTime());
    m_todo.setLastModified(castOther->m_todo.getLastModified());
}

OrganizerCacheTodo::~OrganizerCacheTodo()
{
}

OrganizerCacheTodo OrganizerCacheTodo::operator=(const OrganizerCacheTodo& other)
{
    if (&other != this) {
        m_todo = const_cast<OrganizerCacheTodo &>(other).m_todo;
        OrganizerCacheTodo *castOther = const_cast<OrganizerCacheTodo *>(&other);
        m_todo.setGUid(castOther->m_todo.getGUid());
        m_todo.setCreatedTime(castOther->m_todo.getCreatedTime());
        m_todo.setLastModified(castOther->m_todo.getLastModified());
    }
    return *this;
}

CTodo* OrganizerCacheTodo::todo()
{
    CTodo *retn = new CTodo(m_todo);
    retn->setGUid(m_todo.getGUid());
    retn->setCreatedTime(m_todo.getCreatedTime());
    retn->setLastModified(m_todo.getLastModified());
    return retn;
}

OrganizerCacheJournal::OrganizerCacheJournal(const CJournal* journal)
{
    CJournal *castJournal = const_cast<CJournal *>(journal);
    m_journal = *castJournal;
    m_journal.setGUid(castJournal->getGUid());
    m_journal.setCreatedTime(castJournal->getCreatedTime());
    m_journal.setLastModified(castJournal->getLastModified());
}

OrganizerCacheJournal::OrganizerCacheJournal(const OrganizerCacheJournal& other)
{
    m_journal = const_cast<OrganizerCacheJournal &>(other).m_journal;
    OrganizerCacheJournal *castOther = const_cast<OrganizerCacheJournal *>(&other);
    m_journal.setGUid(castOther->m_journal.getGUid());
    m_journal.setCreatedTime(castOther->m_journal.getCreatedTime());
    m_journal.setLastModified(castOther->m_journal.getLastModified());
}

OrganizerCacheJournal::~OrganizerCacheJournal()
{
}

OrganizerCacheJournal OrganizerCacheJournal::operator=(const OrganizerCacheJournal& other)
{
    if (&other != this) {
        m_journal = const_cast<OrganizerCacheJournal &>(other).m_journal;
        OrganizerCacheJournal *castOther = const_cast<OrganizerCacheJournal *>(&other);
        m_journal.setGUid(castOther->m_journal.getGUid());
        m_journal.setCreatedTime(castOther->m_journal.getCreatedTime());
        m_journal.setLastModified(castOther->m_journal.getLastModified());
    }
    return *this;
}

CJournal* OrganizerCacheJournal::journal()
{
    CJournal *retn = new CJournal(m_journal);
    retn->setGUid(m_journal.getGUid());
    retn->setCreatedTime(m_journal.getCreatedTime());
    retn->setLastModified(m_journal.getLastModified());
    return retn;
}
