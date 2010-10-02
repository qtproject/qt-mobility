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
#include "qorganizertodotimerange.h"

#include <calrrule.h>

void OrganizerItemRecurrenceTransform::modifyBaseSchemaDefinitions(QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > &schemaDefs) const
{
    // Find recurrence details
    foreach (QString itemTypeName, schemaDefs.keys()) {
        QMap<QString, QOrganizerItemDetailDefinition> details = schemaDefs.value(itemTypeName);
        if (details.contains(QOrganizerItemRecurrence::DefinitionName)) {
            // Remove not supported details and replace the original detail definition
            QOrganizerItemDetailDefinition d = details.value(QOrganizerItemRecurrence::DefinitionName);
            d.removeField(QOrganizerItemRecurrence::FieldExceptionRules);
            schemaDefs[itemTypeName].insert(d.name(), d);
        }
    }
}

void OrganizerItemRecurrenceTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    // *** Repeat rules / RDate / ExDate Methods ***
    QOrganizerItemRecurrence recurrence;

    TCalRRule calRRule;
    entry.GetRRuleL(calRRule);
    if (calRRule.Type() != TCalRRule::EInvalid)
        recurrence.setRecurrenceRules(toItemRecurrenceRulesL(calRRule));

    RArray<TCalTime> calRDateList;
    CleanupClosePushL(calRDateList);
    entry.GetRDatesL(calRDateList);
    if (calRDateList.Count())
        recurrence.setRecurrenceDates(toQDatesL(calRDateList));
    CleanupStack::PopAndDestroy(&calRDateList);

    RArray<TCalTime> calExDateList;
    CleanupClosePushL(calExDateList);
    entry.GetExceptionDatesL(calExDateList);
    if (calExDateList.Count())
        recurrence.setExceptionDates(toQDatesL(calExDateList));
    CleanupStack::PopAndDestroy(&calExDateList);

    // TODO: exceptionRules
    // There is no native support for this.

    if (!recurrence.isEmpty())
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
    if (QOrganizerItemType::TypeNote == item.type()) {
        // Notes do not support the reccurrence rules
        return;
    }
    // Clear all repeating properties from this entry.
    // This is needed for removing recurrence rules from an existing entry.
    entry->ClearRepeatingPropertiesL();
    
    // *** Repeat rules / RDate / ExDate Methods ***
    QOrganizerItemRecurrence recurrence = item.detail<QOrganizerItemRecurrence>();
    if (recurrence.isEmpty())
        return;

    // TODO: can we have recurrenceRules and recurrenceDates at the same time?

    // TODO: Also other item types may have a recurrence item
    QDateTime startDateTime;
    if (QOrganizerItemType::TypeTodo == item.type()) {
        startDateTime = ((QOrganizerTodoTimeRange)
            (item.detail(QOrganizerTodoTimeRange::DefinitionName))).
            startDateTime();
    } else { 
        startDateTime = ((QOrganizerEventTimeRange)
            item.detail(QOrganizerEventTimeRange::DefinitionName)).
            startDateTime();
    }

    if (recurrence.recurrenceRules().count())
        entry->SetRRuleL(toCalRRuleL(recurrence.recurrenceRules(), startDateTime));

    if (recurrence.recurrenceDates().count()) {
        RArray<TCalTime> calRDates;
        CleanupClosePushL(calRDates);
        toTCalTimesL(recurrence.recurrenceDates(), calRDates);
        entry->SetRDatesL(calRDates);
        CleanupStack::PopAndDestroy(&calRDates);
    }

    if (recurrence.exceptionDates().count()) {
        RArray<TCalTime> calExDates;
        CleanupClosePushL(calExDates);
        toTCalTimesL(recurrence.exceptionDates(), calExDates);
        entry->SetExceptionDatesL(calExDates);
        CleanupStack::PopAndDestroy(&calExDates);
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

    rrule.setFrequency(toFrequencyL(calrrule.Type()));

    // Convert by-day into OrganizerItemRecurrenceRule days-of-week
    RArray<TDay> byDay;
    CleanupClosePushL(byDay);
    calrrule.GetByDayL(byDay);
    QList<Qt::DayOfWeek> daysOfWeek;
    for (TInt i(0); i < byDay.Count(); i++) {
        daysOfWeek.append(toDayOfWeekL(byDay[i]));
    }
    CleanupStack::PopAndDestroy(&byDay);
    if (daysOfWeek.count()) {
        rrule.setDaysOfWeek(daysOfWeek);
    }

    // Convert by-month-day into OrganizerItemRecurrenceRule days-of-month
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

    // Convert TDayOfMonth array into OrganizerItemRecurrenceRule
    // days-of-week and positions
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

    // Set start of week
    if (calrrule.WkSt()) {
        Qt::DayOfWeek startOfWeek = toDayOfWeekL(calrrule.WkSt());
        if(Qt::Monday != startOfWeek)
           rrule.setWeekStart(startOfWeek);
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

        // daysOfYear is not supported by symbian calendar server
        if (!rrule.daysOfYear().isEmpty()) {
            User::Leave(KErrNotSupported);
        }

        // weeksOfYear is not supported by symbian calendar server
        if (!rrule.weeksOfYear().isEmpty()) {
            User::Leave(KErrNotSupported);
        }

        // Convert frequency to recurrence rule type
        calRule.SetType(toTypeL(rrule.frequency()));

        if (calRule.Type() == TCalRRule::EWeekly) {
            // Note: this is a special case; days of week can be converted
            // either to TDay array or to TDayOfMonth array, depending on
            // the recurrence frequency. This is a daily recurrence
            // -> Convert days-of-week into TDay array
            toDaysL(rrule.daysOfWeek(), startDateTime, calRule);
        } else if (calRule.Type() == TCalRRule::EMonthly) {
            // Note: this is a special case; days of week can be converted
            // either to TDay array or to TDayOfMonth array, depending on
            // the recurrence frequency. This is a monthly recurrence
            // -> Convert days-of-week and positions into TDayOfMonth array
            toDaysOfMonthL(rrule.daysOfWeek(), rrule.positions(), calRule);

            // Set days of month to the start date in case both days-of-month
            // and days-of-week are empty. This is required because Symbian
            // calendar server does not allow monthly recurring event with
            // both by-month-day and by-day to be empty.
            if (rrule.daysOfMonth().isEmpty()
                && rrule.daysOfWeek().isEmpty()) {
                QList<int> tempDaysOfMonth;
                tempDaysOfMonth.append(startDateTime.date().day());
                rrule.setDaysOfMonth(tempDaysOfMonth);
            }
            // Convert days-of-month into by-month-day
            toMonthDaysL(rrule.daysOfMonth(), calRule);
        } else if (calRule.Type() == TCalRRule::EYearly) {
            // Note: this is a special case; days of week can be converted
            // either to TDay array or to TDayOfMonth array, depending on
            // the recurrence frequency. This is a yearly recurrence
            // -> Convert days-of-week and positions into TDayOfMonth array
            toDaysOfMonthL(rrule.daysOfWeek(), rrule.positions(), calRule);
        }

        // Convert months into by-month
        toTMonthsL(rrule.months(), calRule);

        // Set the start date
        calRule.SetDtStart(toTCalTimeL(startDateTime));

        // Set the count if it is defined, otherwise set the end date
        if (rrule.count()) {
            calRule.SetCount(rrule.count());
        } else if (rrule.endDate().isValid()) {
            if (rrule.endDate() < startDateTime.date()) {
                // End date before start date!
                User::Leave(KErrArgument);
            }
            calRule.SetUntil(toTCalTimeL(rrule.endDate()));
        }

        // Set the interval. Values greater than 255 or less than 1 are
        // ignored by TCalRRule. QOrganizerItemRecurrenceRule::interval()
        // defaults to 1 if it was not set.
        calRule.SetInterval(rrule.interval());

        // Set start of the week
        // Note: Symbian calendar server ignores start of week for everything
        // else but weekly recurrences
        if (rrule.weekStart() != (Qt::Monday)) {
            calRule.SetWkSt(toTDayL(rrule.weekStart()));
        }
    }
    return calRule;
}

