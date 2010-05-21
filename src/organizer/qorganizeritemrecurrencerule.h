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


#ifndef QORGANIZERITEMRECURRENCERULE_H
#define QORGANIZERITEMRECURRENCERULE_H

#include <QString>
#include <QDateTime>
#include <QList>

#include "qtorganizerglobal.h"
#include "qorganizeritemrecurrencerule_p.h"

QTM_BEGIN_NAMESPACE

/*
 * This is based on RRULE in iCalendar.
 * An item can have multiple QOIRecurrence details, in which case the dates are calculated
 * separately for each and then unioned.
 *
 * The fields of this class all correspond to RRULE fragments.
 * 
 * The general rules for interaction between the fields when generating the occurence dates is as
 * follows:
 *
 * When a criterion takes a list, the items in the list are unioned together.
 * eg. with [dayOfWeek == Tuesday,Thursday], the event occurs if it is Tuesday or Thursday
 *
 * Frequency and specific criteria interact a bit more complicatedly.  For each criterion on a
 * larger timespan than the frequncy, the dates matching the criterion are intersected with the
 * dates resulting from the frequency.
 * eg. [frequency = Daily, month = January] means every day in January
 * For each criterion on a shorter timespan than the frequency, the criterion is unioned.
 * eg. [frequency = Weekly, dayOfWeek = Wednesday,Friday] means every Wednesday and Friday of
 * every week.
 * This makes the frequency field superfluous in many cases when other criteria are present.
 * eg. all of the below mean the same thing:
 * [frequency = Daily, dayOfWeek = Monday,Tuesday]
 * [frequency = Weekly, dayOfWeek = Monday,Tuesday]
 * [frequency = Monthly, dayOfWeek = Monday,Tuesday]
 * [frequency = Yearly, dayOfWeek = Monday,Tuesday]
 * However, the frequency field may start affecting the result differently when other fields are
 * added like interval and position.
 *
 * Information not contained in the rule is in some cases derived from the startDate field for the
 * purpose of calculating occurrence dates.  There are three cases where such derivation is
 * necessary.
 * Case 1: frequency == Weekly.  If dayOfWeek is not specified, derive it from the week day that
 * startDate occurs on.
 * Case 2: frequency == Monthly.  If neither dayOfWeek or dayOfMonth is specified, dayOfMonth should
 * be derived from the startDate
 * Case 3: frequency == Yearly.  If none of month, weekOfYear, dayOfYear, dayOfMonth or dayOfWeek
 * are specified, derive month and dayOfMonth.  If month is specified but not weekOfYear, dayOfYear,
 * dayOfMonth or dayOfWeek, then derive dayOfMonth.  If weekOfYear is specified but not dayOfYear,
 * dayOfWeek or dayOfMonth, derive dayOfWeek from the startDate.
 * For any cases not covered here, do not derive any of the fields.
 */

class Q_ORGANIZER_EXPORT QOrganizerItemRecurrenceRule
{
public:
#ifdef Q_QDOC
    const char* FieldFrequency;
    const char* FieldCount;
    const char* FieldStartDate;
    const char* FieldEndDate;
    const char* FieldInterval;
    const char* FieldDaysOfWeek;
    const char* FieldDaysOfMonth;
    const char* FieldDaysOfYear;
    const char* FieldMonths;
    const char* FieldWeeksOfYear;
    const char* FieldPosition;
    const char* FieldWeekStart;
#else
    Q_DECLARE_LATIN1_CONSTANT(FieldFrequency, "Frequency");
    Q_DECLARE_LATIN1_CONSTANT(FieldCount, "Count");
    Q_DECLARE_LATIN1_CONSTANT(FieldStartDate, "StartDate");
    Q_DECLARE_LATIN1_CONSTANT(FieldEndDate, "EndDate");
    Q_DECLARE_LATIN1_CONSTANT(FieldInterval, "Interval");
    Q_DECLARE_LATIN1_CONSTANT(FieldDaysOfWeek, "DaysOfWeek");
    Q_DECLARE_LATIN1_CONSTANT(FieldDaysOfMonth, "DaysOfMonth");
    Q_DECLARE_LATIN1_CONSTANT(FieldDaysOfYear, "DaysOfYear");
    Q_DECLARE_LATIN1_CONSTANT(FieldMonths, "Months");
    Q_DECLARE_LATIN1_CONSTANT(FieldWeeksOfYear, "WeeksOfYear");
    Q_DECLARE_LATIN1_CONSTANT(FieldPosition, "Position");
    Q_DECLARE_LATIN1_CONSTANT(FieldWeekStart, "WeekStart");
#endif

    // enums

    enum Frequency {
        NoRegularFrequency = 0, // eg: setMonths(<Jan, Feb>), setDaysOfMonth(15,-3)); // doesn't occur monthly, so no freq.
        Daily,
        Weekly,
        Monthly,
        Yearly
    };

    enum Month {
        January = 1,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };

    // Compulsory for a valid rule
    void setStartDate(const QDate& startDate);
    QDate startDate() const;

    // Default: Weekly
    void setFrequency(Frequency freq);
    Frequency frequency() const;

    /* end critierion: neither, either or both of count/endDate could be set.  If both is set, the
     * recurrence ends on whichever occurs first.  */
    // eg: 10 means ten times, 0 means not set.
    // Default: 0
    void setCount(int count);
    int count() const;
    // invalid means not set.
    // Default: null
    void setEndDate(const QDate& endDate);
    QDate endDate() const;

    // eg. if frequency = Daily and interval = 2, every second day
    // Default: 1
    void setInterval(int interval);
    int interval() const;

    // Defaults for the below: empty
    // eg: Monday,Wednesday,Thursday == every Mon, Wed and Thurs.
    void setDaysOfWeek(const QList<Qt::DayOfWeek>& days);
    QList<Qt::DayOfWeek> daysOfWeek() const;
    // eg: 31,-3 == 31st day of month (if it exists) and 3rd-last day of month
    void setDaysOfMonth(const QList<int>& days);
    QList<int> daysOfMonth() const;
    // eg: 47th,-5 == 47th and 5th-last day of year
    void setDaysOfYear(const QList<int>& days);
    QList<int> daysOfYear() const;
    // eg: January,February == during Jan and Feb
    void setMonths(const QList<Month>& months);
    QList<Month> months() const;
    // eg. 13,53,-3 == weeks 13 and 53 (if it exists) and the 3rd-last week of the year
    void setWeeksOfYear(const QList<int>& weeks);
    QList<int> weeksOfYear() const;

    // eg. frequency = Monthly, dayOfWeek = Tuesday, positions = 1,-1 means first and last Tuesday
    // of every month.
    // All other criteria are applied first, then for each time period as specified by frequency,
    // dates are selected via the 1-based indices specified by position.
    void setPosition(const QList<int>& pos);
    QList<int> position() const;

    // Default: Monday
    // sets the day that the week starts on (significant for Weekly with interval > 1, and for weekOfYear)
    void setWeekStart(Qt::DayOfWeek day);
    Qt::DayOfWeek weekStart() const;

    QVariantMap variantValues() const;

    static QOrganizerItemRecurrenceRule fromVariantValues(const QVariantMap& variantValues);

private:
    void setVariantValues(const QVariantMap& variantValues);
    QSharedDataPointer<QOrganizerItemRecurrenceRulePrivate> d;
};

QTM_END_NAMESPACE

#endif
