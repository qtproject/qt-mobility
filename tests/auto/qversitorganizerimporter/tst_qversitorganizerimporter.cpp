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

//TESTED_COMPONENT=src/versit

#include "tst_qversitorganizerimporter.h"
#include <QtTest/QtTest>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QList<QOrganizerItem>);
Q_DECLARE_METATYPE(QList<QOrganizerItemDetail>);
Q_DECLARE_METATYPE(QList<QVersitProperty>);

void tst_QVersitOrganizerImporter::testImport()
{
    QFETCH(QVersitDocument, document);
    QFETCH(QList<QOrganizerItem>, expectedItems);

    QVersitOrganizerImporter importer;
    QVERIFY(importer.importDocument(document));
    QVERIFY(importer.errorMap().isEmpty());
    QList<QOrganizerItem> items = importer.items();
    if (items != expectedItems) {
        qDebug() << "Actual:" << items;
        qDebug() << "Expected:" << expectedItems;
        QCOMPARE(items, expectedItems);
    }
}

void tst_QVersitOrganizerImporter::testImport_data()
{
    QTest::addColumn<QVersitDocument>("document");
    QTest::addColumn<QList<QOrganizerItem> >("expectedItems");

    {
        QVersitDocument document(QVersitDocument::ICalendar20Type);
        document.setComponentType(QLatin1String("VCALENDAR"));
        QVersitProperty property;
        property.setName(QLatin1String("PRODID"));
        property.setValue(QLatin1String("-//hacksw/handcal//NONSGML v1.0//EN"));
        document.addProperty(property);
        QVersitDocument nested(QVersitDocument::ICalendar20Type);
        nested.setComponentType(QLatin1String("VEVENT"));
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

        QTest::newRow("sample event and todo") << document << items;
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

        QTest::newRow("event occurrence exception") << document << items;
    }

    {
        QVersitDocument document(QVersitDocument::ICalendar20Type);
        document.setComponentType(QLatin1String("VCALENDAR"));
        QVersitDocument nested(QVersitDocument::ICalendar20Type);
        nested.setComponentType(QLatin1String("VTODO"));
        QVersitProperty property;
        property.setName(QLatin1String("RRULE"));
        property.setValue(QLatin1String("FREQ=WEEKLY"));
        nested.addProperty(property);
        document.addSubDocument(nested);

        QOrganizerTodo todo;
        QOrganizerRecurrenceRule rrule;
        rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
        todo.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rrule);

        QList<QOrganizerItem> items;
        items << static_cast<QOrganizerItem>(todo);

        QTest::newRow("todo recurrence rule") << document << items;
    }
}

void tst_QVersitOrganizerImporter::testImportEventProperties()
{
    QFETCH(QList<QVersitProperty>, properties);
    QFETCH(QList<QOrganizerItemDetail>, expectedDetails);

    QVersitDocument document(QVersitDocument::ICalendar20Type);
    document.setComponentType(QLatin1String("VCALENDAR"));
    QVersitDocument nested(QVersitDocument::ICalendar20Type);
    nested.setComponentType(QLatin1String("VEVENT"));
    foreach (const QVersitProperty& property, properties) {
        nested.addProperty(property);
    }
    document.addSubDocument(nested);

    QVersitOrganizerImporter importer;
    QVERIFY(importer.importDocument(document));
    QVERIFY(importer.errorMap().isEmpty());
    QList<QOrganizerItem> items = importer.items();
    QCOMPARE(items.size(), 1);

    foreach (const QOrganizerItemDetail& expectedDetail, expectedDetails) {
        QList<QOrganizerItemDetail> actualDetails = items.first().details(expectedDetail.definitionName());
        if (!actualDetails.contains(expectedDetail)) {
            qDebug() << "Actual:" << actualDetails;
            qDebug() << "Expected to find:" << expectedDetail;
            QVERIFY(false);
        }
    } 
}

