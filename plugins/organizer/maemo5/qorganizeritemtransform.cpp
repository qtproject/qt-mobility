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

#include "qorganizermaemo5ids_p.h"
#include "qorganizeritemtransform.h"
#include "qtorganizer.h"

#include <QtCore/quuid.h>
#include <CalendarErrors.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>
#include <CRecurrence.h>
#include <CComponentDetails.h>
#include <CAlarm.h>
#include <alarmd/libalarm.h>

QTM_USE_NAMESPACE

OrganizerItemTransform::OrganizerItemTransform()
{
}

OrganizerItemTransform::~OrganizerItemTransform()
{
}

void OrganizerItemTransform::setManagerUri(QString managerUri)
{
    m_managerUri = managerUri;
}

QString OrganizerItemTransform::managerUri() const
{
    return m_managerUri;
}

OrganizerRecurrenceTransform* OrganizerItemTransform::recurrenceTransformer()
{
    return &m_recTransformer;
}

// This should probably accept a std::string
bool OrganizerItemTransform::addGeoToQOIL(const QString &src, QOrganizerItemLocation* detail) const
{
    double latitude = 0;
    double longitude = 0;

    Q_ASSERT(detail);

    QStringList sl = src.split(QLatin1Char(';'));
    if (sl.count() == 2) {
        bool ok = false;
        latitude = sl.at(0).toDouble(&ok);
        if (ok)
            longitude = sl.at(1).toDouble(&ok);
        if (ok) {
            detail->setLatitude(latitude);
            detail->setLongitude(longitude);
            return true;
        }
    }
    return false;
}

void OrganizerItemTransform::addEventPostSaveDetails(QOrganizerItem *item, CEvent *cevent)
{
    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1) {
        QOrganizerItemPriority priority = item->detail<QOrganizerItemPriority>();
        priority.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.
        item->saveDetail(&priority);
    }

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cevent->getDateStart());
    if (!tempdt.isNull()) {
        QOrganizerEventTime eventTime = item->detail<QOrganizerEventTime>();
        eventTime.setStartDateTime(tempdt);
        item->saveDetail(&eventTime);
    }

    // End time
    tempdt = QDateTime::fromTime_t(cevent->getDateEnd());
    if (!tempdt.isNull()) {
        QOrganizerEventTime eventTime = item->detail<QOrganizerEventTime>();
        eventTime.setEndDateTime(tempdt);
        item->saveDetail(&eventTime);
    }

    // Recurrence
    m_recTransformer.transformToQrecurrence(cevent->getRecurrence());
    QOrganizerItemRecurrence recurrence = item->detail<QOrganizerItemRecurrence>();
    recurrence.setRecurrenceRules(m_recTransformer.recurrenceRules());
    recurrence.setExceptionRules(m_recTransformer.exceptionRules());
    recurrence.setRecurrenceDates(m_recTransformer.recurrenceDates());
    recurrence.setExceptionDates(m_recTransformer.exceptionDates());
    item->saveDetail(&recurrence);

    // Timestamps
    time_t createdTime = cevent->getCreatedTime();
    time_t lastModifiedTime = cevent->getLastModified();

    if (createdTime || lastModifiedTime) {
        QOrganizerItemTimestamp timeStamps = item->detail<QOrganizerItemTimestamp>();
        timeStamps.setCreated(QDateTime::fromTime_t(createdTime));
        timeStamps.setLastModified(QDateTime::fromTime_t(lastModifiedTime));
        item->saveDetail(&timeStamps);
    }
}

