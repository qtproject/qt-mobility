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
  of a organizeritems backend.
 
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
        // found the organizeritem successfully.
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

QList<QDateTime> QOrganizerItemMemoryEngine::generateDateTimes(const QOrganizerItem& recurringItem, const QOrganizerItemRecurrenceRule& rrule, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount) const
{
    // If no endDateTime is given, we'll only generate items that occur within the next 4 years of periodStart.
    QList<QDateTime> retn;

    // call nextMatchingDate here in a loop until maxCount or rCount is reached, or until our timelimit (4yrs+periodStart) is reached.
    bool useMaxCount = periodEnd.isNull();             // if no period end given, just return maxCount instances.
    bool useRCount = rrule.count() > 0;                // if an rrule count is given, use it (as well or instead of, depending on which comes first) rrule.endDate().
    QDate realPeriodEnd = (useMaxCount ? periodStart.addDays(1461).date() : periodEnd.date()); // periodStart + 4 years
    QDate nextMatch = periodStart.date();
    while (true) {
        nextMatch = nextMatchingDate(nextMatch, realPeriodEnd, rrule);
        if (!nextMatch.isNull()) {
            QDateTime nmdt;
            nmdt.setDate(nextMatch);
            if (recurringItem.type() == QOrganizerItemType::TypeEvent) {
                QOrganizerEvent evt = recurringItem;
                nmdt.setTime(evt.startDateTime().time());
            } else if (recurringItem.type() == QOrganizerItemType::TypeTodo) {
                QOrganizerTodo todo = recurringItem;
                nmdt.setTime(todo.notBeforeDateTime().time()); // XXX TODO: verify this is the right field to use..?
            } else {
                // erm... not a recurring item in our schema...
                return QList<QDateTime>();
            }

            // XXX TODO: check that nmdt is within the required start and end times / period,
            // because our instance date generation code merely checks dates, not datetimes.

            // now ensure that we aren't overfilling our return set (depending on rCount and maxCount)
            bool lessThanMaxCount = false;
            if (!useMaxCount || (useMaxCount && retn.size() < maxCount))
                lessThanMaxCount = true;

            bool lessThanRCount = false;
            if (!useRCount || (useRCount && retn.size() < rrule.count()))
                lessThanRCount = true;

            if (lessThanMaxCount && lessThanRCount)
                retn.append(nmdt);
        }

        if ((useMaxCount && retn.size() == maxCount) || (useRCount && retn.size() == rrule.count()) || nextMatch.isNull()) {
            // we have reached our count of dates to return
            // or there are no more matches in the given time period.
            break;
        }

        // add one day to our nextmatch, so that the algorithm selects forwards.
        nextMatch = nextMatch.addDays(1);
    }

    return retn;
}