void tst_QVersitOrganizerImporter::testImportEventProperties_data()
{
    QTest::addColumn<QList<QVersitProperty> >("properties");
    QTest::addColumn<QList<QOrganizerItemDetail> >("expectedDetails");

    {
        QVersitProperty property;
        property.setName(QLatin1String("SUMMARY"));
        property.setValue(QLatin1String("jabberwocky"));
        QOrganizerItemDisplayLabel displayLabel;
        displayLabel.setLabel(QLatin1String("jabberwocky"));
        QTest::newRow("one summary") << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << displayLabel);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("DESCRIPTION"));
        property.setValue(QLatin1String("Twinkle, twinkle, little bat! How I wonder what you're at."));
        QOrganizerItemDescription description;
        description.setDescription(QLatin1String("Twinkle, twinkle, little bat! How I wonder what you're at."));
        QTest::newRow("one description") << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << description);
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
            << (QList<QVersitProperty>() << property1 << property2)
            << (QList<QOrganizerItemDetail>() << comment1 << comment2);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("UID"));
        property.setValue(QLatin1String("1234567"));
        QOrganizerItemGuid guid;
        guid.setGuid(QLatin1String("1234567"));
        QTest::newRow("guid")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << guid);
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
        QTest::newRow("dtstart and dtend") << properties
            << (QList<QOrganizerItemDetail>() << etr);

        dtend.setValue(QLatin1String("20100102T235959"));
        properties.prepend(dtend);
        QTest::newRow("multiple dtstart and dtend") << properties
            << (QList<QOrganizerItemDetail>() << etr); // last takes precedence
    }

    {
        QVersitProperty dtstart;
        dtstart.setName(QLatin1String("DTSTART"));
        dtstart.setValue(QLatin1String("20091231T000000")); // equivalent to 2010, day zero

        QVersitProperty duration;
        duration.setName(QLatin1String("DURATION"));
        duration.setValue(QLatin1String("P15DT5H7M20S"));
        QOrganizerEventTime etr;
        etr.setStartDateTime(QDateTime(QDate(2009, 12, 31), QTime(0, 0, 0)));
        etr.setEndDateTime(QDateTime(QDate(2010, 1, 15), QTime(5, 7, 20)));
        QTest::newRow("dtstart and duration: days hours minutes seconds")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);
        QTest::newRow("duration and dtstart")
            << (QList<QVersitProperty>() << duration << dtstart)
            << (QList<QOrganizerItemDetail>() << etr);

        duration.setValue(QLatin1String("PT5H7M20S"));
        etr.setEndDateTime(QDateTime(QDate(2009, 12, 31), QTime(5, 7, 20)));
        QTest::newRow("dtstart and duration: hours minutes seconds")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);

        duration.setValue(QLatin1String("PT7M20S"));
        etr.setEndDateTime(QDateTime(QDate(2009, 12, 31), QTime(0, 7, 20)));
        QTest::newRow("dtstart and duration: minutes seconds")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);

        duration.setValue(QLatin1String("PT20S"));
        etr.setEndDateTime(QDateTime(QDate(2009, 12, 31), QTime(0, 0, 20)));
        QTest::newRow("dtstart and duration: seconds")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);

        duration.setValue(QLatin1String("P15DT5H7M"));
        etr.setEndDateTime(QDateTime(QDate(2010, 1, 15), QTime(5, 7, 0)));
        QTest::newRow("dtstart and duration: days hours minutes")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);

        duration.setValue(QLatin1String("P15DT5H"));
        etr.setEndDateTime(QDateTime(QDate(2010, 1, 15), QTime(5, 0, 0)));
        QTest::newRow("dtstart and duration: days hours")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);

        duration.setValue(QLatin1String("P15D"));
        etr.setEndDateTime(QDateTime(QDate(2010, 1, 15), QTime(0, 0, 0)));
        QTest::newRow("dtstart and duration: days")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);

        duration.setValue(QLatin1String("P7W"));
        etr.setEndDateTime(QDateTime(QDate(2010, 2, 18), QTime(0, 0, 0)));
        QTest::newRow("dtstart and duration: weeks")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);

        dtstart.setValue(QLatin1String("20100115T050720"));
        duration.setValue(QLatin1String("-P15DT5H7M20S"));
        etr.setStartDateTime(QDateTime(QDate(2010, 1, 15), QTime(5, 7, 20)));
        etr.setEndDateTime(QDateTime(QDate(2009, 12, 31), QTime(0, 0, 0)));
        QTest::newRow("dtstart and duration: negative")
            << (QList<QVersitProperty>() << dtstart << duration)
            << (QList<QOrganizerItemDetail>() << etr);
    }

    {
        QList<QVersitProperty> properties;
        QVersitProperty created;
        created.setName(QLatin1String("CREATED"));
        created.setValue(QLatin1String("20100102T030405Z"));
        properties << created;
        QVersitProperty modified;
        modified.setName(QLatin1String("LAST-MODIFIED"));
        modified.setValue(QLatin1String("20100102T030406Z"));
        properties << modified;
        QOrganizerItemTimestamp timestamp;
        timestamp.setCreated(QDateTime(QDate(2010, 1, 2), QTime(3, 4, 5), Qt::UTC));
        timestamp.setLastModified(QDateTime(QDate(2010, 1, 2), QTime(3, 4, 6), Qt::UTC));
        QTest::newRow("created and last modified") << properties
            << (QList<QOrganizerItemDetail>() << timestamp);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=DAILY"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Daily);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule daily")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);

        rrule.setValue(QLatin1String("FREQ=WEEKLY"));
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Weekly);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule weekly")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);

        rrule.setValue(QLatin1String("FREQ=MONTHLY"));
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule monthly")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);

        rrule.setValue(QLatin1String("FREQ=YEARLY"));
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule yearly")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);

        rrule.setValue(QLatin1String("FREQ=MONTHLY;INTERVAL=2;BYDAY=TU"));
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        recurrenceRule.setInterval(2);
        recurrenceRule.setDaysOfWeek(QSet<Qt::DayOfWeek>() << Qt::Tuesday);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule monthly")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=MONTHLY;BYMONTHDAY=1,-3"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        recurrenceRule.setDaysOfMonth(QSet<int>() << 1 << -3);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule bymonthday")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYWEEKNO=1,-3"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        recurrenceRule.setWeeksOfYear(QSet<int>() << 1 << -3);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule byweekno")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
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
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYYEARDAY=1,366,-1"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        recurrenceRule.setDaysOfYear(QSet<int>() << 1 << 366 << -1);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule byyearday")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
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
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
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
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
        rrule.setName(QLatin1String("RRULE"));
        rrule.setValue(QLatin1String("FREQ=YEARLY;BYMONTH=4;BYDAY=1SU"));
        QOrganizerItemRecurrence recurrence;
        QOrganizerRecurrenceRule recurrenceRule;
        recurrenceRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        recurrenceRule.setMonthsOfYear(QSet<QOrganizerRecurrenceRule::Month>()
                << QOrganizerRecurrenceRule::April);
        recurrenceRule.setDaysOfWeek(QSet<Qt::DayOfWeek>() << Qt::Sunday);
        recurrenceRule.setPositions(QSet<int>() << 1);
        recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << recurrenceRule);
        QTest::newRow("rrule byday with position")
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
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
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty rrule;
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
            << (QList<QVersitProperty>() << rrule)
            << (QList<QOrganizerItemDetail>() << recurrence);
    }

    {
        QVersitProperty dtstart;
        dtstart.setName(QLatin1String("DTSTART"));
        dtstart.setValue(QLatin1String("19970304T110000"));
        QVersitProperty dtend;
        dtend.setName(QLatin1String("DTEND"));
        dtend.setValue(QLatin1String("19970304T110000"));
        QVersitProperty rdate;
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
            << (QList<QVersitProperty>() << dtstart << dtend << rdate)
            << (QList<QOrganizerItemDetail>() << etr << recurrence);

        // XXX times in RDATE are ignored
        rdate.setValue(QLatin1String("19970304T133700"));
        recurrenceDates.clear();
        recurrenceDates << QDate(1997, 3, 4);
        recurrence.setRecurrenceDates(recurrenceDates);
        QTest::newRow("rdate with time")
            << (QList<QVersitProperty>() << dtstart << dtend << rdate)
            << (QList<QOrganizerItemDetail>() << etr << recurrence);

        rdate.setValue(QLatin1String("19970304,19970504,19970704"));
        recurrenceDates.clear();
        recurrenceDates << QDate(1997, 3, 4)
                        << QDate(1997, 5, 4)
                        << QDate(1997, 7, 4);
        recurrence.setRecurrenceDates(recurrenceDates);
        QTest::newRow("multiple rdate")
            << (QList<QVersitProperty>() << dtstart << dtend << rdate)
            << (QList<QOrganizerItemDetail>() << etr << recurrence);
    }

    {
        QVersitProperty dtstart;
        dtstart.setName(QLatin1String("DTSTART"));
        dtstart.setValue(QLatin1String("19970304T110000"));
        QVersitProperty dtend;
        dtend.setName(QLatin1String("DTEND"));
        dtend.setValue(QLatin1String("19970304T110000"));
        QVersitProperty rdate;
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
            << (QList<QVersitProperty>() << dtstart << dtend << rdate)
            << (QList<QOrganizerItemDetail>() << etr << recurrence);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("PRIORITY"));
        property.setValue(QLatin1String("0"));

        QOrganizerItemPriority detail;
        detail.setPriority(QOrganizerItemPriority::UnknownPriority);
        QTest::newRow("priority 0")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("1"));
        detail.setPriority(QOrganizerItemPriority::HighestPriority);
        QTest::newRow("priority 1")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("2"));
        detail.setPriority(QOrganizerItemPriority::ExtremelyHighPriority);
        QTest::newRow("priority 2")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("3"));
        detail.setPriority(QOrganizerItemPriority::VeryHighPriority);
        QTest::newRow("priority 3")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("4"));
        detail.setPriority(QOrganizerItemPriority::HighPriority);
        QTest::newRow("priority 4")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("5"));
        detail.setPriority(QOrganizerItemPriority::MediumPriority);
        QTest::newRow("priority 5")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("6"));
        detail.setPriority(QOrganizerItemPriority::LowPriority);
        QTest::newRow("priority 6")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("7"));
        detail.setPriority(QOrganizerItemPriority::VeryLowPriority);
        QTest::newRow("priority 7")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("8"));
        detail.setPriority(QOrganizerItemPriority::ExtremelyLowPriority);
        QTest::newRow("priority 8")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);

        property.setValue(QLatin1String("9"));
        detail.setPriority(QOrganizerItemPriority::LowestPriority);
        QTest::newRow("priority 9")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << detail);
    }
}

