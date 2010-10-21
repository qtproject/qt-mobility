/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QString>
#include <QDateTime>
#include <QList>

#include "qtorganizerglobal.h"
#include "qorganizerrecurrencerule_p.h"
#include "qorganizerrecurrencerule.h"

QTM_BEGIN_NAMESPACE


/*!
 * \class QOrganizerRecurrenceRule
 *
 * \brief The QOrganizerRecurrenceRule class describes the a rule by which a QOrganizerItem repeats.

  \inmodule QtOrganizer
 *
 * This class is a mapping of a subset of the iCalendar RRULE property value, and each field in this
 * class corresponds to a fragment of iCalendar's RRULE.  This class supports the same fragments as
 * those supported by RRULE, except for describing recurrences on a higher frequency than Daily.
 * That is, this class doesn't support hourly, minutely or secondly recurrences, nor does it support
 * specifying which hour, minute or second of a day to recur on.  These types of rules are
 * unsupported because most calendaring backends don't support them, and it simplifies recurrences
 * by enforcing that there can be at most one occurrence of an item per day.
 *
 *
 * The general rules for interaction between the fields when generating the occurence dates is as
 * follows:
 *
 * When a criterion takes a list, the items in the list are unioned together.
 * eg. with [dayOfWeek == Tuesday,Thursday], the event occurs if it is Tuesday or Thursday
 *
 * Frequency and specific criteria interact a bit more complicatedly.  For each criterion on a
 * larger timespan than the frequency, the dates matching the criterion are intersected with the
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
 * added like interval and positions.
 *
 * For the purpose of calculating occurrence dates, information not contained in the rule is in some
 * cases derived from the startDateTime field of the event that the detail is associated with.
 * There are three cases where such derivation is necessary.
 * Case 1: frequency == Weekly.  If dayOfWeek is not specified, derive it from the week day that
 * the startDateTime occurs on.
 * Case 2: frequency == Monthly.  If neither dayOfWeek or dayOfMonth is specified, dayOfMonth should
 * be derived from the startDateTime
 * Case 3: frequency == Yearly.  If none of month, weekOfYear, dayOfYear, dayOfMonth or dayOfWeek
 * are specified, derive month and dayOfMonth.  If month is specified but not weekOfYear, dayOfYear,
 * dayOfMonth or dayOfWeek, then derive dayOfMonth.  If weekOfYear is specified but not dayOfYear,
 * dayOfWeek or dayOfMonth, derive dayOfWeek from the startDateTime.
 * For any cases not covered here, do not derive any of the fields.
 */

/*!
 * \enum QOrganizerRecurrenceRule::Frequency
 * \value Invalid Signifies that the entire rrule is invalid.
 * \value Daily
 * \value Weekly
 * \value Monthly
 * \value Yearly
 */

/*!
 * \enum QOrganizerRecurrenceRule::Month
 * \value January
 * \value February
 * \value March
 * \value April
 * \value May
 * \value June
 * \value July
 * \value August
 * \value September
 * \value October
 * \value November
 * \value December
 */

/*!
 * \enum QOrganizerRecurrenceRule::LimitType
 * \value NoLimit The recurrence rule has no limit specified
 * \value CountLimit The recurrence rule specifies a certain count of repetitions in the series
 * \value DateLimit The recurrence rule specifies that the series ends at a particular date
 */

/*!
 * Constructs a QOrganizerRecurrenceRule object describing a weekly recurrence.
 */
QOrganizerRecurrenceRule::QOrganizerRecurrenceRule()
    : d(new QOrganizerRecurrenceRulePrivate)
{
}

/*!
 * Destroys the QOrganizerRecurrenceRule object.
 */
QOrganizerRecurrenceRule::~QOrganizerRecurrenceRule()
{
}

/*!
 * Constructs a QOrganizerRecurrenceRule object as a copy of \a other.
 */
QOrganizerRecurrenceRule::QOrganizerRecurrenceRule(const QOrganizerRecurrenceRule& other)
    : d(other.d)
{
}

/*!
 * Assigns this detail to be equal to \a other.
 */
QOrganizerRecurrenceRule& QOrganizerRecurrenceRule::operator=(const QOrganizerRecurrenceRule& other)
{
    this->d = other.d;
    return *this;
}
/*!
  Returns true if this recurrence rule is equal to the \a other; otherwise returns false.
 */
bool QOrganizerRecurrenceRule::operator==(const QOrganizerRecurrenceRule& other) const
{
    // if both ids are null then they are equal.
    if (d == 0 && other.d == 0)
        return true;

    if (d && other.d) {
        return  d->firstDayOfWeek == other.d->firstDayOfWeek
             && d->frequency == other.d->frequency
             && d->interval == other.d->interval
             && d->limitCount == other.d->limitCount
             && d->limitDate == other.d->limitDate
             && d->limitType == other.d->limitType
             && d->positions == other.d->positions
             && d->daysOfMonth == other.d->daysOfMonth
             && d->daysOfWeek == other.d->daysOfWeek
             && d->daysOfYear == other.d->daysOfYear
             && d->monthsOfYear == other.d->monthsOfYear
             && d->weeksOfYear == other.d->weeksOfYear;
    }
    return false;
}

