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
#include "organizereventtimerangetransform.h"
#include "qorganizereventtimerange.h"

void OrganizerEventTimeRangeTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    if (item->type() == QOrganizerItemType::TypeEvent) // type has already been converted
    {
        TCalTime startTime = entry.StartTimeL();
        TCalTime endTime = entry.EndTimeL();

        QOrganizerEventTimeRange range;
        if (startTime.TimeUtcL() != Time::NullTTime())
            range.setStartDateTime(toQDateTimeL(startTime));

        // Check if the end time is defined and if the end time is different to
        // start time. Effectively this means that if a QtMobility Organizer API
        // client defines an end time that is exactly the same as start time, the
        // end time is lost.
        if (endTime.TimeUtcL() != Time::NullTTime()
            && endTime.TimeUtcL() != startTime.TimeUtcL())
            range.setEndDateTime(toQDateTimeL(endTime));

        item->saveDetail(&range);
    }
}

void OrganizerEventTimeRangeTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    if (item.type() == QOrganizerItemType::TypeEvent)
    {
        QOrganizerEventTimeRange range = item.detail<QOrganizerEventTimeRange>();
        // Symbian calendar server makes the client process panic in case there
        // is no start time for an event. As a work-around let's check the
        // parameters and leave in that case.
        // TODO: A VEVENT with empty start time is allowed by the iCalendar
        // specification (RFC2445); file a bug report against symbian calendar
        // server in 10.1 or later platforms.
        if (!range.isEmpty() && range.startDateTime().isValid())
            entry->SetStartAndEndTimeL(toTCalTimeL(range.startDateTime()), toTCalTimeL(range.endDateTime()));
        else
            User::Leave(KErrArgument);        
    }
}

QString OrganizerEventTimeRangeTransform::detailDefinitionName()
{
    return QOrganizerEventTimeRange::DefinitionName;
}
