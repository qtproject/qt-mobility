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
    void setStartDate(const QDateTime& startDate) {setValue(FieldStartDate, startDate);}
    QDateTime startDate() const {return variantValue(FieldStartDate).toDateTime();}

    // Default: Weekly
    void setFrequency(Frequency freq) {setValue(FieldFrequency, static_cast<int>(freq));}
    Frequency frequency() const {return static_cast<Frequency>(variantValue(FieldFrequency).toInt());}

    /* end critierion: neither, either or both of count/endDate could be set.  If both is set, the
     * recurrence ends on whichever occurs first.  */
    // eg: 10 means ten times, 0 means not set.
    // Default: 0
    void setCount(int count) {setValue(FieldCount, count);}
    int count() const {return variantValue(FieldCount).toInt();}
    // invalid means not set.
    // Default: null
    void setEndDate(const QDateTime& endDate) {setValue(FieldEndDate, endDate);}
    QDateTime endDate() const {return variantValue(FieldEndDate).toDateTime();}

    // eg. if frequency = Daily and interval = 2, every second day
    // Default: 1
    void setInterval(int interval) {setValue(FieldInterval, interval);}
    int interval() const {return variantValue(FieldInterval).toInt();}

    // XXX TODO: move the implementations below to the details.cpp file.

    // Defaults for the below: empty
    // eg: Monday,Wednesday,Thursday == every Mon, Wed and Thurs.
    void setDayOfWeek(const QList<Qt::DayOfWeek>& days)
    {
        QVariantList variantDays;
        for (int i = 0; i < days.size(); ++i) {
            variantDays << days.at(i);
        }
        setValue(FieldDayOfWeek, variantDays);
    }
    QList<Qt::DayOfWeek> dayOfWeek() const
    {
        QList<Qt::DayOfWeek> retn;
        QVariantList variantDays = variantValue(FieldDayOfWeek).toList();
        for (int i = 0; i < variantDays.size(); ++i) {
            Qt::DayOfWeek curr = static_cast<Qt::DayOfWeek>(variantDays.at(i).toInt());
            retn << curr;
        }
        return retn;
    }
    // eg: 31,-3 == 31st day of month (if it exists) and 3rd-last day of month
    void setDayOfMonth(const QList<int>& days)
    {
        QVariantList saveList;
        for (int i = 0; i < days.size(); ++i) {
            saveList << days.at(i);
        }
        setValue(FieldDayOfMonth, saveList);
    }
    QList<int> dayOfMonth() const
    {
        QList<int> retn;
        QVariantList loadList = variantValue(FieldDayOfMonth).toList();
        for (int i = 0; i < loadList.size(); ++i) {
            retn << loadList.at(i).toInt();
        }
        return retn;
    }
    // eg: 47th,-5 == 47th and 5th-last day of year
    void setDayOfYear(const QList<int>& days)
    {
        QVariantList saveList;
        for (int i = 0; i < days.size(); ++i) {
            saveList << days.at(i);
        }
        setValue(FieldDayOfMonth, saveList);
    }
    QList<int> dayOfYear() const
    {
        QList<int> retn;
        QVariantList loadList = variantValue(FieldDayOfYear).toList();
        for (int i = 0; i < loadList.size(); ++i) {
            retn << loadList.at(i).toInt();
        }
        return retn;
    }
    // eg: January,February == during Jan and Feb
    void setMonth(const QList<Month>& months)
    {
        QVariantList saveList;
        for (int i = 0; i < months.size(); ++i) {
            saveList << static_cast<int>(months.at(i));
        }
        setValue(FieldDayOfMonth, saveList);
    }
    QList<Month> month() const
    {
        QList<Month> retn;
        QVariantList loadList = variantValue(FieldMonth).toList();
        for (int i = 0; i < loadList.size(); ++i) {
            retn << static_cast<Month>(loadList.at(i).toInt());
        }
        return retn;
    }
    // eg. 13,53,-3 == weeks 13 and 53 (if it exists) and the 3rd-last week of the year
    void setWeekOfYear(const QList<int>& weeks)
    {
        QVariantList saveList;
        for (int i = 0; i < weeks.size(); ++i) {
            saveList << weeks.at(i);
        }
        setValue(FieldDayOfMonth, saveList);
    }
    QList<int> weekOfYear() const
    {
        QList<int> retn;
        QVariantList loadList = variantValue(FieldWeekOfYear).toList();
        for (int i = 0; i < loadList.size(); ++i) {
            retn << loadList.at(i).toInt();
        }
        return retn;
    }

    // eg. frequency = Monthly, dayOfWeek = Tuesday, positions = 1,-1 means first and last Tuesday
    // of every month.
    // All other criteria are applied first, then for each time period as specified by frequency,
    // dates are selected via the 1-based indices specified by position.
    void setPosition(const QList<int>& pos)
    {
        QVariantList saveList;
        for (int i = 0; i < pos.size(); ++i) {
            saveList << pos.at(i);
        }
        setValue(FieldDayOfMonth, saveList);
    }
    QList<int> position() const
    {
        QList<int> retn;
        QVariantList loadList = variantValue(FieldPosition).toList();
        for (int i = 0; i < loadList.size(); ++i) {
            retn << loadList.at(i).toInt();
        }
        return retn;
    }

    // Default: Monday
    // sets the day that the week starts on (significant for Weekly with interval > 1, and for weekOfYear)
    void setWeekStart(Qt::DayOfWeek day) {setValue(FieldWeekStart, static_cast<int>(day));}
    Qt::DayOfWeek weekStart() const {return static_cast<Qt::DayOfWeek>(variantValue(FieldWeekStart).toInt());}
};

QTM_END_NAMESPACE

#endif