void OrganizerItemTransform::addTodoPostSaveDetails(QOrganizerItem *item, CTodo *ctodo)
{
    // Priority
    int tempint = ctodo->getPriority();
    if (tempint != -1) {
        QOrganizerItemPriority priority = item->detail<QOrganizerItemPriority>();
        priority.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.
        item->saveDetail(&priority);
    }

    // Date start
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull()) {
        QOrganizerTodoTime todoTime = item->detail<QOrganizerTodoTime>();
        todoTime.setStartDateTime(tempdt);
        item->saveDetail(&todoTime);
    }

    // Due
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull()) {
        QOrganizerTodoTime todoTime = item->detail<QOrganizerTodoTime>();
        todoTime.setDueDateTime(tempdt);
        item->saveDetail(&todoTime);
    }

    // Completed time
    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull()) {
        QOrganizerTodoProgress todoProgress = item->detail<QOrganizerTodoProgress>();
        todoProgress.setFinishedDateTime(tempdt);
        item->saveDetail(&todoProgress);
    }

    // Percent complete
    tempint = ctodo->getPercentComplete();
    if (!tempdt.isNull()) {
        QOrganizerTodoProgress todoProgress = item->detail<QOrganizerTodoProgress>();
        todoProgress.setPercentageComplete(tempint);
        item->saveDetail(&todoProgress);
    }

    // Status
    QOrganizerTodoProgress todoProgress = item->detail<QOrganizerTodoProgress>();
    todoProgress.setStatus(static_cast<QOrganizerTodoProgress::Status>(ctodo->getStatus()));
    item->saveDetail(&todoProgress);

    // Timestamps
    time_t createdTime = ctodo->getCreatedTime();
    time_t lastModifiedTime = ctodo->getLastModified();

    if (createdTime || lastModifiedTime) {
        QOrganizerItemTimestamp timeStamps = item->detail<QOrganizerItemTimestamp>();
        timeStamps.setCreated(QDateTime::fromTime_t(createdTime));
        timeStamps.setLastModified(QDateTime::fromTime_t(lastModifiedTime));
        item->saveDetail(&timeStamps);
    }
}

void OrganizerItemTransform::addJournalPostSaveDetails(QOrganizerItem *item, CJournal *cjournal)
{
    // Journal time
    QDateTime tempdt = QDateTime::fromTime_t(cjournal->getDateStart());
    if (!tempdt.isNull()) {
        QOrganizerJournalTime journalTime = item->detail<QOrganizerJournalTime>();
        journalTime.setEntryDateTime(tempdt);
        item->saveDetail(&journalTime);
    }

    // Timestamps
    time_t createdTime = cjournal->getCreatedTime();
    time_t lastModifiedTime = cjournal->getLastModified();

    if (createdTime || lastModifiedTime) {
        QOrganizerItemTimestamp timeStamps = item->detail<QOrganizerItemTimestamp>();
        timeStamps.setCreated(QDateTime::fromTime_t(createdTime));
        timeStamps.setLastModified(QDateTime::fromTime_t(lastModifiedTime));
        item->saveDetail(&timeStamps);
    }
}

void OrganizerItemTransform::sortDetails(QOrganizerItem *item) const
{
    if(!item)
        return;

    QList<QOrganizerItemDetail> detailsTmp = item->details();
    qSort(detailsTmp.begin(), detailsTmp.end(), detailLessThan);
    item->clearDetails();
    int detailCount = detailsTmp.count();
    for (int i = 0; i < detailCount; ++i) {
        QOrganizerItemDetail curr = detailsTmp.at(i);
        item->saveDetail(&curr);
    }
}

bool detailLessThan(const QOrganizerItemDetail &d1, const QOrganizerItemDetail &d2)
{
    return (d1.definitionName() < d2.definitionName());
}

QOrganizerEvent OrganizerItemTransform::convertCEventToQEvent(CEvent *cevent)
{
    QOrganizerEvent retn;

    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.

    // Location geo coordinates
    QOrganizerItemLocation loc = retn.detail<QOrganizerItemLocation>();
    if(addGeoToQOIL(QString::fromStdString(cevent->getGeo()), &loc))
        retn.saveDetail(&loc);

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cevent->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // End time
    tempdt = QDateTime::fromTime_t(cevent->getDateEnd());
    if (!tempdt.isNull())
        retn.setEndDateTime(tempdt);

    // Recurrence information
    m_recTransformer.transformToQrecurrence(cevent->getRecurrence());

    retn.setRecurrenceRules(m_recTransformer.recurrenceRules());
    retn.setExceptionRules(m_recTransformer.exceptionRules());
    retn.setRecurrenceDates(m_recTransformer.recurrenceDates());
    retn.setExceptionDates(m_recTransformer.exceptionDates());

    return retn;
}

QOrganizerEventOccurrence OrganizerItemTransform::convertCEventToQEventOccurrence(CEvent *cevent)
{
    QDateTime instanceStartDate = QDateTime::fromTime_t(cevent->getDateStart());
    QDateTime instanceEndDate = QDateTime::fromTime_t(cevent->getDateEnd());
    return convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate, QOrganizerItemId(0));
}

