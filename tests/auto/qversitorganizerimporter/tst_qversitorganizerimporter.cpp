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
    QVERIFY(importer.errors().isEmpty());
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

    QOrganizerEvent event;
    event.setDisplayLabel(QLatin1String("Bastille Day Party"));
    event.setStartDateTime(QDateTime(QDate(1997, 7, 14), QTime(17, 0, 0), Qt::UTC));
    event.setEndDateTime(QDateTime(QDate(1997, 7, 15), QTime(3, 59, 59), Qt::UTC));

    QList<QOrganizerItem> items;
    items << static_cast<QOrganizerItem>(event);

    QTest::newRow("sample event") << document << items;
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
    QVERIFY(importer.errors().isEmpty());
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
        QOrganizerItemEventTimeRange etr;
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
}

QTEST_MAIN(tst_QVersitOrganizerImporter)