QDate QOrganizerItemMemoryEngine::nextMatchingDate(const QDate& currDate, const QDate& untilDate, const QOrganizerItemRecurrenceRule& rrule) const
{
    // gets the next date (starting from currDate INCLUSIVE) which matches the rrule but is less than untilDate
    // if none found, returns an invalid, null QDate.
    // if currDate > untilDate OR currDate > rrule.endDate, it will return an invalid, null QDate.
    // XXX TODO: observe the rrule.count() as well as endDate!  requires generation from startDate of rrule... hrm....

    // keep going until the untilDate or the endDate of the rrule, whichever comes first.
    QDate realUntilDate = untilDate;
    if (rrule.endDate().isValid() && rrule.endDate() < untilDate)
        realUntilDate = rrule.endDate();
    QDate startDate = rrule.startDate();

    if (currDate > realUntilDate)
        return QDate();

    QList<Qt::DayOfWeek> daysOfWeek = rrule.daysOfWeek();
    qSort(daysOfWeek);
    QList<int> daysOfMonth = rrule.daysOfMonth();
    qSort(daysOfMonth);
    QList<int> daysOfYear = rrule.daysOfYear();
    qSort(daysOfYear);
    QList<int> weeksOfYear = rrule.weeksOfYear();
    qSort(weeksOfYear);
    QList<QOrganizerItemRecurrenceRule::Month> monthsOfYear = rrule.months();
    qSort(monthsOfYear);

    QOrganizerItemRecurrenceRule::Frequency freq = rrule.frequency();
    int interval = rrule.interval();
    if (interval <= 1)
        interval = 1;

    QDate tempDate = currDate;
    while (tempDate <= realUntilDate) {
        // first, do FREQ+INTERVAL matching based on dateStart+rrule
        switch (freq) {
            case QOrganizerItemRecurrenceRule::Yearly:
            {
                if (tempDate.day() != startDate.day() || tempDate.month() != startDate.month()) {
                    // we haven't reached the right day of the year yet.
                    tempDate = tempDate.addDays(1);
                    continue;
                }

                int yearsDelta = tempDate.year() - startDate.year();
                if (yearsDelta % interval > 0) {
                    // this year doesn't match the interval.
                    tempDate = tempDate.addDays(tempDate.daysInYear() - tempDate.dayOfYear());
                    continue;
                }
            }
            break;

            case QOrganizerItemRecurrenceRule::Monthly:
            {
                if (tempDate.day() != startDate.day()) {
                    // we haven't reached the right day of the month yet.
                    tempDate = tempDate.addDays(1);
                    continue;
                }

                int monthsDelta = tempDate.month() - startDate.month() + (12 * (tempDate.year() - startDate.year()));
                if (monthsDelta % interval > 0) {
                    // this month doesn't match.
                    tempDate = tempDate.addDays(tempDate.daysInMonth() - tempDate.day());
                    continue;
                }
            }
            break;

            case QOrganizerItemRecurrenceRule::Weekly:
            {
                // Weekly is a tricky one, because of ISO week stuff.
                // first, get the tempDate to fall on a "7 days-from-startdate" multiple
                while (startDate.daysTo(tempDate) % 7 > 0)
                    tempDate = tempDate.addDays(1);

                int weekCount = 0;
                QDate weeklyDate = startDate;
                while (weeklyDate < tempDate) {
                    int weeklyDateWeek = weeklyDate.weekNumber();
                    weeklyDate = weeklyDate.addDays(1);
                    if (weeklyDate.weekNumber() > weeklyDateWeek) {
                        weekCount += 1;
                    }
                }

                if (weekCount % interval > 0) {
                    // this week doesn't match.
                    tempDate = tempDate.addDays(7);
                    continue;
                }
            }
            break;

            default: // daily
            {
                int daysDelta = startDate.daysTo(tempDate);
                if (daysDelta % interval > 0) {
                    // this day doesn't match.
                    tempDate = tempDate.addDays(daysDelta % interval);
                    continue;
                }
            }
            break;
        }

        // then, check months, weeksInYear, daysInMonth, daysInWeek, etc.
        if (monthsOfYear.size() > 0 && !monthsOfYear.contains(static_cast<QOrganizerItemRecurrenceRule::Month>(tempDate.month()))) {
            // this day didn't match.
            tempDate = tempDate.addDays(1);
            continue;
        }

        if (weeksOfYear.size() > 0 && !weeksOfYear.contains(tempDate.weekNumber())) {
            // this day didn't match.
            tempDate = tempDate.addDays(1);
            continue;
        }

        if (daysOfYear.size() > 0 && !daysOfYear.contains(tempDate.day())) {
            // this day didn't match.
            tempDate = tempDate.addDays(1);
            continue;
        }

        if (daysOfMonth.size() > 0 && !daysOfMonth.contains(tempDate.day())) {
            // this day didn't match.
            tempDate = tempDate.addDays(1);
            continue;
        }

        // XXX TODO: confirm that QDate::dayOfWeek() returns a weekday rather than weekdayNumber (ISO)
        if (daysOfWeek.size() > 0 && !daysOfWeek.contains(static_cast<Qt::DayOfWeek>(tempDate.dayOfWeek()))) {
            // this day didn't match.
            tempDate = tempDate.addDays(1);
            continue;
        }

        // matches every criteria
        if (tempDate >= startDate && tempDate <= realUntilDate)
            return tempDate;
        tempDate = tempDate.addDays(1);
    }

    // no match.
    return QDate();
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
            lowerBound = instance.notBeforeDateTime();
            upperBound = instance.dueDateTime();
        }

        if ((lowerBound.isNull() || lowerBound > periodStart) && (upperBound.isNull() || upperBound < periodEnd)) {
            // this occurrence fulfils the criteria.
            retn.append(currException);
        }
    }

    // then, generate the required (unchanged) instances from the generator.
    // before doing that, we have to find out all of the exception dates.
    QList<QDateTime> xdates = recur.exceptionDates();
    QList<QOrganizerItemRecurrenceRule> xrules = recur.exceptionRules();
    foreach (const QOrganizerItemRecurrenceRule& xrule, xrules) {
        bool xruleEndsBeforePeriodStarts = (!xrule.endDate().isNull()) && (xrule.endDate() < periodStart.date());
        bool xruleStartsAfterPeriodEnds = (!xrule.startDate().isNull()) && (xrule.startDate() > periodEnd.date());
        if (!xruleEndsBeforePeriodStarts && !xruleStartsAfterPeriodEnds) {
            // we cannot skip it, since it applies in the given time period.
            xdates += generateDateTimes(generator, xrule, periodStart, periodEnd, 50); // max count of 50 is arbitrary...
        }
    }

    // now generate a list of rdates (from the recurrenceDates and recurrenceRules)
    QList<QDateTime> rdates = recur.recurrenceDates();
    QList<QOrganizerItemRecurrenceRule> rrules = recur.recurrenceRules();
    foreach (const QOrganizerItemRecurrenceRule& rrule, rrules) {
        bool rruleEndsBeforePeriodStarts = (!rrule.endDate().isNull()) && (rrule.endDate() < periodStart.date());
        bool rruleStartsAfterPeriodEnds = (!rrule.startDate().isNull()) && (rrule.startDate() > periodEnd.date());
        if (!rruleEndsBeforePeriodStarts && !rruleStartsAfterPeriodEnds) {
            // we cannot skip it, since it applies in the given time period.
            rdates += generateDateTimes(generator, rrule, periodStart, periodEnd, 50); // max count of 50 is arbitrary...
        }
    }

    // now order the contents of retn by date
    qSort(rdates);

    // now for each rdate which isn't also an xdate
    foreach (const QDateTime& rdate, rdates) {
        if (!xdates.contains(rdate)) {
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
                && det.definitionName() != QOrganizerItemEventTimeRange::DefinitionName
                && det.definitionName() != QOrganizerItemTodoTimeRange::DefinitionName) {
            occDets.append(det);
        }
    }

    // add the detail which identifies exactly which instance this item is.
    QOrganizerItemInstanceOrigin currOrigin;
    currOrigin.setParentLocalId(generator.localId());
    currOrigin.setOriginalTimestamp(rdate);
    occDets.append(currOrigin);

    // save those details in the instance.
    foreach (const QOrganizerItemDetail& det, occDets) {
        QOrganizerItemDetail modifiable = det;
        instanceItem.saveDetail(&modifiable);
    }

    // and update the time range in the instance based on the current instance date
    if (generator.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerItemEventTimeRange etr = generator.detail<QOrganizerItemEventTimeRange>();
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
        QOrganizerItemTodoTimeRange ttr = generator.detail<QOrganizerItemTodoTimeRange>();
        QDateTime temp = ttr.dueDateTime();
        temp.setDate(rdate.date());
        ttr.setDueDateTime(temp);
        temp = ttr.notBeforeDateTime();
        temp.setDate(rdate.date());
        ttr.setNotBeforeDateTime(temp);
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

    /* First filter out organizeritems - check for default filter first */
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
bool QOrganizerItemMemoryEngine::saveItem(QOrganizerItem* theOrganizerItem, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // ensure that the organizeritem's details conform to their definitions
    if (!validateItem(*theOrganizerItem, error)) {
        return false;
    }

    // check to see if this organizeritem already exists
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

        // Looks ok, so continue
        d->m_organizeritems.replace(index, *theOrganizerItem);
        changeSet.insertChangedItem(theOrganizerItem->localId());
    } else {
        // id does not exist; if not zero, fail.
        QOrganizerItemId newId;
        newId.setManagerUri(managerUri());
        if (theOrganizerItem->id() != QOrganizerItemId() && theOrganizerItem->id() != newId) {
            // the ID is not empty, and it doesn't identify an existing organizeritem in our database either.
            *error = QOrganizerItemManager::DoesNotExistError;
            return false;
        }

        /* New organizeritem */
        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        ts.setCreated(ts.lastModified());
        setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        // update the organizeritem item - set its ID
        newId.setLocalId(++d->m_nextOrganizerItemId);
        theOrganizerItem->setId(newId);

        // finally, add the organizeritem to our internal lists and return
        d->m_organizeritems.append(*theOrganizerItem);                   // add organizeritem to list
        d->m_organizeritemIds.append(theOrganizerItem->localId());  // track the organizeritem id.

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

/*! \reimp */
bool QOrganizerItemMemoryEngine::saveItems(QList<QOrganizerItem>* organizeritems, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
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
        if (!saveItem(&current, changeSet, error)) {
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

/*! Removes the organizeritem identified by the given \a organizeritemId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizeritems and relationships as required */
bool QOrganizerItemMemoryEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    int index = d->m_organizeritemIds.indexOf(organizeritemId);

    if (index == -1) {
        *error = QOrganizerItemManager::DoesNotExistError;
        return false;
    }

    // remove the organizeritem from the lists.
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
    filling the \a changeSet with ids of changed organizeritems as required */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet. TODO.
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
    filling the \a changeSet with ids of changed organizeritems as required */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet...
    // we should also check to see if the changes have invalidated any organizeritem data, and add the ids of those organizeritems
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
            saveItems(&organizeritems, &errorMap, &operationError);

            updateItemSaveRequest(r, organizeritems, operationError, errorMap, QOrganizerItemAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerItemAbstractRequest::ItemRemoveRequest:
        {
            // this implementation provides scant information to the user
            // the operation either succeeds (all organizeritems matching the filter were removed)
            // or it fails (one or more organizeritems matching the filter could not be removed)
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