QOrganizerEventOccurrence OrganizerItemTransform::convertCEventToQEventOccurrence(CEvent* cevent, const QDateTime& instanceStartDate, const QDateTime &instanceEndDate)
{
    QOrganizerEventOccurrence retn;

    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.

    // Location geo coordinates
    QOrganizerItemLocation loc = retn.detail<QOrganizerItemLocation>();
    if(addGeoToQOIL(QString::fromStdString(cevent->getGeo()), &loc))
        retn.saveDetail(&loc);

    // Start time
    if (!instanceStartDate.isNull())
        retn.setStartDateTime(instanceStartDate);

    // End time
    if (!instanceEndDate.isNull())
        retn.setEndDateTime(instanceEndDate);

    // Set parent local id
    QString idString = QString::fromStdString(cevent->getId());
    retn.setParentId(QOrganizerItemId(new QOrganizerItemMaemo5EngineId(idString.toUInt())));

    // Set original event date
    retn.setOriginalDate(instanceStartDate.date());

    return retn;
}

QOrganizerEventOccurrence OrganizerItemTransform::convertCEventToQEventOccurrence(CEvent *cevent, const QDateTime &instanceStartDate, const QDateTime &instanceEndDate, QOrganizerItemId parentId)
{
    QOrganizerEventOccurrence retn = convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate);
    retn.setParentId(parentId);
    return retn;
}

QOrganizerTodo OrganizerItemTransform::convertCTodoToQTodo(CTodo *ctodo)
{
    QOrganizerTodo retn;

    // Priority
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));

    // Date start
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // Due
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        retn.setDueDateTime(tempdt);

    // Completed time
    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        retn.setFinishedDateTime(tempdt);

    // Percent complete
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        retn.setProgressPercentage(tempint);

    // Status
    retn.setStatus(static_cast<QOrganizerTodoProgress::Status>(ctodo->getStatus()));

    // Location geo coordinates
    QOrganizerItemLocation loc = retn.detail<QOrganizerItemLocation>();
    if(addGeoToQOIL(QString::fromStdString(ctodo->getGeo()), &loc))
        retn.saveDetail(&loc);

    return retn;
}

QOrganizerTodoOccurrence OrganizerItemTransform::convertCTodoToQTodoOccurrence(CTodo *ctodo)
{
    QOrganizerTodoOccurrence retn;

    // Keep the following details same as in convertCTodoToQTodo:

    // Priority
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));

    // Date start
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // Due
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        retn.setDueDateTime(tempdt);

    // Completed time
    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        retn.setFinishedDateTime(tempdt);

    // Percent complete
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        retn.setProgressPercentage(tempint);

    // Status
    retn.setStatus(static_cast<QOrganizerTodoProgress::Status>(ctodo->getStatus()));

    // Location geo coordinates
    QOrganizerItemLocation loc = retn.detail<QOrganizerItemLocation>();
    if(addGeoToQOIL(QString::fromStdString(ctodo->getGeo()), &loc))
        retn.saveDetail(&loc);

    // Only the following are occurrence specific details:

    // In maemo, the parent id is the same as this id (todo's only have one occurrence)
    retn.setParentId(makeItemLocalId(QString::fromStdString(ctodo->getId()).toUInt()));

    // Original date
    retn.setOriginalDate(retn.startDateTime().date());

    return retn;
}

QOrganizerJournal OrganizerItemTransform::convertCJournalToQJournal(CJournal *cjournal)
{
    QOrganizerJournal retn;

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cjournal->getDateStart());
    if (!tempdt.isNull())
        retn.setDateTime( tempdt );

    return retn;
}

