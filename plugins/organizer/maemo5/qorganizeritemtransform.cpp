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

#include "qorganizeritemtransform.h"
#include "qtorganizer.h"

#include <QtCore/quuid.h>
#include <CalendarErrors.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>
#include <CRecurrence.h>
#include <CComponentDetails.h>

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

QOrganizerEvent OrganizerItemTransform::convertCEventToQEvent(CEvent *cevent)
{
    QOrganizerEvent retn;

    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1)
    retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.

    // Location geo coordinates
    QString tempstr = QString::fromStdString(cevent->getGeo());
    if (!tempstr.isNull())
        retn.setLocationGeoCoordinates(tempstr);

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
    return convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate, QOrganizerItemLocalId(0));
}

QOrganizerEventOccurrence OrganizerItemTransform::convertCEventToQEventOccurrence(CEvent* cevent, const QDateTime& instanceStartDate, const QDateTime &instanceEndDate)
{
    QOrganizerEventOccurrence retn;

    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.

    // Location geo coordinates
    QString tempstr = QString::fromStdString(cevent->getGeo());
    if (!tempstr.isNull())
        retn.setLocationGeoCoordinates(tempstr);

    // Start time
    if (!instanceStartDate.isNull())
        retn.setStartDateTime(instanceStartDate);

    // End time
    if (!instanceEndDate.isNull())
        retn.setEndDateTime(instanceEndDate);

    // Set parent local id
    QString idString = QString::fromStdString(cevent->getId());
    QOrganizerItemLocalId localId = idString.toUInt();
    retn.setParentLocalId(localId);

    // Set original event date
    retn.setOriginalDate(instanceStartDate.date());

    return retn;
}

QOrganizerEventOccurrence OrganizerItemTransform::convertCEventToQEventOccurrence(CEvent *cevent, const QDateTime &instanceStartDate, const QDateTime &instanceEndDate, QOrganizerItemLocalId parentLocalId)
{
    QOrganizerEventOccurrence retn = convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate);
    retn.setParentLocalId(parentLocalId);
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

    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        retn.setFinishedDateTime(tempdt);
    tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // TODO: How all the time fields of todos should be mapped????

    // Percent complete
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        retn.setProgressPercentage(tempint);

    // Status
    retn.setStatus(static_cast<QOrganizerTodoProgress::Status>(ctodo->getStatus()));

    return retn;
}

QOrganizerTodoOccurrence OrganizerItemTransform::convertCTodoToQTodoOccurrence(CTodo *ctodo, const QString &calendarName)
{
    // TODO: The code in this method is from the Proof of Concept
    // TODO: Replace the Proof of Concept codes at some point

    QOrganizerTodoOccurrence retn;
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);
    QString tempstr = QString::fromStdString(ctodo->getSummary());
    if (!tempstr.isEmpty())
        retn.setDisplayLabel(tempstr);
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        retn.setDueDateTime(tempdt);
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        retn.setProgressPercentage(tempint);
    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        retn.setFinishedDateTime(tempdt);
    tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // status is always available..
    // TODO: Commented out, check this!
    //ret.setStatus(static_cast<QOrganizerItemTodoProgress::Status>(ctodo->getStatus()));

    // now, in maemo, the parent id is the same as this id (todo's only have one occurrence).

    // TODO: These were removed from API, check if these are still needed?
    //ret.setParentItemId(rId);
    //ret.setOriginalDateTime(QDateTime::fromTime_t(ctodo->getDue())); // XXX TODO: verify this is the correct field to use as the instance date...

    return retn;
}

QOrganizerJournal OrganizerItemTransform::convertCJournalToQJournal(CJournal *cjournal)
{
    QOrganizerJournal retn;

    // TODO: What else should be set for a journal?

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cjournal->getDateStart());
    if (!tempdt.isNull())
        retn.setDateTime( tempdt );

    return retn;
}

void OrganizerItemTransform::fillInCommonCComponentDetails(QOrganizerItem *item, CComponent *component, bool setId)
{
    // TODO: Possible fields to add (defined in the CComponent superclass):
    // TODO: flags, status, (datestart), (dateend), lastmodified, (comments)
    // TODO: recurrence, until, rtype, alarm, allday, tzoffset... still others??

    // TODO: What to do with attendee, instanceorigin, participation, priority, provenance
    // TODO: rsvpinfo, timestamp... others??

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
            il.setLocationName( tempstr );
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
        else {
            /*
            ig.setGuid(randomGuid()); // no GUID was set, generate a random GUID
            qDebug() << "GUID " << ig.guid() << " given to item with label " << item->displayLabel();
            */
        }
        item->saveDetail(&ig);

        // Set component ID
        QOrganizerItemId id;
        id.setManagerUri(managerUri());
        if (setId) {
            QString idString = QString::fromStdString(component->getId());
            QOrganizerItemLocalId localId = idString.toUInt();
            id.setLocalId(localId);
        }
        else {
            id.setLocalId(QOrganizerItemLocalId()); // no local id
        }
        item->setId(id);

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
    }
}