/*
 * Convert TCalRRule type into QOrganizerItemRecurrenceRule frequency.
 */
QOrganizerItemRecurrenceRule::Frequency OrganizerItemRecurrenceTransform::toFrequencyL(TCalRRule::TType type) const
{
    switch (type)
    {
    case TCalRRule::EDaily:
        return QOrganizerItemRecurrenceRule::Daily;
    case TCalRRule::EWeekly:
        return QOrganizerItemRecurrenceRule::Weekly;
    case TCalRRule::EMonthly:
        return QOrganizerItemRecurrenceRule::Monthly;
    case TCalRRule::EYearly:
        return QOrganizerItemRecurrenceRule::Yearly;
    default:
        // Other frequencies types not supported
        User::Leave(KErrNotSupported);
        return QOrganizerItemRecurrenceRule::Daily; // never happens
    }
}

/*
 * Convert QOrganizerItemRecurrenceRule frequency into TCalRRule type.
 */
TCalRRule::TType OrganizerItemRecurrenceTransform::toTypeL(QOrganizerItemRecurrenceRule::Frequency frequency) const
{
    switch (frequency)
    {
    case QOrganizerItemRecurrenceRule::Daily:
        return TCalRRule::EDaily;
    case QOrganizerItemRecurrenceRule::Weekly:
        return TCalRRule::EWeekly;
    case QOrganizerItemRecurrenceRule::Monthly:
        return TCalRRule::EMonthly;
    case QOrganizerItemRecurrenceRule::Yearly:
        return TCalRRule::EYearly;
    default:
        // Other frequencies types not supported by symbian calendar server
        User::Leave(KErrNotSupported);
        return TCalRRule::EInvalid; // never happens
    }
}

