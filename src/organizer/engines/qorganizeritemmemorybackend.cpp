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

#include "qorganizermanager.h"

#include "qorganizeritemdetaildefinition.h"
#include "qorganizermanagerengine.h"
#include "qorganizerabstractrequest.h"
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
#include <QStringBuilder>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemMemoryEngine
  \brief The QOrganizerItemMemoryEngine class provides an in-memory implementation
  of an organizer item backend.
  \inmodule QtOrganizer
  \internal
 
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
  \class QOrganizerItemMemoryEngineId
  \brief The QOrganizerItemMemoryEngineId class provides an id which uniquely identifies
  a QOrganizerItem stored within a collection stored within a a QOrganizerItemMemoryEngine.

  \internal
  It may be used as a reference implementation, although since different platforms
  have different semantics for ids (datastore-unique versus calendar-unique, etc),
  the precise implementation required may differ.
 */
QOrganizerItemMemoryEngineId::QOrganizerItemMemoryEngineId()
    : QOrganizerItemEngineId(), m_collectionId(0), m_itemId(0)
{
}

QOrganizerItemMemoryEngineId::QOrganizerItemMemoryEngineId(quint32 collectionId, quint32 itemId, const QString& managerUri)
    : QOrganizerItemEngineId(), m_collectionId(collectionId), m_itemId(itemId), m_managerUri(managerUri)
{
}

QOrganizerItemMemoryEngineId::~QOrganizerItemMemoryEngineId()
{
}

QOrganizerItemMemoryEngineId::QOrganizerItemMemoryEngineId(const QOrganizerItemMemoryEngineId& other)
    : QOrganizerItemEngineId(), m_collectionId(other.m_collectionId), m_itemId(other.m_itemId), m_managerUri(other.m_managerUri)
{
}

QOrganizerItemMemoryEngineId::QOrganizerItemMemoryEngineId(const QString& idString)
    : QOrganizerItemEngineId()
{
    int temp = 0;
    int colonIndex = idString.indexOf(QLatin1String(":"), 0);
    m_collectionId = idString.mid(temp, colonIndex).toUInt();
    temp = colonIndex + 1;
    colonIndex = idString.indexOf(QLatin1String(":"), temp);
    m_itemId = idString.mid(temp, (colonIndex-temp)).toUInt();
    temp = colonIndex + 1;
    m_managerUri = idString.mid(temp);
}

bool QOrganizerItemMemoryEngineId::isEqualTo(const QOrganizerItemEngineId* other) const
{
    // note: we don't need to check the collectionId because itemIds in the memory
    // engine are unique regardless of which collection the item is in; also, we
    // don't need to check the managerUri, because this function is not called if
    // the managerUris don't match.
    if (m_itemId != static_cast<const QOrganizerItemMemoryEngineId*>(other)->m_itemId)
        return false;
    return true;
}

bool QOrganizerItemMemoryEngineId::isLessThan(const QOrganizerItemEngineId* other) const
{
    // order by collection, then by item in collection.
    const QOrganizerItemMemoryEngineId* otherPtr = static_cast<const QOrganizerItemMemoryEngineId*>(other);
    if (m_managerUri < otherPtr->m_managerUri)
        return true;
    if (m_collectionId < otherPtr->m_collectionId)
        return true;
    if (m_collectionId == otherPtr->m_collectionId)
        return (m_itemId < otherPtr->m_itemId);
    return false;
}

QString QOrganizerItemMemoryEngineId::managerUri() const
{
    return m_managerUri;
}

QString QOrganizerItemMemoryEngineId::toString() const
{
    return (QString::number(m_collectionId) % QLatin1Char(':') % QString::number(m_itemId) % QLatin1Char(':') % m_managerUri);
}

