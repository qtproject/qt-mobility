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


QTM_USE_NAMESPACE

QVersitOrganizerImporterPrivate::QVersitOrganizerImporterPrivate() :
    mPropertyHandler(NULL),
    mDefaultResourceHandler(new QVersitDefaultResourceHandler),
    mResourceHandler(mDefaultResourceHandler)
{
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
    if (property.name() == QLatin1String("SUMMARY")) {
        success = createDisplayLabel(property, item, &updatedDetails);
    } else if (property.name() == QLatin1String("CREATED")) {
        success = createTimestampCreated(property, item, &updatedDetails);
    } else if (property.name() == QLatin1String("LAST-MODIFIED")) {
        success = createTimestampModified(property, item, &updatedDetails);
    }

    if (document.componentType() == QLatin1String("VEVENT")) {
        if (property.name() == QLatin1String("DTSTART")) {
            success = createStartDateTime(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("DTEND")) {
            success = createEndDateTime(property, item, &updatedDetails);
        } else if (property.name() == QLatin1String("DURATION")) {
            success = createDuration(property, item, &updatedDetails);
        }
    }

    foreach (QOrganizerItemDetail detail, updatedDetails) {
        item->saveDetail(&detail);
    }
}

bool QVersitOrganizerImporterPrivate::createDisplayLabel(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails)
{
    if (property.value().isEmpty())
        return false;
    QOrganizerItemDisplayLabel displayLabel(item->detail<QOrganizerItemDisplayLabel>());
    displayLabel.setLabel(property.value());
    updatedDetails->append(displayLabel);
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

bool QVersitOrganizerImporterPrivate::createStartDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property.value());
    if (!datetime.isValid())
        return false;
    QOrganizerItemEventTimeRange etr(item->detail<QOrganizerItemEventTimeRange>());
    etr.setStartDateTime(datetime);
    updatedDetails->append(etr);
    return true;
}

bool QVersitOrganizerImporterPrivate::createEndDateTime(
        const QVersitProperty& property,
        QOrganizerItem* item,
        QList<QOrganizerItemDetail>* updatedDetails) {
    if (property.value().isEmpty())
        return false;
    QDateTime datetime = parseDateTime(property.value());
    if (!datetime.isValid())
        return false;
    QOrganizerItemEventTimeRange etr(item->detail<QOrganizerItemEventTimeRange>());
    etr.setEndDateTime(datetime);
    updatedDetails->append(etr);
    return true;
}

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
    if (!startTime.isValid()) // TODO allow setting duration before start time
        return false;
    etr.setEndDateTime(
            startTime.addDays(7*duration.weeks() + duration.days())
                     .addSecs(3600*duration.hours() + 60*duration.minutes() + duration.seconds()));
    updatedDetails->append(etr);
    return true;
}

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

/*! Parse the iCalendar duration string in an RDP fashion with a two symbol lookahead. */
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
