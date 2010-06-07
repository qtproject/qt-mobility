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
    QVERIFY(exporter.exportItems(items, QVersitDocument::ICalendar20Type));
    QVERIFY(exporter.errors().isEmpty());
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

    QOrganizerEvent event;
    event.setDisplayLabel(QLatin1String("Bastille Day Party"));
    event.setStartDateTime(QDateTime(QDate(1997, 7, 14), QTime(17, 0, 0), Qt::UTC));
    event.setEndDateTime(QDateTime(QDate(1997, 7, 15), QTime(3, 59, 59), Qt::UTC));

    QList<QOrganizerItem> items;
    items << static_cast<QOrganizerItem>(event);

    QTest::newRow("sample event") << items << document;
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
    QVERIFY(exporter.exportItems(QList<QOrganizerItem>() << item, QVersitDocument::ICalendar20Type));
    QVERIFY(exporter.errors().isEmpty());
    QVersitDocument document = exporter.document();
    QList<QVersitDocument> subDocuments = document.subDocuments();
    QCOMPARE(subDocuments.size(), 1);

    foreach(const QVersitProperty& expectedProperty, expectedProperties) {
        QVersitProperty actualProperty = findPropertyByName(subDocuments.first(), expectedProperty.name());
        if (actualProperty != expectedProperty) {
            qDebug() << "Actual:" << actualProperty;
            qDebug() << "Expected:" << expectedProperty;
            QCOMPARE(actualProperty, expectedProperty);
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
        QList<QVersitProperty> properties;
        QVersitProperty dtstart;
        dtstart.setName(QLatin1String("DTSTART"));
        dtstart.setValue(QLatin1String("20100102T030405"));
        properties << dtstart;
        QVersitProperty dtend;
        dtend.setName(QLatin1String("DTEND"));
        dtend.setValue(QLatin1String("20100102T030406"));
        properties << dtend;
        QOrganizerItemEventTimeRange etr;
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
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=DAILY"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Daily);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule daily")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);

        rrule.setValue(QLatin1String("FREQ=WEEKLY"));
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule weekly")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);

        rrule.setValue(QLatin1String("FREQ=MONTHLY"));
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule monthly")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);

        rrule.setValue(QLatin1String("FREQ=YEARLY"));
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule yearly")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);

        rrule.setValue(QLatin1String("FREQ=MONTHLY;INTERVAL=2;BYDAY=TU"));
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
        recurrenceRule.setInterval(2);
        recurrenceRule.setDaysOfWeek(QList<Qt::DayOfWeek>() << Qt::Tuesday);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule monthly")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=MONTHLY;BYMONTHDAY=1,-3"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
        recurrenceRule.setDaysOfMonth(QList<int>() << 1 << -3);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule bymonthday")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYWEEKNO=1,-3"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
        recurrenceRule.setWeeksOfYear(QList<int>() << 1 << -3);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule byweekno")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYMONTH=1,10"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
        recurrenceRule.setMonths(QList<QOrganizerItemRecurrenceRule::Month>()
                << QOrganizerItemRecurrenceRule::January
                << QOrganizerItemRecurrenceRule::October);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule bymonth")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYYEARDAY=1,366,-1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
        recurrenceRule.setDaysOfYear(QList<int>() << 1 << 366 << -1);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule byyearday")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=WEEKLY;COUNT=4;INTERVAL=2;BYDAY=TU,SU;WKST=SU"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
        recurrenceRule.setInterval(2);
        recurrenceRule.setCount(4);
        recurrenceRule.setDaysOfWeek(QList<Qt::DayOfWeek>() << Qt::Tuesday << Qt::Sunday);
        recurrenceRule.setWeekStart(Qt::Sunday);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule wkst")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=MONTHLY;BYDAY=MO,TU,WE,TH,FR;BYSETPOS=-1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
        recurrenceRule.setDaysOfWeek(QList<Qt::DayOfWeek>()
                << Qt::Monday << Qt::Tuesday << Qt::Wednesday << Qt::Thursday << Qt::Friday);
        recurrenceRule.setPosition(QList<int>() << -1);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule bysetpos")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=DAILY;UNTIL=20000131;BYMONTH=1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Daily);
        recurrenceRule.setEndDate(QDate(2000, 1, 31));
        recurrenceRule.setMonths(QList<QOrganizerItemRecurrenceRule::Month>()
                << QOrganizerItemRecurrenceRule::January);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule until")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=DAILY;COUNT=5;BYMONTH=1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerItemRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerItemRecurrenceRule::Daily);
        recurrenceRule.setCount(5);
        recurrenceRule.setMonths(QList<QOrganizerItemRecurrenceRule::Month>()
                << QOrganizerItemRecurrenceRule::January);
        recurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule count")
            << (QList<QOrganizerItemDetail>() << recurrence)
            << (QList<QVersitProperty>() << rrule);
    }

    {
        QVersitProperty rdate;
        rdate.insertParameter(QLatin1String("VALUE"), QLatin1String("DATE"));
        rdate.setName(QLatin1String("RDATE"));
        rdate.setValue(QLatin1String("19970304"));
        QOrganizerItemEventTimeRange etr;
        etr.setStartDateTime(QDateTime(QDate(1997, 3, 4), QTime(11, 0, 0)));
        etr.setEndDateTime(QDateTime(QDate(1997, 3, 4), QTime(11, 0, 0)));
        QOrganizerItemRecurrence recurrence;
        QList<QDate> recurrenceDates;
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
        QOrganizerItemEventTimeRange etr;
        etr.setStartDateTime(QDateTime(QDate(1997, 3, 4), QTime(11, 0, 0)));
        etr.setEndDateTime(QDateTime(QDate(1997, 3, 4), QTime(11, 0, 0)));
        QOrganizerItemRecurrence recurrence;
        QList<QDate> exceptionDates;
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

QVersitProperty tst_QVersitOrganizerExporter::findPropertyByName(
        const QVersitDocument &document, const QString &propertyName)
{
    foreach (const QVersitProperty& property, document.properties()) {
        if (property.name() == propertyName)
            return property;
    }
    return QVersitProperty();
}

QTEST_MAIN(tst_QVersitOrganizerExporter)
