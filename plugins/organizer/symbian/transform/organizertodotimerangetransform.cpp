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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "organizertodotimerangetransform.h"
#include "qorganizertodotime.h"

void OrganizerTodoTimeTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    if(item->type() == QOrganizerItemType::TypeTodo)
    {
        TCalTime startTime = entry.StartTimeL();
        TCalTime endTime = entry.EndTimeL();

        QOrganizerTodoTime range;
        if (startTime.TimeUtcL() != Time::NullTTime())
            range.setStartDateTime(toQDateTimeL(startTime));
        if (endTime.TimeUtcL() != Time::NullTTime())
            range.setDueDateTime(toQDateTimeL(endTime));

        if (!range.isEmpty())
            item->saveDetail(&range);
    }
}

void OrganizerTodoTimeTransform::transformToDetailL(const CCalInstance& instance, QOrganizerItem *itemOccurrence)
{
    if(itemOccurrence->type() == QOrganizerItemType::TypeTodoOccurrence)
    {
        TCalTime startTime = instance.StartTimeL();
        TCalTime endTime = instance.EndTimeL();

        QOrganizerTodoTime range;
        if (startTime.TimeUtcL() != Time::NullTTime())
            range.setStartDateTime(toQDateTimeL(startTime));
        if (endTime.TimeUtcL() != Time::NullTTime())
            range.setDueDateTime(toQDateTimeL(endTime));
        
        if (!range.isEmpty())
            itemOccurrence->saveDetail(&range);
    }
}

void OrganizerTodoTimeTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    if(item.type() == QOrganizerItemType::TypeTodo || item.type() == QOrganizerItemType::TypeTodoOccurrence)
    {
        QOrganizerTodoTime range = item.detail<QOrganizerTodoTime>();
        
        // Nothing to do?
        if (range.isEmpty())
            return;
        
        // On some platforms symbian calendar server does not leave
        // when start date is more than due date. So make the check here.
        if (range.startDateTime().isValid() && range.dueDateTime().isValid()) {
            if (range.startDateTime() > range.dueDateTime())
                User::Leave(KErrArgument);
        }        
            
        entry->SetStartAndEndTimeL(toTCalTimeL(range.startDateTime()), toTCalTimeL(range.dueDateTime()));
    }
}

QString OrganizerTodoTimeTransform::detailDefinitionName()
{
    return QOrganizerTodoTime::DefinitionName;    
}