void OrganizerItemTransform::fillInCommonCComponentDetails(QOrganizerItem *item, CComponent *component, bool setId)
{
    if (item) {
        // Summary
        QString tempstr = QString::fromStdString(component->getSummary());
        if (!tempstr.isEmpty())
            item->setDisplayLabel(tempstr);

        // Description
        tempstr = QString::fromStdString(component->getDescription());
        if (!tempstr.isEmpty())
            item->setDescription(tempstr);

        // Location
        tempstr = QString::fromStdString(component->getLocation());
        if(!tempstr.isEmpty()) {
            QOrganizerItemLocation il = item->detail<QOrganizerItemLocation>();
            il.setLabel(tempstr);
            item->saveDetail(&il);
        }

        // Timestamps
        time_t createdTime = component->getCreatedTime();
        time_t lastModifiedTime = component->getLastModified();

        if (createdTime || lastModifiedTime) {
            QOrganizerItemTimestamp timeStamps = item->detail<QOrganizerItemTimestamp>();
            timeStamps.setCreated(QDateTime::fromTime_t(createdTime));
            timeStamps.setLastModified(QDateTime::fromTime_t(lastModifiedTime));
            item->saveDetail(&timeStamps);
        }

        // GUid
        QOrganizerItemGuid ig = item->detail<QOrganizerItemGuid>();
        tempstr = QString::fromStdString(component->getGUid());
        if(!tempstr.isEmpty())
            ig.setGuid(tempstr);
        item->saveDetail(&ig);

        // Set component ID
        if (setId) {
            QString idString = QString::fromStdString(component->getId());
            item->setId(QOrganizerItemId(new QOrganizerItemMaemo5EngineId(idString.toUInt())));
        } else {
            item->setId(QOrganizerItemId());
        }

        // Set comments
        CComponentDetails *componentDetails = dynamic_cast<CComponentDetails*>(component);
        if (componentDetails) {
            string comments = componentDetails->getComments();
            if (!comments.empty()) {
                QStringList commentList = QString::fromStdString(comments).split('\n', QString::SkipEmptyParts);
                foreach(const QString comment, commentList)
                    item->addComment(comment);
            }
        }

        // Reminder (alarm)
        CAlarm *alarm = component->getAlarm();
        if (alarm) {
            // TODO: Only visual remainders are supported
            QOrganizerItemVisualReminder reminder = item->detail<QOrganizerItemVisualReminder>();
            reminder.setRepetition(alarm->getRepeat(), reminder.repetitionDelay());

            // Alarm time and messages can't be read with CAlarm,
            // read them straight from the alarm framework:

            // Get the cookie
            std::vector<long> cookies = alarm->getCookie();
            if (cookies.size() > 0) {
                cookie_t cookie = static_cast<cookie_t>(cookies[0]); // only one alarm supported

                alarm_event_t *eve = 0;
                if ((eve = alarmd_event_get(cookie)) != 0) {
                    QString message = QString::fromStdString(alarm_event_get_title(eve));
                    reminder.setMessage(message);
                    time_t alarmTime = alarm_event_get_trigger(eve);
                    alarm_event_delete(eve);

                    QDateTime sTime = QDateTime::fromTime_t(component->getDateStart());
                    QDateTime aTime = QDateTime::fromTime_t(alarmTime);
                    reminder.setSecondsBeforeStart(aTime.secsTo(sTime));
                }
            }

            item->saveDetail(&reminder);
        }
    }
}

