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


QTM_USE_NAMESPACE

QVersitOrganizerImporterPrivate::QVersitOrganizerImporterPrivate() :
    mPropertyHandler(NULL),
    mDefaultResourceHandler(new QVersitDefaultResourceHandler),
    mResourceHandler(mDefaultResourceHandler),
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
}

QVersitOrganizerImporterPrivate::~QVersitOrganizerImporterPrivate()
{
    delete mDefaultResourceHandler;
}

bool QVersitOrganizerImporterPrivate::importDocument(
        const QVersitDocument& document,
        QOrganizerItem* item,
        QVersitOrganizerImporter::Error* error)
{
    if (document.componentType() == QLatin1String("VEVENT")) {
        item->setType(QOrganizerItemType::TypeEvent);
    } else {
        *error = QVersitOrganizerImporter::InvalidDocumentError;
        return false;
    }
    const QList<QVersitProperty> properties = document.properties();
    if (properties.isEmpty()) {
        *error = QVersitOrganizerImporter::EmptyDocumentError;
        return false;
    }

    foreach (const QVersitProperty& property, properties) {
        importProperty(document, property, item);
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
        }
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
    QDateTime datetime = parseDateTime(property.value());
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
    QDateTime datetime = parseDateTime(property.value());
    if (!datetime.isValid())
        return false;
    QOrganizerItemTimestamp timestamp(item->detail<QOrganizerItemTimestamp>());
    timestamp.setLastModified(datetime);
    updatedDetails->append(timestamp);
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
    QDateTime newStart = parseDateTime(property.value());
    if (!newStart.isValid())
        return false;
    QOrganizerItemEventTimeRange etr(item->detail<QOrganizerItemEventTimeRange>());
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
    QDateTime newEnd = parseDateTime(property.value());
    if (!newEnd.isValid())
        return false;
    QOrganizerItemEventTimeRange etr(item->detail<QOrganizerItemEventTimeRange>());
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
    QOrganizerItemEventTimeRange etr(item->detail<QOrganizerItemEventTimeRange>());
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

/*! Parses \a str as an ISO 8601 datetime in basic format, either in UTC timezone or floating
 * timezone.  Returns an invalid QDateTime if the string cannot be parsed.
 */
QDateTime QVersitOrganizerImporterPrivate::parseDateTime(QString str)
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
    QOrganizerItemRecurrenceRule rule;
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
bool QVersitOrganizerImporterPrivate::parseRecurRule(const QString& str, QOrganizerItemRecurrenceRule* rule)
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
        rule->setFrequency(QOrganizerItemRecurrenceRule::Daily);
    } else if (freqValue == QLatin1String("WEEKLY")) {
        rule->setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    } else if (freqValue == QLatin1String("MONTHLY")) {
        rule->setFrequency(QOrganizerItemRecurrenceRule::Monthly);
    } else if (freqValue == QLatin1String("YEARLY")) {
        rule->setFrequency(QOrganizerItemRecurrenceRule::Yearly);
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
                                                         QOrganizerItemRecurrenceRule* rule)
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
            rule->setCount(count);
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
            rule->setEndDate(date);
    } else if (key == QLatin1String("BYDAY")) {
        QList<Qt::DayOfWeek> days;
        QStringList dayParts = value.split(QLatin1Char(','));
        foreach (const QString& dayStr, dayParts) {
            int day = parseDayOfWeek(dayStr);
            if (day != -1) {
                days << (Qt::DayOfWeek)day;
            }
        }
        if (!days.isEmpty()) {
            rule->setDaysOfWeek(days);
        }
    } else if (key == QLatin1String("BYMONTHDAY")) {
        QList<int> days = parseIntList(value, -31, 31);
        if (!days.isEmpty()) {
            rule->setDaysOfMonth(days);
        }
    } else if (key == QLatin1String("BYWEEKNO")) {
        QList<int> weeks = parseIntList(value, -53, 53);
        if (!weeks.isEmpty()) {
            rule->setWeeksOfYear(weeks);
        }
    } else if (key == QLatin1String("BYMONTH")) {
        QList<QOrganizerItemRecurrenceRule::Month> months;
        QStringList monthParts = value.split(QLatin1Char(','));
        foreach (const QString& monthParts, monthParts) {
            bool ok;
            int month = monthParts.toInt(&ok);
            if (ok && month >= 1 && month <= 12) {
                months << (QOrganizerItemRecurrenceRule::Month)month;
            }
        }
        if (!months.isEmpty()) {
            rule->setMonths(months);
        }
    } else if (key == QLatin1String("BYYEARDAY")) {
        QList<int> days = parseIntList(value, -366, 366);
        if (!days.isEmpty()) {
            rule->setDaysOfYear(days);
        }
    } else if (key == QLatin1String("BYSETPOS")) {
        QList<int> poss = parseIntList(value, -366, 366);
        if (!poss.isEmpty()) {
            rule->setPosition(poss);
        }
    } else if (key == QLatin1String("WKST")) {
        int day = parseDayOfWeek(value);
        if (day != -1) {
            rule->setWeekStart((Qt::DayOfWeek)day);
        }
    }
}

/*!
 * Parses and returns a comma-separated list of integers.  Only non-zero values between \a min and
 * \a max (inclusive) are added
 */
QList<int> QVersitOrganizerImporterPrivate::parseIntList(const QString& str, int min, int max)
{
    QList<int> values;
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
int QVersitOrganizerImporterPrivate::parseDayOfWeek(const QString& str)
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
    QList<QDate> dates;
    if (!parseDateList(property.value(), &dates))
        return false;
    QOrganizerItemRecurrence detail(item->detail<QOrganizerItemRecurrence>());
    if (property.name() == QLatin1String("RDATE")) {
        detail.setRecurrenceDates(detail.recurrenceDates() << dates);
    } else if (property.name() == QLatin1String("EXDATE")) {
        detail.setExceptionDates(detail.exceptionDates() << dates);
    } 
    updatedDetails->append(detail);
    return true;
}

/*!
 * Parses a string like "19970304,19970504,19970704" into a list of QDates
 */
bool QVersitOrganizerImporterPrivate::parseDateList(const QString& str, QList<QDate>* dates)
{
    QStringList parts = str.split(QLatin1Char(','));
    if (parts.size() == 0)
        return false;

    foreach (QString part, parts) {
        int tIndex = part.indexOf(QLatin1Char('T'));
        if (tIndex >= 0) {
            part = part.left(tIndex);
        }
        QDate date = QDate::fromString(part, QLatin1String("yyyyMMdd"));
        if (date.isValid())
            *dates << date;
        else
            return false;
    }
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
