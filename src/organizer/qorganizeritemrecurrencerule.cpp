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
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldPositions, "Positions");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::FieldWeekStart, "WeekStart");

/*!
 * \class QOrganizerItemRecurrenceRule
 *
 * \brief The QOrganizerItemRecurrenceRule class describes the a rule by which a QOrganizerItem repeats.
 *
 * This class is a mapping of a subset of the iCalendar RRULE property value, and each field in this
 * class corresponds to a fragment of iCalendar's RRULE.  This class supports the same fragments as
 * those supported by RRULE, except for describing recurrences on a higher frequency than Daily.
 * That is, this class doesn't support hourly, minutely or secondly recurrences, nor does it support
 * specifying which hour, minute or second of a day to recur on.  These types of rules are
 * unsupported because most calendaring backends don't support them, and it simplifies recurrences
 * by enforcing that there can be at most one occurrence of an item per day.
 */

/*!
 * Constructs a QOrganizerItemRecurrenceRule object describing a weekly recurrence.
 */
QOrganizerItemRecurrenceRule::QOrganizerItemRecurrenceRule()
    : d(new QOrganizerItemRecurrenceRulePrivate)
{
}

/*!
 * Destroys the QOrganizerItemRecurrenceRule object.
 */
QOrganizerItemRecurrenceRule::~QOrganizerItemRecurrenceRule()
{
}

/*!
 * Constructs a QOrganizerItemRecurrenceRule object as a copy of \a other.
 */
QOrganizerItemRecurrenceRule::QOrganizerItemRecurrenceRule(const QOrganizerItemRecurrenceRule& other)
    : d(other.d)
{
}

/*!
 * Assigns this detail to be equal to \a other.
 */
QOrganizerItemRecurrenceRule& QOrganizerItemRecurrenceRule::operator=(const QOrganizerItemRecurrenceRule& other)
{
    this->d = other.d;
    return *this;
}

/*!
 * Sets the frequency with which the item recurs to \a freq.
 *
 * This corresponds to the FREQ fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setFrequency(Frequency freq)
{
    d->m_variantValues.insert(FieldFrequency, static_cast<int>(freq));
}

/*!
 * Returns the frequency with which the item recurs.  The default frequency is 1.
 */
QOrganizerItemRecurrenceRule::Frequency QOrganizerItemRecurrenceRule::frequency() const
{
    if (d->m_variantValues.contains(FieldFrequency))
        return static_cast<Frequency>(d->m_variantValues.value(FieldFrequency).toInt());
    else
        return Weekly;
}

/*! Sets the "count" condition of the recurrence rule.  If an end-date was previously set,
 * it is removed as count and endDate are mutually exclusive.
 *
 * The "count" condition is the maximum number of times the item should recur.  Setting this
 * to 0 or a negative value removes the count condition.
 *
 * This corresponds to the COUNT fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setCount(int count)
{
    if (count > 0)
        d->m_variantValues.remove(FieldEndDate);
    d->m_variantValues.insert(FieldCount, count);
}

/*! Returns the "count" condition specified by the recurrence rule.  The default count is 0 (ie.
 * unlimited)
 */
int QOrganizerItemRecurrenceRule::count() const
{
    return d->m_variantValues.value(FieldCount).toInt();
}

/*! Sets the end-date condition of the recurrence rule.  If a "count" condition was previously
 * set, it is removed as count and endDate are mutually exclusive.
 *
 * The end-date condition is the date after which the item should not recur.  Setting this to
 * the null date removes the end-date condition.
 *
 * This corresponds to the UNTIL fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setEndDate(const QDate& endDate)
{
    if (endDate.isValid())
        d->m_variantValues.remove(FieldCount);
    d->m_variantValues.insert(FieldEndDate, endDate);
}

/*! Returns the end-date condition specified by the recurrence rule.  The default end date is the
 * null date (ie. no end date).
 */
