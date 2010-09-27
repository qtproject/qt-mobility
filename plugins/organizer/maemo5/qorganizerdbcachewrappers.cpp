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
    m_event.setGUid(const_cast<CEvent *>(event)->getGUid());
}

OrganizerCacheEvent::OrganizerCacheEvent(const OrganizerCacheEvent& other)
{
    m_event = other.m_event;
    m_event.setGUid(const_cast<OrganizerCacheEvent *>(&other)->m_event.getGUid());
}

OrganizerCacheEvent::~OrganizerCacheEvent()
{
}

OrganizerCacheEvent OrganizerCacheEvent::operator=(const OrganizerCacheEvent& other)
{
    if (&other != this) {
        m_event = other.m_event;
        m_event.setGUid(const_cast<OrganizerCacheEvent *>(&other)->m_event.getGUid());
    }
    return *this;
}

CEvent* OrganizerCacheEvent::event()
{
    CEvent *retn = new CEvent(m_event);
    retn->setGUid(m_event.getGUid());
    return retn;
}

OrganizerCacheTodo::OrganizerCacheTodo(const CTodo* todo)
{
    m_todo = *(const_cast<CTodo *>(todo));
    m_todo.setGUid(const_cast<CTodo *>(todo)->getGUid());
}

OrganizerCacheTodo::OrganizerCacheTodo(const OrganizerCacheTodo& other)
{
    m_todo = const_cast<OrganizerCacheTodo &>(other).m_todo;
    m_todo.setGUid(const_cast<OrganizerCacheTodo *>(&other)->m_todo.getGUid());
}

OrganizerCacheTodo::~OrganizerCacheTodo()
{
}

OrganizerCacheTodo OrganizerCacheTodo::operator=(const OrganizerCacheTodo& other)
{
    if (&other != this) {
        m_todo = const_cast<OrganizerCacheTodo &>(other).m_todo;
        m_todo.setGUid(const_cast<OrganizerCacheTodo *>(&other)->m_todo.getGUid());
    }
    return *this;
}

CTodo* OrganizerCacheTodo::todo()
{
    CTodo *retn = new CTodo(m_todo);
    retn->setGUid(m_todo.getGUid());
    return retn;
}

OrganizerCacheJournal::OrganizerCacheJournal(const CJournal* journal)
{
    m_journal = *(const_cast<CJournal *>(journal));
    m_journal.setGUid(const_cast<CJournal *>(journal)->getGUid());
}

OrganizerCacheJournal::OrganizerCacheJournal(const OrganizerCacheJournal& other)
{
    m_journal = const_cast<OrganizerCacheJournal &>(other).m_journal;
    m_journal.setGUid(const_cast<OrganizerCacheJournal *>(&other)->m_journal.getGUid());
}

OrganizerCacheJournal::~OrganizerCacheJournal()
{
}

OrganizerCacheJournal OrganizerCacheJournal::operator=(const OrganizerCacheJournal& other)
{
    if (&other != this) {
        m_journal = const_cast<OrganizerCacheJournal &>(other).m_journal;
        m_journal.setGUid(const_cast<OrganizerCacheJournal *>(&other)->m_journal.getGUid());
    }
    return *this;
}

CJournal* OrganizerCacheJournal::journal()
{
    CJournal *retn = new CJournal(m_journal);
    retn->setGUid(m_journal.getGUid());
    return retn;
}
