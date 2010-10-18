/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/versit

#include "tst_qversitorganizerexporter.h"
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QOrganizerItem>);
Q_DECLARE_METATYPE(QList<QOrganizerItemDetail>);
Q_DECLARE_METATYPE(QList<QVersitProperty>);

void tst_QVersitOrganizerExporter::testExport()
{
    QFETCH(QList<QOrganizerItem>, items);
    QFETCH(QVersitDocument, expectedDocument);

    QVersitOrganizerExporter exporter;
    QVERIFY(exporter.exportItems(items));
    QVERIFY(exporter.errorMap().isEmpty());
    QVersitDocument document = exporter.document();
    if (document != expectedDocument) {
        qDebug() << "Actual:" << document;
        qDebug() << "Expected:" << expectedDocument;
        QCOMPARE(document, expectedDocument);
    }
}

void tst_QVersitOrganizerExporter::testExport_data()
{
    QTest::addColumn<QList<QOrganizerItem> >("items");
    QTest::addColumn<QVersitDocument>("expectedDocument");

    {
        QVersitDocument document(QVersitDocument::ICalendar20Type);
        document.setComponentType(QLatin1String("VCALENDAR"));
        QVersitDocument nested(QVersitDocument::ICalendar20Type);
        nested.setComponentType(QLatin1String("VEVENT"));
        QVersitProperty property;
        property.setName(QLatin1String("SUMMARY"));
        property.setValue(QLatin1String("Bastille Day Party"));
        nested.addProperty(property);
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("19970714T170000Z"));
        nested.addProperty(property);
        property.setName(QLatin1String("DTEND"));
        property.setValue(QLatin1String("19970715T035959Z"));
        nested.addProperty(property);
        document.addSubDocument(nested);

        nested.clear();
        nested.setType(QVersitDocument::ICalendar20Type);
        nested.setComponentType(QLatin1String("VTODO"));
        property.setName(QLatin1String("SUMMARY"));
        property.setValue(QLatin1String("Take out the garbage"));
        nested.addProperty(property);
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("20100609T080000"));
        nested.addProperty(property);
        property.setName(QLatin1String("DUE"));
        property.setValue(QLatin1String("20100610T080000"));
        nested.addProperty(property);
        document.addSubDocument(nested);

        nested.clear();
        nested.setType(QVersitDocument::ICalendar20Type);
        nested.setComponentType(QLatin1String("VJOURNAL"));
        property.setName(QLatin1String("SUMMARY"));
        property.setValue(QLatin1String("Trip to Thailand"));
        nested.addProperty(property);
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("20100615T112300"));
        nested.addProperty(property);
        document.addSubDocument(nested);

        QOrganizerEvent event;
        event.setDisplayLabel(QLatin1String("Bastille Day Party"));
        event.setStartDateTime(QDateTime(QDate(1997, 7, 14), QTime(17, 0, 0), Qt::UTC));
        event.setEndDateTime(QDateTime(QDate(1997, 7, 15), QTime(3, 59, 59), Qt::UTC));

        QOrganizerTodo todo;
        todo.setDisplayLabel(QLatin1String("Take out the garbage"));
        todo.setStartDateTime(QDateTime(QDate(2010, 6, 9), QTime(8, 0, 0)));
        todo.setDueDateTime(QDateTime(QDate(2010, 6, 10), QTime(8, 0, 0)));

        QOrganizerJournal journal;
        journal.setDisplayLabel(QLatin1String("Trip to Thailand"));
        journal.setDateTime(QDateTime(QDate(2010, 6, 15), QTime(11, 23, 0)));

        QList<QOrganizerItem> items;
        items << static_cast<QOrganizerItem>(event);
        items << static_cast<QOrganizerItem>(todo);
        items << static_cast<QOrganizerItem>(journal);

        QTest::newRow("sample event, todo and journal") << items << document;
    }

    {
        QVersitDocument document(QVersitDocument::ICalendar20Type);
        document.setComponentType(QLatin1String("VCALENDAR"));
        QVersitDocument nested(QVersitDocument::ICalendar20Type);
        nested.setComponentType(QLatin1String("VEVENT"));
        QVersitProperty property;
        property.setName(QLatin1String("UID"));
        property.setValue(QLatin1String("1234"));
        nested.addProperty(property);
        property.setName(QLatin1String("RECURRENCE-ID"));
        property.setValue(QLatin1String("20100608"));
        nested.addProperty(property);
        document.addSubDocument(nested);

        // An event occurrence with OriginalDate and Guid set
        QOrganizerEventOccurrence event;
        event.setGuid(QLatin1String("1234"));
        event.setOriginalDate(QDate(2010, 6, 8));

        QList<QOrganizerItem> items;
        items << static_cast<QOrganizerItem>(event);

        QTest::newRow("event occurrence exception") << items << document;
    }
}