void tst_QVersitOrganizerImporter::testImportTodoProperties()
{
    QFETCH(QList<QVersitProperty>, properties);
    QFETCH(QList<QOrganizerItemDetail>, expectedDetails);

    QVersitDocument document(QVersitDocument::ICalendar20Type);
    document.setComponentType(QLatin1String("VCALENDAR"));
    QVersitDocument nested(QVersitDocument::ICalendar20Type);
    nested.setComponentType(QLatin1String("VTODO"));
    foreach (const QVersitProperty& property, properties) {
        nested.addProperty(property);
    }
    document.addSubDocument(nested);

    QVersitOrganizerImporter importer;
    QVERIFY(importer.importDocument(document));
    QVERIFY(importer.errorMap().isEmpty());
    QList<QOrganizerItem> items = importer.items();
    QCOMPARE(items.size(), 1);

    foreach (const QOrganizerItemDetail& expectedDetail, expectedDetails) {
        QOrganizerItemDetail actualDetail = items.first().detail(expectedDetail.definitionName());
        if (actualDetail != expectedDetail) {
            qDebug() << "Actual:" << actualDetail;
            qDebug() << "Expected:" << expectedDetail;
            QCOMPARE(actualDetail, expectedDetail);
        }
    } 
}

void tst_QVersitOrganizerImporter::testImportTodoProperties_data()
{
    QTest::addColumn<QList<QVersitProperty> >("properties");
    QTest::addColumn<QList<QOrganizerItemDetail> >("expectedDetails");

    {
        QVersitProperty property;
        property.setName(QLatin1String("STATUS"));
        property.setValue(QLatin1String("COMPLETED"));
        QOrganizerTodoProgress progress;
        progress.setStatus(QOrganizerTodoProgress::StatusComplete);
        QTest::newRow("status completed")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << progress);

        property.setValue(QLatin1String("NEEDS-ACTION"));
        progress.setStatus(QOrganizerTodoProgress::StatusNotStarted);
        QTest::newRow("status needs-action")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << progress);

        property.setValue(QLatin1String("IN-PROCESS"));
        progress.setStatus(QOrganizerTodoProgress::StatusInProgress);
        QTest::newRow("status in-process")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << progress);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("PERCENT-COMPLETE"));
        property.setValue(QLatin1String("42"));
        QOrganizerTodoProgress progress;
        progress.setPercentageComplete(42);
        QTest::newRow("percent-complete")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << progress);
    }

    {
        QVersitProperty property;
        property.setName(QLatin1String("COMPLETED"));
        property.setValue(QLatin1String("20100609T161500"));
        QOrganizerTodoProgress progress;
        progress.setFinishedDateTime(QDateTime(QDate(2010, 6, 9), QTime(16, 15, 0)));
        QTest::newRow("completed")
            << (QList<QVersitProperty>() << property)
            << (QList<QOrganizerItemDetail>() << progress);
    }
}