QDate QOrganizerItemRecurrenceRule::endDate() const
{
    return d->m_variantValues.value(FieldEndDate).toDate();
}

/*!
 * Sets the interval, between cycles of length given by frequency(), in which the item should recur
 * to \a interval.
 *
 * For example, if the frequency() is QOrganizerItemRecurrenceRule::Daily and the interval is set to
 * 2, the item should recur every second day.
 *
 * This corresponds to the INTERVAL fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setInterval(int interval)
{
    d->m_variantValues.insert(FieldInterval, interval);
}

/*!
 * Returns the interval of recurrence.  The default interval is 1.
 */
int QOrganizerItemRecurrenceRule::interval() const
{
    if (d->m_variantValues.contains(FieldInterval))
        return d->m_variantValues.value(FieldInterval).toInt();
    else
        return 1;
}

// Defaults for the below: empty
// eg: Monday,Wednesday,Thursday == every Mon, Wed and Thurs.
/*!
 * Sets the days of week on which the item should recur to \a days.
 *
 * This corresponds to the BYDAY fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setDaysOfWeek(const QList<Qt::DayOfWeek>& days)
{
    QVariantList variantDays;
    for (int i = 0; i < days.size(); ++i) {
        variantDays << days.at(i);
    }
    d->m_variantValues.insert(FieldDaysOfWeek, variantDays);
}

/*!
 * Returns a list of the days of week that the item should recur on.  If not set, this is the empty
 * list, which signifies that it should be implied, if necessary, by the day-of-week of the item.
 */
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

/*!
 * Sets the days of the month on which the item should recur to \a days.  Negative values in the list
 * represent the number of days from the end of the month.  eg. 1 represents the first day of the
 * month and -1 represents the last day of the month.
 *
 * This corresponds to the BYMONTHDAY fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setDaysOfMonth(const QList<int>& days)
{
    QVariantList saveList;
    for (int i = 0; i < days.size(); ++i) {
        saveList << days.at(i);
    }
    d->m_variantValues.insert(FieldDaysOfMonth, saveList);
}

/*!
 * Returns a list of the days of the month that the item should recur on.  If not set, this is the
 * empty list, which signifies that it should be implied, if necessary, by the day-of-month of the
 * item.
 */
QList<int> QOrganizerItemRecurrenceRule::daysOfMonth() const
{
    QList<int> retn;
    QVariantList loadList = d->m_variantValues.value(FieldDaysOfMonth).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << loadList.at(i).toInt();
    }
    return retn;
}

/*!
 * Sets the days of the year on which the item should recur to \a days.  Negative values in the list
 * represent the number of days from the end of the year.  eg. 1 represents the first day of the
 * year and -1 represents the last day of the year.
 *
 * This corresponds to the BYYEARDAY fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setDaysOfYear(const QList<int>& days)
{
    QVariantList saveList;
    for (int i = 0; i < days.size(); ++i) {
        saveList << days.at(i);
    }
    d->m_variantValues.insert(FieldDaysOfYear, saveList);
}

/*!
 * Returns a list of the days of the year that the item should recur on.  If not set, this is the
 * empty list, which signifies that it should be implied, if necessary, by the day-of-year of the
 * item.
 */
QList<int> QOrganizerItemRecurrenceRule::daysOfYear() const
{
    QList<int> retn;
    QVariantList loadList = d->m_variantValues.value(FieldDaysOfYear).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << loadList.at(i).toInt();
    }
    return retn;
}

/*!
 * Sets the months on which the item should recur to \a months.
 *
 * This corresponds to the BYMONTHDAY fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setMonths(const QList<Month>& months)
{
    QVariantList saveList;
    for (int i = 0; i < months.size(); ++i) {
        saveList << static_cast<int>(months.at(i));
    }
    d->m_variantValues.insert(FieldMonths, saveList);
}

/*!
 * Returns a list of the months that the item should recur on.  If not set, this is the
 * empty list, which signifies that it should be implied, if necessary, by the month of the
 * item.
 */