/*
 * Convert days-of-week into TDay array and store it to the by-day of the
 * TCalRRule. If days-of-week is empty use the week day of start date
 * instead.
 */
void OrganizerItemRecurrenceTransform::toDaysL(QList<Qt::DayOfWeek> daysOfWeek, QDateTime startDateTime, TCalRRule &calRRule) const
{
    RArray<TDay> byDay;
    CleanupClosePushL(byDay);
    if (daysOfWeek.isEmpty()) {
        // Note: Symbian calendar server does not allow storing weekly
        // recurrence without "by day" data! To overcome this issue we use
        // the start date time to determine the "by day" if the client did
        // not specify "days of week".
        QList<Qt::DayOfWeek> daysOfWeek;
        int dayOfWeek = startDateTime.date().dayOfWeek();
        Qt::DayOfWeek day = Qt::DayOfWeek(dayOfWeek);
        byDay.AppendL(toTDayL(day));
    } else {
        foreach (Qt::DayOfWeek dayOfWeek, daysOfWeek) {
            byDay.AppendL(toTDayL(dayOfWeek));
        }
    }
    calRRule.SetByDay(byDay);
    CleanupStack::PopAndDestroy(&byDay);
}

/*
 * Convert days-of-week and positions into TDayOfMonth array and store it to
 * the by-day of the TCalRRule
 */
void OrganizerItemRecurrenceTransform::toDaysOfMonthL(QList<Qt::DayOfWeek> daysOfWeek, QList<int> positions, TCalRRule &calRRule) const
{
    if (!daysOfWeek.isEmpty()) {
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
        foreach (Qt::DayOfWeek dayOfWeek, daysOfWeek) {
            foreach (int position, positions) {
                TCalRRule::TDayOfMonth dayOfMonth(toTDayL(dayOfWeek), position);
                dayArray.AppendL(dayOfMonth);
            }
        }
        if (dayArray.Count()) {
            calRRule.SetByDay(dayArray);
        }
        CleanupStack::PopAndDestroy(&dayArray);
    }
}

/*
 * Convert days-of-month into month day array and set it as the "by-month-day"
 * of the TCalRRule
 */
void OrganizerItemRecurrenceTransform::toMonthDaysL(QList<int> daysOfMonth, TCalRRule &calRule) const
{
    if (daysOfMonth.count()) {
        RArray<TInt> byMonthDay;
        CleanupClosePushL(byMonthDay);
        foreach (int dayOfMonth, daysOfMonth) {
            // symbian calendar server uses 0-based month days
            byMonthDay.AppendL(dayOfMonth - 1);
        }
        calRule.SetByMonthDay(byMonthDay);
        CleanupStack::PopAndDestroy(&byMonthDay);
    }
}

/*
 * Convert months into TMonth array and set it as the "by-month" of the
 * TCalRRule
 */
void OrganizerItemRecurrenceTransform::toTMonthsL(QList<QOrganizerItemRecurrenceRule::Month> months, TCalRRule &calRRule) const
{
    if (months.count() > 1) {
        // Symbian recurrence rule only supports one month, all the others
        // are ignored. It is better to leave in such case to keep the
        // clients informed of the constraint.
        User::Leave(KErrNotSupported);
    }

    if (months.count()) {
        RArray<TMonth> byMonth;
        CleanupClosePushL(byMonth);
        foreach (QOrganizerItemRecurrenceRule::Month month, months) {
            byMonth.AppendL(toTMonthL(month));
        }
        calRRule.SetByMonth(byMonth);
        CleanupStack::PopAndDestroy(&byMonth);
    }
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
