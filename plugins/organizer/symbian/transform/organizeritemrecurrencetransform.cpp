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

    TCalRRule calRRule;
    entry.GetRRuleL(calRRule);
    //TODO: to put invalid rule check once remove recurrence apis are available.
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

void OrganizerItemRecurrenceTransform::transformToDetailL(const CCalInstance& instance, QOrganizerItem *itemInstance)
{
    Q_UNUSED(instance);
    Q_UNUSED(itemInstance);
    // There is no recurrence rules with item instances
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

    if (recurrence.recurrenceRules().count()) {
        entry->SetRRuleL(toCalRRuleL(recurrence.recurrenceRules(), timerange.startDateTime()));
    } else {
        entry->ClearRepeatingPropertiesL();
    }

    if (recurrence.recurrenceDates().count()) {
        RArray<TCalTime> calRDates;
        toTCalTimesL(recurrence.recurrenceDates(), calRDates);
        entry->SetRDatesL(calRDates);
    }

    if (recurrence.exceptionDates().count()) {
        RArray<TCalTime> calExDates;
        toTCalTimesL(recurrence.exceptionDates(), calExDates);
        entry->SetExceptionDatesL(calExDates);
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
    QList<QOrganizerItemRecurrenceRule> rrules;
    QOrganizerItemRecurrenceRule rrule;

    if (calrrule.Type() == TCalRRule::EWeekly) {
        rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
        RArray<TDay> byDay;
        CleanupClosePushL(byDay);
        calrrule.GetByDayL(byDay);
        QList<Qt::DayOfWeek> daysOfWeek;
        for (TInt i(0); i < byDay.Count(); i++) {
            daysOfWeek.append(toDayOfWeekL(byDay[i]));
        }
        CleanupStack::PopAndDestroy(&byDay);

        //Set start of week
        rrule.setDaysOfWeek(daysOfWeek);
        if (calrrule.WkSt()) {
            Qt::DayOfWeek startOfWeek = toDayOfWeekL(calrrule.WkSt());
            if(Qt::Monday != startOfWeek)
               rrule.setWeekStart(startOfWeek);
        }
    } else if(calrrule.Type() == TCalRRule::EMonthly) {
        rrule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);

        // by month day
        RArray<TInt> byMonthDay;
        CleanupClosePushL(byMonthDay);
        calrrule.GetByMonthDayL(byMonthDay);
        QList<int> daysOfMonth;
        for (TInt i(0); i < byMonthDay.Count(); i++) {
            // symbian calendar server uses 0-based month days
            daysOfMonth.append(byMonthDay[i] + 1);
        }
        CleanupStack::PopAndDestroy(&byMonthDay);
        if (daysOfMonth.count()) {
            rrule.setDaysOfMonth(daysOfMonth);
        }

        // Convert TDayOfMonth array into days-of-week and positions
        RArray<TCalRRule::TDayOfMonth> tDayOfMonthArray;
        CleanupClosePushL(tDayOfMonthArray);
        calrrule.GetByDayL(tDayOfMonthArray);
        for (TInt i(0); i < tDayOfMonthArray.Count(); i++) {
            TCalRRule::TDayOfMonth dayOfMonth = tDayOfMonthArray[i];
            // Append the day-of-week part of the day-of-month to the
            // recurrence rule if it does not exist already (do not add
            // duplicates)
            QList<Qt::DayOfWeek> daysOfWeek = rrule.daysOfWeek();
            if (!daysOfWeek.contains(toDayOfWeekL(dayOfMonth.Day()))) {
                daysOfWeek.append(toDayOfWeekL(dayOfMonth.Day()));
                rrule.setDaysOfWeek(daysOfWeek);
            }
            // Append the position part of day-of-month to the recurrence rule
            QList<int> positions = rrule.positions();
            positions.append(dayOfMonth.WeekInMonth());
            rrule.setPositions(positions);
        }
        CleanupStack::PopAndDestroy(&tDayOfMonthArray);
    } else if(calrrule.Type() == TCalRRule::EYearly) {
        rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);

        // Convert TDayOfMonth array into days-of-week and positions
        RArray<TCalRRule::TDayOfMonth> daysOfMonth;
        CleanupClosePushL(daysOfMonth);
        calrrule.GetByDayL(daysOfMonth);
        for (TInt i(0); i < daysOfMonth.Count(); i++) {
            TCalRRule::TDayOfMonth dayOfMonth = daysOfMonth[i];
            // Append the day-of-week part of the day-of-month to the
            // recurrence rule if it does not exist already (do not add
            // duplicates)
            QList<Qt::DayOfWeek> daysOfWeek = rrule.daysOfWeek();
            if (!daysOfWeek.contains(toDayOfWeekL(dayOfMonth.Day()))) {
                daysOfWeek.append(toDayOfWeekL(dayOfMonth.Day()));
                rrule.setDaysOfWeek(daysOfWeek);
            }
            // Append the position part of day-of-month to the recurrence rule
            QList<int> positions = rrule.positions();
            positions.append(dayOfMonth.WeekInMonth());
            rrule.setPositions(positions);
        }
        CleanupStack::PopAndDestroy(&daysOfMonth);
        // Convert by-month into QOrganizerItemRecurrenceRule months
        RArray<TMonth> byMonth;
        CleanupClosePushL(byMonth);
        calrrule.GetByMonthL(byMonth);
        QList<QOrganizerItemRecurrenceRule::Month> months;
        for (TInt i(0); i < byMonth.Count(); i++) {
            months.append(toMonthL(byMonth[i]));
        }
        CleanupStack::PopAndDestroy(&byMonth);
        if (months.count()) {
            rrule.setMonths(months);
        }
    } else if (calrrule.Type() == TCalRRule::EDaily) {
        rrule.setFrequency(QOrganizerItemRecurrenceRule::Daily);
    } else {
        return rrules;
    }

    // Count has higher priority than end date
    if (calrrule.Count()) {
        rrule.setCount(calrrule.Count());
    } else if (calrrule.Until().TimeUtcL() != Time::NullTTime()) {
        rrule.setEndDate(toQDateTimeL(calrrule.Until()).date());
    }
    // Set the interval
    rrule.setInterval(calrrule.Interval());

    rrules.append(rrule);
    return rrules;
}

