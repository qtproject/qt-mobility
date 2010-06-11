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

#include <QString>
#include <QDateTime>
#include <QList>

#include "qtorganizerglobal.h"
#include "qorganizeritemrecurrencerule_p.h"
#include "qorganizeritemrecurrencerule.h"

QTM_BEGIN_NAMESPACE

Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldFrequency, "Frequency");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldCount, "Count");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldStartDate, "StartDate");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldEndDate, "EndDate");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldInterval, "Interval");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldDaysOfWeek, "DaysOfWeek");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldDaysOfMonth, "DaysOfMonth");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldDaysOfYear, "DaysOfYear");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldMonths, "Months");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldWeeksOfYear, "WeeksOfYear");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldPosition, "Position");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldWeekStart, "WeekStart");

QOrganizerItemRecurrenceRule::QOrganizerItemRecurrenceRule()
    : d(new QOrganizerItemRecurrenceRulePrivate)
{
}

QOrganizerItemRecurrenceRule::~QOrganizerItemRecurrenceRule()
{
}

QOrganizerItemRecurrenceRule::QOrganizerItemRecurrenceRule(const QOrganizerItemRecurrenceRule& other)
    : d(other.d)
{
}

QOrganizerItemRecurrenceRule& QOrganizerItemRecurrenceRule::operator=(const QOrganizerItemRecurrenceRule& other)
{
    this->d = other.d;
    return *this;
}

// Default: Weekly
void QOrganizerItemRecurrenceRule::setFrequency(Frequency freq)
{
    d->m_variantValues.insert(FieldFrequency, static_cast<int>(freq));
}

QOrganizerItemRecurrenceRule::Frequency QOrganizerItemRecurrenceRule::frequency() const
{
    return static_cast<Frequency>(d->m_variantValues.value(FieldFrequency).toInt());
}

/* end critierion: neither, either or both of count/endDate could be set.  If both is set, the
 * recurrence ends on whichever occurs first.  */
// eg: 10 means ten times, 0 means not set.
// Default: 0
void QOrganizerItemRecurrenceRule::setCount(int count)
{
    d->m_variantValues.insert(FieldCount, count);
}

int QOrganizerItemRecurrenceRule::count() const
{
    return d->m_variantValues.value(FieldCount).toInt();
}

// invalid means not set.
// Default: null
void QOrganizerItemRecurrenceRule::setEndDate(const QDate& endDate)
{
    d->m_variantValues.insert(FieldEndDate, endDate);
}

QDate QOrganizerItemRecurrenceRule::endDate() const
{
    return d->m_variantValues.value(FieldEndDate).toDate();
}

// eg. if frequency = Daily and interval = 2, every second day
// Default: 1
void QOrganizerItemRecurrenceRule::setInterval(int interval)
{
    d->m_variantValues.insert(FieldInterval, interval);
}

int QOrganizerItemRecurrenceRule::interval() const
{
    return d->m_variantValues.value(FieldInterval).toInt();
}

// Defaults for the below: empty
// eg: Monday,Wednesday,Thursday == every Mon, Wed and Thurs.
void QOrganizerItemRecurrenceRule::setDaysOfWeek(const QList<Qt::DayOfWeek>& days)
{
    QVariantList variantDays;
    for (int i = 0; i < days.size(); ++i) {
        variantDays << days.at(i);
    }
    d->m_variantValues.insert(FieldDaysOfWeek, variantDays);
}

QList<Qt::DayOfWeek> QOrganizerItemRecurrenceRule::daysOfWeek() const
{
    QList<Qt::DayOfWeek> retn;
    QVariantList variantDays = d->m_variantValues.value(FieldDaysOfWeek).toList();
    for (int i = 0; i < variantDays.size(); ++i) {
        Qt::DayOfWeek curr = static_cast<Qt::DayOfWeek>(variantDays.at(i).toInt());
        retn << curr;
    }
    return retn;
}

// eg: 31,-3 == 31st day of month (if it exists) and 3rd-last day of month
void QOrganizerItemRecurrenceRule::setDaysOfMonth(const QList<int>& days)
{
    QVariantList saveList;
    for (int i = 0; i < days.size(); ++i) {
        saveList << days.at(i);
    }
    d->m_variantValues.insert(FieldDaysOfMonth, saveList);
}

QList<int> QOrganizerItemRecurrenceRule::daysOfMonth() const
{
    QList<int> retn;
    QVariantList loadList = d->m_variantValues.value(FieldDaysOfMonth).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << loadList.at(i).toInt();
    }
    return retn;
}

