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

class QString;
class QDateTime;
#include <QList>

#include "qtorganizerglobal.h"
#include "qorganizeritemdetail.h"

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

class Q_ORGANIZER_EXPORT QOrganizerItemRecurrenceRule : public QOrganizerItemDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldFrequency;
    const char* FieldCount;
    const char* FieldStartDate;
    const char* FieldEndDate;
    const char* FieldInterval;
    const char* FieldDayOfWeek;
    const char* FieldDayOfMonth;
    const char* FieldDayOfYear;
    const char* FieldMonth;
    const char* FieldWeekOfYear;
    const char* FieldPosition;
    const char* FieldWeekStart;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_DETAIL(QOrganizerItemRecurrenceRule, "RecurrenceRule")
    Q_DECLARE_LATIN1_CONSTANT(FieldFrequency, "Frequency");
    Q_DECLARE_LATIN1_CONSTANT(FieldCount, "Count");
    Q_DECLARE_LATIN1_CONSTANT(FieldStartDate, "StartDate");
    Q_DECLARE_LATIN1_CONSTANT(FieldEndDate, "EndDate");
    Q_DECLARE_LATIN1_CONSTANT(FieldInterval, "Interval");
    Q_DECLARE_LATIN1_CONSTANT(FieldDayOfWeek, "DayOfWeek");
    Q_DECLARE_LATIN1_CONSTANT(FieldDayOfMonth, "DayOfMonth");
    Q_DECLARE_LATIN1_CONSTANT(FieldDayOfYear, "DayOfYear");
    Q_DECLARE_LATIN1_CONSTANT(FieldMonth, "Month");
    Q_DECLARE_LATIN1_CONSTANT(FieldWeekOfYear, "WeekOfYear");
    Q_DECLARE_LATIN1_CONSTANT(FieldPosition, "Position");
    Q_DECLARE_LATIN1_CONSTANT(FieldWeekStart, "WeekStart");
#endif

    // enums

    enum Frequency {
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
    bool setStartDate(const QDateTime& startDate);
    QDateTime startDate() const;

    // Default: Weekly
    bool setFrequency(Frequency freq);                 // eg: Daily
    Frequency frequency() const;

    /* end critierion: neither, either or both of count/endDate could be set.  If both is set, the
     * recurrence ends on whichever occurs first.  */
    // eg: 10 means ten times, 0 means not set.
    // Default: 0
    bool setCount(int count);
    int count() const;
    // invalid means not set.
    // Default: null
    bool setEndDate(const QDateTime& endDate);
    QDateTime endDate() const;

    // eg. if frequency = Daily and interval = 2, every second day
    // Default: 1
    bool setInterval(int interval);
    int interval() const;

    // Defaults for the below: empty
    // eg: Monday,Wednesday,Thursday == every Mon, Wed and Thurs.
    bool setDayOfWeek(const QList<Qt::DayOfWeek>& days);
    QList<Qt::DayOfWeek> dayOfWeek() const;
    // eg: 31,-3 == 31st day of month (if it exists) and 3rd-last day of month
    bool setDayOfMonth(const QList<int>& days);
    QList<int> dayOfMonth() const;
    // eg: 47th,-5 == 47th and 5th-last day of year
    bool setDayOfYear(const QList<int>& days);
    QList<int> dayOfYear() const;
    // eg: January,February == during Jan and Feb
    bool setMonth(const QList<Month>& months);
    QList<Month> month() const;
    // eg. 13,53,-3 == weeks 13 and 53 (if it exists) and the 3rd-last week of the year
    bool setWeekOfYear(const QList<int>& weeks);
    QList<int> weekOfYear() const;

    // eg. frequency = Monthly, dayOfWeek = Tuesday, positions = 1,-1 means first and last Tuesday
    // of every month.
    // All other criteria are applied first, then for each time period as specified by frequency,
    // dates are selected via the 1-based indices specified by position.
    bool setPosition(const QList<int>& pos);
    QList<int> position() const;

    // Default: Monday
    // sets the day that the week starts on (significant for Weekly with interval > 1, and for weekOfYear)
    bool setWeekStart(Qt::DayOfWeek day);
    Qt::DayOfWeek weekStart() const;
};

QTM_END_NAMESPACE

#endif
