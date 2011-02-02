/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qversitorganizerimporter_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"
#include "qtorganizer.h"
#include "qversitorganizerdefs_p.h"
#include "qversitorganizerpluginloader_p.h"


QTM_USE_NAMESPACE

QVersitOrganizerImporterPrivate::QVersitOrganizerImporterPrivate(const QString& profile) :
    mPropertyHandler(NULL),
    mTimeZoneHandler(NULL),
    mDurationSpecified(false)
{
    int versitPropertyCount =
        sizeof(versitOrganizerDetailMappings)/sizeof(VersitDetailMapping);
    for (int i = 0; i < versitPropertyCount; i++) {
        mPropertyMappings.insert(
                QLatin1String(versitOrganizerDetailMappings[i].versitPropertyName),
                QPair<QString,QString>(
                    QLatin1String(versitOrganizerDetailMappings[i].detailDefinitionName),
                    QLatin1String(versitOrganizerDetailMappings[i].detailFieldName)));
    }

    mPluginPropertyHandlers = QVersitOrganizerPluginLoader::instance()->createOrganizerHandlers(profile);
    mTimeZoneHandler = QVersitOrganizerPluginLoader::instance()->timeZoneHandler();
}

QVersitOrganizerImporterPrivate::~QVersitOrganizerImporterPrivate()
{
    foreach (QVersitOrganizerHandler* pluginHandler, mPluginPropertyHandlers) {
        delete pluginHandler;
    }
}

bool QVersitOrganizerImporterPrivate::importDocument(
        const QVersitDocument& topLevel,
        const QVersitDocument& subDocument,
        QOrganizerItem* item,
        QVersitOrganizerImporter::Error* error)
{
    if (subDocument.componentType() == QLatin1String("VEVENT")) {
        item->setType(QOrganizerItemType::TypeEvent);
    } else if (subDocument.componentType() == QLatin1String("VTODO")) {
        item->setType(QOrganizerItemType::TypeTodo);
    } else if (subDocument.componentType() == QLatin1String("VJOURNAL")) {
        item->setType(QOrganizerItemType::TypeJournal);
    } else if (subDocument.componentType() == QLatin1String("VTIMEZONE")) {
        mTimeZones.addTimeZone(importTimeZone(subDocument));
        *error = QVersitOrganizerImporter::NoError;
        return false;
    } else {
        *error = QVersitOrganizerImporter::InvalidDocumentError;
        return false;
    }
    const QList<QVersitProperty> properties = subDocument.properties();
    if (properties.isEmpty()) {
        *error = QVersitOrganizerImporter::EmptyDocumentError;
        return false;
    }

    foreach (const QVersitProperty& property, properties) {
        importProperty(subDocument, property, item);
    }
    // run plugin handlers
    foreach (QVersitOrganizerImporterPropertyHandler* handler, mPluginPropertyHandlers) {
        handler->subDocumentProcessed(topLevel, subDocument, item);
    }
    // run property handlers
    if (mPropertyHandler) {
        mPropertyHandler->subDocumentProcessed(topLevel, subDocument, item);
    }
    return true;
}