CComponent* OrganizerItemTransform::createCComponent(CCalendar *cal, const QOrganizerItem *item)
{
    QOrganizerItemLocalId itemId = item->localId();
    QString itemIdStr = QString::number(itemId);
    int calId = cal->getCalendarId();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    CComponent *retn = 0; // Return null on errors

    if (item->type() == QOrganizerItemType::TypeEvent
        || item->type() == QOrganizerItemType::TypeEventOccurrence) {

        CEvent *cevent = cal->getEvent(itemIdStr.toStdString(), calError);
        if (!cevent) {
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
        if (!event->locationGeoCoordinates().isEmpty())
            cevent->setGeo(event->locationGeoCoordinates().toStdString());

        // Priority
        if (!event->detail("QOrganizerItemPriority::DefinitionName").isEmpty())
            cevent->setPriority(static_cast<int>(event->priority()));

        // Start date
        if (!event->startDateTime().isNull())
            cevent->setDateStart(event->startDateTime().toTime_t());

        // End date
        if (!event->endDateTime().isNull())
            cevent->setDateEnd(event->endDateTime().toTime_t());

        if (item->type() == QOrganizerItemType::TypeEvent) {
            // Build and set the recurrence information for the event
            CRecurrence *recurrence = createCRecurrence(item);
            cevent->setRecurrence(recurrence);
            delete recurrence; // setting makes a copy
            recurrence = 0;
        }
        // (else nothing: QOrganizerItemEventOccurrence does not have recurrence information)

        retn = cevent;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo
            || item->type() == QOrganizerItemType::TypeTodoOccurrence) {

        CTodo *ctodo = cal->getTodo(itemIdStr.toStdString(), calError);
        if (!ctodo) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            ctodo = new CTodo();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            ctodo->setId(itemIdStr.toStdString());
        }
        ctodo->setCalendarId(calId);

        // Set the todo specific details:
        const QOrganizerTodo *todo = static_cast<const QOrganizerTodo *>(item);

        // Location geo coordinates
        if (!todo->detail("QOrganizerItemLocation::DefinitionName").isEmpty())
            ctodo->setGeo(todo->detail<QOrganizerItemLocation>().geoLocation().toStdString());

        // Priority
        if (!todo->detail("QOrganizerItemPriority::DefinitionName").isEmpty())
            ctodo->setPriority(static_cast<int>(todo->priority()));

        // Start date
        // TODO: ctodo->setDateStart and ctodo->setDue actually affect to the
        // same attribute of CTodo instance... How should these be set?

        // Due date
        if (!todo->dueDateTime().isNull())
            ctodo->setDue(todo->dueDateTime().toTime_t());
        // TODO: CTodo::setCompleted, CTodo::setPercentComplete ??

        // TODO: How shall all these time fields be set??????
        /*
        if (!todo->finishedDateTime().isNull())
            ctodo->setDateStart(todo->finishedDateTime().toTime_t());
        */

        // Progress percentage
        ctodo->setPercentComplete(todo->progressPercentage());

        // Build and set the recurrence information for the todo
        CRecurrence *recurrence = createCRecurrence(item);
        ctodo->setRecurrence(recurrence);
        delete recurrence; // setting makes a copy
        recurrence = 0;

        retn = ctodo;
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        CJournal *cjournal = cal->getJournal(itemIdStr.toStdString(), calError);
        if (!cjournal) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            cjournal = new CJournal();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            cjournal->setId(itemIdStr.toStdString());
        }
        cjournal->setCalendarId(calId);

        // Set journal specific details
        // TODO: There are no journal specific details in Maemo.
        // TODO: Maybe should be removed and should be set on the upper level?
        const QOrganizerJournal *journal = static_cast<const QOrganizerJournal *>(item);
        if (!journal->dateTime().isNull())
            cjournal->setDateStart(journal->dateTime().toTime_t());

        retn = cjournal;
    }
    else if (item->type() == QOrganizerItemType::TypeNote) {
        // TODO
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
        QOrganizerItemLocation location = item->detail("QOrganizerItemLocation::DefinitionName");
        if (!location.isEmpty())
            retn->setLocation(location.locationName().toStdString());

        // dateStart and dateEnd are common fields for all CComponents, but those are set
        // separately for each item type here, because there are independent time ranges
        // defined for each type in the Qt Mobility API.

        // (TODO: Status ?)

        // (TODO: Until ?)

        // GUid
        if (!item->guid().isEmpty())
            retn->setGUid(item->guid().toStdString());
        else {
            retn->setGUid(randomGuid().toStdString()); // no GUID was set, generate a random GUID
            qDebug() << "GUID " << QString::fromStdString(retn->getGUid()) << " given to item with label " << item->displayLabel();
        }

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
    }

    return retn;
}