TCalRRule OrganizerItemRecurrenceTransform::toCalRRuleL(QList<QOrganizerItemRecurrenceRule> recrules, QDateTime startDateTime) const
{
    TCalRRule calRule;
    if (recrules.count()) {
        // TODO: only taking the first available into account
        QOrganizerItemRecurrenceRule rrule = recrules[0];


        // Set the start date
        calRule.SetDtStart(toTCalTimeL(startDateTime));

        // Set the count else the end date
        if (rrule.count()) {
            calRule.SetCount(rrule.count());
        } else if (rrule.endDate().isValid()) {
            if (rrule.endDate() < startDateTime.date()) {
                // End date before start date!
                User::Leave(KErrArgument);
            }
            calRule.SetUntil(toTCalTimeL(rrule.endDate()));
        }
        // Set the interval(default is 1)
        calRule.SetInterval(rrule.interval());

        if (rrule.frequency() == QOrganizerItemRecurrenceRule::Weekly) {
            calRule.SetType(TCalRRule::EWeekly);

            // TODO: how about daysOfMonth, daysOfYear and so on?
            RArray<TDay> byDay;
            CleanupClosePushL(byDay);
            if (rrule.daysOfWeek().isEmpty()) {
                // Note: Symbian calendar server does not allow storing weekly
                // recurrence without "by day" data! To overcome this issue we use
                // the start date time to determine the "by day" if the client did
                // not specify "days of week".
                QList<Qt::DayOfWeek> daysOfWeek;
                int dayOfWeek = startDateTime.date().dayOfWeek();
                Qt::DayOfWeek day = Qt::DayOfWeek(dayOfWeek);
                byDay.AppendL(toTDayL(day));
            } else {
                foreach (Qt::DayOfWeek dayOfWeek, rrule.daysOfWeek()) {
                    byDay.AppendL(toTDayL(dayOfWeek));
                }
            }
            calRule.SetByDay(byDay);
            CleanupStack::PopAndDestroy(&byDay);
            // Set start of the week
            if (rrule.weekStart()!= (Qt::Monday)) {
                calRule.SetWkSt(toTDayL(rrule.weekStart()));
            }
        } else if (rrule.frequency() == QOrganizerItemRecurrenceRule::Monthly) {
            calRule.SetType(TCalRRule::EMonthly);
            // TODO: You cannot set "by day" for a TCalRRule of type EMonthly,
            // how about days of year?
            RArray<TInt> byMonthDay;
            CleanupClosePushL(byMonthDay);
            if (!rrule.daysOfMonth().isEmpty()) {
                foreach (int dayOfMonth, rrule.daysOfMonth()) {
                    // symbian calendar server uses 0-based month days
                    byMonthDay.AppendL(dayOfMonth - 1);
                }
            } else if (!rrule.daysOfWeek().isEmpty()) {
                // Convert days-of-week and positions into TDayOfMonth array and
                // store it to the by-day of the TCalRRule
                QList<int> positions = rrule.positions();
                if (positions.isEmpty()) {
                    // If the position is not available, the default is to use
                    // all positions (according to the RFC-2445)
                    // Note: this will cause the following issue for Qt
                    // Organizer API clients: If an item does not have the
                    // positions field it will appear during save
                    positions << 1 << 2 << 3 << 4 << -1;
                }
                RArray<TCalRRule::TDayOfMonth> dayArray;
                CleanupClosePushL(dayArray);
                foreach (Qt::DayOfWeek dayOfWeek, rrule.daysOfWeek()) {
                    foreach (int position, positions) {
                        TCalRRule::TDayOfMonth dayOfMonth(toTDayL(dayOfWeek), position);
                        dayArray.AppendL(dayOfMonth);
                    }
                }
                if (dayArray.Count()) {
                    calRule.SetByDay(dayArray);
                }
                CleanupStack::PopAndDestroy(&dayArray);
            } else {
                int day = startDateTime.date().day();
                byMonthDay.AppendL(day - 1);
            }

            if (byMonthDay.Count()) {
                calRule.SetByMonthDay(byMonthDay);
            }
            CleanupStack::PopAndDestroy(&byMonthDay);
        } else if (rrule.frequency() == QOrganizerItemRecurrenceRule::Yearly) {
            calRule.SetType(TCalRRule::EYearly);

            // Convert days-of-week and positions into TDayOfMonth array and
            // store it to the by-day of the TCalRRule
            if (!rrule.daysOfWeek().isEmpty()) {
                QList<int> positions = rrule.positions();
                if (positions.count() != 1) {
                    // If the position is not available, the default is to use
                    // all positions (according to the RFC-2445)
                    // Symbian calendar server supports storing only one
                    // position for a yearly recurring event (see the
                    // documentation of TCalRRule::SetByDay), so we must give
                    // an error.
                    User::Leave(KErrArgument);
                }
                RArray<TCalRRule::TDayOfMonth> dayArray;
                CleanupClosePushL(dayArray);
                foreach (Qt::DayOfWeek dayOfWeek, rrule.daysOfWeek()) {
                    foreach (int position, positions) {
                        TCalRRule::TDayOfMonth dayOfMonth(toTDayL(dayOfWeek), position);
                        dayArray.AppendL(dayOfMonth);
                    }
                }
                if (dayArray.Count()) {
                    calRule.SetByDay(dayArray);
                }
                CleanupStack::PopAndDestroy(&dayArray);
            }

            // Convert months into TMonth array and set it as the "by-month" of
            // the TCalRRule
            if (rrule.months().count()) {
                RArray<TMonth> byMonth;
                CleanupClosePushL(byMonth);
                foreach (QOrganizerItemRecurrenceRule::Month month, rrule.months()) {
                    byMonth.AppendL(toTMonthL(month));
                }
                calRule.SetByMonth(byMonth);
                CleanupStack::PopAndDestroy(&byMonth);
            }
        // Converting the daily recurrance rule with interval and occurance
        } else if (rrule.frequency() == QOrganizerItemRecurrenceRule::Daily) {
            calRule.SetType(TCalRRule::EDaily);
        } else {
            User::Leave(KErrNotSupported);
        }
    }
    return calRule;
}

