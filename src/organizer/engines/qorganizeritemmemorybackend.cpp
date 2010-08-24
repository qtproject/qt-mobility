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

#include "qorganizeritemmanager.h"

#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemrequests.h"
#include "qorganizeritemchangeset.h"
#include "qorganizeritemdetails.h"
#include "qorganizerevent.h"
#include "qorganizereventoccurrence.h"
#include "qorganizertodo.h"
#include "qorganizertodooccurrence.h"

#include "qorganizeritemmemorybackend_p.h"

#include <QTimer>
#include <QUuid>
#include <QSharedData>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemMemoryEngine
  \brief The QOrganizerItemMemoryEngine class provides an in-memory implementation
  of an organizer item backend.
  \inmodule QtOrganizer
 
  It may be used as a reference implementation, or when persistent storage is not required.
 
  During construction, it will load the in-memory data associated with the memory store
  identified by the "id" parameter from the given parameters if it exists, or a new,
  anonymous store if it does not.
 
  Data stored in this engine is only available in the current process.
 
  This engine supports sharing, so an internal reference count is increased
  whenever a manager uses this backend, and is decreased when the manager
  no longer requires this engine.
 */

/* static data for manager class */
QMap<QString, QOrganizerItemMemoryEngineData*> QOrganizerItemMemoryEngine::engineDatas;

/*!
 * Factory function for creating a new in-memory backend, based
 * on the given \a parameters.
 *
 * The same engine will be returned for multiple calls with the
 * same value for the "id" parameter, while one of them is in scope.
 */
QOrganizerItemMemoryEngine* QOrganizerItemMemoryEngine::createMemoryEngine(const QMap<QString, QString>& parameters)
{
    bool anonymous = false;
    QString idValue = parameters.value(QLatin1String("id"));
    if (idValue.isNull() || idValue.isEmpty()) {
        // no store given?  new, anonymous store.
        idValue = QUuid::createUuid().toString();
        anonymous = true;
    }

    QOrganizerItemMemoryEngineData* data = engineDatas.value(idValue);
    if (data) {
        data->m_refCount.ref();
    } else {
        data = new QOrganizerItemMemoryEngineData();
        data->m_id = idValue;
        data->m_anonymous = anonymous;
        engineDatas.insert(idValue, data);
    }
    return new QOrganizerItemMemoryEngine(data);
}

/*!
 * Constructs a new in-memory backend which shares the given \a data with
 * other shared memory engines.
 */
QOrganizerItemMemoryEngine::QOrganizerItemMemoryEngine(QOrganizerItemMemoryEngineData* data)
    : d(data)
{
    d->m_sharedEngines.append(this);
}

/*! Frees any memory used by this engine */
QOrganizerItemMemoryEngine::~QOrganizerItemMemoryEngine()
{
    d->m_sharedEngines.removeAll(this);
    if (!d->m_refCount.deref()) {
        engineDatas.remove(d->m_id);
        delete d;
    }
}

/*! \reimp */
QString QOrganizerItemMemoryEngine::managerName() const
{
    return QLatin1String("memory");
}

/*! \reimp */
QMap<QString, QString> QOrganizerItemMemoryEngine::managerParameters() const
{
    QMap<QString, QString> params;
    params.insert(QLatin1String("id"), d->m_id);
    return params;
}

/*! \reimp */
QOrganizerItem QOrganizerItemMemoryEngine::item(const QOrganizerItemLocalId& organizeritemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    int index = d->m_organizeritemIds.indexOf(organizeritemId);
    if (index != -1) {
        // found the organizer item successfully.
        *error = QOrganizerItemManager::NoError;
        return d->m_organizeritems.at(index);
    }

    *error = QOrganizerItemManager::DoesNotExistError;
    return QOrganizerItem();
}

/*! \reimp */
QList<QOrganizerItemLocalId> QOrganizerItemMemoryEngine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    /* Special case the fast case */
    if (filter.type() == QOrganizerItemFilter::DefaultFilter && sortOrders.count() == 0) {
        return d->m_organizeritemIds;
    } else {
        QList<QOrganizerItem> clist = items(filter, sortOrders, QOrganizerItemFetchHint(), error);

        /* Extract the ids */
        QList<QOrganizerItemLocalId> ids;
        foreach(const QOrganizerItem& c, clist)
            ids.append(c.localId());

        return ids;
    }
}

/*! \reimp */
QList<QOrganizerItem> QOrganizerItemMemoryEngine::itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);
    Q_UNUSED(fetchHint);
    // not implemented. XXX TODO.
    *error = QOrganizerItemManager::NotSupportedError;
    return QList<QOrganizerItem>();
}