void QVersitOrganizerImporterPrivate::importProperty(
        const QVersitDocument& document,
        const QVersitProperty& property,
        QOrganizerItem* item)
{
    QList<QOrganizerItemDetail> updatedDetails;

    bool success = false;
    if (property.name() == QLatin1String("CREATED")) {
        success = createTimestampCreated(property, item, &updatedDetails);
    } else if (property.name() == QLatin1String("LAST-MODIFIED")) {
        success = createTimestampModified(property, item, &updatedDetails);
    } else if (property.name() == QLatin1String("PRIORITY")) {
        success = createPriority(property, item, &updatedDetails);
    } else if (property.name() == QLatin1String("COMMENT")) {
        success = createComment(property, &updatedDetails);
    } else if (mPropertyMappings.contains(property.name())) {
        success = createSimpleDetail(property, item, &updatedDetails);
    } else if (document.componentType() == QLatin1String("VEVENT")) {
        if (property.name() == QLatin1String("DTSTART")) {
            success = createStartDateTime(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("DTEND")) {
            success = createEndDateTime(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("DURATION")) {
            success = createDuration(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("RRULE")
               || (property.name() == QLatin1String("EXRULE"))) {
            success = createRecurrenceRule(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("RDATE")
               || (property.name() == QLatin1String("EXDATE"))) {
            success = createRecurrenceDates(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("RECURRENCE-ID")) {
            success = createRecurrenceId(property, item, &updatedDetails);
        }
    } else if (document.componentType() == QLatin1String("VTODO")) {
        if (property.name() == QLatin1String("DTSTART")) {
            success = createTodoStartDateTime(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("DUE")) {
            success = createDueDateTime(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("RRULE")
               || (property.name() == QLatin1String("EXRULE"))) {
            success = createRecurrenceRule(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("RDATE")
               || (property.name() == QLatin1String("EXDATE"))) {
            success = createRecurrenceDates(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("STATUS")) {
            success = createStatus(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("PERCENT-COMPLETE")) {
            success = createPercentageComplete(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("COMPLETED")) {
            success = createFinishedDateTime(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("RECURRENCE-ID")) {
            success = createRecurrenceId(property, item, &updatedDetails);
        }
    } else if (document.componentType() == QLatin1String("VJOURNAL")) {
        if (property.name() == QLatin1String("DTSTART")) {
            success = createJournalEntryDateTime(property, item, &updatedDetails);
        }
    }

    // run plugin handlers
    foreach (QVersitOrganizerImporterPropertyHandler* handler, mPluginPropertyHandlers) {
        handler->propertyProcessed(document, property, *item, &success, &updatedDetails);
    }
    // run the handler, if set
    if (mPropertyHandler) {
        mPropertyHandler->propertyProcessed(document, property, *item, &success, &updatedDetails);
    }

    foreach (QOrganizerItemDetail detail, updatedDetails) {
        item->saveDetail(&detail);
    }
}

bool QVersitOrganizerImporterPrivate::createSimpleDetail(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails)
{
    if (property.value().isEmpty())
        return false;
    QPair<QString, QString> mapping = mPropertyMappings[property.name()];
    QString definitionName = mapping.first;
    QString fieldName = mapping.second;
    QOrganizerItemDetail detail(item->detail(definitionName));
    if (detail.isEmpty())
        detail = QOrganizerItemDetail(definitionName);
    detail.setValue(fieldName, property.value());
    updatedDetails->append(detail);
    return true;
}

bool QVersitOrganizerImporterPrivate::createTimestampCreated(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property);
    if (!datetime.isValid())
        return false;
    QOrganizerItemTimestamp timestamp(item->detail<QOrganizerItemTimestamp>());
    timestamp.setCreated(datetime);
    updatedDetails->append(timestamp);
    return true;
}

bool QVersitOrganizerImporterPrivate::createTimestampModified(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property);
    if (!datetime.isValid())
        return false;
    QOrganizerItemTimestamp timestamp(item->detail<QOrganizerItemTimestamp>());
    timestamp.setLastModified(datetime);
    updatedDetails->append(timestamp);
    return true;
}

bool QVersitOrganizerImporterPrivate::createPriority(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().length() != 1)
        return false;

    bool ok;
    int p = property.value().toInt(&ok);
    if (!ok)
        return false;
    QOrganizerItemPriority priority(item->detail<QOrganizerItemPriority>());
    priority.setPriority(QOrganizerItemPriority::Priority(p));
    updatedDetails->append(priority);
    return true;
}

bool QVersitOrganizerImporterPrivate::createComment(
        const QVersitProperty& property,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QOrganizerItemComment comment;
    comment.setComment(property.value());
    updatedDetails->append(comment);
    return true;
}

bool QVersitOrganizerImporterPrivate::createRecurrenceId(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    QDate date = parseDate(property.value());
    if (!date.isValid())
        return false;
    QOrganizerItemParent origin(item->detail<QOrganizerItemParent>());
    origin.setOriginalDate(date);
    updatedDetails->append(origin);
    item->setType(QOrganizerItemType::TypeEventOccurrence);
    return true;
}

/*! Set the startDateTime field of the EventTimeRange detail.  If the end date has been set from a
 * DURATION, it will be updated.
 */
bool QVersitOrganizerImporterPrivate::createStartDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    bool hasTime;
    QDateTime newStart = parseDateTime(property, &hasTime);
    if (!newStart.isValid())
        return false;
    QOrganizerEventTime etr(item->detail<QOrganizerEventTime>());
    if (mDurationSpecified) {
        // Need to fix up the end date to match the duration of the event
        QDateTime start = etr.startDateTime();
        QDateTime end = etr.endDateTime();
        if (!start.isValid()) {
            // not having a start date set is treated as a start date of epoch
            start = QDateTime(QDate(1970, 1, 1));
        }
        // newEnd = end + (newStart - start)
        int durationDays = start.daysTo(newStart);
        QDateTime newEnd = end.addDays(durationDays);
        int durationSecs = start.addDays(durationDays).secsTo(newStart);
        newEnd = newEnd.addSecs(durationSecs);
        etr.setEndDateTime(newEnd);
    }
    etr.setStartDateTime(newStart);
    if (!etr.isAllDay() && !hasTime)
        etr.setAllDay(true);
    updatedDetails->append(etr);
    return true;
}

/*! Set the endDateTime field of the EventTimeRange detail.
 */
bool QVersitOrganizerImporterPrivate::createEndDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    bool hasTime;
    QDateTime newEnd = parseDateTime(property, &hasTime);
    if (!newEnd.isValid())
        return false;
    QOrganizerEventTime etr(item->detail<QOrganizerEventTime>());
    if (!etr.isAllDay() && !hasTime)
        etr.setAllDay(true);

    // In iCalendar, the end date is exclusive while in Qt Organizer, it is inclusive.
    if (etr.isAllDay())
        etr.setEndDateTime(newEnd.addDays(-1));
    else
        etr.setEndDateTime(newEnd);

    updatedDetails->append(etr);
    mDurationSpecified = false;
    return true;
}


/*! Sets the endDateTime field of the EventTimeRange detail using a DURATION property.
 */
bool QVersitOrganizerImporterPrivate::createDuration(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    Duration duration = Duration::parseDuration(property.value());
    if (!duration.isValid())
        return false;
    QOrganizerEventTime etr(item->detail<QOrganizerEventTime>());
    QDateTime startTime = etr.startDateTime();
    if (!startTime.isValid()) {
        // not having a start date set is treated as a start date of epoch
        startTime = QDateTime(QDate(1970, 1, 1));
    }
    etr.setEndDateTime(
            startTime.addDays(7*duration.weeks() + duration.days())
                     .addSecs(3600*duration.hours() + 60*duration.minutes() + duration.seconds()));
    updatedDetails->append(etr);
    mDurationSpecified = true;
    return true;
}

/*! Set the StartDateTime field of the TodoTimeRange detail.
 */
bool QVersitOrganizerImporterPrivate::createTodoStartDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    bool hasTime;
    QDateTime newStart = parseDateTime(property, &hasTime);
    if (!newStart.isValid())
        return false;
    QOrganizerTodoTime ttr(item->detail<QOrganizerTodoTime>());
    ttr.setStartDateTime(newStart);
    if (!ttr.isAllDay() && !hasTime)
        ttr.setAllDay(true);
    updatedDetails->append(ttr);
    return true;
}

/*! Set the DueDateTime field of the TodoTimeRange detail.
 */
bool QVersitOrganizerImporterPrivate::createDueDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    bool hasTime;
    QDateTime newEnd = parseDateTime(property, &hasTime);
    if (!newEnd.isValid())
        return false;
    QOrganizerTodoTime ttr(item->detail<QOrganizerTodoTime>());
    ttr.setDueDateTime(newEnd);
    if (!ttr.isAllDay() && !hasTime)
        ttr.setAllDay(true);
    updatedDetails->append(ttr);
    mDurationSpecified = false;
    return true;
}

/*! Set the EntryDateTime field of the JournalTimeRange detail.
 */
bool QVersitOrganizerImporterPrivate::createJournalEntryDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime dateTime = parseDateTime(property);
    if (!dateTime.isValid())
        return false;
    QOrganizerJournalTime jtr(item->detail<QOrganizerJournalTime>());
    jtr.setEntryDateTime(dateTime);
    updatedDetails->append(jtr);
    return true;
}

/*! Parses a datetime stored in the \a property as an ISO 8601 datetime in basic format, either in
 * UTC time zone, floating time zone, or (if a TZID parameter exists in \a property), as a foreign
 * time zone (returned as a UTC datetime).  Returns an invalid QDateTime if the string cannot be
 * parsed.
 *
 * \a hasTime is set to true if the parsed date-time has a time, or false if it is a date only.
 * (the time portion is set to some valid but arbitrary value).
 */
QDateTime QVersitOrganizerImporterPrivate::parseDateTime(const QVersitProperty& property,
                                                         bool* hasTime) const
{
    const QMultiHash<QString, QString> parameters = property.parameters();
    if (parameters.find(QLatin1String("VALUE"), QLatin1String("DATE")) == parameters.constEnd()) {
        // try parsing a datetime
        if (hasTime)
            *hasTime = true;
        QDateTime datetime(parseDateTime(property.value()));
        if (datetime.isValid() && datetime.timeSpec() == Qt::LocalTime) {
            QMultiHash<QString, QString> params = property.parameters();
            QString tzid = params.value(QLatin1String("TZID"));
            if (!tzid.isEmpty()) {
                if (tzid.at(0) == QLatin1Char('/') && mTimeZoneHandler)
                    datetime = mTimeZoneHandler->convertTimeZoneToUtc(datetime, tzid);
                else
                    datetime = mTimeZones.convert(datetime, tzid);
            }
        }
        return datetime;
    } else {
        if (hasTime)
            *hasTime = false;
        QDateTime retn;
        retn.setDate(QDate::fromString(property.value(), QLatin1String("yyyyMMdd")));
        retn.setTime(QTime(0, 0, 0));
        return retn;
    }
}

/*! Parses \a str as an ISO 8601 datetime in basic format, either in UTC timezone or floating
 * timezone.  Returns an invalid QDateTime if the string cannot be parsed.
 */
QDateTime QVersitOrganizerImporterPrivate::parseDateTime(QString str) const
{
    bool utc = str.endsWith(QLatin1Char('Z'), Qt::CaseInsensitive);
    if (utc)
        str.chop(1); // take away z from end;
    QDateTime dt(QDateTime::fromString(str, QLatin1String("yyyyMMddTHHmmss")));
    if (utc)
        dt.setTimeSpec(Qt::UTC);
    return dt;
}

/*!
 * Imports a RRULE, EXRULE, RDATE or EXDATE property
 */
bool QVersitOrganizerImporterPrivate::createRecurrenceRule(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QOrganizerRecurrenceRule rule;
    if (!parseRecurRule(property.value(), &rule))
        return false;
    QOrganizerItemRecurrence detail(item->detail<QOrganizerItemRecurrence>());
    if (property.name() == QLatin1String("RRULE")) {
        detail.setRecurrenceRules(detail.recurrenceRules() << rule);
    } else if (property.name() == QLatin1String("EXRULE")) {
        detail.setExceptionRules(detail.exceptionRules() << rule);
    } 
    updatedDetails->append(detail);
    return true;
}

/*!
 * Parses an iCalendar recurrence rule string \a str and puts the result in \a rule.
 * Return true on success, false on failure.
 */
bool QVersitOrganizerImporterPrivate::parseRecurRule(const QString& str, QOrganizerRecurrenceRule* rule) const
{
    QStringList parts = str.split(QLatin1Char(';'));
    if (parts.size() == 0)
        return false;
    
    QString freqPart = parts.takeFirst();
    QStringList freqParts = freqPart.split(QLatin1Char('='));
    if (freqParts.size() != 2)
        return false;
    if (freqParts.at(0) != QLatin1String("FREQ"))
        return false;
    QString freqValue = freqParts.at(1);
    if (freqValue == QLatin1String("DAILY")) {
        rule->setFrequency(QOrganizerRecurrenceRule::Daily);
    } else if (freqValue == QLatin1String("WEEKLY")) {
        rule->setFrequency(QOrganizerRecurrenceRule::Weekly);
    } else if (freqValue == QLatin1String("MONTHLY")) {
        rule->setFrequency(QOrganizerRecurrenceRule::Monthly);
    } else if (freqValue == QLatin1String("YEARLY")) {
        rule->setFrequency(QOrganizerRecurrenceRule::Yearly);
    } else {
        return false;
    }

    foreach (const QString& part, parts) {
        QStringList keyValue = part.split(QLatin1Char('='));
        if (keyValue.size() != 2)
            return false;
        parseRecurFragment(keyValue.at(0), keyValue.at(1), rule);
    }
    return true;
}

/*!
 * Parses a fragment of an iCalendar string (the part between the semicolons) and updates \a rule.
 * \a key is the part of the fragment before the equals sign and \a value is the part after.
 */
void QVersitOrganizerImporterPrivate::parseRecurFragment(const QString& key, const QString& value,
                                                         QOrganizerRecurrenceRule* rule) const
{
    if (key == QLatin1String("INTERVAL")) {
        bool ok;
        int n = value.toInt(&ok);
        if (ok && n >= 1)
            rule->setInterval(n);
    } else if (key == QLatin1String("COUNT")) {
        bool ok;
        int count = value.toInt(&ok);
        if (ok && count >= 0) {
            rule->setLimit(count);
        }
    } else if (key == QLatin1String("UNTIL")) {
        QDate date;
        if (value.contains(QLatin1Char('T'))) {
            QDateTime dt = parseDateTime(value);
            date = dt.date();
        } else {
            date = QDate::fromString(value, QLatin1String("yyyyMMdd"));
        }
        if (date.isValid())
            rule->setLimit(date);
    } else if (key == QLatin1String("BYDAY")) {
        QSet<Qt::DayOfWeek> days;
        QStringList dayParts = value.split(QLatin1Char(','));
        foreach (QString dayStr, dayParts) {
            if (dayStr.length() < 2) {
                // bad day specifier
                continue;
            } else if (dayStr.length() > 2) {
                // parse something like -2SU, meaning the second-last Sunday
                QString posStr = dayStr;
                dayStr = dayStr.right(2); // dayStr = last two chars
                posStr.chop(2); // posStr = all except last two chars
                bool ok;
                int pos = posStr.toInt(&ok);
                if (!ok)
                    continue;
                rule->setPositions(QSet<int>() << pos);
            }
            int day = parseDayOfWeek(dayStr);
            if (day != -1) {
                days << (Qt::DayOfWeek)day;
            }
        }
        if (!days.isEmpty()) {
            rule->setDaysOfWeek(days);
        }
    } else if (key == QLatin1String("BYMONTHDAY")) {
        QSet<int> days = parseInts(value, -31, 31);
        if (!days.isEmpty()) {
            rule->setDaysOfMonth(days);
        }
    } else if (key == QLatin1String("BYWEEKNO")) {
        QSet<int> weeks = parseInts(value, -53, 53);
        if (!weeks.isEmpty()) {
            rule->setWeeksOfYear(weeks);
        }
    } else if (key == QLatin1String("BYMONTH")) {
        QSet<QOrganizerRecurrenceRule::Month> months;
        QStringList monthParts = value.split(QLatin1Char(','));
        foreach (const QString& monthPart, monthParts) {
            bool ok;
            int month = monthPart.toInt(&ok);
            if (ok && month >= 1 && month <= 12) {
                months << (QOrganizerRecurrenceRule::Month)month;
            }
        }
        if (!months.isEmpty()) {
            rule->setMonthsOfYear(months);
        }
    } else if (key == QLatin1String("BYYEARDAY")) {
        QSet<int> days = parseInts(value, -366, 366);
        if (!days.isEmpty()) {
            rule->setDaysOfYear(days);
        }
    } else if (key == QLatin1String("BYSETPOS")) {
        QSet<int> poss = parseInts(value, -366, 366);
        if (!poss.isEmpty()) {
            rule->setPositions(poss);
        }
    } else if (key == QLatin1String("WKST")) {
        int day = parseDayOfWeek(value);
        if (day != -1) {
            rule->setFirstDayOfWeek((Qt::DayOfWeek)day);
        }
    }
}

/*!
 * Parses and returns a comma-separated list of integers.  Only non-zero values between \a min and
 * \a max (inclusive) are added
 */
QSet<int> QVersitOrganizerImporterPrivate::parseInts(const QString& str, int min, int max) const
{
    QSet<int> values;
    QStringList parts = str.split(QLatin1Char(','));
    foreach (const QString& part, parts) {
        bool ok;
        int value = part.toInt(&ok);
        if (ok && value >= min && value <= max && value != 0) {
            values << value;
        }
    }
    return values;
}

/*!
 * Parses an iCalendar two-character string representing a day of week and returns an int
 * corresponding to Qt::DayOfWeek.  Returns -1 on parse failure.
 */
int QVersitOrganizerImporterPrivate::parseDayOfWeek(const QString& str) const
{
    if (str == QLatin1String("MO")) {
        return Qt::Monday;
    } else if (str == QLatin1String("TU")) {
        return Qt::Tuesday;
    } else if (str == QLatin1String("WE")) {
        return Qt::Wednesday;
    } else if (str == QLatin1String("TH")) {
        return Qt::Thursday;
    } else if (str == QLatin1String("FR")) {
        return Qt::Friday;
    } else if (str == QLatin1String("SA")) {
        return Qt::Saturday;
    } else if (str == QLatin1String("SU")) {
        return Qt::Sunday;
    } else {
        return -1;
    }
}

/*!
 * Parses an iCalendar RDATE or EXDATE property and updates the recurrenceDates or
 * exceptionDates in the Recurrence detail.
 */
bool QVersitOrganizerImporterPrivate::createRecurrenceDates(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails)
{
    if (property.value().isEmpty())
        return false;
    QSet<QDate> dates;
    if (!parseDates(property.value(), &dates))
        return false;
    QOrganizerItemRecurrence detail(item->detail<QOrganizerItemRecurrence>());
    if (property.name() == QLatin1String("RDATE")) {
        detail.setRecurrenceDates(detail.recurrenceDates() + dates);
    } else if (property.name() == QLatin1String("EXDATE")) {
        detail.setExceptionDates(detail.exceptionDates() + dates);
    } 
    updatedDetails->append(detail);
    return true;
}

/*!
 * Parses a string like "19970304,19970504,19970704" into a list of QDates
 */
bool QVersitOrganizerImporterPrivate::parseDates(const QString& str, QSet<QDate>* dates) const
{
    QStringList parts = str.split(QLatin1Char(','));
    if (parts.size() == 0)
        return false;

    foreach (QString part, parts) {
        QDate date = parseDate(part);
        if (date.isValid())
            *dates << date;
        else
            return false;
    }
    return true;
}

/*!
 * Parses a date in either yyyyMMdd or yyyyMMddTHHmmss format (in the latter case, ignoring the
 * time)
 */
QDate QVersitOrganizerImporterPrivate::parseDate(QString str) const
{
    int tIndex = str.indexOf(QLatin1Char('T'));
    if (tIndex >= 0) {
        str = str.left(tIndex);
    }
    return QDate::fromString(str, QLatin1String("yyyyMMdd"));
}

bool QVersitOrganizerImporterPrivate::createStatus(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    QOrganizerTodoProgress::Status status;
    if (property.value() == QLatin1String("COMPLETED"))
        status = QOrganizerTodoProgress::StatusComplete;
    else if (property.value() == QLatin1String("NEEDS-ACTION"))
        status = QOrganizerTodoProgress::StatusNotStarted;
    else if (property.value() == QLatin1String("IN-PROCESS"))
        status = QOrganizerTodoProgress::StatusInProgress;
    else
        return false;

    QOrganizerTodoProgress progress(item->detail<QOrganizerTodoProgress>());
    progress.setStatus(status);
    updatedDetails->append(progress);
    return true;
}

bool QVersitOrganizerImporterPrivate::createPercentageComplete(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    bool ok = false;
    int percent = property.value().toInt(&ok);
    if (!ok)
        return false;

    QOrganizerTodoProgress progress(item->detail<QOrganizerTodoProgress>());
    progress.setPercentageComplete(percent);
    updatedDetails->append(progress);
    return true;
}

bool QVersitOrganizerImporterPrivate::createFinishedDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property);
    if (!datetime.isValid())
        return false;
    QOrganizerTodoProgress progress(item->detail<QOrganizerTodoProgress>());
    progress.setFinishedDateTime(datetime);
    updatedDetails->append(progress);
    return true;
}

/*! Parse the iCalendar duration string \a str in an RDP fashion with a two symbol lookahead, and
 * returns a Duration that represents it. */
Duration Duration::parseDuration(QString str)
{
    QString token = nextToken(&str);
    if (token.isEmpty())
        return invalidDuration();

    Duration dur;
    // Accept a + or - if present
    if (token == QLatin1String("+")) {
        token = nextToken(&str);
    } else if (token == QLatin1String("-")) {
        dur.setNegative(true);
        token = nextToken(&str);
    } else if (token.isEmpty()) {
        return invalidDuration();
    } else {
        // There was no + or - so keep parsing
    }

    // Accept a P
    if (token != QLatin1String("P")) {
        return invalidDuration();
    }

    token = nextToken(&str);
    if (token.isEmpty()) {
        return invalidDuration();
    } else if (token == QLatin1String("T")) {
        // we see a time
        parseDurationTime(&str, &dur);
    } else if (token.at(0).isDigit()) {
        // it's either a date or a week - we're not sure yet
        int value = token.toInt(); // always succeeds because nextToken next returns a mix of digits/nondigits
        token = nextToken(&str);
        if (token == QLatin1String("D")) {
            // it's a date
            dur.setDays(value);
            token = nextToken(&str);
            // dates optionally define a time
            if (token == QLatin1String("T"))
                parseDurationTime(&str, &dur);
        } else if (token == QLatin1String("W")) {
            dur.setWeeks(value);
        } else {
            return invalidDuration();
        }
    } else {
        return invalidDuration();
    }

    // check that there aren't extra characters on the end
    if (!str.isEmpty())
        dur.setValid(false);
    return dur;

}

/*! Parse a duration string starting from after the "T" character.  Removes parsed part from \a str
 * and updates \a dur with the findings.
 */
void Duration::parseDurationTime(QString* str, Duration* dur)
{
    QString token = nextToken(str);
    if (token.isEmpty() || !token.at(0).isDigit())
        dur->setValid(false);

    int value = token.toInt(); // always succeeds

    token = nextToken(str);
    if (token == QLatin1String("H")) {
        dur->setHours(value);
        if (!str->isEmpty())
            parseDurationMinutes(str, dur);
    } else if (token == QLatin1String("M")) {
        dur->setMinutes(value);
        if (!str->isEmpty())
            parseDurationSeconds(str, dur);
    } else if (token == QLatin1String("S")) {
        dur->setSeconds(value);
    }
}

/*! Parse a duration string starting from the part describing the number of minutes.  Removes parsed
 * part from \a str and updates \a dur with the findings.
 */
void Duration::parseDurationMinutes(QString* str, Duration* dur)
{
    QString token = nextToken(str);
    if (token.isEmpty() || !token.at(0).isDigit())
        dur->setValid(false);

    int value = token.toInt(); // always succeeds
    token = nextToken(str);
    if (token != QLatin1String("M")) {
        dur->setValid(false);
        return;
    }
    dur->setMinutes(value);

    if (!str->isEmpty())
        parseDurationSeconds(str, dur);
}

/*! Parse a duration string starting from the part describing the number of seconds.  Removes parsed
 * part from \a str and updates \a dur with the findings.
 */
void Duration::parseDurationSeconds(QString* str, Duration* dur)
{
    QString token = nextToken(str);
    if (token.isEmpty() || !token.at(0).isDigit())
        dur->setValid(false);

    int value = token.toInt(); // always succeeds
    token = nextToken(str);
    if (token != QLatin1String("S")) {
        dur->setValid(false);
        return;
    }
    dur->setSeconds(value);
}

/*! Removes and returns a "token" from the start of an iCalendar DURATION string, \a str.  A token
 * is either a single +, - or upper-case letter, or a string of digits.  If \a str is empty, an
 * empty string is returned.  If \a str is not empty but starts with an invalid character, a null
 * string is returned.
 */
QString Duration::nextToken(QString* str)
{
    int len = str->length();
    if (len == 0)
        return QString::fromAscii(""); // empty (not null) QString
    QChar first = str->at(0);
    if (first == QLatin1Char('+') || first == QLatin1Char('-') || first.isUpper()) {
        QString ret(str->left(1));
        *str = str->mid(1);
        return ret;
    } else if (first.isDigit()) {
        // find the largest n such that the leftmost n characters are digits
        int n = 1;
        for (n = 1; n < len && str->at(n).isDigit(); n++) {
        }
        QString ret = str->left(n);
        *str = str->mid(n);
        return ret;
    } else {
        return QString(); // null QString
    }
}

TimeZone QVersitOrganizerImporterPrivate::importTimeZone(const QVersitDocument& document) const
{
    TimeZone timeZone;
    foreach (const QVersitProperty& property, document.properties()) {
        if (property.name() == QLatin1String("TZID") && !property.value().isEmpty()) {
            timeZone.setTzid(property.value());
        }
    }
    foreach (const QVersitDocument& subDocument, document.subDocuments()) {
        timeZone.addPhase(importTimeZonePhase(subDocument));
    }
    return timeZone;
}

TimeZonePhase QVersitOrganizerImporterPrivate::importTimeZonePhase(const QVersitDocument& document) const
{
    TimeZonePhase phase;
    phase.setStandard(document.componentType() == QLatin1String("STANDARD"));

    foreach (const QVersitProperty& property, document.properties()) {
        if (property.name() == QLatin1String("TZOFFSETTO")) {
            QString value(property.value());
            if (value.size() == 5
                    && (value.at(0) == QLatin1Char('+') || value.at(0) == QLatin1Char('-'))
                    && value.at(1).isDigit()
                    && value.at(2).isDigit()
                    && value.at(3).isDigit()
                    && value.at(4).isDigit()) {
                phase.setUtcOffset((value.at(0) == QLatin1Char('+') ? 1 : -1) * ( // deal with sign
                        value.mid(1, 2).toInt() * 3600 // hours part
                        + value.mid(3, 2).toInt() * 60 // minutes part
                        ));
            }
        } else if (property.name() == QLatin1String("DTSTART")) {
            QDateTime dt(parseDateTime(property.value()));
            if (dt.isValid() && dt.timeSpec() == Qt::LocalTime) {
                phase.setStartDateTime(dt);
            }
        } else if (property.name() == QLatin1String("RRULE")) {
            QOrganizerRecurrenceRule rrule;
            if (parseRecurRule(property.value(), &rrule)) {
                phase.setRecurrenceRule(rrule);
            }
        } else if (property.name() == QLatin1String("RDATE")) {
            QSet<QDate> dates;
            if (parseDates(property.value(), &dates)) {
                phase.setRecurrenceDates(dates);
            }
        }
    }
    return phase;
}