/*!
 * Sets the frequency with which the item recurs to \a freq.
 *
 * This corresponds to the FREQ fragment in iCalendar's RRULE.
 */
void QOrganizerRecurrenceRule::setFrequency(Frequency freq)
{
    d->frequency = freq;
}

/*!
 * Returns the frequency with which the item recurs.  The default frequency is Invalid.
 */
QOrganizerRecurrenceRule::Frequency QOrganizerRecurrenceRule::frequency() const
{
    return d->frequency;
}

/*! Sets the "count" condition of the recurrence rule to \a count.  If an end-date was previously
 * set, it is removed as count and endDate are mutually exclusive.
 *
 * The "count" condition is the maximum number of times the item should recur.  Calling clearLimit() or setting this
 * to a negative value removes the count condition.
 *
 * This corresponds to the COUNT fragment in iCalendar's RRULE.
 * \sa clearLimit()
 */
void QOrganizerRecurrenceRule::setLimit(int count)
{
    if (count < 0) {
        clearLimit();
    } else {
        d->limitType = QOrganizerRecurrenceRule::CountLimit;
        d->limitCount = count;
        d->limitDate = QDate();
    }
}

/*! Sets the end-date condition of the recurrence rule to \a date.  If a "count" condition was
 * previously set, it is removed as count and endDate are mutually exclusive.
 *
 * The end-date condition is the date after which the item should not recur.  Calling clearLimit() or setting this to
 * the null date removes the end-date condition.
 *
 * This corresponds to the UNTIL fragment in iCalendar's RRULE.
 * \sa clearLimit()
 */
void QOrganizerRecurrenceRule::setLimit(const QDate& date)
{
    if (!date.isValid()) {
        clearLimit();
    } else {
        d->limitType = QOrganizerRecurrenceRule::DateLimit;
        d->limitDate = date;
        d->limitCount = -1;
    }
}

/*! Clear any recurrence rule limitation conditions.
 */
void QOrganizerRecurrenceRule::clearLimit()
{
    d->limitType = QOrganizerRecurrenceRule::NoLimit;
    d->limitCount = -1;
    d->limitDate = QDate();
}

/*! Returns the type of limitation specified by the recurrence rule.  The default limit type is NoLimit (ie.
 * unlimited)
 */
QOrganizerRecurrenceRule::LimitType QOrganizerRecurrenceRule::limitType() const
{
    return d->limitType;
}

/*! Returns the "count" condition specified by the recurrence rule.  The default count is -1 (ie.
 * unlimited)
 */
int QOrganizerRecurrenceRule::limitCount() const
{
    if (d->limitType == QOrganizerRecurrenceRule::CountLimit)
        return d->limitCount;
    return -1;
}


/*! Returns the end-date condition specified by the recurrence rule.  The default end date is the
 * null date (ie. no end date).
 */
QDate QOrganizerRecurrenceRule::limitDate() const
{
    if (d->limitType == QOrganizerRecurrenceRule::DateLimit)
        return d->limitDate;
    return QDate();
}

/*!
 * Sets the interval, between cycles of length given by frequency(), in which the item should recur
 * to \a interval.
 *
 * For example, if the frequency() is QOrganizerRecurrenceRule::Daily and the interval is set to
 * 2, the item should recur every second day.
 *
 * This corresponds to the INTERVAL fragment in iCalendar's RRULE.
 */
void QOrganizerRecurrenceRule::setInterval(int interval)
{
    if (interval > 0)
        d->interval = interval;
}

/*!
 * Returns the interval of recurrence.  The default interval is 1.
 */
int QOrganizerRecurrenceRule::interval() const
{
    return d->interval > 0 ? d->interval : 1;
}

// Defaults for the below: empty
// eg: Monday,Wednesday,Thursday == every Mon, Wed and Thurs.
/*!
 * Sets the days of week on which the item should recur to \a days.
 *
 * This corresponds to the BYDAY fragment in iCalendar's RRULE.
 */
void QOrganizerRecurrenceRule::setDaysOfWeek(const QSet<Qt::DayOfWeek>& days)
{
    d->daysOfWeek = days;
}

/*!
 * Returns a list of the days of week that the item should recur on.  If not set, this is the empty
 * list, which signifies that it should be implied, if necessary, by the day-of-week of the item.
 */
QSet<Qt::DayOfWeek> QOrganizerRecurrenceRule::daysOfWeek() const
{
    return d->daysOfWeek;
}

/*!
 * Sets the days of the month on which the item should recur to \a days.  Negative values in the list
 * represent the number of days from the end of the month.  eg. 1 represents the first day of the
 * month and -1 represents the last day of the month.
 *
 * This corresponds to the BYMONTHDAY fragment in iCalendar's RRULE.
 */