QList<QDateTime> QOrganizerItemMemoryEngine::generateDateTimes(const QDateTime& initialDateTime, QOrganizerItemRecurrenceRule rrule, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount) const
{
    QList<QDateTime> retn;

    if (periodEnd.isValid() || maxCount <= 0)
        maxCount = INT_MAX; // count of returned items is unlimited
    if (rrule.count() > 0)
        maxCount = qMin(maxCount, rrule.count());

    QDateTime realPeriodEnd(periodEnd);
    if (!periodEnd.isValid()) {
        // If no endDateTime is given, we'll only generate items that occur within the next 4 years of periodStart.
        realPeriodEnd.setDate(periodStart.date().addDays(1461));
        realPeriodEnd.setTime(periodStart.time());
    }
    if (rrule.endDate().isValid() && rrule.endDate() < realPeriodEnd.date())
        realPeriodEnd.setDate(rrule.endDate());

    QDate nextDate = periodStart.date();

    inferMissingCriteria(&rrule, initialDateTime.date());

    while (nextDate < realPeriodEnd.date()) {
        // Skip nextDate if it is not the right multiple of intervals away from initialDateTime.
        if (inIntervaledPeriod(nextDate, initialDateTime.date(), rrule.frequency(), rrule.interval(), rrule.weekStart())) {
            // Calculate the inclusive start and exclusive end of nextDate's week/month/year
            QDate subPeriodStart = firstDateInPeriod(nextDate, rrule.frequency(), rrule.weekStart());
            QDate subPeriodEnd = firstDateInNextPeriod(nextDate, rrule.frequency(), rrule.weekStart());

            // Compute matchesInPeriod to be the set of dates in the current week/month/year that match the rrule
            QList<QDate> matchesInPeriod = filterByPosition(
                    matchingDates(subPeriodStart, subPeriodEnd, rrule),
                    rrule.positions());

            // A final filter over the dates list before adding it to the returned list
            foreach (const QDate& match, matchesInPeriod) {
                nextDate = match;
                if (match >= realPeriodEnd.date() || retn.size() >= maxCount)
                    break;

                QDateTime generatedDateTime;
                generatedDateTime.setDate(match);
                generatedDateTime.setTime(initialDateTime.time());
                if (generatedDateTime >= periodStart) {
                    if (generatedDateTime < realPeriodEnd) {
                        retn.append(generatedDateTime);
                    } else {
                        // We've gone past the end of the period.  Ensure we break both the foreach and
                        // the while loop
                        nextDate = match.addDays(1);
                        break;
                    }
                }
            }
        }
        nextDate = firstDateInNextPeriod(nextDate, rrule.frequency(), rrule.weekStart());
    }

    return retn;
}

/*!
 * Determines if \a rrule is underspecified and if so, fills in missing information based on \a
 * initialDate.
 */
void QOrganizerItemMemoryEngine::inferMissingCriteria(QOrganizerItemRecurrenceRule* rrule, const QDate& initialDate) const
{
    switch (rrule->frequency()) {
        case QOrganizerItemRecurrenceRule::Weekly:
            if (rrule->daysOfWeek().isEmpty()) {
                // derive day of week
                QList<Qt::DayOfWeek> days;
                days.append(static_cast<Qt::DayOfWeek>(initialDate.dayOfWeek()));
                rrule->setDaysOfWeek(days);
            }
            break;
        case QOrganizerItemRecurrenceRule::Monthly:
            if (rrule->daysOfWeek().isEmpty() && rrule->daysOfMonth().isEmpty()) {
                // derive day of month
                QList<int> days;
                days.append(initialDate.day());
                rrule->setDaysOfMonth(days);
            }
            break;
        case QOrganizerItemRecurrenceRule::Yearly:
            if (rrule->months().isEmpty()
                    && rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of month and month of year
                QList<int> daysOfMonth;
                daysOfMonth.append(initialDate.day());
                rrule->setDaysOfMonth(daysOfMonth);
                QList<QOrganizerItemRecurrenceRule::Month> months;
                months.append(static_cast<QOrganizerItemRecurrenceRule::Month>(initialDate.month()));
                rrule->setMonths(months);
            } else if (!rrule->months().isEmpty()
                    && rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of month
                QList<int> daysOfMonth;
                daysOfMonth.append(initialDate.day());
                rrule->setDaysOfMonth(daysOfMonth);
            } else if (!rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of week
                QList<Qt::DayOfWeek> days;
                days.append(static_cast<Qt::DayOfWeek>(initialDate.dayOfWeek()));
                rrule->setDaysOfWeek(days);
            }
            break;
        case QOrganizerItemRecurrenceRule::Daily:
            break;
    }
}

/*!
 * Returns true iff the calendar period (specified by \a frequency) of \a date is an \a
 * interval-multiple of periods ahead of the calendar period of \a initialDate.  For Weekly
 * frequencies, \a firstDayOfWeek is used to determine when the week boundary is.
 * eg. If \a frequency is Monthly and \a interval is 3, then true is returned iff \a date is in the
 * same month as \a initialDate, in a month 3 months ahead, 6 months ahead, etc.
 */
bool QOrganizerItemMemoryEngine::inIntervaledPeriod(const QDate& date, const QDate& initialDate, QOrganizerItemRecurrenceRule::Frequency frequency, int interval, Qt::DayOfWeek firstDayOfWeek) const
{
    if (interval <= 1)
        return true;
    switch (frequency) {
        case QOrganizerItemRecurrenceRule::Yearly: {
            int yearsDelta = date.year() - initialDate.year();
            return (yearsDelta % interval == 0);
        }
        case QOrganizerItemRecurrenceRule::Monthly: {
            int monthsDelta = date.month() - initialDate.month() + (12 * (date.year() - initialDate.year()));
            return (monthsDelta % interval == 0);
        }
        case QOrganizerItemRecurrenceRule::Weekly: {
            // we need to adjust for the week start specified by the client if the interval is greater than 1
            // ie, every time we hit the day specified, we increment the week count.
            int weekCount = 0;
            QDate tempDate = initialDate;
            while (tempDate < date) {
                tempDate = tempDate.addDays(1);
                if (static_cast<Qt::DayOfWeek>(tempDate.dayOfWeek()) == firstDayOfWeek) {
                    weekCount += 1;
                }
            }
            return (weekCount % interval == 0);
        }
        case QOrganizerItemRecurrenceRule::Daily: {
            int daysDelta = initialDate.daysTo(date);
            return (daysDelta % interval == 0);
        }
        default:
            Q_ASSERT(false);
            return true;
    }
}

