/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <calalarm.h>

#include "qorganizeritemreminder.h"
#include "qorganizeritemaudiblereminder.h"
#include "qorganizeritemvisualreminder.h"
#include "qorganizeritememailreminder.h"
#include "qorganizereventtime.h"
#include "qorganizertodotime.h"
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
        reminder.setSecondsBeforeStart(offsetSeconds);
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

    if (reminder.secondsBeforeStart() < 0) {
        // Reminder time must be earlier than or equal to the start time
        User::Leave(KErrArgument);
    }

    // Get the time offset in minutes
    int timeOffset = reminder.secondsBeforeStart() / secondsInOneMinute;

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