CComponent* OrganizerItemTransform::createCComponent(CCalendar *cal, const QOrganizerItem *item, QOrganizerManager::Error *error)
{
    *error = QOrganizerManager::InvalidItemTypeError;

    QOrganizerItemId itemId = item->id();

    QString itemIdStr = "";
    if (!itemId.isNull())
        itemIdStr = QString::number(readItemLocalId(itemId));

    int calId = cal->getCalendarId();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    CComponent *retn = 0; // Return null on errors

    QDateTime dateStartForAlarm;

    if (item->type() == QOrganizerItemType::TypeEvent
        || item->type() == QOrganizerItemType::TypeEventOccurrence) {

        *error = QOrganizerManager::NoError;

        CEvent *cevent = cal->getEvent(itemIdStr.toStdString(), calError);
        if (!cevent) {
            if (!itemId.isNull()) {
                // If item id is not null, the item must exist. Otherwise return 'does not found'.
                *error = QOrganizerManager::DoesNotExistError;
                return retn;
            }
            // Event did not existed in calendar, create a new CEvent with an empty ID
            cevent = new CEvent();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            cevent->setId(itemIdStr.toStdString());
        }
        cevent->setCalendarId(calId);

        // Set the event specific details:
        const QOrganizerEvent *event = static_cast<const QOrganizerEvent *>(item);

        // Location geo coordinates
        QOrganizerItemLocation loc = event->detail<QOrganizerItemLocation>();
        if (loc.hasValue(QOrganizerItemLocation::FieldLatitude) && loc.hasValue(QOrganizerItemLocation::FieldLongitude)) {
            // std::ostringstream, perhaps
            char buff[64];
            if (snprintf(buff, sizeof(buff), "%.18f;%.18f", loc.latitude(), loc.longitude()) > 0)
                cevent->setGeo(std::string(buff));
        }

        // Priority
        cevent->setPriority(static_cast<int>(event->priority()));

        // Start date
        if (!event->startDateTime().isNull()) {
            time_t startTime = event->startDateTime().toTime_t();
            cevent->setDateStart(startTime);
            dateStartForAlarm = QDateTime::fromTime_t(startTime);
        }

        // End date
        if (!event->endDateTime().isNull())
            cevent->setDateEnd(event->endDateTime().toTime_t());

        if (item->type() == QOrganizerItemType::TypeEvent) {
            // Build and set the recurrence information for the event
            CRecurrence *recurrence = createCRecurrence(item, error);
            cevent->setRecurrence(recurrence);
            delete recurrence; // setting makes a copy
            recurrence = 0;
        }
        // (else nothing: QOrganizerItemEventOccurrence does not have recurrence information)

        retn = cevent;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo
            || item->type() == QOrganizerItemType::TypeTodoOccurrence) {

        *error = QOrganizerManager::NoError;

        CTodo *ctodo = cal->getTodo(itemIdStr.toStdString(), calError);
        if (!ctodo) {
            if (!itemId.isNull()) {
                // If item id is not null, the item must exist. Otherwise return 'does not found'.
                *error = QOrganizerManager::DoesNotExistError;
                return retn;
            }
            // Todo did not existed in calendar, create a new CEvent with an empty ID
            ctodo = new CTodo();
        }
        else {
            // Todo existed in calendar, use the existing event's ID
            ctodo->setId(itemIdStr.toStdString());
        }
        ctodo->setCalendarId(calId);

        // Set the todo specific details:
        const QOrganizerTodo *todo = static_cast<const QOrganizerTodo *>(item);

        // Priority
        ctodo->setPriority(static_cast<int>(todo->priority()));

        // Due date
        if (!todo->dueDateTime().isNull())
            ctodo->setDue(todo->dueDateTime().toTime_t());

        // Date start
        if (!todo->startDateTime().isNull()) {
            time_t startTime = todo->startDateTime().toTime_t();
            ctodo->setDateStart(startTime);
            dateStartForAlarm = QDateTime::fromTime_t(startTime);
        }

        // Completed time
        if (!todo->finishedDateTime().isNull())
            ctodo->setCompleted(todo->finishedDateTime().toTime_t());

        // Percent complete
        ctodo->setPercentComplete(todo->progressPercentage());

        // Status
        ctodo->setStatus(todo->status());

        // Location geo coordinates
        QOrganizerItemLocation loc = todo->detail<QOrganizerItemLocation>();
        if (loc.hasValue(QOrganizerItemLocation::FieldLatitude) && loc.hasValue(QOrganizerItemLocation::FieldLongitude)) {
            // std::ostringstream, perhaps
            char buff[64];
            if (snprintf(buff, sizeof(buff), "%.18f;%.18f", loc.latitude(), loc.longitude()) > 0)
                ctodo->setGeo(std::string(buff));
        }

        // Recurrence is not set as todos can't contain any recurrence information in Maemo5

        retn = ctodo;
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        *error = QOrganizerManager::NoError;
        CJournal *cjournal = cal->getJournal(itemIdStr.toStdString(), calError);
        if (!cjournal) {
            if (!itemId.isNull()) {
                // If item id is not null, the item must exist. Otherwise return 'does not found'.
                *error = QOrganizerManager::DoesNotExistError;
                return retn;
            }
            // Journal did not existed in calendar, create a new CEvent with an empty ID
            cjournal = new CJournal();
        }
        else {
            // Journal existed in calendar, use the existing event's ID
            cjournal->setId(itemIdStr.toStdString());
        }
        cjournal->setCalendarId(calId);

        // Set journal specific details
        const QOrganizerJournal *journal = static_cast<const QOrganizerJournal *>(item);
        if (!journal->dateTime().isNull())
            cjournal->setDateStart(journal->dateTime().toTime_t());

        retn = cjournal;
    }

    if (retn) {
        // Set the details common for all the CComponents:

        // Summary
        if (!item->displayLabel().isEmpty())
            retn->setSummary(item->displayLabel().toStdString());

        // Descriptiom
        if (!item->description().isEmpty())
            retn->setDescription(item->description().toStdString());

        // Location (Geo location is not set here as it's not a general CComponent detail)
        QOrganizerItemLocation location = item->detail(QOrganizerItemLocation::DefinitionName);
        if (!location.label().isEmpty()) {
            retn->setLocation(location.label().toStdString());
        }

        // dateStart and dateEnd are common fields for all CComponents, but those are set
        // separately for each item type here, because there are independent time ranges
        // defined for each type in the Qt Mobility API.

        // GUid
        if (!item->guid().isEmpty())
            retn->setGUid(item->guid().toStdString());
        else
            retn->setGUid(randomGuid().toStdString()); // no GUID was set, generate a random GUID

        // Comments
        QStringList commentList = item->comments();
        if (!commentList.isEmpty()) {
            string comments;
            foreach(QString comment, commentList)
                comments += comment.append('\n').toStdString();

            CComponentDetails *componentDetails = dynamic_cast<CComponentDetails*>(retn);
            if (componentDetails)
                componentDetails->setComments(comments);
        }

        // Visual reminder (alarm)
        QOrganizerItemVisualReminder reminder = item->detail<QOrganizerItemVisualReminder>();

        if (!reminder.isEmpty() && dateStartForAlarm.isValid()) {
            QDateTime deltaDateTime = dateStartForAlarm.addSecs(-reminder.secondsBeforeStart());

            // Set alarm for the ccomponent
            CAlarm alarm;

            alarm.setRepeat(reminder.repetitionCount());
            alarm.setDuration(E_AM_EXACTDATETIME); // TODO: repetitionDelay()?
            alarm.setTrigger(deltaDateTime.toTime_t());

            retn->setAlarm(&alarm); // makes a copy
        }
    }

    return retn;
}

