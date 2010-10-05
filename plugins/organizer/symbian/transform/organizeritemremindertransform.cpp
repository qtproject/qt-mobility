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

#include <calalarm.h>

#include "qorganizeritemreminder.h"
#include "qorganizeritemaudiblereminder.h"
#include "qorganizeritemvisualreminder.h"
#include "qorganizeritememailreminder.h"
#include "qorganizereventtimerange.h"
#include "qorganizertodotimerange.h"
#include "organizeritemremindertransform.h"

const int secondsInOneMinute = 60;

void OrganizerItemReminderTransform::modifyBaseSchemaDefinitions(QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > &schemaDefs) const
{
    // Find reminder details
    foreach (QString itemTypeName, schemaDefs.keys()) {
        if (itemTypeName == QOrganizerItemType::TypeNote) {
            schemaDefs[itemTypeName].remove(QOrganizerItemReminder::DefinitionName);
            continue;
        }
        QMap<QString, QOrganizerItemDetailDefinition> details = schemaDefs.value(itemTypeName);
        if (details.contains(QOrganizerItemReminder::DefinitionName)) {
            // Symbian alarm subsystem does not support setting the count or delay values
            // The behaviour for these are pre-determined and cannot change per reminder
            QOrganizerItemDetailDefinition d = details.value(QOrganizerItemReminder::DefinitionName);
            d.removeField(QOrganizerItemReminder::FieldRepetitionCount);
            d.removeField(QOrganizerItemReminder::FieldRepetitionDelay);
            schemaDefs[itemTypeName].insert(d.name(), d);
        }
    }
}

void OrganizerItemReminderTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    CCalAlarm *alarm = entry.AlarmL();
    if (alarm) {
        CleanupStack::PushL(alarm);
        QOrganizerItemReminder reminder;
        int offsetSeconds = alarm->TimeOffset().Int() * secondsInOneMinute;
        // Set both the offset and dateTime values for the reminder detail
        reminder.setTimeDelta(offsetSeconds);
        QDateTime startTime = toQDateTimeL(entry.StartTimeL());
        QDateTime reminderTime = startTime.addSecs(-offsetSeconds);
        reminder.setDateTime(reminderTime);
        item->saveDetail(&reminder);
        CleanupStack::PopAndDestroy(alarm);
    }
}

void OrganizerItemReminderTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    if (QOrganizerItemType::TypeNote == item.type()) {
        return;
    }
    QOrganizerItemReminder reminder = item.detail<QOrganizerItemReminder>();

    if (reminder.isEmpty()) {
		// Remove the alarm from the entry
		entry->SetAlarmL(0);
        return;
	}

    QDateTime reminderDateTime = reminder.dateTime();
    int timeOffset(0);

    // If the reminder detail has a valid startDateTime, use it
    if (!reminderDateTime.isNull()) {
        QDateTime startDateTime;
        if(item.type() == QOrganizerItemType::TypeEvent ||
           item.type() == QOrganizerItemType::TypeEventOccurrence) {
            QOrganizerEventTimeRange timeRange = item.detail<QOrganizerEventTimeRange>();
            startDateTime = timeRange.startDateTime();
        } else if (item.type() == QOrganizerItemType::TypeTodo ||
                   item.type() == QOrganizerItemType::TypeTodoOccurrence) {
            QOrganizerTodoTimeRange timeRange = item.detail<QOrganizerTodoTimeRange>();
            startDateTime = timeRange.startDateTime();
        }
        if (reminderDateTime > startDateTime) {
            // Reminder time must be less than or equal to the start time
            User::Leave(KErrArgument);
        }
        // Get the time offset in minutes
        timeOffset = reminderDateTime.secsTo(startDateTime) / secondsInOneMinute;
    }

    // If there is a valid reminder delta, use it
    if (reminder.variantValues().contains(QOrganizerItemReminder::FieldTimeDelta)) {

        if (timeOffset) {
            // If both startDateTime and delta are defined, they must match
            if (timeOffset != reminder.timeDelta() / secondsInOneMinute)
                User::Leave(KErrArgument);
        } else {
            // Convert delta to minutes
            timeOffset = reminder.timeDelta() / secondsInOneMinute;
        }
    }

    // Now add the alarm details in the entry
    CCalAlarm *alarm = CCalAlarm::NewL();
    CleanupStack::PushL(alarm);
    alarm->SetTimeOffset(timeOffset);
    entry->SetAlarmL(alarm);
    CleanupStack::PopAndDestroy(alarm);
}

QString OrganizerItemReminderTransform::detailDefinitionName()
{
    return QOrganizerItemReminder::DefinitionName;
}