/*! Test that bad input causes the exporter to fail with the correct error */
void tst_QVersitOrganizerExporter::testExportError()
{
    QFETCH(QList<QOrganizerItem>, items);
    QFETCH(QVersitDocument, expectedDocument);
    QFETCH(int, expectedError);

    QVersitOrganizerExporter exporter;
    QVERIFY(!exporter.exportItems(items));
    QVERIFY(!exporter.errorMap().isEmpty());
    QVERIFY(exporter.errorMap()[0] == expectedError);
    QVersitDocument document = exporter.document();
    if (document != expectedDocument) {
        qDebug() << "Actual:" << document;
        qDebug() << "Expected:" << expectedDocument;
        QCOMPARE(document, expectedDocument);
    }
}

void tst_QVersitOrganizerExporter::testExportError_data()
{
    QTest::addColumn<QList<QOrganizerItem> >("items");
    QTest::addColumn<QVersitDocument>("expectedDocument");
    QTest::addColumn<int>("expectedError");

    QVersitDocument document(QVersitDocument::ICalendar20Type);
    document.setComponentType(QLatin1String("VCALENDAR"));

    // An event occurrence with OriginalDate set but no Guid
    QOrganizerEventOccurrence event;
    event.setOriginalDate(QDate(2010, 6, 8));

    QList<QOrganizerItem> items;
    items << static_cast<QOrganizerItem>(event);

    QTest::newRow("event occurrence exception with no guid") << items << document
        << (int)QVersitOrganizerExporter::UnderspecifiedOccurrenceError;
}

void tst_QVersitOrganizerExporter::testExportEventDetails()
{
    QFETCH(QList<QOrganizerItemDetail>, details);
    QFETCH(QList<QVersitProperty>, expectedProperties);

    QVersitOrganizerExporter exporter;
    QOrganizerEvent item;
    foreach (QOrganizerItemDetail detail, details) {
        item.saveDetail(&detail);
    }
    QVERIFY(exporter.exportItems(QList<QOrganizerItem>() << item));
    QVERIFY(exporter.errorMap().isEmpty());
    QVersitDocument document = exporter.document();
    QList<QVersitDocument> subDocuments = document.subDocuments();
    QCOMPARE(subDocuments.size(), 1);

    foreach(const QVersitProperty& expectedProperty, expectedProperties) {
        QList<QVersitProperty> actualProperties =
            findPropertiesByName(subDocuments.first(), expectedProperty.name());
        if (!actualProperties.contains(expectedProperty)) {
            qDebug() << "Actual:" << actualProperties;
            qDebug() << "Expected to find:" << expectedProperty;
            QVERIFY(false);
        }
    }
}