void tst_QVersitOrganizerImporter::testTimeZones()
{
    QFETCH(QString, tzid);
    QFETCH(QVersitDocument, timezoneSpec);
    QFETCH(QString, datetimeString);
    QFETCH(QDateTime, expected);

    QVersitDocument document(QVersitDocument::ICalendar20Type);
    document.setComponentType(QLatin1String("VCALENDAR"));
    if (!tzid.isEmpty()) {
        document.addSubDocument(timezoneSpec);
    }
    QVersitDocument vevent(QVersitDocument::ICalendar20Type);
    vevent.setComponentType(QLatin1String("VEVENT"));
    QVersitProperty property;
    property.setName(QLatin1String("DTSTART"));
    property.setValue(datetimeString);
    if (!tzid.isEmpty()) {
        property.insertParameter(QLatin1String("TZID"), tzid);
    }
    vevent.addProperty(property);
    document.addSubDocument(vevent);

    QVersitOrganizerImporter importer;
    QVERIFY(importer.importDocument(document));
    QVERIFY(importer.errorMap().isEmpty());
    QList<QOrganizerItem> items = importer.items();
    QCOMPARE(items.size(), 1);

    QOrganizerEvent event = static_cast<QOrganizerEvent>(items.first());
    QCOMPARE(event.type(), QString(QLatin1String(QOrganizerItemType::TypeEvent)));
    QDateTime actualDatetime = event.startDateTime();
    QCOMPARE(actualDatetime, expected);
    QCOMPARE(actualDatetime.timeSpec(), expected.timeSpec());
}