CRecurrence* OrganizerItemTransform::createCRecurrence(const QOrganizerItem* item, QOrganizerManager::Error *error)
{
    *error = QOrganizerManager::NoError;

    // Only the event and todo types contain recurrence information
    if (item->type() == QOrganizerItemType::TypeEvent) {
        m_recTransformer.beginTransformToCrecurrence();
        const QOrganizerEvent *event = static_cast<const QOrganizerEvent *>(item);

        // Add recurrence rules
        QSet<QOrganizerRecurrenceRule> recurrenceRules = event->recurrenceRules();
        foreach (QOrganizerRecurrenceRule rule, recurrenceRules)
            m_recTransformer.addQOrganizerRecurrenceRule(rule);

        // Add exception rules
        QSet<QOrganizerRecurrenceRule> exceptionRules = event->exceptionRules();
        foreach (QOrganizerRecurrenceRule rule, exceptionRules)
            m_recTransformer.addQOrganizerItemExceptionRule(rule);

        // Add recurrence dates
        QDate dateLimit = event->startDateTime().date().addYears(6);
        QSet<QDate> recurrenceDates = event->recurrenceDates();
        foreach (QDate recDate, recurrenceDates) {
            // Because recurrence dates are simulated with setting an appropriate
            // recurrence rule, no date must be set 6 years or more after
            // the current event's date. Otherwise setting a correct rule would be impossible.
            if (recDate >= dateLimit)
                *error = QOrganizerManager::NotSupportedError;

            // Still set the date, let the caller decise what to do
            m_recTransformer.addQOrganizerItemRecurrenceDate(recDate);
        }

        // Add exception dates
        QSet<QDate> exceptionDates = event->exceptionDates();
        foreach (QDate exceptionDate, exceptionDates) {
            // Because exception dates are simulated with setting an appropriate
            // exception rule, no date must be set 6 years or more after
            // the current event's date. Otherwise setting a correct rule would be impossible.
            if (exceptionDate >= dateLimit)
                *error = QOrganizerManager::NotSupportedError;

            // Still set the date, let the caller decise what to do
            m_recTransformer.addQOrganizerItemExceptionDate(exceptionDate);
        }

        return m_recTransformer.crecurrence();
    }

    return 0; // no recurrence information for this item type
}

