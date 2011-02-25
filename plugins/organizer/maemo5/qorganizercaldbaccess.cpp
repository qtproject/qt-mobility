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

#include "qorganizercaldbaccess.h"
#include "qorganizerdbcache.h"
#include "qorganizermaemo5ids_p.h"
#include "qtorganizer.h"

#include <CalendarErrors.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>
#include <CCalendar.h>
#include <CAlarm.h>
#include <CRecurrence.h>
#include <CAttendee.h>
#include <COrganizer.h>
#include <CProperties.h>

/**
 * entryType enum
 * This enum is from the Maemo5 calendar backend file Common.h
 * The enumeration is pasted here to avoid including Common.h file,
 * which contains dependencies to D-Bus handling and other components
 * that are not actually needed here.
*/
enum entryType {
    E_CALENDAR = 0,
    E_EVENT,
    E_TODO,
    E_JOURNAL,
    E_BDAY,
    E_SPARE,
    E_VTIMEZONE
};

/**
 * constants from the calendar backend
 * These constants are pasted here to avoid including Common.h file,
 * which contains dependencies to D-Bus handling and other components
 * that are not actually needed here.
*/
const int HAS_RECURRENCE = 3;
const int HAS_ALARM = 4;
const int HAS_RECURRENCE_ALARM = 5;

QTM_USE_NAMESPACE

const QString selectComponentCalIdType = QString("select CalendarId, ComponentType from Components where Id = :compId");

const QString selectInnerJoinBatchGuid = QString("select * from components left join componentdetails on components.Id = componentdetails.Id Left Join alarm on components.Id = alarm.Id Left Join Recursive on components.Id = Recursive.Id where Calendarid = :calId AND Components.ComponentType = :compType AND Components.Uid = :compUid");

const QString cookieFixer = QString("update ALARM set CookieId = :newCookie where CookieId = :oldCookie");

OrganizerCalendarDatabaseAccess::OrganizerCalendarDatabaseAccess(OrganizerDbCache* dbCache) : m_dbCache(dbCache)
{
}

OrganizerCalendarDatabaseAccess::~OrganizerCalendarDatabaseAccess()
{
    close();
}

bool OrganizerCalendarDatabaseAccess::open(QString databasePathName)
{
    if (!m_db.isOpen()) {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setHostName("localhost");
        m_db.setDatabaseName(databasePathName);
        bool ok = m_db.open();
        return ok;
    }
    else {
        // database is already open
        return true;
    }
}

void OrganizerCalendarDatabaseAccess::close()
{
    m_db.close();
}

int OrganizerCalendarDatabaseAccess::calIdOf(QOrganizerItemId id)
{
    quint32 convertedId = readItemLocalId(id);
    if (m_dbCache->containsCalId(convertedId)) {
        return m_dbCache->takeCalId(convertedId);
    }
    else {
        QSqlQuery query;
        if (!query.prepare(selectComponentCalIdType))
            return -1;
        query.bindValue(":compId", QString::number(convertedId));

        int retn = -1;
        if (query.exec()) {
            if (query.next())
                retn = query.value(0).toInt();
        }

        m_dbCache->insertCalId(convertedId, retn);
        return retn;
    }
}

int OrganizerCalendarDatabaseAccess::typeOf(QOrganizerItemId id)
{
    quint32 convertedId = readItemLocalId(id);
    if (m_dbCache->containsTypeId(convertedId)) {
        return m_dbCache->takeTypeId(convertedId);
    }
    else {
        QSqlQuery query;
        if (!query.prepare(selectComponentCalIdType))
            return -1;
        query.bindValue(":compId", QString::number(convertedId));

        int retn = -1;
        if (query.exec()) {
            if (query.next())
                retn = query.value(1).toInt();
        }

        m_dbCache->insertTypeId(convertedId, retn);
        return retn;
    }
}

