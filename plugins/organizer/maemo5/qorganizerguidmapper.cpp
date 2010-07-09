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

#include "qorganizerguidmapper.h"
#include "qtorganizer.h"

#include <CalendarErrors.h>
#include <CEvent.h>
#include <CJournal.h>
#include <CTodo.h>

QTM_USE_NAMESPACE

OrganizerGuidMapper::OrganizerGuidMapper()
    : m_targetCalendar( 0 ), m_itemsEnumerated(false)
{
}

OrganizerGuidMapper::~OrganizerGuidMapper()
{
}

void OrganizerGuidMapper::setCalendar(CCalendar *targetCalendar)
{
    if (targetCalendar != m_targetCalendar) {
        // Calendar changed
        m_targetCalendar = targetCalendar;
        m_GuidToItemId.clear();
        m_itemIdToItemType.clear();
        m_itemsEnumerated = false;
    }
}

QList<QString> OrganizerGuidMapper::itemIds(const QString &guid, const QString &itemType)
{
    init();
    const QList<QString> correspondingIds = m_GuidToItemId.values(qMakePair(guid, itemType));
    if (!correspondingIds.isEmpty()) {
        return correspondingIds;
    }
    else {
        // have to re-fetch all the items as the asked GUID may have
        // been added by some other application after the last
        // enumerateItems() call

        // TODO: CCalendarDB::commitDB sends a DBus message on commit.
        // Can we use it to observe when the calendar DB has actually
        // changed to avoid worthless iterating over all the items?
        // If possible, change also the hasItem() method!

        m_GuidToItemId.clear();
        m_itemIdToItemType.clear();
        enumerateItems();
        return m_GuidToItemId.values(qMakePair(guid, itemType));
    }
}

QString OrganizerGuidMapper::typeOf(const QString &id)
{
    init();
    QString type = m_itemIdToItemType.value(id);
    if (!type.isEmpty()) {
        return type;
    }
    else {
        // have to re-fetch all the items
        m_GuidToItemId.clear();
        m_itemIdToItemType.clear();
        enumerateItems();
        return m_itemIdToItemType.value(id);
    }
}

void OrganizerGuidMapper::addMapping(const QString &guid, const QString &itemType, const QString &id)
{
    // note: itemType must NOT be any occurrence type
    init();
    if (!m_itemIdToItemType.contains(id)) {
        m_GuidToItemId.insertMulti(qMakePair(guid, itemType), id);
        m_itemIdToItemType.insert(id, itemType);
    }

    /*
    // Old implementation
    init();
    const QList<QString> ids = m_GuidToItemId.values();
    if (ids.indexOf(id) == -1) { // no duplicate ids allowed
        m_GuidToItemId.insertMulti(qMakePair(guid, itemType), id);
        m_itemIdToItemType.insert(id, itemType);
    }
    */



    /* // ONLY FOR DEBUGGING THE MAPPING, REMOVE LATER
    QMultiMap<QString, QString>::iterator i = m_GuidToItemId.begin();
    while( i != m_GuidToItemId.end() ) {
        QString k = i.key();
        QString v = i.value();
        qDebug() << k << " = " << v;
        ++i;
    }
    */
}

void OrganizerGuidMapper::init()
{
    if (!m_itemsEnumerated)
        enumerateItems();
}

void OrganizerGuidMapper::enumerateItems()
{
    if (!m_targetCalendar)
        return;

    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    // Events
    const QString eventType = QOrganizerItemType::TypeEvent;
    const std::vector< CEvent* > events = m_targetCalendar->getEvents(calError);
    std::vector< CEvent* >::const_iterator event;
    for (event = events.begin(); event != events.end(); ++event)
    {
        CEvent *cevent = *event;
        if (cevent) {
            QString guid = QString::fromStdString(cevent->getGUid());
            if (!guid.isEmpty())
                m_GuidToItemId.insertMulti(qMakePair(guid, eventType), QString::fromStdString(cevent->getId()));

            QString id = QString::fromStdString(cevent->getId());
            if (!id.isEmpty())
                m_itemIdToItemType.insert(id, eventType);

            delete cevent;
        }
    }

    // Todos
    const QString todoType = QOrganizerItemType::TypeTodo;
    const std::vector< CTodo* > todos = m_targetCalendar->getTodos(calError);
    std::vector< CTodo* >::const_iterator todo;
    for (todo = todos.begin(); todo != todos.end(); ++todo)
    {
        CTodo *ctodo = *todo;
        if (ctodo) {
            QString guid = QString::fromStdString(ctodo->getGUid());
            if (!guid.isEmpty())
                m_GuidToItemId.insertMulti(qMakePair(guid, todoType), QString::fromStdString(ctodo->getId()));

            QString id = QString::fromStdString(ctodo->getId());
            if (!id.isEmpty())
                m_itemIdToItemType.insert(id, todoType);

            delete ctodo;
        }
    }

    // Journals
    const QString journalType = QOrganizerItemType::TypeJournal;
    const std::vector< CJournal* > journals = m_targetCalendar->getJournals(calError);
    std::vector< CJournal* >::const_iterator journal;
    for (journal = journals.begin(); journal != journals.end(); ++journal)
    {
        CJournal *cjournal = *journal;
        if (cjournal) {
            QString guid = QString::fromStdString(cjournal->getGUid());
            if (!guid.isEmpty())
                m_GuidToItemId.insertMulti(qMakePair(guid, journalType), QString::fromStdString(cjournal->getId()));

            QString id = QString::fromStdString(cjournal->getId());
            if (!id.isEmpty())
                m_itemIdToItemType.insert(id, journalType);

            delete cjournal;
        }
    }

    m_itemsEnumerated = true;
}