QPair<qint32, qint32> OrganizerItemTransform::modifyAlarmEvent(CCalendar *cal, QOrganizerItem *item, CComponent *component)
{
    qint32 oldCookie = -1;
    qint32 newCookie = -1;
    CAlarm *alarm = component->getAlarm();
    if (alarm) {
        std::vector<long> cookies = alarm->getCookie();
        if (!cookies.empty()) {
            oldCookie = cookies[0];

            QOrganizerItemVisualReminder reminder = item->detail<QOrganizerItemVisualReminder>();
            QDateTime reminderDateTime;
            QDateTime startDateTime;
            if (item->type() == QOrganizerItemType::TypeEvent || item->type() == QOrganizerItemType::TypeEventOccurrence) {
                startDateTime = item->detail<QOrganizerEventTime>().startDateTime();
            }
            else if (item->type() == QOrganizerItemType::TypeTodo || item->type() == QOrganizerItemType::TypeTodoOccurrence) {
                startDateTime = item->detail<QOrganizerTodoTime>().startDateTime();
            }
            else if (item->type() == QOrganizerItemType::TypeJournal) {
                startDateTime = item->detail<QOrganizerJournalTime>().entryDateTime();
            }

            // XXX Shouldn't this check to see if the reminder is valid - if invalid, delete the alarm event?
            reminderDateTime = startDateTime.addSecs(-reminder.secondsBeforeStart());

            int ignoreErrors = 0;
            newCookie = alarm->modifyAlarmEvent(oldCookie, reminderDateTime.toTime_t(), reminder.message().toStdString(),
                                                component->getLocation(), component->getDateStart(), component->getDateEnd(),
                                                component->getId(), cal->getCalendarId(), component->getDescription(),
                                                component->getType(), component->getAllDay(), QString("").toStdString(),
                                                ignoreErrors);

            /*
            newCookie = alarm->modifyAlarmEvent(oldCookie, reminder.dateTime().toTime_t(), reminder.message().toStdString(),
                                                component->getLocation(), component->getDateStart(), component->getDateEnd(),
                                                component->getId(), cal->getCalendarId(), component->getDescription(),
                                                component->getType(), component->getAllDay(), QString("").toStdString(),
                                                ignoreErrors);
                                                */
        }
    }
    return QPair<qint32, qint32>(oldCookie, newCookie);
}

QOrganizerManager::Error OrganizerItemTransform::calErrorToManagerError(int calError) const
{
    switch(calError) {
        case CALENDAR_OPERATION_SUCCESSFUL:
            return QOrganizerManager::NoError;

        case CALENDAR_SYSTEM_ERROR:
        case CALENDAR_DATABASE_ERROR:
        // case CALENDAR_DBUS_ERROR: // CALENDAR_DBUS_ERROR and CALENDAR_FILE_ERROR have the same value 3
        case CALENDAR_LTIME_ERROR:
        case CALENDAR_LIBALARM_ERROR:
        case CALENDAR_ALARM_ERROR:
        case CALENDAR_APP_ERROR:
        case CALENDAR_FUNC_ERROR:
        case CALENDAR_ICAL_PARSE_ERROR:
        case CALENDAR_INVALID_FILE:
        case CALENDAR_INVALID_ICSFILE:
        case CALENDAR_SCHEMA_CHANGED:
        case CALENDAR_IMPORT_INCOMPLETE:
            return QOrganizerManager::UnspecifiedError;

        case CALENDAR_MEMORY_ERROR:
            return QOrganizerManager::OutOfMemoryError;

        case CALENDAR_FILE_ERROR:
        case CALENDAR_DOESNOT_EXISTS:
        case CALENDAR_NONE_INDB:
        case NO_DUPLICATE_ITEM:
        case CALENDAR_FETCH_NOITEMS:
            return QOrganizerManager::DoesNotExistError;

        case CALENDAR_DISK_FULL:
        case CALENDAR_DB_FULL:
            return QOrganizerManager::LimitReachedError;

        case CALENDAR_INVALID_ARG_ERROR:
            return QOrganizerManager::BadArgumentError;

        case CALENDAR_ALREADY_EXISTS:
        case CALENDAR_ENTRY_DUPLICATED:
            return QOrganizerManager::AlreadyExistsError;

        case CALENDAR_CANNOT_BE_DELETED:
        case EXT_ITEM_RETAINED:
        case LOCAL_ITEM_RETAINED:
            return QOrganizerManager::PermissionsError;

        case CALENDAR_DB_LOCKED:
            return QOrganizerManager::LockedError;

        case CALENDAR_ICS_COMPONENT_INVALID:
        case CALENDAR_BATCH_ADD_INVALID:
            return QOrganizerManager::InvalidDetailError;

        default:
            return QOrganizerManager::UnspecifiedError;
    }
}

