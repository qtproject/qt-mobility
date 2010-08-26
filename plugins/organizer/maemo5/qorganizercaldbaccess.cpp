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

#include "qorganizercaldbaccess.h"
#include "qtorganizer.h"

#include <CalendarErrors.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>
#include <CAlarm.h>
#include <CRecurrence.h>

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

QTM_USE_NAMESPACE

const QString selectLeftJoin = QString("select * from components left join componentdetails on components.Id = componentdetails.Id Left Join alarm on components.Id = alarm.Id Left Join Recursive on components.Id = Recursive.Id where components.Id = \"$1\" ");

const QString selectInnerJoinBatchGuid = QString("select * from components left join componentdetails on components.Id = componentdetails.Id Left Join alarm on components.Id = alarm.Id Left Join Recursive on components.Id = Recursive.Id where Calendarid = $1 AND Components.ComponentType = $2 AND Components.Uid = \"$3\"");

OrganizerCalendarDatabaseAccess::OrganizerCalendarDatabaseAccess()
{
}

OrganizerCalendarDatabaseAccess::~OrganizerCalendarDatabaseAccess()
{
    close();
}

bool OrganizerCalendarDatabaseAccess::open(QString databasePathName)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setHostName("localhost");
    m_db.setDatabaseName(databasePathName);
    bool ok = m_db.open();
    return ok;
}

void OrganizerCalendarDatabaseAccess::close()
{
    m_db.close();
}

int OrganizerCalendarDatabaseAccess::typeOf(QOrganizerItemLocalId id, bool &ok)
{
    QString queryString = selectLeftJoin;
    queryString.replace("$1", QString::number(id)); // TODO: binding parameter values do not work for some reason
    QSqlQuery query(queryString);
    ok = query.exec();

    int retn = -1;
    if (query.next()) {
        retn = query.value(2).toInt();
    }
    else {
        ok = false;
    }
}

std::vector<CEvent *> OrganizerCalendarDatabaseAccess::getEvents(int calId, std::string guid, int &pErrorCode)
{
    const int columnNumber = 49;
    CEvent *event = 0;
    CAlarm *pAlarm = 0;
    CRecurrence *pRec = 0;
    std::vector<CEvent*> listEvent;
    int iI_EventCount = 0;
    int iJ_EventCount = 0;
    //int iK_EventCount = 0;
    pErrorCode = CALENDAR_OPERATION_SUCCESSFUL;
    std::vector<long> vCookie;
    std::vector<std::string> vERule;
    std::vector<std::string> vEdate;
    std::vector<std::string> vRdate;
    std::vector<std::string> vRRule;

    QString queryString = selectInnerJoinBatchGuid;
    queryString.replace("$1", QString::number(calId));
    queryString.replace("$2", QString::number(E_EVENT));
    queryString.replace("$3", QString::fromStdString(guid));
    qDebug() << queryString;
    QSqlQuery pQuery(queryString);
    bool ok = pQuery.exec();

    // TODO: Map query execution errors to calendar errors

    while (pQuery.next()) {
        event = new CEvent();
        pAlarm = new CAlarm();
        pRec = new CRecurrence();
        for (iJ_EventCount = 0; iJ_EventCount < columnNumber; ++iJ_EventCount) {
            //iK_EventCount = columnNumber + (iI_EventCount * columnNumber);


        }

        ++iI_EventCount;
    }


    std::vector<CEvent *> tmp;
    return tmp;
}

std::vector<CTodo *> OrganizerCalendarDatabaseAccess::getTodos(int calId, std::string guid, int &pErrorCode)
{

}

std::vector<CJournal *> OrganizerCalendarDatabaseAccess::getJournals(int calId, std::string guid, int &pErrorCode)
{

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