CRecurrence* OrganizerItemTransform::createCRecurrence(const QOrganizerItem* item)
{
    // Only the event and todo types contain recurrence information
    if (item->type() == QOrganizerItemType::TypeEvent) {
        m_recTransformer.beginTransformToCrecurrence();
        const QOrganizerEvent *event = static_cast<const QOrganizerEvent *>( item );

        // Add recurrence rules
        QList<QOrganizerItemRecurrenceRule> recurrenceRules = event->recurrenceRules();
        foreach (QOrganizerItemRecurrenceRule rule, recurrenceRules)
            m_recTransformer.addQOrganizerItemRecurrenceRule(rule);

        // Add exception rules
        QList<QOrganizerItemRecurrenceRule> exceptionRules = event->exceptionRules();
        foreach (QOrganizerItemRecurrenceRule rule, exceptionRules)
            m_recTransformer.addQOrganizerItemExceptionRule(rule);

        // Add recurrence dates
        QList<QDate> recurrenceDates = event->recurrenceDates();
        foreach (QDate recDate, recurrenceDates)
            m_recTransformer.addQOrganizerItemRecurrenceDate(recDate);

        // Add exception dates
        QList<QDate> exceptionDates = event->exceptionDates();
        foreach (QDate exceptionDate, exceptionDates)
            m_recTransformer.addQOrganizerItemExceptionDate(exceptionDate);

        return m_recTransformer.crecurrence(); // TODO: This may need error handling?
    }
    else if (item->type() == QOrganizerItemType::TypeTodo) {
        const QOrganizerTodo *todo = static_cast<const QOrganizerTodo *>(item);
        QList<QDate> recurrenceDates = todo->recurrenceDates();
        QList<QOrganizerItemRecurrenceRule> recurrenceRules = todo->recurrenceRules();

        // TODO
    }

    return 0; // no recurrence information for this item type
}

QOrganizerItemManager::Error OrganizerItemTransform::calErrorToManagerError(int calError) const
{
    switch(calError) {
        case CALENDAR_OPERATION_SUCCESSFUL:
            return QOrganizerItemManager::NoError;

        case CALENDAR_SYSTEM_ERROR:
        case CALENDAR_DATABASE_ERROR:
        // case CALENDAR_DBUS_ERROR: // CALENDAR_DBUS_ERROR and CALENDAR_FILE_ERROR has the same value 3
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
            return QOrganizerItemManager::UnspecifiedError;

        case CALENDAR_MEMORY_ERROR:
            return QOrganizerItemManager::OutOfMemoryError;

        case CALENDAR_FILE_ERROR:
        case CALENDAR_DOESNOT_EXISTS:
        case CALENDAR_NONE_INDB:
        case NO_DUPLICATE_ITEM:
        case CALENDAR_FETCH_NOITEMS:
            return QOrganizerItemManager::DoesNotExistError;

        case CALENDAR_DISK_FULL:
        case CALENDAR_DB_FULL:
            return QOrganizerItemManager::LimitReachedError;

        case CALENDAR_INVALID_ARG_ERROR:
            return QOrganizerItemManager::BadArgumentError;

        case CALENDAR_ALREADY_EXISTS:
        case CALENDAR_ENTRY_DUPLICATED:
            return QOrganizerItemManager::AlreadyExistsError;

        case CALENDAR_CANNOT_BE_DELETED:
        case EXT_ITEM_RETAINED:
        case LOCAL_ITEM_RETAINED:
            return QOrganizerItemManager::PermissionsError;

        case CALENDAR_DB_LOCKED:
            return QOrganizerItemManager::LockedError;

        case CALENDAR_ICS_COMPONENT_INVALID:
        case CALENDAR_BATCH_ADD_INVALID:
            return QOrganizerItemManager::InvalidDetailError;

        default:
            return QOrganizerItemManager::UnspecifiedError;
    }
}

QString OrganizerItemTransform::randomGuid() const
{
    QUuid guid = QUuid::createUuid();
    return guid.toString();
}