// eg: 47th,-5 == 47th and 5th-last day of year
void QOrganizerItemRecurrenceRule::setDaysOfYear(const QList<int>& days)
{
    QVariantList saveList;
    for (int i = 0; i < days.size(); ++i) {
        saveList << days.at(i);
    }
    d->m_variantValues.insert(FieldDaysOfYear, saveList);
}

QList<int> QOrganizerItemRecurrenceRule::daysOfYear() const
{
    QList<int> retn;
    QVariantList loadList = d->m_variantValues.value(FieldDaysOfYear).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << loadList.at(i).toInt();
    }
    return retn;
}

// eg: January,February == during Jan and Feb
void QOrganizerItemRecurrenceRule::setMonths(const QList<Month>& months)
{
    QVariantList saveList;
    for (int i = 0; i < months.size(); ++i) {
        saveList << static_cast<int>(months.at(i));
    }
    d->m_variantValues.insert(FieldMonths, saveList);
}

QList<QOrganizerItemRecurrenceRule::Month> QOrganizerItemRecurrenceRule::months() const
{
    QList<QOrganizerItemRecurrenceRule::Month> retn;
    QVariantList loadList = d->m_variantValues.value(FieldMonths).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << static_cast<QOrganizerItemRecurrenceRule::Month>(loadList.at(i).toInt());
    }
    return retn;
}

// eg. 13,53,-3 == weeks 13 and 53 (if it exists) and the 3rd-last week of the year
void QOrganizerItemRecurrenceRule::setWeeksOfYear(const QList<int>& weeks)
{
    QVariantList saveList;
    for (int i = 0; i < weeks.size(); ++i) {
        saveList << weeks.at(i);
    }
    d->m_variantValues.insert(FieldWeeksOfYear, saveList);
}

QList<int> QOrganizerItemRecurrenceRule::weeksOfYear() const
{
    QList<int> retn;
    QVariantList loadList = d->m_variantValues.value(FieldWeeksOfYear).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << loadList.at(i).toInt();
    }
    return retn;
}

// eg. frequency = Monthly, dayOfWeek = Tuesday, positions = 1,-1 means first and last Tuesday
// of every month.
// All other criteria are applied first, then for each time period as specified by frequency,
// dates are selected via the 1-based indices specified by position.
void QOrganizerItemRecurrenceRule::setPositions(const QList<int>& pos)
{
    QVariantList saveList;
    for (int i = 0; i < pos.size(); ++i) {
        saveList << pos.at(i);
    }
    d->m_variantValues.insert(FieldPosition, saveList);
}

QList<int> QOrganizerItemRecurrenceRule::positions() const
{
    QList<int> retn;
    QVariantList loadList = d->m_variantValues.value(FieldPosition).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << loadList.at(i).toInt();
    }
    return retn;
}

// Default: Monday
// sets the day that the week starts on (significant for Weekly with interval > 1, and for weekOfYear)
void QOrganizerItemRecurrenceRule::setWeekStart(Qt::DayOfWeek day)
{
    d->m_variantValues.insert(FieldWeekStart, static_cast<int>(day));
}

Qt::DayOfWeek QOrganizerItemRecurrenceRule::weekStart() const
{
    return static_cast<Qt::DayOfWeek>(d->m_variantValues.value(FieldWeekStart).toInt());
}

QVariantMap QOrganizerItemRecurrenceRule::variantValues() const
{
    return d->m_variantValues;
}

void QOrganizerItemRecurrenceRule::setVariantValues(const QVariantMap& variantValues)
{
    d->m_variantValues = variantValues;
}

QOrganizerItemRecurrenceRule QOrganizerItemRecurrenceRule::fromVariantValues(const QVariantMap& variantValues)
{
    // XXX TODO: make this more efficient... (conversion from qvariant functions go into the private class)
    QOrganizerItemRecurrenceRule ret;
    QOrganizerItemRecurrenceRule converter;
    converter.setVariantValues(variantValues);

    ret.setCount(converter.count());
    ret.setDaysOfMonth(converter.daysOfMonth());
    ret.setDaysOfWeek(converter.daysOfWeek());
    ret.setDaysOfYear(converter.daysOfYear());
    ret.setEndDate(converter.endDate());
    ret.setFrequency(converter.frequency());
    ret.setInterval(converter.interval());
    ret.setMonths(converter.months());
    ret.setPositions(converter.positions());
    ret.setWeeksOfYear(converter.weeksOfYear());
    ret.setWeekStart(converter.weekStart());

    return ret;
}

QTM_END_NAMESPACE
