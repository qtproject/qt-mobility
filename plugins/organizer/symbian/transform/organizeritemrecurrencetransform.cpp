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
#include "organizeritemrecurrencetransform.h"
#include "qorganizeritemrecurrence.h"
#include "qorganizereventtimerange.h"
#include <calrrule.h>


void OrganizerItemRecurrenceTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    // *** Repeat rules / RDate / ExDate Methods ***
    QOrganizerItemRecurrence recurrence;

    // TODO: do not set QOrganizerItemRecurrence if there are none

    TCalRRule calRRule;
    entry.GetRRuleL(calRRule);
    recurrence.setRecurrenceRules(toItemRecurrenceRulesL(calRRule));

    RArray<TCalTime> calRDateList;
    entry.GetRDatesL(calRDateList);
    if (calRDateList.Count()) {
        recurrence.setRecurrenceDates(toQDatesL(calRDateList));
    }

    RArray<TCalTime> calExDateList;
    entry.GetExceptionDatesL(calExDateList);
    if (calExDateList.Count()) {
        recurrence.setExceptionDates(toQDatesL(calExDateList));
    }

    // TODO: exceptionRules
    // There is no native support for this.

    item->saveDetail(&recurrence);
}

void OrganizerItemRecurrenceTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    // *** Repeat rules / RDate / ExDate Methods ***
    QOrganizerItemRecurrence recurrence = item.detail<QOrganizerItemRecurrence>();
    if (recurrence.isEmpty())
        return;

    // TODO: can we have recurrenceRules and recurrenceDates at the same time?

    // TODO: Also other item types may have a recurrence item
    QOrganizerEventTimeRange timerange = item.detail(QOrganizerEventTimeRange::DefinitionName);

    if (recurrence.recurrenceRules().count())
        entry->SetRRuleL(toCalRRuleL(recurrence.recurrenceRules(), timerange.startDateTime()));

    if (recurrence.recurrenceDates().count()) {
        RArray<TCalTime> calRDates;
        toTCalTimesL(recurrence.recurrenceDates(), calRDates);
        entry->SetRDatesL(calRDates);
    }

    if (recurrence.exceptionDates().count()) {
        RArray<TCalTime> calExDates;
        toTCalTimesL(recurrence.exceptionDates(), calExDates);
        entry->SetRDatesL(calExDates);
    }

    //  TODO: what about recurrence.exceptionRules()? there is no match in native API.
}

QString OrganizerItemRecurrenceTransform::detailDefinitionName()
{
    return QOrganizerItemRecurrence::DefinitionName;    
}

void OrganizerItemRecurrenceTransform::toTCalTimesL(const QList<QDate> &dates, RArray<TCalTime> &calDates) const
{
    foreach (QDate date, dates) {
        QDateTime dateTime(date);
        TCalTime calTime = toTCalTimeL(dateTime);
        calDates.Append(calTime);
    }
}

QList<QDate> OrganizerItemRecurrenceTransform::toQDatesL(const RArray<TCalTime> &calDates) const
{
    QList<QDate> dates;
    int count = calDates.Count();
    for (int i=0; i<count; i++) {
        QDateTime dateTime = toQDateTimeL(calDates[i]);
        dates << dateTime.date();
    }
    return dates;
}

QList<QOrganizerItemRecurrenceRule> OrganizerItemRecurrenceTransform::toItemRecurrenceRulesL(const TCalRRule &calrrule) const
{
    // TODO: only conversion for "day of week" is implemented

    QList<Qt::DayOfWeek> daysOfWeek;
    RArray<TDay> byDay;
    CleanupClosePushL(byDay);
    calrrule.GetByDayL(byDay);
    for (TInt i(0); i < byDay.Count(); i++) {
        daysOfWeek.append(toDayOfWeek(byDay[i]));
    }
    CleanupStack::PopAndDestroy(&byDay);
    
    QOrganizerItemRecurrenceRule rrule;
    rrule.setDaysOfWeek(daysOfWeek);

    // Count has higher priority than end date
    if (calrrule.Count()) {
        rrule.setCount(calrrule.Count());
    } else if (calrrule.Until().TimeUtcL() != Time::NullTTime()) {
        rrule.setEndDate(toQDateTimeL(calrrule.Until()).date());
    }

    //TODO: rrule.setInterval(calrrule.Interval());
    // TODO: other frequencies?
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);

    QList<QOrganizerItemRecurrenceRule> rrules;
    rrules.append(rrule);
    return rrules;
}

TCalRRule OrganizerItemRecurrenceTransform::toCalRRuleL(QList<QOrganizerItemRecurrenceRule> recrules, QDateTime startDateTime) const
{
    TCalRRule calRule;
    if (recrules.count()) {
        // TODO: only taking the first available into account
        QOrganizerItemRecurrenceRule rrule = recrules[0];

        if (rrule.frequency() == QOrganizerItemRecurrenceRule::Weekly) {
            calRule.SetType(TCalRRule::EWeekly);
            calRule.SetDtStart(toTCalTimeL(startDateTime));
            if (rrule.count()) {
                calRule.SetCount(rrule.count());
            }
            if (rrule.endDate().isValid()) {
                if (rrule.endDate() < startDateTime.date()) {
                    // End date before start date!
                    User::Leave(KErrArgument);
                }
                calRule.SetUntil(toTCalTimeL(rrule.endDate()));
            }

            RArray<TDay> byDay;
            // TODO: how about daysOfMonth, daysOfYear and so on?
            foreach (Qt::DayOfWeek dayOfWeek, rrule.daysOfWeek()) {
                byDay.AppendL(toTDay(dayOfWeek));
            }

            // TODO: Symbian calendar server does not allow storing weekly
            // recurrence without "by day" data! This means that a client
            // must set "days of week" for a QOrganizerItemRecurrenceRule
            if (byDay.Count()){
                calRule.SetByDay(byDay);
            }
            byDay.Close();

            //TODO: ? calRule.SetWkSt(EMonday);
            //TODO: calRule.SetInterval(rrule.interval());
        } else {
            // TODO: implement the rest of the frequencies
            User::Leave(KErrNotReady);
        }
    }

    return calRule;
}

Qt::DayOfWeek OrganizerItemRecurrenceTransform::toDayOfWeek(TDay day) const
{
    switch (day) {
    case EMonday:
        return Qt::Monday;
    case ETuesday:
        return Qt::Tuesday;
    case EWednesday:
        return Qt::Wednesday;
    case EThursday:
        return Qt::Thursday;
    case EFriday:
        return Qt::Friday;
    case ESaturday:
        return Qt::Saturday;
    case ESunday:
        return Qt::Sunday;
    default:
        // Should never happen
        return Qt::Monday;
    }
}

TDay OrganizerItemRecurrenceTransform::toTDay(Qt::DayOfWeek dayOfWeek) const
{
    switch (dayOfWeek) {
    case Qt::Monday:
        return EMonday;
    case Qt::Tuesday:
        return ETuesday;
    case Qt::Wednesday:
        return EWednesday;
    case Qt::Thursday:
        return EThursday;
    case Qt::Friday:
        return EFriday;
    case Qt::Saturday:
        return ESaturday;
    case Qt::Sunday:
        return ESunday;
    default:
        // Should never happen
        return EMonday;
    }
}
