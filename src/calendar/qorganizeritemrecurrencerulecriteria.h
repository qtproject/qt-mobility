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


#ifndef QORGANIZERITEMRECURRENCERULECRITERIA_H
#define QORGANIZERITEMRECURRENCERULECRITERIA_H

#include <QString>
#include <QDateTime>
#include <QList>
#include <QPair>
#include <QSharedDataPointer>

#include "qtorganizeritemsglobal.h"

QTM_BEGIN_NAMESPACE

/*
 * Idea: build up an expression tree of these criteria objects.
 * The expression tree gets expanded to occurrence dates, internally?
 */

class Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRuleCriteria
{
public:

    // enums

    enum Frequency {
        Daily,
        Weekly,
        Monthly,
        Yearly
    };

    enum Day {
        Sunday,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday
    };

    enum Month {
        January,
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


    // mutators

    bool setStartDate(const QDateTime& startDate);
    bool setEndDate(const QDateTime& endDate);

    bool setFrequency(Frequency freq, int interval);   // eg: Daily,2 == every second day.
    bool setRepetitions(int reps);                     // eg: 10 == ten times

    bool setDayCriterion(const QList<Day>& days);      // eg: Monday,Wednesday,Thursday == every Mon, Wed and Thurs.
    bool setDayOfMonthCriterion(int which, Day day);   // eg: -2, Monday == second last monday of the month
    bool setDayOfYearCriterion(int which, Day day);    // eg: 28, Friday == 28th friday of the year

    bool setMonthCriterion(const QList<Month>& months);// eg: January,February == during Jan and Feb


    // accessors

    QDateTime startDate() const;
    QDateTime endDate() const;
    Frequency frequency() const;
    int interval() const;

    bool isDayCriterionSet() const;
    bool isDayOfMonthCriterionSet() const;
    bool isDayOfYearCriterionSet() const;
    QList<Day> dayCriterion() const;
    QPair<int, Day> dayOfMonthCriterion() const;
    QPair<int, Day> dayOfYearCriterion() const;

    bool isMonthCriterionSet() const;
    QList<Month> monthCriterion() const;

    // mutual exclusion of criteria:
    // setDayCriterion is mex to setDayOfMonth and setDayOfYear
    // setMonthCriterion is mex to setDayOfYearCriterion // is it?  maybe not... 28th friday of the year, if it's in june (ie not if it's in july)


    // total-specification of occurrence dates - mutually exclusive to all of the above criteria

    void setOccurrences(const QList<QDateTime>& occurrences);
    bool isTotallySpecified() const; // returns true if setOccurrences() has been called.


    // accessing the occurrence dates - lazy calculation?

    QList<QDateTime> calculateOccurrences() const; // not a simple accessor, unless isTotallySpecified() returns true.

private:
    QSharedDataPointer<QOrganizerItemRecurrenceRuleCriteriaPrivate> d;
};

QTM_END_NAMESPACE

#endif