QString OrganizerItemTransform::fromCalendarColour(CalendarColour calendarColour)
{
    QMap<CalendarColour, QString> colourMap = calendarColourMap();
    if (colourMap.contains(calendarColour))
        return colourMap[calendarColour];
    else
        return QString();
}

CalendarColour OrganizerItemTransform::toCalendarColour(QString calendarColour)
{
    QMap<CalendarColour, QString> colourMap = calendarColourMap();
    if (colourMap.values().contains(calendarColour))
        return colourMap.key(calendarColour);
    else
        return COLOUR_DARKBLUE; // default color
}

QMap<CalendarColour, QString> OrganizerItemTransform::calendarColourMap() const
{
    QMap<CalendarColour, QString> retn;
    retn[COLOUR_DARKBLUE] = "Dark blue";
    retn[COLOUR_DARKGREEN] = "Dark green";
    retn[COLOUR_DARKRED] = "Dark red";
    retn[COLOUR_ORANGE] = "Orange";
    retn[COLOUR_VIOLET] = "Violet";
    retn[COLOUR_YELLOW] = "Yellow";
    retn[COLOUR_WHITE] = "White";
    retn[COLOUR_RED] = "Red";
    retn[COLOUR_GREEN] = "Green";
    retn[COLOUR_BLUE] = "Blue";
    retn[COLOUR_NEXT_FREE] = "Next free";
    return retn;
}

QString OrganizerItemTransform::fromCalendarType(CalendarType calendarType)
{
    QMap<CalendarType, QString> typeMap = calendarTypeMap();
    if (typeMap.contains(calendarType))
        return typeMap[calendarType];
    else
        return QString();
}

CalendarType OrganizerItemTransform::toCalendarType(QString calendarType)
{
    QMap<CalendarType, QString> typeMap = calendarTypeMap();
    if (typeMap.values().contains(calendarType))
        return typeMap.key(calendarType);
    else
        return LOCAL_CALENDAR; // default type
}

QMap<CalendarType, QString> OrganizerItemTransform::calendarTypeMap() const
{
    QMap<CalendarType, QString> retn;
    retn[LOCAL_CALENDAR] = "Local";
    retn[BIRTHDAY_CALENDAR] = "Birthday";
    retn[SYNC_CALENDAR] = "Sync";
    retn[DEFAULT_PRIVATE] = "Default private";
    retn[DEFAULT_SYNC] = "Default sync";
    return retn;
}

void OrganizerItemTransform::modifyBaseSchemaDefinitions(QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > &schemaDefs) const
{
    // Add all the supported detail definition names
    QStringList supportedDetails;
    supportedDetails << QOrganizerEventTime::DefinitionName;
    supportedDetails << QOrganizerItemComment::DefinitionName;
    supportedDetails << QOrganizerItemDescription::DefinitionName;
    supportedDetails << QOrganizerItemDisplayLabel::DefinitionName;
    supportedDetails << QOrganizerItemGuid::DefinitionName;
    supportedDetails << QOrganizerItemLocation::DefinitionName;
    supportedDetails << QOrganizerItemParent::DefinitionName;
    supportedDetails << QOrganizerItemPriority::DefinitionName;
    supportedDetails << QOrganizerItemRecurrence::DefinitionName;
    supportedDetails << QOrganizerItemTimestamp::DefinitionName;
    supportedDetails << QOrganizerItemType::DefinitionName;
    supportedDetails << QOrganizerItemVisualReminder::DefinitionName;
    supportedDetails << QOrganizerJournalTime::DefinitionName;
    supportedDetails << QOrganizerTodoProgress::DefinitionName;
    supportedDetails << QOrganizerTodoTime::DefinitionName;

    // Remove all not supported details
    foreach (QString itemTypeName, schemaDefs.keys()) {
        QMap<QString, QOrganizerItemDetailDefinition> details = schemaDefs.value(itemTypeName);
        foreach (QString detailName, details.keys()) {
            if (!supportedDetails.contains(detailName))
                schemaDefs[itemTypeName].remove(detailName);
        }
    }
}

QString OrganizerItemTransform::randomGuid() const
{
    QUuid guid = QUuid::createUuid();
    return guid.toString();
}