void QOrganizerRecurrenceRule::setDaysOfMonth(const QSet<int>& days)
{
    d->daysOfMonth = days;
}

/*!
 * Returns a list of the days of the month that the item should recur on.  If not set, this is the
 * empty list, which signifies that it should be implied, if necessary, by the day-of-month of the
 * item.
 */
QSet<int> QOrganizerRecurrenceRule::daysOfMonth() const
{
    return d->daysOfMonth;
}

/*!
 * Sets the days of the year on which the item should recur to \a days.  Negative values in the list
 * represent the number of days from the end of the year.  eg. 1 represents the first day of the
 * year and -1 represents the last day of the year.
 *
 * This corresponds to the BYYEARDAY fragment in iCalendar's RRULE.
 */
void QOrganizerRecurrenceRule::setDaysOfYear(const QSet<int>& days)
{
    d->daysOfYear = days;
}

/*!
 * Returns a list of the days of the year that the item should recur on.  If not set, this is the
 * empty list, which signifies that it should be implied, if necessary, by the day-of-year of the
 * item.
 */
QSet<int> QOrganizerRecurrenceRule::daysOfYear() const
{
    return d->daysOfYear;
}

/*!
 * Sets the months on which the item should recur to \a months.
 *
 * This corresponds to the BYMONTHDAY fragment in iCalendar's RRULE.
 */
void QOrganizerRecurrenceRule::setMonthsOfYear(const QSet<Month>& months)
{
    d->monthsOfYear = months;
}

/*!
 * Returns a list of the months that the item should recur on.  If not set, this is the
 * empty list, which signifies that it should be implied, if necessary, by the month of the
 * item.
 */
QSet<QOrganizerRecurrenceRule::Month> QOrganizerRecurrenceRule::monthsOfYear() const
{
    return d->monthsOfYear;
}

/*!
 * Sets the weeks of the year on which the item should recur to \a weeks.  Negative values in the list
 * represent the number of weeks from the end of the year.  eg. 1 represents the first week of the
 * year and -1 represents the last week of the year.
 *
 * This corresponds to the BYWEEK fragment in iCalendar's RRULE.
 */
void QOrganizerRecurrenceRule::setWeeksOfYear(const QSet<int>& weeks)
{
    d->weeksOfYear = weeks;
}

/*!
 * Returns a list of the weeks of the year that the item should recur on.  If not set, this is the
 * empty list, which signifies that it should be implied, if necessary, by the week number of the
 * item.
 */
QSet<int> QOrganizerRecurrenceRule::weeksOfYear() const
{
    return d->weeksOfYear;
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
void QOrganizerRecurrenceRule::setPositions(const QSet<int>& pos)
{
    d->positions = pos;
}

/*!
 * Returns the position-list of the recurrence rule.  If not set, this is the empty list, which
 * signifies that the recurrence dates should not be restricted by position.
 */
QSet<int> QOrganizerRecurrenceRule::positions() const
{
    return d->positions;
}

/*! Sets the day that the week starts on to \a day, for the purposes of calculating recurrences.
 * This is significant when the frequency is Weekly and the interval is greater than 1, or when
 * weekOfYear is set.  See the iCalendar spec for examples of its significance.
 *
 * This corresponds to the BYWKST fragment in iCalendar's RRULE.
 */
void QOrganizerRecurrenceRule::setFirstDayOfWeek(Qt::DayOfWeek day)
{
    d->firstDayOfWeek = day;
}

/*!
 * Returns the day that the week starts on.  If not set, this is Monday.
 */
Qt::DayOfWeek QOrganizerRecurrenceRule::firstDayOfWeek() const
{
    return d->firstDayOfWeek;
}


uint qHash(const QOrganizerRecurrenceRule& r)
{
    uint hash(0);
    static const unsigned int prime1 = 11;
    static const unsigned int prime2 = 31;
    static const unsigned int prime3 = 47;

    foreach(int day, r.daysOfMonth()){
        hash += day;
    }
    hash *= prime1;

    foreach(Qt::DayOfWeek day, r.daysOfWeek()){
        hash += day;
    }
    hash *= prime2;

    foreach(int day, r.daysOfYear()){
        hash += day;
    }
    hash *= prime3;

    foreach(QOrganizerRecurrenceRule::Month month, r.monthsOfYear()){
        hash += month;
    }
    hash *= prime1;

    foreach(int week, r.weeksOfYear()){
        hash += week;
    }
    hash *= prime2;

    foreach(int pos, r.positions()){
        hash += pos;
    }
    hash *= prime3;

    hash += static_cast<uint>(r.firstDayOfWeek())
          + static_cast<uint>(r.frequency())
          + r.interval()
          + r.limitCount()
          + qHash(r.limitDate())
          + static_cast<uint>(r.limitType());

    return hash * prime1;
}

QTM_END_NAMESPACE