void tst_QVersitOrganizerImporter::testTimeZones_data()
{
    QTest::addColumn<QString>("tzid"); // set this to empty if you don't want to associate a tzid
    QTest::addColumn<QVersitDocument>("timezoneSpec");
    QTest::addColumn<QString>("datetimeString");
    QTest::addColumn<QDateTime>("expected");

    QVersitDocument vtimezone(QVersitDocument::ICalendar20Type);
    vtimezone.setComponentType(QLatin1String("VTIMEZONE"));
    QTest::newRow("utc") << QString() << QVersitDocument(QVersitDocument::ICalendar20Type)
        << QString::fromAscii("20100102T030405Z")
        << QDateTime(QDate(2010, 1, 2), QTime(3, 4, 5), Qt::UTC);

    QTest::newRow("floating") << QString() << QVersitDocument(QVersitDocument::ICalendar20Type)
        << QString::fromAscii("20100102T030405")
        << QDateTime(QDate(2010, 1, 2), QTime(3, 4, 5), Qt::LocalTime);

    {
        QVersitDocument vtimezone(QVersitDocument::ICalendar20Type);
        vtimezone.setComponentType(QLatin1String("VTIMEZONE"));

        QVersitProperty property;
        property.setName(QLatin1String("TZID"));
        property.setValue(QLatin1String("Asia/Singapore"));
        vtimezone.addProperty(property);
        property.setName(QLatin1String("X-LIC-LOCATION"));
        property.setValue(QLatin1String("Asia/Singapore"));
        vtimezone.addProperty(property);

        QVersitDocument standard(QVersitDocument::ICalendar20Type);
        standard.setComponentType(QLatin1String("STANDARD"));
        property.setName(QLatin1String("TZOFFSETFROM"));
        property.setValue(QLatin1String("+0800"));
        standard.addProperty(property);
        property.setName(QLatin1String("TZOFFSETTO"));
        property.setValue(QLatin1String("+0800"));
        standard.addProperty(property);
        property.setName(QLatin1String("TZNAME"));
        property.setValue(QLatin1String("EST"));
        standard.addProperty(property);
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("19700101T000000"));
        standard.addProperty(property);
        vtimezone.addSubDocument(standard);

        QTest::newRow("no dst") << QString::fromAscii("Asia/Singapore")
            << vtimezone << QString::fromAscii("20100102T100405")
            << QDateTime(QDate(2010, 1, 2), QTime(2, 4, 5), Qt::UTC);
    }

    {
        QVersitDocument vtimezone(QVersitDocument::ICalendar20Type);
        vtimezone.setComponentType(QLatin1String("VTIMEZONE"));

        QVersitProperty property;
        property.setName(QLatin1String("TZID"));
        property.setValue(QLatin1String("Australia/Sydney"));
        vtimezone.addProperty(property);
        property.setName(QLatin1String("X-LIC-LOCATION"));
        property.setValue(QLatin1String("Australia/Sydney"));
        vtimezone.addProperty(property);

        QVersitDocument standard(QVersitDocument::ICalendar20Type);
        standard.setComponentType(QLatin1String("STANDARD"));
        property.setName(QLatin1String("TZOFFSETFROM"));
        property.setValue(QLatin1String("+1100"));
        standard.addProperty(property);
        property.setName(QLatin1String("TZOFFSETTO"));
        property.setValue(QLatin1String("+1000"));
        standard.addProperty(property);
        property.setName(QLatin1String("TZNAME"));
        property.setValue(QLatin1String("EST"));
        standard.addProperty(property);
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("19700405T030000"));
        standard.addProperty(property);
        property.setName(QLatin1String("RRULE"));
        property.setValue(QLatin1String("FREQ=YEARLY;BYMONTH=4;BYDAY=1SU"));
        standard.addProperty(property);
        vtimezone.addSubDocument(standard);

        QVersitDocument daylight(QVersitDocument::ICalendar20Type);
        daylight.setComponentType(QLatin1String("DAYLIGHT"));
        property.setName(QLatin1String("TZOFFSETFROM"));
        property.setValue(QLatin1String("+1000"));
        daylight.addProperty(property);
        property.setName(QLatin1String("TZOFFSETTO"));
        property.setValue(QLatin1String("+1100"));
        daylight.addProperty(property);
        property.setName(QLatin1String("TZNAME"));
        property.setValue(QLatin1String("EST"));
        daylight.addProperty(property);
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("19701004T020000"));
        daylight.addProperty(property);
        property.setName(QLatin1String("RRULE"));
        property.setValue(QLatin1String("FREQ=YEARLY;BYMONTH=10;BYDAY=1SU"));
        daylight.addProperty(property);
        vtimezone.addSubDocument(daylight);

        QTest::newRow("dst area in standard time") << QString::fromAscii("Australia/Sydney")
            << vtimezone << QString::fromAscii("20100502T100405")
            << QDateTime(QDate(2010, 5, 2), QTime(0, 4, 5), Qt::UTC);

        QTest::newRow("dst") << QString::fromAscii("Australia/Sydney")
            << vtimezone << QString::fromAscii("20100102T100405")
            << QDateTime(QDate(2010, 1, 1), QTime(23, 4, 5), Qt::UTC);
    }
    
    {
        QVersitDocument vtimezone(QVersitDocument::ICalendar20Type);
        vtimezone.setComponentType(QLatin1String("VTIMEZONE"));

        QVersitProperty property;
        property.setName(QLatin1String("TZID"));
        property.setValue(QLatin1String("US-Eastern"));
        vtimezone.addProperty(property);

        QVersitDocument standard(QVersitDocument::ICalendar20Type);
        standard.setComponentType(QLatin1String("STANDARD"));
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("19961026T020000"));
        standard.addProperty(property);
        property.setName(QLatin1String("RDATE"));
        property.setValue(QLatin1String("19971026T020000"));
        standard.addProperty(property);
        property.setName(QLatin1String("TZOFFSETFROM"));
        property.setValue(QLatin1String("-0400"));
        standard.addProperty(property);
        property.setName(QLatin1String("TZOFFSETTO"));
        property.setValue(QLatin1String("-0500"));
        standard.addProperty(property);
        vtimezone.addSubDocument(standard);

        QVersitDocument daylight(QVersitDocument::ICalendar20Type);
        daylight.setComponentType(QLatin1String("DAYLIGHT"));
        property.setName(QLatin1String("DTSTART"));
        property.setValue(QLatin1String("19960406T020000"));
        daylight.addProperty(property);
        property.setName(QLatin1String("RDATE"));
        property.setValue(QLatin1String("19970406T020000"));
        daylight.addProperty(property);
        property.setName(QLatin1String("TZOFFSETFROM"));
        property.setValue(QLatin1String("-0500"));
        daylight.addProperty(property);
        property.setName(QLatin1String("TZOFFSETTO"));
        property.setValue(QLatin1String("-0400"));
        daylight.addProperty(property);
        vtimezone.addSubDocument(daylight);

        QTest::newRow("dst specified with rdate - daylight") << QString::fromAscii("US-Eastern")
            << vtimezone << QString::fromAscii("19970615T100000")
            << QDateTime(QDate(1997, 6, 15), QTime(14, 0, 0), Qt::UTC);

        QTest::newRow("dst specified with rdate - standard") << QString::fromAscii("US-Eastern")
            << vtimezone << QString::fromAscii("19971215T100000")
            << QDateTime(QDate(1997, 12, 15), QTime(15, 0, 0), Qt::UTC);
    }
}

QTEST_MAIN(tst_QVersitOrganizerImporter)