QList<QOrganizerItemRecurrenceRule::Month> QOrganizerItemRecurrenceRule::months() const
{
    QList<QOrganizerItemRecurrenceRule::Month> retn;
    QVariantList loadList = d->m_variantValues.value(FieldMonths).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << static_cast<QOrganizerItemRecurrenceRule::Month>(loadList.at(i).toInt());
    }
    return retn;
}

/*!
 * Sets the weeks of the year on which the item should recur to \a weeks.  Negative values in the list
 * represent the number of weeks from the end of the year.  eg. 1 represents the first week of the
 * year and -1 represents the last week of the year.
 *
 * This corresponds to the BYWEEK fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setWeeksOfYear(const QList<int>& weeks)
{
    QVariantList saveList;
    for (int i = 0; i < weeks.size(); ++i) {
        saveList << weeks.at(i);
    }
    d->m_variantValues.insert(FieldWeeksOfYear, saveList);
}

/*!
 * Returns a list of the weeks of the year that the item should recur on.  If not set, this is the
 * empty list, which signifies that it should be implied, if necessary, by the week number of the
 * item.
 */
QList<int> QOrganizerItemRecurrenceRule::weeksOfYear() const
{
    QList<int> retn;
    QVariantList loadList = d->m_variantValues.value(FieldWeeksOfYear).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << loadList.at(i).toInt();
    }
    return retn;
}

/*!
 * Sets the list of positions that the item should recur on to \a pos.  This specifies that the item
 * should only recur on the nth occurrence within the set of events otherwise specified by the rule,
 * for the values of n in \a pos.
 *
 * Negative values in the list represnet a position counting from the end of the set.
 *
 * For example, if frequency() == Monthly and months() is the list Monday, Tuesday, Wednesday,
 * Thursday, Friday, and positions() == -1, this specifies that the item should recur on the last
 * weekday of each month.
 *
 * This corresponds to the BYSETPOS fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setPositions(const QList<int>& pos)
{
    QVariantList saveList;
    for (int i = 0; i < pos.size(); ++i) {
        saveList << pos.at(i);
    }
    d->m_variantValues.insert(FieldPositions, saveList);
}

/*!
 * Returns the position-list of the recurrence rule.  If not set, this is the empty list, which
 * signifies that the recurrence dates should not be restricted by position.
 */
QList<int> QOrganizerItemRecurrenceRule::positions() const
{
    QList<int> retn;
    QVariantList loadList = d->m_variantValues.value(FieldPositions).toList();
    for (int i = 0; i < loadList.size(); ++i) {
        retn << loadList.at(i).toInt();
    }
    return retn;
}

/*! Sets the day that the week starts on, for the purposes of calculating recurrences.  This is
 * significant when the frequency is Weekly and the interval is greater than 1, or when weekOfYear
 * is set.  See the iCalendar spec for examples of its significance.
 *
 * This corresponds to the BYWKST fragment in iCalendar's RRULE.
 */
void QOrganizerItemRecurrenceRule::setWeekStart(Qt::DayOfWeek day)
{
    d->m_variantValues.insert(FieldWeekStart, static_cast<int>(day));
}

/*!
 * Returns the day that the week starts on.  If not set, this is Monday.
 */
Qt::DayOfWeek QOrganizerItemRecurrenceRule::weekStart() const
{
    if (d->m_variantValues.contains(FieldWeekStart))
        return static_cast<Qt::DayOfWeek>(d->m_variantValues.value(FieldWeekStart).toInt());
    else
        return Qt::Monday;
}


/*!
 * \internal
 * TODO: delete these functions
 */
QVariantMap QOrganizerItemRecurrenceRule::variantValues() const
{
    return d->m_variantValues;
}

/*!
 * \internal
 */
void QOrganizerItemRecurrenceRule::setVariantValues(const QVariantMap& variantValues)
{
    d->m_variantValues = variantValues;
}

/*!
 * \internal
 */
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