/*!
 * Returns the date which is the first date of the calendar period that \a date resides in.  eg. if
 * the \a frequency is Monthly, then this returns the first day of \a date's month.  If the \a
 * frequency is Weekly, then it returns the first day of \a date's week, considering the week to
 * start on \a firstDayOfWeek
 */
QDate QOrganizerItemMemoryEngine::firstDateInPeriod(const QDate& date, QOrganizerItemRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const
{
    QDate retn(date);
    switch (frequency) {
        case QOrganizerItemRecurrenceRule::Yearly:
            retn.setDate(date.year(), 1, 1);
            return retn;
        case QOrganizerItemRecurrenceRule::Monthly:
            retn.setDate(date.year(), date.month(), 1);
            return retn;
        case QOrganizerItemRecurrenceRule::Weekly:
            while (retn.dayOfWeek() != firstDayOfWeek) {
                retn = retn.addDays(-1);
            } 
            return retn;
        case QOrganizerItemRecurrenceRule::Daily:
            return retn;
        default:
            Q_ASSERT(false);
            return retn;
    }
}

/*!
 * Returns the date which is the first date of the next calendar period after \a date specified by
 * \a frequency.  eg. if \a frequency is Monthly, then this returns the first day of the next month.
 * If \a frequency is Weekly, then it returns the first \a firstDayOfWeek after \a date.
 */
QDate QOrganizerItemMemoryEngine::firstDateInNextPeriod(const QDate& date, QOrganizerItemRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const
{
    QDate retn(date);
    switch (frequency) {
        case QOrganizerItemRecurrenceRule::Yearly:
            retn.setDate(date.year()+1, 1, 1);
            return retn;
        case QOrganizerItemRecurrenceRule::Monthly:
            {
            int newMonth = date.month() + 1;
            int newYear = date.year() + (newMonth==13 ? 1 : 0);
            retn.setDate(newYear, newMonth==13 ? 1 : newMonth, 1);
            }
            return retn;
        case QOrganizerItemRecurrenceRule::Weekly:
            do {
                retn = retn.addDays(1);
            } while (retn.dayOfWeek() != firstDayOfWeek);
            return retn;
        case QOrganizerItemRecurrenceRule::Daily:
            retn = retn.addDays(1);
            return retn;
        default:
            Q_ASSERT(false);
            return retn;
    }
}

/*!
 * Returns a list of dates between \a periodStart (inclusive) and \a periodEnd (exclusive) which
 * match the \a rrule.  Only daysOfWeek, daysOfMonth, daysOfYear, weeksOfYear and months from the \a
 * rrule are matched.
 */
QList<QDate> QOrganizerItemMemoryEngine::matchingDates(const QDate& periodStart, const QDate& periodEnd, const QOrganizerItemRecurrenceRule& rrule) const
{
    QList<QDate> retn;

    QList<Qt::DayOfWeek> daysOfWeek = rrule.daysOfWeek();
    QList<int> daysOfMonth = rrule.daysOfMonth();
    QList<int> daysOfYear = rrule.daysOfYear();
    QList<int> weeksOfYear = rrule.weeksOfYear();
    QList<QOrganizerItemRecurrenceRule::Month> monthsOfYear = rrule.months();

    QDate tempDate = periodStart;
    while (tempDate < periodEnd) {
        if ((monthsOfYear.isEmpty() || monthsOfYear.contains(static_cast<QOrganizerItemRecurrenceRule::Month>(tempDate.month())))
                && (weeksOfYear.isEmpty() || weeksOfYear.contains(tempDate.weekNumber()))
                && (daysOfYear.isEmpty() || daysOfYear.contains(tempDate.dayOfYear()))
                && (daysOfMonth.isEmpty() || daysOfMonth.contains(tempDate.day()))
                && (daysOfWeek.isEmpty() || daysOfWeek.contains(static_cast<Qt::DayOfWeek>(tempDate.dayOfWeek())))) {
            retn.append(tempDate);
        }
        tempDate = tempDate.addDays(1);
    }
    return retn;
}

/*!
 * Returns a list of dates from \a dates which are at the indices specified by \a positions.
 * For positive values in \a positions, the values represent a 1-based index into \a dates.
 * For negative values, they represent indices counting from the end of \a dates (eg. -1 means the
 * last value of \a dates).
 */
QList<QDate> QOrganizerItemMemoryEngine::filterByPosition(const QList<QDate>& dates, const QList<int> positions) const
{
    if (positions.isEmpty()) {
        return dates;
    } else {
        QList<QDate> retn;
        foreach (int i, positions) {
            if (i >= 1 && i <= dates.size()) {
                // positions is 1-indexed, but the QList is 0-indexed
                retn.append(dates[i-1]);
            } else if (i <= -1 && i >= -dates.size()) {
                // for negative values, count from the end of the list
                retn.append(dates[dates.size() + i]);
            }
        }
        return retn;
    }
}

/*! \reimp */
QList<QOrganizerItem> QOrganizerItemMemoryEngine::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const
{
    // given the generating item, grab it's QOrganizerItemRecurrence detail (if it exists), and calculate all of the dates within the given period.
    // how would a real backend do this?
    // Also, should this also return the exception instances (ie, return any persistent instances with parent information == generator?)
    // XXX TODO: in detail validation, ensure that the referenced parent Id exists...

    if (periodStart > periodEnd) {
        *error = QOrganizerItemManager::BadArgumentError;
        return QList<QOrganizerItem>();
    }

    QList<QOrganizerItem> retn;
    QOrganizerItemRecurrence recur = generator.detail(QOrganizerItemRecurrence::DefinitionName);

    // first, retrieve all persisted instances (exceptions) which occur between the specified datetimes.
    QOrganizerItemDetailFilter parentFilter;
    parentFilter.setDetailDefinitionName(QOrganizerItemInstanceOrigin::DefinitionName, QOrganizerItemInstanceOrigin::FieldParentLocalId);
    parentFilter.setValue(generator.localId());
    QList<QOrganizerItem> persistedExceptions = items(parentFilter, QList<QOrganizerItemSortOrder>(), QOrganizerItemFetchHint(), error);
    foreach (const QOrganizerItem& currException, persistedExceptions) {
        QDateTime lowerBound;
        QDateTime upperBound;
        if (currException.type() == QOrganizerItemType::TypeEventOccurrence) {
            QOrganizerEventOccurrence instance = currException;
            lowerBound = instance.startDateTime();
            upperBound = instance.endDateTime();
        } else {
            QOrganizerTodoOccurrence instance = currException;
            lowerBound = instance.startDateTime();
            upperBound = instance.dueDateTime();
        }

        if ((lowerBound.isNull() || lowerBound > periodStart) && (upperBound.isNull() || upperBound < periodEnd)) {
            // this occurrence fulfils the criteria.
            retn.append(currException);
        }
    }

    QDateTime initialDateTime;
    if (generator.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEvent evt = generator;
        initialDateTime = evt.startDateTime();
    } else if (generator.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerTodo todo = generator;
        initialDateTime = todo.startDateTime();
    } else {
        // erm... not a recurring item in our schema...
        return QList<QOrganizerItem>() << generator;
    }

    // then, generate the required (unchanged) instances from the generator.
    // before doing that, we have to find out all of the exception dates.
    QList<QDate> xdates;
    foreach (const QDate& xdate, recur.exceptionDates()) {
        xdates += xdate;
    }
    QList<QOrganizerItemRecurrenceRule> xrules = recur.exceptionRules();
    foreach (const QOrganizerItemRecurrenceRule& xrule, xrules) {
        if ((xrule.endDate().isNull()) || (xrule.endDate() >= periodStart.date())) {
            // we cannot skip it, since it applies in the given time period.
            QList<QDateTime> xdatetimes = generateDateTimes(initialDateTime, xrule, periodStart, periodEnd, 50); // max count of 50 is arbitrary...
            foreach (const QDateTime& xdatetime, xdatetimes) {
                xdates += xdatetime.date();
            }
        }
    }

    // now generate a list of rdates (from the recurrenceDates and recurrenceRules)
    QList<QDateTime> rdates;
    foreach (const QDate& rdate, recur.recurrenceDates()) {
        rdates += QDateTime(rdate, initialDateTime.time());
    }
    QList<QOrganizerItemRecurrenceRule> rrules = recur.recurrenceRules();
    foreach (const QOrganizerItemRecurrenceRule& rrule, rrules) {
        if ((rrule.endDate().isNull()) || (rrule.endDate() >= periodStart.date())) {
            // we cannot skip it, since it applies in the given time period.
            rdates += generateDateTimes(initialDateTime, rrule, periodStart, periodEnd, 50); // max count of 50 is arbitrary...
        }
    }

    // now order the contents of retn by date
    qSort(rdates);

    // now for each rdate which isn't also an xdate
    foreach (const QDateTime& rdate, rdates) {
        if (!xdates.contains(rdate.date())) {
            // generate the required instance and add it to the return list.
            retn.append(generateInstance(generator, rdate));
        }
    }

    // and return the first maxCount entries.
    return retn.mid(0, maxCount);
}

QOrganizerItem QOrganizerItemMemoryEngine::generateInstance(const QOrganizerItem& generator, const QDateTime& rdate)
{
    QOrganizerItem instanceItem;
    if (generator.type() == QOrganizerItemType::TypeEvent) {
        instanceItem = QOrganizerEventOccurrence();
    } else {
        instanceItem = QOrganizerTodoOccurrence();
    }

    // XXX TODO: something better than this linear search...
    // Grab all details from the generator except the recurrence information, and event/todo time range
    QList<QOrganizerItemDetail> allDets = generator.details();
    QList<QOrganizerItemDetail> occDets;
    foreach (const QOrganizerItemDetail& det, allDets) {
        if (det.definitionName() != QOrganizerItemRecurrence::DefinitionName
                && det.definitionName() != QOrganizerEventTimeRange::DefinitionName
                && det.definitionName() != QOrganizerTodoTimeRange::DefinitionName) {
            occDets.append(det);
        }
    }

    // add the detail which identifies exactly which instance this item is.
    QOrganizerItemInstanceOrigin currOrigin;
    currOrigin.setParentLocalId(generator.localId());
    currOrigin.setOriginalDate(rdate.date());
    occDets.append(currOrigin);

    // save those details in the instance.
    foreach (const QOrganizerItemDetail& det, occDets) {
        // copy every detail except the type
        if (det.definitionName() != QOrganizerItemType::DefinitionName) {
            QOrganizerItemDetail modifiable = det;
            instanceItem.saveDetail(&modifiable);
        }
    }

    // and update the time range in the instance based on the current instance date
    if (generator.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEventTimeRange etr = generator.detail<QOrganizerEventTimeRange>();
        QDateTime temp = etr.startDateTime();
        temp.setDate(rdate.date());
        etr.setStartDateTime(temp);
        temp = etr.endDateTime();
        temp.setDate(rdate.date());
        etr.setEndDateTime(temp);
        instanceItem.saveDetail(&etr);
    }

    // for todo's?
    if (generator.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerTodoTimeRange ttr = generator.detail<QOrganizerTodoTimeRange>();
        QDateTime temp = ttr.dueDateTime();
        temp.setDate(rdate.date());
        ttr.setDueDateTime(temp);
        temp = ttr.startDateTime();
        temp.setDate(rdate.date());
        ttr.setStartDateTime(temp);
        instanceItem.saveDetail(&ttr);
    }

    return instanceItem;
}

/*! \reimp */
QList<QOrganizerItem> QOrganizerItemMemoryEngine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    Q_UNUSED(error);

    QList<QOrganizerItem> sorted;

    /* First filter out organizer items - check for default filter first */
    if (filter.type() == QOrganizerItemFilter::DefaultFilter) {
        foreach(const QOrganizerItem&c, d->m_organizeritems) {
            QOrganizerItemManagerEngine::addSorted(&sorted,c, sortOrders);
        }
    } else {
        foreach(const QOrganizerItem&c, d->m_organizeritems) {
            if (QOrganizerItemManagerEngine::testFilter(filter, c))
                QOrganizerItemManagerEngine::addSorted(&sorted,c, sortOrders);
        }
    }

    return sorted;
}

/*! Saves the given organizeritem \a theOrganizerItem, storing any error to \a error and
    filling the \a changeSet with ids of changed organizeritems as required */
bool QOrganizerItemMemoryEngine::saveItem(QOrganizerItem* theOrganizerItem, const QOrganizerCollectionLocalId& collectionId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(collectionId);
    // XXX TODO: save in in-memory collection.

    // ensure that the organizer item's details conform to their definitions
    if (!validateItem(*theOrganizerItem, error)) {
        return false;
    }

    // check to see if this organizer item already exists
    int index = d->m_organizeritemIds.indexOf(theOrganizerItem->id().localId());
    if (index != -1) {
        /* We also need to check that there are no modified create only details */
        QOrganizerItem oldOrganizerItem = d->m_organizeritems.at(index);

        if (oldOrganizerItem.type() != theOrganizerItem->type()) {
            *error = QOrganizerItemManager::AlreadyExistsError;
            return false;
        }

        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        QOrganizerItemManagerEngine::setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        if (!fixOccurrenceReferences(theOrganizerItem, error)) {
            return false;
        }

        // Looks ok, so continue
        d->m_organizeritems.replace(index, *theOrganizerItem);
        changeSet.insertChangedItem(theOrganizerItem->localId());
    } else {
        // id does not exist; if not zero, fail.
        QOrganizerItemId newId;
        newId.setManagerUri(managerUri());
        if (theOrganizerItem->id() != QOrganizerItemId() && theOrganizerItem->id() != newId) {
            // the ID is not empty, and it doesn't identify an existing organizer item in our database either.
            *error = QOrganizerItemManager::DoesNotExistError;
            return false;
        }

        /* New organizer item */
        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        ts.setCreated(ts.lastModified());
        setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        // update the organizer item - set its ID
        newId.setLocalId(++d->m_nextOrganizerItemId);
        theOrganizerItem->setId(newId);

        if (!fixOccurrenceReferences(theOrganizerItem, error)) {
            return false;
        }
        // set the guid if not set, and ensure that it's the same as the parent's
        if (theOrganizerItem->guid().isEmpty())
            theOrganizerItem->setGuid(QUuid::createUuid().toString());

        // if we're saving an exception occurrence, we need to add it's original date as an exdate to the parent.
        if (theOrganizerItem->type() == QOrganizerItemType::TypeEventOccurrence) {
            // update the event by adding an EX-DATE which corresponds to the original date of the occurrence being saved.
            QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
            QOrganizerItemInstanceOrigin origin = theOrganizerItem->detail<QOrganizerItemInstanceOrigin>();
            QOrganizerItemLocalId parentId = origin.parentLocalId();
            QOrganizerEvent parentEvent = item(parentId, QOrganizerItemFetchHint(), &tempError);
            QDate originalDate = origin.originalDate();
            QList<QDate> currentExceptionDates = parentEvent.exceptionDates();
            if (!currentExceptionDates.contains(originalDate)) {
                currentExceptionDates.append(originalDate);
                parentEvent.setExceptionDates(currentExceptionDates);
                int parentEventIndex = d->m_organizeritemIds.indexOf(parentEvent.localId());
                d->m_organizeritems.replace(parentEventIndex, parentEvent);
                changeSet.insertChangedItem(parentEvent.localId()); // is this correct?  it's an exception, so change parent?
            }
        } else if (theOrganizerItem->type() == QOrganizerItemType::TypeTodoOccurrence) {
            // update the todo by adding an EX-DATE which corresponds to the original date of the occurrence being saved.
            QOrganizerItemManager::Error tempError = QOrganizerItemManager::NoError;
            QOrganizerItemInstanceOrigin origin = theOrganizerItem->detail<QOrganizerItemInstanceOrigin>();
            QOrganizerItemLocalId parentId = origin.parentLocalId();
            QOrganizerTodo parentTodo = item(parentId, QOrganizerItemFetchHint(), &tempError);
            QDate originalDate = origin.originalDate();
            QList<QDate> currentExceptionDates = parentTodo.exceptionDates();
            if (!currentExceptionDates.contains(originalDate)) {
                currentExceptionDates.append(originalDate);
                parentTodo.setExceptionDates(currentExceptionDates);
                int parentTodoIndex = d->m_organizeritemIds.indexOf(parentTodo.localId());
                d->m_organizeritems.replace(parentTodoIndex, parentTodo);
                changeSet.insertChangedItem(parentTodo.localId()); // is this correct?  it's an exception, so change parent?
            }
        }

        // finally, add the organizer item to our internal lists and return
        d->m_organizeritems.append(*theOrganizerItem);              // add organizer item to list
        d->m_organizeritemIds.append(theOrganizerItem->localId());  // track the organizer item id.

        changeSet.insertAddedItem(theOrganizerItem->localId());

        // XXX TODO: prior to all of this, need to check:
        // 1) is it an Occurrence item?
        //      - if so, does it differ from the generated instance for that date?
        //          - if not different, return AlreadyExistsError
        //          - if different, save it, AND THEN UPDATE THE PARENT ITEM with EXDATE added!
        //      - if not, the current codepath is ok.
    }

    *error = QOrganizerItemManager::NoError;     // successful.
    return true;
}

/*!
 * For Occurrence type items, ensure the ParentLocalId and the Guid are set consistently.  Returns
 * false and sets \a error on error, returns true otherwise.
 */
bool QOrganizerItemMemoryEngine::fixOccurrenceReferences(QOrganizerItem* theItem, QOrganizerItemManager::Error* error)
{
    if (theItem->type() == QOrganizerItemType::TypeEventOccurrence
            || theItem->type() == QOrganizerItemType::TypeTodoOccurrence) {
        const QString guid = theItem->guid();
        QOrganizerItemInstanceOrigin instanceOrigin = theItem->detail<QOrganizerItemInstanceOrigin>();
        if (!instanceOrigin.originalDate().isValid()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return false;
        }
        QOrganizerItemLocalId parentId = instanceOrigin.parentLocalId();
        if (!guid.isEmpty()) {
            if (parentId != 0) {
                QOrganizerItemManager::Error tempError;
                QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
                if (guid != parentItem.guid()
                        || !typesAreRelated(theItem->type(), parentItem.type())) {
                    // parentId and guid are both set and inconsistent, or the parent is the wrong
                    // type
                    *error = QOrganizerItemManager::InvalidOccurrenceError;
                    return false;
                }
            } else {
                // guid set but not parentId
                // find an item with the given guid
                foreach (const QOrganizerItem& item, d->m_organizeritems) {
                    if (item.guid() == guid) {
                        parentId = item.localId();
                        break;
                    }
                }
                if (parentId == 0) {
                    // couldn't find an item with the given guid
                    *error = QOrganizerItemManager::InvalidOccurrenceError;
                    return false;
                }
                QOrganizerItemManager::Error tempError;
                QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
                if (!typesAreRelated(theItem->type(), parentItem.type())) {
                    // the parent is the wrong type
                    *error = QOrganizerItemManager::InvalidOccurrenceError;
                    return false;
                }
                // found a matching item - set the parentId of the occurrence
                QOrganizerItemInstanceOrigin origin = theItem->detail<QOrganizerItemInstanceOrigin>();
                origin.setParentLocalId(parentId);
                theItem->saveDetail(&origin);
            }
        } else if (parentId != 0) {
            QOrganizerItemManager::Error tempError;
            QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
            if (parentItem.guid().isEmpty()
                    || !typesAreRelated(theItem->type(), parentItem.type())) {
                // found the matching item but it has no guid, or it isn't the right type
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return false;
            }
            theItem->setGuid(parentItem.guid());
        } else {
            // neither parentId or guid is supplied
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return false;
        }
    }
    return true;
}

/*!
 * Returns true if and only if \a occurrenceType is the "Occurrence" version of \a parentType.
 */
bool QOrganizerItemMemoryEngine::typesAreRelated(const QString& occurrenceType, const QString& parentType)
{
    return ((parentType == QOrganizerItemType::TypeEvent
                && occurrenceType == QOrganizerItemType::TypeEventOccurrence)
            || (parentType == QOrganizerItemType::TypeTodo
                && occurrenceType == QOrganizerItemType::TypeTodoOccurrence));
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::saveItems(QList<QOrganizerItem>* organizeritems, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    if(errorMap) {
        errorMap->clear();
    }

    if (!organizeritems) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    QOrganizerItemChangeSet changeSet;
    QOrganizerItem current;
    QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
    for (int i = 0; i < organizeritems->count(); i++) {
        current = organizeritems->at(i);
        if (!saveItem(&current, collectionId, changeSet, error)) {
            operationError = *error;
            errorMap->insert(i, operationError);
        } else {
            (*organizeritems)[i] = current;
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);
    // return false if some error occurred
    return (*error == QOrganizerItemManager::NoError);
}

/*! Removes the organizer item identified by the given \a organizeritemId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items and relationships as required */
bool QOrganizerItemMemoryEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    int index = d->m_organizeritemIds.indexOf(organizeritemId);

    if (index == -1) {
        *error = QOrganizerItemManager::DoesNotExistError;
        return false;
    }

    // remove the organizer item from the lists.
    QOrganizerItemId thisOrganizerItem;
    thisOrganizerItem.setManagerUri(managerUri());
    thisOrganizerItem.setLocalId(organizeritemId);

    d->m_organizeritems.removeAt(index);
    d->m_organizeritemIds.removeAt(index);
    *error = QOrganizerItemManager::NoError;

    changeSet.insertRemovedItem(organizeritemId);
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::removeItems(const QList<QOrganizerItemLocalId>& organizeritemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    if (organizeritemIds.count() == 0) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }
    
    QOrganizerItemChangeSet changeSet;
    QOrganizerItemLocalId current;
    QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
    for (int i = 0; i < organizeritemIds.count(); i++) {
        current = organizeritemIds.at(i);
        if (!removeItem(current, changeSet, error)) {
            operationError = *error;
            errorMap->insert(i, operationError);
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);
    // return false if some errors occurred
    return (*error == QOrganizerItemManager::NoError);
}

/*! \reimp */
QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemMemoryEngine::detailDefinitions(const QString& organizeritemType, QOrganizerItemManager::Error* error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        d->m_definitions = QOrganizerItemManagerEngine::schemaDefinitions();
    }

    *error = QOrganizerItemManager::NoError;
    return d->m_definitions.value(organizeritemType);
}

/*! Saves the given detail definition \a def, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items as required */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet. // XXX TODO.
    Q_UNUSED(changeSet);

    if (!validateDefinition(def, error)) {
        return false;
    }

    detailDefinitions(organizeritemType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QOrganizerItemDetailDefinition> defsForThisType = d->m_definitions.value(organizeritemType);
    defsForThisType.insert(def.name(), def);
    d->m_definitions.insert(organizeritemType, defsForThisType);

    *error = QOrganizerItemManager::NoError;
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    bool retn = saveDetailDefinition(def, organizeritemType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! Removes the detail definition identified by \a definitionId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items as required */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet...
    // we should also check to see if the changes have invalidated any organizer item data, and add the ids of those organizer items
    // to the change set.  TODO!
    Q_UNUSED(changeSet);

    if (definitionId.isEmpty()) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    detailDefinitions(organizeritemType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QOrganizerItemDetailDefinition> defsForThisType = d->m_definitions.value(organizeritemType);
    bool success = defsForThisType.remove(definitionId);
    d->m_definitions.insert(organizeritemType, defsForThisType);
    if (success)
        *error = QOrganizerItemManager::NoError;
    else
        *error = QOrganizerItemManager::DoesNotExistError;
    return success;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    bool retn = removeDetailDefinition(definitionId, organizeritemType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! \reimp */
void QOrganizerItemMemoryEngine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::startRequest(QOrganizerItemAbstractRequest* req)
{
    if (!req)
        return false;
    updateRequestState(req, QOrganizerItemAbstractRequest::ActiveState);
    performAsynchronousOperation(req);
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    Q_UNUSED(req); // we can't cancel since we complete immediately
    return false;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    // in our implementation, we always complete any operation we start.
    Q_UNUSED(msecs);
    Q_UNUSED(req);

    return true;
}

/*!
 * This slot is called some time after an asynchronous request is started.
 * It performs the required operation, sets the result and returns.
 */
void QOrganizerItemMemoryEngine::performAsynchronousOperation(QOrganizerItemAbstractRequest *currentRequest)
{
    // store up changes, and emit signals once at the end of the (possibly batch) operation.
    QOrganizerItemChangeSet changeSet;

    // Now perform the active request and emit required signals.
    Q_ASSERT(currentRequest->state() == QOrganizerItemAbstractRequest::ActiveState);
    switch (currentRequest->type()) {
        case QOrganizerItemAbstractRequest::ItemFetchRequest:
        {
            QOrganizerItemFetchRequest* r = static_cast<QOrganizerItemFetchRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();
            QOrganizerItemFetchHint fetchHint = r->fetchHint();

            QOrganizerItemManager::Error operationError;
            QList<QOrganizerItem> requestedOrganizerItems = items(filter, sorting, fetchHint, &operationError);

            // update the request with the results.
            if (!requestedOrganizerItems.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateItemFetchRequest(r, requestedOrganizerItems, operationError, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest:
        {
            QOrganizerItemLocalIdFetchRequest* r = static_cast<QOrganizerItemLocalIdFetchRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItemLocalId> requestedOrganizerItemIds = itemIds(filter, sorting, &operationError);

            if (!requestedOrganizerItemIds.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateItemLocalIdFetchRequest(r, requestedOrganizerItemIds, operationError, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::ItemSaveRequest:
        {
            QOrganizerItemSaveRequest* r = static_cast<QOrganizerItemSaveRequest*>(currentRequest);
            QList<QOrganizerItem> organizeritems = r->items();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            saveItems(&organizeritems, r->collectionId(), &errorMap, &operationError);

            updateItemSaveRequest(r, organizeritems, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::ItemRemoveRequest:
        {
            // this implementation provides scant information to the user
            // the operation either succeeds (all organizer items matching the filter were removed)
            // or it fails (one or more organizer items matching the filter could not be removed)
            // if a failure occurred, the request error will be set to the most recent
            // error that occurred during the remove operation.
            QOrganizerItemRemoveRequest* r = static_cast<QOrganizerItemRemoveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QList<QOrganizerItemLocalId> organizeritemsToRemove = r->itemIds();
            QMap<int, QOrganizerItemManager::Error> errorMap;

            for (int i = 0; i < organizeritemsToRemove.size(); i++) {
                QOrganizerItemManager::Error tempError;
                removeItem(organizeritemsToRemove.at(i), changeSet, &tempError);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateItemRemoveRequest(r, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest:
        {
            QOrganizerItemDetailDefinitionFetchRequest* r = static_cast<QOrganizerItemDetailDefinitionFetchRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            QMap<QString, QOrganizerItemDetailDefinition> requestedDefinitions;
            QStringList names = r->definitionNames();
            if (names.isEmpty())
                names = detailDefinitions(r->itemType(), &operationError).keys(); // all definitions.

            QOrganizerItemManager::Error tempError;
            for (int i = 0; i < names.size(); i++) {
                QOrganizerItemDetailDefinition current = detailDefinition(names.at(i), r->itemType(), &tempError);
                requestedDefinitions.insert(names.at(i), current);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || !requestedDefinitions.isEmpty() || operationError != QOrganizerItemManager::NoError)
                updateDefinitionFetchRequest(r, requestedDefinitions, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest:
        {
            QOrganizerItemDetailDefinitionSaveRequest* r = static_cast<QOrganizerItemDetailDefinitionSaveRequest*>(currentRequest);
            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;
            QList<QOrganizerItemDetailDefinition> definitions = r->definitions();
            QList<QOrganizerItemDetailDefinition> savedDefinitions;

            QOrganizerItemManager::Error tempError;
            for (int i = 0; i < definitions.size(); i++) {
                QOrganizerItemDetailDefinition current = definitions.at(i);
                saveDetailDefinition(current, r->itemType(), changeSet, &tempError);
                savedDefinitions.append(current);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // update the request with the results.
            updateDefinitionSaveRequest(r, savedDefinitions, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest:
        {
            QOrganizerItemDetailDefinitionRemoveRequest* r = static_cast<QOrganizerItemDetailDefinitionRemoveRequest*>(currentRequest);
            QStringList names = r->definitionNames();

            QOrganizerItemManager::Error operationError = QOrganizerItemManager::NoError;
            QMap<int, QOrganizerItemManager::Error> errorMap;

            for (int i = 0; i < names.size(); i++) {
                QOrganizerItemManager::Error tempError;
                removeDetailDefinition(names.at(i), r->itemType(), changeSet, &tempError);

                if (tempError != QOrganizerItemManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // there are no results, so just update the status with the error.
            updateDefinitionRemoveRequest(r, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        default: // unknown request type.
        break;
    }

    // now emit any signals we have to emit
    d->emitSharedSignals(&changeSet);
}

/*!
 * \reimp
 */
bool QOrganizerItemMemoryEngine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& organizeritemType) const
{
    if (!supportedItemTypes().contains(organizeritemType))
        return false;

    switch (feature) {
        case QOrganizerItemManager::MutableDefinitions:
            return true;
        case QOrganizerItemManager::Anonymous:
            return d->m_anonymous;

        default:
            return false;
    }
}

/*!
 * \reimp
 */
QList<QVariant::Type> QOrganizerItemMemoryEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);
    st.append(QVariant::Time);
    st.append(QVariant::Bool);
    st.append(QVariant::Char);
    st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::LongLong);
    st.append(QVariant::ULongLong);
    st.append(QVariant::Double);

    return st;
}

/*!
 * The function returns true if the backend natively supports the given filter \a filter, otherwise false.
 */
bool QOrganizerItemMemoryEngine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    Q_UNUSED(filter);

    // Until we add hashes for common stuff, fall back to slow code
    return false;
}

#include "moc_qorganizeritemmemorybackend_p.cpp"

QTM_END_NAMESPACE