QOrganizerItemEngineId* QOrganizerItemMemoryEngineId::clone() const
{
    return new QOrganizerItemMemoryEngineId(m_collectionId, m_itemId, m_managerUri);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug& QOrganizerItemMemoryEngineId::debugStreamOut(QDebug& dbg) const
{
    dbg.nospace() << "QOrganizerItemMemoryEngineId(" << m_collectionId << ", " << m_itemId << "," << m_managerUri << ")";
    return dbg.maybeSpace();
}
#endif

uint QOrganizerItemMemoryEngineId::hash() const
{
    // Note: doesn't need to be unique, since == ensures difference.
    // Hash function merely determines distribution in a hash table.
    // We certainly don't want to qHash(managerUri) since that's slow.
    // We don't need anything other than itemId in the memory engine,
    // since it's unique across collections.
    return m_itemId;
}

/*!
  \class QOrganizerCollectionMemoryEngineId
  \brief The QOrganizerCollectionMemoryEngineId class provides an id which uniquely identifies
  a QOrganizerCollection stored within a collection stored within a a QOrganizerCollectionMemoryEngine.
  \internal

  It may be used as a reference implementation, although since different platforms
  have different semantics for ids (datastore-unique versus calendar-unique, etc),
  the precise implementation required may differ.
 */
QOrganizerCollectionMemoryEngineId::QOrganizerCollectionMemoryEngineId()
    : QOrganizerCollectionEngineId(), m_collectionId(0)
{
}

QOrganizerCollectionMemoryEngineId::QOrganizerCollectionMemoryEngineId(quint32 collectionId, const QString& managerUri)
    : QOrganizerCollectionEngineId(), m_collectionId(collectionId), m_managerUri(managerUri)
{
}

QOrganizerCollectionMemoryEngineId::QOrganizerCollectionMemoryEngineId(const QOrganizerCollectionMemoryEngineId& other)
    : QOrganizerCollectionEngineId(), m_collectionId(other.m_collectionId)
{
}

QOrganizerCollectionMemoryEngineId::QOrganizerCollectionMemoryEngineId(const QString& idString)
    : QOrganizerCollectionEngineId()
{
    int colonIndex = idString.indexOf(QLatin1String(":"));
    m_collectionId = idString.mid(0, colonIndex).toUInt();
    m_managerUri = idString.mid(colonIndex+1);
}

QOrganizerCollectionMemoryEngineId::~QOrganizerCollectionMemoryEngineId()
{
}

bool QOrganizerCollectionMemoryEngineId::isEqualTo(const QOrganizerCollectionEngineId* other) const
{
    // note: we don't need to check the managerUri because this function is not called
    // if the managerUris are different.
    if (m_collectionId != static_cast<const QOrganizerCollectionMemoryEngineId*>(other)->m_collectionId)
        return false;
    return true;
}

bool QOrganizerCollectionMemoryEngineId::isLessThan(const QOrganizerCollectionEngineId* other) const
{
    // order by collection, then by item in collection.
    const QOrganizerCollectionMemoryEngineId* otherPtr = static_cast<const QOrganizerCollectionMemoryEngineId*>(other);
    if (m_managerUri < otherPtr->m_managerUri)
        return true;
    if (m_collectionId < otherPtr->m_collectionId)
        return true;
    return false;
}

QString QOrganizerCollectionMemoryEngineId::managerUri() const
{
    return m_managerUri;
}

QString QOrganizerCollectionMemoryEngineId::toString() const
{
    return (QString::number(m_collectionId) % QLatin1Char(':') % m_managerUri);
}

QOrganizerCollectionEngineId* QOrganizerCollectionMemoryEngineId::clone() const
{
    return new QOrganizerCollectionMemoryEngineId(m_collectionId, m_managerUri);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug& QOrganizerCollectionMemoryEngineId::debugStreamOut(QDebug& dbg) const
{
    dbg.nospace() << "QOrganizerCollectionMemoryEngineId(" << m_collectionId << "," << m_managerUri << ")";
    return dbg.maybeSpace();
}
#endif

uint QOrganizerCollectionMemoryEngineId::hash() const
{
    return m_collectionId;
}

QOrganizerItemMemoryEngineData::QOrganizerItemMemoryEngineData()
    : QSharedData(),
    m_refCount(QAtomicInt(1)),
    m_nextOrganizerItemId(1),
    m_nextOrganizerCollectionId(2),
    m_anonymous(false)
{

}

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

    // the default collection always exists.
    if (d->m_organizerCollectionIds.isEmpty()) {
        d->m_managerUri = managerUri();
        QOrganizerCollectionId defaultId = QOrganizerCollectionId(new QOrganizerCollectionMemoryEngineId(1, d->m_managerUri));
        QOrganizerCollection defaultCollection;
        defaultCollection.setId(defaultId);
        defaultCollection.setMetaData(QOrganizerCollection::KeyName, QString(QLatin1String("Default Collection")));
        d->m_organizerCollectionIds << defaultId;
        d->m_organizerCollections << defaultCollection;
    }
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
QOrganizerItem QOrganizerItemMemoryEngine::item(const QOrganizerItemId& organizeritemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    QOrganizerItem retn(item(organizeritemId));
    *error = retn.isEmpty() ? QOrganizerManager::DoesNotExistError : QOrganizerManager::NoError;
    return retn;
}

/*! \reimp */
QList<QOrganizerItemId> QOrganizerItemMemoryEngine::itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerManager::Error* error) const
{
    /* Special case the fast case */
    if (startDate.isNull() && endDate.isNull() && filter.type() == QOrganizerItemFilter::DefaultFilter && sortOrders.count() == 0) {
        return d->m_idToItemHash.keys();
    } else {
        return QOrganizerManager::extractIds(itemsForExport(startDate, endDate, filter, sortOrders, QOrganizerItemFetchHint(), error));
    }
}

QList<QDateTime> QOrganizerItemMemoryEngine::generateDateTimes(const QDateTime& initialDateTime, QOrganizerRecurrenceRule rrule, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount) const
{
    QList<QDateTime> retn;

    if (periodEnd.isValid() || maxCount <= 0)
        maxCount = INT_MAX; // count of returned items is unlimited
    if (rrule.limitType() == QOrganizerRecurrenceRule::CountLimit)
        maxCount = qMin(maxCount, rrule.limitCount());

    QDateTime realPeriodEnd(periodEnd);
    if (rrule.limitType() == QOrganizerRecurrenceRule::DateLimit
            && rrule.limitDate() < realPeriodEnd.date()) {
        realPeriodEnd.setDate(rrule.limitDate());
        realPeriodEnd.setTime(QTime(23,59,59,999)); // the last instant of the limit date, since it's prior to the periodEnd.
    }

    QDate nextDate = periodStart.date();

    inferMissingCriteria(&rrule, initialDateTime.date());

    while (nextDate <= realPeriodEnd.date()) {
        // Skip nextDate if it is not the right multiple of intervals away from initialDateTime.
        if (inMultipleOfInterval(nextDate, initialDateTime.date(), rrule.frequency(), rrule.interval(), rrule.firstDayOfWeek(), maxCount)) {
            // Calculate the inclusive start and inclusive end of nextDate's week/month/year
            QDate subPeriodStart(firstDateInPeriod(nextDate, rrule.frequency(), rrule.firstDayOfWeek()));
            QDate subPeriodEnd(firstDateInNextPeriod(nextDate, rrule.frequency(), rrule.firstDayOfWeek()).addDays(-1));

            // Compute matchesInPeriod to be the set of dates in the current week/month/year that match the rrule
            QList<QDate> matchesInPeriod(filterByPosition(
                    matchingDates(subPeriodStart, subPeriodEnd, rrule),
                    rrule.positions()));

            // A final filter over the dates list before adding it to the returned list
            foreach (const QDate& match, matchesInPeriod) {
                nextDate = match;
                if (match > realPeriodEnd.date() || retn.size() >= maxCount) {
                    break;
                }

                QDateTime generatedDateTime;
                generatedDateTime.setDate(match);
                generatedDateTime.setTime(initialDateTime.time());
                if (generatedDateTime >= periodStart && generatedDateTime <= realPeriodEnd) {
                    retn.append(generatedDateTime);
                } else {
                    // We've gone past the end of the period.  Ensure we break both the foreach and
                    // the while loop
                    nextDate = match.addDays(1);
                    break;
                }
            }
        }
        nextDate = firstDateInNextPeriod(nextDate, rrule.frequency(), rrule.firstDayOfWeek());
    }

    return retn;
}

/*!
 * Determines if \a rrule is underspecified and if so, fills in missing information based on \a
 * initialDate.
 */
void QOrganizerItemMemoryEngine::inferMissingCriteria(QOrganizerRecurrenceRule* rrule, const QDate& initialDate) const
{
    switch (rrule->frequency()) {
        case QOrganizerRecurrenceRule::Weekly:
            if (rrule->daysOfWeek().isEmpty()) {
                // derive day of week
                QSet<Qt::DayOfWeek> days;
                days << static_cast<Qt::DayOfWeek>(initialDate.dayOfWeek());
                rrule->setDaysOfWeek(days);
            }
            break;
        case QOrganizerRecurrenceRule::Monthly:
            if (rrule->daysOfWeek().isEmpty() && rrule->daysOfMonth().isEmpty()) {
                // derive day of month
                QSet<int> days;
                days << initialDate.day();
                rrule->setDaysOfMonth(days);
            }
            break;
        case QOrganizerRecurrenceRule::Yearly:
            if (rrule->monthsOfYear().isEmpty()
                    && rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of month and month of year
                QSet<int> daysOfMonth;
                daysOfMonth << initialDate.day();
                rrule->setDaysOfMonth(daysOfMonth);
                QSet<QOrganizerRecurrenceRule::Month> months;
                months << static_cast<QOrganizerRecurrenceRule::Month>(initialDate.month());
                rrule->setMonthsOfYear(months);
            } else if (!rrule->monthsOfYear().isEmpty()
                    && rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of month
                QSet<int> daysOfMonth;
                daysOfMonth << initialDate.day();
                rrule->setDaysOfMonth(daysOfMonth);
            } else if (!rrule->weeksOfYear().isEmpty()
                    && rrule->daysOfYear().isEmpty()
                    && rrule->daysOfMonth().isEmpty()
                    && rrule->daysOfWeek().isEmpty()) {
                // derive day of week
                QSet<Qt::DayOfWeek> days;
                days << static_cast<Qt::DayOfWeek>(initialDate.dayOfWeek());
                rrule->setDaysOfWeek(days);
            }
            break;
        case QOrganizerRecurrenceRule::Daily:
            break;
        case QOrganizerRecurrenceRule::Invalid:
            Q_ASSERT(false);
    }
}

/*!
 * Returns true iff the calendar period (specified by \a frequency) of \a date is an \a
 * interval-multiple of periods ahead of the calendar period of \a initialDate. If the recurrence
 * have a \a maxCount then take it into account. For Weekly frequencies, \a firstDayOfWeek is used
 * to determine when the week boundary is.
 * eg. If \a frequency is Monthly and \a interval is 3, then true is returned iff \a date is in the
 * same month as \a initialDate, in a month 3 months ahead, 6 months ahead, etc.
 */
bool QOrganizerItemMemoryEngine::inMultipleOfInterval(const QDate& date, const QDate& initialDate, QOrganizerRecurrenceRule::Frequency frequency, int interval, Qt::DayOfWeek firstDayOfWeek, int maxCount) const
{
    switch (frequency) {
        case QOrganizerRecurrenceRule::Yearly: {
            int yearsDelta = date.year() - initialDate.year();
            if (maxCount && maxCount * interval <= yearsDelta)
                return false;
            return (yearsDelta % interval == 0);
        }
        case QOrganizerRecurrenceRule::Monthly: {
            int monthsDelta = date.month() - initialDate.month() + (12 * (date.year() - initialDate.year()));
            if (maxCount && maxCount * interval <= monthsDelta)
                return false;
            return (monthsDelta % interval == 0);
        }
        case QOrganizerRecurrenceRule::Weekly: {
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
            if (maxCount && maxCount * interval <= weekCount)
                return false;
            return (weekCount % interval == 0);
        }
        case QOrganizerRecurrenceRule::Daily: {
            int daysDelta = initialDate.daysTo(date);
            if (maxCount && maxCount * interval <= daysDelta)
                return false;
            return (daysDelta % interval == 0);
        }
        case QOrganizerRecurrenceRule::Invalid:
            Q_ASSERT(false);
    }
    return true;
}

/*!
 * Returns the date which is the first date of the calendar period that \a date resides in.  eg. if
 * the \a frequency is Monthly, then this returns the first day of \a date's month.  If the \a
 * frequency is Weekly, then it returns the first day of \a date's week, considering the week to
 * start on \a firstDayOfWeek
 */
QDate QOrganizerItemMemoryEngine::firstDateInPeriod(const QDate& date, QOrganizerRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const
{
    QDate retn(date);
    switch (frequency) {
        case QOrganizerRecurrenceRule::Yearly:
            retn.setDate(date.year(), 1, 1);
            return retn;
        case QOrganizerRecurrenceRule::Monthly:
            retn.setDate(date.year(), date.month(), 1);
            return retn;
        case QOrganizerRecurrenceRule::Weekly:
            while (retn.dayOfWeek() != firstDayOfWeek) {
                retn = retn.addDays(-1);
            } 
            return retn;
        case QOrganizerRecurrenceRule::Daily:
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
QDate QOrganizerItemMemoryEngine::firstDateInNextPeriod(const QDate& date, QOrganizerRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const
{
    QDate retn(date);
    switch (frequency) {
        case QOrganizerRecurrenceRule::Yearly:
            retn.setDate(date.year()+1, 1, 1);
            return retn;
        case QOrganizerRecurrenceRule::Monthly:
            {
            int newMonth = date.month() + 1;
            int newYear = date.year() + (newMonth==13 ? 1 : 0);
            retn.setDate(newYear, newMonth==13 ? 1 : newMonth, 1);
            }
            return retn;
        case QOrganizerRecurrenceRule::Weekly:
            do {
                retn = retn.addDays(1);
            } while (retn.dayOfWeek() != firstDayOfWeek);
            return retn;
        case QOrganizerRecurrenceRule::Daily:
            retn = retn.addDays(1);
            return retn;
        case QOrganizerRecurrenceRule::Invalid:
            Q_ASSERT(false);
    }
    return retn;
}

/*!
 * Returns a list of dates between \a periodStart (inclusive) and \a periodEnd (inclusive) which
 * match the \a rrule.  Only daysOfWeek, daysOfMonth, daysOfYear, weeksOfYear and months from the \a
 * rrule are matched.
 */
QList<QDate> QOrganizerItemMemoryEngine::matchingDates(const QDate& periodStart, const QDate& periodEnd, const QOrganizerRecurrenceRule& rrule) const
{
    QList<QDate> retn;

    QSet<Qt::DayOfWeek> daysOfWeek = rrule.daysOfWeek();
    QSet<int> daysOfMonth = rrule.daysOfMonth();
    QSet<int> daysOfYear = rrule.daysOfYear();
    QSet<int> weeksOfYear = rrule.weeksOfYear();
    QSet<QOrganizerRecurrenceRule::Month> monthsOfYear = rrule.monthsOfYear();

    QDate tempDate = periodStart;
    while (tempDate <= periodEnd) {
        if ((monthsOfYear.isEmpty() || monthsOfYear.contains(static_cast<QOrganizerRecurrenceRule::Month>(tempDate.month())))
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
QList<QDate> QOrganizerItemMemoryEngine::filterByPosition(const QList<QDate>& dates, const QSet<int> positions) const
{
    if (positions.isEmpty()) {
        return dates;
    }

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

bool QOrganizerItemMemoryEngine::itemHasReccurence(const QOrganizerItem& oi) const
{
    if (oi.type() == QOrganizerItemType::TypeEvent || oi.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerItemRecurrence recur = oi.detail(QOrganizerItemRecurrence::DefinitionName);
        return !recur.recurrenceDates().isEmpty() || !recur.recurrenceRules().isEmpty();
    }

    return false;
}


QList<QOrganizerItem> QOrganizerItemMemoryEngine::internalItemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, bool includeExceptions, QOrganizerManager::Error* error) const
{
    // given the generating item, grab it's QOrganizerItemRecurrence detail (if it exists), and calculate all of the dates within the given period.
    // how would a real backend do this?
    // Also, should this also return the exception instances (ie, return any persistent instances with parent information == parent item?)
    // XXX TODO: in detail validation, ensure that the referenced parent Id exists...

    QDateTime realPeriodStart(periodStart);
    QDateTime realPeriodEnd(periodEnd);
    QDateTime initialDateTime;
    if (parentItem.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEvent evt = parentItem;
        initialDateTime = evt.startDateTime();
    } else if (parentItem.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerTodo todo = parentItem;
        initialDateTime = todo.startDateTime();
    } else {
        // erm... not a recurring item in our schema...
        return QList<QOrganizerItem>();
    }

    if (initialDateTime > realPeriodStart)
        realPeriodStart = initialDateTime;
    if (!periodEnd.isValid()) {
        // If no endDateTime is given, we'll only generate items that occur within the next 4 years of realPeriodStart.
        realPeriodEnd.setDate(realPeriodStart.date().addDays(1461));
        realPeriodEnd.setTime(realPeriodStart.time());
    }
    if (realPeriodStart > realPeriodEnd) {
        *error = QOrganizerManager::BadArgumentError;
        return QList<QOrganizerItem>();
    }

    QList<QOrganizerItem> retn;
    QOrganizerItemRecurrence recur = parentItem.detail(QOrganizerItemRecurrence::DefinitionName);

    if (includeExceptions) {
        // first, retrieve all persisted instances (exceptions) which occur between the specified datetimes.
        foreach (const QOrganizerItem& item, d->m_idToItemHash) {
            if (item.detail<QOrganizerItemParent>().parentId() == parentItem.id()) {
                QDateTime lowerBound;
                QDateTime upperBound;
                if (item.type() == QOrganizerItemType::TypeEventOccurrence) {
                    QOrganizerEventOccurrence instance = item;
                    lowerBound = instance.startDateTime();
                    upperBound = instance.endDateTime();
                } else {
                    QOrganizerTodoOccurrence instance = item;
                    lowerBound = instance.startDateTime();
                    upperBound = instance.dueDateTime();
                }

                if ((lowerBound.isNull() || lowerBound >= realPeriodStart) && (upperBound.isNull() || upperBound <= realPeriodEnd)) {
                    // this occurrence fulfils the criteria.
                    retn.append(item);
                }
            }
        }
    }

    // then, generate the required (unchanged) instances from the parentItem.
    // before doing that, we have to find out all of the exception dates.
    QList<QDate> xdates;
    foreach (const QDate& xdate, recur.exceptionDates()) {
        xdates += xdate;
    }
    QSet<QOrganizerRecurrenceRule> xrules = recur.exceptionRules();
    foreach (const QOrganizerRecurrenceRule& xrule, xrules) {
        if (xrule.frequency() != QOrganizerRecurrenceRule::Invalid
                && ((xrule.limitType() != QOrganizerRecurrenceRule::DateLimit) || (xrule.limitDate() >= realPeriodStart.date()))) {
            // we cannot skip it, since it applies in the given time period.
            QList<QDateTime> xdatetimes = generateDateTimes(initialDateTime, xrule, realPeriodStart, realPeriodEnd, 50); // max count of 50 is arbitrary...
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
    QSet<QOrganizerRecurrenceRule> rrules = recur.recurrenceRules();
    foreach (const QOrganizerRecurrenceRule& rrule, rrules) {
        if (rrule.frequency() != QOrganizerRecurrenceRule::Invalid
                && ((rrule.limitType() != QOrganizerRecurrenceRule::DateLimit) || (rrule.limitDate() >= realPeriodStart.date()))) {
            // we cannot skip it, since it applies in the given time period.
            rdates += generateDateTimes(initialDateTime, rrule, realPeriodStart, realPeriodEnd, 50); // max count of 50 is arbitrary...
        }
    }

    // now order the contents of retn by date
    qSort(rdates);
    if (qBinaryFind(rdates, initialDateTime) == rdates.end()) {
        rdates.prepend(initialDateTime);
    }

    // now for each rdate which isn't also an xdate
    foreach (const QDateTime& rdate, rdates) {
        if (!xdates.contains(rdate.date()) && rdate >= realPeriodStart && rdate <= realPeriodEnd) {
            // generate the required instance and add it to the return list.
            retn.append(generateOccurrence(parentItem, rdate));
        }
    }

    // and return the first maxCount entries.
    return retn.mid(0, maxCount);
}

/*! \reimp */
QList<QOrganizerItem> QOrganizerItemMemoryEngine::itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    Q_UNUSED(fetchHint);
    return internalItemOccurrences(parentItem, periodStart, periodEnd, maxCount, true, error);
}

QOrganizerItem QOrganizerItemMemoryEngine::generateOccurrence(const QOrganizerItem& parentItem, const QDateTime& rdate)
{
    QOrganizerItem instanceItem;
    if (parentItem.type() == QOrganizerItemType::TypeEvent) {
        instanceItem = QOrganizerEventOccurrence();
    } else {
        instanceItem = QOrganizerTodoOccurrence();
    }

    // XXX TODO: something better than this linear search...
    // Grab all details from the parent item except the recurrence information, and event/todo time range
    QList<QOrganizerItemDetail> allDets = parentItem.details();
    QList<QOrganizerItemDetail> occDets;
    foreach (const QOrganizerItemDetail& det, allDets) {
        if (det.definitionName() != QOrganizerItemRecurrence::DefinitionName
                && det.definitionName() != QOrganizerEventTime::DefinitionName
                && det.definitionName() != QOrganizerTodoTime::DefinitionName) {
            occDets.append(det);
        }
    }

    // add the detail which identifies exactly which instance this item is.
    QOrganizerItemParent currOrigin;
    currOrigin.setParentId(parentItem.id());
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
    if (parentItem.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEventTime etr = parentItem.detail<QOrganizerEventTime>();
        QDateTime temp = etr.startDateTime();
        temp.setDate(rdate.date());
        etr.setStartDateTime(temp);
        temp = etr.endDateTime();
        temp.setDate(rdate.date());
        etr.setEndDateTime(temp);
        instanceItem.saveDetail(&etr);
    }

    // for todo's?
    if (parentItem.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerTodoTime ttr = parentItem.detail<QOrganizerTodoTime>();
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

QList<QOrganizerItem> QOrganizerItemMemoryEngine::items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    return internalItems(startDate, endDate, filter, sortOrders, fetchHint, error, false);
}

QList<QOrganizerItem> QOrganizerItemMemoryEngine::itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    return internalItems(startDate, endDate, filter, sortOrders, fetchHint, error, true);
}

QOrganizerItem QOrganizerItemMemoryEngine::item(const QOrganizerItemId& organizeritemId) const
{
    return d->m_idToItemHash.value(organizeritemId);
}

QList<QOrganizerItem> QOrganizerItemMemoryEngine::internalItems(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error, bool forExport) const
{
    Q_UNUSED(fetchHint); // no optimisations are possible in the memory backend; ignore the fetch hint.
    Q_UNUSED(error);

    QList<QOrganizerItem> sorted;
    QSet<QOrganizerItemId> parentsAdded;
    bool isDefFilter = (filter.type() == QOrganizerItemFilter::DefaultFilter);

    foreach(const QOrganizerItem& c, d->m_idToItemHash) {
        if (itemHasReccurence(c)) {
            addItemRecurrences(sorted, c, startDate, endDate, filter, sortOrders, forExport, &parentsAdded);
        } else {
            if ((isDefFilter || QOrganizerManagerEngine::testFilter(filter, c)) && QOrganizerManagerEngine::isItemBetweenDates(c, startDate, endDate)) {
                QOrganizerManagerEngine::addSorted(&sorted,c, sortOrders);
                if (forExport
                        && (c.type() == QOrganizerItemType::TypeEventOccurrence
                        ||  c.type() == QOrganizerItemType::TypeTodoOccurrence)) {
                    QOrganizerItemId parentId(c.detail<QOrganizerItemParent>().parentId());
                    if (!parentsAdded.contains(parentId)) {
                        parentsAdded.insert(parentId);
                        QOrganizerManagerEngine::addSorted(&sorted, item(parentId), sortOrders);

                    }

                }
            }
        }
    }

    return sorted;
}


void QOrganizerItemMemoryEngine::addItemRecurrences(QList<QOrganizerItem>& sorted, const QOrganizerItem& c, const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, bool forExport, QSet<QOrganizerItemId>* parentsAdded) const
{
    QOrganizerManager::Error error = QOrganizerManager::NoError;
    QList<QOrganizerItem> recItems = internalItemOccurrences(c, startDate, endDate, forExport ? 1 : 50, false, &error); // XXX TODO: why maxcount of 50?

    if (filter.type() == QOrganizerItemFilter::DefaultFilter) {
        foreach(const QOrganizerItem& oi, recItems) {
            QOrganizerManagerEngine::addSorted(&sorted, forExport ? c : oi, sortOrders);
            if (forExport)
                parentsAdded->insert(c.id());
        }
    } else {
        foreach(const QOrganizerItem& oi, recItems) {
            if (QOrganizerManagerEngine::testFilter(filter, oi)) {
                QOrganizerManagerEngine::addSorted(&sorted, forExport ? c : oi, sortOrders);
                if (forExport)
                    parentsAdded->insert(c.id());
            }
        }
    }
}

/*! Saves the given organizeritem \a theOrganizerItem, storing any error to \a error and
    filling the \a changeSet with ids of changed organizeritems as required */
bool QOrganizerItemMemoryEngine::saveItem(QOrganizerItem* theOrganizerItem, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error)
{
    QOrganizerCollectionId targetCollectionId = theOrganizerItem->collectionId();

    // check that the collection exists (or is null :. default collection):
    if (!targetCollectionId.isNull() && !d->m_organizerCollectionIds.contains(targetCollectionId)) {
        *error = QOrganizerManager::InvalidCollectionError;
        return false;
    }

    // ensure that the organizer item's details conform to their definitions
    if (!validateItem(*theOrganizerItem, error)) {
        return false;
    }

    // check to see if this organizer item already exists
    const QOrganizerItemId& theOrganizerItemId = theOrganizerItem->id();
    QHash<QOrganizerItemId, QOrganizerItem>::const_iterator hashIterator = d->m_idToItemHash.find(theOrganizerItemId);
    if (hashIterator != d->m_idToItemHash.end()) {
        /* We also need to check that there are no modified create only details */
        QOrganizerItem oldOrganizerItem = hashIterator.value();

        if (oldOrganizerItem.type() != theOrganizerItem->type()) {
            *error = QOrganizerManager::AlreadyExistsError;
            return false;
        }

        // check that the old and new collection is the same (ie, not attempting to save to a different collection)
        if (targetCollectionId.isNull()) {
            // it already exists, so save it where it already exists.
            targetCollectionId = d->m_itemsInCollections.key(theOrganizerItemId);
        } else if (!d->m_itemsInCollections.values(targetCollectionId).contains(theOrganizerItemId)) {
            // the given collection id was non-null but doesn't already contain this item.  error.
            *error = QOrganizerManager::InvalidCollectionError;
            return false;
        }

        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        QOrganizerManagerEngine::setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        if (!fixOccurrenceReferences(theOrganizerItem, error)) {
            return false;
        }

        // Looks ok, so continue
        d->m_idToItemHash.insert(theOrganizerItemId, *theOrganizerItem); // replacement insert.
        changeSet.insertChangedItem(theOrganizerItemId);
    } else {
        // id does not exist; if not zero, fail.
        if (!theOrganizerItemId.isNull()) {
            // the ID is not empty, and it doesn't identify an existing organizer item in our database either.
            *error = QOrganizerManager::DoesNotExistError;
            return false;
        }

        /* New organizer item */
        QOrganizerItemTimestamp ts = theOrganizerItem->detail(QOrganizerItemTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        ts.setCreated(ts.lastModified());
        setDetailAccessConstraints(&ts, QOrganizerItemDetail::ReadOnly | QOrganizerItemDetail::Irremovable);
        theOrganizerItem->saveDetail(&ts);

        // now modify the target collection id; if null, set to default id.  BUT record whether we did that.
        bool targetCollectionWasNull = false; // this determines for OCCURRENCES whether we ignore the default id.
        if (targetCollectionId.isNull()) {
            targetCollectionWasNull = true;
            targetCollectionId = QOrganizerCollectionId(new QOrganizerCollectionMemoryEngineId(1, d->m_managerUri));
        }

        // update the organizer item - set its ID
        quint32 nextOrganizerItemId = d->m_nextOrganizerItemId; // don't increment the persistent version until we're successful or we know it collides.
        nextOrganizerItemId += 1; // but do increment the temporary version to check for collision
        QOrganizerItemMemoryEngineId* newMemoryEngineId = new QOrganizerItemMemoryEngineId;
        newMemoryEngineId->m_collectionId = static_cast<const QOrganizerCollectionMemoryEngineId*>(QOrganizerManagerEngine::engineCollectionId(targetCollectionId))->m_collectionId;
        newMemoryEngineId->m_itemId = nextOrganizerItemId;
        newMemoryEngineId->m_managerUri = d->m_managerUri;
        QOrganizerItemId newOrganizerItemId = QOrganizerItemId(newMemoryEngineId);
        theOrganizerItem->setId(newOrganizerItemId);
        // note: do NOT delete the QOrganizerItemMemoryEngineId -- the QOrganizerItemId ctor takes ownership of it.

        if (!fixOccurrenceReferences(theOrganizerItem, error)) {
            return false;
        }
        // set the guid if not set, and ensure that it's the same as the parent's
        if (theOrganizerItem->guid().isEmpty())
            theOrganizerItem->setGuid(QUuid::createUuid().toString());

        // if we're saving an exception occurrence, we need to add it's original date as an exdate to the parent.
        QOrganizerItemId parentHashKey;
        if (theOrganizerItem->type() == QOrganizerItemType::TypeEventOccurrence) {
            // update the event by adding an EX-DATE which corresponds to the original date of the occurrence being saved.
            QOrganizerManager::Error tempError = QOrganizerManager::NoError;
            QOrganizerItemParent origin = theOrganizerItem->detail<QOrganizerItemParent>();
            QOrganizerItemId parentId = origin.parentId();

            // for occurrences, if given a null collection id, save it in the same collection as the parent.
            // otherwise, ensure that the parent is in the same collection.  You cannot save an exception to a different collection than the parent.
            if (targetCollectionWasNull) {
                targetCollectionId = d->m_itemsInCollections.key(parentId);
                if (targetCollectionId.isNull()) {
                    *error = QOrganizerManager::UnspecifiedError; // this should never occur; parent should _always_ be in a collection.
                    return false;
                }
                const QOrganizerCollectionMemoryEngineId* newColId = static_cast<const QOrganizerCollectionMemoryEngineId*>(QOrganizerManagerEngine::engineCollectionId(targetCollectionId));
                QOrganizerItemMemoryEngineId* newId = new QOrganizerItemMemoryEngineId(newColId->m_collectionId, nextOrganizerItemId, d->m_managerUri);
                theOrganizerItem->setId(QOrganizerItemId(newId));
            } else if (!d->m_itemsInCollections.values(targetCollectionId).contains(parentId)) {
                // nope, the specified collection doesn't contain the parent.  error.
                *error = QOrganizerManager::InvalidCollectionError;
                return false;
            }

            QOrganizerEvent parentEvent = item(parentId, QOrganizerItemFetchHint(), &tempError);
            QDate originalDate = origin.originalDate();
            QSet<QDate> currentExceptionDates = parentEvent.exceptionDates();
            if (!currentExceptionDates.contains(originalDate)) {
                currentExceptionDates << originalDate;
                parentEvent.setExceptionDates(currentExceptionDates);
                d->m_idToItemHash.insert(parentEvent.id(), parentEvent); // replacement insert
                changeSet.insertChangedItem(parentEvent.id()); // is this correct?  it's an exception, so change parent?
            }
            parentHashKey = parentEvent.id(); // this will be the key into the d->m_parentIdToChildIdHash hash.
        } else if (theOrganizerItem->type() == QOrganizerItemType::TypeTodoOccurrence) {
            // update the todo by adding an EX-DATE which corresponds to the original date of the occurrence being saved.
            QOrganizerManager::Error tempError = QOrganizerManager::NoError;
            QOrganizerItemParent origin = theOrganizerItem->detail<QOrganizerItemParent>();
            QOrganizerItemId parentId = origin.parentId();

            // for occurrences, if given a null collection id, save it in the same collection as the parent.
            // otherwise, ensure that the parent is in the same collection.  You cannot save an exception to a different collection than the parent.
            if (targetCollectionWasNull) {
                targetCollectionId = d->m_itemsInCollections.key(parentId);
                if (targetCollectionId.isNull()) {
                    *error = QOrganizerManager::UnspecifiedError; // this should never occur; parent should _always_ be in a collection.
                    return false;
                }
                const QOrganizerCollectionMemoryEngineId* newColId = static_cast<const QOrganizerCollectionMemoryEngineId*>(QOrganizerManagerEngine::engineCollectionId(targetCollectionId));
                QOrganizerItemMemoryEngineId* newId = new QOrganizerItemMemoryEngineId(newColId->m_collectionId, nextOrganizerItemId, d->m_managerUri);
                theOrganizerItem->setId(QOrganizerItemId(newId));
            } else if (!d->m_itemsInCollections.values(targetCollectionId).contains(parentId)) {
                // nope, the specified collection doesn't contain the parent.  error.
                *error = QOrganizerManager::InvalidCollectionError;
                return false;
            }

            QOrganizerTodo parentTodo = item(parentId, QOrganizerItemFetchHint(), &tempError);
            QDate originalDate = origin.originalDate();
            QSet<QDate> currentExceptionDates = parentTodo.exceptionDates();
            if (!currentExceptionDates.contains(originalDate)) {
                currentExceptionDates << originalDate;
                parentTodo.setExceptionDates(currentExceptionDates);
                d->m_idToItemHash.insert(parentTodo.id(), parentTodo); // replacement insert
                changeSet.insertChangedItem(parentTodo.id()); // is this correct?  it's an exception, so change parent?
            }
            parentHashKey = parentTodo.id(); // this will be the key into the d->m_parentIdToChildIdHash hash.
        }

        // given that we were successful, now increment the persistent version of the next item id.
        d->m_nextOrganizerItemId += 1;

        // finally, add the organizer item to our internal lists and return
        theOrganizerItem->setCollectionId(targetCollectionId);
        d->m_idToItemHash.insert(theOrganizerItem->id(), *theOrganizerItem);  // add organizer item to hash
        if (!parentHashKey.isNull()) {
            // if it was an occurrence, we need to add it to the children hash.
            d->m_parentIdToChildIdHash.insert(parentHashKey, theOrganizerItem->id());
        }
        d->m_itemsInCollections.insert(targetCollectionId, theOrganizerItem->id());

        changeSet.insertAddedItem(theOrganizerItem->id());
    }

    *error = QOrganizerManager::NoError;     // successful.
    return true;
}

/*!
 * For Occurrence type items, ensure the ParentId and the Guid are set consistently.  Returns
 * false and sets \a error on error, returns true otherwise.
 */
bool QOrganizerItemMemoryEngine::fixOccurrenceReferences(QOrganizerItem* theItem, QOrganizerManager::Error* error)
{
    if (theItem->type() == QOrganizerItemType::TypeEventOccurrence
            || theItem->type() == QOrganizerItemType::TypeTodoOccurrence) {
        const QString guid = theItem->guid();
        QOrganizerItemParent instanceOrigin = theItem->detail<QOrganizerItemParent>();
        if (!instanceOrigin.originalDate().isValid()) {
            *error = QOrganizerManager::InvalidOccurrenceError;
            return false;
        }
        QOrganizerItemId parentId = instanceOrigin.parentId();
        if (!guid.isEmpty()) {
            if (!parentId.isNull()) {
                QOrganizerManager::Error tempError;
                QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
                if (guid != parentItem.guid()
                        || !typesAreRelated(theItem->type(), parentItem.type())) {
                    // parentId and guid are both set and inconsistent, or the parent is the wrong
                    // type
                    *error = QOrganizerManager::InvalidOccurrenceError;
                    return false;
                }
            } else {
                // guid set but not parentId
                // find an item with the given guid
                foreach (const QOrganizerItem& item, d->m_idToItemHash) {
                    if (item.guid() == guid) {
                        parentId = item.id();
                        break;
                    }
                }
                if (parentId.isNull()) {
                    // couldn't find an item with the given guid
                    *error = QOrganizerManager::InvalidOccurrenceError;
                    return false;
                }
                QOrganizerManager::Error tempError;
                QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
                if (!typesAreRelated(theItem->type(), parentItem.type())) {
                    // the parent is the wrong type
                    *error = QOrganizerManager::InvalidOccurrenceError;
                    return false;
                }
                // found a matching item - set the parentId of the occurrence
                QOrganizerItemParent origin = theItem->detail<QOrganizerItemParent>();
                origin.setParentId(parentId);
                theItem->saveDetail(&origin);
            }
        } else if (!parentId.isNull()) {
            QOrganizerManager::Error tempError;
            QOrganizerItem parentItem = item(parentId, QOrganizerItemFetchHint(), &tempError);
            if (parentItem.guid().isEmpty()
                    || !typesAreRelated(theItem->type(), parentItem.type())) {
                // found the matching item but it has no guid, or it isn't the right type
                *error = QOrganizerManager::InvalidOccurrenceError;
                return false;
            }
            theItem->setGuid(parentItem.guid());
        } else {
            // neither parentId or guid is supplied
            *error = QOrganizerManager::InvalidOccurrenceError;
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
bool QOrganizerItemMemoryEngine::saveItems(QList<QOrganizerItem>* organizeritems, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error)
{
    if(errorMap) {
        errorMap->clear();
    }

    if (!organizeritems) {
        *error = QOrganizerManager::BadArgumentError;
        return false;
    }

    QOrganizerItemChangeSet changeSet;
    QOrganizerItem current;
    QOrganizerManager::Error operationError = QOrganizerManager::NoError;
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
    return (*error == QOrganizerManager::NoError);
}

/*! Removes the organizer item identified by the given \a organizeritemId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items as required */
bool QOrganizerItemMemoryEngine::removeItem(const QOrganizerItemId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error)
{
    QHash<QOrganizerItemId, QOrganizerItem>::const_iterator hashIterator = d->m_idToItemHash.find(organizeritemId);
    if (hashIterator == d->m_idToItemHash.end()) {
        *error = QOrganizerManager::DoesNotExistError;
        return false;
    }

    // if it is a child item, remove itself from the children hash
    QOrganizerItem thisItem = hashIterator.value();
    QOrganizerItemParent parentDetail = thisItem.detail<QOrganizerItemParent>();
    if (!parentDetail.parentId().isNull()) {
        d->m_parentIdToChildIdHash.remove(parentDetail.parentId(), organizeritemId);
    }

    // if it is a parent item, remove any children.
    QList<QOrganizerItemId> childrenIds = d->m_parentIdToChildIdHash.values(organizeritemId);
    foreach (const QOrganizerItemId& childId, childrenIds) {
        // remove the child occurrence from our lists.
        d->m_idToItemHash.remove(childId);
        d->m_itemsInCollections.remove(d->m_itemsInCollections.key(childId), childId);
    }

    // remove the organizer item from the lists.
    d->m_idToItemHash.remove(organizeritemId);
    d->m_parentIdToChildIdHash.remove(organizeritemId);
    d->m_itemsInCollections.remove(d->m_itemsInCollections.key(organizeritemId), organizeritemId);
    *error = QOrganizerManager::NoError;

    changeSet.insertRemovedItem(organizeritemId);
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::removeItems(const QList<QOrganizerItemId>& organizeritemIds, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error)
{
    if (organizeritemIds.count() == 0) {
        *error = QOrganizerManager::BadArgumentError;
        return false;
    }

    QOrganizerItemChangeSet changeSet;
    QOrganizerItemId current;
    QOrganizerManager::Error operationError = QOrganizerManager::NoError;
    for (int i = 0; i < organizeritemIds.count(); i++) {
        current = organizeritemIds.at(i);
        if (!removeItem(current, changeSet, error)) {
            operationError = *error;
            if (errorMap)
                errorMap->insert(i, operationError);
        }
    }

    *error = operationError;
    d->emitSharedSignals(&changeSet);

    // return false if some errors occurred
    return (*error == QOrganizerManager::NoError);
}

QOrganizerCollection QOrganizerItemMemoryEngine::defaultCollection(QOrganizerManager::Error* error) const
{
    // default collection has id of 1.
    *error = QOrganizerManager::NoError;
    QOrganizerCollectionId defaultCollectionId = QOrganizerCollectionId(new QOrganizerCollectionMemoryEngineId(1, d->m_managerUri));
    for (int i = 0; i < d->m_organizerCollections.size(); ++i) {
        if (d->m_organizerCollections.at(i).id() == defaultCollectionId) {
            return d->m_organizerCollections.at(i);
        }
    }

    *error = QOrganizerManager::UnspecifiedError;
    return QOrganizerCollection();
}

QOrganizerCollection QOrganizerItemMemoryEngine::collection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NoError;
    for (int i = 0; i < d->m_organizerCollections.size(); ++i) {
        if (d->m_organizerCollections.at(i).id() == collectionId) {
            return d->m_organizerCollections.at(i);
        }
    }

    *error = QOrganizerManager::DoesNotExistError;
    return QOrganizerCollection();
}
QList<QOrganizerCollection> QOrganizerItemMemoryEngine::collections(QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NoError;
    return d->m_organizerCollections;
}

QOrganizerCollection QOrganizerItemMemoryEngine::compatibleCollection(const QOrganizerCollection& original, QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NoError;

    // we don't allow people to change the default collection.
    QOrganizerCollectionId defaultCollectionId = QOrganizerCollectionId(new QOrganizerCollectionMemoryEngineId(1, d->m_managerUri));
    if (original.id() == defaultCollectionId) {
        for (int i = 0; i < d->m_organizerCollections.size(); ++i) {
            QOrganizerCollection current = d->m_organizerCollections.at(i);
            if (current.id() == defaultCollectionId) {
                return current;
            }
        }
    }

    // if it isn't the default id, it's fine, since anything can be saved in the memory engine.
    return original;
}

bool QOrganizerItemMemoryEngine::saveCollection(QOrganizerCollection* collection, QOrganizerManager::Error* error)
{
    QOrganizerCollectionChangeSet cs; // for signal emission.

    *error = QOrganizerManager::NoError;
    QOrganizerCollectionId colId = collection->id();
    if (colId == QOrganizerCollectionId(new QOrganizerCollectionMemoryEngineId(1, d->m_managerUri))) {
        // attempting to update the default collection.  this is not allowed in the memory engine.
        *error = QOrganizerManager::PermissionsError;
        return false;
    }

    // if it's not the default collection, they can do whatever they like.  A collection does not need any metadata to be valid.
    for (int i = 0; i < d->m_organizerCollectionIds.size(); ++i) {
        if (d->m_organizerCollectionIds.at(i) == colId) {
            // this collection already exists.  update our internal list.
            d->m_organizerCollections.replace(i, *collection);
            cs.insertChangedCollection(colId);
            cs.emitSignals(this);
            return true;
        }
    }

    // this must be a new collection.  check that the id is null.
    if ((collection->id().managerUri() != d->m_managerUri && !collection->id().managerUri().isEmpty()) || !colId.isNull()) {
        // nope, this collection belongs in another manager, or has been deleted.
        *error = QOrganizerManager::DoesNotExistError;
        return false;
    }

    // this is a new collection with a null id; create a new id, add it to our list.
    QOrganizerCollectionId newId = QOrganizerCollectionId(new QOrganizerCollectionMemoryEngineId(d->m_nextOrganizerCollectionId++, d->m_managerUri));
    collection->setId(newId);
    d->m_organizerCollections.append(*collection);
    d->m_organizerCollectionIds.append(newId);
    cs.insertAddedCollection(newId);
    cs.emitSignals(this);
    return true;
}

bool QOrganizerItemMemoryEngine::removeCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error)
{
    QOrganizerCollectionChangeSet cs; // for signal emission.
    *error = QOrganizerManager::NoError;
    if (collectionId == QOrganizerCollectionId(new QOrganizerCollectionMemoryEngineId(1, d->m_managerUri))) {
        // attempting to remove the default collection.  this is not allowed in the memory engine.
        *error = QOrganizerManager::PermissionsError;
        return false;
    }

    // try to find the collection to remove it (and the items it contains)
    QList<QOrganizerItemId> itemsToRemove = d->m_itemsInCollections.values(collectionId);
    for (int i = 0; i < d->m_organizerCollectionIds.size(); ++i) {
        if (d->m_organizerCollectionIds.at(i) == collectionId) {
            // found the collection to remove.  remove the items in the collection.
            if (!itemsToRemove.isEmpty()) {
                if (!removeItems(itemsToRemove, 0, error)) {
                    // without transaction support, we can't back out.  but the operation should fail.
                    return false;
                }
            }

            // now remove the collection from our lists.
            d->m_organizerCollectionIds.removeAt(i);
            d->m_organizerCollections.removeAt(i);
            d->m_itemsInCollections.remove(collectionId);
            cs.insertRemovedCollection(collectionId);
            cs.emitSignals(this);
            return true;
        }
    }

    // the collection doesn't exist...
    *error = QOrganizerManager::DoesNotExistError;
    return false;
}

/*! \reimp */
QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemMemoryEngine::detailDefinitions(const QString& organizeritemType, QOrganizerManager::Error* error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        d->m_definitions = QOrganizerManagerEngine::schemaDefinitions();
    }

    *error = QOrganizerManager::NoError;
    return d->m_definitions.value(organizeritemType);
}

/*! Saves the given detail definition \a def, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items as required */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error)
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

    *error = QOrganizerManager::NoError;
    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    bool retn = saveDetailDefinition(def, organizeritemType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! Removes the detail definition identified by \a definitionId, storing any error to \a error and
    filling the \a changeSet with ids of changed organizer items as required */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error)
{
    // we should check for changes to the database in this function, and add ids of changed data to changeSet...
    // we should also check to see if the changes have invalidated any organizer item data, and add the ids of those organizer items
    // to the change set.  TODO!
    Q_UNUSED(changeSet);

    if (definitionId.isEmpty()) {
        *error = QOrganizerManager::BadArgumentError;
        return false;
    }

    detailDefinitions(organizeritemType, error); // just to populate the definitions if we haven't already.
    QMap<QString, QOrganizerItemDetailDefinition> defsForThisType = d->m_definitions.value(organizeritemType);
    bool success = defsForThisType.remove(definitionId);
    d->m_definitions.insert(organizeritemType, defsForThisType);
    if (success)
        *error = QOrganizerManager::NoError;
    else
        *error = QOrganizerManager::DoesNotExistError;
    return success;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    bool retn = removeDetailDefinition(definitionId, organizeritemType, changeSet, error);
    d->emitSharedSignals(&changeSet);
    return retn;
}

/*! \reimp */
void QOrganizerItemMemoryEngine::requestDestroyed(QOrganizerAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::startRequest(QOrganizerAbstractRequest* req)
{
    if (!req)
        return false;

    QWeakPointer<QOrganizerAbstractRequest> checkDeletion(req);
    updateRequestState(req, QOrganizerAbstractRequest::ActiveState);
    if (!checkDeletion.isNull())
        performAsynchronousOperation(req);

    return true;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::cancelRequest(QOrganizerAbstractRequest* req)
{
    Q_UNUSED(req); // we can't cancel since we complete immediately
    return false;
}

/*! \reimp */
bool QOrganizerItemMemoryEngine::waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs)
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
void QOrganizerItemMemoryEngine::performAsynchronousOperation(QOrganizerAbstractRequest *currentRequest)
{
    // store up changes, and emit signals once at the end of the (possibly batch) operation.
    QOrganizerItemChangeSet changeSet;

    // Now perform the active request and emit required signals.
    Q_ASSERT(currentRequest->state() == QOrganizerAbstractRequest::ActiveState);
    switch (currentRequest->type()) {
        case QOrganizerAbstractRequest::ItemFetchRequest:
        {
            QOrganizerItemFetchRequest* r = static_cast<QOrganizerItemFetchRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();
            QOrganizerItemFetchHint fetchHint = r->fetchHint();
            QDateTime startDate = r->startDate();
            QDateTime endDate = r->endDate();

            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QList<QOrganizerItem> requestedOrganizerItems = items(startDate, endDate, filter, sorting, fetchHint, &operationError);

            // update the request with the results.
            if (!requestedOrganizerItems.isEmpty() || operationError != QOrganizerManager::NoError)
                updateItemFetchRequest(r, requestedOrganizerItems, operationError, QOrganizerAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::ItemFetchForExportRequest:
        {
            QOrganizerItemFetchForExportRequest* r = static_cast<QOrganizerItemFetchForExportRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();
            QOrganizerItemFetchHint fetchHint = r->fetchHint();
            QDateTime startDate = r->startDate();
            QDateTime endDate = r->endDate();

            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QList<QOrganizerItem> requestedOrganizerItems = itemsForExport(startDate, endDate, filter, sorting, fetchHint, &operationError);

            // update the request with the results.
            if (!requestedOrganizerItems.isEmpty() || operationError != QOrganizerManager::NoError)
                updateItemFetchForExportRequest(r, requestedOrganizerItems, operationError, QOrganizerAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::ItemOccurrenceFetchRequest:
        {
            QOrganizerItemOccurrenceFetchRequest* r = static_cast<QOrganizerItemOccurrenceFetchRequest*>(currentRequest);
            QOrganizerItem parentItem(r->parentItem());
            QDateTime startDate(r->startDate());
            QDateTime endDate(r->endDate());
            int countLimit = r->maxOccurrences();
            QOrganizerItemFetchHint fetchHint = r->fetchHint();

            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QList<QOrganizerItem> requestedOrganizerItems = itemOccurrences(parentItem, startDate, endDate, countLimit, fetchHint, &operationError);

            // update the request with the results.
            if (!requestedOrganizerItems.isEmpty() || operationError != QOrganizerManager::NoError)
                updateItemOccurrenceFetchRequest(r, requestedOrganizerItems, operationError, QOrganizerAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerAbstractRequest::FinishedState);
        }
        break;


        case QOrganizerAbstractRequest::ItemIdFetchRequest:
        {
            QOrganizerItemIdFetchRequest* r = static_cast<QOrganizerItemIdFetchRequest*>(currentRequest);
            QOrganizerItemFilter filter = r->filter();
            QList<QOrganizerItemSortOrder> sorting = r->sorting();
            QDateTime startDate = r->startDate();
            QDateTime endDate = r->endDate();

            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QList<QOrganizerItemId> requestedOrganizerItemIds = itemIds(startDate, endDate, filter, sorting, &operationError);

            if (!requestedOrganizerItemIds.isEmpty() || operationError != QOrganizerManager::NoError)
                updateItemIdFetchRequest(r, requestedOrganizerItemIds, operationError, QOrganizerAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::ItemSaveRequest:
        {
            QOrganizerItemSaveRequest* r = static_cast<QOrganizerItemSaveRequest*>(currentRequest);
            QList<QOrganizerItem> organizeritems = r->items();

            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QMap<int, QOrganizerManager::Error> errorMap;
            saveItems(&organizeritems, &errorMap, &operationError);

            updateItemSaveRequest(r, organizeritems, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::ItemRemoveRequest:
        {
            QOrganizerItemRemoveRequest* r = static_cast<QOrganizerItemRemoveRequest*>(currentRequest);
            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QList<QOrganizerItemId> organizeritemsToRemove = r->itemIds();
            QMap<int, QOrganizerManager::Error> errorMap;

            for (int i = 0; i < organizeritemsToRemove.size(); i++) {
                QOrganizerManager::Error tempError = QOrganizerManager::NoError;
                removeItem(organizeritemsToRemove.at(i), changeSet, &tempError);

                if (tempError != QOrganizerManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || operationError != QOrganizerManager::NoError)
                updateItemRemoveRequest(r, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::CollectionFetchRequest:
        {
            QOrganizerCollectionFetchRequest* r = static_cast<QOrganizerCollectionFetchRequest*>(currentRequest);
            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QList<QOrganizerCollection> requestedOrganizerCollections = collections(&operationError);

            // update the request with the results.
            updateCollectionFetchRequest(r, requestedOrganizerCollections, operationError, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::CollectionSaveRequest:
        {
            QOrganizerCollectionSaveRequest* r = static_cast<QOrganizerCollectionSaveRequest*>(currentRequest);
            QList<QOrganizerCollection> collections = r->collections();
            QList<QOrganizerCollection> retn;

            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QMap<int, QOrganizerManager::Error> errorMap;
            for (int i = 0; i < collections.size(); ++i) {
                QOrganizerManager::Error tempError = QOrganizerManager::NoError;
                QOrganizerCollection curr = collections.at(i);
                if (!saveCollection(&curr, &tempError)) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
                retn.append(curr);
            }

            updateCollectionSaveRequest(r, retn, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::CollectionRemoveRequest:
        {
            // removes the collections identified in the list of ids.
            QOrganizerCollectionRemoveRequest* r = static_cast<QOrganizerCollectionRemoveRequest*>(currentRequest);
            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QList<QOrganizerCollectionId> collectionsToRemove = r->collectionIds();
            QMap<int, QOrganizerManager::Error> errorMap;

            for (int i = 0; i < collectionsToRemove.size(); i++) {
                QOrganizerManager::Error tempError = QOrganizerManager::NoError;
                removeCollection(collectionsToRemove.at(i), &tempError);

                if (tempError != QOrganizerManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || operationError != QOrganizerManager::NoError)
                updateCollectionRemoveRequest(r, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::DetailDefinitionFetchRequest:
        {
            QOrganizerItemDetailDefinitionFetchRequest* r = static_cast<QOrganizerItemDetailDefinitionFetchRequest*>(currentRequest);
            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QMap<int, QOrganizerManager::Error> errorMap;
            QMap<QString, QOrganizerItemDetailDefinition> requestedDefinitions;
            QStringList names = r->definitionNames();
            if (names.isEmpty())
                names = detailDefinitions(r->itemType(), &operationError).keys(); // all definitions.

            QOrganizerManager::Error tempError = QOrganizerManager::NoError;
            for (int i = 0; i < names.size(); i++) {
                QOrganizerItemDetailDefinition current = detailDefinition(names.at(i), r->itemType(), &tempError);
                requestedDefinitions.insert(names.at(i), current);

                if (tempError != QOrganizerManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            if (!errorMap.isEmpty() || !requestedDefinitions.isEmpty() || operationError != QOrganizerManager::NoError)
                updateDefinitionFetchRequest(r, requestedDefinitions, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
            else
                updateRequestState(currentRequest, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::DetailDefinitionSaveRequest:
        {
            QOrganizerItemDetailDefinitionSaveRequest* r = static_cast<QOrganizerItemDetailDefinitionSaveRequest*>(currentRequest);
            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QMap<int, QOrganizerManager::Error> errorMap;
            QList<QOrganizerItemDetailDefinition> definitions = r->definitions();
            QList<QOrganizerItemDetailDefinition> savedDefinitions;

            QOrganizerManager::Error tempError = QOrganizerManager::NoError;
            for (int i = 0; i < definitions.size(); i++) {
                QOrganizerItemDetailDefinition current = definitions.at(i);
                saveDetailDefinition(current, r->itemType(), changeSet, &tempError);
                savedDefinitions.append(current);

                if (tempError != QOrganizerManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // update the request with the results.
            updateDefinitionSaveRequest(r, savedDefinitions, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
        }
        break;

        case QOrganizerAbstractRequest::DetailDefinitionRemoveRequest:
        {
            QOrganizerItemDetailDefinitionRemoveRequest* r = static_cast<QOrganizerItemDetailDefinitionRemoveRequest*>(currentRequest);
            QStringList names = r->definitionNames();

            QOrganizerManager::Error operationError = QOrganizerManager::NoError;
            QMap<int, QOrganizerManager::Error> errorMap;

            for (int i = 0; i < names.size(); i++) {
                QOrganizerManager::Error tempError = QOrganizerManager::NoError;
                removeDetailDefinition(names.at(i), r->itemType(), changeSet, &tempError);

                if (tempError != QOrganizerManager::NoError) {
                    errorMap.insert(i, tempError);
                    operationError = tempError;
                }
            }

            // there are no results, so just update the status with the error.
            updateDefinitionRemoveRequest(r, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
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
bool QOrganizerItemMemoryEngine::hasFeature(QOrganizerManager::ManagerFeature feature, const QString& organizeritemType) const
{
    if (!supportedItemTypes().contains(organizeritemType))
        return false;

    switch (feature) {
        case QOrganizerManager::MutableDefinitions:
            return true;
        case QOrganizerManager::Anonymous:
            return d->m_anonymous;

        default:
            return false;
    }
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