std::vector<CEvent *> OrganizerCalendarDatabaseAccess::getEvents(int calId, std::string guid, int &pErrorCode)
{
    std::vector<CEvent*> listEvent;

    OrganizerGuidCacheKey cacheKey(calId, E_EVENT, QString::fromStdString(guid));
    if (m_dbCache->containsEventVector(cacheKey))
    {
        // found in cache
        m_dbCache->takeEventVector(cacheKey, listEvent);
        return listEvent;
    }

    const int columnNumber = 49;
    CEvent *event = 0;
    CAlarm *pAlarm = 0;
    CRecurrence *pRec = 0;
    int iI_EventCount = 0;
    int iJ_EventCount = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    std::vector<long> vCookie;
    std::vector<std::string> vERule;
    std::vector<std::string> vEdate;
    std::vector<std::string> vRdate;
    std::vector<std::string> vRRule;

    QSqlQuery pQuery;
    if (!pQuery.prepare(selectInnerJoinBatchGuid)) {
        pErrorCode = CALENDAR_DATABASE_ERROR;
        return listEvent;
    }
    pQuery.bindValue(":calId", QString::number(calId));
    pQuery.bindValue(":compType", QString::number(E_EVENT));
    pQuery.bindValue(":compUid", QString::fromStdString(guid));

    bool ok = pQuery.exec();

    sqliteErrorMapper(pQuery.lastError(), pErrorCode);
    if (!ok)
        return listEvent;

    while (pQuery.next()) {
        event = new CEvent();
        pAlarm = new CAlarm();
        pRec = new CRecurrence();
        for (iJ_EventCount = 0; iJ_EventCount < columnNumber; ++iJ_EventCount) {
            switch(iJ_EventCount) {
            case 0: // ID1
                event->setId(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 1:
                break;

            case 2: // ID3
                event->setType(pQuery.value(iJ_EventCount).toInt());
                break;

            case 3: // ID4
                event->setFlags(pQuery.value(iJ_EventCount).toInt());
                break;

            case 4: // ID5
                event->setDateStart(pQuery.value(iJ_EventCount).toInt());
                break;

            case 5: // ID6
                event->setDateEnd(pQuery.value(iJ_EventCount).toInt());
                break;

            case 6: // ID7
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setSummary(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 7: // ID8
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setLocation(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 8: // ID9
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setDescription(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 9: // ID10
                event->setStatus(pQuery.value(iJ_EventCount).toInt());
                break;

            case 10: // ID11
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setGUid(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 11: // ID12
                event->setUntil(pQuery.value(iJ_EventCount).toInt());
                break;

            case 12: // ID13
                event->setAllDay(pQuery.value(iJ_EventCount).toInt());
                break;

            case 13: // ID14
                event->setCreatedTime(pQuery.value(iJ_EventCount).toInt());
                break;

            case 14: // ID15
                event->setLastModified(pQuery.value(iJ_EventCount).toInt());
                break;

            case 15: // ID16
                event->setTzid(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 16: // ID17
            case 17: // ID18
            case 18: // ID19
                break;

            case 19: // ID20
                if (pQuery.value(iJ_EventCount).toInt())
                    event->setClas(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 20: // ID21
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setGeo(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 21: // ID22
                event->setPriority(pQuery.value(iJ_EventCount).toInt());
                break;

            case 22: // ID23
                event->setDateStamp(pQuery.value(iJ_EventCount).toInt());
                break;

            case 23: // ID24
                event->setSequence(pQuery.value(iJ_EventCount).toInt());
                break;

            case 24: // ID25
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setTransparency(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 25: // ID26
                event->setUid(pQuery.value(iJ_EventCount).toInt());
                break;

            case 26: // ID27
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setUrl(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 27: // ID28
                if (!pQuery.value(iJ_EventCount).toString().isEmpty()) {
                    std::vector<std::string> vAtt;
                    vAtt.push_back(pQuery.value(iJ_EventCount).toString().toStdString());
                    event->setAttachments(vAtt);
                }
                break;

            case 28: // ID29
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setCategories(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 29: // ID30
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setComments(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 30: // ID31
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setContact(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 31: // ID32
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setRelated(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 32: // ID33
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    event->setResources(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 33: // ID34
            case 34: // ID35
            case 35: // ID36
                break;

            case 36: // ID37
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    pAlarm->setTrigger(pQuery.value(iJ_EventCount).toInt());
                break;

            case 37: // ID38
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    pAlarm->setRepeat(pQuery.value(iJ_EventCount).toInt());
                break;

            case 38: // ID39
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    pAlarm->setDuration(pQuery.value(iJ_EventCount).toInt());
                break;

            case 39: // ID40
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    pAlarm->setAction(pQuery.value(iJ_EventCount).toInt());
                break;

            case 40: // ID41
                if (!pQuery.value(iJ_EventCount).toString().isEmpty()) {
                    vCookie.push_back(pQuery.value(iJ_EventCount).toInt());
                    pAlarm->setCookie(vCookie);
                }
                break;

            case 41: // ID42
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    pAlarm->setAttach(pQuery.value(iJ_EventCount).toString().toStdString());
                break;

            case 42: // ID43
                break;

            case 43: // ID44
                if (!pQuery.value(iJ_EventCount).toString().isEmpty()) {
                    vRRule = event->extractSubStrings(pQuery.value(iJ_EventCount).toString().toStdString(), EXCLAMATION);
                    pRec->setRrule(vRRule);
                }
                break;

            case 44: // ID45
                if (!pQuery.value(iJ_EventCount).toString().isEmpty()) {
                    vRdate = event->extractSubStrings(pQuery.value(iJ_EventCount).toString().toStdString(), SEMI_COLON);
                    pRec->setRDays(vRdate);
                }
                break;

            case 45: // ID46
                if (!pQuery.value(iJ_EventCount).toString().isEmpty()) {
                    vEdate = event->extractSubStrings(pQuery.value(iJ_EventCount).toString().toStdString(), SEMI_COLON);
                    pRec->setEDays(vEdate);
                }
                break;

            case 46: // ID47
                if (!pQuery.value(iJ_EventCount).toString().isEmpty()) {
                    vERule = event->extractSubStrings(pQuery.value(iJ_EventCount).toString().toStdString(), EXCLAMATION);
                    pRec->setErule(vERule);
                }
                break;

            case 47: // ID48
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    pRec->setRecurId(pQuery.value(iJ_EventCount).toInt());
                break;

            case 48: // ID49
                if (!pQuery.value(iJ_EventCount).toString().isEmpty())
                    pRec->setRtype(pQuery.value(iJ_EventCount).toInt());
                break;
            }
        }

        if ((event->getFlags() == HAS_RECURRENCE) ||
            (event->getFlags() == HAS_RECURRENCE_ALARM)) {
            event->setRecurrence(pRec);
        }
        delete pRec;
        pRec = 0;

        if ((event->getFlags() == HAS_ALARM) ||
            (event->getFlags() == HAS_RECURRENCE_ALARM)) {
            event->setAlarm(pAlarm);
        }
        delete pAlarm;
        pAlarm = 0;

        std::vector<CAttendee *> listAttendee;

        listAttendee = event->retrieveAttendeeDetails();
        COrganizer *pOrg = 0;

        pOrg = event->retrieveOrganizerDetails();
        if (listAttendee.size())
            event->setAttendees(listAttendee);

        std::vector<CAttendee *>::iterator listAttendeeIterator;
        for (listAttendeeIterator = listAttendee.begin(); listAttendeeIterator != listAttendee.end(); ++listAttendeeIterator)
            delete *listAttendeeIterator;

        if (pOrg) {
            event->setOrganizer(pOrg);
            delete pOrg;
            pOrg = 0;
        }

        /*retrieve xprop */
        std::vector<CProperties *> vPropList;
        vPropList = event->retrieveXPropertyDetails();
        event->setXProperties(vPropList);

        std::vector<CProperties *>::iterator vPropListIterator;
        for (vPropListIterator = vPropList.begin(); vPropListIterator != vPropList.end(); ++vPropListIterator)
            delete *vPropListIterator;

        /*retrieve params */
        std::map<std::string, std::vector<CParameters *> > paramMap;
        paramMap = event->retrieveParameterDetails();

        event->setHashMap(paramMap);
        paramMap.clear();

        /* push the event in to the list */
        listEvent.push_back(event);

        ++iI_EventCount;
    }

    // put to cache
    m_dbCache->insertEventVector(cacheKey, listEvent);

    return listEvent;
}

std::vector<CTodo *> OrganizerCalendarDatabaseAccess::getTodos(int calId, std::string guid, int &pErrorCode)
{
    std::vector<CTodo*> listTodo;

    OrganizerGuidCacheKey cacheKey(calId, E_TODO, QString::fromStdString(guid));
    if (m_dbCache->containsTodoVector(cacheKey))
    {
        // found in cache
        m_dbCache->takeTodoVector(cacheKey, listTodo);
        return listTodo;
    }

    const int columnNumber = 49;
    CTodo *todo = 0;
    CAlarm *pAlarm = 0;
    int iI_TodoCount = 0;
    int iJ_TodoCount = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    std::vector<long> vCookie;

    QSqlQuery pQuery;
    if (!pQuery.prepare(selectInnerJoinBatchGuid)) {
        pErrorCode = CALENDAR_DATABASE_ERROR;
        return listTodo;
    }
    pQuery.bindValue(":calId", QString::number(calId));
    pQuery.bindValue(":compType", QString::number(E_TODO));
    pQuery.bindValue(":compUid", QString::fromStdString(guid));
    bool ok = pQuery.exec();

    sqliteErrorMapper(pQuery.lastError(), pErrorCode);
    if (!ok)
        return listTodo;

    while (pQuery.next()) {
        todo = new CTodo();
        pAlarm = new CAlarm();
        for (iJ_TodoCount = 0; iJ_TodoCount < columnNumber; ++iJ_TodoCount) {
            switch(iJ_TodoCount) {
            case 0: // ID1
                todo->setId(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 1:
                break;

            case 2: // ID3
                todo->setType(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 3: // ID4
                todo->setFlags(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 4: // ID5
                todo->setDateStart(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 5: // ID6
                todo->setDateEnd(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 6: // ID7
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setSummary(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 7: // ID8
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setLocation(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 8: // ID9
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setDescription(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 9: // ID10
                todo->setStatus(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 10: // ID11
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setGUid(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 11: // ID12
                todo->setUntil(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 12: // ID13
                todo->setAllDay(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 13: // ID14
                todo->setCreatedTime(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 14: // ID15
                todo->setLastModified(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 15: // ID16
                todo->setTzid(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 16: // ID17
            case 17: // ID18
            case 18: // ID19
                break;

            case 19: // ID20
                if (pQuery.value(iJ_TodoCount).toInt())
                    todo->setClas(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 20: // ID21
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setGeo(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 21: // ID22
                todo->setPriority(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 22: // ID23
                todo->setDateStamp(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 23: // ID24
                todo->setSequence(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 24: // ID25
                break;

            case 25: // ID26
                todo->setUid(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 26: // ID27
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setUrl(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 27: // ID28
                break;

            case 28: // ID29
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setCategories(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 29: // ID30
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setComments(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 30: // ID31
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setContact(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 31: // ID32
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setRelated(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 32: // ID33
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    todo->setResources(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            case 33: // ID34
                todo->setPercentComplete(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 34: // ID35
                todo->setCompleted(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 35: // ID36
                break;

            case 36: // ID37
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    pAlarm->setTrigger(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 37: // ID38
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    pAlarm->setRepeat(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 38: // ID39
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    pAlarm->setDuration(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 39: // ID40
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    pAlarm->setAction(pQuery.value(iJ_TodoCount).toInt());
                break;

            case 40: // ID41
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty()) {
                    vCookie.push_back(pQuery.value(iJ_TodoCount).toInt());
                    pAlarm->setCookie(vCookie);
                }
                break;

            case 41: // ID42
                if (!pQuery.value(iJ_TodoCount).toString().isEmpty())
                    pAlarm->setAttach(pQuery.value(iJ_TodoCount).toString().toStdString());
                break;

            default:
                break;
            }
        }

        if (todo->getFlags() == HAS_ALARM)
            todo->setAlarm(pAlarm);

        delete pAlarm;
        pAlarm = 0;

        /*retrieve xprop */
        std::vector<CProperties *> vPropList;
        vPropList = todo->retrieveXPropertyDetails();
        todo->setXProperties(vPropList);

        std::vector<CProperties *>::iterator vPropListIterator;
        for (vPropListIterator = vPropList.begin(); vPropListIterator != vPropList.end(); ++vPropListIterator)
            delete *vPropListIterator;

        /*retrieve params */
        std::map<std::string, std::vector<CParameters *> > paramMap;
        paramMap = todo->retrieveParameterDetails();
        todo->setHashMap(paramMap);
        paramMap.clear();

        /* push the todo in to the list */
        listTodo.push_back(todo);

        ++iI_TodoCount;
    }

    // put to cache
    m_dbCache->insertTodoVector(cacheKey, listTodo);

    return listTodo;
}

std::vector<CJournal *> OrganizerCalendarDatabaseAccess::getJournals(int calId, std::string guid, int &pErrorCode)
{
    std::vector<CJournal*> listJournal;

    OrganizerGuidCacheKey cacheKey(calId, E_JOURNAL, QString::fromStdString(guid));
    if (m_dbCache->containsJournalVector(cacheKey))
    {
        // found in cache
        m_dbCache->takeJournalVector(cacheKey, listJournal);
        return listJournal;
    }

    const int columnNumber = 49;
    CJournal *journal = 0;
    int iI_JourCount = 0;
    int iJ_JourCount = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;

    QSqlQuery pQuery;
    if (!pQuery.prepare(selectInnerJoinBatchGuid)) {
        pErrorCode = CALENDAR_DATABASE_ERROR;
        return listJournal;
    }
    pQuery.bindValue(":calId", QString::number(calId));
    pQuery.bindValue(":compType", QString::number(E_JOURNAL));
    pQuery.bindValue(":compUid", QString::fromStdString(guid));
    bool ok = pQuery.exec();

    sqliteErrorMapper(pQuery.lastError(), pErrorCode);
    if (!ok)
        return listJournal;

    while (pQuery.next()) {
        journal = new CJournal();
        for (iJ_JourCount = 0; iJ_JourCount < columnNumber; ++iJ_JourCount) {
            switch(iJ_JourCount) {
            case 0: // ID1
                journal->setId(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 1:
                break;

            case 2: // ID3
                journal->setType(pQuery.value(iJ_JourCount).toInt());
                break;

            case 3: // ID4
                journal->setFlags(pQuery.value(iJ_JourCount).toInt());
                break;

            case 4: // ID5
                journal->setDateStart(pQuery.value(iJ_JourCount).toInt());
                break;

            case 5: // ID6
                journal->setDateEnd(pQuery.value(iJ_JourCount).toInt());
                break;

            case 6: // ID7
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setSummary(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 7: // ID8
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setLocation(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 8: // ID9
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setDescription(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 9: // ID10
                journal->setStatus(pQuery.value(iJ_JourCount).toInt());
                break;

            case 10: // ID11
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setGUid(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 11: // ID12
                journal->setUntil(pQuery.value(iJ_JourCount).toInt());
                break;

            case 12: // ID13
                journal->setAllDay(pQuery.value(iJ_JourCount).toInt());
                break;

            case 13: // ID14
                journal->setCreatedTime(pQuery.value(iJ_JourCount).toInt());
                break;

            case 14: // ID15
                journal->setLastModified(pQuery.value(iJ_JourCount).toInt());
                break;

            case 15: // ID16
                journal->setTzid(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 16: // ID17
            case 17: // ID18
            case 18: // ID19
                break;

            case 19: // ID20
                if (pQuery.value(iJ_JourCount).toInt())
                    journal->setClas(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 20: // ID21
            case 21: // ID22
                break;

            case 22: // ID23
                journal->setDateStamp(pQuery.value(iJ_JourCount).toInt());
                break;

            case 23: // ID24
                journal->setSequence(pQuery.value(iJ_JourCount).toInt());
                break;

            case 24: // ID25
                break;

            case 25: // ID26
                journal->setUid(pQuery.value(iJ_JourCount).toInt());
                break;

            case 26: // ID27
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setUrl(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 27: // ID28
                break;

            case 28: // ID29
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setCategories(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 29: // ID30
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setComments(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 30: // ID31
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setContact(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 31: // ID32
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setRelated(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            case 32: // ID33
                if (!pQuery.value(iJ_JourCount).toString().isEmpty())
                    journal->setResources(pQuery.value(iJ_JourCount).toString().toStdString());
                break;

            default:
                break;
            }
        }

        /*retrieve xprop */
        std::vector<CProperties *> vPropList;
        vPropList = journal->retrieveXPropertyDetails();
        journal->setXProperties(vPropList);

        std::vector<CProperties *>::iterator vPropListIterator;
        for (vPropListIterator = vPropList.begin(); vPropListIterator != vPropList.end(); ++vPropListIterator)
            delete *vPropListIterator;

        /*retrieve params */
        std::map<std::string, std::vector<CParameters *> > paramMap;
        paramMap = journal->retrieveParameterDetails();
        journal->setHashMap(paramMap);
        paramMap.clear();

        /* push the journal in to the list */
        listJournal.push_back(journal);

        ++iI_JourCount;
    }

    // put to cache
    m_dbCache->insertJournalVector(cacheKey, listJournal);

    return listJournal;
}

CEvent* OrganizerCalendarDatabaseAccess::getEvent(CCalendar* cal, const std::string& id, int& calError)
{
    OrganizerIdCacheKey cacheKey(cal->getCalendarId(), QString::fromStdString(id));
    if (m_dbCache->containsEvent(cacheKey)) {
        return m_dbCache->takeEvent(cacheKey);
    }
    else {
        CEvent* event = cal->getEvent(id, calError);
        m_dbCache->insertEvent(cacheKey, event);
        return event;
    }
}

CTodo* OrganizerCalendarDatabaseAccess::getTodo(CCalendar* cal, const std::string& id, int& calError)
{
    OrganizerIdCacheKey cacheKey(cal->getCalendarId(), QString::fromStdString(id));
    if (m_dbCache->containsTodo(cacheKey)) {
        return m_dbCache->takeTodo(cacheKey);
    }
    else {
        CTodo* todo = cal->getTodo(id, calError);
        m_dbCache->insertTodo(cacheKey, todo);
        return todo;
    }
}

CJournal* OrganizerCalendarDatabaseAccess::getJournal(CCalendar* cal, const std::string& id, int& calError)
{
    OrganizerIdCacheKey cacheKey(cal->getCalendarId(), QString::fromStdString(id));
    if (m_dbCache->containsJournal(cacheKey)) {
        return m_dbCache->takeJournal(cacheKey);
    }
    else {
        CJournal* journal = cal->getJournal(id, calError);
        m_dbCache->insertJournal(cacheKey, journal);
        return journal;
    }
}

void OrganizerCalendarDatabaseAccess::getIdList(CCalendar* cal, int compType, int& calError, std::vector<std::string> &result)
{
    OrganizerCalIdTypeIdCacheKey cacheKey(cal->getCalendarId(), compType);
    if (m_dbCache->containsIds(cacheKey)) {
        m_dbCache->takeIdsVector(cacheKey, result);
    }
    else {
        result = cal->getIdList(compType, calError);
        m_dbCache->insertIds(cacheKey, result);
    }
}

void OrganizerCalendarDatabaseAccess::fixAlarmCookie(QPair<qint32, qint32> change)
{
    QSqlQuery query;
    if (!query.prepare(cookieFixer))
        return;
    query.bindValue(":oldCookie", change.first);
    query.bindValue(":newCookie", change.second);
    query.exec();
    if (m_dbCache)
        m_dbCache->invalidate();
}

void OrganizerCalendarDatabaseAccess::sqliteErrorMapper(const QSqlError &sqlError, int& errorCode)
{
    switch (sqlError.type()) {
        case QSqlError::NoError:
            errorCode = CALENDAR_OPERATION_SUCCESSFUL;
            break;
        case QSqlError::ConnectionError:
        case QSqlError::StatementError:
        case QSqlError::TransactionError:
        case QSqlError::UnknownError:
        default :
            errorCode = CALENDAR_DATABASE_ERROR;
            break;
    }
}