Qt::DayOfWeek OrganizerItemRecurrenceTransform::toDayOfWeekL(TDay day) const
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
        User::Leave(KErrArgument);
    }
    return Qt::Monday; // never happens
}

TDay OrganizerItemRecurrenceTransform::toTDayL(Qt::DayOfWeek dayOfWeek) const
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
        User::Leave(KErrArgument);
    }
    return EMonday; // never happens
}

QOrganizerItemRecurrenceRule::Month OrganizerItemRecurrenceTransform::toMonthL(TMonth month) const
{
    switch (month) {
    case EJanuary:
        return QOrganizerItemRecurrenceRule::January;
    case EFebruary:
        return QOrganizerItemRecurrenceRule::February;
    case EMarch:
        return QOrganizerItemRecurrenceRule::March;
    case EApril:
        return QOrganizerItemRecurrenceRule::April;
    case EMay:
        return QOrganizerItemRecurrenceRule::May;
    case EJune:
        return QOrganizerItemRecurrenceRule::June;
    case EJuly:
        return QOrganizerItemRecurrenceRule::July;
    case EAugust:
        return QOrganizerItemRecurrenceRule::August;
    case ESeptember:
        return QOrganizerItemRecurrenceRule::September;
    case EOctober:
        return QOrganizerItemRecurrenceRule::October;
    case ENovember:
        return QOrganizerItemRecurrenceRule::November;
    case EDecember:
        return QOrganizerItemRecurrenceRule::December;
    default:
        // Should never happen
        User::Leave(KErrArgument);
    }
    return QOrganizerItemRecurrenceRule::January; // never happens
}

TMonth OrganizerItemRecurrenceTransform::toTMonthL(QOrganizerItemRecurrenceRule::Month month) const
{
    switch (month) {
    case QOrganizerItemRecurrenceRule::January:
        return EJanuary;
    case QOrganizerItemRecurrenceRule::February:
        return EFebruary;
    case QOrganizerItemRecurrenceRule::March:
        return EMarch;
    case QOrganizerItemRecurrenceRule::April:
        return EApril;
    case QOrganizerItemRecurrenceRule::May:
        return EMay;
    case QOrganizerItemRecurrenceRule::June:
        return EJune;
    case QOrganizerItemRecurrenceRule::July:
        return EJuly;
    case QOrganizerItemRecurrenceRule::August:
        return EAugust;
    case QOrganizerItemRecurrenceRule::September:
        return ESeptember;
    case QOrganizerItemRecurrenceRule::October:
        return EOctober;
    case QOrganizerItemRecurrenceRule::November:
        return ENovember;
    case QOrganizerItemRecurrenceRule::December:
        return EDecember;
    default:
        // Should never happen
        User::Leave(KErrArgument);
    }
    return EJanuary; // never happens
}