void tst_QVersitOrganizerExporter::testExportEventDetails_data()
{
    QTest::addColumn<QList<QOrganizerItemDetail> >("details");
    QTest::addColumn<QList<QVersitProperty> >("expectedProperties");

    {
        QVersitProperty property;
        property.setName(QLatin1String("SUMMARY"));
        property.setValue(QLatin1String("jabberwocky"));
        QOrganizerItemDisplayLabel displayLabel;
        displayLabel.setLabel(QLatin1String("jabberwocky"));
        QTest::newRow("one summary") << (QList<QOrganizerItemDetail>() << displayLabel)
            << (QList<QVersitProperty>() << property);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("DESCRIPTION"));
        property.setValue(QLatin1String("Twinkle, twinkle, little bat! How I wonder what you're at."));
        QOrganizerItemDescription description;
        description.setDescription(QLatin1String("Twinkle, twinkle, little bat! How I wonder what you're at."));
        QTest::newRow("one description") << (QList<QOrganizerItemDetail>() << description)
            << (QList<QVersitProperty>() << property);
    }

    {
        QVersitProperty property1;
        property1.setName(QLatin1String("COMMENT"));
        property1.setValue(QLatin1String("Comment 1"));
        QVersitProperty property2;
        property2.setName(QLatin1String("COMMENT"));
        property2.setValue(QLatin1String("Comment 2"));
        QOrganizerItemComment comment1;
        comment1.setComment(QLatin1String("Comment 1"));
        QOrganizerItemComment comment2;
        comment2.setComment(QLatin1String("Comment 2"));
        QTest::newRow("two comments")
            << (QList<QOrganizerItemDetail>() << comment1 << comment2)
            << (QList<QVersitProperty>() << property1 << property2);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("UID"));
        property.setValue(QLatin1String("1234567"));
        QOrganizerItemGuid guid;
        guid.setGuid(QLatin1String("1234567"));
        QTest::newRow("guid") << (QList<QOrganizerItemDetail>() << guid)
            << (QList<QVersitProperty>() << property);
    }

    {
        QList<QVersitProperty> properties;
        QVersitProperty dtstart;
        dtstart.setName(QLatin1String("DTSTART"));
        dtstart.setValue(QLatin1String("20100102T030405"));
        properties << dtstart;
        QVersitProperty dtend;
        dtend.setName(QLatin1String("DTEND"));
        dtend.setValue(QLatin1String("20100102T030406"));
        properties << dtend;
        QOrganizerEventTime etr;
        etr.setStartDateTime(QDateTime(QDate(2010, 1, 2), QTime(3, 4, 5)));
        etr.setEndDateTime(QDateTime(QDate(2010, 1, 2), QTime(3, 4, 6)));
        QTest::newRow("dtstart and dtend") << (QList<QOrganizerItemDetail>() << etr)
            << properties;
    }

    {
        QList<QVersitProperty> properties;
        QVersitProperty created;
        created.setName(QLatin1String("CREATED"));
        created.setValue(QDateTime(QDate(2010, 1, 2), QTime(3, 4, 5)).toUTC().toString(
                    QLatin1String("yyyyMMddTHHmmssZ")));
        properties << created;
        QVersitProperty modified;
        modified.setName(QLatin1String("LAST-MODIFIED"));
        modified.setValue(QDateTime(QDate(2010, 1, 2), QTime(3, 4, 6)).toUTC().toString(
                    QLatin1String("yyyyMMddTHHmmssZ")));
        properties << modified;
        QOrganizerItemTimestamp timestamp;
        timestamp.setCreated(QDateTime(QDate(2010, 1, 2), QTime(3, 4, 5)));
        timestamp.setLastModified(QDateTime(QDate(2010, 1, 2), QTime(3, 4, 6)));
        QTest::newRow("created and last modified")
            << (QList<QOrganizerItemDetail>() << timestamp)
            << properties;
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=DAILY"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Daily);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule daily")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);

        rrule.setValue(QLatin1String("FREQ=WEEKLY"));
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Weekly);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule weekly")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);

        rrule.setValue(QLatin1String("FREQ=MONTHLY"));
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule monthly")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);

        rrule.setValue(QLatin1String("FREQ=YEARLY"));
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule yearly")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);

        rrule.setValue(QLatin1String("FREQ=MONTHLY;INTERVAL=2;BYDAY=TU"));
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        recurrenceRule.setInterval(2);
        recurrenceRule.setDaysOfWeek(QSet<Qt::DayOfWeek>() << Qt::Tuesday);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule monthly")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=MONTHLY;BYMONTHDAY=-3,1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        recurrenceRule.setDaysOfMonth(QSet<int>() << 1 << -3);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule bymonthday")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYWEEKNO=-3,1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        recurrenceRule.setWeeksOfYear(QSet<int>() << 1 << -3);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule byweekno")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYMONTH=1,10"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        recurrenceRule.setMonthsOfYear(QSet<QOrganizerRecurrenceRule::Month>()
                << QOrganizerRecurrenceRule::January
                << QOrganizerRecurrenceRule::October);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule bymonth")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYYEARDAY=-1,1,366"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        recurrenceRule.setDaysOfYear(QSet<int>() << 1 << 366 << -1);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule byyearday")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=WEEKLY;COUNT=4;INTERVAL=2;BYDAY=TU,SU;WKST=SU"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Weekly);
        recurrenceRule.setInterval(2);
        recurrenceRule.setLimit(4);
        recurrenceRule.setDaysOfWeek(QSet<Qt::DayOfWeek>() << Qt::Tuesday << Qt::Sunday);
        recurrenceRule.setFirstDayOfWeek(Qt::Sunday);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule wkst")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=MONTHLY;BYDAY=MO,TU,WE,TH,FR;BYSETPOS=-1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        recurrenceRule.setDaysOfWeek(QSet<Qt::DayOfWeek>()
                << Qt::Monday << Qt::Tuesday << Qt::Wednesday << Qt::Thursday << Qt::Friday);
        recurrenceRule.setPositions(QSet<int>() << -1);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule bysetpos")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=DAILY;UNTIL=20000131;BYMONTH=1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Daily);
        recurrenceRule.setLimit(QDate(2000, 1, 31));
        recurrenceRule.setMonthsOfYear(QSet<QOrganizerRecurrenceRule::Month>()
                << QOrganizerRecurrenceRule::January);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule until")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setValueType(QVersitProperty::PreformattedType);
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=DAILY;COUNT=5;BYMONTH=1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Daily);
        recurrenceRule.setLimit(5);
        recurrenceRule.setMonthsOfYear(QSet<QOrganizerRecurrenceRule::Month>()
                << QOrganizerRecurrenceRule::January);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule count")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rdate;
        rdate.insertParameter(QLatin1String("VALUE"), QLatin1String("DATE"));
        rdate.setName(QLatin1String("RDATE"));
        rdate.setValue(QLatin1String("19970304"));
        QOrganizerEventTime etr;
        etr.setStartDateTime(QDateTime(QDate(1997, 3, 4), QTime(11, 0, 0)));
        etr.setEndDateTime(QDateTime(QDate(1997, 3, 4), QTime(11, 0, 0)));
        QOrganizerItemRecurrence recurrence;
        QSet<QDate> recurrenceDates;
        recurrenceDates << QDate(1997, 3, 4);
        recurrence.setRecurrenceDates(recurrenceDates);
        QTest::newRow("rdate")
            << (QList<QOrganizerItemDetail>() << etr << recurrence)
            << (QList<QVersitProperty>() << rdate);

        rdate.setValue(QLatin1String("19970304,19970504,19970704"));
        recurrenceDates.clear();
        recurrenceDates << QDate(1997, 3, 4)
                        << QDate(1997, 5, 4)
                        << QDate(1997, 7, 4);
        recurrence.setRecurrenceDates(recurrenceDates);
        QTest::newRow("multiple rdate")
            << (QList<QOrganizerItemDetail>() << etr << recurrence)
            << (QList<QVersitProperty>() << rdate);
    }

    {
        QVersitProperty rdate;
        rdate.insertParameter(QLatin1String("VALUE"), QLatin1String("DATE"));
        rdate.setName(QLatin1String("EXDATE"));
        rdate.setValue(QLatin1String("19970304"));
        QOrganizerEventTime etr;
        etr.setStartDateTime(QDateTime(QDate(1997, 3, 4), QTime(11, 0, 0)));
        etr.setEndDateTime(QDateTime(QDate(1997, 3, 4), QTime(11, 0, 0)));
        QOrganizerItemRecurrence recurrence;
        QSet<QDate> exceptionDates;
        exceptionDates << QDate(1997, 3, 4);
        recurrence.setExceptionDates(exceptionDates);
        QTest::newRow("exdate")
            << (QList<QOrganizerItemDetail>() << etr << recurrence)
            << (QList<QVersitProperty>() << rdate);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("PRIORITY"));
        property.setValue(QLatin1String("0"));

        QOrganizerItemPriority detail;
        detail.setPriority(QOrganizerItemPriority::UnknownPriority);
        QTest::newRow("priority 0")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("1"));
        detail.setPriority(QOrganizerItemPriority::HighestPriority);
        QTest::newRow("priority 1")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("2"));
        detail.setPriority(QOrganizerItemPriority::ExtremelyHighPriority);
        QTest::newRow("priority 2")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("3"));
        detail.setPriority(QOrganizerItemPriority::VeryHighPriority);
        QTest::newRow("priority 3")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("4"));
        detail.setPriority(QOrganizerItemPriority::HighPriority);
        QTest::newRow("priority 4")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("5"));
        detail.setPriority(QOrganizerItemPriority::MediumPriority);
        QTest::newRow("priority 5")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("6"));
        detail.setPriority(QOrganizerItemPriority::LowPriority);
        QTest::newRow("priority 6")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("7"));
        detail.setPriority(QOrganizerItemPriority::VeryLowPriority);
        QTest::newRow("priority 7")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("8"));
        detail.setPriority(QOrganizerItemPriority::ExtremelyLowPriority);
        QTest::newRow("priority 8")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("9"));
        detail.setPriority(QOrganizerItemPriority::LowestPriority);
        QTest::newRow("priority 9")
            << (QList<QOrganizerItemDetail>() << detail)
            << (QList<QVersitProperty>() << property);
    }
}

void tst_QVersitOrganizerExporter::testExportTodoDetails()
{
    QFETCH(QList<QOrganizerItemDetail>, details);
    QFETCH(QList<QVersitProperty>, expectedProperties);

    QVersitOrganizerExporter exporter;
    QOrganizerTodo item;
    foreach (QOrganizerItemDetail detail, details) {
        item.saveDetail(&detail);
    }
    QVERIFY(exporter.exportItems(QList<QOrganizerItem>() << item));
    QVERIFY(exporter.errorMap().isEmpty());
    QVersitDocument document = exporter.document();
    QList<QVersitDocument> subDocuments = document.subDocuments();
    QCOMPARE(subDocuments.size(), 1);

    foreach(const QVersitProperty& expectedProperty, expectedProperties) {
        QList<QVersitProperty> actualProperties =
            findPropertiesByName(subDocuments.first(), expectedProperty.name());
        if (!actualProperties.contains(expectedProperty)) {
            qDebug() << "Actual:" << actualProperties;
            qDebug() << "Expected to find:" << expectedProperty;
            QVERIFY(false);
        }
    }
}

void tst_QVersitOrganizerExporter::testExportTodoDetails_data()
{
    QTest::addColumn<QList<QOrganizerItemDetail> >("details");
    QTest::addColumn<QList<QVersitProperty> >("expectedProperties");

    {
        QVersitProperty property;
        property.setName(QLatin1String("STATUS"));
        property.setValue(QLatin1String("COMPLETED"));
        QOrganizerTodoProgress progress;
        progress.setStatus(QOrganizerTodoProgress::StatusComplete);
        QTest::newRow("status completed")
            << (QList<QOrganizerItemDetail>() << progress)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("NEEDS-ACTION"));
        progress.setStatus(QOrganizerTodoProgress::StatusNotStarted);
        QTest::newRow("status needs-action")
            << (QList<QOrganizerItemDetail>() << progress)
            << (QList<QVersitProperty>() << property);

        property.setValue(QLatin1String("IN-PROCESS"));
        progress.setStatus(QOrganizerTodoProgress::StatusInProgress);
        QTest::newRow("status in-process")
            << (QList<QOrganizerItemDetail>() << progress)
            << (QList<QVersitProperty>() << property);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("PERCENT-COMPLETE"));
        property.setValue(QLatin1String("42"));
        QOrganizerTodoProgress progress;
        progress.setPercentageComplete(42);
        QTest::newRow("percent-complete")
            << (QList<QOrganizerItemDetail>() << progress)
            << (QList<QVersitProperty>() << property);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("COMPLETED"));
        property.setValue(QLatin1String("20100609T161500"));
        QOrganizerTodoProgress progress;
        progress.setFinishedDateTime(QDateTime(QDate(2010, 6, 9), QTime(16, 15, 0)));
        QTest::newRow("completed")
            << (QList<QOrganizerItemDetail>() << progress)
            << (QList<QVersitProperty>() << property);
    }
}


QList<QVersitProperty> tst_QVersitOrganizerExporter::findPropertiesByName(
        const QVersitDocument &document, const QString &propertyName)
{
    QList<QVersitProperty> retval;
    foreach (const QVersitProperty& property, document.properties()) {
        if (property.name() == propertyName)
            retval << property;
    }
    return retval;
}

QTEST_MAIN(tst_QVersitOrganizerExporter)
